#ifndef CHECK_H
#define CHECK_H

namespace Cosmos {
namespace Support {
namespace Log {
class Checker
{
public:
    Checker(string base_name, string date_time="");
    ~Checker();
    int32_t Report(string name, string description, string result, bool pass);

private:
    int32_t sequence_num = 0;
    string file_name;
    FILE *fp = nullptr;
};
}
}
}

#endif // CHECK_H
