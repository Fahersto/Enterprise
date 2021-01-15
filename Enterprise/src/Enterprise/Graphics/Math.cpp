#include "EP_PCH.h"
#include "Math.h"

using namespace Enterprise::Math;


Matrix4 Matrices::Identity()
{
	return Matrix4();
}

Matrix4 Matrices::Orthographic(float left, float right, float top, float bottom, float near, float far)
{
	return Matrix4();
}

Matrix4 Matrices::Perspective(float fov, float aspectRatio, float near, float far)
{
	return Matrix4();
}

Matrix4 Matrices::PixelSpace()
{
	return Matrix4();
}
