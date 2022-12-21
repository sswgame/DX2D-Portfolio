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

//내가 정의한 것들(Start)
#define Alpha   g_float_0

//내가 정의한 것들 (End)
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
  
    //input.position.xy => 스크린 좌표계(픽셀좌표)
    //따라서 화면 해상도로 자신의 스크린 좌표를 나누면, 화면 전체 기준 UV 위치가 된다
    //float2 direction = normalize(float2(0.5f, 0.5f) - _in.vUV);
    
    //float fDist = distance(float2(0.5f, 0.5f), _in.vUV);

    // 왜곡 강도에 영향을 주는 중심으로부터 떨어진 비율( 중심에 가까울 수록 0에 가깝다.)
    //float fRatio = (fDist / 0.5f);
    //fRatio = fRatio * (0.2f + (fRatio) * 0.4f);

    //vScreenUV += direction * sin(abs((fRatio - g_fAccTime * 0.1f) * 100.f)) * 0.001f;


    float2 screenUV = input.vPosition.xy / resolution;
    outColor = g_tex_0.Sample(g_sam_1, screenUV);

    //회색처리
    outColor = dot(outColor.rgb, float3(0.3f, 0.59f, 0.11f));
    outColor.a = Alpha;

    return outColor;
}

#endif
