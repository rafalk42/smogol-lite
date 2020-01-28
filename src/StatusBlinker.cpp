#include <Arduino.h>

#include "StatusBlinker.h"


StatusBlinker::StatusBlinker(Debug *debug, unsigned int ledPin, bool ledPinInverterd) :
    debug(debug),
    ledPin(ledPin),
    ledPinInverterd(ledPinInverterd),
    patterns({
        // timeOnRepeat, timeOffRepeat, repeatOn, timeOff)
        {Status::Unknown, {25, 0, 1, 25}},
        {Status::Initializing, {25, 0, 1, 25}},
        {Status::Nominal, {100, 0, 1, 3000}},
        {Status::NoSensorData, {200, 200, 2, 1000}},
        {Status::NoConnection, {200, 200, 3, 1000}}
    }),
    currentPattern(&patterns.at(Status::Unknown))
{
}

void StatusBlinker::initialize()
{
    pinMode(ledPin, OUTPUT);
}

void StatusBlinker::setStatus(Status newStatus)
{
    if (newStatus == currentStatus)
    {
        return;
    }
    debug->info("New status", _statusToText(newStatus));

    currentPattern = &patterns.at(newStatus);
    patternStartMillis = millis();
    currentStatus = newStatus;
}

void StatusBlinker::handle()
{
    unsigned long currentTime = millis();
    if ((currentTime - patternStartMillis) >= currentPattern->totalTime())
    {
        patternStartMillis = currentTime;
    }

    bool value = currentPattern->onOrOff(currentTime - patternStartMillis);

    digitalWrite(ledPin, value ^ ledPinInverterd);
}

StatusBlinker::BlinkPattern::BlinkPattern(unsigned int timeOnRepeat, unsigned int timeOffRepeat, unsigned int repeatOn, unsigned int timeOff) :
    timeOnRepeat(timeOnRepeat),
    timeOffRepeat(timeOffRepeat),
    repeatOn(repeatOn),
    timeOff(timeOff)
{
}

unsigned long StatusBlinker::BlinkPattern::totalTime() const
{
    return (timeOnRepeat + timeOffRepeat) * repeatOn + timeOff;
}

bool StatusBlinker::BlinkPattern::onOrOff(unsigned long timeFromStart) const
{
    if (timeFromStart < ((timeOnRepeat + timeOffRepeat) * repeatOn - timeOffRepeat))
    {
        auto timeInOnSegment = timeFromStart % (timeOnRepeat + timeOffRepeat) * repeatOn;

        return timeInOnSegment < timeOnRepeat;
    }
    else
    {
        return false;
    }
}

const char *StatusBlinker::_statusToText(Status status)
{
    switch(status)
    {
        default:
        case Status::Unknown: return "Unknown";
        case Status::Initializing: return "Initializing";
        case Status::Nominal: return "Nominal";
        case Status::NoConnection: return "NoConnection";
        case Status::NoSensorData: return "NoSensorData";
    }
}