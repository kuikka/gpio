#pragma once

#include <QFile>
#include <QSocketNotifier>

#include "gpio_pin.h"


class QtGpioPin : public QObject, public GpioPin
{
    Q_OBJECT

public:
    QtGpioPin(int pin, QObject *parent = 0);
    virtual ~QtGpioPin();

    virtual bool isValid() const;
    virtual bool canSetDirection() const;
    virtual bool setDirectionInput();
    virtual bool setDirectionOutput(int state);
    virtual GpioDirection getDirection() const;

    virtual bool setValue(int value);
    virtual bool getValue();

    bool notify();

signals:
    void valueChanged();

private slots:
    void OnFileChanged(int fd);

protected:
    bool init();
    static bool isPinExported(int pin);
    static bool exportPin(int pin);

private:
    QFile m_file;
    bool m_valid = false;
    bool m_writeable = false;
    QSocketNotifier *m_notifier = nullptr;
};
