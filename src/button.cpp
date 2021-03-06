#include "../include/motion-detection-tcp/button.h"

#include "../include/third-party/camera/camera.h"
#include "adafruit/bbio.h"

using namespace adafruit::bbio;

Button::Button(std::function<void()> onClick) : clickCheckThread(&Button::checkClicked, this, onClick), buttonAlive(true) {}

void Button::checkClicked(std::function<void()> onClick)
{
    adafruit::bbio::init(lib_options(LOG_DEBUG, nullptr, LOG_PERROR));
    Gpio gpio("P8_43", Gpio::Direction::Input);
    Gpio::Value position;

    while (buttonAlive)
    {
        position = gpio.get_value();
        if (position == Gpio::Value::Low)
        {
            onClick();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

Button::~Button()
{
    buttonAlive = false;
    clickCheckThread.join();    
}
