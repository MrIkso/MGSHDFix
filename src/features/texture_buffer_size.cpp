#include "texture_buffer_size.hpp"
#include "common.hpp"
#include "spdlog/spdlog.h"

void TextureBufferSize::Initialize()
{
    if (iTextureBufferSizeMB > 128 && (eGameType & (MG | MGS3)))
    {
        // MG/MG2 | MGS3: texture buffer size extension
        uint32_t NewSize = iTextureBufferSizeMB * 1024 * 1024;

        // Scan for the 9 mallocs which set buffer inside CTextureBuffer::sInstance
        bool failure = false;
        for (int i = 0; i < 9; i++)
        {
            uint8_t* MGS3_CTextureBufferMallocResult = Memory::PatternScanSilent(baseModule, "75 ?? B9 00 00 00 08 FF");
            if (MGS3_CTextureBufferMallocResult)
            {
                uint32_t* bufferAmount = (uint32_t*)(MGS3_CTextureBufferMallocResult + 3);
                spdlog::info("MG/MG2 | MGS 3: Texture Buffer Size: #{} ({:s}+{:x}) old buffer size: {}", i, sExeName.c_str(), (uintptr_t)MGS3_CTextureBufferMallocResult - (uintptr_t)baseModule, (uintptr_t)*bufferAmount);
                Memory::Write((uintptr_t)bufferAmount, NewSize);
                spdlog::info("MG/MG2 | MGS 3: Texture Buffer Size: #{} ({:s}+{:x}) new buffer size: {}", i, sExeName.c_str(), (uintptr_t)MGS3_CTextureBufferMallocResult - (uintptr_t)baseModule, (uintptr_t)*bufferAmount);
            }
            else
            {
                spdlog::error("MG/MG2 | MGS 3: Texture Buffer Size: #{}: Pattern scan failed.", i);
                failure = true;
                break;
            }
        }

        if (!failure)
        {
            // CBaseTexture::Create seems to contain code that mallocs buffers based on 16MiB shifted by index of the mip being loaded
            // (ie: size = 16MiB >> mipIndex)
            // We'll make sure to increase the base 16MiB size it uses too
            uint8_t* MGS3_CBaseTextureMallocScanResult = Memory::PatternScanSilent(baseModule, "75 ?? 00 00 00 08 8B ??");
            if (MGS3_CBaseTextureMallocScanResult)
            {
                uint32_t* bufferAmount = (uint32_t*)(MGS3_CBaseTextureMallocScanResult + 3);
                spdlog::info("MG/MG2 | MGS 3: Texture Buffer Size: #{} ({:s}+{:x}) old buffer size: {}", 9, sExeName.c_str(), (uintptr_t)MGS3_CBaseTextureMallocScanResult - (uintptr_t)baseModule, (uintptr_t)*bufferAmount);
                Memory::Write((uintptr_t)bufferAmount, NewSize);
                spdlog::info("MG/MG2 | MGS 3: Texture Buffer Size: #{} ({:s}+{:x}) new buffer size: {}", 9, sExeName.c_str(), (uintptr_t)MGS3_CBaseTextureMallocScanResult - (uintptr_t)baseModule, (uintptr_t)*bufferAmount);
            }
            else
            {
                spdlog::error("MG/MG2 | MGS 3: Texture Buffer Size: #{}: Pattern scan failed.", 9);
            }
        }
    }
}

