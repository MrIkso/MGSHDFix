#include "version_checking.hpp"
#include <windows.h>
#include <winhttp.h>

#include <fstream>
#include <regex>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>

#pragma comment(lib, "winhttp.lib")

LatestVersionChecker::LatestVersionChecker(const std::string& dllVersion,
    const std::string& repoOwner,
    const std::string& repoName,
    const std::string& cacheFile,
    int cacheTTLHours)
    : m_dllVersion(dllVersion),
    m_repoOwner(repoOwner),
    m_repoName(repoName),
    m_cacheFile(cacheFile),
    m_cacheTTLHours(cacheTTLHours)
{
}

bool LatestVersionChecker::isLatestVersion()
{
    std::string cachedVersion;
    if (loadCache(cachedVersion))
    {
        return cachedVersion == m_dllVersion;
    }

    std::string latestVersion;
    if (!queryGitHubLatestVersion(latestVersion))
    {
        // On failure, assume latest
        return true;
    }

    saveCache(latestVersion);
    return latestVersion == m_dllVersion;
}

bool LatestVersionChecker::loadCache(std::string& cachedVersion)
{
    std::ifstream file(m_cacheFile);
    if (!file) return false;

    std::string versionLine, timeLine;
    if (!std::getline(file, versionLine) || !std::getline(file, timeLine))
        return false;

    auto cachedTime = parseISO8601(timeLine);
    auto now = std::chrono::system_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::hours>(now - cachedTime);
    if (age.count() > m_cacheTTLHours)
        return false;  // Cache stale

    cachedVersion = versionLine;
    return true;
}

void LatestVersionChecker::saveCache(const std::string& latestVersion)
{
    std::ofstream file(m_cacheFile);
    if (!file) return;

    file << latestVersion << "\n";
    file << currentTimeISO8601() << "\n";
}

bool LatestVersionChecker::queryGitHubLatestVersion(std::string& latestVersion)
{
    HINTERNET hSession = WinHttpOpen(
        L"MGSHDFix/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        nullptr,
        nullptr,
        0);
    if (!hSession) return false;

    HINTERNET hConnect = WinHttpConnect(
        hSession,
        L"api.github.com",
        INTERNET_DEFAULT_HTTPS_PORT,
        0);
    if (!hConnect)
    {
        WinHttpCloseHandle(hSession);
        return false;
    }

    // Build path "/repos/{owner}/{repo}/releases/latest"
    std::wstring path = L"/repos/" + std::wstring(m_repoOwner.begin(), m_repoOwner.end()) + L"/" +
        std::wstring(m_repoName.begin(), m_repoName.end()) + L"/releases/latest";

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect,
        L"GET",
        path.c_str(),
        nullptr,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (!hRequest)
    {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    WinHttpAddRequestHeaders(
        hRequest,
        L"User-Agent: MGSHDFix",
        -1,
        WINHTTP_ADDREQ_FLAG_ADD);

    std::string response;

    if (WinHttpSendRequest(hRequest, nullptr, 0, nullptr, 0, 0, 0) &&
        WinHttpReceiveResponse(hRequest, nullptr))
    {
        DWORD size = 0;
        do
        {
            DWORD downloaded = 0;
            WinHttpQueryDataAvailable(hRequest, &size);
            if (!size) break;

            std::string buffer(size, 0);
            WinHttpReadData(hRequest, &buffer[0], size, &downloaded);
            response.append(buffer, 0, downloaded);
        } while (size > 0);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    std::smatch m;
    std::regex re(R"delim("tag_name"\s*:\s*"\s*v?([^"]+)")delim");
    if (std::regex_search(response, m, re) && m.size() > 1)
    {
        latestVersion = m[1];
        return true;
    }

    return false;
}

std::string LatestVersionChecker::currentTimeISO8601()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::ostringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%Y-%m-%dT%H:%M:%S");
    return ss.str();
}

std::chrono::system_clock::time_point LatestVersionChecker::parseISO8601(const std::string& timeStr)
{
    std::tm tm = {};
    std::istringstream ss(timeStr);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}
