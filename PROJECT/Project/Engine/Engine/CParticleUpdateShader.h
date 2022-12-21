#pragma once
#include "CComputeShader.h"

class CParticleUpdateShader : public CComputeShader
{
private:
	CStructuredBuffer* m_pParticleBuffer;
	CStructuredBuffer* m_pDataBuffer;

public:
	void SetParticleBuffer(CStructuredBuffer* pBuffer) { m_pParticleBuffer = pBuffer; }
	void SetParticleDataBuffer(CStructuredBuffer* pBuffer) { m_pDataBuffer = pBuffer; }

	void SetObjectWorldPos(Vec3 worldPos)
	{
		m_Param.fArr[1] = worldPos.x;
		m_Param.fArr[2] = worldPos.y;
		m_Param.fArr[3] = worldPos.z;
	}

	void SetParticleCreateDistance(float distance) { m_Param.fArr[0] = distance; }
	void SetParticleMinMaxTime(float min, float max) { m_Param.v2Arr[0] = Vec2(min, max); }
	void SetStartEndSpeed(float min, float max) { m_Param.v2Arr[1] = Vec2(min, max); }
	void SetDirection(Vec2 direction) { m_Param.v2Arr[2] = direction; }
	void SetStartEndColor(Vec4 startColor, Vec4 endColor)
	{
		m_Param.v4Arr[0] = startColor;
		m_Param.v4Arr[1] = endColor;
	}

	void SetStartEndScale(Vec3 startColor, Vec3 endColor)
	{
		m_Param.v4Arr[2] = startColor;
		m_Param.v4Arr[3] = endColor;
	}


public:
	void UpdateData() override;
	void Clear() override;

public:
	CParticleUpdateShader();
	virtual ~CParticleUpdateShader();
};
