#ifndef VEC3_HPP
# define VEC3_HPP

#include <cmath>

struct Vec3
{
	float	x;
	float	y;
	float	z;

	Vec3();
	Vec3(float x, float y, float z);

	Vec3	operator+(const Vec3 &v) const;
	Vec3	operator-(const Vec3 &v) const;
	Vec3	operator*(float scalar) const;

	float	length(void) const;
	Vec3	normalize(void) const;

	static float	dot(const Vec3 &a, const Vec3 &b);
	static Vec3		cross(const Vec3 &a, const Vec3 &b);
};

#endif
