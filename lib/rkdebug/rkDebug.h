#pragma once

#include <cstdio>
#include <functional>


enum class DebugFormat {Reset, NoSpace, NoSpaceSingle, Quote};

class Debug
{
    public:
        Debug(std::function<unsigned long()> millisGetter);
        template <typename H, typename... T> void error(H p, T... t) { _logIt(DebugLevel::Error, p, t...); }
        template <typename H, typename... T> void info(H p, T... t) { _logIt(DebugLevel::Info, p, t...); }
        template <typename H, typename... T> void debug(H p, T... t) { _logIt(DebugLevel::Debug, p, t...); }
        template <typename H, typename... T> void trace(H p, T... t) { _logIt(DebugLevel::Trace, p, t...); }

    protected:
        virtual void _output(const char *text) = 0;
    
    private:
        enum class DebugLevel {Error, Info, Debug, Trace};
        
        void _output(int value);
        void _output(unsigned int value);
        void _output(double value);
        void _formatReset();
        void _foo(DebugFormat debugFormat, bool isLastArg);
        void _logIt();
        const char *_getLevelText(DebugLevel level);

        const std::function<unsigned long()> millisGetter;
        bool formatNoSpace;
        bool formatNoSpaceSingle;
        bool formatQuote;

        template <typename H> void _foo(H p, bool isLastArg)
        {
            if (formatQuote)
            {
                _output("\"");
                _output(p);
                _output("\"");
                // only applies to a single item, so reset it immediately
                formatQuote = false;
            }
            else
            {
                _output(p);
            }

            if (!(formatNoSpace || formatNoSpaceSingle)
                && !isLastArg)
            {
                _output(" ");
                // reset it immediately after outputing the space
                formatNoSpaceSingle = false;
            }
        }

        template <typename H, typename... T> void _logIt(DebugLevel level, H p, T... t)
        {
            _formatReset();

            char prefix[32];
            float millisSinceBoot = millisGetter() / 1000.0f;
            sprintf(prefix, "[%.3f %s] ", millisSinceBoot, _getLevelText(level));
            
            _output(prefix);
            _foo(p, sizeof...(t) == 0);
            _logIt(t...);
        }

        template <typename H, typename... T> void _logIt(H p, T... t)
        {
            _foo(p, sizeof...(t) == 0);
            _logIt(t...);
        }
};
