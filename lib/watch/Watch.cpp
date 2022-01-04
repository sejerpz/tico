/*
 *
 * Copyright (C) 2022 Andrea Del Signore
 *
 * This file is part of TiCO.
 * 
 * TiCO is free software: you can redistribute it and/or modify it under the terms of 
 * the GNU General Public License as published by the Free Software Foundation, 
 * either version 3 of the License, or (at your option) any later version.
 *
 * TiCo is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with Foobar. 
 * If not, see <https://www.gnu.org/licenses/>. 
 * 
 * Author:
 *    Andrea Del Signore <sejerpz@gmail.com>
 * 
 */

#include <Arduino.h>
#include "Watch.h"

namespace Watch {
    void Event::initialize(unsigned long currentMillis) {
    }

    void DigitalPin::initialize(unsigned long currentMillis) {
        value = digitalRead(pin);
        lastDebounce = 0;
        Event::initialize(currentMillis);
    }

    bool DigitalPin::isTriggered(unsigned long currentMillis) {
        uint8_t currentValue = digitalRead(pin);

        if (currentValue != value) {
            lastDebounce = currentMillis;
            value = currentValue;
        } else if (lastDebounce != 0) {
            if (currentMillis - lastDebounce > debounce) {
                lastDebounce = 0;
                return true;
            }
        }

        return false;
    }


    bool Button::isTriggered(unsigned long currentMillis) {
        if (DigitalPin::isTriggered(currentMillis)) {
            if (DigitalPin::value == clickPinStatus) {
                state = BTN_DOWN;
                lastDebounce = currentMillis;
            } else  {
                if (state == BTN_DOWN) {
                    state = BTN_CLICK;
                    return true;
                } else {
                    state = BTN_NONE;
                }
            }
        } else if (state == BTN_DOWN && currentMillis - lastDebounce > longClickTimeMs) {
            state = BTN_LONG_CLICK;
            return true;
        }
        
        return false;
    }


    void Timer::initialize(unsigned long currentMillis) {
        lastMillis = currentMillis;
        Event::initialize(currentMillis);
    }

    bool Timer::isTriggered(unsigned long currentMillis) {
        if (enabled && (currentMillis - lastMillis >= time)) {
            lastMillis = currentMillis;
            return true;
        } else {
            return false;
        }
    }

    void Timer::change(int time, unsigned long currentMillis, bool enabled) {
        this->time = time;
        this->enabled = enabled;
        lastMillis = currentMillis;
    }

    void Memory::initialize(unsigned long currentMillis) {
        _current = new uint8_t[_size];
        copy(_memory, _current, _size);
        Event::initialize(currentMillis);
    }

    bool Memory::isTriggered(unsigned long currentMillis) {
        for(size_t i=0; i < _size;i++) {
            if (_current[i] != _memory[i]) {
                copy(_memory, _current, _size);
                return true;
            }
        }

        return false;
    }

    void Memory::copy(uint8_t *src, uint8_t *dst, size_t size) {
        for(size_t i=0; i < size;i++) {
            dst[i] = src[i];
        }
    }
    
    void setup(Event *ctx[], const unsigned long currentMillis) {
        int i=0;
        Event *evt = ctx[i];
        while (evt != NULL) {
            evt->initialize(currentMillis);
            evt = ctx[++i];
        }
    }

    void loop(Event *ctx[],const unsigned long currentMillis) {
        int i=0;
        Event *evt = ctx[i];
        while (evt != NULL) {
            if (evt->isTriggered(currentMillis)) {
                evt->currentMillis = currentMillis;
                evt->callback(evt);
            }

            evt = ctx[++i];
        }
    }
}