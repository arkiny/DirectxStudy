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

	// C++11���ʹ� ������ �ʱ�ȭ�� �����ϴ�.
	ID3D11Device* md3dDevice = nullptr;

	// å������ �� ���ؽ�Ʈ�� ���ؼ� ImmediateContext��� �̸����� ����ϰ� �ֽ��ϴ�.
	// �̴� ��� ������ ����Ͽ� Ŀ�ǵ������ ����ϴ� ���̰�,
	// DeferredContext��� �������� ��Ƽ �����带 �̿��� Ŀ�ǵ������ �����Ͽ� ������ ����� �ֽ��ϴ�.
	ID3D11DeviceContext* md3dImmediateContext = nullptr;

	// ���� ü��
	IDXGISwapChain* mSwapChain = nullptr;
	ID3D11Texture2D* mDepthStencilBuffer = nullptr;

	// ����Ÿ�ٺ�
	ID3D11RenderTargetView* mRenderTargetView = nullptr;

	// �������ٽ� �ڿ�
	ID3D11DepthStencilView* mDepthStencilView = nullptr;

	D3D11_VIEWPORT mScreenViewport;

	std::wstring mMainWndCaption = L"D3D11Application";

	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;

	int mClientWidth = 800;
	int mClientHeight = 600;
	bool mEnable4xMsaa = false;
};

