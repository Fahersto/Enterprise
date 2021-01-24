#pragma once
#include "Core.h"

namespace Enterprise::Math
{

struct Mat3;
struct Mat4;

/// A 2D vector.
struct Vec2
{
	float x;
	float y;

	Vec2& operator+=(const Vec2& other);
	Vec2& operator+=(Vec2&& other);
	Vec2& operator-=(const Vec2& other);
	Vec2& operator-=(Vec2&& other);
	Vec2& operator*=(const Vec2& other);
	Vec2& operator*=(Vec2&& other);
	Vec2& operator/=(const Vec2& other);
	Vec2& operator/=(Vec2&& other);

	Vec2& operator*=(float other);
	Vec2& operator/=(float other);

	Vec2& operator*=(Mat3&& other);
	Vec2& operator*=(const Mat3& other);
	Vec2& operator*=(Mat4&& other);
	Vec2& operator*=(const Mat4& other);
};

Vec2 operator+(const Vec2& lhs, const Vec2& rhs);
Vec2 operator+(const Vec2& lhs, Vec2&& rhs);
Vec2 operator-(const Vec2& lhs, const Vec2& rhs);
Vec2 operator-(const Vec2& lhs, Vec2&& rhs);
Vec2 operator*(const Vec2& lhs, const Vec2& rhs);
Vec2 operator*(const Vec2& lhs, Vec2&& rhs);
Vec2 operator/(const Vec2& lhs, const Vec2& rhs);
Vec2 operator/(const Vec2& lhs, Vec2&& rhs);

Vec2 operator+(Vec2&& lhs, const Vec2& rhs);
Vec2 operator+(Vec2&& lhs, Vec2&& rhs);
Vec2 operator-(Vec2&& lhs, const Vec2& rhs);
Vec2 operator-(Vec2&& lhs, Vec2&& rhs);
Vec2 operator*(Vec2&& lhs, const Vec2& rhs);
Vec2 operator*(Vec2&& lhs, Vec2&& rhs);
Vec2 operator/(Vec2&& lhs, const Vec2& rhs);
Vec2 operator/(Vec2&& lhs, Vec2&& rhs);

Vec2 operator*(const Vec2& lhs, float rhs);
Vec2 operator/(const Vec2& lhs, float rhs);
Vec2 operator*(Vec2&& lhs, float rhs);
Vec2 operator/(Vec2&& lhs, float rhs);

Vec2 operator*(const Vec2& lhs, const Mat3& rhs);
Vec2 operator*(const Vec2& lhs, Mat3&& rhs);
Vec2 operator*(const Vec2& lhs, const Mat4& rhs);
Vec2 operator*(const Vec2& lhs, Mat4&& rhs);

Vec2 operator*(Vec2&& lhs, const Mat3& rhs);
Vec2 operator*(Vec2&& lhs, Mat3&& rhs);
Vec2 operator*(Vec2&& lhs, const Mat4& rhs);
Vec2 operator*(Vec2&& lhs, Mat4&& rhs);

Vec2 operator*(float lhs, const Vec2& rhs);
Vec2 operator*(float lhs, Vec2&& rhs);

/// A 3D vector.
struct Vec3
{
	float x;
	float y;
	float z;

	Vec3& operator+=(const Vec3& other);
	Vec3& operator+=(Vec3&& other);
	Vec3& operator-=(const Vec3& other);
	Vec3& operator-=(Vec3&& other);
	Vec3& operator*=(const Vec3& other);
	Vec3& operator*=(Vec3&& other);
	Vec3& operator/=(const Vec3& other);
	Vec3& operator/=(Vec3&& other);

	Vec3& operator*=(float other);
	Vec3& operator/=(float other);

	Vec3& operator*=(const Mat3& other);
	Vec3& operator*=(Mat3&& other);
	Vec3& operator*=(const Mat4& other);
	Vec3& operator*=(Mat4&& other);
};

Vec3 operator+(const Vec3& lhs, const Vec3& rhs);
Vec3 operator+(const Vec3& lhs, Vec3&& rhs);
Vec3 operator-(const Vec3& lhs, const Vec3& rhs);
Vec3 operator-(const Vec3& lhs, Vec3&& rhs);
Vec3 operator*(const Vec3& lhs, const Vec3& rhs);
Vec3 operator*(const Vec3& lhs, Vec3&& rhs);
Vec3 operator/(const Vec3& lhs, const Vec3& rhs);
Vec3 operator/(const Vec3& lhs, Vec3&& rhs);

Vec3 operator+(Vec3&& lhs, const Vec3& rhs);
Vec3 operator+(Vec3&& lhs, Vec3&& rhs);
Vec3 operator-(Vec3&& lhs, const Vec3& rhs);
Vec3 operator-(Vec3&& lhs, Vec3&& rhs);
Vec3 operator*(Vec3&& lhs, const Vec3& rhs);
Vec3 operator*(Vec3&& lhs, Vec3&& rhs);
Vec3 operator/(Vec3&& lhs, const Vec3& rhs);
Vec3 operator/(Vec3&& lhs, Vec3&& rhs);

Vec3 operator*(const Vec3& lhs, float rhs);
Vec3 operator/(const Vec3& lhs, float rhs);
Vec3 operator*(Vec3&& lhs, float rhs);
Vec3 operator/(Vec3&& lhs, float rhs);

Vec3 operator*(const Vec3& lhs, const Mat3& rhs);
Vec3 operator*(const Vec3& lhs, Mat3&& rhs);
Vec3 operator*(const Vec3& lhs, const Mat4& rhs);
Vec3 operator*(const Vec3& lhs, Mat4&& rhs);

Vec3 operator*(Vec3&& lhs, const Mat3& rhs);
Vec3 operator*(Vec3&& lhs, Mat3&& rhs);
Vec3 operator*(Vec3&& lhs, const Mat4& rhs);
Vec3 operator*(Vec3&& lhs, Mat4&& rhs);

Vec3 operator*(float lhs, const Vec3& rhs);
Vec3 operator*(float lhs, Vec3&& rhs);

/// A 4D vector.
struct Vec4
{
	float x;
	float y;
	float z;
	float w;

	Vec4& operator+=(const Vec4& other);
	Vec4& operator+=(Vec4&& other);
	Vec4& operator-=(const Vec4& other);
	Vec4& operator-=(Vec4&& other);
	Vec4& operator*=(const Vec4& other);
	Vec4& operator*=(Vec4&& other);
	Vec4& operator/=(const Vec4& other);
	Vec4& operator/=(Vec4&& other);
	
	Vec4& operator*=(float other);
	Vec4& operator/=(float other);

	Vec4& operator*=(const Mat3& other);
	Vec4& operator*=(Mat3&& other);
	Vec4& operator*=(const Mat4& other);
	Vec4& operator*=(Mat4&& other);
};

Vec4 operator+(const Vec4& lhs, const Vec4& rhs);
Vec4 operator+(const Vec4& lhs, Vec4&& rhs);
Vec4 operator-(const Vec4& lhs, const Vec4& rhs);
Vec4 operator-(const Vec4& lhs, Vec4&& rhs);
Vec4 operator*(const Vec4& lhs, const Vec4& rhs);
Vec4 operator*(const Vec4& lhs, Vec4&& rhs);
Vec4 operator/(const Vec4& lhs, const Vec4& rhs);
Vec4 operator/(const Vec4& lhs, Vec4&& rhs);

Vec4 operator+(Vec4&& lhs, const Vec4& rhs);
Vec4 operator+(Vec4&& lhs, Vec4&& rhs);
Vec4 operator-(Vec4&& lhs, const Vec4& rhs);
Vec4 operator-(Vec4&& lhs, Vec4&& rhs);
Vec4 operator*(Vec4&& lhs, const Vec4& rhs);
Vec4 operator*(Vec4&& lhs, Vec4&& rhs);
Vec4 operator/(Vec4&& lhs, const Vec4& rhs);
Vec4 operator/(Vec4&& lhs, Vec4&& rhs);

Vec4 operator*(const Vec4& lhs, float rhs);
Vec4 operator/(const Vec4& lhs, float rhs);

Vec4 operator*(Vec4&& lhs, float rhs);
Vec4 operator/(Vec4&& lhs, float rhs);

Vec4 operator*(const Vec4& lhs, const Mat3& rhs);
Vec4 operator*(const Vec4& lhs, Mat3&& rhs);
Vec4 operator*(const Vec4& lhs, const Mat4& rhs);
Vec4 operator*(const Vec4& lhs, Mat4&& rhs);

Vec4 operator*(Vec4&& lhs, const Mat3& rhs);
Vec4 operator*(Vec4&& lhs, Mat3&& rhs);
Vec4 operator*(Vec4&& lhs, const Mat4& rhs);
Vec4 operator*(Vec4&& lhs, Mat4&& rhs);

Vec4 operator*(float lhs, Vec4& rhs);
Vec4 operator*(float lhs, Vec4&& rhs);

/// A 3x3 Matrix.
struct Mat3
{
	float data[9];

	static Mat3 Identity();

	Mat3& operator*=(const Mat3& other);
	Mat3& operator*=(Mat3&& other);
};

Mat3 operator*(const Mat3& lhs, const Mat3& rhs);
Mat3 operator*(const Mat3& lhs, Mat3&& rhs);
Mat3 operator*(Mat3&& lhs, const Mat3& rhs);
Mat3 operator*(Mat3&& lhs, Mat3&& rhs);

Vec3 operator*(const Mat3& lhs, const Vec3& rhs);
Vec3 operator*(const Mat3& lhs, Vec3&& rhs);
Vec3 operator*(Mat3&& lhs, const Vec3& rhs);
Vec3 operator*(Mat3&& lhs, Vec3&& rhs);

/// A 4x4 Matrix.
struct Mat4
{
	float data[16];

	static Mat4 Identity();

	static Mat4 Translation(float x, float y, float z);
	static Mat4 Translation(Vec3 trans);
	static Mat4 Rotation(float roll, float pitch, float yaw);
	static Mat4 Rotation(Vec3 rot);
	static Mat4 Scale(float scale);
	static Mat4 Scale(float x, float y, float z);
	static Mat4 Scale(Vec3 scale);

	static Mat4 Orthographic(float left, float right, float bottom, float top, float nearClip, float farClip);
	static Mat4 Frustrum(float left, float right, float bottom, float top, float nearClip, float farClip);
	static Mat4 Perspective(float fov, float aspectRatio, float nearClip, float farClip);

	Mat4& operator*=(const Mat4& other);
	Mat4& operator*=(Mat4&& other);
};

Mat4 operator*(const Mat4& lhs, const Mat4& rhs);
Mat4 operator*(const Mat4& lhs, Mat4&& rhs);
Mat4 operator*(Mat4&& lhs, const Mat4& rhs);
Mat4 operator*(Mat4&& lhs, Mat4&& rhs);

Vec4 operator*(const Mat4& lhs, const Vec4& rhs);
Vec4 operator*(const Mat4& lhs, Vec4&& rhs);
Vec4 operator*(Mat4&& lhs, const Vec4& rhs);
Vec4 operator*(Mat4&& lhs, Vec4&& rhs);

}
