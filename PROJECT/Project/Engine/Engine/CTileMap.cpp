#include "pch.h"
#include "CTileMap.h"

#include "CTransform.h"

#include "CResMgr.h"

CTileMap::CTileMap()
	:
	CRenderComponent(COMPONENT_TYPE::TILEMAP)
  , m_rowCount(0)
  , m_colCount(0)
  , m_iTileCountX(0)
  , m_iTileCountY(0)
  , m_bBufferUpdated(false)
{
	// �޽�, ����
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\TileMapMtrl.mtrl"));

	m_vecTileData.resize(m_iTileCountX * m_iTileCountY);
	m_pBuffer = new CStructuredBuffer;
}

CTileMap::CTileMap(const CTileMap& _origin)
	:
	CRenderComponent(_origin)
  , m_pAtlasTex{_origin.m_pAtlasTex}
  , m_vSlicePixel(_origin.m_vSlicePixel)
  , m_vSliceUV(_origin.m_vSliceUV)
  , m_rowCount(_origin.m_rowCount)
  , m_colCount(_origin.m_colCount)
  , m_iTileCountX(_origin.m_iTileCountX)
  , m_iTileCountY(_origin.m_iTileCountY)
  , m_vecTileData(_origin.m_vecTileData)
  , m_pBuffer(nullptr)
  , m_bBufferUpdated(false)
{
	// �޽�, ����
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\TileMapMtrl.mtrl"));

	m_pBuffer = new CStructuredBuffer{};
	m_pBuffer->Create(sizeof(tTileData), m_iTileCountX * m_iTileCountY, SB_TYPE::READ_ONLY, true, nullptr);
}

CTileMap::~CTileMap()
{
	SAFE_DELETE(m_pBuffer);
}

void CTileMap::finalupdate() { }

void CTileMap::UpdateData()
{
	if (nullptr == m_pAtlasTex)
		return;

	GetMaterial()->SetTexParam(TEX_PARAM::TEX_0, m_pAtlasTex);

	GetMaterial()->SetScalarParam(SCALAR_PARAM::INT_0, &m_iTileCountX);
	GetMaterial()->SetScalarParam(SCALAR_PARAM::INT_1, &m_iTileCountY);
	GetMaterial()->SetScalarParam(SCALAR_PARAM::VEC2_0, &m_vSliceUV);
	
	// ��� Ÿ�� ������(m_vecTileData) �� ����ȭ���۸� ���� t16 �������ͷ� ���ε�
	if (false == m_bBufferUpdated)
	{
		m_pBuffer->SetData(m_vecTileData.data(), m_iTileCountX * m_iTileCountY);
		m_bBufferUpdated = true;
	}

	m_pBuffer->UpdateData(PS, 16);
}


void CTileMap::render()
{
	if (nullptr == GetMesh() || nullptr == GetMaterial() || nullptr == m_pAtlasTex)
		return;

	UpdateData();

	Transform()->UpdateData();
	GetMaterial()->UpdateData();
	GetMesh()->render();
}


void CTileMap::SetTileMapCount(UINT _iCountX, UINT _iCountY)
{
	m_iTileCountX = _iCountX;
	m_iTileCountY = _iCountY;

	ClearTileData();
}

void CTileMap::SetTileData(int tileIndex, int imageIndex)
{
	if (nullptr == m_pAtlasTex) { return; }

	m_vecTileData[tileIndex].iImgIdx = imageIndex;
	// ��Ʋ�󽺿��� Ÿ���� ��, �� ���� ���ϱ�
	m_colCount = static_cast<UINT>(m_pAtlasTex->Width()) / static_cast<UINT>(m_vSlicePixel.x);
	m_rowCount = static_cast<UINT>(m_pAtlasTex->Height()) / static_cast<UINT>(m_vSlicePixel.y);

	const int row = m_vecTileData[tileIndex].iImgIdx / m_colCount;
	const int col = m_vecTileData[tileIndex].iImgIdx % m_colCount;

	m_vecTileData[tileIndex].vLTUV = Vec2(m_vSliceUV.x * col, m_vSliceUV.y * row);

	m_bBufferUpdated = false;
}


void CTileMap::ClearTileData()
{
	m_vecTileData.clear();
	m_vecTileData.shrink_to_fit();
	m_vecTileData.resize(m_iTileCountX * m_iTileCountY);

	// ����ȭ ���۵� ũ�⿡ �����Ѵ�.
	const size_t bufferSize = (m_iTileCountX * m_iTileCountY) * sizeof(tTileData);

	if (m_pBuffer->GetBufferSize() < bufferSize)
	{
		m_pBuffer->Create(sizeof(tTileData), m_iTileCountX * m_iTileCountY, SB_TYPE::READ_ONLY, true, nullptr);
	}

	m_bBufferUpdated = false;
}


void CTileMap::SaveToScene(FILE* _pFile)
{
	CRenderComponent::SaveToScene(_pFile);

	SaveResPtr(m_pAtlasTex, _pFile);

	fwrite(&m_vSlicePixel, sizeof(Vec2), 1, _pFile);
	fwrite(&m_vSliceUV, sizeof(Vec2), 1, _pFile);
	fwrite(&m_rowCount, sizeof(UINT), 1, _pFile);
	fwrite(&m_colCount, sizeof(UINT), 1, _pFile);
	fwrite(&m_iTileCountX, sizeof(UINT), 1, _pFile);
	fwrite(&m_iTileCountY, sizeof(UINT), 1, _pFile);
	fwrite(m_vecTileData.data(), sizeof(tTileData), m_vecTileData.size(), _pFile);
}

void CTileMap::LoadFromScene(FILE* _pFile)
{
	CRenderComponent::LoadFromScene(_pFile);

	LoadResPtr(m_pAtlasTex, _pFile);

	fread(&m_vSlicePixel, sizeof(Vec2), 1, _pFile);
	fread(&m_vSliceUV, sizeof(Vec2), 1, _pFile);
	fread(&m_rowCount, sizeof(UINT), 1, _pFile);
	fread(&m_colCount, sizeof(UINT), 1, _pFile);
	fread(&m_iTileCountX, sizeof(UINT), 1, _pFile);
	fread(&m_iTileCountY, sizeof(UINT), 1, _pFile);

	ClearTileData();
	fread(m_vecTileData.data(), sizeof(tTileData), m_iTileCountX * m_iTileCountY, _pFile);
}
