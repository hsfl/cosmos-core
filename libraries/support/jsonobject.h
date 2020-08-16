#ifndef JSONOBJECT_H
#define JSONOBJECT_H
#include "support/jsonvalue.h"

class JSONObject
{
    struct jsonTuple {
        string key;
        JSONValue value;
        jsonTuple(string k, JSONValue v){
            key = k;
            value = v;
        };
    };
public:

    JSONObject();
    JSONObject(string key, JSONValue value);

    void addElement(string key, JSONValue value);
    void addElement(string key, JSONObject value);
    void addElement(string key, JSONArray value);
    void addElement(string key, string value);
    void addElement(string key, double value);
//    void addElement(string key, int value);
    void addElement(string key, int64_t value);
    void addElement(string key, int32_t value);
    void addElement(string key, int16_t value);
    void addElement(string key, int8_t value);
    void addElement(string key, uint64_t value);
    void addElement(string key, uint32_t value);
    void addElement(string key, uint16_t value);
    void addElement(string key, uint8_t value);
//    void addElement(string key, uint8_t value);
    void addElement(string key, bool value);
    string to_json_string();
    string to_json_object();
    void get_json_string(string &jstring);    void clear();

private:
    vector<jsonTuple> contents;
};

#endif // JSONOBJECT_H
