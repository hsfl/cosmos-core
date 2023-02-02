#include "support/configCosmos.h"
#include "support/logger.h"
#include "support/timelib.h"

int main()
{
    Log::Logger debuglog;

    debuglog.Set(Log::LogType::LOG_STDOUT_FFLUSH);
    debuglog.Printf("Hi there");
    debuglog.Printf("The Time Is %s\n", utc2iso8601(currentmjd()).c_str());
}
