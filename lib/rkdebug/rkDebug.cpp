#include "rkDebug.h"


Debug::Debug(std::function<unsigned long()> millisGetter) :
    millisGetter(millisGetter)
{
}

void Debug::_output(int value)
{
    char buffer[16];
    sprintf(buffer, "%d", value);
    _output(buffer);
}

void Debug::_output(unsigned int value)
{
    char buffer[16];
    sprintf(buffer, "%u", value);
    _output(buffer);
}

void Debug::_output(double value)
{
    char buffer[16];
    sprintf(buffer, "%0.3f", value);
    _output(buffer);
}

void Debug::_formatReset()
{
    formatNoSpace = false;
    formatNoSpaceSingle = false;
    formatQuote = false;
}

void Debug::_foo(DebugFormat debugFormat, bool isLastArg)
{
    switch (debugFormat)
    {
        case DebugFormat::Reset:
            _formatReset();
        break;
        case DebugFormat::NoSpace:
            formatNoSpace = true;
        break;
        case DebugFormat::NoSpaceSingle:
            formatNoSpaceSingle = true;
        break;
        case DebugFormat::Quote:
            formatQuote = true;
        break;
    }
}

void Debug::_logIt()
{
    _output("\n");
}

const char *Debug::_getLevelText(DebugLevel level)
{
    switch (level)
    {
        case DebugLevel::Error: return "ERROR"; break;
        case DebugLevel::Info:  return "INFO "; break;
        case DebugLevel::Debug: return "DEBUG"; break;
        case DebugLevel::Trace: return "TRACE"; break;
        default: return "DEFAULT";
    }
}
