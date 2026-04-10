#include "Vec3.hpp"

Vec3::Vec3() : x(0), y(0), z(0) {}

Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

Vec3	Vec3::operator+(const Vec3 &v) const
{
	return (Vec3(x + v.x, y + v.y, z + v.z));
}

Vec3	Vec3::operator-(const Vec3 &v) const
{
	return (Vec3(x - v.x, y - v.y, z - v.z));
}

Vec3	Vec3::operator*(float scalar) const
{
	return (Vec3(x * scalar, y * scalar, z * scalar));
}

float	Vec3::length(void) const
{
	return (std::sqrt(x * x + y * y + z * z));
}

Vec3	Vec3::normalize(void) const
{
	float	len = length();

	if (len == 0)
		return (Vec3());

	return (Vec3(x / len, y / len, z / len));
}

float	Vec3::dot(const Vec3 &a, const Vec3 &b)
{
	return
	(
		a.x * b.x +
		a.y * b.y +
		a.z * b.z
	);
}

Vec3	Vec3::cross(const Vec3 &a, const Vec3 &b)
{
	return
	(
		Vec3
		(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		)
	);
}
