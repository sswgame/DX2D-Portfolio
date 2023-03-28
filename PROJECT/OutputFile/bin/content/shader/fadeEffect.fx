#ifndef _PLAYER
#define _PLAYER

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
// Fade Effect Shader
// g_tex_0 : Output Texture
// Rasterizer : CULL_NONE
// BlendState : Default
// DepthStencilState : NO_TEST_NO_WRITE
// DOMAIN : Masked
// =========================

//���� ������ �͵�(Start)
#define Alpha   g_float_0

//���� ������ �͵� (End)
VTX_OUT VS_FadeEffect(VTX_IN input)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(input.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(input.vPos, 1.f), g_matWorld).xyz;
    output.vUV = input.vUV;
    
    return output;
}

float4 PS_FadeEffect(VTX_OUT input) : SV_Target
{
    float4 outColor = (float4) 0.f;
  
    //input.position.xy => ��ũ�� ��ǥ��(�ȼ���ǥ)
    //���� ȭ�� �ػ󵵷� �ڽ��� ��ũ�� ��ǥ�� ������, ȭ�� ��ü ���� UV ��ġ�� �ȴ�
    //float2 direction = normalize(float2(0.5f, 0.5f) - _in.vUV);
    
    //float fDist = distance(float2(0.5f, 0.5f), _in.vUV);

    // �ְ� ������ ������ �ִ� �߽����κ��� ������ ����( �߽ɿ� ����� ���� 0�� ������.)
    //float fRatio = (fDist / 0.5f);
    //fRatio = fRatio * (0.2f + (fRatio) * 0.4f);

    //vScreenUV += direction * sin(abs((fRatio - g_fAccTime * 0.1f) * 100.f)) * 0.001f;


    float2 screenUV = input.vPosition.xy / resolution;
    outColor = g_tex_0.Sample(g_sam_1, screenUV);

    //ȸ��ó��
    outColor = dot(outColor.rgb, float3(0.3f, 0.59f, 0.11f));
    outColor.a = Alpha;

    return outColor;
}

#endif
