#pragma once
#include "CComponent.h"

#include "CMesh.h"
#include "CMaterial.h"


enum class COLLIDER2D_TYPE
{
	BOX,
	CIRCLE,
};

class CCollider2D final
	: public CComponent
{
private:
	Matrix          m_matColWorld;
	COLLIDER2D_TYPE m_colliderType;

	Vec2 m_offsetPos;					// 오브젝트로 부터의 충돌체의 상대 거리
	Vec2 m_offsetScale;					// 오브젝트 크기로 부터 충돌체의 배율

	Ptr<CMesh>     m_pMesh;				// 충돌체 모양
	Ptr<CMaterial> m_pMaterial;			// 충돌체 랜더링 재질

	int m_collisionCount;				// 충돌 횟수


public:
	void SetCollider2DType(COLLIDER2D_TYPE _type);

	void SetOffsetPos(Vec2 offset) { m_offsetPos = offset; }
	void SetOffsetPos(float x, float y) { m_offsetPos = Vec2{x, y}; }
	Vec2 GetOffsetPos() const { return m_offsetPos; }

	void SetOffsetScale(Vec2 offsetScale);
	void SetOffsetScale(float x, float y);
	Vec2 GetOffsetScale() const { return m_offsetScale; }

	COLLIDER2D_TYPE GetCollider2DType() const { return m_colliderType; }
	Vec3            GetWorldPos() const { return m_matColWorld.Translation(); }
	Vec3            GetWorldScale() const { return Vec3{m_offsetScale}; }

	Matrix GetWorldMat() const { return m_matColWorld; }

public:
	void finalupdate() override;
	void UpdateData() override;
	void render();

public:
	// 이전 프레임에는 충돌 X, 이번 프레임에 충돌 시작일 때
	void OnCollisionEnter(CCollider2D* pOther);
	// 이전 프레임에도 충돌 중, 지금도 충돌 중일 때
	void OnCollision(CCollider2D* pOther);
	// 이전 프레임에 충돌 중, 지금은 충돌 X 일때
	void OnCollisionExit(CCollider2D* pOther);

public:
	void SaveToScene(FILE* pFile) override;
	void LoadFromScene(FILE* pFile) override;
	CLONE(CCollider2D);
public:
	CCollider2D();
	CCollider2D(const CCollider2D& origin);
	virtual ~CCollider2D();
};
