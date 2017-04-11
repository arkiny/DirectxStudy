#include "stdafx.h"
#include "BoxApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	BoxApp theApp(hInstance);

	if (!theApp.Init())
		return 0;

	return theApp.Run();
}


BoxApp::BoxApp(HINSTANCE hInstance)
	:D3DApp(hInstance)
	, mTheta(1.5f * MathHelper::Pi), mPhi(0.25f * MathHelper::Pi), mRadius(8)
{
	mMainWndCaption = L"BoxDemo";

	XMMATRIX I = XMMatrixIdentity();
	
	XMStoreFloat4x4(&mWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
	
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;
}


BoxApp::~BoxApp()
{
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
}

bool BoxApp::Init()
{
	if (!D3DApp::Init())
		return false;

	BuildGeometryBuffers();
	BuildFX();
	BuildVertexLayout();

	return true;
}

void BoxApp::OnResize()
{
	D3DApp::OnResize();

	// �����찡 ������ �Ǹ�, ��Ⱦ��� �������� ��Ʈ������ ��������� �մϴ�.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void BoxApp::UpdateScene(float dt)
{
	// ���Ǹ��� ������ ī�׽þ� ��ǥ�� ��ȯ���ݴϴ�.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	// �� ��Ʈ������ �������ݴϴ�. 
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}

void BoxApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ���ؽ��� ��ǲ ���̾ƿ� ����
	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	// ���ؽ� ���� ���ε�
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
	// �ε��� ���� ���ε�
	md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

	// �ܽ�źƮ ���� ����
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world*view*proj;

	// ���ε� �� �з����� ���� ó��
	mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	// ��ũ�� ���ε�
	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		// �׸��� ȣ�⿡�� ���� ���ε��� ����
		// ù������ ��ġ
		// ����� ���ε鿡 �������� ������
		md3dImmediateContext->DrawIndexed(36, 0, 0);
	}

	HR(mSwapChain->Present(0, 0));
}

void BoxApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void BoxApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void BoxApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// ������ �ȼ��������� .25���� ġȯ
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// �˵� ī�޶�� ������Ʈ
		mTheta += dx;
		mPhi += dy;

		// ���� ����.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// �� �ȼ��� 0.005 ������ �����̰� ó��
		float dx = 0.005f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - mLastMousePos.y);

		// ��ǲ�� ���� ī�޶� �ݰ� ����
		mRadius += dx - dy;

		// ���� ����
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void BoxApp::BuildGeometryBuffers()
{
	// ���ؽ� ���� ����
	Vertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), (const float*)&Colors::White },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), (const float*)&Colors::Black },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), (const float*)&Colors::Red },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), (const float*)&Colors::Green },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), (const float*)&Colors::Blue },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), (const float*)&Colors::Yellow },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), (const float*)&Colors::Cyan },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), (const float*)&Colors::Magenta }
	};

	//typedef struct D3D11_BUFFER_DESC
	//{
	//	UINT ByteWidth; // ������ ũ�� (����Ʈ)
	//	D3D11_USAGE Usage; // ������ ���뵵
	//	UINT BindFlags; // ���ε� �Ǵ� ������ ���ó
	//	UINT CPUAccessFlags; // CPU �� ���ۿ� ���� �ϴ� ��� ����
	//	UINT MiscFlags; // �������۴� ������ ����.
	//	UINT StructureByteStride; // ���� �����϶� ���� ���� �ϳ��� ũ�� 
	//} 	D3D11_BUFFER_DESC;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE; // �ø����� �������� ������
	vbd.ByteWidth = sizeof(Vertex) * 8; // ������ ����
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���ؽ� ���ۿ� ���ε� �Ұ�
	vbd.CPUAccessFlags = 0; // cpu ���� ���Ұ�
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	//typedef struct D3D11_SUBRESOURCE_DATA
	//{
	//	const void *pSysMem;
	//	UINT SysMemPitch; // �������ۿ����� �Ⱦ�
	//	UINT SysMemSlicePitch; // �������ۿ����� �Ⱦ�
	//} 	D3D11_SUBRESOURCE_DATA;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices; // �������۸� �ʱ�ȭ�� �ڷḦ ���� �ý��� �޸� �迭
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));


	// �ε��� ���� ����

	UINT indices[] = {
		// �ո�
		0, 1, 2,
		0, 2, 3,

		// �޸�
		4, 6, 5,
		4, 7, 6,

		// ���� ��
		4, 5, 1,
		4, 1, 0,

		// ������ ��
		3, 2, 6,
		3, 6, 7,

		// ����
		1, 5, 6,
		1, 6, 2,

		// �Ʒ���
		4, 0, 3,
		4, 3, 7
	};

	// ���ؽ� ���ۿ� ���������, �ε��� ���۷� ���ε��Ѵ�.
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// ���ι��� �ʱ�ȭ �ڷ� ����
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));
}

void BoxApp::BuildFX()
{
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;

	//----------------------------------------------------------------------------
	// D3DX11Compile:
	// ------------------
	// Compiles an effect or shader.
	//
	// Parameters:
	//  pSrcFile
	//      �ҽ����� �̸�
	//  pDefines
	//      �Ⱦ� ��� ���������, �𸮾󿡼��� ���� ������ ������ �����Ѵ�.
	//  pInclude
	//      ������ �ÿ� ������ø� �������ִ� �ɼ�      
	//  pFunctionName
	//      ������ �̸� ����
	//  pProfile ���̴� ���� ����
	//      Instruction set to be used when generating code.  Currently supported
	//      profiles are "vs_1_1",  "vs_2_0", "vs_2_a", "vs_2_sw", "vs_3_0",
	//                   "vs_3_sw", "vs_4_0", "vs_4_1",
	//                   "ps_2_0",  "ps_2_a", "ps_2_b", "ps_2_sw", "ps_3_0", 
	//                   "ps_3_sw", "ps_4_0", "ps_4_1",
	//                   "gs_4_0",  "gs_4_1",
	//                   "tx_1_0",
	//                   "fx_4_0",  "fx_4_1"
	//      Note that this entrypoint does not compile fx_2_0 targets, for that
	//      you need to use the D3DX9 function.
	//  Flags1 // ������ �ɼ� �÷���
	//      See D3D10_SHADER_xxx flags.
	//  Flags2 // ��� ��� �÷���
	//      See D3D10_EFFECT_xxx flags.
	//  ppShader // �񵿱� ������ �ɼ������� �Ⱦ�
	//      Returns a buffer containing the created shader.  This buffer contains
	//      the compiled shader code, as well as any embedded debug and symbol
	//      table info.  (See D3D10GetShaderConstantTable)
	//  ppErrorMsgs  ���� �޽��� ��� ����ü ����
	//      Returns a buffer containing a listing of errors and warnings that were
	//      encountered during the compile.  If you are running in a debugger,
	//      these are the same messages you will see in your debug output.
	//  pHResult // �񵿱� �����Ͻÿ� �����ڵ� ��ȸ
	//      Pointer to a memory location to receive the return value upon completion.
	//      Maybe NULL if not needed.
	//      If pPump != NULL, pHResult must be a valid memory location until the
	//      the asynchronous execution completes.
	//----------------------------------------------------------------------------

	HRESULT hr = D3DX11CompileFromFile(L"../FX/color.fx", 0, 0, 0, "fx_5_0", shaderFlags,
		0, 0, &compiledShader, &compilationMsgs, 0);

	// compilationMsgs can store errors or warnings.
	if (compilationMsgs != 0)
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	}

	// Even if there are no compilationMsgs, check to make sure there were no other errors.
	if (FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
	}

	// ������ ���� ����, �ش� ���̴��� ���� ȿ�� ��ü�� ��Ÿ���� ��ü ����
	//----------------------------------------------------------------------------
	// D3DX11CreateEffectFromMemory:
	// --------------------------
	// Creates an effect from a binary effect or file
	//
	// Parameters:
	//
	// [in]
	//
	//
	//  pData
	//      ������ �� ����Ʈ �ڷ�
	//  DataLength
	//      �����ϵ� ȿ�� �ڷ��� ����Ʈ ���� ũ��
	//  FXFlags
	//      ȿ�� �÷��׵�
	//      
	//  pDevice
	//      ����̽� ��ġ�� ����Ű�� ������
	//
	// [out]
	//
	//  ppEffect
	//      ������ ����Ʈ ������ ������
	//
	//----------------------------------------------------------------------------
	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
		0, md3dDevice, &mFX));

	// Done with compiled shader.
	ReleaseCOM(compiledShader);

	mTech = mFX->GetTechniqueByName("ColorTech"); // ��ũ�� �����ͼ� ����
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix(); // ��� ��Ʈ���� ���ε��� ���� ������ ����
}

void BoxApp::BuildVertexLayout()
{
	// ���� ����ü�� �������� � �뵵���� ����

	// ���ؽ� �ø��� �� ���̾ƿ� ����
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		// �ø��� ���ڿ� �̸�, �ø��ؿ� �ο��� �ε���, ������ �ڷ�����, ���� ���۽����� ����, ������ (�������� ��� 0������, �÷� ������ 12����Ʈ �ǳʶپ ó���ؾ��Ѵ�), �ν�ź������ ����, �ν��Ͻ� ���� ����Ʈ)
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// ��ǲ ���̾ƿ� ����
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	// �Է¹�ġ ����

	//virtual HRESULT STDMETHODCALLTYPE CreateInputLayout(
	//	/* [annotation] */ ���� ����ü�� �����ϴ� D3D11_INPUT_ELEMENT_DESC �迭
	//	__in_ecount(NumElements)  const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
	//	/* [annotation] */ �迭�� ���� ����
	//	__in_range(0, D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT)  UINT NumElements,
	//	/* [annotation] */ ���� ���̴��� �������ؼ� ���� ����Ʈ�ڵ带 ����Ű�� ������
	//	__in  const void *pShaderBytecodeWithInputSignature,
	//	/* [annotation] */ ����Ʈ �ڵ��� ũ��
	//	__in  SIZE_T BytecodeLength,
	//	/* [annotation] */ ������ �Է� ��ġ
	//	__out_opt  ID3D11InputLayout **ppInputLayout) = 0;

	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}