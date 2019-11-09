/**
 * The contents of this file are based on the article posted at the
 * following location:
 *
 *   http://crascit.com/2015/06/03/on-leaving-scope-part-2/
 *
 * The material in that article has some commonality with the code made
 * available as part of Facebook's folly library at:
 *
 *   https://github.com/facebook/folly/blob/master/folly/ScopeGuard.h
 *
 * Furthermore, similar material is currently part of a draft proposal
 * to the C++ standards committee, referencing the same work by Andrei
 * Alexandresu that led to the folly implementation. The draft proposal
 * can be found at:
 *
 *   http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4189.pdf
 *
 * With the above in mind, the content below is made available under
 * the same license terms as folly to minimize any legal concerns.
 * Should there be ambiguity over copyright ownership between Facebook
 * and myself for any material included in this file, it should be
 * interpreted that Facebook is the copyright owner for the ambiguous
 * section of code concerned.
 *
 *   Craig Scott
 *   3rd June 2015
 *
 * ----------------------------------------------------------------------
 *
 * Copyright 2015 Craig Scott
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

namespace lotr {
    template<typename Func>
    class on_leaving_scope_class {
    public:
        // Prevent copying
        on_leaving_scope_class(const on_leaving_scope_class &) = delete;

        on_leaving_scope_class &operator=(const on_leaving_scope_class &) = delete;

        // Allow moving
        on_leaving_scope_class(on_leaving_scope_class &&other) noexcept :
                m_func(move(other.m_func)),
                m_active(other.m_active) {
            other.m_active = false;
        }

        // Accept lvalue function objects
        explicit on_leaving_scope_class(const Func &f) :
                m_func(f),
                m_active(true) {
        }

        // Accept rvalue function objects
        explicit on_leaving_scope_class(Func &&f) :
                m_func(move(f)),
                m_active(true) {
        }

        // Only invoke function object if it
        // hasn't been moved elsewhere
        ~on_leaving_scope_class() {
            if (m_active)
                m_func();
        }

    private:
        Func m_func;
        bool m_active;
    };

    template<typename Func>
    on_leaving_scope_class<typename decay<Func>::type>
    on_leaving_scope(Func &&f) {
        return on_leaving_scope_class<typename decay<Func>::type>(forward<Func>(f));
    }
}

