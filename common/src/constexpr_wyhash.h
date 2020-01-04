/*
    IdleBossHunter
    Copyright (C) 2020 Michael de Lang

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

#include <wyhash.h>
#include <string_view>

using namespace std;

#pragma once
namespace ibh {
    static constexpr uint64_t constexpr_wyrotr(uint64_t v, unsigned k) { return (v >> k) | (v << (64 - k)); }

    static constexpr uint64_t constexpr_wymum(uint64_t A, uint64_t B) {
#ifdef    WYHASH32
        uint64_t	hh=(A>>32)*(B>>32),	hl=(A>>32)*(unsigned)B,	lh=(unsigned)A*(B>>32),	ll=(uint64_t)(unsigned)A*(unsigned)B;
        return	constexpr_wyrotr(hl,32)^constexpr_wyrotr(lh,32)^hh^ll;
#else
#ifdef __SIZEOF_INT128__
        __uint128_t r = A;
        r *= B;
        return (r >> 64) ^ r;
#elif    defined(_MSC_VER) && defined(_M_X64)
        A=_umul128(A, B, &B);	return	A^B;
#else
        uint64_t	ha=A>>32,	hb=B>>32,	la=(uint32_t)A,	lb=(uint32_t)B,	hi, lo;
        uint64_t	rh=ha*hb,	rm0=ha*lb,	rm1=hb*la,	rl=la*lb,	t=rl+(rm0<<32),	c=t<rl;
        lo=t+(rm1<<32);	c+=lo<t;hi=rh+(rm0>>32)+(rm1>>32)+c;	return hi^lo;
#endif
#endif
    }

    template<typename T>
    static constexpr uint64_t constexpr_wyr8(const T *p) {
        uint64_t v = 0;
        v = ((uint64_t) p[0] << 56) | ((uint64_t) p[1] << 48) | ((uint64_t) p[2] << 40) | ((uint64_t) p[3] << 32) |
            (p[4] << 24) | (p[5] << 16) | (p[6] << 8) | p[7];
        return v;
    }

    template<typename T>
    static constexpr uint64_t constexpr_wyr4(const T *p) {
        uint32_t v = 0;
        v = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
        return v;
    }

    template<typename T>
    static constexpr uint64_t constexpr_wyr3(const T *p, unsigned k) {
        return (((uint64_t) p[0]) << 16) | (((uint64_t) p[k >> 1]) << 8) | p[k - 1];
    }

    template<typename T>
    static constexpr uint64_t constexpr_wyhash(const T *key, uint64_t len, uint64_t seed) {
        static_assert(sizeof(T) == 1, "T must be a char or uint8_t kind type");
#if defined(__GNUC__) || defined(__INTEL_COMPILER)
        if (__builtin_expect(!len, 0)) return 0;
#else
        if(!len)	return	0;
#endif
        const T *p = key;
        if (len < 4)
            return constexpr_wymum(constexpr_wymum(constexpr_wyr3(p, len) ^ seed ^ _wyp0, seed ^ _wyp1), len ^ _wyp4);
        else if (len <= 8)
            return constexpr_wymum(
                    constexpr_wymum(constexpr_wyr4(p) ^ seed ^ _wyp0, constexpr_wyr4(p + len - 4) ^ seed ^ _wyp1),
                    len ^ _wyp4);
        else if (len <= 16)
            return constexpr_wymum(
                    constexpr_wymum(constexpr_wyr8(p) ^ seed ^ _wyp0, constexpr_wyr8(p + len - 8) ^ seed ^ _wyp1),
                    len ^ _wyp4);
        else if (len <= 24)
            return constexpr_wymum(
                    constexpr_wymum(constexpr_wyr8(p) ^ seed ^ _wyp0, constexpr_wyr8(p + 8) ^ seed ^ _wyp1) ^
                    constexpr_wymum(constexpr_wyr8(p + len - 8) ^ seed ^ _wyp2, seed ^ _wyp3), len ^ _wyp4);
        else if (len <= 32)
            return constexpr_wymum(
                    constexpr_wymum(constexpr_wyr8(p) ^ seed ^ _wyp0, constexpr_wyr8(p + 8) ^ seed ^ _wyp1) ^
                    constexpr_wymum(constexpr_wyr8(p + 16) ^ seed ^ _wyp2, constexpr_wyr8(p + len - 8) ^ seed ^ _wyp3),
                    len ^ _wyp4);
        uint64_t see1 = seed, i = len;
        if (i >= 256)
            for (; i >= 256; i -= 256, p += 256) {
                seed = constexpr_wymum(constexpr_wyr8(p) ^ seed ^ _wyp0, constexpr_wyr8(p + 8) ^ seed ^ _wyp1) ^
                       constexpr_wymum(constexpr_wyr8(p + 16) ^ seed ^ _wyp2, constexpr_wyr8(p + 24) ^ seed ^ _wyp3);
                see1 = constexpr_wymum(constexpr_wyr8(p + 32) ^ see1 ^ _wyp1, constexpr_wyr8(p + 40) ^ see1 ^ _wyp2) ^
                       constexpr_wymum(constexpr_wyr8(p + 48) ^ see1 ^ _wyp3, constexpr_wyr8(p + 56) ^ see1 ^ _wyp0);
                seed = constexpr_wymum(constexpr_wyr8(p + 64) ^ seed ^ _wyp0, constexpr_wyr8(p + 72) ^ seed ^ _wyp1) ^
                       constexpr_wymum(constexpr_wyr8(p + 80) ^ seed ^ _wyp2, constexpr_wyr8(p + 88) ^ seed ^ _wyp3);
                see1 = constexpr_wymum(constexpr_wyr8(p + 96) ^ see1 ^ _wyp1, constexpr_wyr8(p + 104) ^ see1 ^ _wyp2) ^
                       constexpr_wymum(constexpr_wyr8(p + 112) ^ see1 ^ _wyp3, constexpr_wyr8(p + 120) ^ see1 ^ _wyp0);
                seed = constexpr_wymum(constexpr_wyr8(p + 128) ^ seed ^ _wyp0, constexpr_wyr8(p + 136) ^ seed ^ _wyp1) ^
                       constexpr_wymum(constexpr_wyr8(p + 144) ^ seed ^ _wyp2, constexpr_wyr8(p + 152) ^ seed ^ _wyp3);
                see1 = constexpr_wymum(constexpr_wyr8(p + 160) ^ see1 ^ _wyp1, constexpr_wyr8(p + 168) ^ see1 ^ _wyp2) ^
                       constexpr_wymum(constexpr_wyr8(p + 176) ^ see1 ^ _wyp3, constexpr_wyr8(p + 184) ^ see1 ^ _wyp0);
                seed = constexpr_wymum(constexpr_wyr8(p + 192) ^ seed ^ _wyp0, constexpr_wyr8(p + 200) ^ seed ^ _wyp1) ^
                       constexpr_wymum(constexpr_wyr8(p + 208) ^ seed ^ _wyp2, constexpr_wyr8(p + 216) ^ seed ^ _wyp3);
                see1 = constexpr_wymum(constexpr_wyr8(p + 224) ^ see1 ^ _wyp1, constexpr_wyr8(p + 232) ^ see1 ^ _wyp2) ^
                       constexpr_wymum(constexpr_wyr8(p + 240) ^ see1 ^ _wyp3, constexpr_wyr8(p + 248) ^ see1 ^ _wyp0);
            }
        for (; i >= 32; i -= 32, p += 32) {
            seed = constexpr_wymum(constexpr_wyr8(p) ^ seed ^ _wyp0, constexpr_wyr8(p + 8) ^ seed ^ _wyp1);
            see1 = constexpr_wymum(constexpr_wyr8(p + 16) ^ see1 ^ _wyp2, constexpr_wyr8(p + 24) ^ see1 ^ _wyp3);
        }
        if (!i) {}
        else if (i < 4) seed = constexpr_wymum(constexpr_wyr3(p, i) ^ seed ^ _wyp0, seed ^ _wyp1);
        else if (i <= 8)
            seed = constexpr_wymum(constexpr_wyr4(p) ^ seed ^ _wyp0, constexpr_wyr4(p + i - 4) ^ seed ^ _wyp1);
        else if (i <= 16)
            seed = constexpr_wymum(constexpr_wyr8(p) ^ seed ^ _wyp0, constexpr_wyr8(p + i - 8) ^ seed ^ _wyp1);
        else if (i <= 24) {
            seed = constexpr_wymum(constexpr_wyr8(p) ^ seed ^ _wyp0, constexpr_wyr8(p + 8) ^ seed ^ _wyp1);
            see1 = constexpr_wymum(constexpr_wyr8(p + i - 8) ^ see1 ^ _wyp2, see1 ^ _wyp3);
        }
        else {
            seed = constexpr_wymum(constexpr_wyr8(p) ^ seed ^ _wyp0, constexpr_wyr8(p + 8) ^ seed ^ _wyp1);
            see1 = constexpr_wymum(constexpr_wyr8(p + 16) ^ see1 ^ _wyp2, constexpr_wyr8(p + i - 8) ^ see1 ^ _wyp3);
        }
        return constexpr_wymum(seed ^ see1, len ^ _wyp4);
    }

    int constexpr string_length(const char *str) {
        return *str ? 1 + string_length(str + 1) : 0;
    }

    template<typename T>
    constexpr auto type_name() {
        string_view name;
        string_view prefix;
        string_view suffix;
#ifdef __clang__
        name = __PRETTY_FUNCTION__;
        prefix = "auto type_name() [T = ";
        suffix = "]";
#elif defined(__GNUC__)
        name = __PRETTY_FUNCTION__;
        prefix = "constexpr auto type_name() [with T = ";
        suffix = "]";
#elif defined(_MSC_VER)
        name = __FUNCSIG__;
        prefix = "auto __cdecl type_name<";
        suffix = ">(void)";
#endif
        name.remove_prefix(prefix.size());
        name.remove_suffix(suffix.size());
        return name;
    }

    template<typename T>
    constexpr uint64_t generate_type() {
        string_view name = type_name<T>();
        return constexpr_wyhash(&name[0], name.size(), 0);
    }
}