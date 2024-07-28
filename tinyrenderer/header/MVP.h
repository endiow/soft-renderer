#pragma once

#include "geometry.h"

class MVP
{
public:
	MVP(Mat4f m, Mat4f v, Mat4f p);
	~MVP();

private:
	Mat4f model_, view_, projection;
	Mat4f screenCoords;
};

MVP::MVP(Mat4f m, Mat4f v, Mat4f p)
{
	model_ = m;
	view_ = v;
	projection = p;
}

MVP::~MVP()
{
}

Matrix local2homo(Vec3f v)
{
	Matrix m(4, 1);
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = 1.0f;
	return m;
}