#ifndef _DASH
#define _DASH

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
// Player Shader
// g_tex_0 : Output Texture
// Rasterizer : CULL_NONE
// BlendState : Default
// DepthStencilState : NO_TEST_NO_WRITE
// DOMAIN : Masked
// =========================

//내가 정의한 것들(Start)
#define LeftTop         g_vec2_0
#define Scale           g_vec2_1
#define BackgroundSize  g_vec2_2
#define Offset          g_vec2_2
#define HasMetaInfo     g_int_0
#define Has_DashCount   g_int_1

#define PointSampler    g_sam_1
#define OutputTexture   g_tex_0
//내가 정의한 것들 (End)
VTX_OUT VS_Dash(VTX_IN input)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(input.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(input.vPos, 1.f), g_matWorld).xyz;
    output.vUV = input.vUV;
    
    return output;
}

float4 PS_Dash(VTX_OUT input) : SV_Target
{
    float4 outColor = (float4) 0.f;

    
    //아틀라스에서 해당 좌표값을 넘겨준 경우와 아닌 경우로 나눠서 또 생각해야한다.
    if (g_btex_0)
    {
        //메타정보가 있는 경우 그것을 토대로 설정
        if (true == HasMetaInfo)
        {
            float2 UV = input.vUV * BackgroundSize;
            UV = UV - (BackgroundSize - Scale) / 2.f + LeftTop;
            const bool isPixelInsideSprite = (LeftTop.x <= UV.x && UV.x <= LeftTop.x + Scale.x)
                                            && (LeftTop.y <= UV.y && UV.y <= LeftTop.y + Scale.y);
            if (false == isPixelInsideSprite)
            {
                discard;
            }

            outColor = OutputTexture.Sample(PointSampler, UV);

            if (Has_DashCount && LeftTop.x <= UV.x && UV.x <=LeftTop.x +Scale.x)
            {
                float3 color = float3(58.f / 255.f, 181.f / 255.f, 243.f / 255.f);
                {
                    outColor.rgb = color;
                }
            }
            
        }
    }
    //텍스처가 없다면 마젠타 색상으로 출력
    else
    {
        outColor = float4(1.f, 0.f, 1.f, 1.f);
    }

    return outColor;
}

#endif
