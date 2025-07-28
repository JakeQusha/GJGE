#pragma once
#include <raylib.h>

inline auto operator+(const Vector2& l, const Vector2& r) -> Vector2 { return {l.x + r.x, l.y + r.y}; }
inline auto operator-(const Vector2& l, const Vector2& r) -> Vector2 { return {l.x - r.x, l.y - r.y}; }
inline auto operator*(const Vector2& l, const Vector2& r) -> Vector2 { return {l.x * r.x, l.y * r.y}; }
inline auto operator/(const Vector2& l, const Vector2& r) -> Vector2 { return {l.x / r.x, l.y / r.y}; }

inline auto operator+(const Vector2& l, const float& r) -> Vector2 { return {l.x + r, l.y + r}; }
inline auto operator-(const Vector2& l, const float& r) -> Vector2 { return {l.x - r, l.y - r}; }
inline auto operator*(const Vector2& l, const float& r) -> Vector2 { return {l.x * r, l.y * r}; }
inline auto operator/(const Vector2& l, const float& r) -> Vector2 { return {l.x / r, l.y / r}; }

inline auto operator+(const Vector3& l, const Vector3& r) -> Vector3 { return {l.x + r.x, l.y + r.y, l.z + r.z}; }
inline auto operator-(const Vector3& l, const Vector3& r) -> Vector3 { return {l.x - r.x, l.y - r.y, l.z - r.z}; }
inline auto operator*(const Vector3& l, const Vector3& r) -> Vector3 { return {l.x * r.x, l.y * r.y, l.z * r.z}; }
inline auto operator/(const Vector3& l, const Vector3& r) -> Vector3 { return {l.x / r.x, l.y / r.y, l.z / r.z}; }

inline auto operator+(const Vector3& l, const float& r) -> Vector3 { return {l.x + r, l.y + r, l.z + r}; }
inline auto operator-(const Vector3& l, const float& r) -> Vector3 { return {l.x - r, l.y - r, l.z - r}; }
inline auto operator*(const Vector3& l, const float& r) -> Vector3 { return {l.x * r, l.y * r, l.z * r}; }
inline auto operator/(const Vector3& l, const float& r) -> Vector3 { return {l.x / r, l.y / r, l.z / r}; }

