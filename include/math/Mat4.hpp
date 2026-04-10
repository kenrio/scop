#ifndef MAT4_HPP
# define MAT4_HPP

#include "Vec3.hpp"
#include "Vec4.hpp"

struct Mat4
{
	float	data[4][4];

	Mat4();
	static	Mat4	identity(void);

	Mat4	operator*(const Mat4 &m) const;
	Vec4	operator*(const Vec4 &v) const;

	float *	value_ptr(void);

	static Mat4	translate(const Mat4 &m, const Vec3 &v);
	static Mat4	rotate(const Mat4 &m, float angle, const Vec3 &axis);
	static Mat4	perspective(float fov, float aspect, float near, float far);
	static Mat4	lookAt(const Vec3 &eye, const Vec3 &center, const Vec3 &up);
};


#endif
