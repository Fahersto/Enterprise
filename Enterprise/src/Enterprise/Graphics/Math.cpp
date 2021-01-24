#include "EP_PCH.h"
#include "Math.h"

namespace Enterprise::Math
{

// Vec2 operators

Vec2& Vec2::operator+=(const Vec2& other)
{
	x = x + other.x;
	y = y + other.y;
	return *this;
}
Vec2& Vec2::operator+=(Vec2&& other)
{
	x = x + other.x;
	y = y + other.y;
	return *this;
}
Vec2& Vec2::operator-=(const Vec2& other)
{
	x = x - other.x;
	y = y - other.y;
	return *this;
}
Vec2& Vec2::operator-=(Vec2&& other)
{
	x = x - other.x;
	y = y - other.y;
	return *this;
}
Vec2& Vec2::operator*=(const Vec2& other)
{
	x = x * other.x;
	y = y * other.y;
	return *this;
}
Vec2& Vec2::operator*=(Vec2&& other)
{
	x = x * other.x;
	y = y * other.y;
	return *this;
}
Vec2& Vec2::operator/=(const Vec2& other)
{
	x = x / other.x;
	y = y / other.y;
	return *this;
}
Vec2& Vec2::operator/=(Vec2&& other)
{
	x = x / other.x;
	y = y / other.y;
	return *this;
}

Vec2& Vec2::operator*=(float other)
{
	x = x * other;
	y = y * other;
	return *this;
}
Vec2& Vec2::operator/=(float other)
{
	x = x / other;
	y = y / other;
	return *this;
}

Vec2& Vec2::operator*=(const Mat3& other) // Vector treated as [x, y, 0]
{
	Vec2 oldVals = { x, y };

	x = oldVals.x * other.data[0] + oldVals.y * other.data[3];
	y = oldVals.x * other.data[1] + oldVals.y * other.data[4];

	return *this;
}
Vec2& Vec2::operator*=(Mat3&& other) // Vector treated as [x, y, 0]
{
	Vec2 oldVals = { x, y };

	x = oldVals.x * other.data[0] + oldVals.y * other.data[3];
	y = oldVals.x * other.data[1] + oldVals.y * other.data[4];

	return *this;
}
Vec2& Vec2::operator*=(const Mat4& other) // Vector treated as [x, y, 0, 1]
{
	Vec2 oldVals = { x, y };

	x = oldVals.x * other.data[0] + oldVals.y * other.data[4] + other.data[12];
	y = oldVals.x * other.data[1] + oldVals.y * other.data[5] + other.data[13];

	return *this;
}
Vec2& Vec2::operator*=(Mat4&& other) // Vector treated as [x, y, 0, 1]
{
	Vec2 oldVals = { x, y };

	x = oldVals.x * other.data[0] + oldVals.y * other.data[4] + other.data[12];
	y = oldVals.x * other.data[1] + oldVals.y * other.data[5] + other.data[13];

	return *this;
}

Vec2 operator+(const Vec2& lhs, const Vec2& rhs)
{
	return
	{
		lhs.x + rhs.x,
		lhs.y + rhs.y
	};
}
Vec2 operator+(const Vec2& lhs, Vec2&& rhs)
{
	return
	{
		lhs.x + rhs.x,
		lhs.y + rhs.y
	};
}
Vec2 operator-(const Vec2& lhs, const Vec2& rhs)
{
	return
	{
		lhs.x - rhs.x,
		lhs.y - rhs.y
	};
}
Vec2 operator-(const Vec2& lhs, Vec2&& rhs)
{
	return
	{
		lhs.x - rhs.x,
		lhs.y - rhs.y
	};
}
Vec2 operator*(const Vec2& lhs, const const Vec2& rhs)
{
	return
	{
		lhs.x * rhs.x,
		lhs.y * rhs.y
	};
}
Vec2 operator*(const Vec2& lhs, Vec2&& rhs)
{
	return
	{
		lhs.x * rhs.x,
		lhs.y * rhs.y
	};
}
Vec2 operator/(const Vec2& lhs, const Vec2& rhs)
{
	return
	{
		lhs.x / rhs.x,
		lhs.y / rhs.y
	};
}
Vec2 operator/(const Vec2& lhs, Vec2&& rhs)
{
	return
	{
		lhs.x / rhs.x,
		lhs.y / rhs.y
	};
}

Vec2 operator+(Vec2&& lhs, const Vec2& rhs)
{
	return
	{
		lhs.x + rhs.x,
		lhs.y + rhs.y
	};
}
Vec2 operator+(Vec2&& lhs, Vec2&& rhs)
{
	return
	{
		lhs.x + rhs.x,
		lhs.y + rhs.y
	};
}
Vec2 operator-(Vec2&& lhs, const Vec2& rhs)
{
	return
	{
		lhs.x - rhs.x,
		lhs.y - rhs.y
	};
}
Vec2 operator-(Vec2&& lhs, Vec2&& rhs)
{
	return
	{
		lhs.x - rhs.x,
		lhs.y - rhs.y
	};
}
Vec2 operator*(Vec2&& lhs, const const Vec2& rhs)
{
	return
	{
		lhs.x * rhs.x,
		lhs.y * rhs.y
	};
}
Vec2 operator*(Vec2&& lhs, Vec2&& rhs)
{
	return
	{
		lhs.x * rhs.x,
		lhs.y * rhs.y
	};
}
Vec2 operator/(Vec2&& lhs, const Vec2& rhs)
{
	return
	{
		lhs.x / rhs.x,
		lhs.y / rhs.y
	};
}
Vec2 operator/(Vec2&& lhs, Vec2&& rhs)
{
	return
	{
		lhs.x / rhs.x,
		lhs.y / rhs.y
	};
}

Vec2 operator*(const Vec2& lhs, float rhs)
{
	return
	{
		lhs.x * rhs,
		lhs.y * rhs
	};
}
Vec2 operator/(const Vec2& lhs, float rhs)
{
	return
	{
		lhs.x / rhs,
		lhs.y / rhs
	};
}
Vec2 operator*(Vec2&& lhs, float rhs)
{
	return
	{
		lhs.x * rhs,
		lhs.y * rhs
	};
}
Vec2 operator/(Vec2&& lhs, float rhs)
{
	return
	{
		lhs.x / rhs,
		lhs.y / rhs
	};
}

Vec2 operator*(const Vec2& lhs, const Mat3& rhs)// Vector treated as [x, y, 0]
{
	return
	{
		lhs.x * rhs.data[0] + lhs.y * rhs.data[3],
		lhs.x * rhs.data[1] + lhs.y * rhs.data[4]
	};
}
Vec2 operator*(const Vec2& lhs, Mat3&& rhs) // Vector treated as [x, y, 0]
{
	return
	{
		lhs.x * rhs.data[0] + lhs.y * rhs.data[3],
		lhs.x * rhs.data[1] + lhs.y * rhs.data[4]
	};
}
Vec2 operator*(const Vec2& lhs, const Mat4& rhs) // Vector treated as [x, y, 0, 1]
{
	return
	{
		lhs.x * rhs.data[0] + lhs.y * rhs.data[4] + rhs.data[12],
		lhs.x * rhs.data[1] + lhs.y * rhs.data[5] + rhs.data[13]
	};
}
Vec2 operator*(const Vec2& lhs, Mat4&& rhs) // Vector treated as [x, y, 0, 1]
{
	return
	{
		lhs.x * rhs.data[0] + lhs.y * rhs.data[4] + rhs.data[12],
		lhs.x * rhs.data[1] + lhs.y * rhs.data[5] + rhs.data[13]
	};
}

Vec2 operator*(Vec2&& lhs, const Mat3& rhs)// Vector treated as [x, y, 0]
{
	return
	{
		lhs.x * rhs.data[0] + lhs.y * rhs.data[3],
		lhs.x * rhs.data[1] + lhs.y * rhs.data[4]
	};
}
Vec2 operator*(Vec2&& lhs, Mat3&& rhs) // Vector treated as [x, y, 0]
{
	return
	{
		lhs.x * rhs.data[0] + lhs.y * rhs.data[3],
		lhs.x * rhs.data[1] + lhs.y * rhs.data[4]
	};
}
Vec2 operator*(Vec2&& lhs, const Mat4& rhs) // Vector treated as [x, y, 0, 1]
{
	return
	{
		lhs.x * rhs.data[0] + lhs.y * rhs.data[4] + rhs.data[12],
		lhs.x * rhs.data[1] + lhs.y * rhs.data[5] + rhs.data[13]
	};
}
Vec2 operator*(Vec2&& lhs, Mat4&& rhs) // Vector treated as [x, y, 0, 1]
{
	return
	{
		lhs.x * rhs.data[0] + lhs.y * rhs.data[4] + rhs.data[12],
		lhs.x * rhs.data[1] + lhs.y * rhs.data[5] + rhs.data[13]
	};
}

Vec2 operator*(float lhs, const Vec2& rhs)
{
	return rhs * lhs;
};
Vec2 operator*(float lhs, Vec2&& rhs)
{
	return rhs * lhs;
};

// Vec3 operators

Vec3& Vec3::operator+=(const Vec3& other)
{
	x = x + other.x;
	y = y + other.y;
	z = z + other.z;
	return *this;
}
Vec3& Vec3::operator+=(Vec3&& other)
{
	x = x + other.x;
	y = y + other.y;
	z = z + other.z;
	return *this;
}
Vec3& Vec3::operator-=(const Vec3& other)
{
	x = x - other.x;
	y = y - other.y;
	z = z - other.z;
	return *this;
}
Vec3& Vec3::operator-=(Vec3&& other)
{
	x = x - other.x;
	y = y - other.y;
	z = z - other.z;
	return *this;
}
Vec3& Vec3::operator*=(const Vec3& other)
{
	x = x * other.x;
	y = y * other.y;
	z = z * other.z;
	return *this;
}
Vec3& Vec3::operator*=(Vec3&& other)
{
	x = x * other.x;
	y = y * other.y;
	z = z * other.z;
	return *this;
}
Vec3& Vec3::operator/=(const Vec3& other)
{
	x = x / other.x;
	y = y / other.y;
	z = z / other.z;
	return *this;
}
Vec3& Vec3::operator/=(Vec3&& other)
{
	x = x / other.x;
	y = y / other.y;
	z = z / other.z;
	return *this;
}

Vec3& Vec3::operator*=(float other)
{
	x = x * other;
	y = y * other;
	z = z * other;
	return *this;
}
Vec3& Vec3::operator/=(float other)
{
	x = x / other;
	y = y / other;
	z = z / other;
	return *this;
}

Vec3& Vec3::operator*=(const Mat3& other)
{
	Vec3 oldVals = { x, y, z };

	x = oldVals.x * other.data[0] + oldVals.y * other.data[3] + oldVals.z * other.data[6];
	y = oldVals.x * other.data[1] + oldVals.y * other.data[4] + oldVals.z * other.data[7];
	z = oldVals.x * other.data[2] + oldVals.y * other.data[5] + oldVals.z * other.data[8];

	return *this;
}
Vec3& Vec3::operator*=(Mat3&& other)
{
	Vec3 oldVals = { x, y, z };

	x = oldVals.x * other.data[0] + oldVals.y * other.data[3] + oldVals.z * other.data[6];
	y = oldVals.x * other.data[1] + oldVals.y * other.data[4] + oldVals.z * other.data[7];
	z = oldVals.x * other.data[2] + oldVals.y * other.data[5] + oldVals.z * other.data[8];

	return *this;
}
Vec3& Vec3::operator*=(const Mat4& other) // Vector treated as [x, y, z, 1]
{
	Vec3 oldVals = { x, y, z };
	float out_w = x * other.data[3] + y * other.data[7] + z * other.data[11] + other.data[15];

	if (out_w == 0.0f)
	{
		x = oldVals.x * other.data[0] + oldVals.y * other.data[4] + oldVals.z * other.data[8] + other.data[12];
		y = oldVals.x * other.data[1] + oldVals.y * other.data[5] + oldVals.z * other.data[9] + other.data[13];
		z = oldVals.x * other.data[2] + oldVals.y * other.data[6] + oldVals.z * other.data[10] + other.data[14];
	}
	else
	{
		x = (oldVals.x * other.data[0] + oldVals.y * other.data[4] + oldVals.z * other.data[8] + other.data[12]) / out_w;
		y = (oldVals.x * other.data[1] + oldVals.y * other.data[5] + oldVals.z * other.data[9] + other.data[13]) / out_w;
		z = (oldVals.x * other.data[2] + oldVals.y * other.data[6] + oldVals.z * other.data[10] + other.data[14]) / out_w;
	}

	return *this;
}
Vec3& Vec3::operator*=(Mat4&& other) // Vector treated as [x, y, z, 1]
{
	Vec3 oldVals = { x, y, z };
	float out_w = x * other.data[3] + y * other.data[7] + z * other.data[11] + other.data[15];

	if (out_w == 0.0f)
	{
		x = oldVals.x * other.data[0] + oldVals.y * other.data[4] + oldVals.z * other.data[8] + other.data[12];
		y = oldVals.x * other.data[1] + oldVals.y * other.data[5] + oldVals.z * other.data[9] + other.data[13];
		z = oldVals.x * other.data[2] + oldVals.y * other.data[6] + oldVals.z * other.data[10] + other.data[14];
	}
	else
	{
		x = (oldVals.x * other.data[0] + oldVals.y * other.data[4] + oldVals.z * other.data[8] + other.data[12]) / out_w;
		y = (oldVals.x * other.data[1] + oldVals.y * other.data[5] + oldVals.z * other.data[9] + other.data[13]) / out_w;
		z = (oldVals.x * other.data[2] + oldVals.y * other.data[6] + oldVals.z * other.data[10] + other.data[14]) / out_w;
	}

	return *this;
}

Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
{
	return
	{
		lhs.x + rhs.x,
		lhs.y + rhs.y,
		lhs.z + rhs.z
	};
}
Vec3 operator+(const Vec3& lhs, Vec3&& rhs)
{
	return
	{
		lhs.x + rhs.x,
		lhs.y + rhs.y,
		lhs.z + rhs.z
	};
}
Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
{
	return
	{
		lhs.x - rhs.x,
		lhs.y - rhs.y,
		lhs.z - rhs.z
	};
}
Vec3 operator-(const Vec3& lhs, Vec3&& rhs)
{
	return
	{
		lhs.x - rhs.x,
		lhs.y - rhs.y,
		lhs.z - rhs.z
	};
}
Vec3 operator*(const Vec3& lhs, const Vec3& rhs)
{
	return
	{
		lhs.x * rhs.x,
		lhs.y * rhs.y,
		lhs.z * rhs.z
	};
}
Vec3 operator*(const Vec3& lhs, Vec3&& rhs)
{
	return
	{
		lhs.x * rhs.x,
		lhs.y * rhs.y,
		lhs.z * rhs.z
	};
}
Vec3 operator/(const Vec3& lhs, const Vec3& rhs)
{
	return
	{
		lhs.x / rhs.x,
		lhs.y / rhs.y,
		lhs.z / rhs.z
	};
}
Vec3 operator/(const Vec3& lhs, Vec3&& rhs)
{
	return
	{
		lhs.x / rhs.x,
		lhs.y / rhs.y,
		lhs.z / rhs.z
	};
}

Vec3 operator+(Vec3&& lhs, const Vec3& rhs)
{
	return
	{
		lhs.x + rhs.x,
		lhs.y + rhs.y,
		lhs.z + rhs.z
	};
}
Vec3 operator+(Vec3&& lhs, Vec3&& rhs)
{
	return
	{
		lhs.x + rhs.x,
		lhs.y + rhs.y,
		lhs.z + rhs.z
	};
}
Vec3 operator-(Vec3&& lhs, const Vec3& rhs)
{
	return
	{
		lhs.x - rhs.x,
		lhs.y - rhs.y,
		lhs.z - rhs.z
	};
}
Vec3 operator-(Vec3&& lhs, Vec3&& rhs)
{
	return
	{
		lhs.x - rhs.x,
		lhs.y - rhs.y,
		lhs.z - rhs.z
	};
}
Vec3 operator*(Vec3&& lhs, const Vec3& rhs)
{
	return
	{
		lhs.x * rhs.x,
		lhs.y * rhs.y,
		lhs.z * rhs.z
	};
}
Vec3 operator*(Vec3&& lhs, Vec3&& rhs)
{
	return
	{
		lhs.x * rhs.x,
		lhs.y * rhs.y,
		lhs.z * rhs.z
	};
}
Vec3 operator/(Vec3&& lhs, const Vec3& rhs)
{
	return
	{
		lhs.x / rhs.x,
		lhs.y / rhs.y,
		lhs.z / rhs.z
	};
}
Vec3 operator/(Vec3&& lhs, Vec3&& rhs)
{
	return
	{
		lhs.x / rhs.x,
		lhs.y / rhs.y,
		lhs.z / rhs.z
	};
}

Vec3 operator*(const Vec3& lhs, float rhs)
{
	return
	{
		lhs.x * rhs,
		lhs.y * rhs,
		lhs.z * rhs
	};
}
Vec3 operator/(const Vec3& lhs, float rhs)
{
	return
	{
		lhs.x / rhs,
		lhs.y / rhs,
		lhs.z / rhs
	};
}
Vec3 operator*(Vec3&& lhs, float rhs)
{
	return
	{
		lhs.x * rhs,
		lhs.y * rhs,
		lhs.z * rhs
	};
}
Vec3 operator/(Vec3&& lhs, float rhs)
{
	return
	{
		lhs.x / rhs,
		lhs.y / rhs,
		lhs.z / rhs
	};
}

Vec3 operator*(const Vec3& lhs, const Mat3& rhs)
{
	return
	{
		lhs.x * rhs.data[0] + lhs.y * rhs.data[3] + lhs.z * rhs.data[6],
		lhs.x * rhs.data[1] + lhs.y * rhs.data[4] + lhs.z * rhs.data[7],
		lhs.x * rhs.data[2] + lhs.y * rhs.data[5] + lhs.z * rhs.data[8]
	};
}
Vec3 operator*(const Vec3& lhs, Mat3&& rhs)
{
	return
	{
		lhs.x * rhs.data[0] + lhs.y * rhs.data[3] + lhs.z * rhs.data[6],
		lhs.x * rhs.data[1] + lhs.y * rhs.data[4] + lhs.z * rhs.data[7],
		lhs.x * rhs.data[2] + lhs.y * rhs.data[5] + lhs.z * rhs.data[8]
	};
}
Vec3 operator*(const Vec3& lhs, const Mat4& rhs) // Vector treated as [x, y, z, 1]
{
	float out_w = lhs.x * rhs.data[3] + lhs.y * rhs.data[7] + lhs.z * rhs.data[11] + rhs.data[15];
	if (out_w == 0.0f)
	{
		return
		{
			lhs.x * rhs.data[0] + lhs.y * rhs.data[4] + lhs.z * rhs.data[8] + rhs.data[12],
			lhs.x * rhs.data[1] + lhs.y * rhs.data[5] + lhs.z * rhs.data[9] + rhs.data[13],
			lhs.x * rhs.data[2] + lhs.y * rhs.data[6] + lhs.z * rhs.data[10] + rhs.data[14]
		};
	}
	else
	{
		return
		{
			(lhs.x * rhs.data[0] + lhs.y * rhs.data[4] + lhs.z * rhs.data[8] + rhs.data[12]) / out_w,
			(lhs.x * rhs.data[1] + lhs.y * rhs.data[5] + lhs.z * rhs.data[9] + rhs.data[13]) / out_w,
			(lhs.x * rhs.data[2] + lhs.y * rhs.data[6] + lhs.z * rhs.data[10] + rhs.data[14]) / out_w
		};
	}
}
Vec3 operator*(const Vec3& lhs, Mat4&& rhs) // Vector treated as [x, y, z, 1]
{
	float out_w = lhs.x * rhs.data[3] + lhs.y * rhs.data[7] + lhs.z * rhs.data[11] + rhs.data[15];
	if (out_w == 0.0f)
	{
		return
		{
			lhs.x * rhs.data[0] + lhs.y * rhs.data[4] + lhs.z * rhs.data[8] + rhs.data[12],
			lhs.x * rhs.data[1] + lhs.y * rhs.data[5] + lhs.z * rhs.data[9] + rhs.data[13],
			lhs.x * rhs.data[2] + lhs.y * rhs.data[6] + lhs.z * rhs.data[10] + rhs.data[14]
		};
	}
	else
	{
		return
		{
			(lhs.x * rhs.data[0] + lhs.y * rhs.data[4] + lhs.z * rhs.data[8] + rhs.data[12]) / out_w,
			(lhs.x * rhs.data[1] + lhs.y * rhs.data[5] + lhs.z * rhs.data[9] + rhs.data[13]) / out_w,
			(lhs.x * rhs.data[2] + lhs.y * rhs.data[6] + lhs.z * rhs.data[10] + rhs.data[14]) / out_w
		};
	}
}

Vec3 operator*(Vec3&& lhs, const Mat3& rhs)
{
	return
	{
		lhs.x * rhs.data[0] + lhs.y * rhs.data[3] + lhs.z * rhs.data[6],
		lhs.x * rhs.data[1] + lhs.y * rhs.data[4] + lhs.z * rhs.data[7],
		lhs.x * rhs.data[2] + lhs.y * rhs.data[5] + lhs.z * rhs.data[8]
	};
}
Vec3 operator*(Vec3&& lhs, Mat3&& rhs)
{
	return
	{
		lhs.x * rhs.data[0] + lhs.y * rhs.data[3] + lhs.z * rhs.data[6],
		lhs.x * rhs.data[1] + lhs.y * rhs.data[4] + lhs.z * rhs.data[7],
		lhs.x * rhs.data[2] + lhs.y * rhs.data[5] + lhs.z * rhs.data[8]
	};
}
Vec3 operator*(Vec3&& lhs, const Mat4& rhs) // Vector treated as [x, y, z, 1]
{
	float out_w = lhs.x * rhs.data[3] + lhs.y * rhs.data[7] + lhs.z * rhs.data[11] + rhs.data[15];
	if (out_w == 0.0f)
	{
		return
		{
			lhs.x * rhs.data[0] + lhs.y * rhs.data[4] + lhs.z * rhs.data[8] + rhs.data[12],
			lhs.x * rhs.data[1] + lhs.y * rhs.data[5] + lhs.z * rhs.data[9] + rhs.data[13],
			lhs.x * rhs.data[2] + lhs.y * rhs.data[6] + lhs.z * rhs.data[10] + rhs.data[14]
		};
	}
	else
	{
		return
		{
			(lhs.x * rhs.data[0] + lhs.y * rhs.data[4] + lhs.z * rhs.data[8] + rhs.data[12]) / out_w,
			(lhs.x * rhs.data[1] + lhs.y * rhs.data[5] + lhs.z * rhs.data[9] + rhs.data[13]) / out_w,
			(lhs.x * rhs.data[2] + lhs.y * rhs.data[6] + lhs.z * rhs.data[10] + rhs.data[14]) / out_w
		};
	}
}
Vec3 operator*(Vec3&& lhs, Mat4&& rhs) // Vector treated as [x, y, z, 1]
{
	float out_w = lhs.x * rhs.data[3] + lhs.y * rhs.data[7] + lhs.z * rhs.data[11] + rhs.data[15];
	if (out_w == 0.0f)
	{
		return
		{
			lhs.x * rhs.data[0] + lhs.y * rhs.data[4] + lhs.z * rhs.data[8] + rhs.data[12],
			lhs.x * rhs.data[1] + lhs.y * rhs.data[5] + lhs.z * rhs.data[9] + rhs.data[13],
			lhs.x * rhs.data[2] + lhs.y * rhs.data[6] + lhs.z * rhs.data[10] + rhs.data[14]
		};
	}
	else
	{
		return
		{
			(lhs.x * rhs.data[0] + lhs.y * rhs.data[4] + lhs.z * rhs.data[8] + rhs.data[12]) / out_w,
			(lhs.x * rhs.data[1] + lhs.y * rhs.data[5] + lhs.z * rhs.data[9] + rhs.data[13]) / out_w,
			(lhs.x * rhs.data[2] + lhs.y * rhs.data[6] + lhs.z * rhs.data[10] + rhs.data[14]) / out_w
		};
	}
}

Vec3 operator*(float lhs, const Vec3& rhs)
{
	return rhs * lhs;
};
Vec3 operator*(float lhs, Vec3&& rhs)
{
	return rhs * lhs;
};

// Vec4 operators

Vec4& Vec4::operator+=(const Vec4& other)
{
	x = x + other.x;
	y = y + other.y;
	z = z + other.z;
	w = w + other.w;
	return *this;
}
Vec4& Vec4::operator+=(Vec4&& other)
{
	x = x + other.x;
	y = y + other.y;
	z = z + other.z;
	w = w + other.w;
	return *this;
}
Vec4& Vec4::operator-=(const Vec4& other)
{
	x = x - other.x;
	y = y - other.y;
	z = z - other.z;
	w = w - other.w;
	return *this;
}
Vec4& Vec4::operator-=(Vec4&& other)
{
	x = x - other.x;
	y = y - other.y;
	z = z - other.z;
	w = w - other.w;
	return *this;
}
Vec4& Vec4::operator*=(const Vec4& other)
{
	x = x * other.x;
	y = y * other.y;
	z = z * other.z;
	w = w * other.w;
	return *this;
}
Vec4& Vec4::operator*=(Vec4&& other)
{
	x = x * other.x;
	y = y * other.y;
	z = z * other.z;
	w = w * other.w;
	return *this;
}
Vec4& Vec4::operator/=(const Vec4& other)
{
	x = x / other.x;
	y = y / other.y;
	z = z / other.z;
	w = w / other.w;
	return *this;
}
Vec4& Vec4::operator/=(Vec4&& other)
{
	x = x / other.x;
	y = y / other.y;
	z = z / other.z;
	w = w / other.w;
	return *this;
}

Vec4& Vec4::operator*=(float other)
{
	x = x * other;
	y = y * other;
	z = z * other;
	w = w * other;
	return *this;
}
Vec4& Vec4::operator/=(float other)
{
	x = x / other;
	y = y / other;
	z = z / other;
	w = w / other;
	return *this;
}

Vec4& Vec4::operator*=(const Mat3& other)
{
	Vec4 oldVals = { x, y, z, w };

	if (w == 0.0f)
	{
		x = oldVals.x * other.data[0] + oldVals.y * other.data[3] + oldVals.z * other.data[6];
		y = oldVals.x * other.data[1] + oldVals.y * other.data[4] + oldVals.z * other.data[7];
		z = oldVals.x * other.data[2] + oldVals.y * other.data[5] + oldVals.z * other.data[8];
		w = 0.0f;
	}
	else
	{
		x = oldVals.x / oldVals.w * other.data[0] + oldVals.y / oldVals.w * other.data[3] + oldVals.z / oldVals.w * other.data[6];
		y = oldVals.x / oldVals.w * other.data[1] + oldVals.y / oldVals.w * other.data[4] + oldVals.z / oldVals.w * other.data[7];
		z = oldVals.x / oldVals.w * other.data[2] + oldVals.y / oldVals.w * other.data[5] + oldVals.z / oldVals.w * other.data[8];
		w = 1.0f;
	}

	return *this;
}
Vec4& Vec4::operator*=(Mat3&& other)
{
	Vec4 oldVals = { x, y, z, w };

	if (w == 0.0f)
	{
		x = oldVals.x * other.data[0] + oldVals.y * other.data[3] + oldVals.z * other.data[6];
		y = oldVals.x * other.data[1] + oldVals.y * other.data[4] + oldVals.z * other.data[7];
		z = oldVals.x * other.data[2] + oldVals.y * other.data[5] + oldVals.z * other.data[8];
		w = 0.0f;
	}
	else
	{
		x = oldVals.x / oldVals.w * other.data[0] + oldVals.y / oldVals.w * other.data[3] + oldVals.z / oldVals.w * other.data[6];
		y = oldVals.x / oldVals.w * other.data[1] + oldVals.y / oldVals.w * other.data[4] + oldVals.z / oldVals.w * other.data[7];
		z = oldVals.x / oldVals.w * other.data[2] + oldVals.y / oldVals.w * other.data[5] + oldVals.z / oldVals.w * other.data[8];
		w = 1.0f;
	}

	return *this;
}
Vec4& Vec4::operator*=(const Mat4& other)
{
	Vec4 oldVals = { x, y, z, w };

	x = oldVals.x * other.data[0] + oldVals.y * other.data[4] + oldVals.z * other.data[8] + oldVals.w * other.data[12];
	y = oldVals.x * other.data[1] + oldVals.y * other.data[5] + oldVals.z * other.data[9] + oldVals.w * other.data[13];
	z = oldVals.x * other.data[2] + oldVals.y * other.data[6] + oldVals.z * other.data[10] + oldVals.w * other.data[14];
	w = oldVals.x * other.data[3] + oldVals.y * other.data[7] + oldVals.z * other.data[11] + oldVals.w * other.data[15];

	return *this;
}
Vec4& Vec4::operator*=(Mat4&& other)
{
	Vec4 oldVals = { x, y, z, w };

	x = oldVals.x * other.data[0] + oldVals.y * other.data[4] + oldVals.z * other.data[8] + oldVals.w * other.data[12];
	y = oldVals.x * other.data[1] + oldVals.y * other.data[5] + oldVals.z * other.data[9] + oldVals.w * other.data[13];
	z = oldVals.x * other.data[2] + oldVals.y * other.data[6] + oldVals.z * other.data[10] + oldVals.w * other.data[14];
	w = oldVals.x * other.data[3] + oldVals.y * other.data[7] + oldVals.z * other.data[11] + oldVals.w * other.data[15];

	return *this;
}

Vec4 operator+(const Vec4& lhs, const Vec4& rhs)
{
	return
	{
		lhs.x + rhs.x,
		lhs.y + rhs.y,
		lhs.z + rhs.z,
		lhs.w + rhs.w
	};
}
Vec4 operator+(const Vec4& lhs, Vec4&& rhs)
{
	return
	{
		lhs.x + rhs.x,
		lhs.y + rhs.y,
		lhs.z + rhs.z,
		lhs.w + rhs.w
	};
}
Vec4 operator-(const Vec4& lhs, const Vec4& rhs)
{
	return
	{
		lhs.x - rhs.x,
		lhs.y - rhs.y,
		lhs.z - rhs.z,
		lhs.w - rhs.w
	};
}
Vec4 operator-(const Vec4& lhs, Vec4&& rhs)
{
	return
	{
		lhs.x - rhs.x,
		lhs.y - rhs.y,
		lhs.z - rhs.z,
		lhs.w - rhs.w
	};
}
Vec4 operator*(const Vec4& lhs, const Vec4& rhs)
{
	return
	{
		lhs.x * rhs.x,
		lhs.y * rhs.y,
		lhs.z * rhs.z,
		lhs.w * rhs.w
	};
}
Vec4 operator*(const Vec4& lhs, Vec4&& rhs)
{
	return
	{
		lhs.x * rhs.x,
		lhs.y * rhs.y,
		lhs.z * rhs.z,
		lhs.w * rhs.w
	};
}
Vec4 operator/(const Vec4& lhs, const Vec4& rhs)
{
	return
	{
		lhs.x / rhs.x,
		lhs.y / rhs.y,
		lhs.z / rhs.z,
		lhs.w / rhs.w
	};
}
Vec4 operator/(const Vec4& lhs, Vec4&& rhs)
{
	return
	{
		lhs.x / rhs.x,
		lhs.y / rhs.y,
		lhs.z / rhs.z,
		lhs.w / rhs.w
	};
}

Vec4 operator+(Vec4&& lhs, const Vec4& rhs)
{
	return
	{
		lhs.x + rhs.x,
		lhs.y + rhs.y,
		lhs.z + rhs.z,
		lhs.w + rhs.w
	};
}
Vec4 operator+(Vec4&& lhs, Vec4&& rhs)
{
	return
	{
		lhs.x + rhs.x,
		lhs.y + rhs.y,
		lhs.z + rhs.z,
		lhs.w + rhs.w
	};
}
Vec4 operator-(Vec4&& lhs, const Vec4& rhs)
{
	return
	{
		lhs.x - rhs.x,
		lhs.y - rhs.y,
		lhs.z - rhs.z,
		lhs.w - rhs.w
	};
}
Vec4 operator-(Vec4&& lhs, Vec4&& rhs)
{
	return
	{
		lhs.x - rhs.x,
		lhs.y - rhs.y,
		lhs.z - rhs.z,
		lhs.w - rhs.w
	};
}
Vec4 operator*(Vec4&& lhs, const Vec4& rhs)
{
	return
	{
		lhs.x * rhs.x,
		lhs.y * rhs.y,
		lhs.z * rhs.z,
		lhs.w * rhs.w
	};
}
Vec4 operator*(Vec4&& lhs, Vec4&& rhs)
{
	return
	{
		lhs.x * rhs.x,
		lhs.y * rhs.y,
		lhs.z * rhs.z,
		lhs.w * rhs.w
	};
}
Vec4 operator/(Vec4&& lhs, const Vec4& rhs)
{
	return
	{
		lhs.x / rhs.x,
		lhs.y / rhs.y,
		lhs.z / rhs.z,
		lhs.w / rhs.w
	};
}
Vec4 operator/(Vec4&& lhs, Vec4&& rhs)
{
	return
	{
		lhs.x / rhs.x,
		lhs.y / rhs.y,
		lhs.z / rhs.z,
		lhs.w / rhs.w
	};
}

Vec4 operator*(const Vec4& lhs, float rhs)
{
	return
	{
		lhs.x * rhs,
		lhs.y * rhs,
		lhs.z * rhs,
		lhs.w * rhs
	};
}
Vec4 operator/(const Vec4& lhs, float rhs)
{
	return
	{
		lhs.x / rhs,
		lhs.y / rhs,
		lhs.z / rhs,
		lhs.w / rhs
	};
}
Vec4 operator*(Vec4&& lhs, float rhs)
{
	return
	{
		lhs.x * rhs,
		lhs.y * rhs,
		lhs.z * rhs,
		lhs.w * rhs
	};
}
Vec4 operator/(Vec4&& lhs, float rhs)
{
	return
	{
		lhs.x / rhs,
		lhs.y / rhs,
		lhs.z / rhs,
		lhs.w / rhs
	};
}

Vec4 operator*(const Vec4& lhs, const Mat3& rhs)
{
	if (lhs.w == 0.0f) // Vector treated as [x, y, z]
	{
		return
		{
			lhs.x * rhs.data[0] + lhs.y * rhs.data[3] + lhs.z * rhs.data[6],
			lhs.x * rhs.data[1] + lhs.y * rhs.data[4] + lhs.z * rhs.data[7],
			lhs.x * rhs.data[2] + lhs.y * rhs.data[5] + lhs.z * rhs.data[8],
			0.0f
		};
	}
	else // Vector treated as [x/w, y/w, z/w]
	{
		return
		{
			lhs.x / lhs.w * rhs.data[0] + lhs.y / lhs.w * rhs.data[3] + lhs.z / lhs.w * rhs.data[6],
			lhs.x / lhs.w * rhs.data[1] + lhs.y / lhs.w * rhs.data[4] + lhs.z / lhs.w * rhs.data[7],
			lhs.x / lhs.w * rhs.data[2] + lhs.y / lhs.w * rhs.data[5] + lhs.z / lhs.w * rhs.data[8],
			1.0f
		};
	}
}
Vec4 operator*(const Vec4& lhs, Mat3&& rhs)
{
	if (lhs.w == 0.0f) // Vector treated as [x, y, z]
	{
		return
		{
			lhs.x * rhs.data[0] + lhs.y * rhs.data[3] + lhs.z * rhs.data[6],
			lhs.x * rhs.data[1] + lhs.y * rhs.data[4] + lhs.z * rhs.data[7],
			lhs.x * rhs.data[2] + lhs.y * rhs.data[5] + lhs.z * rhs.data[8],
			0.0f
		};
	}
	else // Vector treated as [x/w, y/w, z/w]
	{
		return
		{
			lhs.x / lhs.w * rhs.data[0] + lhs.y / lhs.w * rhs.data[3] + lhs.z / lhs.w * rhs.data[6],
			lhs.x / lhs.w * rhs.data[1] + lhs.y / lhs.w * rhs.data[4] + lhs.z / lhs.w * rhs.data[7],
			lhs.x / lhs.w * rhs.data[2] + lhs.y / lhs.w * rhs.data[5] + lhs.z / lhs.w * rhs.data[8],
			1.0f
		};
	}
}
Vec4 operator*(const Vec4& lhs, const Mat4& rhs)
{
	return
	{
		lhs.x * rhs.data[0] + lhs.y * rhs.data[4] + lhs.z * rhs.data[8] + lhs.w * rhs.data[12],
		lhs.x * rhs.data[1] + lhs.y * rhs.data[5] + lhs.z * rhs.data[9] + lhs.w * rhs.data[13],
		lhs.x * rhs.data[2] + lhs.y * rhs.data[6] + lhs.z * rhs.data[10] + lhs.w * rhs.data[14],
		lhs.x * rhs.data[3] + lhs.y * rhs.data[7] + lhs.z * rhs.data[11] + lhs.w * rhs.data[15]
	};
}
Vec4 operator*(const Vec4& lhs, Mat4&& rhs)
{
	return
	{
		lhs.x * rhs.data[0] + lhs.y * rhs.data[4] + lhs.z * rhs.data[8] + lhs.w * rhs.data[12],
		lhs.x * rhs.data[1] + lhs.y * rhs.data[5] + lhs.z * rhs.data[9] + lhs.w * rhs.data[13],
		lhs.x * rhs.data[2] + lhs.y * rhs.data[6] + lhs.z * rhs.data[10] + lhs.w * rhs.data[14],
		lhs.x * rhs.data[3] + lhs.y * rhs.data[7] + lhs.z * rhs.data[11] + lhs.w * rhs.data[15]
	};
}

Vec4 operator*(Vec4&& lhs, const Mat3& rhs)
{
	if (lhs.w == 0.0f) // Vector treated as [x, y, z]
	{
		return
		{
			lhs.x * rhs.data[0] + lhs.y * rhs.data[3] + lhs.z * rhs.data[6],
			lhs.x * rhs.data[1] + lhs.y * rhs.data[4] + lhs.z * rhs.data[7],
			lhs.x * rhs.data[2] + lhs.y * rhs.data[5] + lhs.z * rhs.data[8],
			0.0f
		};
	}
	else // Vector treated as [x/w, y/w, z/w]
	{
		return
		{
			lhs.x / lhs.w * rhs.data[0] + lhs.y / lhs.w * rhs.data[3] + lhs.z / lhs.w * rhs.data[6],
			lhs.x / lhs.w * rhs.data[1] + lhs.y / lhs.w * rhs.data[4] + lhs.z / lhs.w * rhs.data[7],
			lhs.x / lhs.w * rhs.data[2] + lhs.y / lhs.w * rhs.data[5] + lhs.z / lhs.w * rhs.data[8],
			1.0f
		};
	}
}
Vec4 operator*(Vec4&& lhs, Mat3&& rhs)
{
	if (lhs.w == 0.0f) // Vector treated as [x, y, z]
	{
		return
		{
			lhs.x * rhs.data[0] + lhs.y * rhs.data[3] + lhs.z * rhs.data[6],
			lhs.x * rhs.data[1] + lhs.y * rhs.data[4] + lhs.z * rhs.data[7],
			lhs.x * rhs.data[2] + lhs.y * rhs.data[5] + lhs.z * rhs.data[8],
			0.0f
		};
	}
	else // Vector treated as [x/w, y/w, z/w]
	{
		return
		{
			lhs.x / lhs.w * rhs.data[0] + lhs.y / lhs.w * rhs.data[3] + lhs.z / lhs.w * rhs.data[6],
			lhs.x / lhs.w * rhs.data[1] + lhs.y / lhs.w * rhs.data[4] + lhs.z / lhs.w * rhs.data[7],
			lhs.x / lhs.w * rhs.data[2] + lhs.y / lhs.w * rhs.data[5] + lhs.z / lhs.w * rhs.data[8],
			1.0f
		};
	}
}
Vec4 operator*(Vec4&& lhs, const Mat4& rhs)
{
	return
	{
		lhs.x * rhs.data[0] + lhs.y * rhs.data[4] + lhs.z * rhs.data[8] + lhs.w * rhs.data[12],
		lhs.x * rhs.data[1] + lhs.y * rhs.data[5] + lhs.z * rhs.data[9] + lhs.w * rhs.data[13],
		lhs.x * rhs.data[2] + lhs.y * rhs.data[6] + lhs.z * rhs.data[10] + lhs.w * rhs.data[14],
		lhs.x * rhs.data[3] + lhs.y * rhs.data[7] + lhs.z * rhs.data[11] + lhs.w * rhs.data[15]
	};
}
Vec4 operator*(Vec4&& lhs, Mat4&& rhs)
{
	return
	{
		lhs.x * rhs.data[0] + lhs.y * rhs.data[4] + lhs.z * rhs.data[8] + lhs.w * rhs.data[12],
		lhs.x * rhs.data[1] + lhs.y * rhs.data[5] + lhs.z * rhs.data[9] + lhs.w * rhs.data[13],
		lhs.x * rhs.data[2] + lhs.y * rhs.data[6] + lhs.z * rhs.data[10] + lhs.w * rhs.data[14],
		lhs.x * rhs.data[3] + lhs.y * rhs.data[7] + lhs.z * rhs.data[11] + lhs.w * rhs.data[15]
	};
}

Vec4 operator*(float lhs, const Vec4& rhs)
{
	return rhs * lhs;
};
Vec4 operator*(float lhs, Vec4&& rhs)
{
	return rhs * lhs;
};

// Mat3 operators

Mat3& Mat3::operator*=(const Mat3& other)
{
	float oldVals[9];
	memcpy(oldVals, data, sizeof(data));

	for (uint_fast8_t i = 0; i < 3; i++)
	{
		for (uint_fast8_t j = 0; j < 3; j++)
		{
			data[i * 3 + j] =
				oldVals[i * 3] * other.data[j] +
				oldVals[i * 3 + 1] * other.data[j + 3] +
				oldVals[i * 3 + 2] * other.data[j + 6];
		}
	}
	return *this;
}
Mat3& Mat3::operator*=(Mat3&& other)
{
	float oldVals[9];
	memcpy(oldVals, data, sizeof(data));

	for (uint_fast8_t i = 0; i < 3; i++)
	{
		for (uint_fast8_t j = 0; j < 3; j++)
		{
			data[i * 3 + j] =
				oldVals[i * 3] * other.data[j] +
				oldVals[i * 3 + 1] * other.data[j + 3] +
				oldVals[i * 3 + 2] * other.data[j + 6];
		}
	}
	return *this;
}

Mat3 operator*(const Mat3& lhs, const Mat3& rhs)
{
	Mat3 returnVal;
	for (uint_fast8_t i = 0; i < 3; i++)
	{
		for (uint_fast8_t j = 0; j < 3; j++)
		{
			returnVal.data[i * 3 + j] =
				lhs.data[i * 3] * rhs.data[j] +
				lhs.data[i * 3 + 1] * rhs.data[j + 3] +
				lhs.data[i * 3 + 2] * rhs.data[j + 6];
		}
	}
	return returnVal;
}
Mat3 operator*(const Mat3& lhs, Mat3&& rhs)
{
	Mat3 returnVal;
	for (uint_fast8_t i = 0; i < 3; i++)
	{
		for (uint_fast8_t j = 0; j < 3; j++)
		{
			returnVal.data[i * 3 + j] =
				lhs.data[i * 3] * rhs.data[j] +
				lhs.data[i * 3 + 1] * rhs.data[j + 3] +
				lhs.data[i * 3 + 2] * rhs.data[j + 6];
		}
	}
	return returnVal;
}
Mat3 operator*(Mat3&& lhs, const Mat3& rhs)
{
	Mat3 returnVal;
	for (uint_fast8_t i = 0; i < 3; i++)
	{
		for (uint_fast8_t j = 0; j < 3; j++)
		{
			returnVal.data[i * 3 + j] =
				lhs.data[i * 3] * rhs.data[j] +
				lhs.data[i * 3 + 1] * rhs.data[j + 3] +
				lhs.data[i * 3 + 2] * rhs.data[j + 6];
		}
	}
	return returnVal;
}
Mat3 operator*(Mat3&& lhs, Mat3&& rhs)
{
	Mat3 returnVal;
	for (uint_fast8_t i = 0; i < 3; i++)
	{
		for (uint_fast8_t j = 0; j < 3; j++)
		{
			returnVal.data[i * 3 + j] =
				lhs.data[i * 3] * rhs.data[j] +
				lhs.data[i * 3 + 1] * rhs.data[j + 3] +
				lhs.data[i * 3 + 2] * rhs.data[j + 6];
		}
	}
	return returnVal;
}

Vec3 operator*(const Mat3& lhs, const Vec3& rhs)
{
	return
	{
		lhs.data[0] * rhs.x + lhs.data[1] * rhs.y + lhs.data[2] * rhs.z,
		lhs.data[3] * rhs.x + lhs.data[4] * rhs.y + lhs.data[5] * rhs.z,
		lhs.data[6] * rhs.x + lhs.data[7] * rhs.y + lhs.data[8] * rhs.z
	};
}
Vec3 operator*(const Mat3& lhs, Vec3&& rhs)
{
	return
	{
		lhs.data[0] * rhs.x + lhs.data[1] * rhs.y + lhs.data[2] * rhs.z,
		lhs.data[3] * rhs.x + lhs.data[4] * rhs.y + lhs.data[5] * rhs.z,
		lhs.data[6] * rhs.x + lhs.data[7] * rhs.y + lhs.data[8] * rhs.z
	};
}
Vec3 operator*(Mat3&& lhs, const Vec3& rhs)
{
	return
	{
		lhs.data[0] * rhs.x + lhs.data[1] * rhs.y + lhs.data[2] * rhs.z,
		lhs.data[3] * rhs.x + lhs.data[4] * rhs.y + lhs.data[5] * rhs.z,
		lhs.data[6] * rhs.x + lhs.data[7] * rhs.y + lhs.data[8] * rhs.z
	};
}
Vec3 operator*(Mat3&& lhs, Vec3&& rhs)
{
	return
	{
		lhs.data[0] * rhs.x + lhs.data[1] * rhs.y + lhs.data[2] * rhs.z,
		lhs.data[3] * rhs.x + lhs.data[4] * rhs.y + lhs.data[5] * rhs.z,
		lhs.data[6] * rhs.x + lhs.data[7] * rhs.y + lhs.data[8] * rhs.z
	};
}

// Mat4 operators

Mat4& Mat4::operator*=(const Mat4& other)
{
	float oldVals[16];
	memcpy(oldVals, data, sizeof(data));

	for (uint_fast8_t i = 0; i < 4; i++)
	{
		for (uint_fast8_t j = 0; j < 4; j++)
		{
			data[i * 4 + j] =
				oldVals[i * 4] * other.data[j] +
				oldVals[i * 4 + 1] * other.data[j + 4] +
				oldVals[i * 4 + 2] * other.data[j + 8] +
				oldVals[i * 4 + 3] * other.data[j + 12];
		}
	}
	return *this;
}
Mat4& Mat4::operator*=(Mat4&& other)
{
	float oldVals[16];
	memcpy(oldVals, data, sizeof(data));

	for (uint_fast8_t i = 0; i < 4; i++)
	{
		for (uint_fast8_t j = 0; j < 4; j++)
		{
			data[i * 4 + j] =
				oldVals[i * 4] * other.data[j] +
				oldVals[i * 4 + 1] * other.data[j + 4] +
				oldVals[i * 4 + 2] * other.data[j + 8] +
				oldVals[i * 4 + 3] * other.data[j + 12];
		}
	}
	return *this;
}

Mat4 operator*(const Mat4& lhs, const Mat4& rhs)
{
	Mat4 returnVal;
	for (uint_fast8_t i = 0; i < 4; i++)
	{
		for (uint_fast8_t j = 0; j < 4; j++)
		{
			returnVal.data[i * 4 + j] =
				lhs.data[i * 4] * rhs.data[j] +
				lhs.data[i * 4 + 1] * rhs.data[j + 4] +
				lhs.data[i * 4 + 2] * rhs.data[j + 8] +
				lhs.data[i * 4 + 3] * rhs.data[j + 12];
		}
	}
	return returnVal;
}
Mat4 operator*(const Mat4& lhs, Mat4&& rhs)
{
	Mat4 returnVal;
	for (uint_fast8_t i = 0; i < 4; i++)
	{
		for (uint_fast8_t j = 0; j < 4; j++)
		{
			returnVal.data[i * 4 + j] =
				lhs.data[i * 4] * rhs.data[j] +
				lhs.data[i * 4 + 1] * rhs.data[j + 4] +
				lhs.data[i * 4 + 2] * rhs.data[j + 8] +
				lhs.data[i * 4 + 3] * rhs.data[j + 12];
		}
	}
	return returnVal;
}
Mat4 operator*(Mat4&& lhs, const Mat4& rhs)
{
	Mat4 returnVal;
	for (uint_fast8_t i = 0; i < 4; i++)
	{
		for (uint_fast8_t j = 0; j < 4; j++)
		{
			returnVal.data[i * 4 + j] =
				lhs.data[i * 4] * rhs.data[j] +
				lhs.data[i * 4 + 1] * rhs.data[j + 4] +
				lhs.data[i * 4 + 2] * rhs.data[j + 8] +
				lhs.data[i * 4 + 3] * rhs.data[j + 12];
		}
	}
	return returnVal;
}
Mat4 operator*(Mat4&& lhs, Mat4&& rhs)
{
	Mat4 returnVal;
	for (uint_fast8_t i = 0; i < 4; i++)
	{
		for (uint_fast8_t j = 0; j < 4; j++)
		{
			returnVal.data[i * 4 + j] =
				lhs.data[i * 4] * rhs.data[j] +
				lhs.data[i * 4 + 1] * rhs.data[j + 4] +
				lhs.data[i * 4 + 2] * rhs.data[j + 8] +
				lhs.data[i * 4 + 3] * rhs.data[j + 12];
		}
	}
	return returnVal;
}

Vec4 operator*(const Mat4& lhs, const Vec4& rhs)
{
	return
	{
		lhs.data[0] * rhs.x + lhs.data[1] * rhs.y + lhs.data[2] * rhs.z + lhs.data[3] * rhs.w,
		lhs.data[4] * rhs.x + lhs.data[5] * rhs.y + lhs.data[6] * rhs.z + lhs.data[7] * rhs.w,
		lhs.data[8] * rhs.x + lhs.data[9] * rhs.y + lhs.data[10] * rhs.z + lhs.data[11] * rhs.w,
		lhs.data[12] * rhs.x + lhs.data[13] * rhs.y + lhs.data[14] * rhs.z + lhs.data[15] * rhs.w
	};
}
Vec4 operator*(const Mat4& lhs, Vec4&& rhs)
{
	return
	{
		lhs.data[0] * rhs.x + lhs.data[1] * rhs.y + lhs.data[2] * rhs.z + lhs.data[3] * rhs.w,
		lhs.data[4] * rhs.x + lhs.data[5] * rhs.y + lhs.data[6] * rhs.z + lhs.data[7] * rhs.w,
		lhs.data[8] * rhs.x + lhs.data[9] * rhs.y + lhs.data[10] * rhs.z + lhs.data[11] * rhs.w,
		lhs.data[12] * rhs.x + lhs.data[13] * rhs.y + lhs.data[14] * rhs.z + lhs.data[15] * rhs.w
	};
}
Vec4 operator*(Mat4&& lhs, const Vec4& rhs)
{
	return
	{
		lhs.data[0] * rhs.x + lhs.data[1] * rhs.y + lhs.data[2] * rhs.z + lhs.data[3] * rhs.w,
		lhs.data[4] * rhs.x + lhs.data[5] * rhs.y + lhs.data[6] * rhs.z + lhs.data[7] * rhs.w,
		lhs.data[8] * rhs.x + lhs.data[9] * rhs.y + lhs.data[10] * rhs.z + lhs.data[11] * rhs.w,
		lhs.data[12] * rhs.x + lhs.data[13] * rhs.y + lhs.data[14] * rhs.z + lhs.data[15] * rhs.w
	};
}
Vec4 operator*(Mat4&& lhs, Vec4&& rhs)
{
	return
	{
		lhs.data[0] * rhs.x + lhs.data[1] * rhs.y + lhs.data[2] * rhs.z + lhs.data[3] * rhs.w,
		lhs.data[4] * rhs.x + lhs.data[5] * rhs.y + lhs.data[6] * rhs.z + lhs.data[7] * rhs.w,
		lhs.data[8] * rhs.x + lhs.data[9] * rhs.y + lhs.data[10] * rhs.z + lhs.data[11] * rhs.w,
		lhs.data[12] * rhs.x + lhs.data[13] * rhs.y + lhs.data[14] * rhs.z + lhs.data[15] * rhs.w
	};
}

// Matrix functions

Mat3 Matrices::Identity3()
{
	return
	{
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
}
Mat4 Matrices::Identity4()
{
	return
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Mat4 Matrices::Translation(float x, float y, float z)
{
	return
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		x,    y,    z,    1.0f
	};
}
Mat4 Matrices::Translation(Vec3 trans)
{
	return
	{
		1.0f,    0.0f,    0.0f,    0.0f,
		0.0f,    1.0f,    0.0f,    0.0f,
		0.0f,    0.0f,    1.0f,    0.0f,
		trans.x, trans.y, trans.z, 1.0f
	};
}
Mat4 Matrices::Rotation(float roll, float pitch, float yaw)
{
	return
	{
		cos(roll) * cos(yaw), cos(roll) * sin(yaw) * sin(pitch) - sin(roll) * cos(pitch), cos(roll) * sin(yaw) * cos(pitch) + sin(roll) * sin(pitch), 0.0f,
		sin(roll) * cos(yaw), sin(roll) * sin(yaw) * sin(pitch) + cos(roll) * cos(pitch), sin(roll) * sin(yaw) * cos(pitch) - cos(roll) * sin(pitch), 0.0f,
		-sin(yaw),            cos(yaw) * sin(pitch),                                      cos(yaw) * cos(pitch),                                      0.0f,
		0.0f,                 0.0f,                                                       0.0f,                                                       1.0f
	};
}
Mat4 Matrices::Rotation(Vec3 rot)
{
	return
	{
		cos(rot.x) * cos(rot.z), cos(rot.x) * sin(rot.z) * sin(rot.y) - sin(rot.x) * cos(rot.y), cos(rot.x) * sin(rot.z) * cos(rot.y) + sin(rot.x) * sin(rot.y), 0.0f,
		sin(rot.x) * cos(rot.z), sin(rot.x) * sin(rot.z) * sin(rot.y) + cos(rot.x) * cos(rot.y), sin(rot.x) * sin(rot.z) * cos(rot.y) - cos(rot.x) * sin(rot.y), 0.0f,
		-sin(rot.z),             cos(rot.z) * sin(rot.y),                                        cos(rot.z) * cos(rot.y),                                        0.0f,
		0.0f,                    0.0f,                                                           0.0f,                                                           1.0f
	};
}
Mat4 Matrices::Scale(float scale)
{
	return
	{
		scale, 0.0f, 0.0f, 0.0f,
		0.0f, scale, 0.0f, 0.0f,
		0.0f, 0.0f, scale, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}
Mat4 Matrices::Scale(float x, float y, float z)
{
	return
	{
		x, 0.0f, 0.0f, 0.0f,
		0.0f, y, 0.0f, 0.0f,
		0.0f, 0.0f, z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}
Mat4 Matrices::Scale(Vec3 scale)
{
	return
	{
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f,    1.0f
	};
}

Mat4 Matrices::Orthographic(float left, float right, float bottom, float top, float nearClip, float farClip)
{
	return
	{
		2.0f / (right - left),             0.0f,                             0.0f,                                        0.0f,
		0.0f,                              2.0f / (top - bottom),            0.0f,                                        0.0f,
		0.0f,                              0.0f,                             2.0f / (farClip - nearClip),                 0.0f,
		-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(farClip - nearClip) / (farClip - nearClip), 1.0f
	};
}
Mat4 Matrices::Frustrum(float left, float right, float bottom, float top, float nearClip, float farClip)
{
	return
	{
		2.0f * nearClip / (right - left), 0.0f,                             0.0f,                                             0.0f,
		0.0f,                             2.0f * nearClip / (top - bottom), 0.0f,                                             0.0f,
		-(right + left) / (right - left), -(top + bottom) / (top - bottom), (farClip + nearClip) / (farClip - nearClip),      1.0f,
		0.0f,                             0.0f,                            -2.0f * farClip * nearClip / (farClip - nearClip), 0.0f
	};
}
Mat4 Matrices::Perspective(float fov, float aspectRatio, float nearClip, float farClip)
{
	float top = tan(fov / 2) * nearClip;
	float bottom = -top;
	float right = top * aspectRatio;
	float left = -right;

	return Frustrum(left, right, bottom, top, nearClip, farClip);
}

}
