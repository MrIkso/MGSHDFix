#include "version_checking.hpp"

#include <windows.h>
#include <winhttp.h>

#include <iostream>
#include <fstream>
#include <regex>
#include <iomanip>
#include <spdlog/spdlog.h>

#pragma comment(lib, "winhttp.lib")

LatestVersionChecker::LatestVersionChecker(const std::string& dllVersion,
    const std::string& repoOwner,
    const std::string& repoName,
    const std::filesystem::path& cacheFile,
    int cacheTTLHours)
    : m_dllVersion(dllVersion),
    m_repoOwner(repoOwner),
    m_repoName(repoName),
    m_cacheFile(cacheFile),
    m_cacheTTLHours(cacheTTLHours)
{
}

bool LatestVersionChecker::checkForUpdates()
{
    std::string cachedLatest, warnedVersion;

    if (loadCache(cachedLatest, warnedVersion))
    {
        int cmp = compareSemVer(m_dllVersion, cachedLatest);

        if (cmp < 0)
        {
            spdlog::warn("Version Check: A new version of MGSHDFix is available. Current Version: {}, Latest Version: {}", m_dllVersion, cachedLatest);
            if (warnedVersion != cachedLatest)
            {
                AllocConsole();
                FILE* dummy;
                freopen_s(&dummy, "CONOUT$", "w", stdout);
                std::cout << "MGSHDFix Update: A new version of MGSHDFix is available for download.\nCurrent Version: " << m_dllVersion << ", Latest Version: " << cachedLatest << std::endl;

                saveCache(cachedLatest, cachedLatest);
                return true;
            }
            return false;
        }
        else if (cmp > 0)
        {
            spdlog::info("Version Check: Welcome back, Commander! You're running a development build of MGSHDFix! Current Version: {}, Latest Release: {}", m_dllVersion, cachedLatest);
            return false;
        }

        spdlog::info("Version Check: MGSHDFix is up to date. Current Version: {}", m_dllVersion);
        return false;
    }

    // No fresh cache -> query GitHub
    std::string githubLatest;
    if (!queryGitHubLatestVersion(githubLatest))
    {
        // If GitHub fails, assume OK
        return false;
    }

    saveCache(githubLatest, "");

    int cmp = compareSemVer(m_dllVersion, githubLatest);

    if (cmp < 0)
    {
        saveCache(githubLatest, githubLatest);
        AllocConsole();
        FILE* dummy;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        std::cout << "MGSHDFix Update: A new version of MGSHDFix is available for download.\nCurrent Version: " << m_dllVersion << ", Latest Version: " << githubLatest << std::endl;
        return true;
    }
    else if (cmp > 0)
    {
        spdlog::info("Welcome back, Commander! You're running a development build of MGSHDFix! Current Version: {}, Latest Release: {}", m_dllVersion, githubLatest);
        return false;
    }

    spdlog::info("MGSHDFix is up to date. Current Version: {}", m_dllVersion);
    return false;
}


bool LatestVersionChecker::loadCache(std::string& cachedLatest, std::string& warnedVersion)
{
    std::ifstream file(m_cacheFile);
    if (!file) return false;

    std::string versionLine, timeLine;
    if (!std::getline(file, versionLine) || !std::getline(file, timeLine))
        return false;

    cachedLatest = versionLine;

    std::getline(file, warnedVersion);  // may or may not exist

    auto cachedTime = parseISO8601(timeLine);
    auto now = std::chrono::system_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::hours>(now - cachedTime);
    if (age.count() > m_cacheTTLHours)
        return false;  // Cache stale

    return true;
}

void LatestVersionChecker::saveCache(const std::string& latestVersion, const std::string& warnedVersion)
{
    std::ofstream file(m_cacheFile);
    if (!file) return;

    file << latestVersion << "\n";
    file << currentTimeISO8601() << "\n";
    file << warnedVersion << "\n";
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

int LatestVersionChecker::compareSemVer(const std::string& a, const std::string& b)
{
    std::istringstream sa(a), sb(b);
    int va[3] = { 0 }, vb[3] = { 0 };

    char dot;
    sa >> va[0] >> dot >> va[1] >> dot >> va[2];
    sb >> vb[0] >> dot >> vb[1] >> dot >> vb[2];

    for (int i = 0; i < 3; ++i)
    {
        if (va[i] < vb[i]) return -1;
        if (va[i] > vb[i]) return 1;
    }
    return 0;
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
