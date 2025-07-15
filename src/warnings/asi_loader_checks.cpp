#include "common.hpp"
#include "asi_loader_checks.hpp"
#include "logging.hpp"

void ASILoaderCompatibility::Check()
{
    spdlog::info("ASI Loader Compatibility Check: Checking for duplicate instances of ASI Loader (ie d3d11.dll, dxgi.dll).");
    //Don't simplify by removing filesystem::exists() from this check. While GetFileDescription does handle non-existent files own its own, checking filesystem::exists() first saves 400+ ms of initialization time
    if (std::filesystem::exists(sExePath / "d3d11.dll") && (Util::GetFileDescription((sExePath / "d3d11.dll").string()) == Util::GetFileDescription((sExePath / "winhttp.dll").string())))
    {
        Logging::ShowConsole();
        std::cout << "DUPLICATE MOD LOADER ERROR: Multiple ASI Loader .dll's detected! This can cause inconsistent bugs and crashes." << std::endl;
        std::cout << "DUPLICATE MOD LOADER ERROR: Please delete d3d11.dll, it has been replaced by winhttp.dll & wininit.dll." << std::endl;
        spdlog::error("DUPLICATE MOD LOADER ERROR: Multiple ASI Loader .dll installations detected! This can cause inconsistent bugs and crashes.");
        spdlog::error("DUPLICATE MOD LOADER ERROR: Please delete d3d11.dll, it has been replaced by winhttp.dll & wininit.dll.");
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
        Logging::ShowConsole();
        std::cout << "DUPLICATE MOD LOADER ERROR: Multiple ASI Loader .dll's detected! This can cause inconsistent bugs and crashes." << std::endl;
        std::cout << "DUPLICATE MOD LOADER ERROR: Please delete dxgi.dll, it has been replaced by winhttp.dll & wininit.dll." << std::endl;
        spdlog::error("DUPLICATE MOD LOADER ERROR: Multiple ASI Loader .dll installations detected! This can cause inconsistent bugs and crashes.");
        spdlog::error("DUPLICATE MOD LOADER ERROR: Please delete dxgi.dll, it has been replaced by winhttp.dll & wininit.dll.");

    }

    spdlog::info("ASI Mod Compatibility Check: Checking for common mod installation issues.");
    if (std::filesystem::exists(sExePath / "MGS2HFBladeMod.dll"))
    {
        Logging::ShowConsole();
        std::cout << "MOD COMPATIBILITY WARNING: MGS2HFBladeMod.dll has an incorrect extension!" << std::endl;

        spdlog::error("MOD COMPATIBILITY WARNING: MGS2HFBladeMod.dll has an incorrect extension!");
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


    spdlog::info("ASI Mod Compatibility Check: Checking for duplicate MGSHDFix installations.");
    Util::CheckForASIFiles(sFixName, true, true, nullptr); //Exit thread & warn the user if multiple copies of MGSHDFix are trying to initialize.
}
