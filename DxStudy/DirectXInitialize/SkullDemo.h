#pragma once

class SkullDemo :
	public D3DApp
{
public:
	SkullDemo(HINSTANCE hinst);
	~SkullDemo();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildGeometryBuffers();
	void BuildFX();
	void BuildVertexLayout();

private:
	ID3D11Buffer* mVB = nullptr;
	ID3D11Buffer* mIB = nullptr;

	ID3DX11Effect* mFX = nullptr;
	ID3DX11EffectTechnique* mTech = nullptr;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj = nullptr;

	ID3D11InputLayout* mInputLayout = nullptr;
	ID3D11RasterizerState* mWireframeRS = nullptr;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	XMFLOAT4X4 mSkullWorld;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
	UINT mSkullIndexCount = 0;
};

