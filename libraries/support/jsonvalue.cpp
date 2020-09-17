#include "support/jsonvalue.h"
#include "support/jsonobject.h"
#include "support/stringlib.h"

JSONValue::JSONValue()
{
    type = JSONValue::JsonValueType::NULL_VALUE;
    value_as_string = "null";
}

JSONValue::JSONValue(string val)
{
    set(val);
}

JSONValue::JSONValue(float val)
{
    set(val);
}

JSONValue::JSONValue(double val)
{
    set(val);
}

JSONValue::JSONValue(uint64_t val)
{
    set(val);
}

JSONValue::JSONValue(uint32_t val)
{
    set(val);
}

JSONValue::JSONValue(uint16_t val)
{
    set(val);
}

JSONValue::JSONValue(uint8_t val)
{
    set(val);
}

JSONValue::JSONValue(int64_t val)
{
    set(val);
}

JSONValue::JSONValue(int32_t val)
{
    set(val);
}

JSONValue::JSONValue(int16_t val)
{
    set(val);
}

JSONValue::JSONValue(int8_t val)
{
    set(val);
}

//JSONValue::JSONValue(uint8_t val)
//{
//    type = JSONValue::JsonValueType::NUMBER;
//    value_as_string = std::to_string(val);
//}

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
        type = JSONValue::JsonValueType::BOOL_TRUE;
        value_as_string = "true";
    } else {
        type = JSONValue::JsonValueType::BOOL_FALSE;
        value_as_string = "false";
    }

}

JSONValue::JSONValue(Vector val)
{
    set(val);
}

JSONValue::JSONValue(Quaternion val)
{
    set(val);
}

void JSONValue::set_null()
{
    type = JSONValue::JsonValueType::NULL_VALUE;
    value_as_string = "null";
}

void JSONValue::set(string val)
{
    type = JSONValue::JsonValueType::STRING;
//    value_as_string = "\"" + val + "\"";
    value_as_string = "\"";
    for (char cval : val)
    {
        switch(cval)
        {
        case '"':
            value_as_string += "\\\"";
            break;
        case '\\':
            value_as_string += "\\\\";
            break;
        case '/':
            value_as_string += "\\/";
            break;
        case '\b':
            value_as_string += "\\\b";
            break;
        case '\f':
            value_as_string += "\\\f";
            break;
        case '\n':
            value_as_string += "\\\n";
            break;
        case '\r':
            value_as_string += "\\\r";
            break;
        case '\t':
            value_as_string += "\\\t";
            break;
        default:
            value_as_string += cval;
            break;
        }
    }
    value_as_string += "\"";
}

void JSONValue::set(float val)
{
    type = JSONValue::JsonValueType::NUMBER;
    char val_str[50];
    sprintf(val_str, "%f", val);
    value_as_string = val_str;
}

void JSONValue::set(double val)
{
    type = JSONValue::JsonValueType::NUMBER;
    char val_str[50];
    sprintf(val_str, "%f", val);
    value_as_string = val_str;
}


void JSONValue::set(uint64_t val)
{
    type = JSONValue::JsonValueType::NUMBER;
    value_as_string = to_unsigned(val);
}

void JSONValue::set(uint32_t val)
{
    type = JSONValue::JsonValueType::NUMBER;
    value_as_string = to_unsigned(val);
}

void JSONValue::set(uint16_t val)
{
    type = JSONValue::JsonValueType::NUMBER;
    value_as_string = to_unsigned(val);
}

void JSONValue::set(uint8_t val)
{
    type = JSONValue::JsonValueType::NUMBER;
    value_as_string = to_unsigned(val);
}

void JSONValue::set(int64_t val)
{
    type = JSONValue::JsonValueType::NUMBER;
    value_as_string = to_signed(val);
}

void JSONValue::set(int32_t val)
{
    type = JSONValue::JsonValueType::NUMBER;
    value_as_string = to_signed(val);
}

void JSONValue::set(int16_t val)
{
    type = JSONValue::JsonValueType::NUMBER;
    value_as_string = to_signed(val);
}

void JSONValue::set(int8_t val)
{
    type = JSONValue::JsonValueType::NUMBER;
    value_as_string = to_signed(val);
}

void JSONValue::set(char val)
{
    type = JSONValue::JsonValueType::STRING;
    value_as_string = "\"" + std::to_string(val) + "\"";
}

void JSONValue::set(JSONObject val)
{
    type = JSONValue::JsonValueType::JSON_OBJECT;
    value_as_string = val.to_json_string();
}

void JSONValue::set(JSONArray val)
{
    string jsonStr;
    type = JSONValue::JsonValueType::JSON_ARRAY;

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
        type = JSONValue::JsonValueType::BOOL_TRUE;
        value_as_string = "true";
    } else {
        type = JSONValue::JsonValueType::BOOL_FALSE;
        value_as_string = "false";
    }

}

void JSONValue::set(Vector val)
{
    type = JSONValue::JsonValueType::JSON_OBJECT;
    JSONObject jobject;
    jobject.addElement("x", to_double(val.x));
    jobject.addElement("y", to_double(val.y));
    jobject.addElement("z", to_double(val.z));
    value_as_string = jobject.to_json_string();
}

void JSONValue::set(Quaternion val)
{
    type = JSONValue::JsonValueType::JSON_OBJECT;
    JSONObject jobject;
    jobject.addElement("x", to_double(val.x));
    jobject.addElement("y", to_double(val.y));
    jobject.addElement("z", to_double(val.z));
    jobject.addElement("w", to_double(val.w));
    value_as_string = jobject.to_json_string();
}


bool JSONValue::isNull()
{
    if(type == JSONValue::JsonValueType::NULL_VALUE){
        return true;
    }
    return false;
}

bool JSONValue::isString()
{
    if(type == JSONValue::JsonValueType::STRING){
        return true;
    }
    return false;
}

bool JSONValue::isNumber()
{
    if(type == JSONValue::JsonValueType::NUMBER){
        return true;
    }
    return false;
}

bool JSONValue::isObject()
{
    if(type == JSONValue::JsonValueType::JSON_OBJECT){
        return true;
    }
    return false;
}

bool JSONValue::isArray()
{
    if(type == JSONValue::JsonValueType::JSON_ARRAY){
        return true;
    }
    return false;
}

bool JSONValue::isBool()
{
    if(type == JSONValue::JsonValueType::BOOL_FALSE){
        return true;
    }
    if(type == JSONValue::JsonValueType::BOOL_TRUE){
        return true;
    }
    return false;
}

string JSONValue::to_json_string()
{
    return value_as_string;
}
