#pragma once

// dx11은 dx9와는 다르게 Full capability를 지원해야 지원하게 되어있다.(MSAA용 멀티샘플링등 몇가지는 예외)
// dx9 은 각 피쳐별로 따로따로 연결하게 되어있었지만, dx11은 일정 이상의 capability가 지원되어야 실행되기 때문이다.

// COM (Component Object Model)은 dx에서 사용하는 오브젝트 관리 테크닉
// 우리는 여기서 COM 객체를 C++ 클래스로 간주하고 사용
// COM객체 내부는 우리가 알수 없음 (인터페이스 형태로 지원)
// 따라서 new로 생성하거나, delete로 삭제 하면 안된다.
// Release처리를 통해 메모리를 해제하는 형식
// 인터페이스 형태를 통해 지원하기 때문에 모든 DX11 COM 객체는 대문자 I로 시작한다.

// 1. D3D11CreateDevice 함수를 이용하여 장치와 장치 문맥을 생성한다.
// 2. MSAA 품질 여부 점검
// 3. 교환사슬 특성을 서술
// 4. DXGI 팩토리 쿼리를 통해 SwapChain 인스턴스 생성
// 5. 후면버퍼 렌더 뷰생성
// 6. 깊이 스텐실 버퍼와, 깊이 스텐실 뷰를 생성
// 7. 깊이 스탠실 뷰를 렌더링 파이프 출력 병합기 단계에 설정
// 8. 뷰포트 설정

// 설명용 파일로 Deprecated

class RHID3D11Device
{
public:
	// 일단 D3D11Device는 하나만 존재함을 가정하고 싱글톤형태로 선언한다.
	static RHID3D11Device* GetInstance()
	{
		static RHID3D11Device Instance;
		return &Instance;
	}
	
	bool Initialize();
	bool Release();


private:
	RHID3D11Device();
	~RHID3D11Device();

	bool CreateDevice();
	bool CreateSwapChain();
	bool CreateRenderTargetView();
	bool CreateDepthStencilView();
	void SetRenderTargets();
	void SetViewport();

private:
	// C++11부터는 생성시 초기화도 가능하다.
	ID3D11Device* D3D11Device = nullptr;

	// 책에서는 이 컨텍스트에 대해서 ImmediateContext라는 이름으로 사용하고 있습니다.
	// 이는 즉시 문맥을 사용하여 커맨드라인을 사용하는 것이고,
	// DeferredContext라는 개념으로 멀티 쓰레드를 이용해 커맨드라인을 생성하여 날리는 방법도 있습니다.
	ID3D11DeviceContext* D3D11DeviceContext = nullptr;
	
	// 스왑 체인
	IDXGISwapChain* SwapChain = nullptr;

	// 렌더타겟뷰
	ID3D11RenderTargetView* RenderTargetView = nullptr;
	
	// 뎁스스텐실 자원
	ID3D11Texture2D* DepthStencilBuffer = nullptr;
	ID3D11DepthStencilView* DepthStencilView = nullptr;

	bool bSupportMSAA = false;
};