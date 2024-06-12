#pragma once

class vec2
{
public:

	float x = 0;
	float y = 0;

	vec2();
	vec2(float xin, float yin);

	bool operator == (const vec2& rhs) const;
	bool operator != (const vec2& rhs) const;

	vec2 operator + (const vec2& rhs) const;
	vec2 operator - (const vec2& rhs) const;
	vec2 operator * (const float val) const;
	vec2 operator / (const float val) const;

	void operator += (const vec2& rhs);
	void operator -= (const vec2& rhs);
	void operator *= (const float val);
	void operator /= (const float val);

	float dist(const vec2& rhs) const;
	float distSqrd(const vec2& rhs) const;
};