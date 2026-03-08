#include "stdafx.h"

#include "common.hpp"
#include "logging.hpp"
#include "custom_font.hpp"

#include "config.hpp"

void CustomFont::Initialize() const
{
    if ((eGameType == MgsGame::MGS3 || eGameType == MgsGame::MGS2) && isEnabled)
    {

        if (sCustomFontPath.empty())
        {
            spdlog::error("MGS 2 | MGS 3: Custom Font: Path to font not set");
            return;
        }

        spdlog::info("MGS 2 | MGS 3: Custom Font: Finding original font on memory");

        // search font on exe memory
        uint8_t* fontSignatureResult = Memory::PatternScan(baseModule, "1F 8B 08 ?? ?? ?? ?? ?? ?? ?? 4D 47 53 5F 46 6F", "MGS2_MGS3_FontScan");
        if (!fontSignatureResult)
        {
            spdlog::error("MGS 2 | MGS 3: Custom Font: Custom Font: Can't find the font signature");
            return;
        }

        uint8_t* fontDataPtr = fontSignatureResult;
        uint32_t offfset = 0;
        if (eGameType == MgsGame::MGS2)
        {
            offfset = 16;
        }
        else if (eGameType == MgsGame::MGS3)
        {
            offfset = 12;
        }
        uint32_t* fontSizePtr = reinterpret_cast<uint32_t*>(fontSignatureResult - offfset);

        spdlog::info("MGS 2 | MGS 3: Custom Font: Original font found with address: {:s}+{:x}", sExeName.c_str(), (uintptr_t)fontDataPtr - (uintptr_t)baseModule);
        spdlog::info("MGS 2 | MGS 3: Custom Font: Original font size ptr: {:s}+{:x}", sExeName.c_str(), (uintptr_t)fontSizePtr - (uintptr_t)baseModule);
        spdlog::info("MGS 2 | MGS 3: Custom Font: Original font size {}", *fontSizePtr);

        // read new font file
        std::ifstream fontFile(sCustomFontPath, std::ios::binary | std::ios::ate);
        if (!fontFile.is_open())
        {
            spdlog::error("MGS 2 | MGS 3: Custom Font: Font file not opened: {}", sCustomFontPath);
            return;
        }

        std::streamsize newFontSize = fontFile.tellg();
        fontFile.seekg(0, std::ios::beg);
        spdlog::info("MGS 2 | MGS 3: Custom Font: New font size: {}.", newFontSize);

        if (newFontSize <= 0 && newFontSize > *fontSizePtr)
        {
            spdlog::error("MGS 2 | MGS 3: Custom Font: Font file empty or bigger of originall");
            fontFile.close();
            return;
        }

        // read new font data
        std::vector<char> fontBuffer(newFontSize);
        if (!fontFile.read(fontBuffer.data(), newFontSize))
        {
            spdlog::error("MGS 2 | MGS 3: Custom Font: New Font data not readed");
            fontFile.close();
            return;
        }
        fontFile.close();

        spdlog::info("MGS 2 | MGS 3: Custom Font: Replacing font on memory...");

        // path size
        Memory::Write((uintptr_t)fontSizePtr, static_cast<uint32_t>(newFontSize));
        spdlog::info("MGS 2 | MGS 3: Custom Font: Font size replaced to {}.", *fontSizePtr);

        // path font data
        Memory::PatchBytes(reinterpret_cast<uintptr_t>(fontDataPtr), fontBuffer.data(), fontBuffer.size());
        spdlog::info("MGS 2 | MGS 3: Custom Font: Font data replaced");
    }
}
