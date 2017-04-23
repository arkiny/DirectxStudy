#pragma once
class ShapesDemo : public D3DApp
{
public:
	ShapesDemo(HINSTANCE hinst);
	~ShapesDemo();

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


	UINT mGridIndexCount;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	XMFLOAT4X4 mSphereWorld[10];
	XMFLOAT4X4 mCylWorld[10];
	XMFLOAT4X4 mBoxWorld;
	XMFLOAT4X4 mGridWorld;
	XMFLOAT4X4 mCenterSphere;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;

	int mBoxVertexOffset = 0;
	int mGridVertexOffset = 0;
	int mSphereVertexOffset = 0;
	int mCylinderVertexOffset = 0;

	UINT mBoxIndexCount = 0;
//	UINT mGridIndexCount = 0;
	UINT mSphereIndexCount = 0;
	UINT mCylinderIndexCount = 0;

	UINT mBoxIndexOffset = 0;
	UINT mGridIndexOffset = 0;
	UINT mSphereIndexOffset = 0;
	UINT mCylinderIndexOffset = 0;
};

