#pragma once
#include "CComponent.h"

class CTransform :
	public CComponent
{
private:
	Vec3 m_vRelativePos;
	Vec3 m_vRelativeScale;
	Vec3 m_vRelativeRot;     // 축별 회전량

	Vec3 m_arrRelativeDir[static_cast<UINT>(DIR_TYPE::END)]; // 상대 방향
	Vec3 m_arrWorldDir[static_cast<UINT>(DIR_TYPE::END)];    // 최종 월드상에서의 방향

	Matrix m_matWorld; // 위치변환 정보 행렬

	bool m_bIgnoreParentScale;

	Vec3 m_prevRelativePos;
	Vec3 m_prevWorldPos;
public:
	void SetRelativePos(const Vec3& _vPos)
	{
		m_prevRelativePos = m_vRelativePos;
		m_vRelativePos    = _vPos;
	}

	void SetRelativePos(float _x, float _y, float _z)
	{
		m_prevRelativePos = m_vRelativePos;
		m_vRelativePos    = Vec3(_x, _y, _z);
	}

	void SetRelativeScale(const Vec3& _vScale) { m_vRelativeScale = _vScale; }
	void SetRelativeScale(float _x, float _y, float _z) { m_vRelativeScale = Vec3(_x, _y, _z); }

	void SetRelativeRotation(const Vec3 _vRot) { m_vRelativeRot = _vRot; }
	void SetRelativeRotation(float _x, float _y, float _z) { m_vRelativeRot = Vec3(_x, _y, _z); }


	const Vec3& GetPrevRelativePos() const { return m_prevRelativePos; }
	const Vec3& GetRelativePos() const { return m_vRelativePos; }
	const Vec3& GetRelativeScale() const { return m_vRelativeScale; }
	const Vec3& GetRelativeRotation() const { return m_vRelativeRot; };

	const Vec3& GetRelativeDir(DIR_TYPE _eType) const { return m_arrRelativeDir[static_cast<UINT>(_eType)]; }
	const Vec3& GetRelativeRightDir() const { return m_arrRelativeDir[static_cast<UINT>(DIR_TYPE::RIGHT)]; }
	const Vec3& GetRelativeUpDir() const { return m_arrRelativeDir[static_cast<UINT>(DIR_TYPE::UP)]; }
	const Vec3& GetRelativeFrontDir() const { return m_arrRelativeDir[static_cast<UINT>(DIR_TYPE::FRONT)]; }

	const Vec3& GetPrevWorldPos() const { return m_prevWorldPos; }
	const Vec3& GetWorldDir(DIR_TYPE _eType) const { return m_arrWorldDir[static_cast<UINT>(_eType)]; }
	const Vec3& GetWorldRightDir() const { return m_arrWorldDir[static_cast<UINT>(DIR_TYPE::RIGHT)]; }
	const Vec3& GetWorldUpDir() const { return m_arrWorldDir[static_cast<UINT>(DIR_TYPE::UP)]; }
	const Vec3& GetWorldFrontDir() const { return m_arrWorldDir[static_cast<UINT>(DIR_TYPE::FRONT)]; }

	Vec3 GetWorldPos() const { return m_matWorld.Translation(); }
	Vec3 GetWorldScale();

	const Matrix& GetWorldMat() const { return m_matWorld; }

	void SetIgnoreParentScale(bool _bSet) { m_bIgnoreParentScale = _bSet; }
	bool IsIgnoreParentScale() const { return m_bIgnoreParentScale; }
	void UpdateData() override;
	void finalupdate() override;

	void active() override;
	void deactive() override;

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

	CLONE(CTransform)
public:
	CTransform();
	virtual ~CTransform();
};
