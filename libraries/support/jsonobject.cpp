#include "support/jsonobject.h"

JSONObject::JSONObject()
{

}

JSONObject::JSONObject(string key, JSONValue value)
{

    JSONObject::jsonTuple t(key,value);
    this->contents.push_back(t);}


void JSONObject::addElement(string key, JSONValue value)
{
    JSONObject::jsonTuple t(key,value);
    this->contents.push_back(t);
}

void JSONObject::addElement(string key, JSONObject value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

void JSONObject::addElement(string key, JSONArray value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

void JSONObject::addElement(string key, string value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

void JSONObject::addElement(string key, double value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

//void JSONObject::addElement(string key, int value)
//{
//    JSONObject::jsonTuple t(key,JSONValue(value));
//    this->contents.push_back(t);
//}

void JSONObject::addElement(string key, int64_t value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

void JSONObject::addElement(string key, int32_t value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

void JSONObject::addElement(string key, int16_t value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

void JSONObject::addElement(string key, int8_t value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

void JSONObject::addElement(string key, uint64_t value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

void JSONObject::addElement(string key, uint32_t value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

void JSONObject::addElement(string key, uint16_t value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

void JSONObject::addElement(string key, uint8_t value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

void JSONObject::addElement(string key, bool value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

string JSONObject::to_json_object()
{
    if(contents.size() <= 0) return "";
    string json_string = "";
    for(size_t i = 0; i < contents.size(); i++){
        json_string += "\"" + contents[i].key + "\":";
        json_string += contents[i].value.to_json_string();
        if(i < contents.size() - 1){
            json_string += ",";
        }
    }
    return json_string;
}

string JSONObject::to_json_string()
{
    if(contents.size() <= 0) return "";
    string json_string = "{" + to_json_object() + "}";
    return json_string;
}

void JSONObject::get_json_string(string &jstring)
{
    jstring.clear();
    jstring.append(to_json_string());
}

void JSONObject::clear()
{
    contents.clear();
}
