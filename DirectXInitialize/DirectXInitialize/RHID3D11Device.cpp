#include "stdafx.h"
#include "RHID3D11Device.h"

RHID3D11Device::RHID3D11Device()
{

}

RHID3D11Device::~RHID3D11Device()
{
	
}

bool RHID3D11Device::Initialize()
{
	bool Result = false;
	Result = CreateDevice();

	if (Result)
	{
		Result = CreateSwapChain();
	}

	if (Result)
	{
		Result = CreateRenderTargetView();
	}

	if (Result)
	{
		Result = CreateDepthStencilView();
	}

	if (Result)
	{
		SetRenderTargets();
		SetViewport();
	}

	return Result;
}

bool RHID3D11Device::Release()
{
	// �翬�� ������ �͵��� Ǯ������Ѵ�.
	SAFE_RELEASE(DepthStencilBuffer);
	SAFE_RELEASE(DepthStencilView);
	SAFE_RELEASE(RenderTargetView);
	SAFE_RELEASE(SwapChain);
	SAFE_RELEASE(D3D11Device);
	SAFE_RELEASE(D3D11DeviceContext);
	return true;
}

bool RHID3D11Device::CreateDevice()
{
	// ID3D11Deivce�� ��� ���� ���� �� �ڿ� �Ҵ翡 ���δ�.
	// ID3D11DeviceContext �������̽��� ���� ����� �����ϰ�, �ڿ����� ����, ������ ����� �����ϴµ� ���δ�.
	

	//HRESULT WINAPI D3D11CreateDevice(
	//	__in_opt IDXGIAdapter* pAdapter, // �ƴ���, �� �ϵ���� ���� �������ٶ� ���δ�. 0�� �������ָ� �⺻ ����Ͱ� �����ȴ�.
	//	D3D_DRIVER_TYPE DriverType, // ����̹� Ÿ���� �����Ѵ�. Reference�� ǥ�� ��ġ��, ��Ȯ���� ��ǥ�� �Ҷ� ����ϴ� �ɷ�, �츮�� �ϵ���� Ÿ���� ����Ѵ�.
	//	HMODULE Software, // ����Ʈ���� �����⸦ �����Ѵ�. �츮�� �ϵ��� ����ϹǷ� ���� �ִ´�.
	//	UINT Flags, // ������ �߰��� �ɼǵ��� �����ϴ� �÷��� ����
	//	__in_ecount_opt(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels, // ����� Dx�� ���� ������ �����Ҷ� ���δ�. NULL �϶��� ������ �ְ�
	//	UINT FeatureLevels, //���� ���� ���� ���� �迭�� ���� ���� ������ �ȴ�. NULL�� �־��� ���, 0
	//	UINT SDKVersion, // D3D11 SDK ������ �����Ѵ�.
	//	__out_opt ID3D11Device** ppDevice, // �Լ��� ������ ����̽��� �����͸� �����ش�.
	//	__out_opt D3D_FEATURE_LEVEL* pFeatureLevel, // �Լ��� ������ ����̽��� ���ķ����� �����ش�.
	//	__out_opt ID3D11DeviceContext** ppImmediateContext); // ������ ��ġ ������ �����ش�.
	
	//typedef
	//enum D3D_DRIVER_TYPE
	//{
	//	D3D_DRIVER_TYPE_UNKNOWN = 0,
	//	D3D_DRIVER_TYPE_HARDWARE = (D3D_DRIVER_TYPE_UNKNOWN + 1),
	//	D3D_DRIVER_TYPE_REFERENCE = (D3D_DRIVER_TYPE_HARDWARE + 1), // �ϵ��� �������� �ʴ� ����� �׽�Ʈ�ϰų�, �������� ���׸� �����Ҷ����δ�.
	//	D3D_DRIVER_TYPE_NULL = (D3D_DRIVER_TYPE_REFERENCE + 1),
	//	D3D_DRIVER_TYPE_SOFTWARE = (D3D_DRIVER_TYPE_NULL + 1), // ����Ʈ���� �������� �����ߴٸ� ����Ѵ�.
	//	D3D_DRIVER_TYPE_WARP = (D3D_DRIVER_TYPE_SOFTWARE + 1) // dx 10.1 ���� ����ϴ� WARP�����⿡���� ���δ�.
	//} 	D3D_DRIVER_TYPE;

	//typedef
	//enum D3D11_CREATE_DEVICE_FLAG
	//{
	//	D3D11_CREATE_DEVICE_SINGLETHREADED = 0x1, // D3D�� �������� �������� �����忡�� ȣ����� �ʴ´ٴ� ������ ������ ���� ����� �����ϴ�.
													// DeferredContext���� ȣ���� �����ϰ� �ȴ�.
	//	D3D11_CREATE_DEVICE_DEBUG = 0x2, // ����� ��� ���忡�� ����� ������ Ȱ��ȭ�Ͽ� ����� �޽����� Ȯ�� �Ҽ� �ִ�.
	//	D3D11_CREATE_DEVICE_SWITCH_TO_REF = 0x4, // ���۷��� ����̽��� ����ġ�Ѵ�.
	//	D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS = 0x8,
	//	D3D11_CREATE_DEVICE_BGRA_SUPPORT = 0x20
	//} 	D3D11_CREATE_DEVICE_FLAG;

	D3D_FEATURE_LEVEL featureLevel;
	UINT createDeviceFlags = 0;

#if defined(_DEBUG) || defined(DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT Result = D3D11CreateDevice(NULL,
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createDeviceFlags,
		0, 0,
		D3D11_SDK_VERSION,
		&D3D11Device,
		&featureLevel,
		&D3D11DeviceContext
		);

	if (FAILED(Result))
	{
		MessageBox(0, L"����̽� ���� ����",0,0);
		assert(SUCCEEDED(Result));
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"���̷�Ʈx 11.0�� �������� �ʽ��ϴ�.", 0, 0);
		assert(SUCCEEDED(Result));
		return false;
	}

	return true;
}

bool RHID3D11Device::CreateSwapChain()
{
	//typedef
	//enum DXGI_SWAP_CHAIN_FLAG
	//{
	//	DXGI_SWAP_CHAIN_FLAG_NONPREROTATED = 1,
	//	DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH = 2,
	//	DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE = 4
	//} 	DXGI_SWAP_CHAIN_FLAG;

	//typedef struct DXGI_SWAP_CHAIN_DESC
	//{
	//	DXGI_MODE_DESC BufferDesc; �ĸ� ���۵��� �Ӽ��� ���ϴ� ����ü
	//	DXGI_SAMPLE_DESC SampleDesc; ����ǥ��ȭ(��Ƽ���ø�)�� ���� ������ ǥ�� ������ ǰ�� ����
	//	DXGI_USAGE BufferUsage; ������ �뵵, �翬�� �ĸ���ۿ� ������ �ϹǷ�, DXGI_REDNERTARGET_OUTPUT ����
	//	UINT BufferCount; �ĸ� ������ ���� ���� (1���� 2���� ����Ҽ� ����)
	//	HWND OutputWindow; ������ ����� ǥ���� â�� �ڵ�
	//	BOOL Windowed; â��带 ���ϳ� �ȿ��ϳ�
	//	DXGI_SWAP_EFFECT SwapEffect; ��ȯ ȿ���� �����ϴ� ����ü DXGI_SWAP_EFFECT_DISCARD������ �˾Ƽ� ���� ȿ������ �� ó��
	//	UINT Flags; ����ü�� �÷���, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH������ ��üȭ�� ���� ��ȯ�� ���÷��̸�带 ���� ������ ������ ó��
	//} 	DXGI_SWAP_CHAIN_DESC;
	
	//typedef enum DXGI_MODE_SCANLINE_ORDER
	//{
	//	DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED = 0,
	//	DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE = 1,
	//	DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST = 2,
	//	DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST = 3
	//} DXGI_MODE_SCANLINE_ORDER;

	//typedef enum DXGI_MODE_SCALING
	//{
	//	DXGI_MODE_SCALING_UNSPECIFIED = 0,
	//	DXGI_MODE_SCALING_CENTERED = 1,
	//	DXGI_MODE_SCALING_STRETCHED = 2
	//} DXGI_MODE_SCALING;

	//typedef enum DXGI_MODE_ROTATION
	//{
	//	DXGI_MODE_ROTATION_UNSPECIFIED = 0,
	//	DXGI_MODE_ROTATION_IDENTITY = 1,
	//	DXGI_MODE_ROTATION_ROTATE90 = 2,
	//	DXGI_MODE_ROTATION_ROTATE180 = 3,
	//	DXGI_MODE_ROTATION_ROTATE270 = 4
	//} DXGI_MODE_ROTATION;

	//typedef struct DXGI_MODE_DESC
	//{
	//	UINT Width;  �ĸ� ���� ����
	//	UINT Height; �ĸ� ���� ���� 
	//	DXGI_RATIONAL RefreshRate; ���÷��� ������
	//	DXGI_FORMAT Format; �ĸ� ���� �ȼ� ����
	//	DXGI_MODE_SCANLINE_ORDER ScanlineOrdering; ���÷��� ��ĵ���� ���
	//	DXGI_MODE_SCALING Scaling; ���÷��� ��ʸ��
	//} DXGI_MODE_DESC;


	HRESULT Result = D3D11Device->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8X8_UNORM, GSettings.MSAASampleNum, &GSettings.MSAAQuality);
	if (Result == 0)
	{
		GSettings.bEnableMSAA = true;
	}
	else
	{
		GSettings.bEnableMSAA = false;
	}

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = GSettings.Width;
	sd.BufferDesc.Height = GSettings.Height;
	// Rate
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
	//SwapChainDesc.BufferDesc.Width = GSettings.Width;
	//SwapChainDesc.BufferDesc.Height = GSettings.Height;
	//SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	//SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	//SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8X8_UNORM;
	//SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	
	if (GSettings.bEnableMSAA)
	{
		sd.SampleDesc.Count = GSettings.MSAASampleNum;
		sd.SampleDesc.Quality = GSettings.MSAAQuality - 1;
	}
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = GSettings.WindowHandle;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// ��ȯ �罽�� ����
	// IDXGIFactory �ν��Ͻ��� ���� CreateSwapChain�޼��带 ȣ���ؼ� ����
	// �Ϲ������� CreateDXGI�� ���� ������ ��� ���� �ٸ� �ν��Ͻ��� ����Ϸ� ��� ������ ������ �߻��ϹǷ� ������ ���� ����̽��� ã������Ѵ�.
	// DXGI( DirectX Graphic Infrastructure - �׷��� �ϵ���� ����, â���� ��üȭ�� ��ȯ�� �׷��ȿ� ���õ� �۾��� ��. 
	//�и��� ������ D2D�� ����, �ٸ� ���̺귯���� ���µ� ������ ����� ����)
	IDXGIDevice* dxgiDevice = 0;
	HR(D3D11Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	// ����ü���� �����Ǿ���.
	HR(dxgiFactory->CreateSwapChain(D3D11Device, &sd, &SwapChain));

	SAFE_RELEASE(dxgiDevice);
	SAFE_RELEASE(dxgiAdapter);
	SAFE_RELEASE(dxgiFactory);
	return true;
}

bool RHID3D11Device::CreateRenderTargetView()
{
	// ��� �ڿ��� ���������ο� ���� ���� ���� �ƴϰ�, �ڿ��� ���� �並 ���������δܰ迡 ���� �� (����Ÿ�ٺ�, Ȥ�� ���̴� ���ҽ���)
	ID3D11Texture2D* backBuffer = nullptr;

	// ��ȯ�罽�� ����Ű�� �����͸� ��´�. 
	//virtual HRESULT STDMETHODCALLTYPE GetBuffer(
	//	/* [in] */ UINT Buffer, �̶� ù��° �Ű������� �ĸ������ ����
	//	/* [annotation][in] */ 
	//	__in  REFIID riid, ������ �������̽� ����(�ؽ��� 2D ����)
	//	/* [annotation][out][in] */ 
	//	__out  void **ppSurface) = 0; �������� �ĸ���۸� ����Ű�� �����͸� ������
	HR(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));

	//virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView(
	//	/* [annotation] */
	//	__in  ID3D11Resource *pResource, ���ҽ� ���(�ĸ����)
	//	/* [annotation] */
	//	__in_opt  const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,  �ڿ��� ��� ���ҵ��� �ڷ����� ����, �̹� �����ؼ� �ڿ��� �����ߴ��� null�� �������൵�ȴ�.
	//	/* [annotation] */
	//	__out_opt  ID3D11RenderTargetView **ppRTView) = 0; // ������ ���� ��� �並 �����ش�.
	HR(D3D11Device->CreateRenderTargetView(backBuffer, 0, &RenderTargetView));

	SAFE_RELEASE(backBuffer); // �������ִ��� ������ �ٹ��۸� ���ؼ� ���۸� ������� ����Ƚ���� �����ϱ� ����

	return true;
}

bool RHID3D11Device::CreateDepthStencilView()
{
	//typedef
	//enum D3D11_USAGE
	//{
	//	D3D11_USAGE_DEFAULT = 0,�ڿ��� GPU�� �а� ����. ���� ���ٽ��� GPU�� ���⶧���� �̰� ����
	//	D3D11_USAGE_IMMUTABLE = 1, �ڿ��� ������ �� ������ �ٲ��� �ʴ� ���
	//	D3D11_USAGE_DYNAMIC = 2, CPU�� �ڿ��� ����ϰ� �����Ҷ�
	//	D3D11_USAGE_STAGING = 3 �������α׷��� �ڿ��� ���纻�� �ϰž� �Ѷ� ���
	//} 	D3D11_USAGE;

	//typedef
	//enum D3D11_BIND_FLAG
	//{
	//	D3D11_BIND_VERTEX_BUFFER = 0x1L,
	//	D3D11_BIND_INDEX_BUFFER = 0x2L,
	//	D3D11_BIND_CONSTANT_BUFFER = 0x4L,
	//	D3D11_BIND_SHADER_RESOURCE = 0x8L, ���̴� �ڿ����� ���ε�
	//	D3D11_BIND_STREAM_OUTPUT = 0x10L,
	//	D3D11_BIND_RENDER_TARGET = 0x20L, ����Ÿ�ٿ� ���ε�
	//	D3D11_BIND_DEPTH_STENCIL = 0x40L, 
	//	D3D11_BIND_UNORDERED_ACCESS = 0x80L
	//} 	D3D11_BIND_FLAG;

	//typedef
	//enum D3D11_CPU_ACCESS_FLAG
	//{
	//	D3D11_CPU_ACCESS_WRITE = 0x10000L, CPU ���ٿ� ���õ� �÷��� Dynamic �϶��� ����ȴ�.
	//	D3D11_CPU_ACCESS_READ = 0x20000L
	//} 	D3D11_CPU_ACCESS_FLAG;

	//typedef struct D3D11_TEXTURE2D_DESC
	//{
	//	UINT Width; �ؽ����� �ʺ�(�ؼ� ����)
	//	UINT Height; �ؽ����� ���� (�ؼ� ����)
	//	UINT MipLevels; �Ӹʼ����� ����, �������ٽǿ����� �ϳ�
	//	UINT ArraySize; �ؽ��� �迭�� �ؽ�ó ����, �������ٽǿ����� �ϳ�
	//	DXGI_FORMAT Format; �ؼ��� ����
	//	DXGI_SAMPLE_DESC SampleDesc; ���� ǥ�� ������ ǰ��, ���� Ÿ�ٿ� ���� ������ ��ġ�ؾ���
	//	D3D11_USAGE Usage; // �ؽ����� �뵵 �� ����
	//	UINT BindFlags;  // �� ����
	//	UINT CPUAccessFlags; // �� ����, ���⼭�� 0�� ����
	//	UINT MiscFlags; // ��Ÿ �÷���
	//} 	D3D11_TEXTURE2D_DESC;
	
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = GSettings.Width;
	depthStencilDesc.Height = GSettings.Height;
	depthStencilDesc.MipLevels = 1; 
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	if (GSettings.bEnableMSAA)
	{
		depthStencilDesc.SampleDesc.Count = GSettings.MSAASampleNum;
		depthStencilDesc.SampleDesc.Quality = GSettings.MSAAQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(D3D11Device->CreateTexture2D(&depthStencilDesc, 0, &DepthStencilBuffer));
	HR(D3D11Device->CreateDepthStencilView(DepthStencilBuffer, 0, &DepthStencilView));

	return true;
}

void RHID3D11Device::SetRenderTargets()
{
	//virtual void STDMETHODCALLTYPE OMSetRenderTargets(
	//	/* [annotation] */
	//	__in_range(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)  UINT NumViews, // ���� Ÿ���� �ټ�, ����� �������� ������ �Ұ��, �������� ���� ����
	//	/* [annotation] */
	//	__in_ecount_opt(NumViews)  ID3D11RenderTargetView *const *ppRenderTargetViews, // ����Ÿ�ٺ��
	//	/* [annotation] */
	//	__in_opt  ID3D11DepthStencilView *pDepthStencilView) = 0; // �������Ľ��� �Ѱ����̴�.
	// ������������ ��º��ձ�ܰ迡 ���ε�
	D3D11DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);
}


void RHID3D11Device::SetViewport()
{
	// ����Ʈ ���� (�׷��ְ��� �ϴ� ����)
	//typedef struct D3D11_VIEWPORT
	//{
	//	FLOAT TopLeftX;
	//	FLOAT TopLeftY;
	//	FLOAT Width;
	//	FLOAT Height;
	//	FLOAT MinDepth;
	//	FLOAT MaxDepth;
	//} 	D3D11_VIEWPORT;

	D3D11_VIEWPORT Vp;
	Vp.TopLeftX = 0;
	Vp.TopLeftY = 0;
	Vp.Width = static_cast<float>(GSettings.Width);
	Vp.Height = static_cast<float>(GSettings.Height);
	Vp.MinDepth = 0;
	Vp.MaxDepth = 1;


	//virtual void STDMETHODCALLTYPE RSSetViewports(
	//	/* [annotation] */
	//	__in_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumViewports,
	//	/* [annotation] */
	//	__in_ecount_opt(NumViewports)  const D3D11_VIEWPORT *pViewports) = 0;
	D3D11DeviceContext->RSSetViewports(1, &Vp);
}