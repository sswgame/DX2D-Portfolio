#pragma once
class CConstBuffer;

class CDevice
	: public CSingleton<CDevice>
{
	SINGLE(CDevice);
private:
	HWND m_hWnd; // Main Window Handle
	Vec2 m_vRenderResolution; // 렌더링 해상도

	ComPtr<ID3D11Device>        m_pDevice;			// GPU 메모리 제어
	ComPtr<ID3D11DeviceContext> m_pDeviceContext;	// GPU Rendering 제어

	ComPtr<IDXGISwapChain> m_pSwapChain;

	D3D11_VIEWPORT       m_tViewPort;
	DXGI_SWAP_CHAIN_DESC m_tSwapChainDesc;

	ComPtr<ID3D11RasterizerState>   m_arrRS[static_cast<UINT>(RS_TYPE::END)];
	ComPtr<ID3D11DepthStencilState> m_arrDS[static_cast<UINT>(DS_TYPE::END)];
	ComPtr<ID3D11BlendState>        m_arrBS[static_cast<UINT>(DS_TYPE::END)];


	CConstBuffer* m_arrCB[static_cast<UINT>(CB_TYPE::END)];


	ComPtr<ID3D11SamplerState> m_arrSam[2];
	Vec4 m_clearColor;

public:
	int init(HWND _hWnd, Vec2 _vRenderResolution);

	void SetRenderTarget();
	void Present() const { m_pSwapChain->Present(1, 0); }
	void ClearTarget();

	Vec2 GetRenderResolution() { return m_vRenderResolution; }

	ComPtr<ID3D11Device>        GetDevice() { return m_pDevice; }
	ComPtr<ID3D11DeviceContext> GetDeviceContext() { return m_pDeviceContext; }

	ComPtr<ID3D11RasterizerState>   GetRS(RS_TYPE _eType) { return m_arrRS[static_cast<UINT>(_eType)]; }
	ComPtr<ID3D11DepthStencilState> GetDS(DS_TYPE _eType) { return m_arrDS[static_cast<UINT>(_eType)]; }
	ComPtr<ID3D11BlendState>        GetBS(BS_TYPE _eType) { return m_arrBS[static_cast<UINT>(_eType)]; }
	CConstBuffer*                   GetCB(CB_TYPE _eType) const { return m_arrCB[static_cast<UINT>(_eType)]; }
	ComPtr<ID3D11SamplerState>      GetSampler(SAMPLER_TYPE type) { return m_arrSam[static_cast<UINT>(type)]; }

	void SetClearTargetColor(const Vec4& color){m_clearColor=color;}
private:
	int  CreateSwapchain();
	int  CreateView();
	int  CreateRasterizerState();
	int  CreateDepthStencilState();
	int  CreateBlendState();
	int  CreateConstBuffer();
	void CreateSamplerState();
};
