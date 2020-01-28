#pragma once

#include <map>

#include "rkDebug.h"


class StatusBlinker
{
    public:
        enum class Status {Unknown, Initializing, Nominal, NoConnection, NoSensorData};

        StatusBlinker(Debug *debug, unsigned int ledPin, bool ledPinInverterd);
        void initialize();
        void setStatus(Status newStatus);
        void handle();

    private:
        class BlinkPattern
        {
            public:
                BlinkPattern(unsigned int timeOnRepeat, unsigned int timeOffRepeat, unsigned int repeatOn, unsigned int timeOff);
                unsigned long totalTime() const;
                bool onOrOff(unsigned long timeFromStart) const;

                const unsigned int timeOnRepeat;
                const unsigned int timeOffRepeat;
                const unsigned int repeatOn;
                const unsigned int timeOff;
        };
        Debug *const debug;
        const unsigned int ledPin;
        const bool ledPinInverterd;
        const std::map<Status, BlinkPattern> patterns;
        Status currentStatus;
        const BlinkPattern *currentPattern;
        unsigned long patternStartMillis;

        const char *_statusToText(Status status);
};
