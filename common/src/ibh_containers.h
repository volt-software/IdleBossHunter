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

#pragma once

#include <robin_hood.h>

#ifdef USE_WYHASH
#include <wyhash.h>
#else
#include <xxh3.h>
#endif

#include <string>
#include <memory>
#include <unordered_set>

using namespace std;

namespace ibh {
    template <typename ... Ts>
    constexpr size_t tuple_sum_size(tuple<Ts...> const &)
    {
        return (sizeof(Ts) + ...);
    }

    template <class T>
    constexpr bool compare_weak_ptr(weak_ptr<T> const &lhs, weak_ptr<T> const &rhs) {
        return !lhs.owner_before(rhs) && !rhs.owner_before(lhs);
    }

#ifdef USE_WYHASH
    template<class Key>
    class custom_hash
    {
    public:
        size_t operator()(Key t) const
        {
            return wyhash(&t, sizeof(t), 1283474321412);
        }
    };

    template<>
    class custom_hash<weak_ptr<void>>
    {
    public:
        size_t operator()(weak_ptr<void> const &key) const
        {
            auto p = key.lock();
            return wyhash(p.get(), sizeof(p.get()), 1283474321412);
        }
    };

    template<>
    class custom_hash<string>
    {
    public:
        size_t operator()(string const &key) const
        {
            return wyhash(key.c_str(), key.size(), 1283474321412);
        }

        size_t operator()(string_view const &key) const
        {
            return wyhash(&key[0], key.size(), 1283474321412);
        }
    };

    template<>
    class custom_hash<tuple<uint64_t, uint64_t>>
    {
    public:
        size_t operator()(tuple<uint64_t, uint64_t> t) const
        {
            return wyhash(&t, tuple_sum_size(t), 1283474321412);
        }
    };

    template<>
    class custom_hash<tuple<int32_t, int32_t>>
    {
    public:
        size_t operator()(tuple<int32_t, int32_t> t) const
        {
            return wyhash(&t, tuple_sum_size(t), 1283474321412);
        }
    };
#else

    template<class Key>
    class custom_hash
    {
    public:
        size_t operator()(Key t) const
        {
            return XXH3_64bits(&t, sizeof(t));
        }
    };

    template<>
    class custom_hash<weak_ptr<void>>
    {
    public:
        size_t operator()(weak_ptr<void> const &key) const
        {
            auto p = key.lock();
            return XXH3_64bits(p.get(), sizeof(p.get()));
        }
    };

    template<>
    class custom_hash<string>
    {
    public:
        size_t operator()(string const &key) const
        {
            return XXH3_64bits(key.c_str(), key.size());
        }

        size_t operator()(string_view const &key) const
        {
            return XXH3_64bits(&key[0], key.size());
        }
    };

    template<>
    class custom_hash<tuple<uint64_t, uint64_t>>
    {
    public:
        size_t operator()(tuple<uint64_t, uint64_t> t) const
        {
            return XXH3_64bits(&t, tuple_sum_size(t));
        }
    };

    template<>
    class custom_hash<tuple<int32_t, int32_t>>
    {
    public:
        size_t operator()(tuple<int32_t, int32_t> t) const
        {
            return XXH3_64bits(&t, tuple_sum_size(t));
        }
    };
#endif

    template<>
    class custom_hash<uint32_t>
    {
    public:
        size_t operator()(uint32_t t) const
        {
            return t;
        }
    };

    template<>
    class custom_hash<uint64_t>
    {
    public:
        size_t operator()(uint64_t t) const
        {
            return t;
        }
    };

    template<>
    class custom_hash<int32_t>
    {
    public:
        size_t operator()(int32_t t) const
        {
            return t;
        }
    };

    template<>
    class custom_hash<int64_t>
    {
    public:
        size_t operator()(int64_t t) const
        {
            return t;
        }
    };

    template<class Key>
    class custom_equalto
    {
    public:
        bool operator()(Key const &lhs, Key const &rhs) const
        {
            return lhs == rhs;
        }
    };

    template<>
    class custom_equalto<weak_ptr<void>>
    {
    public:
        bool operator()(weak_ptr<void> const &lhs, weak_ptr<void> const &rhs) const
        {
            return compare_weak_ptr(lhs, rhs);
        }
    };

    template<>
    class custom_equalto<string>
    {
    public:
        bool operator()(string const &lhs, string const &rhs) const
        {
            return lhs == rhs;
        }

        bool operator()(string const &lhs, string_view const &rhs) const
        {
            return lhs == rhs;
        }

        bool operator()(string_view const &lhs, string const &rhs) const
        {
            return lhs == rhs;
        }
    };

    template<>
    class custom_equalto<tuple<uint64_t, uint64_t>>
    {
    public:
        bool operator()(tuple<uint64_t, uint64_t> const &lhs, tuple<uint64_t, uint64_t> const &rhs) const
        {
            return lhs == rhs;
        }
    };

    template<>
    class custom_equalto<tuple<int32_t, int32_t>>
    {
    public:
        bool operator()(tuple<int32_t, int32_t> const &lhs, tuple<int32_t, int32_t> const &rhs) const
        {
            return lhs == rhs;
        }
    };

    template <typename Key, typename T>
    using ibh_flat_map = robin_hood::unordered_flat_map<Key, T, custom_hash<Key>, custom_equalto<Key>>;
    template <typename Key>
    using ibh_unordered_set = unordered_set<Key, custom_hash<Key>, custom_equalto<Key>, allocator<Key>>;
}
