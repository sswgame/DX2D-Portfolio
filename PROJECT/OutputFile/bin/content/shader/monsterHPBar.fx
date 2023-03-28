#ifndef _MONSTER_HP_BAR
#define _MONSTER_HP_BAR

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
// MonsterHPBar Shader
// g_tex_0 : Output Texture
// Rasterizer : CULL_NONE
// BlendState : AlphaBlend
// DepthStencilState : LESS
// DOMAIN : Translucent
// =========================

//���� ������ �͵�(Start)
#define LeftTop         g_vec2_0
#define Scale           g_vec2_1
#define BackgroundSize  g_vec2_2
#define Offset          g_vec2_2
#define HasMetaInfo     g_int_0
#define IsFacingLeft    g_int_1
#define HP_Ratio        g_float_0

#define PointSampler    g_sam_1
#define OutputTexture   g_tex_0
VTX_OUT VS_MonsterHPBar(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_MonsterHPBar(VTX_OUT input) : SV_Target
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
            const bool isPixelInsideSprite = (LeftTop.x <= UV.x && UV.x <= LeftTop.x + Scale.x)
                                            && (LeftTop.y <= UV.y && UV.y <= LeftTop.y + Scale.y);
            if (false == isPixelInsideSprite)
            {
                discard;
            }

            outColor = OutputTexture.Sample(PointSampler, UV);


            float color = 50.f / 255.f;
            float hpHeight = Scale.y;
            float remainHP = Scale.x * HP_Ratio;

            if (IsFacingLeft)
            {
                float damagedRatio = clamp(Scale.x - remainHP, 0.f, 1.f);
                if (LeftTop.x <= UV.x && UV.x <= LeftTop.x + damagedRatio
                && LeftTop.y <= UV.y && UV.y <= LeftTop.y + hpHeight)
                {
                    return float4(color, color, color, 1.f);
                }
                return float4(1.f, color, color, 1.f);
            }
            else
            {
            //���� HP ����

                if (LeftTop.x <= UV.x && UV.x <= LeftTop.x + remainHP
                && LeftTop.y <= UV.y && UV.y <= LeftTop.y + hpHeight)
                {
                    return float4(1.f, color, color, 1.f);
                }
                return float4(color, color, color, 1.f);
            }
        }
    }
    //�ؽ�ó�� ���ٸ� ����Ÿ �������� ���
    else
    {
        outColor = float4(1.f, 0.f, 1.f, 1.f);
    }

    return outColor;
    
}

#endif