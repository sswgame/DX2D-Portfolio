#ifndef _MONSTER
#define _MONSTER

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
// Monster Shader
// g_tex_0 : Output Texture
// Rasterizer : CULL_NONE
// BlendState : ALPHA_BLEND
// DepthStencilState : LESS
// DOMAIN : Masked
// =========================

//내가 정의한 것들(Start)
#define LeftTop         g_vec2_0
#define Scale           g_vec2_1
#define BackgroundSize  g_vec2_2
#define Offset          g_vec2_2
#define OnAttacked      g_int_0

#define PointSampler    g_sam_1
#define OutputTexture   g_tex_0
//내가 정의한 것들 (End)
VTX_OUT VS_Monster(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Monster(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    // Animation 정보가 있는 경우
    if (g_useAnim2D)
    {
        float2 vUV = _in.vUV * g_vBackgroundSize;
        vUV = vUV - (g_vBackgroundSize - g_vSlice) / 2.f + g_vLT - g_vOffset;
        if (vUV.x < g_vLT.x || g_vLT.x + g_vSlice.x < vUV.x
            || vUV.y < g_vLT.y || g_vLT.y + g_vSlice.y < vUV.y)
        {
            discard;
        }
                
        vOutColor = g_Atlas.Sample(g_sam_1, vUV);
    }

    if (vOutColor.a <= 0.f)
    {
        discard;
    }

    if (OnAttacked)
    {
        vOutColor.rgb = float3(0.8f, 0.f, 0.f);
    }
    return vOutColor;
}


#endif
