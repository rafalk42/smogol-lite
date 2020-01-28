#include <cstdio>
#include <ctime>
#include <cmath>

#include "Debug.h"


int frequency_of_primes (int n) {
  int i,j;
  int freq=n-1;
  for (i=2; i<=n; ++i) for (j=sqrt(i);j>1;--j) if (i%j==0) {--freq; break;}
  return freq;
}

class MyDebug : public Debug
{
    public:
        MyDebug() :
            Debug([]()
            {
                return ((float)clock()) / CLOCKS_PER_SEC * 1000;
            })
        {
        }

        void _output(const char *text)
        {
            printf(text);
        }
};

int main()
{
    Debug *debug = new MyDebug();

    debug->info("Hello", DebugFormat::NoSpaceSingle, DebugFormat::Quote, "world", DebugFormat::Reset, ", and", "a", "happy", "new", DebugFormat::NoSpaceSingle, "year", "!");

    return 0;
}