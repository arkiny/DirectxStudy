#pragma once

class WaveApp :
	public D3DApp
{
public:
	WaveApp(HINSTANCE hInstance);
	~WaveApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	float GetHeight(float x, float z)const;
	void BuildLandGeometryBuffers();
	void BuildWavesGeometryBuffers();
	void BuildFX();
	void BuildVertexLayout();

private:
	ID3D11Buffer* mLandVB = nullptr;
	ID3D11Buffer* mLandIB = nullptr;
	ID3D11Buffer* mWavesVB = nullptr;
	ID3D11Buffer* mWavesIB = nullptr;
	class Waves* pWaves = nullptr;

	ID3DX11Effect* mFX = nullptr;
	ID3DX11EffectTechnique* mTech = nullptr;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj = nullptr;

	ID3D11InputLayout* mInputLayout = nullptr;
	ID3D11RasterizerState* mWireframeRS = nullptr;

	XMFLOAT4X4 mGridWorld;
	XMFLOAT4X4 mWavesWorld;

	UINT mGridIndexCount = 0;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

