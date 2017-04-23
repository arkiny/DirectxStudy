#pragma once

// dx11�� dx9�ʹ� �ٸ��� Full capability�� �����ؾ� �����ϰ� �Ǿ��ִ�.(MSAA�� ��Ƽ���ø��� ����� ����)
// dx9 �� �� ���ĺ��� ���ε��� �����ϰ� �Ǿ��־�����, dx11�� ���� �̻��� capability�� �����Ǿ�� ����Ǳ� �����̴�.

// COM (Component Object Model)�� dx���� ����ϴ� ������Ʈ ���� ��ũ��
// �츮�� ���⼭ COM ��ü�� C++ Ŭ������ �����ϰ� ���
// COM��ü ���δ� �츮�� �˼� ���� (�������̽� ���·� ����)
// ���� new�� �����ϰų�, delete�� ���� �ϸ� �ȵȴ�.
// Releaseó���� ���� �޸𸮸� �����ϴ� ����
// �������̽� ���¸� ���� �����ϱ� ������ ��� DX11 COM ��ü�� �빮�� I�� �����Ѵ�.

// 1. D3D11CreateDevice �Լ��� �̿��Ͽ� ��ġ�� ��ġ ������ �����Ѵ�.
// 2. MSAA ǰ�� ���� ����
// 3. ��ȯ�罽 Ư���� ����
// 4. DXGI ���丮 ������ ���� SwapChain �ν��Ͻ� ����
// 5. �ĸ���� ���� �����
// 6. ���� ���ٽ� ���ۿ�, ���� ���ٽ� �並 ����
// 7. ���� ���Ľ� �並 ������ ������ ��� ���ձ� �ܰ迡 ����
// 8. ����Ʈ ����

// ����� ���Ϸ� Deprecated

class RHID3D11Device
{
public:
	// �ϴ� D3D11Device�� �ϳ��� �������� �����ϰ� �̱������·� �����Ѵ�.
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
	// C++11���ʹ� ������ �ʱ�ȭ�� �����ϴ�.
	ID3D11Device* D3D11Device = nullptr;

	// å������ �� ���ؽ�Ʈ�� ���ؼ� ImmediateContext��� �̸����� ����ϰ� �ֽ��ϴ�.
	// �̴� ��� ������ ����Ͽ� Ŀ�ǵ������ ����ϴ� ���̰�,
	// DeferredContext��� �������� ��Ƽ �����带 �̿��� Ŀ�ǵ������ �����Ͽ� ������ ����� �ֽ��ϴ�.
	ID3D11DeviceContext* D3D11DeviceContext = nullptr;
	
	// ���� ü��
	IDXGISwapChain* SwapChain = nullptr;

	// ����Ÿ�ٺ�
	ID3D11RenderTargetView* RenderTargetView = nullptr;
	
	// �������ٽ� �ڿ�
	ID3D11Texture2D* DepthStencilBuffer = nullptr;
	ID3D11DepthStencilView* DepthStencilView = nullptr;

	bool bSupportMSAA = false;
};