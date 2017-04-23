#pragma once
class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);
	virtual ~D3DApp();

	HINSTANCE AppInst() const;
	HWND MainWnd() const;
	float AspectRatio() const;

	int Run();

	// Framework Methods
	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt) = 0;
	virtual void DrawScene() = 0;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Mouse input processor
	virtual void OnMouseDown(WPARAM btnState, int x, int y){}
	virtual void OnMouseUp(WPARAM btnState, int x, int y){}
	virtual void OnMouseMove(WPARAM btnState, int x, int y){}

protected:
	bool InitMainWindow();
	bool InitDirect3D();
	void CalculateFrameStats();

protected:
	HINSTANCE mhAPPInst;
	HWND mhMainWnd; 
	bool mAppPaused=false;
	bool mMinimized=false;
	bool mMaximized=false;
	bool mResizing=false;
	UINT m4xMsaaQuality=0;

	GameTimer mTimer;

	// C++11부터는 생성시 초기화도 가능하다.
	ID3D11Device* md3dDevice = nullptr;

	// 책에서는 이 컨텍스트에 대해서 ImmediateContext라는 이름으로 사용하고 있습니다.
	// 이는 즉시 문맥을 사용하여 커맨드라인을 사용하는 것이고,
	// DeferredContext라는 개념으로 멀티 쓰레드를 이용해 커맨드라인을 생성하여 날리는 방법도 있습니다.
	ID3D11DeviceContext* md3dImmediateContext = nullptr;

	// 스왑 체인
	IDXGISwapChain* mSwapChain = nullptr;
	ID3D11Texture2D* mDepthStencilBuffer = nullptr;

	// 렌더타겟뷰
	ID3D11RenderTargetView* mRenderTargetView = nullptr;

	// 뎁스스텐실 자원
	ID3D11DepthStencilView* mDepthStencilView = nullptr;

	D3D11_VIEWPORT mScreenViewport;

	std::wstring mMainWndCaption = L"D3D11Application";

	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;

	int mClientWidth = 800;
	int mClientHeight = 600;
	bool mEnable4xMsaa = false;
};

