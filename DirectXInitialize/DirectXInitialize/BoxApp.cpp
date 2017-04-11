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

	// 윈도우가 재조정 되면, 종횡비와 프로젝션 매트릭스를 재계산해줘야 합니다.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void BoxApp::UpdateScene(float dt)
{
	// 스피리컬 각도를 카테시안 좌표로 변환해줍니다.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	// 뷰 매트릭스를 생성해줍니다. 
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

	// 버텍스의 인풋 레이아웃 설정
	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	// 버텍스 버퍼 바인딩
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
	// 인덱스 버퍼 바인딩
	md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

	// 콘스탄트 버퍼 설정
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world*view*proj;

	// 바인딩 된 패러미터 설정 처리
	mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	// 테크닉 바인딩
	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		// 그리기 호출에서 사용될 색인들의 갯수
		// 첫색인의 위치
		// 사용할 색인들에 더해지는 정수값
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
		// 각각의 픽셀움직임을 .25도로 치환
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// 궤도 카메라로 업데이트
		mTheta += dx;
		mPhi += dy;

		// 각도 제한.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// 한 픽셀당 0.005 유닛을 움직이게 처리
		float dx = 0.005f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - mLastMousePos.y);

		// 인풋에 따른 카메라 반경 조절
		mRadius += dx - dy;

		// 각도 제한
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void BoxApp::BuildGeometryBuffers()
{
	// 버텍스 버퍼 생성
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
	//	UINT ByteWidth; // 버퍼의 크기 (바이트)
	//	D3D11_USAGE Usage; // 버퍼의 사용용도
	//	UINT BindFlags; // 바인드 되는 버퍼의 사용처
	//	UINT CPUAccessFlags; // CPU 가 버퍼에 접근 하는 방식 결정
	//	UINT MiscFlags; // 정점버퍼는 쓸일이 없다.
	//	UINT StructureByteStride; // 구조 버퍼일때 쓰는 원소 하나의 크기 
	//} 	D3D11_BUFFER_DESC;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE; // 올리고나선 수정하지 않을거
	vbd.ByteWidth = sizeof(Vertex) * 8; // 사이즈 설정
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 버텍스 버퍼에 바인딩 할것
	vbd.CPUAccessFlags = 0; // cpu 접근 안할것
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	//typedef struct D3D11_SUBRESOURCE_DATA
	//{
	//	const void *pSysMem;
	//	UINT SysMemPitch; // 정점버퍼에서는 안씀
	//	UINT SysMemSlicePitch; // 정점버퍼에서는 안씀
	//} 	D3D11_SUBRESOURCE_DATA;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices; // 정점버퍼를 초기화할 자료를 담은 시스템 메모리 배열
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));


	// 인덱스 버퍼 생성

	UINT indices[] = {
		// 앞면
		0, 1, 2,
		0, 2, 3,

		// 뒷면
		4, 6, 5,
		4, 7, 6,

		// 왼쪽 면
		4, 5, 1,
		4, 1, 0,

		// 오른쪽 면
		3, 2, 6,
		3, 6, 7,

		// 윗면
		1, 5, 6,
		1, 6, 2,

		// 아랫면
		4, 0, 3,
		4, 3, 7
	};

	// 버텍스 버퍼와 비슷하지만, 인덱스 버퍼로 바인딩한다.
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// 색인버퍼 초기화 자료 지정
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
	//      소스파일 이름
	//  pDefines
	//      안쓸 고급 기법이지만, 언리얼에서는 쓰고 있으니 간단히 설명한다.
	//  pInclude
	//      컴파일 시에 디펜던시를 설정해주는 옵션      
	//  pFunctionName
	//      진입점 이름 설정
	//  pProfile 셰이더 버전 설정
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
	//  Flags1 // 컴파일 옵션 플래그
	//      See D3D10_SHADER_xxx flags.
	//  Flags2 // 고급 기법 플레그
	//      See D3D10_EFFECT_xxx flags.
	//  ppShader // 비동기 컴파일 옵션이지만 안씀
	//      Returns a buffer containing the created shader.  This buffer contains
	//      the compiled shader code, as well as any embedded debug and symbol
	//      table info.  (See D3D10GetShaderConstantTable)
	//  ppErrorMsgs  오류 메시지 출력 구조체 리턴
	//      Returns a buffer containing a listing of errors and warnings that were
	//      encountered during the compile.  If you are running in a debugger,
	//      these are the same messages you will see in your debug output.
	//  pHResult // 비동기 컴파일시에 오류코드 조회
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

	// 컴파일 성공 이후, 해당 셰이더를 통해 효과 자체를 나타내는 객체 생성
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
	//      컴파일 된 이펙트 자료
	//  DataLength
	//      컴파일된 효과 자료의 바이트 단위 크기
	//  FXFlags
	//      효과 플래그들
	//      
	//  pDevice
	//      디바이스 장치를 가리키는 포인터
	//
	// [out]
	//
	//  ppEffect
	//      생성된 이펙트 파일을 돌려줌
	//
	//----------------------------------------------------------------------------
	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
		0, md3dDevice, &mFX));

	// Done with compiled shader.
	ReleaseCOM(compiledShader);

	mTech = mFX->GetTechniqueByName("ColorTech"); // 테크를 가져와서 저장
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix(); // 상수 매트릭스 바인딩을 위해 포인터 저장
}

void BoxApp::BuildVertexLayout()
{
	// 정점 구조체의 각성분이 어떤 용도인지 설정

	// 버텍스 시만텍 및 레이아웃 설정
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		// 시만텍 문자열 이름, 시만텍에 부여된 인덱스, 성분의 자료형식, 정점 버퍼슬롯의 색인, 오프셋 (포지션의 경우 0이지만, 컬러 성분은 12바이트 건너뛰어서 처리해야한다), 인스탄스인지 설정, 인스턴스 스텝 레이트)
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// 인풋 레이아웃 생성
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	// 입력배치 생성

	//virtual HRESULT STDMETHODCALLTYPE CreateInputLayout(
	//	/* [annotation] */ 정점 구조체를 서술하는 D3D11_INPUT_ELEMENT_DESC 배열
	//	__in_ecount(NumElements)  const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
	//	/* [annotation] */ 배열의 원소 개수
	//	__in_range(0, D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT)  UINT NumElements,
	//	/* [annotation] */ 정점 쉐이더를 컴파일해서 얻은 바이트코드를 가리키는 포인터
	//	__in  const void *pShaderBytecodeWithInputSignature,
	//	/* [annotation] */ 바이트 코드의 크기
	//	__in  SIZE_T BytecodeLength,
	//	/* [annotation] */ 생성된 입력 배치
	//	__out_opt  ID3D11InputLayout **ppInputLayout) = 0;

	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}