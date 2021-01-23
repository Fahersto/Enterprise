#pragma once
#include "Core.h"

namespace Enterprise::Math
{

/// A 2D vector.
struct Vec2
{
	float x;
	float y;
};

/// A 3D vector.
struct Vec3
{
	float x;
	float y;
	float z;
};

/// A 4D vector.
struct Vec4
{
	float x;
	float y;
	float z;
	float w;
};

/// A 3x3 Matrix.
struct Mat3
{
	float data[9];

	Mat3 operator+(Mat3 other)
	{
		Mat3 returnVal;
		for (uint_fast8_t i = 0; i < 9; i++)
		{
			returnVal.data[i] = data[i] + other.data[i];
		}
		return returnVal;
	}
	Mat3 operator+(Mat3& other)
	{
		Mat3 returnVal;
		for (uint_fast8_t i = 0; i < 9; i++)
		{
			returnVal.data[i] = data[i] + other.data[i];
		}
		return returnVal;
	}
	Mat3 operator-(Mat3 other)
	{
		Mat3 returnVal;
		for (uint_fast8_t i = 0; i < 9; i++)
		{
			returnVal.data[i] = data[i] - other.data[i];
		}
		return returnVal;
	}
	Mat3 operator-(Mat3& other)
	{
		Mat3 returnVal;
		for (uint_fast8_t i = 0; i < 9; i++)
		{
			returnVal.data[i] = data[i] - other.data[i];
		}
		return returnVal;
	}
	Mat3 operator*(Mat3 other)
	{
		Mat3 returnVal;
		for (uint_fast8_t i = 0; i < 3; i++)
		{
			for (uint_fast8_t j = 0; j < 3; j++)
			{
				returnVal.data[i * 3 + j] =
					data[i * 3] * other.data[j] +
					data[i * 3 + 1] * other.data[j + 3] +
					data[i * 3 + 2] * other.data[j + 6];
			}
		}
		return returnVal;
	}
	Mat3 operator*(Mat3& other)
	{
		Mat3 returnVal;
		for (uint_fast8_t i = 0; i < 3; i++)
		{
			for (uint_fast8_t j = 0; j < 3; j++)
			{
				returnVal.data[i * 3 + j] =
					data[i * 3] * other.data[j] +
					data[i * 3 + 1] * other.data[j + 3] +
					data[i * 3 + 2] * other.data[j + 6];
			}
		}
		return returnVal;
	}
};

/// A 4x4 Matrix.
struct Mat4
{
	float data[16];

	Mat4 operator+(Mat4 other)
	{
		Mat4 returnVal;
		for (uint_fast8_t i = 0; i < 16; i++)
		{
			returnVal.data[i] = data[i] + other.data[i];
		}
		return returnVal;
	}
	Mat4 operator+(Mat4& other)
	{
		Mat4 returnVal;
		for (uint_fast8_t i = 0; i < 16; i++)
		{
			returnVal.data[i] = data[i] + other.data[i];
		}
		return returnVal;
	}
	Mat4 operator-(Mat4 other)
	{
		Mat4 returnVal;
		for (uint_fast8_t i = 0; i < 16; i++)
		{
			returnVal.data[i] = data[i] - other.data[i];
		}
		return returnVal;
	}
	Mat4 operator-(Mat4& other)
	{
		Mat4 returnVal;
		for (uint_fast8_t i = 0; i < 16; i++)
		{
			returnVal.data[i] = data[i] - other.data[i];
		}
		return returnVal;
	}
	Mat4 operator*(Mat4 other)
	{
		Mat4 returnVal;
		for (uint_fast8_t i = 0; i < 4; i++)
		{
			for (uint_fast8_t j = 0; j < 4; j++)
			{
				returnVal.data[i * 4 + j] =
					data[i * 4]     * other.data[j] +
					data[i * 4 + 1] * other.data[j + 4] +
					data[i * 4 + 2] * other.data[j + 8] +
					data[i * 4 + 3] * other.data[j + 12];
			}
		}
		return returnVal;
	}
	Mat4 operator*(Mat4& other)
	{
		Mat4 returnVal;
		for (uint_fast8_t i = 0; i < 4; i++)
		{
			for (uint_fast8_t j = 0; j < 4; j++)
			{
				returnVal.data[i * 4 + j] =
					data[i * 4]     * other.data[j] +
					data[i * 4 + 1] * other.data[j + 4] +
					data[i * 4 + 2] * other.data[j + 8] +
					data[i * 4 + 3] * other.data[j + 12];
			}
		}
		return returnVal;
	}
};

/// Standard matrices
/// TODO: Would these work better as static methods of the Mat3/Mat4 classes?
namespace Matrices
{
Mat3 Identity3();
Mat4 Identity4();

Mat4 Translation(float x, float y, float z);
Mat4 Translation(Vec3 trans);
Mat4 Rotation(float roll, float pitch, float yaw);
Mat4 Rotation(Vec3 rot);
Mat4 Scale(float scale);
Mat4 Scale(float x, float y, float z);
Mat4 Scale(Vec3 scale);

Mat4 Orthographic(float left, float right, float bottom, float top, float nearClip, float farClip);
Mat4 Frustrum(float left, float right, float bottom, float top, float nearClip, float farClip);
Mat4 Perspective(float fov, float aspectRatio, float nearClip, float farClip);
}

}
