#pragma once


class GpioPin
{
public:
    enum class GpioDirection {
        Input,
        Output,
        Unknown,
    };

    GpioPin(int pin) : m_pin(pin) {};
    virtual ~GpioPin() {};

    virtual bool isValid() const = 0;
    virtual bool canSetDirection() const = 0;
    virtual bool setDirectionInput() = 0;
    virtual bool setDirectionOutput(int state) = 0;
    virtual GpioDirection getDirection() const = 0;

    virtual bool setValue(int value) = 0;
    virtual bool getValue() = 0;

protected:
    int m_pin;
};

