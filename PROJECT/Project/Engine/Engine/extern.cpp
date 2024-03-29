#include "pch.h"
#include "extern.h"


tTransform g_transform = {};

constexpr Vec3 Vec3::Right = Vec3(1.f, 0.f, 0.f);
constexpr Vec3 Vec3::Up    = Vec3(0.f, 1.f, 0.f);
constexpr Vec3 Vec3::Front = Vec3(0.f, 0.f, 1.f);

tGlobal g_global = {};
