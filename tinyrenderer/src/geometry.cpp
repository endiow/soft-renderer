#include <vector>
#include <cassert>
#include <cmath>
#include <iostream>
#include "geometry.h"

Mat3f::Mat3f()
{
}

Mat3f Mat3f::operator*(Mat3f& a)
{
	Mat3f result;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			result[i][j] = 0.0f;
			for (int k = 0; k < 3; k++)
			{
				result[i][j] += rows[i][k] * a.rows[k][j];
			}
		}
	}
	return result;
}

Vec3f Mat3f::operator*(Vec3f& a)
{
	Vec3f result;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 1; j++)
		{
			result[i] = 0.0f;
			for (int k = 0; k < 3; k++)
			{
				result[i] += rows[i][k] * a[k];
			}
		}
	}
	return result;
}

Mat3f Mat3f::transpose()
{
	Mat3f result;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			result[i][j] = rows[j][i];
		}
	return result;
}

Mat3f Mat3f::inverse()
{
	return Mat3f::identity();
}

Mat3f Mat3f::identity()
{
	Mat3f E;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			E[i][j] = (i == j ? 1.0f : 0.0f);
		}
	return E;
}

std::ostream& operator<<(std::ostream& s, Mat3f& m)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			s << m[i][j];
			if (j < 2) s << "\t";
		}
		s << "\n";
	}
	return s;
}


Mat4f::Mat4f()
{
}

Mat4f Mat4f::operator*(Mat4f& a)
{
	Mat4f result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i][j] = 0.0f;
			for (int k = 0; k < 4; k++)
			{
				result[i][j] += rows[i][k] * a.rows[k][j];
			}
		}
	}
	return result;
}

Vec4f Mat4f::operator*(Vec4f& a)
{
	Vec4f result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 1; j++)
		{
			result[i] = 0.0f;
			for (int k = 0; k < 4; k++)
			{
				result[i] += rows[i][k] * a[k];
			}
		}
	}
	return result;
}

Mat4f Mat4f::transpose()
{
	Mat4f result;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			result[i][j] = rows[j][i];
		}
	return result;
}

Mat4f Mat4f::inverse()
{
	return Mat4f::identity();
}

Mat4f Mat4f::identity()
{
	Mat4f E;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			E[i][j] = (i == j ? 1.0f : 0.0f);
		}
	return E;
}

std::ostream& operator<<(std::ostream& s, Mat4f& m)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			s << m[i][j];
			if (j < 3) s << "\t";
		}
		s << "\n";
	}
	return s;
}