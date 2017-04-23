#pragma once

namespace Vertex
{
	struct PosNormal
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
	};

	// Basic 32-byte vertex structure.
	struct Basic32
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
	};
}

class InputLayoutDesc
{
public:
	static const D3D11_INPUT_ELEMENT_DESC PosNormal[2];
	static const D3D11_INPUT_ELEMENT_DESC Basic32[3];
};

class InputLayouts
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11InputLayout* PosNormal;
	static ID3D11InputLayout* Basic32;
};