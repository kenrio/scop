#ifndef VEC4_HPP
# define VEC4_HPP

#include <cmath>
#include "Vec3.hpp"

struct Vec4
{
	float	x;
	float	y;
	float	z;
	float	w;

	Vec4();
	Vec4(float x, float y, float z, float w);
	Vec4(const Vec3 &v, float w);

	Vec4	operator+(const Vec4 &v) const;
	Vec4	operator-(const Vec4 &v) const;
	Vec4	operator*(float scalar) const;

	static float	dot(const Vec4 &a, const Vec4 &b);
};


#endif
