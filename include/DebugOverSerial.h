#pragma once

#include <Arduino.h>

#include "rkDebug.h"


class DebugOverSerial : public Debug
{
    public:
        DebugOverSerial(std::function<unsigned long()> millisGetter, HardwareSerial *port);
    
    private:
        HardwareSerial *const port;

        void _output(const char *text);
};
