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


// stable map guarantees a map that doesn't invalidate iterators on assign/remove
//#define USE_STABLE_MAP

#ifdef USE_STABLE_MAP
#include <unordered_map>
#else
#include <robin_hood.h>
#endif

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
    constexpr size_t tuple_sum_size(tuple<Ts...> const &) noexcept
    {
        return (sizeof(Ts) + ...);
    }

    template <class T>
    constexpr bool compare_weak_ptr(weak_ptr<T> const &lhs, weak_ptr<T> const &rhs) noexcept {
        return !lhs.owner_before(rhs) && !rhs.owner_before(lhs);
    }

#ifdef USE_WYHASH
    template<class Key>
    struct custom_hash
    {
        size_t operator()(Key t) const noexcept
        {
            return wyhash(&t, sizeof(t), 1283474321412);
        }
    };

    template<>
    struct custom_hash<weak_ptr<void>>
    {
        size_t operator()(weak_ptr<void> const &key) const noexcept
        {
            auto p = key.lock();
            return wyhash(p.get(), sizeof(p.get()), 1283474321412);
        }
    };

    template<>
    struct custom_hash<string>
    {
        size_t operator()(string const &key) const noexcept
        {
            return wyhash(key.c_str(), key.size(), 1283474321412);
        }

        size_t operator()(string_view const &key) const noexcept
        {
            return wyhash(&key[0], key.size(), 1283474321412);
        }
    };

    template<>
    struct custom_hash<tuple<uint64_t, uint64_t>>
    {
        size_t operator()(tuple<uint64_t, uint64_t> t) const noexcept
        {
            return wyhash(&t, tuple_sum_size(t), 1283474321412);
        }
    };

    template<>
    struct custom_hash<tuple<int32_t, int32_t>>
    {
        size_t operator()(tuple<int32_t, int32_t> t) const noexcept
        {
            return wyhash(&t, tuple_sum_size(t), 1283474321412);
        }
    };
#else

    template<class Key>
    struct custom_hash
    {
        size_t operator()(Key t) const noexcept
        {
            return XXH3_64bits(&t, sizeof(t));
        }
    };

    template<>
    struct custom_hash<weak_ptr<void>>
    {
        size_t operator()(weak_ptr<void> const &key) const noexcept
        {
            auto p = key.lock();
            return XXH3_64bits(p.get(), sizeof(p.get()));
        }
    };

    template<>
    struct custom_hash<string>
    {
        size_t operator()(string const &key) const noexcept
        {
            return XXH3_64bits(key.c_str(), key.size());
        }

        size_t operator()(string_view const &key) const noexcept
        {
            return XXH3_64bits(&key[0], key.size());
        }
    };

    template<>
    struct custom_hash<tuple<uint64_t, uint64_t>>
    {
        size_t operator()(tuple<uint64_t, uint64_t> t) const noexcept
        {
            return XXH3_64bits(&t, tuple_sum_size(t));
        }
    };

    template<>
    struct custom_hash<tuple<int32_t, int32_t>>
    {
        size_t operator()(tuple<int32_t, int32_t> t) const noexcept
        {
            return XXH3_64bits(&t, tuple_sum_size(t));
        }
    };
#endif

    template<>
    struct custom_hash<uint32_t>
    {
        size_t operator()(uint32_t t) const noexcept
        {
            return t;
        }
    };

    template<>
    struct custom_hash<uint64_t>
    {
        size_t operator()(uint64_t t) const noexcept
        {
            return t;
        }
    };

    template<>
    struct custom_hash<int32_t>
    {
        size_t operator()(int32_t t) const noexcept
        {
            return t;
        }
    };

    template<>
    struct custom_hash<int64_t>
    {
        size_t operator()(int64_t t) const noexcept
        {
            return t;
        }
    };

    template<class Key>
    struct custom_equalto
    {
        bool operator()(Key const &lhs, Key const &rhs) const noexcept
        {
            return lhs == rhs;
        }
    };

    template<>
    struct custom_equalto<weak_ptr<void>>
    {
        bool operator()(weak_ptr<void> const &lhs, weak_ptr<void> const &rhs) const noexcept
        {
            return compare_weak_ptr(lhs, rhs);
        }
    };

    template<>
    struct custom_equalto<string>
    {
        bool operator()(string const &lhs, string const &rhs) const noexcept
        {
            return lhs == rhs;
        }

        bool operator()(string const &lhs, string_view const &rhs) const noexcept
        {
            return lhs == rhs;
        }

        bool operator()(string_view const &lhs, string const &rhs) const noexcept
        {
            return lhs == rhs;
        }
    };

    template<>
    struct custom_equalto<tuple<uint64_t, uint64_t>>
    {
        bool operator()(tuple<uint64_t, uint64_t> const &lhs, tuple<uint64_t, uint64_t> const &rhs) const noexcept
        {
            return lhs == rhs;
        }
    };

    template<>
    struct custom_equalto<tuple<int32_t, int32_t>>
    {
        bool operator()(tuple<int32_t, int32_t> const &lhs, tuple<int32_t, int32_t> const &rhs) const noexcept
        {
            return lhs == rhs;
        }
    };

#ifdef USE_STABLE_MAP

    // stolen from https://gist.github.com/facontidavide/95f20c28df8ec91729f9d8ab01e7d2df
    // unordered_map has lower performance but at least invalidates iterators less...
    template <typename T, typename Hash, typename Pred>
    class StringMap: public unordered_map<string, T, Hash, Pred>
    {
    public:
        typename unordered_map<string,T, Hash, Pred>::iterator find(const string_view& v )
        {
            tmp_.reserve(v.size());
            tmp_.assign(v.data(), v.size());
            return unordered_map<string, T, Hash, Pred>::find(tmp_);
        }

        typename unordered_map<string,T, Hash, Pred>::iterator find(const string& v )
        {
            return unordered_map<string, T, Hash, Pred>::find(v);
        }

        typename unordered_map<string,T, Hash, Pred>::iterator find(const char* v )
        {
            tmp_.assign(v);
            return unordered_map<string, T, Hash, Pred>::find(v);
        }

    private:
        thread_local static string tmp_;
    };

    template <typename T, typename Hash, typename Pred> thread_local string StringMap<T, Hash, Pred>::tmp_ = {};

    template <typename Key, typename T>
    struct ibh_flat_map_type {
        typedef unordered_map<Key, T, custom_hash<Key>, custom_equalto<Key>> type;
    };

    template <typename T>
    struct ibh_flat_map_type<string, T> {
        typedef StringMap<T, custom_hash<string>, custom_equalto<string>> type;
    };

    template <typename Key, typename T>
    using ibh_flat_map = typename ibh_flat_map_type<Key, T>::type;
#else
    template <typename Key, typename T>
    using ibh_flat_map = robin_hood::unordered_flat_map<Key, T, custom_hash<Key>, custom_equalto<Key>>;
#endif

    template <typename Key>
    using ibh_unordered_set = unordered_set<Key, custom_hash<Key>, custom_equalto<Key>, allocator<Key>>;
}
