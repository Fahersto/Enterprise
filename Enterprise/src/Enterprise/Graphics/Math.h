#pragma once
#include "Core.h"

namespace Enterprise::Math
{

constexpr float DegToRad(float degrees) { return (M_PI / 180.0) * degrees; }
constexpr float RadToDeg(float radians) { return (180.0 / M_PI) * radians; }

struct Mat3;
struct Mat4;

/// A 2D vector.
struct Vec2
{
	float x;
	float y;

	Vec2(float X, float Y) : x(X), y(Y) {};

	/// @cond DOXYGEN_SKIP
	Vec2 operator-() { return Vec2(-x, -y); }

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
	/// @endcond
};

/// @cond DOXYGEN_SKIP
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
/// @endcond

/// A 3D vector.
struct Vec3
{
	float x;
	float y;
	float z;

	Vec3(float X, float Y, float Z) : x(X), y(Y), z(Z) {};

	/// @cond DOXYGEN_SKIP
	Vec3 operator-() { return Vec3(-x, -y, -z); }

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
	/// @endcond
};

/// @cond DOXYGEN_SKIP
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
/// @endcond

/// A 4D vector.
struct Vec4
{
	float x;
	float y;
	float z;
	float w;

	Vec4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {};

	/// @cond DOXYGEN_SKIP
	Vec4 operator-() { return Vec4(-x, -y, -z, -w); }

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
	/// @endcond
};

/// @cond DOXYGEN_SKIP
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
/// @endcond

/// A 3x3 Matrix.
struct Mat3
{
	float data[9];

	/// Generate a 3x3 identity matrix.
	static Mat3 Identity();

	/// @cond DOXYGEN_SKIP
	Mat3& operator*=(const Mat3& other);
	Mat3& operator*=(Mat3&& other);
	/// @endcond
};

/// @cond DOXYGEN_SKIP
Mat3 operator*(const Mat3& lhs, const Mat3& rhs);
Mat3 operator*(const Mat3& lhs, Mat3&& rhs);
Mat3 operator*(Mat3&& lhs, const Mat3& rhs);
Mat3 operator*(Mat3&& lhs, Mat3&& rhs);

Vec3 operator*(const Mat3& lhs, const Vec3& rhs);
Vec3 operator*(const Mat3& lhs, Vec3&& rhs);
Vec3 operator*(Mat3&& lhs, const Vec3& rhs);
Vec3 operator*(Mat3&& lhs, Vec3&& rhs);
/// @endcond

/// A 4x4 Matrix.
struct Mat4
{
	float data[16];

	/// Generate a 4x4 identity matrix.
	static Mat4 Identity();

	/// Generate a 4x4 translation matrix.
	/// @param x X component of translation.
	/// @param y Y component of translation.
	/// @param z Z component of translation.
	static Mat4 Translation(float x, float y, float z);
	/// Generate a 4x4 translation matrix.
	/// @param trans Translation vector.
	static Mat4 Translation(Vec3 trans);
	/// Generate a 4x4 rotation matrix.
	/// @param roll Angle of roll in degrees.
	/// @param pitch Angle of pitch in degrees.
	/// @param yaw Angle of yaw in degrees.
	static Mat4 Rotation(float roll, float pitch, float yaw);
	/// Generate a 4x4 scaling matrix.
	/// @param scale The scale factor.
	static Mat4 Scale(float scale);
	/// Generate a 4x4 scaling matrix.
	/// @param x The scale factor for the x component.
	/// @param y The scale factor for the y component.
	/// @param z The scale factor for the z component.
	static Mat4 Scale(float x, float y, float z);
	/// Generate a 4x4 scaling matrix.
	/// @param scale The multi-dimensional scaling factor.
	static Mat4 Scale(Vec3 scale);

	/// Generate a 4x4 orthographic projection matrix.
	/// @param left The x coordinate, in view space, of the left clipping plane.
	/// @param right The x coordinate, in view space, of the right clipping plane.
	/// @param bottom The y coordinate, in view space, of the lower clipping plane..
	/// @param top The y coordinate, in view space, of the upper clipping plane.
	/// @param nearClip The z coordinate, in view space, of the near clipping plane.
	/// @param farClip The z coordinate, in view space, of the far clipping plane.
	static Mat4 Orthographic(float left, float right, float bottom, float top, float nearClip, float farClip);
	/// Generate a 4x4 perspective projection matrix.
	/// @param left The x coordinate, in view space, where the left clipping plane instersects with the near clipping plane.
	/// @param right The x coordinate, in view space, where the right clipping plane instersects with the near clipping plane.
	/// @param bottom The y coordinate, in view space, where the bottom clipping plane instersects with the near clipping plane.
	/// @param top The y coordinate, in view space, where the top clipping plane instersects with the near clipping plane.
	/// @param nearClip The z coordinate, in view space, of the near clipping plane.
	/// @param farClip The z coordinate, in view space, of the far clipping plane.
	static Mat4 Frustrum(float left, float right, float bottom, float top, float nearClip, float farClip);
	/// Generate a 4x4 perspective projection matrix.
	/// @param fov The field of view angle, in degrees, in the y direction.
	/// @param aspectRatio The aspect ratio of the projection.  Determines the field of view in the x direction.
	/// @param nearClip The z coordinate, in view space, of the near clipping plane.
	/// @param farClip The z coordinate, in view space, of the far clipping plane.
	static Mat4 Perspective(float fov, float aspectRatio, float nearClip, float farClip);

	/// @cond DOXYGEN_SKIP
	Mat4& operator*=(const Mat4& other);
	Mat4& operator*=(Mat4&& other);
	/// @endcond
};

/// @cond DOXYGEN_SKIP
Mat4 operator*(const Mat4& lhs, const Mat4& rhs);
Mat4 operator*(const Mat4& lhs, Mat4&& rhs);
Mat4 operator*(Mat4&& lhs, const Mat4& rhs);
Mat4 operator*(Mat4&& lhs, Mat4&& rhs);

Vec4 operator*(const Mat4& lhs, const Vec4& rhs);
Vec4 operator*(const Mat4& lhs, Vec4&& rhs);
Vec4 operator*(Mat4&& lhs, const Vec4& rhs);
Vec4 operator*(Mat4&& lhs, Vec4&& rhs);
/// @endcond

}
