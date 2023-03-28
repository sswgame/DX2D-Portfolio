#ifndef _PLAYER_HP_BAR
#define _PLAYER_HP_BAR

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

//���� ������ �͵�(Start)
#define LeftTop         g_vec2_0
#define Scale           g_vec2_1
#define BackgroundSize  g_vec2_2
#define Offset          g_vec2_2
#define HasMetaInfo     g_int_0
#define HP_Ratio        g_float_0
#define OffsetRatio     g_float_1

#define PointSampler    g_sam_1
#define OutputTexture   g_tex_0
//���� ������ �͵� (End)
VTX_OUT VS_PlayerHPBar(VTX_IN input)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(input.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(input.vPos, 1.f), g_matWorld).xyz;
    output.vUV = input.vUV;
    
    return output;
}

float4 PS_PlayerHPBar(VTX_OUT input) : SV_Target
{
    float4 outColor = (float4) 0.f;

    
    //��Ʋ�󽺿��� �ش� ��ǥ���� �Ѱ��� ���� �ƴ� ���� ������ �� �����ؾ��Ѵ�.
    if (g_btex_0)
    {
        //��Ÿ������ �ִ� ��� �װ��� ���� ����
        if (true == HasMetaInfo)
        {
            float2 UV = input.vUV * BackgroundSize;
            UV = UV - (BackgroundSize - Scale) / 2.f + LeftTop;
            const bool isPixelInsideSprite =   (LeftTop.x <= UV.x && UV.x <= LeftTop.x + Scale.x)
                                            && (LeftTop.y <= UV.y && UV.y <= LeftTop.y + Scale.y);
            if (false == isPixelInsideSprite)
            {
                discard;
            }

            outColor = OutputTexture.Sample(PointSampler, UV);

            const float offset = LeftTop.x + Scale.x * OffsetRatio;
            const float hpBarWidth = (1.f - OffsetRatio) * Scale.x;
            // g_float_0: 0 ~ 1.f;
            const float remainHP = offset + hpBarWidth * HP_Ratio;
            float color = 80.f / 255.f;
            if (offset < UV.x && UV.x < remainHP)
            {
                outColor.rgb = float3(1.f, color, color);
            }
        }
    }
    //�ؽ�ó�� ���ٸ� ����Ÿ �������� ���
    else
    {
        outColor = float4(1.f, 0.f, 1.f, 1.f);
    }

    if(outColor.a<=0.f)
    {
        discard;
    }
    return outColor;
}

#endif
