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
	// 당연히 생성한 것들은 풀어줘야한다.
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
	// ID3D11Deivce는 기능 지원 점검 및 자원 할당에 쓰인다.
	// ID3D11DeviceContext 인터페이스는 렌더 대상을 설정하고, 자원들을 묶고, 렌더링 명령을 지시하는데 쓰인다.
	

	//HRESULT WINAPI D3D11CreateDevice(
	//	__in_opt IDXGIAdapter* pAdapter, // 아답터, 곧 하드웨를 직접 연결해줄때 쓰인다. 0을 설정해주면 기본 어댑터가 설정된다.
	//	D3D_DRIVER_TYPE DriverType, // 드라이버 타입을 설정한다. Reference는 표준 장치로, 정확성을 목표로 할때 사용하는 걸로, 우리는 하드웨어 타입을 써야한다.
	//	HMODULE Software, // 소프트웨어 구동기를 지정한다. 우리는 하드웨어를 사용하므로 널을 넣는다.
	//	UINT Flags, // 생성시 추가될 옵션들을 설정하는 플래그 집단
	//	__in_ecount_opt(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels, // 사용할 Dx의 피쳐 레벨을 지정할때 쓰인다. NULL 일때는 무조건 최고값
	//	UINT FeatureLevels, //위의 값에 넣은 원소 배열의 갯수 값을 넣으면 된다. NULL을 넣었을 경우, 0
	//	UINT SDKVersion, // D3D11 SDK 버전을 지정한다.
	//	__out_opt ID3D11Device** ppDevice, // 함수가 생성한 디바이스의 포인터를 돌려준다.
	//	__out_opt D3D_FEATURE_LEVEL* pFeatureLevel, // 함수가 생성한 디바이스의 피쳐레벨을 돌려준다.
	//	__out_opt ID3D11DeviceContext** ppImmediateContext); // 생성된 장치 문맥을 돌려준다.
	
	//typedef
	//enum D3D_DRIVER_TYPE
	//{
	//	D3D_DRIVER_TYPE_UNKNOWN = 0,
	//	D3D_DRIVER_TYPE_HARDWARE = (D3D_DRIVER_TYPE_UNKNOWN + 1),
	//	D3D_DRIVER_TYPE_REFERENCE = (D3D_DRIVER_TYPE_HARDWARE + 1), // 하드웨어가 지원하지 않는 기능을 테스트하거나, 구동기의 버그를 시험할때쓰인다.
	//	D3D_DRIVER_TYPE_NULL = (D3D_DRIVER_TYPE_REFERENCE + 1),
	//	D3D_DRIVER_TYPE_SOFTWARE = (D3D_DRIVER_TYPE_NULL + 1), // 소프트웨어 렌더링을 구현했다면 사용한다.
	//	D3D_DRIVER_TYPE_WARP = (D3D_DRIVER_TYPE_SOFTWARE + 1) // dx 10.1 에서 사용하는 WARP구동기에서만 쓰인다.
	//} 	D3D_DRIVER_TYPE;

	//typedef
	//enum D3D11_CREATE_DEVICE_FLAG
	//{
	//	D3D11_CREATE_DEVICE_SINGLETHREADED = 0x1, // D3D의 렌더링이 여러개의 쓰레드에서 호출되지 않는다는 보장이 있을때 성능 향상이 가능하다.
													// DeferredContext관련 호출은 실패하게 된다.
	//	D3D11_CREATE_DEVICE_DEBUG = 0x2, // 디버그 모드 빌드에서 디버그 계층을 활성화하여 디버그 메시지를 확인 할수 있다.
	//	D3D11_CREATE_DEVICE_SWITCH_TO_REF = 0x4, // 리퍼런스 디바이스로 스위치한다.
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
		MessageBox(0, L"디바이스 생성 실패",0,0);
		assert(SUCCEEDED(Result));
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"다이렉트x 11.0을 지원하지 않습니다.", 0, 0);
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
	//	DXGI_MODE_DESC BufferDesc; 후면 버퍼들의 속성을 정하는 구조체
	//	DXGI_SAMPLE_DESC SampleDesc; 다중표본화(멀티샘플링)을 위해 추출할 표번 갯수와 품질 설정
	//	DXGI_USAGE BufferUsage; 버퍼의 용도, 당연히 후면버퍼에 렌더링 하므로, DXGI_REDNERTARGET_OUTPUT 설정
	//	UINT BufferCount; 후면 버퍼의 갯수 설정 (1개나 2개를 상요할수 있음)
	//	HWND OutputWindow; 렌더링 결과를 표시할 창의 핸들
	//	BOOL Windowed; 창모드를 원하나 안원하나
	//	DXGI_SWAP_EFFECT SwapEffect; 교환 효과를 설정하는 구조체 DXGI_SWAP_EFFECT_DISCARD지정시 알아서 가장 효율적인 것 처리
	//	UINT Flags; 스왑체인 플래그, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH설정시 전체화면 모드로 전환시 디스플레이모드를 제일 적합한 것으로 처리
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
	//	UINT Width;  후면 버퍼 넓이
	//	UINT Height; 후면 버퍼 높이 
	//	DXGI_RATIONAL RefreshRate; 디스플레이 갱신율
	//	DXGI_FORMAT Format; 후면 버퍼 픽셀 형식
	//	DXGI_MODE_SCANLINE_ORDER ScanlineOrdering; 디스플레이 스캔라인 모드
	//	DXGI_MODE_SCALING Scaling; 디스플레이 비례모드
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

	// 교환 사슬의 생성
	// IDXGIFactory 인스턴스를 통해 CreateSwapChain메서드를 호출해서 생성
	// 일반적으로 CreateDXGI를 통해 생성할 경우 서로 다른 인스턴스를 사용하려 들기 때문에 오류가 발생하므로 쿼리를 통해 디바이스를 찾아줘야한다.
	// DXGI( DirectX Graphic Infrastructure - 그래픽 하드웨어 나열, 창모드와 전체화면 전환등 그래픽에 관련된 작업을 함. 
	//분리된 이유는 D2D와 같은, 다른 라이브러리을 쓰는데 문제가 생기기 때문)
	IDXGIDevice* dxgiDevice = 0;
	HR(D3D11Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	// 스왑체인이 생성되었다.
	HR(dxgiFactory->CreateSwapChain(D3D11Device, &sd, &SwapChain));

	SAFE_RELEASE(dxgiDevice);
	SAFE_RELEASE(dxgiAdapter);
	SAFE_RELEASE(dxgiFactory);
	return true;
}

bool RHID3D11Device::CreateRenderTargetView()
{
	// 어떠한 자원을 파이프라인에 직접 묶는 것이 아니고, 자원에 대한 뷰를 파이프라인단계에 묶는 것 (렌더타겟뷰, 혹은 셰이더 리소스뷰)
	ID3D11Texture2D* backBuffer = nullptr;

	// 교환사슬을 가리키는 포인터를 얻는다. 
	//virtual HRESULT STDMETHODCALLTYPE GetBuffer(
	//	/* [in] */ UINT Buffer, 이때 첫번째 매개변수는 후면버퍼의 색인
	//	/* [annotation][in] */ 
	//	__in  REFIID riid, 버퍼의 인터페이스 형식(텍스쳐 2D 형식)
	//	/* [annotation][out][in] */ 
	//	__out  void **ppSurface) = 0; 마지막은 후면버퍼를 가리키는 포인터를 돌려줌
	HR(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));

	//virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView(
	//	/* [annotation] */
	//	__in  ID3D11Resource *pResource, 리소스 대상(후면버퍼)
	//	/* [annotation] */
	//	__in_opt  const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,  자원에 담긴 원소들의 자료형식 지정, 이미 지정해서 자원을 생성했더면 null을 지정해줘도된다.
	//	/* [annotation] */
	//	__out_opt  ID3D11RenderTargetView **ppRTView) = 0; // 생성된 렌더 대상 뷰를 돌려준다.
	HR(D3D11Device->CreateRenderTargetView(backBuffer, 0, &RenderTargetView));

	SAFE_RELEASE(backBuffer); // 해제해주는은 이유는 겟버퍼를 통해서 ㅂ퍼를 얻었을때 참조횟수가 증가하기 때문

	return true;
}

bool RHID3D11Device::CreateDepthStencilView()
{
	//typedef
	//enum D3D11_USAGE
	//{
	//	D3D11_USAGE_DEFAULT = 0,자원을 GPU가 읽고 쓸때. 깊이 스텐실이 GPU만 쓰기때문에 이걸 쓴다
	//	D3D11_USAGE_IMMUTABLE = 1, 자원을 생성후 그 내용을 바꾸지 않는 경우
	//	D3D11_USAGE_DYNAMIC = 2, CPU가 자원을 빈번하게 갱신할때
	//	D3D11_USAGE_STAGING = 3 응용프로그램이 자원의 복사본을 일거야 한때 사용
	//} 	D3D11_USAGE;

	//typedef
	//enum D3D11_BIND_FLAG
	//{
	//	D3D11_BIND_VERTEX_BUFFER = 0x1L,
	//	D3D11_BIND_INDEX_BUFFER = 0x2L,
	//	D3D11_BIND_CONSTANT_BUFFER = 0x4L,
	//	D3D11_BIND_SHADER_RESOURCE = 0x8L, 셰이더 자원으로 바인드
	//	D3D11_BIND_STREAM_OUTPUT = 0x10L,
	//	D3D11_BIND_RENDER_TARGET = 0x20L, 렌더타겟에 바인드
	//	D3D11_BIND_DEPTH_STENCIL = 0x40L, 
	//	D3D11_BIND_UNORDERED_ACCESS = 0x80L
	//} 	D3D11_BIND_FLAG;

	//typedef
	//enum D3D11_CPU_ACCESS_FLAG
	//{
	//	D3D11_CPU_ACCESS_WRITE = 0x10000L, CPU 접근에 관련된 플래그 Dynamic 일때만 적용된다.
	//	D3D11_CPU_ACCESS_READ = 0x20000L
	//} 	D3D11_CPU_ACCESS_FLAG;

	//typedef struct D3D11_TEXTURE2D_DESC
	//{
	//	UINT Width; 텍스쳐의 너비(텍셀 단위)
	//	UINT Height; 텍스쳐의 높이 (텍셀 단위)
	//	UINT MipLevels; 밉맵수준의 갯수, 뎁스스텐실에서는 하나
	//	UINT ArraySize; 텍스쳐 배열의 텍스처 갯수, 뎁스스텐실에서는 하나
	//	DXGI_FORMAT Format; 텍셀의 포맷
	//	DXGI_SAMPLE_DESC SampleDesc; 다중 표본 갯수와 품질, 렌더 타겟에 쓰인 설정과 일치해야함
	//	D3D11_USAGE Usage; // 텍스쳐의 용도 위 참조
	//	UINT BindFlags;  // 위 참조
	//	UINT CPUAccessFlags; // 위 참조, 여기서는 0을 지정
	//	UINT MiscFlags; // 기타 플래그
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
	//	__in_range(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)  UINT NumViews, // 렌더 타겟의 겟수, 장면을 여러개에 렌덩힐 할경우, 여러개를 지정 가능
	//	/* [annotation] */
	//	__in_ecount_opt(NumViews)  ID3D11RenderTargetView *const *ppRenderTargetViews, // 렌더타겟뷰들
	//	/* [annotation] */
	//	__in_opt  ID3D11DepthStencilView *pDepthStencilView) = 0; // 뎁스스탠실은 한개뿐이다.
	// 파이프라인의 출력병합기단계에 바인딩
	D3D11DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);
}


void RHID3D11Device::SetViewport()
{
	// 뷰포트 설정 (그려넣고자 하는 범위)
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