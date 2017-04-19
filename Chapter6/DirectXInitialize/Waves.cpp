#include "stdafx.h"
#include "Waves.h"
#include <algorithm>
#include <vector>
#include <cassert>

Waves::Waves()
: mNumRows(0), mNumCols(0), mVertexCount(0), mTriangleCount(0), 
  mK1(0.0f), mK2(0.0f), mK3(0.0f), mTimeStep(0.0f), mSpatialStep(0.0f),
  mPrevSolution(0), mCurrSolution(0), mNormals(0), mTangentX(0)
{
}

Waves::~Waves()
{
	delete[] mPrevSolution;
	delete[] mCurrSolution;
	delete[] mNormals;
	delete[] mTangentX;
}

UINT Waves::RowCount()const
{
	return mNumRows;
}

UINT Waves::ColumnCount()const
{
	return mNumCols;
}

UINT Waves::VertexCount()const
{
	return mVertexCount;
}

UINT Waves::TriangleCount()const
{
	return mTriangleCount;
}

void Waves::Init(UINT m, UINT n, float dx, float dt, float speed, float damping)
{
	mNumRows  = m;
	mNumCols  = n;

	mVertexCount   = m*n;
	mTriangleCount = (m-1)*(n-1)*2;

	mTimeStep    = dt;
	mSpatialStep = dx;

	float d = damping*dt+2.0f;
	float e = (speed*speed)*(dt*dt)/(dx*dx);
	mK1     = (damping*dt-2.0f)/ d;
	mK2     = (4.0f-8.0f*e) / d;
	mK3     = (2.0f*e) / d;

	// 초기화가 다시불렸을때를 대비
	delete[] mPrevSolution;
	delete[] mCurrSolution;
	delete[] mNormals;
	delete[] mTangentX;

	mPrevSolution = new XMFLOAT3[m*n];
	mCurrSolution = new XMFLOAT3[m*n];
	mNormals = new XMFLOAT3[m*n];
	mTangentX = new XMFLOAT3[m*n];

	// 시스템 메모리에 그리드 정점들을 생성한다.

	float halfWidth = (n-1)*dx*0.5f;
	float halfDepth = (m-1)*dx*0.5f;
	for(UINT i = 0; i < m; ++i)
	{
		float z = halfDepth - i*dx;
		for(UINT j = 0; j < n; ++j)
		{
			float x = -halfWidth + j*dx;

			mPrevSolution[i*n+j] = XMFLOAT3(x, 0.0f, z);
			mCurrSolution[i*n+j] = XMFLOAT3(x, 0.0f, z);
			mNormals[i*n + j] = XMFLOAT3(0.0f, 1.0f, 0.0f);
			mTangentX[i*n + j] = XMFLOAT3(1.0f, 0.0f, 0.0f);
		}
	}
}

void Waves::Update(float dt)
{
	static float t = 0;

	// 누적시간
	t += dt;

	// 적절한 시간 스텝때만 업데이트한다.
	if( t >= mTimeStep )
	{
		// 외곽 경계선은 처리하지 않는다.
		for(DWORD i = 1; i < mNumRows-1; ++i)
		{
			for(DWORD j = 1; j < mNumCols-1; ++j)
			{
				// 이 업데이트가 실행 된 이후에, 오래된 이전 버퍼를 무시하고
				// 새로운 버퍼로 업데이트 합니다.
				// 우리가 어떻게 한 요소에 대해서 읽기 쓰기를 하는지 확인하세요.
				// 우리는 이전 ij를 다시 쓰지 않습니다.
				
				// j와 x, i 인덱스들은  z: h(x_j, z_i, t_k) 입니다.
				// 더군다나 우리의 +z 축은 아래로 향합니다.
				// 이건 우리의 인덱스들의 일관성을 지키기 위한것일 뿐입니다.

				mPrevSolution[i*mNumCols+j].y = 
					mK1*mPrevSolution[i*mNumCols+j].y +
					mK2*mCurrSolution[i*mNumCols+j].y +
					mK3*(mCurrSolution[(i+1)*mNumCols+j].y + 
					     mCurrSolution[(i-1)*mNumCols+j].y + 
					     mCurrSolution[i*mNumCols+j+1].y + 
						 mCurrSolution[i*mNumCols+j-1].y);
			}
		}

		// 우리는 이전데이타들을 새로운 데이타로 고체할것입니다.
		// 이 데이타들은 현재 데이타와 이전데이타를 바꿔치기 하는 것만으로 간단히 처리할수 있습니다.
		std::swap(mPrevSolution, mCurrSolution);

		t = 0.0f; // 시간을 리셋한다.

				  //
				  // Compute normals using finite difference scheme.
				  //
		for (UINT i = 1; i < mNumRows - 1; ++i)
		{
			for (UINT j = 1; j < mNumCols - 1; ++j)
			{
				float l = mCurrSolution[i*mNumCols + j - 1].y;
				float r = mCurrSolution[i*mNumCols + j + 1].y;
				float t = mCurrSolution[(i - 1)*mNumCols + j].y;
				float b = mCurrSolution[(i + 1)*mNumCols + j].y;
				mNormals[i*mNumCols + j].x = -r + l;
				mNormals[i*mNumCols + j].y = 2.0f*mSpatialStep;
				mNormals[i*mNumCols + j].z = b - t;

				XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&mNormals[i*mNumCols + j]));
				XMStoreFloat3(&mNormals[i*mNumCols + j], n);

				mTangentX[i*mNumCols + j] = XMFLOAT3(2.0f*mSpatialStep, r - l, 0.0f);
				XMVECTOR T = XMVector3Normalize(XMLoadFloat3(&mTangentX[i*mNumCols + j]));
				XMStoreFloat3(&mTangentX[i*mNumCols + j], T);
			}
		}
	}
}

void Waves::Disturb(UINT i, UINT j, float magnitude)
{
	// 경계부는 견드리지 않는다.
	assert(i > 1 && i < mNumRows-2);
	assert(j > 1 && j < mNumCols-2);

	float halfMag = 0.5f*magnitude;

	// ij번째 정점높이와 그 이웃들을 흩뜨린다.
	mCurrSolution[i*mNumCols+j].y     += magnitude;
	mCurrSolution[i*mNumCols+j+1].y   += halfMag;
	mCurrSolution[i*mNumCols+j-1].y   += halfMag;
	mCurrSolution[(i+1)*mNumCols+j].y += halfMag;
	mCurrSolution[(i-1)*mNumCols+j].y += halfMag;
}
	
