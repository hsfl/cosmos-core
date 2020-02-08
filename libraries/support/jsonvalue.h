#ifndef JSONVALUE_H
#define JSONVALUE_H
#include "support/configCosmos.h"
class JSONObject;
class JSONValue;
typedef struct std::vector<JSONValue> JSONArray;

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
    JSONValue(string val);
    JSONValue(double val);
    JSONValue(int val);
    JSONValue(uint32_t val);

    JSONValue(char val);
    JSONValue(JSONObject val);
    JSONValue(JSONArray val);
    JSONValue(bool val);

    void set_null();
    void set(string val);
    void set(double val);
    void set(int val);

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

    std::string to_json_string();
private:
    std::string value_as_string;
    JsonValueType type;
};

#endif // JSONVALUE_H
