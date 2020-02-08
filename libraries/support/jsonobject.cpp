#include "support/jsonobject.h"

JSONObject::JSONObject()
{

}

void JSONObject::addElement(std::string key, JSONValue value)
{
    JSONObject::jsonTuple t(key,value);
    this->contents.push_back(t);
}

std::string JSONObject::to_json_string()
{
    if(contents.size() <= 0) return "";
    std::string json_string = "{";
    for(int i = 0; i < contents.size(); i++){
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
