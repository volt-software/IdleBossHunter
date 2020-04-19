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

#include <concurrentqueue.h>

using namespace std;

namespace ibh {
    template <typename queue_T>
    struct queue_abstraction {
        explicit queue_abstraction(moodycamel::ConcurrentQueue<queue_T> *_q) : q(_q), ptok(*q) {}
        queue_abstraction(const queue_abstraction &) = delete;
        queue_abstraction(queue_abstraction &&) = delete;
        queue_abstraction& operator=(const queue_abstraction &) = delete;
        queue_abstraction& operator=(queue_abstraction &&) = delete;

        template <typename T>
        void enqueue(T&& t) {
            if(!q->enqueue(ptok, forward<T>(t))){
                throw runtime_error("Couldn't enqueue, probably because of memory allocation issues");
            }
        }

        template <typename T>
        void enqueue_tokenless(T&& t) {
            if(!q->enqueue(forward<T>(t))){
                throw runtime_error("Couldn't enqueue, probably because of memory allocation issues");
            }
        }

        bool try_dequeue_from_producer(queue_T &t) {
            return q->try_dequeue_from_producer(ptok, t);
        }

        bool try_dequeue(queue_T &t) {
            return q->try_dequeue(t);
        }

        bool try_dequeue(moodycamel::ConsumerToken& token, queue_T &t) {
            return q->try_dequeue(token, t);
        }

        moodycamel::ConcurrentQueue<queue_T> *q;
        moodycamel::ProducerToken ptok;
    };
}