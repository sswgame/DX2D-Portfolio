#ifndef _AFTERIMAGE
#define _AFTERIMAGE

#include "value.fx"


struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
    float4 vPosition : SV_Position;
    float3 vWorldPos : POSITION;
    float2 vUV : TEXCOORD;
};


// =========================
// AfterImage Shader
// g_tex_0 : Output Texture
// Rasterizer : CULL_NONE
// BlendState : AlphaBlend
// DepthStencilState : LESS
// DOMAIN : Translucent
// =========================

//내가 정의한 것들(Start)
#define HasMetaInfo     g_int_0
#define ShowImage       g_int_1
#define Alpha           g_float_0

#define LeftTop         g_vec2_0
#define Scale           g_vec2_1
#define BackgroundSize  g_vec2_2
#define Offset          g_vec2_2

#define PointSampler    g_sam_1
#define Texture         g_tex_0
//내가 정의한 것들 (End)
VTX_OUT VS_AfterImage(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_AfterImage(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    if (g_btex_0)
    {
        //메타정보가 있는 경우 그것을 토대로 설정
        if (true == HasMetaInfo)
        {
            float2 UV = _in.vUV * BackgroundSize;
            UV = UV - (BackgroundSize - Scale) / 2.f + LeftTop;
            const bool isPixelInsideSprite = (LeftTop.x <= UV.x && UV.x <= LeftTop.x + Scale.x)
                                            && (LeftTop.y <= UV.y && UV.y <= LeftTop.y + Scale.y);
            if (false == isPixelInsideSprite)
            {
                discard;
            }
                
            vOutColor = Texture.Sample(PointSampler, UV);
            vOutColor.rgba = vOutColor*float4(1.f, 1.f, 1.f,Alpha);
        }
    }
    //애니메이션이 없는 경우 마젠타 색상으로 출력
    else
    {
        vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    }
    
    if (vOutColor.a <= 0.f)
    {
        discard;
    }

    return vOutColor;
}

#endif