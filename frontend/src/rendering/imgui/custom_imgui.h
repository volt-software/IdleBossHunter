/*
    IdleBossHunter client
    Copyright (C) 2020  Michael de Lang

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <cmath>

#ifdef __EMSCRIPTEN__
#include <wasm_simd128.h>
#else
#include <emmintrin.h>
#endif

static inline float  ImFabs(float x)                                            { return fabsf(x); }

#ifdef __EMSCRIPTEN__
static inline float  ImSqrt(float x)                                            { return wasm_f32x4_sqrt(wasm_f32x4_make(x, 0, 0, 0))[0]; }
#else
static inline float  ImSqrt(float x)                                            { return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(x))); }
#endif

static inline float  ImPow(float x, float y)                                    { return powf(x, y); }
static inline double ImPow(double x, double y)                                  { return pow(x, y); }
static inline float  ImFmod(float x, float y)                                   { return fmodf(x, y); }
static inline double ImFmod(double x, double y)                                 { return fmod(x, y); }
static inline float  ImCos(float x)                                             { return cosf(x); }
static inline float  ImSin(float x)                                             { return sinf(x); }
static inline float  ImAcos(float x)                                            { return acosf(x); }
static inline float  ImAtan2(float y, float x)                                  { return atan2f(y, x); }
static inline double ImAtof(const char* s)                                      { return atof(s); }
static inline float  ImFloorStd(float x)                                        { return floorf(x); }   // we already uses our own ImFloor() { return (float)(int)v } internally so the standard one wrapper is named differently (it's used by stb_truetype)
static inline float  ImCeil(float x)                                            { return ceilf(x); }
