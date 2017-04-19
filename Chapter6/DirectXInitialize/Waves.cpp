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

	// �ʱ�ȭ�� �ٽúҷ������� ���
	delete[] mPrevSolution;
	delete[] mCurrSolution;
	delete[] mNormals;
	delete[] mTangentX;

	mPrevSolution = new XMFLOAT3[m*n];
	mCurrSolution = new XMFLOAT3[m*n];
	mNormals = new XMFLOAT3[m*n];
	mTangentX = new XMFLOAT3[m*n];

	// �ý��� �޸𸮿� �׸��� �������� �����Ѵ�.

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

	// �����ð�
	t += dt;

	// ������ �ð� ���ܶ��� ������Ʈ�Ѵ�.
	if( t >= mTimeStep )
	{
		// �ܰ� ��輱�� ó������ �ʴ´�.
		for(DWORD i = 1; i < mNumRows-1; ++i)
		{
			for(DWORD j = 1; j < mNumCols-1; ++j)
			{
				// �� ������Ʈ�� ���� �� ���Ŀ�, ������ ���� ���۸� �����ϰ�
				// ���ο� ���۷� ������Ʈ �մϴ�.
				// �츮�� ��� �� ��ҿ� ���ؼ� �б� ���⸦ �ϴ��� Ȯ���ϼ���.
				// �츮�� ���� ij�� �ٽ� ���� �ʽ��ϴ�.
				
				// j�� x, i �ε�������  z: h(x_j, z_i, t_k) �Դϴ�.
				// �����ٳ� �츮�� +z ���� �Ʒ��� ���մϴ�.
				// �̰� �츮�� �ε������� �ϰ����� ��Ű�� ���Ѱ��� ���Դϴ�.

				mPrevSolution[i*mNumCols+j].y = 
					mK1*mPrevSolution[i*mNumCols+j].y +
					mK2*mCurrSolution[i*mNumCols+j].y +
					mK3*(mCurrSolution[(i+1)*mNumCols+j].y + 
					     mCurrSolution[(i-1)*mNumCols+j].y + 
					     mCurrSolution[i*mNumCols+j+1].y + 
						 mCurrSolution[i*mNumCols+j-1].y);
			}
		}

		// �츮�� ��������Ÿ���� ���ο� ����Ÿ�� ��ü�Ұ��Դϴ�.
		// �� ����Ÿ���� ���� ����Ÿ�� ��������Ÿ�� �ٲ�ġ�� �ϴ� �͸����� ������ ó���Ҽ� �ֽ��ϴ�.
		std::swap(mPrevSolution, mCurrSolution);

		t = 0.0f; // �ð��� �����Ѵ�.

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
	// ���δ� �ߵ帮�� �ʴ´�.
	assert(i > 1 && i < mNumRows-2);
	assert(j > 1 && j < mNumCols-2);

	float halfMag = 0.5f*magnitude;

	// ij��° �������̿� �� �̿����� ��߸���.
	mCurrSolution[i*mNumCols+j].y     += magnitude;
	mCurrSolution[i*mNumCols+j+1].y   += halfMag;
	mCurrSolution[i*mNumCols+j-1].y   += halfMag;
	mCurrSolution[(i+1)*mNumCols+j].y += halfMag;
	mCurrSolution[(i-1)*mNumCols+j].y += halfMag;
}
	
