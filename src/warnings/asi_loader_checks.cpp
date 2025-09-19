#include "stdafx.h"
#include "common.hpp"
#include "asi_loader_checks.hpp"


#include "logging.hpp"


namespace
{
    std::time_t ToTimeTWithOffset(std::filesystem::file_time_type ftime, int offsetHours)
    {
        using namespace std::chrono;
        auto sctp = time_point_cast<system_clock::duration>(
            ftime - decltype(ftime)::clock::now() + system_clock::now()
        );
        std::time_t t = system_clock::to_time_t(sctp);
        return t + (offsetHours * 60 * 60);
    }

    std::vector<std::filesystem::path> g_InstalledMods;

    const std::unordered_set<std::string> g_BaseGameFiles =
    {
        "steam_api64.dll",
        "sdkencryptedappticket64.dll",
        "unityplayer.dll",
        "engine.dll",
        "mg1.dll",
        "mg2.dll",
        "renderer.dll",
        "mgshdfix.asi",
    };

    bool IsBlacklisted(const std::filesystem::path& file)
    {
        const std::string filename = file.filename().string();
        return std::ranges::any_of(g_BaseGameFiles, [&](const std::string& banned) {
                               return _stricmp(filename.c_str(), banned.c_str()) == 0;
                           });
    }

    void GenerateInstalledModList(const std::filesystem::path& pathToCheck)
    {
        if (!std::filesystem::is_directory(pathToCheck))
        {
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(pathToCheck, std::filesystem::directory_options::skip_permission_denied))
        {
            const auto path = entry.path();
            const auto ext = path.extension().string();

            if (_stricmp(ext.c_str(), ".asi") != 0 && _stricmp(ext.c_str(), ".dll") != 0)
            {
                continue;
            }
            if (IsBlacklisted(path))
            {
                continue;
            }

            auto rel = path.lexically_relative(sExePath);
            g_InstalledMods.push_back(rel.empty() ? path.filename() : rel);
        }
    }

    void CheckInstalledMods()
    {
        g_InstalledMods.clear();

        GenerateInstalledModList(sExePath);
        GenerateInstalledModList(sExePath / "plugins");
        GenerateInstalledModList(sExePath / "scripts");
        GenerateInstalledModList(sExePath / "update");

        const bool texturePackDetected = std::filesystem::exists(sExePath / "textures/flatlist/ovr_stm/_win/w01a_stpt02.bmp.ctxr") ||
                                  std::filesystem::exists(sExePath / "textures/flatlist/ovr_stm/_win/00d81b4d.ctxr");
        if (texturePackDetected)
        {
            spdlog::info("---------- Installed Mods ----------");
            spdlog::info("    Upscaled texture pack installed.");
        }
        if (g_InstalledMods.empty())
        {
            return;
        }
        if (!texturePackDetected)
        {
            spdlog::info("---------- Installed Mods ----------");
        }

        std::map<std::string, std::vector<std::filesystem::path>> grouped;
        for (const auto& mod : g_InstalledMods)
        {
            std::string group = mod.has_parent_path() ? (*mod.begin()).string() : ".";
            grouped[group].push_back(mod);
        }

        for (auto& [group, files] : grouped)
        {
            std::sort(files.begin(), files.end());
        }

        auto logGroup = [](const std::string& group, const std::vector<std::filesystem::path>& files)
            {
                if (group != ".")
                {
                    spdlog::info("=== {} ===", group);
                }

                for (const auto& mod : files)
                {
                    auto absPath = sExePath / mod;
                    std::string productName = Util::GetFileProductName(absPath);

                    auto ftime = std::filesystem::last_write_time(absPath);
                    std::time_t mstTime = ToTimeTWithOffset(ftime, -7);

                    std::tm tm {};
                    localtime_s(&tm, &mstTime);

                    char timeBuf[64];
                    strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S (MST / GMT -7)", &tm);

                    if (!productName.empty())
                    {
                        spdlog::info("    {}   |   {}   |   modified {}", mod.string(), productName, timeBuf);
                    }
                    else
                    {
                        spdlog::info("    {}   |   modified {}", mod.string(), timeBuf);
                    }
                }
            };

        if (grouped.contains("."))
        {
            logGroup(".", grouped["."]);
        }
        for (auto& [group, files] : grouped)
        {
            if (group == ".")
            {
                continue;
            }
            logGroup(group, files);
        }
    }
}



void ASILoaderCompatibility::Check()
{
    spdlog::info("ASI Loader Compatibility Check: Checking for duplicate instances of ASI Loader (ie d3d11.dll, dxgi.dll).");
    //Don't simplify by removing filesystem::exists() from this check. While GetFileDescription does handle non-existent files own its own, checking filesystem::exists() first saves 400+ ms of initialization time
    if (std::filesystem::exists(sExePath / "d3d11.dll") && (Util::GetFileDescription((sExePath / "d3d11.dll").string()) == Util::GetFileDescription((sExePath / "winhttp.dll").string())))
    {
        spdlog::error("DUPLICATE MOD LOADER ERROR: Multiple ASI Loader .dll installations detected! This can cause inconsistent bugs and crashes.");
        spdlog::error("DUPLICATE MOD LOADER ERROR: Please delete d3d11.dll, it has been replaced by winhttp.dll & wininit.dll.");
        Logging::ShowConsole();
        std::cout << "DUPLICATE MOD LOADER ERROR: Multiple ASI Loader .dll's detected! This can cause inconsistent bugs and crashes." << std::endl;
        std::cout << "DUPLICATE MOD LOADER ERROR: Please delete d3d11.dll, it has been replaced by winhttp.dll & wininit.dll." << std::endl;
        if (Util::IsSteamOS())
        {
            std::cout << "DUPLICATE MOD LOADER ERROR: Steam Deck / Linux users must also replace their Steam game launch paramaters with the following command:" << std::endl;
            std::cout << "WINEDLLOVERRIDES=\"wininet,winhttp=n,b\" % command %" << std::endl;
            spdlog::error("DUPLICATE MOD LOADER ERROR: Steam Deck / Linux users must also replace their Steam game launch paramaters with the following command:");
            spdlog::error("WINEDLLOVERRIDES=\"wininet,winhttp=n,b\" % command %");
        }
    }

    if (std::filesystem::exists(sExePath / "dxgi.dll") && Util::GetFileDescription((sExePath / "dxgi.dll").string()) == "File description not found.")
    {
        spdlog::error("DUPLICATE MOD LOADER ERROR: Multiple ASI Loader .dll installations detected! This can cause inconsistent bugs and crashes.");
        spdlog::error("DUPLICATE MOD LOADER ERROR: Please delete dxgi.dll, it has been replaced by winhttp.dll & wininit.dll.");
        Logging::ShowConsole();
        std::cout << "DUPLICATE MOD LOADER ERROR: Multiple ASI Loader .dll's detected! This can cause inconsistent bugs and crashes." << std::endl;
        std::cout << "DUPLICATE MOD LOADER ERROR: Please delete dxgi.dll, it has been replaced by winhttp.dll & wininit.dll." << std::endl;
    }

    spdlog::info("ASI Mod Compatibility Check: Checking for common mod installation issues.");

    CheckInstalledMods();

    if (std::filesystem::exists(sExePath / "MGS2HFBladeMod.dll"))
    {
        spdlog::error("MOD COMPATIBILITY WARNING: MGS2HFBladeMod.dll has an incorrect extension!");
        Logging::ShowConsole();
        std::cout << "MOD COMPATIBILITY WARNING: MGS2HFBladeMod.dll has an incorrect extension!" << std::endl;
        std::error_code ec;
        std::filesystem::rename(sExePath / "MGS2HFBladeMod.dll", sExePath / "MGS2HFBladeMod.asi", ec);
        if (ec)
        {
            std::cout << "MOD COMPATIBILITY WARNING: Failed to rename MGS2HFBladeMod.dll: " << ec.message() << std::endl;
            std::cout << "MOD COMPATIBILITY WARNING: Please rename MGS2HFBladeMod.dll to MGS2HFBladeMod.asi manually to fix ASI Loader compatibility." << std::endl;
            spdlog::error("MOD COMPATIBILITY WARNING: Failed to rename MGS2HFBladeMod.dll: {}", ec.message());
            spdlog::error("MOD COMPATIBILITY WARNING: Please rename MGS2HFBladeMod.dll to MGS2HFBladeMod.asi manually to fix ASI Loader compatibility.");
        }
        else
        {
            std::cout << "MOD COMPATIBILITY WARNING: MGS2HFBladeMod.dll has been renamed to MGS2HFBladeMod.asi to fix ASI Loader compatibility." << std::endl;
            std::cout << "MOD COMPATIBILITY WARNING: You may have to restart your game for the fix to take effect." << std::endl;
            spdlog::error("MOD COMPATIBILITY WARNING: MGS2HFBladeMod.dll has been renamed to MGS2HFBladeMod.asi to fix ASI Loader compatibility.");
            spdlog::error("MOD COMPATIBILITY WARNING: You may have to restart your game for the fix to take effect.");
        }
    }

    if ((eGameType & (LAUNCHER|MGS3)) && Util::CheckForASIFiles("KeepAiming", false, false, nullptr))
    {
        std::string message =
            "MOD COMPATIBILITY ALERT:\n"
            "KeepAiming.asi's functionality has been integrated directly into MGSHDFix.\n"
            "Having both present will cause the game to crash on launch.\n"
            "Please remove KeepAiming.asi from your game directory.\n";

        int result = MessageBoxA(
            nullptr,
            message.c_str(),
            "MGSHDFix Mod Compatibility Alert",
            MB_ICONERROR | MB_OK
        );
        spdlog::error("MOD COMPATIBILITY ALERT: KeepAiming.asi's functionality has been integrated directly into MGSHDFix.");
        spdlog::error("MOD COMPATIBILITY ALERT: Having both present will cause the game to crash on launch.");
        spdlog::error("MOD COMPATIBILITY ALERT: Please remove KeepAiming.asi from your game directory.");
        if (eGameType & LAUNCHER)
        {
            return FreeLibraryAndExitThread(baseModule, 1); //Only exit if we're in the launcher. KeepAiming will crash the game
        }                                                           //immediately once our init mutex releases, which will prevent the console & log messages from being printed.
    }

}
