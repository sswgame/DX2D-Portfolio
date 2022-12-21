#ifndef _STRUCT
#define _STRUCT

struct tTileData
{
    int iImgIdx;
    float2 vLTUV;
    int iPadding;
};


struct tParticle
{
    float3  pos;
    float3  scale;
    float3  direction;
    float4  color;

    int     isAlive;
    float   elapsedTime;
    float   maxTime;
};

struct tAnimationParticle
{
    float2 leftTopUV;
    float2 sliceUV;
    float2 backgroundUV;
    float2 offsetUV;
    float duration;
    int currentFrameIndex;
    int isRepeat;
    int maxFrameCount;
};

struct tParticleData
{
    int     aliveCount;
    float3  vPadding;
};


struct tLightColor
{
    float4 vDiff;
    float4 vSpec;
    float4 vAmb;
};

struct tLightInfo
{
    tLightColor color;
    float3 vLightDir;
    int iLightType;
    float3 vWorldPos;
    float fAngle;
    float fRange;
    float3 vPadding;
};

struct tNumber
{
    uint number;
};

#endif
