#pragma once

class cVector
{
public:
	cVector();
	cVector(float x, float y, float z);
	~cVector();

	cVector operator+(cVector& v);
	cVector operator-(cVector& v);

	// 
	bool operator==(cVector& v);

	bool equal(cVector& in);
	
	cVector add(cVector& in);
	cVector sub(cVector& in);
	cVector mul(float scalar);

	cVector normalized();
	float length();

	float dot(cVector& in);
	cVector cross(cVector& in);

	void print();

private:
	float x, y, z;
};