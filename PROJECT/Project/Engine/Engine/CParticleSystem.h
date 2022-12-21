#pragma once
#include "CRenderComponent.h"

#include "CParticleUpdateShader.h"
#include "CStructuredBuffer.h"

#undef max
#undef min


class CParticleSystem : public CRenderComponent
{
private:
	Ptr<CParticleUpdateShader> m_pCS;           // 파티클 업데이트 쉐이더

	CStructuredBuffer* m_pParticleBuffer;   // 파티클 구조화 버퍼
	CStructuredBuffer* m_pDataBuffer;       // 추가 데이터 버퍼(파티클 AliveCount)
	UINT               m_maxCount;        // 파티클 최대 개수

	int  m_isPosInherit;      // 오브젝트 좌표에 영향
	UINT m_aliveCount;      // 이번 프레임에 생성(활성화) 될 파티클 수

	float m_minLifeTime;     // 파티클의 최소 생명 시간
	float m_maxLifeTime;     // 파티클의 최대 생명 시간

	float m_startSpeed;      // 파티클의 최소 속력
	float m_endSpeed;        // 파티클의 최대 속력

	Vec4 m_startColor;      // 파티클 초기 색상
	Vec4 m_endColor;        // 파티클 최종 색상

	Vec3 m_startScale;      // 파티클 초기 크기
	Vec3 m_endScale;        // 파티클 최종 크기


	float m_creationRange;  // 파티클 생성 범위
	float m_creationTerm;      // 파티클 생성 간격
	float m_accTime;

	Vec2 m_direction;

public:
	void                       SetComputeShader(Ptr<CParticleUpdateShader> pCS) { m_pCS = pCS; }
	Ptr<CParticleUpdateShader> GetComputeShader() const { return m_pCS; }

	void                       SetMaxParticleCount(UINT count);
	UINT                       GetMaxParticleCount() const { return m_maxCount; }

	void SetPosInherit(bool enable);
	bool PosInherit() const { return m_isPosInherit; }

	void SetAliveCount(UINT count);
	UINT GetAliveCount() const { return m_aliveCount; }

	void SetMinMaxLifeTime(float min, float max);
	Vec2 GetMinMaxLifeTime() const { return Vec2{m_minLifeTime, m_maxLifeTime}; }

	void SetMinMaxSpeed(float min, float max);
	Vec2 GetMinMaxSpeed() const { return Vec2{m_startSpeed, m_endSpeed}; }

	void                  SetStartEndColor(Vec4 startColor, Vec4 endColor);
	std::pair<Vec4, Vec4> GetStartEndColor() const { return std::pair<Vec4, Vec4>{m_startColor, m_endColor}; }

	void                  SetStartEndScale(Vec3 startScale, Vec3 endScale);
	std::pair<Vec3, Vec3> GetStartEndScale() const { return std::pair<Vec3, Vec3>{m_startScale, m_endScale}; }

	void  SetRange(float range);
	float GetRange() const { return m_creationRange; }

	void  SetTerm(float term);
	float GetTerm() const { return m_creationTerm; }

	void SetDirection(Vec2 direction) { m_direction = direction; }
	Vec2 GetDirection() const { return m_direction; }
public:
	void finalupdate() override;
	void render() override;


public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;
	CLONE(CParticleSystem);
public:
	CParticleSystem();
	CParticleSystem(const CParticleSystem& _origin);
	virtual ~CParticleSystem();
};
