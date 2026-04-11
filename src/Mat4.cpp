#include "math/Mat4.hpp"

Mat4::Mat4()
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			data[i][j] = 0.0f;
		}
	}
}

Mat4	Mat4::identity(void)
{
	Mat4	m;

	m.data[0][0] = 1.0f;
	m.data[1][1] = 1.0f;
	m.data[2][2] = 1.0f;
	m.data[3][3] = 1.0f;

	return (m);
}

float const *	Mat4::value_ptr(void) const
{
	return (&data[0][0]);
}

Mat4	Mat4::operator*(const Mat4 &m) const
{
	Mat4	result;

	for (int col = 0; col < 4; ++col)
	{
		for (int row = 0; row < 4; ++row)
		{
			for (int k = 0; k < 4; ++k)
				result.data[col][row] += data[k][row] * m.data[col][k];
		}
	}

	return (result);
}

Vec4	Mat4::operator*(const Vec4 &v) const
{
	Vec4	result;

	result.x = data[0][0] * v.x + data[1][0] * v.y + data[2][0] * v.z + data[3][0] * v.w;
	result.y = data[0][1] * v.x + data[1][1] * v.y + data[2][1] * v.z + data[3][1] * v.w;
	result.z = data[0][2] * v.x + data[1][2] * v.y + data[2][2] * v.z + data[3][2] * v.w;
	result.w = data[0][3] * v.x + data[1][3] * v.y + data[2][3] * v.z + data[3][3] * v.w;

	return (result);
}

Mat4	Mat4::translate(const Mat4 &m, const Vec3 &v)
{
	Mat4	t = identity();

	t.data[3][0] = v.x;
	t.data[3][1] = v.y;
	t.data[3][2] = v.z;

	return (m * t);
}

Mat4	Mat4::rotate(const Mat4 &m, float angle, const Vec3 &axis)
{
	Vec3	a = axis.normalize();

	float	c = std::cos(angle);
	float	s = std::sin(angle);
	float	t = 1.0f - c;

	Mat4	r = identity();

	r.data[0][0] = t * a.x * a.x + c;
	r.data[0][1] = t * a.x * a.y + s * a.z;
	r.data[0][2] = t * a.x * a.z - s * a.y;

	r.data[1][0] = t * a.x * a.y - s * a.z;
	r.data[1][1] = t * a.y * a.y + c;
	r.data[1][2] = t * a.y * a.z + s * a.x;

	r.data[2][0] = t * a.x * a.z + s * a.y;
	r.data[2][1] = t * a.y * a.z - s * a.x;
	r.data[2][2] = t * a.z * a.z + c;

	return (m * r);
}

Mat4	Mat4::perspective(float fov, float aspect, float near, float far)
{
	Mat4	m;

	float	f = 1.0f / std::tan(fov / 2.0f);

	m.data[0][0] = f / aspect;
	m.data[1][1] = f;
	m.data[2][2] = (far + near) / (near - far);
	m.data[2][3] = -1.0f;
	m.data[3][2] = (2.0f * far * near) / (near - far);

	return (m);
}

Mat4	Mat4::lookAt(const Vec3 &eye, const Vec3 &center, const Vec3 &up)
{
	Vec3	forward = (center - eye).normalize();
	Vec3	right = Vec3::cross(forward, up).normalize();
	Vec3	camUp = Vec3::cross(right, forward);

	Mat4	m = identity();

	m.data[0][0] = right.x;
	m.data[1][0] = right.y;
	m.data[2][0] = right.z;
	m.data[3][0] = -Vec3::dot(right, eye);

	m.data[0][1] = camUp.x;
	m.data[1][1] = camUp.y;
	m.data[2][1] = camUp.z;
	m.data[3][1] = -Vec3::dot(camUp, eye);

	m.data[0][2] = -forward.x;
	m.data[1][2] = -forward.y;
	m.data[2][2] = -forward.z;
	m.data[3][2] = Vec3::dot(forward, eye);

	return (m);
}
