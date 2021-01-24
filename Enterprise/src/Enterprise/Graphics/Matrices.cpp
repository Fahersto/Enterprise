#include "EP_PCH.h"
#include "Math.h"

namespace Enterprise::Math
{

Mat3 Mat3::Identity()
{
	return
	{
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
}

Mat4 Mat4::Identity()
{
	return
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Mat4 Mat4::Translation(float x, float y, float z)
{
	return
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		x,    y,    z,    1.0f
	};
}
Mat4 Mat4::Translation(Vec3 trans)
{
	return
	{
		1.0f,    0.0f,    0.0f,    0.0f,
		0.0f,    1.0f,    0.0f,    0.0f,
		0.0f,    0.0f,    1.0f,    0.0f,
		trans.x, trans.y, trans.z, 1.0f
	};
}
Mat4 Mat4::Rotation(float roll, float pitch, float yaw)
{
	return
	{
		cos(roll) * cos(yaw), cos(roll) * sin(yaw) * sin(pitch) - sin(roll) * cos(pitch), cos(roll) * sin(yaw) * cos(pitch) + sin(roll) * sin(pitch), 0.0f,
		sin(roll) * cos(yaw), sin(roll) * sin(yaw) * sin(pitch) + cos(roll) * cos(pitch), sin(roll) * sin(yaw) * cos(pitch) - cos(roll) * sin(pitch), 0.0f,
		-sin(yaw),            cos(yaw) * sin(pitch),                                      cos(yaw) * cos(pitch),                                      0.0f,
		0.0f,                 0.0f,                                                       0.0f,                                                       1.0f
	};
}
Mat4 Mat4::Scale(float scale)
{
	return
	{
		scale, 0.0f, 0.0f, 0.0f,
		0.0f, scale, 0.0f, 0.0f,
		0.0f, 0.0f, scale, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}
Mat4 Mat4::Scale(float x, float y, float z)
{
	return
	{
		x, 0.0f, 0.0f, 0.0f,
		0.0f, y, 0.0f, 0.0f,
		0.0f, 0.0f, z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}
Mat4 Mat4::Scale(Vec3 scale)
{
	return
	{
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f,    1.0f
	};
}

Mat4 Mat4::Orthographic(float left, float right, float bottom, float top, float nearClip, float farClip)
{
	return
	{
		2.0f / (right - left),             0.0f,                             0.0f,                                        0.0f,
		0.0f,                              2.0f / (top - bottom),            0.0f,                                        0.0f,
		0.0f,                              0.0f,                             2.0f / (farClip - nearClip),                 0.0f,
		-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(farClip - nearClip) / (farClip - nearClip), 1.0f
	};
}
Mat4 Mat4::Frustrum(float left, float right, float bottom, float top, float nearClip, float farClip)
{
	return
	{
		2.0f * nearClip / (right - left), 0.0f,                             0.0f,                                             0.0f,
		0.0f,                             2.0f * nearClip / (top - bottom), 0.0f,                                             0.0f,
		-(right + left) / (right - left), -(top + bottom) / (top - bottom), (farClip + nearClip) / (farClip - nearClip),      1.0f,
		0.0f,                             0.0f,                            -2.0f * farClip * nearClip / (farClip - nearClip), 0.0f
	};
}
Mat4 Mat4::Perspective(float fov, float aspectRatio, float nearClip, float farClip)
{
	float top = tan(fov / 2) * nearClip;
	float bottom = -top;
	float right = top * aspectRatio;
	float left = -right;

	return Frustrum(left, right, bottom, top, nearClip, farClip);
}

}
