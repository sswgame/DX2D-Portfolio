#ifndef _FADE_POSTEFFECT
#define  _FADE_POSTEFFECT

#include "value.fx"

struct VTX_IN
{
    float3 pos : POSITION;
    float2 UV : TEXCOORD;
};

struct VTX_OUT
{
    float4 position : SV_Position;
    float2 UV : TEXCOORD;
};

//g_tex_0 : RayTCurrent GetRenderTargetSampleCount Copy Texture

VTX_OUT VS_FadePostEffect(VTX_IN input)
{
    VTX_OUT output = (VTX_OUT) 0.f;

    output.position = mul(float4(input.pos, 1.f), g_matWVP);
    output.UV = input.UV;

    return output;
}

float4 PS_FadePostEffect(VTX_OUT input) : SV_Target
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


    float2 screenUV = input.position.xy / resolution;
    outColor = g_tex_0.Sample(g_sam_1, screenUV);

    //ȸ��ó��
    outColor = dot(outColor.rgb, float3(0.3f, 0.59f, 0.11f));

    return outColor;

}
#endif
