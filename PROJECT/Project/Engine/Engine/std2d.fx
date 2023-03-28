#ifndef _STD2D
#define _STD2D

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
// Std2D Shader
// g_tex_0 : Output Texture
// Rasterizer : CULL_NONE
// BlendState : Default
// DepthStencilState : LESS
// DOMAIN : Masked
// =========================

//���� ������ �͵�(Start)
#define LeftTop         g_vec2_0
#define Scale           g_vec2_1
#define BackgroundSize  g_vec2_2
#define Offset          g_vec2_2
#define HasMetaInfo     g_int_0
#define Alpha           g_float_0
#define PointSampler    g_sam_1
#define OutputTexture   g_tex_0
//���� ������ �͵� (End)
VTX_OUT VS_Std2D(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;    
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Std2D(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    // Animation ������ �ִ� ���
    if (g_useAnim2D)
    {
        float2 vUV = _in.vUV * g_vBackgroundSize;
        vUV = vUV - (g_vBackgroundSize - g_vSlice) / 2.f + g_vLT - g_vOffset;        
        if (vUV.x < g_vLT.x || g_vLT.x + g_vSlice.x < vUV.x
            || vUV.y < g_vLT.y || g_vLT.y + g_vSlice.y < vUV.y)
        {            
            discard;
        }
                
        vOutColor = g_Atlas.Sample(g_sam_1, vUV);     
    }
    //�ִϸ��̼��� ���� ��� ��Ʋ�󽺿��� �ش� ��ǥ���� �Ѱ��� ���� �ƴ� ���� ������ �� �����ؾ��Ѵ�.
    else
    {        
        if(g_btex_0)
        {
            //��Ÿ������ �ִ� ��� �װ��� ���� ����
            if(true == HasMetaInfo)
            {
                float2 UV = _in.vUV * BackgroundSize;
                UV = UV - (BackgroundSize - Scale) / 2.f + LeftTop;
                if (UV.x < LeftTop.x || LeftTop.x + Scale.x < UV.x  || UV.y < LeftTop.y || LeftTop.y + Scale.y < UV.y)
                {
                    discard;
                }
                vOutColor = OutputTexture.Sample(PointSampler, UV);
            }
            //���� ��� �ش� �̹����� �ϳ��� �ؽ�ó�� ���� ����
            else
            {
                vOutColor = OutputTexture.Sample(PointSampler, _in.vUV);
            }
            
        }
        //�ؽ�ó�� ���ٸ� ����Ÿ �������� ���
        else
        {
            vOutColor = float4(1.f, 0.f, 1.f, 1.f);
        }        
    }
    
    if (vOutColor.a <= 0.f)
    {
        discard;
    }        
 
    return vOutColor;
}

// ========================
// Std2DAlphaBlend
// BlendState           : Alpha Blend
// DepthStencilState    : No_Write
// DOMAIN               : OPAQUE
//=========================
VTX_OUT VS_Std2DAlpha(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Std2DAlpha(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
       
    return vOutColor;
}



// ================
// 
// ================











// ==================
// Collider2D Shader
// g_int_0 : Collision
// ==================
VTX_OUT VS_Collider2D(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}


float4 PS_Collider2D(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
       
    if(g_int_0)
    {
        vOutColor = float4(1.f, 0.f, 0.f, 1.f);
    }
    else
    {
        vOutColor = float4(0.f, 1.f, 0.f, 1.f);
    }
        
    return vOutColor;
}







#endif
