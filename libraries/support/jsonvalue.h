#ifndef JSONVALUE_H
#define JSONVALUE_H
#include "support/configCosmos.h"
#include "math/vector.h"
using namespace Cosmos::Math::Vectors;
using namespace Cosmos::Math::Quaternions;

class JSONObject;
class JSONValue;
typedef struct vector<JSONValue> JSONArray;


class JSONValue
{


public:
    enum class JsonValueType : uint8_t
        {
            NULL_VALUE = 0,
            STRING = 1,
            NUMBER = 2,
            JSON_OBJECT = 3,
            JSON_ARRAY = 4,
            BOOL_TRUE = 5,
            BOOL_FALSE = 6,

        };
    JSONValue();
    JSONValue(uint8_t val);
    JSONValue(int8_t val);
    JSONValue(char val);
    JSONValue(bool val);
    JSONValue(uint16_t val);
    JSONValue(int16_t val);
    JSONValue(uint32_t val);
    JSONValue(int32_t val);
    JSONValue(uint64_t val);
    JSONValue(int64_t val);

    JSONValue(float val);
    JSONValue(double val);

    JSONValue(string val);
    JSONValue(Vector val);
    JSONValue(Quaternion val);

    JSONValue(JSONObject val);
    JSONValue(JSONArray val);

    void set_null();
    void set(string val);
    void set(float val);
    void set(double val);
    void set(uint64_t val);
    void set(uint32_t val);
    void set(uint16_t val);
    void set(uint8_t val);
    void set(int64_t val);
    void set(int32_t val);
    void set(int16_t val);
    void set(int8_t val);

    void set(Vector val);
    void set(Quaternion val);

    void set(char val);
    void set(JSONObject val);
    void set(JSONArray val);
    void set(bool val);


    bool isNull();
    bool isString();
    bool isNumber();
    bool isObject();
    bool isArray();
    bool isBool();

    string to_json_string();
private:
    string value_as_string;
    JsonValueType type;
};

#endif // JSONVALUE_H
