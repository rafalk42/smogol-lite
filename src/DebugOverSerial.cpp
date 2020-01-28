#include "DebugOverSerial.h"


DebugOverSerial::DebugOverSerial(std::function<unsigned long()> millisGetter, HardwareSerial *port) :
    Debug(millisGetter),
    port(port)
{
    port->begin(76800);
}

void DebugOverSerial::_output(const char *text)
{
    port->write(text);
}
