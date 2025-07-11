#pragma once
#include <raylib.h>

inline Vector2 operator+(const Vector2& l, const Vector2& r) { return {l.x + r.x, l.y + r.y}; }
inline Vector2 operator-(const Vector2& l, const Vector2& r) { return {l.x - r.x, l.y - r.y}; }
inline Vector2 operator*(const Vector2& l, const Vector2& r) { return {l.x * r.x, l.y * r.y}; }
inline Vector2 operator/(const Vector2& l, const Vector2& r) { return {l.x / r.x, l.y / r.y}; }

inline Vector2 operator+(const Vector2& l, const float& r) { return {l.x + r, l.y + r}; }
inline Vector2 operator-(const Vector2& l, const float& r) { return {l.x - r, l.y - r}; }
inline Vector2 operator*(const Vector2& l, const float& r) { return {l.x * r, l.y * r}; }
inline Vector2 operator/(const Vector2& l, const float& r) { return {l.x / r, l.y / r}; }

