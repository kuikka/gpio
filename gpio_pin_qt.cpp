#include <QDebug>
#include <QCoreApplication>
#include "gpio_pin_qt.h"

QtGpioPin::QtGpioPin(int pin, QObject *parent)
:    QObject(parent), GpioPin(pin)
{
    init();
}

QtGpioPin::~QtGpioPin()
{
}

bool QtGpioPin::isPinExported(int pin)
{
    qDebug() << pin;
    QString filePath = QString("/sys/class/gpio/gpio_%1/value").arg(pin);

    QFile f(filePath);

    return f.exists();
}

bool QtGpioPin::exportPin(int pin)
{
    qDebug() << pin;
    QString filePath("/sys/class/gpio/export");
    QString pinNumber = QString("%1\n").arg(pin);

    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly))
        return false;

    if (!f.write(pinNumber.toLatin1()))
    {
        qWarning() << "Unable to export pin " << pin;
        return false;
    }

    return true;
}

bool QtGpioPin::init()
{
    qDebug();
    if (!isPinExported(m_pin))
    {
        if (!exportPin(m_pin))
        {
            return false;
        }
    }

    QString filePath = QString("/sys/class/gpio/gpio_%1/value").arg(m_pin);

    m_file.setFileName(filePath);
    if (!m_file.exists())
    {
        qWarning() << "File " << filePath << " does not exist";
        return false;
    }

    if (m_file.open(QIODevice::ReadWrite))
    {
        m_valid = true;
        m_writeable = true;
    }
    else
    {
        if (m_file.open(QIODevice::ReadOnly))
        {
            m_valid = true;
            m_writeable = false;
        }
    }
    return m_valid;
}

bool QtGpioPin::isValid() const
{
    return m_valid;
}

bool QtGpioPin::canSetDirection() const
{
    QString filePath = QString("/sys/class/gpio/gpio_%1/direction").arg(m_pin);

    QFile f(filePath);
    if (!f.exists())
        return false;

    if (!f.open(QIODevice::WriteOnly))
        return false;

    return true;
}


bool QtGpioPin::setDirectionInput()
{
    QString filePath = QString("/sys/class/gpio/gpio_%1/direction").arg(m_pin);

    const char *in = "in\n";

    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly))
        return false;
    return f.write(in);
}

bool QtGpioPin::setDirectionOutput(int state)
{
    QString filePath = QString("/sys/class/gpio/gpio_%1/direction").arg(m_pin);

    const char *high = "high\n";
    const char *low = "low\n";
    const char *output = state ? high : low;

    QFile f(filePath);

    if (!f.open(QIODevice::WriteOnly))
        return false;

    return f.write(output);
}


GpioPin::GpioDirection QtGpioPin::getDirection() const
{
}

bool QtGpioPin::setValue(int value)
{
    static const char *low = "0\n";
    static const char *high = "1\n";
    const char *value_str = value ? high : low;

    return m_file.write(value_str);
}

bool QtGpioPin::getValue()
{
    QByteArray data = m_file.read(8);
    if (data.length() < 1)
        return false;

    if (data.data()[0] == '0')
        return false;

    return true;
}

bool QtGpioPin::notify()
{
    if (m_notifier)
        return true;

    m_notifier = new QSocketNotifier(m_file.handle(), QSocketNotifier::Exception, this);
    if (!m_notifier)
        return false;

    connect(m_notifier, SIGNAL(activated(int)), this, SLOT(OnFileChanged(int)));

    return true;
}

void QtGpioPin::OnFileChanged(int /* fd */)
{
    emit valueChanged();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QtGpioPin *pin = new QtGpioPin(100, &app);
    qDebug() << pin->isValid();

    app.exec();
}
