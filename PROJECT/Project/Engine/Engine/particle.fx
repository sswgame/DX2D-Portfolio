#ifndef _PARTICLE
#define _PARTICLE

#include "value.fx"
#include "struct.fx"
#include "func.fx"

// =======================
// Particle Update Shader
#define MaxThreadCount      g_int_0
#define MaxParticleCount    g_int_1

#define CreateRange         g_float_0
#define WorldPos            float3(g_float_1, g_float_2, g_float_3)

#define MinLifeTime         g_vec2_0.x
#define MaxLifeTime         g_vec2_0.y

#define MinSpeed            g_vec2_1.x
#define MaxSpeed            g_vec2_1.y

#define StartColor          g_vec4_0
#define EndColor            g_vec4_1
#define StartScale          g_vec4_2
#define EndScale            g_vec4_3


// =======================
#define PI                  3.141592f
#define Direction           g_vec2_2
RWStructuredBuffer<tParticle> ParticleBuffer : register(u0);
RWStructuredBuffer<tParticleData> DataBuffer : register(u1);

[numthreads(32, 1, 1)]
void CS_Particle(int3 id : SV_DispatchThreadID)
{   
    if (MaxParticleCount < id.x)
    {
        return;
    }
          
    // 파티클이 비활성화 되어있다면...
    if (false == ParticleBuffer[id.x].isAlive)
    {
        int originalValue = DataBuffer[0].aliveCount;
        
        // Alive 기회가 있으면 반복
        while (0 < originalValue)
        {
            int inputValue = originalValue - 1;
            int outputValue = 0;
    
            InterlockedCompareExchange(DataBuffer[0].aliveCount, originalValue, inputValue, outputValue);
    
            // 활성화 성공한 경우, Alive 를 True 로 변경
            if (originalValue == outputValue)
            {
                ParticleBuffer[id.x].isAlive = true;
                break;
            }
            
            originalValue = DataBuffer[0].aliveCount;
        }
        
        
        // 활성화 된 파티클의 초기값을 랜덤하게 부여
        if (ParticleBuffer[id.x].isAlive)
        {
            // 랜덤한 위치 부여
            float2 vSampleUV = float2(((float) id.x / (float) MaxThreadCount) + accTime, accTime);
            vSampleUV.y += sin(vSampleUV.x * 3.141592f * 2.f) * 0.5f;
            float3 vOut = GaussianSample(g_noise_cloud, vSampleUV).rgb;
                        
            float3 vRandom =
            {
                GaussianSample(g_noise_01, vSampleUV + float2(0.1f, 0.f)).r
                , GaussianSample(g_noise_01, vSampleUV + float2(0.2f, 0.f)).r
                , GaussianSample(g_noise_01, vSampleUV + float2(0.3f, 0.f)).r
            };
            
            ParticleBuffer[id.x].pos = WorldPos + (vOut - 0.5f) * CreateRange;
            ParticleBuffer[id.x].scale = float3(10.f, 10.f, 1.f);
            ParticleBuffer[id.x].direction = normalize(vRandom - 0.5f);
                        
            ParticleBuffer[id.x].elapsedTime = 0.f;
            ParticleBuffer[id.x].maxTime = MinLifeTime + (MaxLifeTime - MinLifeTime) * vOut.r;
        }
    }
    
    // 활성화 된 파티클
    else
    {
        // 생존시간 누적
        ParticleBuffer[id.x].elapsedTime += deltaTime;
        
        // 최대 생존시간을 넘어선 경우
        if (ParticleBuffer[id.x].maxTime <= ParticleBuffer[id.x].elapsedTime)
        {
            ParticleBuffer[id.x].isAlive = 0;
            return;
        }
                
        float fLifeRatio = ParticleBuffer[id.x].elapsedTime / ParticleBuffer[id.x].maxTime;
        
        float fSpeed = MinSpeed + (MaxSpeed - MinSpeed) * fLifeRatio;
        float4 vColor = StartColor + (EndColor - StartColor) * fLifeRatio;
        float4 vScale = StartScale + (EndScale - StartScale) * fLifeRatio;
        
        
        ParticleBuffer[id.x].pos += ParticleBuffer[id.x].direction * fSpeed * deltaTime;
        ParticleBuffer[id.x].color = vColor;
        ParticleBuffer[id.x].scale = vScale.xyz;
    }
}

#endif