#include "stdafx.h"
#include "ShapesDemo.h"

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
//	PSTR cmdLine, int showCmd)
//{
//	// ����� ���带 ���� ó��
//#if defined(DEBUG) | defined(_DEBUG)
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif
//
//	ShapesDemo theApp(hInstance);
//
//	if (!theApp.Init())
//		return 0;
//
//	return theApp.Run();
//}

ShapesDemo::ShapesDemo(HINSTANCE hinst)
	: D3DApp(hinst),
	mfxWorldViewProj(0), mInputLayout(0), mGridIndexCount(0),
	mTheta(1.5f*MathHelper::Pi), mPhi(0.1f*MathHelper::Pi), mRadius(50.0f)
{
	mMainWndCaption = L"Shape Demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);

	XMMATRIX boxScale = XMMatrixScaling(2.0f, 1.0f, 2.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));

	XMMATRIX centerSphereScale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
	XMMATRIX centerSphereOffset = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	XMStoreFloat4x4(&mCenterSphere, XMMatrixMultiply(centerSphereScale, centerSphereOffset));

	for (int i = 0; i < 5; ++i)
	{
		XMStoreFloat4x4(&mCylWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f));
		XMStoreFloat4x4(&mCylWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f));

		XMStoreFloat4x4(&mSphereWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f));
		XMStoreFloat4x4(&mSphereWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f));
	}
}


ShapesDemo::~ShapesDemo()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
	ReleaseCOM(mWireframeRS);
}

bool ShapesDemo::Init()
{
	if (!D3DApp::Init())
		return false;

	BuildGeometryBuffers();
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

void ShapesDemo::OnResize()
{
	D3DApp::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void ShapesDemo::UpdateScene(float dt)
{
	// ���Ǹ��� ������ ī�׽þ����� ������
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	// �þ� ��Ʈ���� ����
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}

void ShapesDemo::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	md3dImmediateContext->RSSetState(mWireframeRS);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX viewProj = view*proj;
	
	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// �׸��带 �׸���.
		XMMATRIX world = XMLoadFloat4x4(&mGridWorld);
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

		// ���ڸ� �׸���.
		world = XMLoadFloat4x4(&mBoxWorld);
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);

		// �߾��� ���� �׸���.
		world = XMLoadFloat4x4(&mCenterSphere);
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);

		// ����յ��� �׸���.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&mCylWorld[i]);
			mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
			mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
		}

		// ��� ���� ������ �׸���.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&mSphereWorld[i]);
			mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
			mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
		}
	}

	HR(mSwapChain->Present(0, 0));
}

void ShapesDemo::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void ShapesDemo::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void ShapesDemo::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// �ȼ��� .25 �� ó��
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// �ڽ��� ���� ���� ó��
		mTheta += dx;
		mPhi += dy;

		// ���� ����
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// ���� ���� ó��
		float dx = 0.2f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.2f*static_cast<float>(y - mLastMousePos.y);

		// ī�޶� ���� ó��
		mRadius += dx - dy;

		// ���� ����
		mRadius = MathHelper::Clamp(mRadius, 50.0f, 500.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void ShapesDemo::BuildGeometryBuffers()
{
	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(20.0f, 30.0f, 60, 40, grid);
	geoGen.CreateSphere(0.5f, 20, 20, sphere);
	geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, cylinder);

	// ���� ������ ����
	mBoxVertexOffset = 0;
	mGridVertexOffset = box.Vertices.size();
	mSphereVertexOffset = mGridVertexOffset + grid.Vertices.size();
	mCylinderVertexOffset = mSphereVertexOffset + sphere.Vertices.size();

	// �ε��� ���� ����
	mBoxIndexCount = box.Indices.size();
	mGridIndexCount = grid.Indices.size();
	mSphereIndexCount = sphere.Indices.size();
	mCylinderIndexCount = cylinder.Indices.size();

	// ����� �ε������ۿ��� �������� �����صд�.
	mBoxIndexOffset = 0;
	mGridIndexOffset = mBoxIndexCount;
	mSphereIndexOffset = mGridIndexOffset + mGridIndexCount;
	mCylinderIndexOffset = mSphereIndexOffset + mSphereIndexCount;

	UINT totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();
	
	UINT totalIndexCount =
		mBoxIndexCount +
		mGridIndexCount +
		mSphereIndexCount +
		mCylinderIndexCount;

	std::vector<Vertex> vertices(totalVertexCount);

	XMFLOAT4 black(0, 0, 0, 1.0f);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Color = black;
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Color = black;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Color = black;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Color = black;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void ShapesDemo::BuildFX()
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

void ShapesDemo::BuildVertexLayout()
{
	// ���ؽ� ���̾ƿ� ����
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// ��ǲ ���̾ƿ� ����
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}
