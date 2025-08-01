#pragma once
#include <d3d11.h>

class VectorScalingFix final
{
private:
    bool CompileGeometryShader();

    static constexpr int DEFAULT_LINE_SCALE = 360;
    ID3DBlob* compiledShaderBytecode = nullptr;

public:
    void Initialize();
    void LoadCompiledShader() const;

    bool bEnableVectorLineFix = false;
    bool bNeedsCompiler = false;
    double iVectorLineScale = 360;
};

inline VectorScalingFix g_VectorScalingFix;
