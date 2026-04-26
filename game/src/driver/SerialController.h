#pragma once
#include <termios.h>  // POSIX terminal control definitions

#include <string>

class SerialController
{
   private:
    int serialPort = -1;
    bool connected = false;
    char readBuffer[256];

    // Input States
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;

   public:
    SerialController();
    ~SerialController();

    // Returns true if connection successful
    // Device example: "/dev/ttyACM0" (Linux) or "/dev/cu.usbmodem..." (Mac)
    bool Connect(const std::string& portName, int baudRate = 9600);
    void Disconnect();

    // Call this at the start of every frame
    void Update();

    // Getters for Game Logic
    bool IsUp() const { return up; }
    bool IsDown() const { return down; }
    bool IsLeft() const { return left; }
    bool IsRight() const { return right; }
    bool IsConnected() const { return connected; }
};
