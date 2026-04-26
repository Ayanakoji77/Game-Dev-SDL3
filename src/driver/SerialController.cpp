#include "SerialController.h"

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <termios.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

SerialController::SerialController() {}

SerialController::~SerialController() { Disconnect(); }

bool SerialController::Connect(const std::string& portName, int baudRate)
{
    serialPort = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

    if (serialPort == -1)
    {
        perror("Unable to open serial port");
        return false;
    }

    // Configure the serial port
    struct termios tty;
    if (tcgetattr(serialPort, &tty) != 0)
    {
        perror("Error from tcgetattr");
        return false;
    }

    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag |= CREAD | CLOCAL;

    cfmakeraw(&tty);

    if (tcsetattr(serialPort, TCSANOW, &tty) != 0)
    {
        perror("Error from tcsetattr");
        return false;
    }

    connected = true;
    std::cout << "Controller Connected on " << portName << std::endl;
    return true;
}

void SerialController::Disconnect()
{
    if (connected && serialPort >= 0)
    {
        close(serialPort);
        connected = false;
    }
}

void SerialController::Update()
{
    if (!connected)
        return;

    char buffer[1];
    int n = read(serialPort, buffer, 1);

    if (n > 0)
    {
        uint8_t byte = static_cast<uint8_t>(buffer[0]);

        // Decode using Bitwise AND (&)
        // Check Bit 0 (UP)
        up = (byte & (1 << 0)) != 0;

        // Check Bit 1 (DOWN)
        down = (byte & (1 << 1)) != 0;

        // Check Bit 2 (LEFT)
        left = (byte & (1 << 2)) != 0;

        // Check Bit 3 (RIGHT)
        right = (byte & (1 << 3)) != 0;
    }
}
