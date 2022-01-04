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

#ifndef WATCH_LIBRARY
#define WATCH_LIBRARY

#define BTN_NONE    0
#define BTN_CLICK   1
#define BTN_LONG_CLICK  2
#define BTN_DOWN    10

namespace Watch {
    struct Event {
        unsigned long currentMillis = 0;
        virtual void initialize(unsigned long currentMillis);
        virtual bool isTriggered(unsigned long currentMillis) = 0;
        void (*callback) (Event *evt);
        Event(void (*callback) (Event *evt)) : callback(callback) { }
    };

    struct DigitalPin : Event {
       uint8_t pin;
       uint8_t debounce;
       uint8_t value;
       void initialize(unsigned long currentMillis) override;
       bool isTriggered(unsigned long currentMillis) override;
       DigitalPin(uint8_t pin, void (*callback) (Event *evt), uint8_t debounce = 50) :  Event(callback), pin(pin), debounce(debounce) { }

       private:
           unsigned long lastDebounce = 0;
    };

    struct Button : DigitalPin {
        uint8_t state = BTN_NONE;
        uint16_t longClickTimeMs;
        uint8_t clickPinStatus;
        bool isTriggered(unsigned long currentMillis) override;
        Button(uint8_t pin, void (*callback) (Event *evt), uint8_t debounce = 50, uint16_t longClickTimeMs = 1000, uint8_t clickPinStatus = LOW) 
            : DigitalPin(pin, callback, debounce),
            longClickTimeMs(longClickTimeMs),
            clickPinStatus(clickPinStatus)
        { }

        private:
            unsigned long lastDebounce = 0;
     };

    struct Timer : Event {
        unsigned int time;
        void initialize(unsigned long currentMillis) override;
        bool isTriggered(unsigned long currentMillis) override;
        void change(int time, unsigned long currentMillis, bool enabled = true);
        Timer(unsigned int time, void (*callback) (Event *evt), bool enabled = true) :  Event(callback), time(time), enabled(enabled) { }

        private:
            bool enabled;
            unsigned long lastMillis = 0;
     };

    struct Memory : Event {
        void initialize(unsigned long currentMillis) override;
        bool isTriggered(unsigned long currentMillis) override;
        Memory(void *memory, unsigned long size, void (*callback) (Event *evt)) :  Event(callback), _memory((uint8_t *)memory), _size((size_t)size) { }

        private:
            uint8_t *_memory;
            uint8_t *_current;
            size_t _size;

            void copy(uint8_t *src, uint8_t *dst, size_t size);
     };

    void setup(Event *ctx[], const unsigned long currentMillis);
    void loop(Event *ctx[], const unsigned long currentMillis);
}
#endif