#pragma once
#include "Core.h"

namespace Enterprise::Math
{

/// A 2D vector.
struct Vec2
{
	float X;
	float Y;
};

/// A 3D vector.
struct Vec3
{
	union
	{
		float X;
		float Red;
		float Roll;
	};
	union
	{
		float Y;
		float Green;
		float Pitch;
	};
	union
	{
		float Z;
		float Blue;
		float Yaw;
	};
};

/// A 4D vector.
struct Vec4
{
	union
	{
		float X;
		float Red;
	};
	union
	{
		float Y;
		float Green;
	};
	union
	{
		float Z;
		float Blue;
	};
	union
	{
		float W;
		float Alpha;
	};
};

/// A 3x3 Matrix.
struct Matrix3
{
	float c1[3];
	float c2[3];
	float c3[3];
};

/// A 4x4 Matrix.
struct Matrix4
{
	float c1[4];
	float c2[4];
	float c3[4];
	float c4[4];
};

/// Standard matrices lookup/generation.
namespace Matrices
{
static Matrix4 Identity();
static Matrix4 Orthographic(float left, float right, float top, float bottom, float near, float far);
static Matrix4 Perspective(float fov, float aspectRatio, float near, float far);
static Matrix4 PixelSpace();
}

}
