#ifndef _PARTICLERENDER
#define _PARTICLERENDER

#include "value.fx"
#include "struct.fx"

// ====================
// ParticleRenderShader
// Mesh     : PointMesh
// Domain   : Opaque
// Blend    : AlphaBlend
// DepthStencil : Less
// Rasterizer : CULL_NONE


// Parameter
#define PARTICLE_INDEX      g_int_1
#define POS_INHERIT         g_int_0
#define POINT_FILTER        g_sam_0
#define ANISOTROPIC_FILTER  g_sam_1
#define INPUT_TEXTURE       g_tex_0
#define IS_TEXTURE_BINDED   g_btex_0
// ====================
StructuredBuffer<tParticle> ParticleBuffer : register(t16);

struct VTX_IN
{
    uint InstID : SV_InstanceID;
};

struct VTX_OUT
{
    uint instanceID : SV_InstanceID;
};


VTX_OUT VS_ParticleRender(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
   
    output.instanceID = _in.InstID;
    
    return output;
}

struct GS_OUT
{
    float4 position : SV_Position;
    float2 UV : TEXCOORD;
    uint instanceID : SV_InstanceID;
};

// 1 개의 정점을 6개로 분할
[maxvertexcount(6)]
void GS_ParticleRender(point VTX_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
    uint ID = input[0].instanceID;
    
    // 해당 인스턴스가 비활성화면, 출력 정점이 없다 ==> 아무일도 없다
    if (false == ParticleBuffer[ID].isAlive)
    {
        return;
    }
     // 파티클 정점의 월드위치 구하기
    float3 particleWorldPos = (float3) 0.f;

    if (POS_INHERIT)
    {
        particleWorldPos = g_matWorld._41_42_43 + ParticleBuffer[ID].pos;
    }
    else
    {
        particleWorldPos = ParticleBuffer[ID].pos;
    }
    // World 좌표를 View, Proj 으로 변환
    float4 viewPos = mul(float4(particleWorldPos, 1.f), g_matView);
    float3 scale = ParticleBuffer[ID].scale;
    
    // ViewSpace 에서 Point 를 Rect 로 분할
    // 0 --- 1
    // |  \  |
    // 3 --- 2
    float3 arrRect[4] =
    {
        float3(viewPos.x - scale.x / 2.f, viewPos.y + scale.y / 2.f, viewPos.z),
        float3(viewPos.x + scale.x / 2.f, viewPos.y + scale.y / 2.f, viewPos.z),
        float3(viewPos.x + scale.x / 2.f, viewPos.y - scale.y / 2.f, viewPos.z),
        float3(viewPos.x - scale.x / 2.f, viewPos.y - scale.y / 2.f, viewPos.z)
    };
    
    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    
    for (int i = 0; i < 4; ++i)
    {
        output[i].position = mul(float4(arrRect[i], 1.f), g_matProj);
        output[i].instanceID = input[0].instanceID;
    }
    
    output[0].UV = float2(0.f, 0.f);
    output[1].UV = float2(1.f, 0.f);
    output[2].UV = float2(1.f, 1.f);
    output[3].UV = float2(0.f, 1.f);
        
            
    outputStream.Append(output[0]);
    outputStream.Append(output[2]);
    outputStream.Append(output[3]);
    outputStream.RestartStrip();
    
    outputStream.Append(output[2]);
    outputStream.Append(output[0]);
    outputStream.Append(output[1]);
    outputStream.RestartStrip();
}


float4 PS_ParticleRender(GS_OUT input) : SV_Target
{
    float4 outColor = (float4) 0.f;
    
    if (IS_TEXTURE_BINDED)
    {
        outColor = INPUT_TEXTURE.Sample(ANISOTROPIC_FILTER, input.UV) * ParticleBuffer[input.instanceID].color;
    }
    else
    {
        outColor = ParticleBuffer[input.instanceID].color;
    }
    
    return outColor;
}

#endif