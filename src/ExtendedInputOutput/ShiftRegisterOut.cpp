#include "ShiftRegisterOut.h"
#include "ExtendedInputOutput.h"
#include <cstring>

using namespace ExtIO;

ShiftRegisterOut::ShiftRegisterOut(pin_t dataPin, pin_t clockPin, pin_t latchPin, uint8_t bitOrder, pin_t length)
    : dataPin(dataPin), clockPin(clockPin), latchPin(latchPin), bitOrder(bitOrder), ExtendedIOElement(length)
{
    this->length = length;
    bufferLength = (length + 7) / 8;
    stateBuffer = (uint8_t *)malloc(bufferLength);
    begin();
}
ShiftRegisterOut::~ShiftRegisterOut()
{
    free(stateBuffer);
}

void ShiftRegisterOut::digitalWrite(pin_t pin, uint8_t val)
{
    int8_t bufferIndex = pinToBufferIndex(pin);
    if (bufferIndex == INVALID_PIN)
        return;
    if (val)
        stateBuffer[bufferIndex] |= pinToBitMask(pin);
    else
        stateBuffer[bufferIndex] &= ~pinToBitMask(pin);
    refresh();
}

int ShiftRegisterOut::digitalRead(pin_t pin)
{
    int8_t bufferIndex = pinToBufferIndex(pin);
    if (bufferIndex == INVALID_PIN)
        return 0;
    return !!(stateBuffer[bufferIndex] & pinToBitMask(pin));
}

void ShiftRegisterOut::begin()
{
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(latchPin, OUTPUT);
    reset();
}

void ShiftRegisterOut::refresh()
{
    digitalWrite(latchPin, LOW);
    if (bitOrder == LSBFIRST)
        for (uint8_t i = 0; i < bufferLength; i++)
            shiftOut(dataPin, clockPin, LSBFIRST, stateBuffer[i]);
    else
        for (int8_t i = bufferLength - 1; i >= 0; i--)
            shiftOut(dataPin, clockPin, MSBFIRST, stateBuffer[i]);
    digitalWrite(latchPin, HIGH);
}

void ShiftRegisterOut::reset()
{
    memset(stateBuffer, 0, bufferLength);
    refresh();
}

pin_t ShiftRegisterOut::green(pin_t id)
{
    return 3 * id + greenBit + start;
}
pin_t ShiftRegisterOut::red(pin_t id)
{
    return 3 * id + redBit + start;
}
pin_t ShiftRegisterOut::blue(pin_t id)
{
    return 3 * id + blueBit + start;
}

int8_t ShiftRegisterOut::pinToBufferIndex(pin_t pin)
{
    if (pin >= length)
        return INVALID_PIN;
    return pin >> 3; // pin / 8;
}
uint8_t ShiftRegisterOut::pinToBitMask(pin_t pin)
{
    return 1 << (pin & 7); // 1 << (pin % 8);
}