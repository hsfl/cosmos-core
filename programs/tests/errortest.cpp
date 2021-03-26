#include "support/configCosmos.h"
#include "support/cosmos-errno.h"
#include "support/timelib.h"

int main()
{
    Error errorlog;

    errorlog.Set(Error::LogType::LOG_FILE_FFLUSH);
    errorlog.Printf("Hi there");
    errorlog.Printf("The Time Is %s\n", utc2iso8601(currentmjd()));
}
