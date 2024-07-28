#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <vector>

template <class t>
class Vec2 {
public:
	t x, y;
	Vec2<t>() :x(t()), y(t()) {}
	Vec2<t>(t _x, t _y) : x(_x), y(_y) {}
	Vec2<t>(const Vec2<t>& v) { *this = v; }
	Vec2<t>& operator=(const Vec2<t>& v)
	{
		if (this != &v) {
			this->x = v.x;
			this->y = v.y;
		}
		return *this;
	}
	inline Vec2<t> operator +(const Vec2<t>& V) const { return Vec2<t>(x + V.x, y + V.y); }//向量加法
	inline Vec2<t> operator -(const Vec2<t>& V) const { return Vec2<t>(x - V.x, y - V.y); }//向量减法
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(x * f, y * f); }//向量的数乘运算

	inline t& operator[](const int idx) { if (idx <= 0) return x; else return y; }

	float norm() const { return std::sqrt(x * x + y * y); }//向量的模长
	Vec2<t>& normalize(t l = 1) { *this = (*this) * (l / norm()); return *this; }//向量归一化

	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);//向量输出
};

template <class t>
std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

//模板特化
typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;

template <class t>
struct Vec3 {
	t x, y, z;
	Vec3<t>() :x(t()), y(t()), z(t()) {}
	Vec3<t>(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
	Vec3<t>(const Vec3<t>& v) { *this = v; }
	Vec3<t>& operator=(const Vec3<t>& v)
	{
		if (this != &v) {
			this->x = v.x;
			this->y = v.y;
			this->z = v.z;
		}
		return *this;
	}
	inline Vec3<t> operator +(const Vec3<t>& V) const { return Vec3<t>(x + V.x, y + V.y, z + V.z); }//向量加法
	inline Vec3<t> operator -(const Vec3<t>& V) const { return Vec3<t>(x - V.x, y - V.y, z - V.z); }//向量减法
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x * f, y * f, z * f); }//向量的数乘运算
	inline Vec3<t> operator ^(const Vec3<t>& v) const { return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }//向量叉乘运算（外积）
	inline t       operator *(const Vec3<t>& v) const { return x * v.x + y * v.y + z * v.z; }//向量的点乘（内积）

	float norm() const { return std::sqrt(x * x + y * y + z * z); }//向量的模长
	Vec3<t>& normalize(t l = 1) { *this = (*this) * (l / norm()); return *this; }//向量归一化

	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);//向量输出
	inline t& operator[](const int idx) { if (idx <= 0) return x; else if (idx == 1) return y; else return z; }
};

template <class t>
std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

//模板特化
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template <class t>
struct Vec4
{
	t x, y, z, w;
	Vec4<t>() :x(t()), y(t()), z(t()), w(t()) {}
	Vec4<t>(t _x, t _y, t _z, t _w) : x(_x), y(_y), z(_z), w(_w) {}
	Vec4<t>(const Vec4<t>& v) { *this = v; }
	Vec4<t>& operator=(const Vec4<t>& v)
	{
		if (this != &v) {
			this->x = v.x;
			this->y = v.y;
			this->z = v.z;
			this->w = v.w;
		}
		return *this;
	}
	inline Vec4<t> operator +(const Vec4<t>& V) const { return Vec4<t>(x + V.x, y + V.y, z + V.z, w + V.w); }//向量加法
	inline Vec4<t> operator -(const Vec4<t>& V) const { return Vec4<t>(x - V.x, y - V.y, z - V.z, w - V.w); }//向量减法
	inline Vec4<t> operator *(float f)          const { return Vec4<t>(x * f, y * f, z * f, w * f); }//向量的数乘运算
	inline t       operator *(const Vec4<t>& v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }//向量的点乘（内积）

	float norm() const { return std::sqrt(x * x + y * y + z * z + w * w); }//向量的模长
	Vec4<t>& normalize(t l = 1) { *this = (*this) * (l / norm()); return *this; }//向量归一化

	template <class > friend std::ostream& operator<<(std::ostream& s, Vec4<t>& v);//向量输出
	inline t& operator[](const int idx) { if (idx <= 0) return x; else if (idx == 1) return y; else if (idx == 2) return z; else return w; }
};

template <class t>
std::ostream& operator<<(std::ostream& s, Vec4<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")\n";
	return s;
}

//模板特化
typedef Vec4<float> Vec4f;
typedef Vec4<int>   Vec4i;

class Mat3f
{
public:
	Vec3f rows[3];

	Mat3f();

	Vec3f& operator[](const int i) { return rows[i]; }
	Vec3f operator[](const int i) const { return rows[i]; }

	Mat3f operator*(Mat3f& a);
	Vec3f operator*(Vec3f& a);

	Mat3f transpose();
	Mat3f inverse();

	static Mat3f identity();

	friend std::ostream& operator<<(std::ostream& s, Mat3f& m);
};


class Mat4f
{
public:
	Vec4f rows[4];

	Mat4f();

	Vec4f& operator[](const int i) { return rows[i]; }
	Vec4f operator[](const int i) const { return rows[i]; }

	Mat4f operator*(Mat4f& a);
	Vec4f operator*(Vec4f& a);

	Mat4f transpose();
	Mat4f inverse();

	static Mat4f identity();

	friend std::ostream& operator<<(std::ostream& s, Mat4f& m);
};

#endif //__GEOMETRY_H__