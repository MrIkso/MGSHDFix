#pragma once

#include <string>
#include <chrono>
#include <filesystem>

class LatestVersionChecker
{
public:
    explicit LatestVersionChecker(
        const std::string& dllVersion,
        const std::string& repoOwner,
        const std::string& repoName,
        const std::filesystem::path& cacheFile = "version_cache.txt",
        int cacheTTLHours = 24);

    /// Runs the version check. Returns true if the user should see the outdated warning popup.
    bool checkForUpdates();

private:
    std::string m_dllVersion;
    std::string m_repoOwner;
    std::string m_repoName;
    std::filesystem::path m_cacheFile;
    int m_cacheTTLHours;

    bool loadCache(std::string& cachedLatest, std::string& warnedVersion, bool& cacheIsFresh);
    void saveCache(const std::string& latestVersion, const std::string& warnedVersion);
    bool queryGitHubLatestVersion(std::string& latestVersion);

    std::wstring buildUserAgent() const;

    static int compareSemVer(const std::string& a, const std::string& b);
    static std::string currentTimeISO8601();
    static std::chrono::system_clock::time_point parseISO8601(const std::string& timeStr);
};