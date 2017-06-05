#pragma once
#include <fstream>

class GpioInterface
{
public:

    using value_type = bool;
    enum class GpioDirection {
        Input,
        Output
    };

    virtual bool exportGpio(int gpio) = 0;
    virtual bool isValidGpio(int gpio) = 0;
    virtual bool canSetDirection(int gpio) = 0;

    virtual bool setDirectionInput(int gpio) = 0;
    virtual bool setDirectionOutput(int gpio, bool value_type) = 0;
    virtual GpioDirection getDirection(int gpio) = 0;

    virtual bool setValue(int gpio, value_type value) = 0;
    virtual value_type getValue(int gpio) = 0;
};

static bool FileExists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

static bool FileWriteable(const std::string& path) {
    std::ofstream f(path);
    return f.good();
}


class SysfsGpio : public GpioInterface {
protected:
    static const std::string kSysfsRoot = "/sys";
    static const std::string kGpioRoot = kSysfsRoot + "/class/gpio";
    static std::string GpioBaseDir(int gpio) {
        return std::string(kGpioRoot + "gpio" + std::to_string(gpio));

public:

    SysfsGpio() {
    }

    ~SysfsGpio() {
    }

    bool exportGpio(int gpio) {
        std::ofstream f(kGpioRoot + "/export");
        if (!f)
            return false;

        std::string s = to_string(gpio);
        f << s;

        return isValidGpio(gpio);
    }

    bool isValidGpio(int gpio) {
        return FileExists(GpioBaseDir + "/value");
    }

    bool canSetDirection(int gpio) {
        return FileWriteable(GpioBaseDir + "/direction");
    }

    bool setDirectionInput(int gpio) {
        if (!canSetDirection(gpio))
            return false;

        std::ofstream f(GpioBaseDir + "/direction");
        f << "in";
        return f.good();
    }

    bool setDirectionOutput(int gpio, value_type value) {
        if (!canSetDirection(gpio))
            return false;

        std::ofstream f(GpioBaseDir + "/direction");
        if (value)
            f << "high";
        else
            f << "low";

        return f.good();
    }

    GpioDirection getDirection(int gpio) {
        std::ifstream f(GpioBaseDir + "/direction");
        std::string direction;

       f >> direction;
       if (f.fail())
           return false;
    }

    bool setValue(int gpio, value_type value) = 0;
    value_type getValue(int gpio) = 0;
};

class GpioInterfaceFactory
{
public:
    static GpioInterface *interface()
    {
        if (!m_interface)
	    return m_interface;

	std::lock_guard<std::mutex> guard;
	if (!m_interface)
	{
            m_interface = new SysfsGpio;
	}
    }
private:
    std::atomic<GpioInterface*> m_interface;
    std::mutex m_lock;
};
