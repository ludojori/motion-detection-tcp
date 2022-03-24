#ifndef BUTTON_H
#define BUTTON_H

#include "../third-party/camera/camera.h"
#include "adafruit/bbio.h"

#include <thread>
#include <mutex>
#include <condition_variable>

#include <functional>

class Button
{
private:
    //bool buttonIsDown;
    //std::mutex clicked;
    //std::condition_variable clickedCondition;
	//std::function<void()> onClick;
	bool buttonAlive;
	std::thread clickCheckThread;

public:    
    Button(std::function<void()> onClick);

	void operator=(Button& rhs) = delete;
    void checkClicked(std::function<void()> onClick);

	~Button();
};


#endif /*BUTTON_H*/
/*
void ServerCommunication::
    changePic()
{
	adafruit::bbio::init(lib_options(LOG_DEBUG, nullptr, LOG_PERROR));
	Gpio gpio("P8_43", Gpio::Direction::Input);
	Gpio::Value position;

	while (true)
	{
		position = gpio.get_value();
		if (position == Gpio::Value::Low)
		{
			generateMovement();
			std::cout << "Picture changed!" << std::endl;
			int height, width;
			getResolution(&height, &width);
			int pixelsCount = height * width;

			unsigned int *pixels = new unsigned int[pixelsCount];

			getCurrentImage(pixels);
            //end
			notifyClients(pixels, pixelsCount);
			delete[] pixels;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
}


*/