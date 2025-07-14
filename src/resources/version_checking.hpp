#pragma once

#include <chrono>
#include <string>

class LatestVersionChecker
{
public:
    explicit LatestVersionChecker(const std::string& dllVersion,
        const std::string& repoOwner,
        const std::string& repoName,
        const std::string& cacheFile = "version_cache.txt",
        int cacheTTLHours = 24);

    /// Checks if the current DLL version matches the latest GitHub release.
    /// Uses a cached version if available and fresh.
    bool isLatestVersion();

private:
    std::string m_dllVersion;
    std::string m_repoOwner;
    std::string m_repoName;
    std::string m_cacheFile;
    int m_cacheTTLHours;

    bool loadCache(std::string& cachedVersion);
    void saveCache(const std::string& latestVersion);
    bool queryGitHubLatestVersion(std::string& latestVersion);

    // Helper time functions
    static std::string currentTimeISO8601();
    static std::chrono::system_clock::time_point parseISO8601(const std::string& timeStr);
};
