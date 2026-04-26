#pragma once

class Timer
{
    float length, time;
    bool timeout;

   public:
    Timer(float length) : length(length), time(0), timeout(false) {}

    void step(float deltaTime)
    {
        timeout = false;
        time += deltaTime;
        while (time >= length)
        {
            time -= length;
            timeout = true;
        }
    }

    bool isTimeout() const { return timeout; }
    float getTime() const { return time; }
    float getLength() const { return length; }
    void reset()
    {
        time = 0;
        timeout = false;
    }
};
