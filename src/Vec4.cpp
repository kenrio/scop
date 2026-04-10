#include "Vec4.hpp"

Vec4::Vec4() : x(0), y(0), z(0), w(0) {}

Vec4::Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

Vec4::Vec4(const Vec3 &v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}

Vec4	Vec4::operator+(const Vec4 &v) const
{
	return (Vec4(x + v.x, y + v.y, z + v.z, w + v.w));
}

Vec4	Vec4::operator-(const Vec4 &v) const
{
	return (Vec4(x - v.x, y - v.y, z - v.z, w - v.w));
}

Vec4	Vec4::operator*(float scalar) const
{
	return (Vec4(x * scalar, y * scalar, z * scalar, w * scalar));
}

float	Vec4::dot(const Vec4 &a, const Vec4 &b)
{
	return
	(
		a.x * b.x +
		a.y * b.y +
		a.z * b.z +
		a.w * b.w
	);
}
