#ifndef JSONOBJECT_H
#define JSONOBJECT_H
#include "support/jsonvalue.h"

class JSONObject
{
    struct jsonTuple {
        std::string key;
        JSONValue value;
        jsonTuple(std::string k, JSONValue v){
            key = k;
            value = v;
        };
    };
public:

    JSONObject();
    JSONObject(std::string key, JSONValue value);

    void addElement(std::string key, JSONValue value);
    void addElement(std::string key, JSONObject value);
    void addElement(std::string key, JSONArray value);
    void addElement(std::string key, std::string value);
    void addElement(std::string key, double value);
    void addElement(std::string key, int value);
    void addElement(std::string key, uint32_t value);
    void addElement(std::string key, uint8_t value);
    std::string to_json_string();
    void get_json_string(std::string &jstring);
    void clear();

private:
    std::vector<jsonTuple> contents;
};

#endif // JSONOBJECT_H
