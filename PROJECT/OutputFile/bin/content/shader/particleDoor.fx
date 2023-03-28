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
void CS_ParticleDoor(int3 id : SV_DispatchThreadID)
{
    if (MaxParticleCount < id.x)
    {
        return;
    }
          
    // ��ƼŬ�� ��Ȱ��ȭ �Ǿ��ִٸ�...
    if (false == ParticleBuffer[id.x].isAlive)
    {
        int originalValue = DataBuffer[0].aliveCount;
        
        // Alive ��ȸ�� ������ �ݺ�
        while (0 < originalValue)
        {
            int inputValue = originalValue - 1;
            int outputValue = 0;
    
            InterlockedCompareExchange(DataBuffer[0].aliveCount, originalValue, inputValue, outputValue);
    
            // Ȱ��ȭ ������ ���, Alive �� True �� ����
            if (originalValue == outputValue)
            {
                ParticleBuffer[id.x].isAlive = true;
                break;
            }
            
            originalValue = DataBuffer[0].aliveCount;
        }
        
        
        // Ȱ��ȭ �� ��ƼŬ�� �ʱⰪ�� �����ϰ� �ο�
        // �� ���ʸ鿡�� ����Ͽ� �ٸ��� ������ ���� �Ѵ�.
        if (ParticleBuffer[id.x].isAlive)
        {
            // ������ ��ġ �ο�
            float2 sampleUV = float2(((float) id.x / (float) MaxThreadCount) + accTime, accTime);
            sampleUV.y += sin(sampleUV.x * PI * 2.f) * 0.5f;
   
            float3 random =
            {
                GaussianSample(g_noise_cloud, sampleUV + float2(0.1f, 0.f)).r
                , GaussianSample(g_noise_cloud, sampleUV + float2(0.2f, 0.f)).r
                , GaussianSample(g_noise_cloud, sampleUV + float2(0.3f, 0.f)).r
            };
            
            ParticleBuffer[id.x].pos = WorldPos + (random - 0.5f).x * CreateRange;
            ParticleBuffer[id.x].elapsedTime = 0.f;
            ParticleBuffer[id.x].maxTime = MinLifeTime + (MaxLifeTime - MinLifeTime) * random.x;
            ParticleBuffer[id.x].direction = float3(Direction.x, Direction.y, 0.f);
            if (1.f - abs(Direction.x) < 0.001f)
            {
                ParticleBuffer[id.x].pos.x = WorldPos.x;
                ParticleBuffer[id.x].pos.y = clamp(ParticleBuffer[id.x].pos.y, WorldPos.y - CreateRange, WorldPos.y + CreateRange);
            }
            else if (1.f - abs(Direction.y) < 0.001f)
            {
                ParticleBuffer[id.x].pos.y = WorldPos.y;
                ParticleBuffer[id.x].pos.x = clamp(ParticleBuffer[id.x].pos.x, WorldPos.x - CreateRange, WorldPos.x + CreateRange);
            }
            
        }
    }
    
    // Ȱ��ȭ �� ��ƼŬ
    else
    {
        // �����ð� ����
        ParticleBuffer[id.x].elapsedTime += deltaTime;
        
        // �ִ� �����ð��� �Ѿ ���
        if (ParticleBuffer[id.x].maxTime <= ParticleBuffer[id.x].elapsedTime)
        {
            ParticleBuffer[id.x].isAlive = 0;
            return;
        }
                
        float lifeRatio = ParticleBuffer[id.x].elapsedTime / ParticleBuffer[id.x].maxTime;
        float speed = MinSpeed + (MaxSpeed - MinSpeed) * lifeRatio;
        float4 color = StartColor + (EndColor - StartColor) * lifeRatio;
        if (id.x % 2 == 0)
        {
            ParticleBuffer[id.x].scale = (StartScale + (EndScale - StartScale) * lifeRatio).xyz;
        }
        else
        {
            ParticleBuffer[id.x].scale = (EndScale - (EndScale - StartScale) * lifeRatio).xyz;

        }
        ParticleBuffer[id.x].pos += ParticleBuffer[id.x].direction * speed * deltaTime;
        ParticleBuffer[id.x].color = color;
    }
}

#endif