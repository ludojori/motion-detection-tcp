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
	bool buttonAlive;
	std::thread clickCheckThread;

public:    
    Button(std::function<void()> onClick);

	void operator=(Button& rhs) = delete;
    void checkClicked(std::function<void()> onClick);

	~Button();
};


#endif /*BUTTON_H*/
