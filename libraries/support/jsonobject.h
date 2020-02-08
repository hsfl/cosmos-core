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

    void addElement(std::string key, JSONValue value);
    std::string to_json_string();
    void get_json_string(std::string &jstring);
private:
    std::vector<jsonTuple> contents;
};

#endif // JSONOBJECT_H
