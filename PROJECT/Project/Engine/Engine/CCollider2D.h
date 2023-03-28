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

	Vec2 m_offsetPos;					// ������Ʈ�� ������ �浹ü�� ��� �Ÿ�
	Vec2 m_offsetScale;					// ������Ʈ ũ��� ���� �浹ü�� ����

	Ptr<CMesh>     m_pMesh;				// �浹ü ���
	Ptr<CMaterial> m_pMaterial;			// �浹ü ������ ����

	int m_collisionCount;				// �浹 Ƚ��


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
	// ���� �����ӿ��� �浹 X, �̹� �����ӿ� �浹 ������ ��
	void OnCollisionEnter(CCollider2D* pOther);
	// ���� �����ӿ��� �浹 ��, ���ݵ� �浹 ���� ��
	void OnCollision(CCollider2D* pOther);
	// ���� �����ӿ� �浹 ��, ������ �浹 X �϶�
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
