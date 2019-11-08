/*
    Realm of Aesir client
    Copyright (C) 2016  Michael de Lang

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

#include "timer.h"

#include <chrono>

using namespace std::chrono;

template<typename tdur>
timer<tdur>::timer() noexcept : _start_ticks(0), _paused_ticks(0), _paused(false), _started(false) {
}

template<typename tdur>
void timer<tdur>::start() noexcept {
    _started = true;
    _paused = false;

    _start_ticks = duration_cast<tdur>(time_point_cast<tdur>(hr_clock::now()).time_since_epoch()).count();
    _paused_ticks = 0;
}

template<typename tdur>
void timer<tdur>::stop() noexcept {
    _started = false;
    _paused = false;

    _start_ticks = 0;
    _paused_ticks = 0;
}

template<typename tdur>
void timer<tdur>::pause() noexcept {
    if(_started && !_paused) {
        _paused = true;

        _paused_ticks = duration_cast<tdur>(time_point_cast<tdur>(hr_clock::now()).time_since_epoch()).count() - _start_ticks;
        _start_ticks = 0;
    }
}

template<typename tdur>
void timer<tdur>::unpause() noexcept {
    if(_started && _paused) {
        _paused = false;

        _start_ticks = duration_cast<tdur>(time_point_cast<tdur>(hr_clock::now()).time_since_epoch()).count() - _paused_ticks;
        _paused_ticks = 0;
    }
}

template<typename tdur>
uint64_t timer<tdur>::get_ticks() const noexcept {
    if(!_started) {
        return 0;
    }

    if(_paused) {
        return _paused_ticks;
    }

    return duration_cast<tdur>(time_point_cast<tdur>(hr_clock::now()).time_since_epoch()).count() - _start_ticks;
}

template<typename tdur>
bool timer<tdur>::is_started() const noexcept {
    return _started;
}

template<typename tdur>
bool timer<tdur>::is_paused() const noexcept {
    return _paused;
}

template class timer<milliseconds>;
template class timer<microseconds>;
template class timer<nanoseconds>;