#include "support/jsonobject.h"

JSONObject::JSONObject()
{

}

JSONObject::JSONObject(std::string key, JSONValue value)
{

    JSONObject::jsonTuple t(key,value);
    this->contents.push_back(t);}


void JSONObject::addElement(std::string key, JSONValue value)
{
    JSONObject::jsonTuple t(key,value);
    this->contents.push_back(t);
}

void JSONObject::addElement(std::string key, JSONObject value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

void JSONObject::addElement(std::string key, JSONArray value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

void JSONObject::addElement(std::string key, std::string value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

void JSONObject::addElement(std::string key, double value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

void JSONObject::addElement(std::string key, int value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

void JSONObject::addElement(std::string key, uint32_t value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

void JSONObject::addElement(std::string key, uint8_t value)
{
    JSONObject::jsonTuple t(key,JSONValue(value));
    this->contents.push_back(t);
}

std::string JSONObject::to_json_string()
{
    if(contents.size() <= 0) return "";
    std::string json_string = "{";
    for(size_t i = 0; i < contents.size(); i++){
        json_string += "\"" + contents[i].key + "\":";
        json_string += contents[i].value.to_json_string();
        if(i < contents.size() - 1){
            json_string += ",";
        }
    }

    json_string += "}";
    return json_string;
}

void JSONObject::get_json_string(std::string &jstring)
{
    jstring.clear();
    jstring.append(to_json_string());
}

void JSONObject::clear()
{
    contents.clear();
}
