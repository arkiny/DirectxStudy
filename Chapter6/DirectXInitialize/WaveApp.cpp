#include "stdafx.h"
#include "WaveApp.h"
#include "Waves.h"

WaveApp::WaveApp(HINSTANCE hInstance)
	:D3DApp(hInstance)
	, mTheta(1.5f*MathHelper::Pi), mPhi(0.1f*MathHelper::Pi), mRadius(200.0f)
{
	mMainWndCaption = L"Waves Demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridWorld, I);
	XMStoreFloat4x4(&mWavesWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
	if (pWaves == nullptr)
	{
		pWaves = new Waves();
	}
}


WaveApp::~WaveApp()
{
	ReleaseCOM(mLandVB);
	ReleaseCOM(mLandIB);
	ReleaseCOM(mWavesVB);
	ReleaseCOM(mWavesIB);
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
	ReleaseCOM(mWireframeRS);
	SafeDelete(pWaves);
}

bool WaveApp::Init()
{
	if (!D3DApp::Init())
		return false;

	if (pWaves)
	{
		pWaves->Init(200, 200, 0.8f, 0.03f, 3.25f, 0.4f);
	}	

	BuildLandGeometryBuffers();
	BuildWavesGeometryBuffers();
	BuildFX();
	BuildVertexLayout();

	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	HR(md3dDevice->CreateRasterizerState(&wireframeDesc, &mWireframeRS));

	return true;
}

void WaveApp::OnResize()
{
	D3DApp::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void WaveApp::UpdateScene(float dt)
{
	// ���Ǹ��� ��ǥ�� ī�׽þ����� ��ȯ
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	// ���Ʈ���� ��ȯ
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);

	//
	// .25�� ���� ���� ���̺긦 �����Ѵ�.
	//
	static float t_base = 0.0f;
	if ((mTimer.GetGameTime() - t_base) >= 0.25f)
	{
		t_base += 0.25f;

		DWORD i = 5 + rand() % 190;
		DWORD j = 5 + rand() % 190;

		float r = MathHelper::RandF(1.0f, 2.0f);

		pWaves->Disturb(i, j, r);
	}

	pWaves->Update(dt);

	//
	// ���ο� ����� �ַ�ǿ� �ݿ��Ѵ�.
	//

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(md3dImmediateContext->Map(mWavesVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

	Vertex* v = reinterpret_cast<Vertex*>(mappedData.pData);
	for (UINT i = 0; i < pWaves->VertexCount(); ++i)
	{
		v[i].Pos = pWaves->operator[](i);
		v[i].Color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	md3dImmediateContext->Unmap(mWavesVB, 0);
}

void WaveApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		//
		// ���� �׸���.
		//
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mLandVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mLandIB, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX world = XMLoadFloat4x4(&mGridWorld);
		XMMATRIX worldViewProj = world*view*proj;
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mGridIndexCount, 0, 0);

		//
		// ������ �׸���.
		//

		md3dImmediateContext->RSSetState(mWireframeRS);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mWavesVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mWavesIB, DXGI_FORMAT_R32_UINT, 0);

		world = XMLoadFloat4x4(&mWavesWorld);
		worldViewProj = world*view*proj;
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(3 * pWaves->TriangleCount(), 0, 0);

		// �����Ͷ������� �������� �ǵ�����.
		md3dImmediateContext->RSSetState(0);
	}

	HR(mSwapChain->Present(0, 0));
}

void WaveApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void WaveApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void WaveApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// ������ �ȼ��� .25�� �̵��ϵ����Ѵ�.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// �˵� ī�޶� ó��
		mTheta += dx;
		mPhi += dy;

		// ���� ����
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// �ȼ��� .2���� �̵� ó��
		float dx = 0.2f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.2f*static_cast<float>(y - mLastMousePos.y);

		// ī�޶� �ݰ��� ó��
		mRadius += dx - dy;

		// ī�޶� �ݰ� ����.
		mRadius = MathHelper::Clamp(mRadius, 50.0f, 500.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

float WaveApp::GetHeight(float x, float z) const
{
	return 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));
}

void WaveApp::BuildLandGeometryBuffers()
{
	GeometryGenerator::MeshData grid;

	GeometryGenerator geoGen;

	geoGen.CreateGrid(160.0f, 160.0f, 50, 50, grid);

	mGridIndexCount = grid.Indices.size();

	// ���׽� ������ �����ϰ�, ���� ���� ���� ĥ���ش�.
	
	std::vector<Vertex> vertices(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;

		p.y = GetHeight(p.x, p.z);

		vertices[i].Pos = p;

		// ���� ���� ���� �����ش�.
		if (p.y < -10.0f)
		{
			// �� �ؾ� ��
			vertices[i].Color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		}
		else if (p.y < 5.0f)
		{
			// ���� ���λ�
			vertices[i].Color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		}
		else if (p.y < 12.0f)
		{
			// ��ο� ���λ�
			vertices[i].Color = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		}
		else if (p.y < 20.0f)
		{
			// ��ο� ����
			vertices[i].Color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		}
		else
		{
			// �Ͼ� ��
			vertices[i].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * grid.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mLandVB));

	//
	// �ε������� �������ش�.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mGridIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &grid.Indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mLandIB));
}

void WaveApp::BuildWavesGeometryBuffers()
{
	// ���ؽ� ���۸� �����. �츮�� ���⼭ ������ �������ְ�
	// �ùķ��̼ǿ��� �Ź� ������Ʈ ���� �����̴�.

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex) * pWaves->VertexCount();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	HR(md3dDevice->CreateBuffer(&vbd, 0, &mWavesVB));


	// �ε��� ���۸� �����. �ε��� ���۵��� ������ �ְ�, �ѹ��� �������ָ� �ȴ�.
	
	std::vector<UINT> indices(3 * pWaves->TriangleCount()); // �Ѹ鿡 3���� �ε���

														   // ��� ���带 ��ȯ�Ѵ�.
	UINT m = pWaves->RowCount();
	UINT n = pWaves->ColumnCount();
	int k = 0;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (DWORD j = 0; j < n - 1; ++j)
		{
			indices[k] = i*n + j;
			indices[k + 1] = i*n + j + 1;
			indices[k + 2] = (i + 1)*n + j;

			indices[k + 3] = (i + 1)*n + j;
			indices[k + 4] = i*n + j + 1;
			indices[k + 5] = (i + 1)*n + j + 1;

			k += 6; // ���� �����
		}
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mWavesIB));
}

void WaveApp::BuildFX()
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
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix(); // ��� ��Ʈ����
}

void WaveApp::BuildVertexLayout()
{
	// ���ؽ� �Է� ���̾ƿ� ����
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// �ε��� �Է� ���̾ƿ� ����
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}