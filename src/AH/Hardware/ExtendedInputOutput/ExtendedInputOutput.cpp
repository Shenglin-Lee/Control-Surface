#include <AH/Settings/Warnings.hpp>
AH_DIAGNOSTIC_WERROR() // Enable errors on warnings

#include "ExtendedIOElement.hpp"
#include "ExtendedInputOutput.hpp"
#include <AH/Error/Error.hpp>

BEGIN_AH_NAMESPACE

namespace ExtIO {

bool isNativePin(pin_t pin) {
    return pin < NUM_DIGITAL_PINS + NUM_ANALOG_INPUTS;
}

template <class T>
bool inRange(T target, T start, T end) {
    return target >= start && target < end;
}

ExtendedIOElement *getIOElementOfPin(pin_t pin) {
    for (auto &el : ExtendedIOElement::getAll())
        if (pin < el.getStart())
            break;
        else if (inRange(pin, el.getStart(), el.getEnd()))
            return &el;

    FATAL_ERROR(
        F("The given pin does not correspond to an Extended IO element."),
        0x8888);
    return nullptr;
}

void pinMode(pin_t pin, PinMode_t mode) {
    if (pin == NO_PIN)
        return; // LCOV_EXCL_LINE
    else if (isNativePin(pin)) {
        ::pinMode(pin, mode);
    } else {
        auto el = getIOElementOfPin(pin);
        el->pinMode(pin - el->getStart(), mode);
    }
}
void pinMode(int pin, PinMode_t mode) { ::pinMode(pin, mode); }

void digitalWrite(pin_t pin, PinStatus_t val) {
    if (pin == NO_PIN)
        return; // LCOV_EXCL_LINE
    else if (isNativePin(pin)) {
        ::digitalWrite(pin, val);
    } else {
        auto el = getIOElementOfPin(pin);
        el->digitalWrite(pin - el->getStart(), val);
    }
}
void digitalWrite(int pin, PinStatus_t val) { ::digitalWrite(pin, val); }

PinStatus_t digitalRead(pin_t pin) {
    if (pin == NO_PIN)
        return 0; // LCOV_EXCL_LINE
    else if (isNativePin(pin)) {
        return ::digitalRead(pin);
    } else {
        auto el = getIOElementOfPin(pin);
        return el->digitalRead(pin - el->getStart());
    }
    return 0;
}
PinStatus_t digitalRead(int pin) { return ::digitalRead(pin); }

analog_t analogRead(pin_t pin) {
    if (pin == NO_PIN)
        return 0; // LCOV_EXCL_LINE
    else if (isNativePin(pin)) {
        return ::analogRead(pin);
    } else {
        auto el = getIOElementOfPin(pin);
        return el->analogRead(pin - el->getStart());
    }
    return 0;
}
analog_t analogRead(int pin) { return ::analogRead(pin); }

void analogWrite(pin_t pin, analog_t val) {
    if (pin == NO_PIN)
        return; // LCOV_EXCL_LINE
    else if (isNativePin(pin)) {
#ifndef ESP32
        ::analogWrite(pin, val);
#endif
    } else {
        auto el = getIOElementOfPin(pin);
        el->analogWrite(pin - el->getStart(), val);
    }
}
void analogWrite(pin_t pin, int val) { analogWrite(pin, (analog_t)val); }
#ifndef ESP32
void analogWrite(int pin, analog_t val) { ::analogWrite(pin, val); }
void analogWrite(int pin, int val) { ::analogWrite(pin, val); }
#endif

void pinModeBuffered(pin_t pin, PinMode_t mode) {
    if (pin == NO_PIN)
        return; // LCOV_EXCL_LINE
    else if (isNativePin(pin)) {
        ::pinMode(pin, mode);
    } else {
        auto el = getIOElementOfPin(pin);
        el->pinModeBuffered(pin - el->getStart(), mode);
    }
}

void digitalWriteBuffered(pin_t pin, PinStatus_t val) {
    if (pin == NO_PIN)
        return; // LCOV_EXCL_LINE
    else if (isNativePin(pin)) {
        ::digitalWrite(pin, val);
    } else {
        auto el = getIOElementOfPin(pin);
        el->digitalWriteBuffered(pin - el->getStart(), val);
    }
}

PinStatus_t digitalReadBuffered(pin_t pin) {
    if (pin == NO_PIN)
        return 0; // LCOV_EXCL_LINE
    else if (isNativePin(pin)) {
        return ::digitalRead(pin);
    } else {
        auto el = getIOElementOfPin(pin);
        return el->digitalReadBuffered(pin - el->getStart());
    }
    return 0;
}

analog_t analogReadBuffered(pin_t pin) {
    if (pin == NO_PIN)
        return 0; // LCOV_EXCL_LINE
    else if (isNativePin(pin)) {
        return ::analogRead(pin);
    } else {
        auto el = getIOElementOfPin(pin);
        return el->analogReadBuffered(pin - el->getStart());
    }
    return 0;
}

void analogWriteBuffered(pin_t pin, analog_t val) {
    if (pin == NO_PIN)
        return; // LCOV_EXCL_LINE
    else if (isNativePin(pin)) {
#ifndef ESP32
        ::analogWrite(pin, val);
#endif
    } else {
        auto el = getIOElementOfPin(pin);
        el->analogWriteBuffered(pin - el->getStart(), val);
    }
}
void analogWriteBuffered(pin_t pin, int val) {
    analogWriteBuffered(pin, (analog_t)val);
}

void shiftOut(pin_t dataPin, pin_t clockPin, BitOrder_t bitOrder, uint8_t val) {
    // Native version
    if (isNativePin(dataPin) && isNativePin(clockPin)) {
        ::shiftOut((int)dataPin, (int)clockPin, bitOrder, val);
    }
    // ExtIO version
    else if (!isNativePin(dataPin) && !isNativePin(clockPin)) {
        auto dataEl = getIOElementOfPin(dataPin);
        auto dataPinN = dataPin - dataEl->getStart();
        auto clockEl = getIOElementOfPin(clockPin);
        auto clockPinN = clockPin - clockEl->getStart();

        for (uint8_t i = 0; i < 8; i++) {
            uint8_t mask = bitOrder == LSBFIRST ? (1 << i) : (1 << (7 - i));
            dataEl->digitalWrite(dataPinN, (val & mask) ? HIGH : LOW);
            clockEl->digitalWrite(clockPinN, HIGH);
            clockEl->digitalWrite(clockPinN, LOW);
        }
    }
    // Mixed version (slow)
    else {
        for (uint8_t i = 0; i < 8; i++) {
            uint8_t mask = bitOrder == LSBFIRST ? (1 << i) : (1 << (7 - i));
            digitalWrite(dataPin, (val & mask) ? HIGH : LOW);

            digitalWrite(clockPin, HIGH);
            digitalWrite(clockPin, LOW);
        }
    }
}
void shiftOut(int dataPin, int clockPin, BitOrder_t bitOrder, uint8_t val) {
    ::shiftOut(dataPin, clockPin, bitOrder, val);
}

} // namespace ExtIO

END_AH_NAMESPACE

AH_DIAGNOSTIC_POP()