#include "support/jsonvalue.h"
#include "support/jsonobject.h"

JSONValue::JSONValue()
{
    this->type = JSONValue::JsonValueType::NULL_VALUE;
    this->value_as_string = "null";
}

JSONValue::JSONValue(std::string val)
{
    set(val);
}

JSONValue::JSONValue(double val)
{
    set(val);
}

JSONValue::JSONValue(int val)
{
    set(val);
}

JSONValue::JSONValue(uint32_t val)
{
    this->type = JSONValue::JsonValueType::NUMBER;
    this->value_as_string = std::to_string(val);
}

JSONValue::JSONValue(uint8_t val)
{
    this->type = JSONValue::JsonValueType::NUMBER;
    this->value_as_string = std::to_string(val);
}

JSONValue::JSONValue(char val)
{
    set(val);
}

JSONValue::JSONValue(JSONObject val)
{
    set(val);

}

JSONValue::JSONValue(JSONArray val)
{
    set(val);

}

JSONValue::JSONValue(bool val)
{
    if(val){
        this->type = JSONValue::JsonValueType::BOOL_TRUE;
        value_as_string = "true";
    } else {
        this->type = JSONValue::JsonValueType::BOOL_FALSE;
        value_as_string = "false";
    }

}

void JSONValue::set_null()
{
    this->type = JSONValue::JsonValueType::NULL_VALUE;
    this->value_as_string = "null";
}

void JSONValue::set(std::string val)
{
    this->type = JSONValue::JsonValueType::STRING;
    this->value_as_string = "\"" + val + "\"";
}

void JSONValue::set(double val)
{
    this->type = JSONValue::JsonValueType::NUMBER;
    char val_str[50];
    sprintf(val_str, "%f", val);
    this->value_as_string = val_str;
}

void JSONValue::set(int val)
{
    this->type = JSONValue::JsonValueType::NUMBER;
    this->value_as_string = std::to_string(val);
}

void JSONValue::set(char val)
{
    this->type = JSONValue::JsonValueType::STRING;
    this->value_as_string = "\"" + std::to_string(val) + "\"";
}

void JSONValue::set(JSONObject val)
{
    this->type = JSONValue::JsonValueType::JSON_OBJECT;
    value_as_string = val.to_json_string();
}

void JSONValue::set(JSONArray val)
{
    std::string jsonStr;
    this->type = JSONValue::JsonValueType::JSON_ARRAY;

    jsonStr = "[";
    for(size_t i=0; i < val.size(); i++){
        jsonStr += val[i].to_json_string();
        if(i < val.size()-1){
            jsonStr+= ",";
        }
    }
    jsonStr += "]";

    value_as_string = jsonStr;

}

void JSONValue::set(bool val)
{
    if(val){
        this->type = JSONValue::JsonValueType::BOOL_TRUE;
        value_as_string = "true";
    } else {
        this->type = JSONValue::JsonValueType::BOOL_FALSE;
        value_as_string = "false";
    }

}
bool JSONValue::isNull()
{
    if(this->type == JSONValue::JsonValueType::NULL_VALUE){
        return true;
    }
    return false;
}

bool JSONValue::isString()
{
    if(this->type == JSONValue::JsonValueType::STRING){
        return true;
    }
    return false;
}

bool JSONValue::isNumber()
{
    if(this->type == JSONValue::JsonValueType::NUMBER){
        return true;
    }
    return false;
}

bool JSONValue::isObject()
{
    if(this->type == JSONValue::JsonValueType::JSON_OBJECT){
        return true;
    }
    return false;
}

bool JSONValue::isArray()
{
    if(this->type == JSONValue::JsonValueType::JSON_ARRAY){
        return true;
    }
    return false;
}

bool JSONValue::isBool()
{
    if(this->type == JSONValue::JsonValueType::BOOL_FALSE){
        return true;
    }
    if(this->type == JSONValue::JsonValueType::BOOL_TRUE){
        return true;
    }
    return false;
}

std::string JSONValue::to_json_string()
{
    return value_as_string;
}
