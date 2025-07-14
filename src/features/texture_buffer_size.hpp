#pragma once

class TextureBufferSize final
{
public:
    void Initialize() const;

    int iTextureBufferSizeMB = 128; // Default value, can be overridden by config
};

inline TextureBufferSize g_TextureBufferSize;
