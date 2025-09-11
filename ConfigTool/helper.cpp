#include "pch.h"
#include "helper.hpp"
#include <wx/filefn.h>
#include <wx/log.h>


namespace Helper
{
    std::filesystem::path FindASILocation(std::string fileName)
    {
        static const std::array<std::string, 4> paths = { "", "plugins", "scripts", "update" };

        std::filesystem::path base = wxGetCwd().ToStdString();

        for (const auto& p : paths)
        {
            std::filesystem::path filePath = base / p / (fileName + ".asi");
            if (std::filesystem::exists(filePath))
            {
                return filePath.parent_path();
            }
        }
        wxLogError("Unable to locate %s.asi in %s and the other supported ASI directories.", fileName, wxGetCwd());
        return {};
    }

    static std::vector<int> parseVersionString(const std::string& versionStr)
    {
        std::vector<int> parts;
        std::istringstream ss(versionStr);
        std::string token;

        while (std::getline(ss, token, '.'))
        {
            if (token.empty())
            {
                parts.push_back(0);
                {
                    continue;
                }
            }

            size_t i = 0;
            while (i < token.size() && std::isdigit(static_cast<unsigned char>(token[i])))
                ++i;

            int value = (i > 0) ? std::stoi(token.substr(0, i)) : 0;
            parts.push_back(value);

            if (i < token.size())
            {
                // take first suffix letter -> 'a' = 1, 'b' = 2, etc.
                char c = static_cast<char>(std::tolower(token[i]));
                if (c >= 'a' && c <= 'z')
                {
                    parts.push_back((c - 'a') + 1);
                }
                else
                {
                    parts.push_back(1); // fallback for weird suffix
                }
            }
        }

        return parts;
    }

    VersionCompareResult compareSemVer(const std::string& currentVersion,
        const std::string& targetVersion)
    {
        std::vector<int> currentParts = parseVersionString(currentVersion);
        std::vector<int> targetParts = parseVersionString(targetVersion);

        size_t n = std::max(currentParts.size(), targetParts.size());
        currentParts.resize(n, 0);
        targetParts.resize(n, 0);

        for (size_t i = 0; i < n; ++i)
        {
            if (currentParts[i] < targetParts[i])
            {
                return VersionCompareResult::Older;
            }
            if (currentParts[i] > targetParts[i])
            {
                return VersionCompareResult::Newer;
            }
        }
        return VersionCompareResult::Equal;
    }

}
