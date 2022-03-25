#include "gmock/gmock.h"
#include "../include/motion-detection-tcp/button.h"

class MockButton : public Button
{
public:
    MockButton(std::function<void()> onClick) : Button(onClick) {}
    MOCK_METHOD(void, checkClicked, (std::function<void()> onClick), (override));
};