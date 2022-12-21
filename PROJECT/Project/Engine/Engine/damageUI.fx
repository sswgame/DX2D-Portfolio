#ifndef _PLAYER
#define _PLAYER

#include "value.fx"

//number
StructuredBuffer<tNumber> g_arrNum : register(t13);

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
// damageUI shader
// g_tex_0 : Output Texture
// Rasterizer : CULL_NONE
// BlendState : Default
// DepthStencilState : NO_TEST_NO_WRITE
// DOMAIN : Masked
// =========================

#define NUMBER_COUNT    g_int_0
#define ALPHA           g_float_0
#define R               g_float_1
#define G               g_float_2
#define B               g_float_3

VTX_OUT VS_DamageUI(VTX_IN input)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(input.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(input.vPos, 1.f), g_matWorld).xyz;
    output.vUV = input.vUV;
    
    return output;
}

float4 PS_DamageUI(VTX_OUT input) : SV_Target
{
    int index = input.vUV.x * NUMBER_COUNT;
    float2 UV = input.vUV;

    UV.x = g_arrNum[index].number * 0.1f + (UV.x - (1.f / NUMBER_COUNT) * index) * NUMBER_COUNT * 0.1f;

    float4 outputColor = g_tex_0.Sample(g_sam_0, UV);

    if(outputColor.a > 0.f)
    {
        outputColor.a = ALPHA;
    }
    float3 randomColor = float3(R, G, B);
    outputColor.rgb *= randomColor;

    return outputColor;
}

#endif
