#include "vec2.h"
#include <math.h>

vec2::vec2()
{
}

vec2::vec2(float xin, float yin)
	: x(xin), y(yin)
{
}

bool vec2::operator == (const vec2& rhs) const
{
	return (x == rhs.x && y == rhs.y);
}

bool vec2::operator != (const vec2& rhs) const
{
	return (x != rhs.x || y != rhs.y);
}

vec2 vec2::operator + (const vec2& rhs) const
{
	return vec2(x + rhs.x, y + rhs.y);
}

vec2 vec2::operator - (const vec2& rhs) const
{
	return vec2(x - rhs.x, y - rhs.y);
}

vec2 vec2::operator * (const float val) const
{
	return vec2(x * val, y * val);
}

vec2 vec2::operator / (const float val) const
{
	return vec2(x / val, y / val);
}

void vec2::operator += (const vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;
}

void vec2::operator -= (const vec2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
}

void vec2::operator *= (const float val)
{
	x *= val;
	y *= val;
}

void vec2::operator /= (const float val)
{
	x /= val;
	y /= val;
}

float vec2::dist(const vec2& rhs) const
{
	return sqrt((rhs.x - x) * (rhs.x - x) + (rhs.y - y) * (rhs.y - y));
}

float vec2::distSqrd(const vec2& rhs) const
{
	return (rhs.x - x) * (rhs.x - x) + (rhs.y - y) * (rhs.y - y);
}
