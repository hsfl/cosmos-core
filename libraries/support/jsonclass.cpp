#include "jsonclass.h"

namespace Cosmos
{
    namespace Support
    {

        Json::Json(string json)
        {
            if (!json.empty())
            {
                extract_contents(json);
            }
        }

        Json::Type Json::contents_type(string name)
        {
            Object::iterator mit;
            if ((mit=ObjectContents.find(name)) != ObjectContents.end())
            {
                return mit->second.type;
            }
            else {
                return Type::Undefined;
            }
        }

        string Json::contents_string(string name)
        {
            string contents = "";
            if (contents_type(name) == Type::String)
            {
                contents = ObjectContents.at(name).svalue;
            }
            return contents;
        }

        double Json::contents_number(string name)
        {
            double contents = 0.;
            if (contents_type(name) == Type::Number)
            {
                contents = ObjectContents.at(name).nvalue;
            }
            return contents;
        }

        bool Json::contents_bool(string name)
        {
            bool contents = false;
            if (contents_type(name) == Type::True)
            {
                contents = ObjectContents.at(name).bvalue;
            }
            return contents;
        }

        Json::Object Json::contents_object(string name)
        {
            Object contents;
            if (contents_type(name) == Type::Object)
            {
                contents = ObjectContents.at(name).object;
            }
            return contents;
        }

        Json::Array Json::contents_array(string name)
        {
            Array contents;
            if (contents_type(name) == Type::Array)
            {
                contents = ObjectContents.at(name).array;
            }
            return contents;
        }


		// JIMNOTE:	this function isn't really needed any more with Namespace 2.0 (and maybe others)
		//			do the following for the class you are trying to initialize from a json string:
		//
		//			my_class my_instance m;			// declare an instance of your class
		//			m.from_json(input_JSON_string); // now the instance holds the contents of the JSON string!
		//
        int32_t Json::extract_contents(string json)
        {
            int32_t iretn = 0;

            Content = json;

            // Determine overall type
            string::iterator begin = json.begin();
            string::iterator end = json.end();

            iretn = skip_to_character(begin, end, "[{");
            switch (iretn)
            {
            case '[':
                {
                    JType = Type::Array;
                    iretn = skip_character(begin, end, '[');
                    if (iretn <= 0)
                    {
                        return iretn;
                    }
                    iretn = extract_values(begin, end, ArrayContents);
                    if (iretn < 0)
                    {
                        return iretn;
                    }
                    iretn = skip_white(begin, end);
                    if (iretn < 0)
                    {
                        return iretn;
                    }
                    iretn = skip_character(begin, end, ']');
                    if (iretn <= 0)
                    {
                        return iretn;
                    }
                }
                break;
            case '{':
                {
                    JType = Type::Object;
                    iretn = skip_character(begin, end, '{');
                    if (iretn <= 0)
                    {
                        return iretn;
                    }
                    iretn = extract_members(begin, end, ObjectContents);
                    if (iretn < 0)
                    {
                        return iretn;
                    }
                    iretn = skip_white(begin, end);
                    if (iretn < 0)
                    {
                        return iretn;
                    }
                    iretn = skip_character(begin, end, '}');
                    if (iretn <= 0)
                    {
                        return iretn;
                    }
                }
                break;
            default:
                return iretn;
            }
            return iretn;
        }

        //! Extract JSON values.
        //! Build JSON Object from values extracted from string containing JSON.
        //! \param json String containing JSON values.
        //! \return ::Object containing heirarchical vector of ::Member
        int32_t Json::extract_members(string::iterator &begin, string::iterator &end, Object &members)
        {
            members.clear();
            while (begin != end)
            {
                Member tmember;
                int32_t iretn = extract_member(begin, end, tmember);
                if (iretn < 0)
                {
                    return iretn;
                }
                else if (iretn > 0)
                {
                    members.emplace(tmember);
                }
                else {
                    break;
                }
            }

            return static_cast<int32_t>(members.size());
        }

        int32_t Json::extract_values(string::iterator &begin, string::iterator &end, Array &values)
        {
            values.clear();
            while (begin != end)
            {
                Value tvalue;
                int32_t iretn = extract_value(begin, end, tvalue);
                if (iretn < 0)
                {
                    return iretn;
                }
                else if (iretn > 0)
                {
                    values.push_back(tvalue);
                }
                else {
                    break;
                }
            }

            return static_cast<int32_t>(values.size());
        }

        //! Extract JSON member.
        //! Extract next member of JSON Obect from provided string.
        //! \param begin Beginning string iterator.
        //! \param end End string iterator.
        //! \param value ::Member containing value.
        //! \return Zero or negative error.
        int32_t Json::extract_member(string::iterator &begin, string::iterator &end, Member &member)
        {
            int32_t iretn = 0;
            iretn = extract_name(begin, end, member.first);
            if (iretn <= 0)
            {
                return iretn;
            }
            iretn = extract_value(begin, end, member.second);
            return iretn;
        }

        //! Extract next JSON value.
        //! Extract the next JSON value from the c++ string represented by the supplied iterator. Leave the iterator pointing
        //! to the beginning of the next value.
        //! \param it Reference to string iterator.
        //! \return A pointer the the discovered value, or a nullptr.
        int32_t Json::extract_value(string::iterator &begin, string::iterator &end, Value &value)
        {
            int32_t iretn = 0;

            iretn = skip_white(begin, end);
            if (iretn < 0)
            {
                return iretn;
            }

            value.begin = begin;
            switch (*begin)
            {
            case '}':
            case ']':
                return 0;
            case '{':
                value.type = Type::Object;
                iretn = skip_character(begin, end, '{');
                if (iretn <= 0)
                {
                    return iretn;
                }
                iretn = extract_members(begin, end, value.object);
                if (iretn < 0)
                {
                    return iretn;
                }
                iretn = skip_white(begin, end);
                if (iretn < 0)
                {
                    return iretn;
                }
                iretn = skip_character(begin, end, '}');
                if (iretn <= 0)
                {
                    return iretn;
                }
//                if (++begin == end || *begin == 0)
//                {
//                    return JSON_ERROR_EOS;
//                }
                break;
            case '[':
                value.type = Type::Array;
                iretn = skip_character(begin, end, '[');
                if (iretn <= 0)
                {
                    return iretn;
                }
                iretn = extract_values(begin, end, value.array);
                if (iretn < 0)
                {
                    return iretn;
                }
                iretn = skip_white(begin, end);
                if (iretn < 0)
                {
                    return iretn;
                }
                iretn = skip_character(begin, end, ']');
                if (iretn <= 0)
                {
                    return iretn;
                }
//                if (++begin == end || *begin == 0)
//                {
//                    return JSON_ERROR_EOS;
//                }
                break;
            case '"':
                {
                    value.svalue.clear();
                    uint16_t ilen = (end - begin) + 1;
                    uint16_t i2;
                    for (i2=1; i2<ilen; i2++) //start from ptr[1] rather than ptr[0] which is a '"'?
                    {
                        if (*(begin+i2) == '"')
                            break;  //exits for loop?
                        if (*(begin+i2) == '\\')
                        {
                            switch (*(begin+i2+1))
                            {
                            case '"':
                            case '\\':
                            case '/':
                                value.svalue.push_back(*(begin+i2+1));
                                break;
                            case 'b':
                                value.svalue.push_back('\b');
                                break;
                            case 'f':
                                value.svalue.push_back('\f');
                                break;
                            case 'n':
                                value.svalue.push_back('\n');
                                break;
                            case 'r':
                                value.svalue.push_back('\r');
                                break;
                            case 't':
                                value.svalue.push_back('\t');
                                break;
                            default:
                                i2 += 3;
                            }
                            i2++;
                        }
                        else
                        {
                            value.svalue.push_back(*(begin+i2));
                        }
                    }

                    if (i2 >= ilen)
                    {
                        begin = begin + (ilen-1);
                        return(JSON_ERROR_SCAN);
                    }

                    // i2 is last character in string +1, index is length of extracted string

                    begin += (i2 + 1);
                    value.type = Type::String;
                }
                break;
            case 'f':
                if (++begin == end || *begin == 0)
                {
                    return JSON_ERROR_EOS;
                }
                if (*begin != 'a')
                {
                    return JSON_ERROR_SCAN;
                }
                if (++begin == end || *begin == 0)
                {
                    return JSON_ERROR_EOS;
                }
                if (*begin != 'l')
                {
                    return JSON_ERROR_SCAN;
                }
                if (++begin == end || *begin == 0)
                {
                    return JSON_ERROR_EOS;
                }
                if (*begin != 's')
                {
                    return JSON_ERROR_SCAN;
                }
                if (++begin == end || *begin == 0)
                {
                    return JSON_ERROR_EOS;
                }
                if (*begin != 'e')
                {
                    return JSON_ERROR_SCAN;
                }
                value.type = Type::False;
                if (++begin == end || *begin == 0)
                {
                    return JSON_ERROR_EOS;
                }
                break;
            case 't':

                if (++begin == end || *begin == 0)
                {
                    return JSON_ERROR_EOS;
                }
                if (*begin != 'r')
                {
                    return JSON_ERROR_SCAN;
                }

                if (++begin == end || *begin == 0)
                {
                    return JSON_ERROR_EOS;
                }
                if (*begin != 'u')
                {
                    return JSON_ERROR_SCAN;
                }

                if (++begin == end || *begin == 0)
                {
                    return JSON_ERROR_EOS;
                }
                if (*begin != 'e')
                {
                    return JSON_ERROR_SCAN;
                }
                value.type = Type::True;
                if (++begin == end || *begin == 0)
                {
                    return JSON_ERROR_EOS;
                }
                break;
            case 'n':

                if (++begin == end || *begin == 0)
                {
                    return JSON_ERROR_EOS;
                }
                if (*begin != 'u')
                {
                    return JSON_ERROR_SCAN;
                }

                if (++begin == end || *begin == 0)
                {
                    return JSON_ERROR_EOS;
                }
                if (*begin != 'l')
                {
                    return JSON_ERROR_SCAN;
                }

                if (++begin == end || *begin == 0)
                {
                    return JSON_ERROR_EOS;
                }
                if (*begin != 'l')
                {
                    return JSON_ERROR_SCAN;
                }
                value.type = Type::Null;
                if (++begin == end || *begin == 0)
                {
                    return JSON_ERROR_EOS;
                }
                break;
            case 0:
                return JSON_ERROR_EOS;
            default:
                value.svalue.clear();
                do
                {
                    if (*begin == 0)
                    {
                        return JSON_ERROR_EOS;
                    }
                    value.svalue.push_back(*begin);
                    if (++begin == end)
                    {
                        return JSON_ERROR_EOS;
                    }
                } while ((*begin>='0'&&*begin<='9')||*begin=='e'||*begin=='E'||*begin=='.'||*begin=='-'||*begin=='+');
                value.type = Type::Number;
                sscanf(value.svalue.c_str(), "%lf", &value.nvalue);
                break;
            }
            skip_white(begin, end);
            skip_character(begin, end, ',');
            skip_white(begin, end);
            value.end = begin;

            switch (value.type)
            {
            case Type::True:
            case Type::False:
            case Type::Null:
            case Type::Number:
            case Type::String:
            case Type::Object:
            case Type::Array:
                break;
            case Type::Undefined:
            default:
                return JSON_ERROR_SCAN;
            }
            return static_cast<int32_t>(value.type);
        }

        int32_t Json::skip_white(string::iterator &begin, string::iterator &end)
        {
            if (*begin == 0)
                return (JSON_ERROR_EOS);

            while (begin != end && *begin != 0 && isspace(*begin))
            {
                ++begin;
            }

            if (begin == end || *begin == 0)
                return (JSON_ERROR_EOS);
            else
                return 0;
        }

        int32_t Json::skip_character(string::iterator &begin, string::iterator &end, const char character)
        {
            int32_t iretn = 0;

            iretn = skip_white(begin, end);
            if (iretn < 0)
            {
                return iretn;
            }

            if (begin == end || *begin == 0)
            {
                return (JSON_ERROR_EOS);
            }

            if (*begin != character)
            {
                return 0;
            }
            ++begin;

            return 1;
        }

        int32_t Json::skip_to_character(string::iterator &begin, string::iterator &end, string characters)
        {

            if (begin == end || *begin == 0)
            {
                return (JSON_ERROR_EOS);
            }

            while (begin != end && *begin != 0)
            {
                for (char character : characters)
                {
                    if (*begin == character)
                    {
                        return *begin;
                    }
                }
                ++begin;
                if (begin == end || *begin == 0)
                {
                    return (JSON_ERROR_EOS);
                }
            }

            return (0);
        }

        int32_t Json::extract_name(string::iterator &begin, string::iterator &end, string &name)
        {
            int32_t iretn = 1;

            if (begin == end || *begin == 0)
                return (JSON_ERROR_EOS);

            //Skip whitespace before name
            if ((iretn = skip_white(begin, end)) < 0)
            {
                return iretn;
            }

            //Parse name
            iretn = extract_string(begin, end, name);
            if (iretn <= 0)
            {
                return iretn;
            }

            //Skip whitespace after name
            if ((iretn = skip_white(begin, end)) < 0)
            {
                return iretn;
            }

            if ((iretn = skip_character(begin, end, ':')) <= 0)
                return iretn;

            //Skip whitespace after seperator
            if ((iretn = skip_white(begin, end)) < 0)
            {
                return iretn;
            }

            return 1;
        }

        //! Extract next JSON string.
        //! Extract the next JSON String from the c++ string represented by the supplied iterator. Leave the iterator pointing
        //! to the beginning of the next value.
        //! \param it Reference to string iterator.
        //! \return The discovered string, or an empty string.
        int32_t Json::extract_string(string::iterator &begin, string::iterator &end, string &ostring)
        {
            int32_t iretn = 0;

            if (begin == end || *begin == 0)
                return (JSON_ERROR_EOS);

            //Skip whitespace before string
            if ((iretn = skip_white(begin, end)) < 0)
            {
                return iretn;
            }

            //Skip '"' before string
            if ((iretn = skip_character(begin, end, '"')) <= 0)
            {
                return iretn;
            }

            // Start of object, get string
            ostring.clear();

            while (begin != end)
            {
                if (*(begin) == '"')
                {
                    break;  // end of string
                }
                if (*(begin) == '\\') // escaped character
                {
                    if (begin + 1 != end)
                    {
                        switch (*(begin+1))
                        {
                        case '"':
                        case '\\':
                        case '/':
                            ostring.push_back(*(begin+1));
                            break;
                        case 'b':
                            ostring.push_back('\b');
                            break;
                        case 'f':
                            ostring.push_back('\f');
                            break;
                        case 'n':
                            ostring.push_back('\n');
                            break;
                        case 'r':
                            ostring.push_back('\r');
                            break;
                        case 't':
                            ostring.push_back('\t');
                            break;
                        default:
                            if (begin + 2 == end)
                            {
                                begin += 2;
                            }
                            else
                            {
                                begin += 3;
                            }
                        }
                        ++begin;
                    }
                }
                else
                {
                    ostring.push_back(*(begin)); // just a character
                }
                ++begin;
            }

            //Skip '"' afer string
            if ((iretn = skip_character(begin, end, '"')) <= 0)
            {
                return iretn;
            }

            if (begin == end)
            {
                return JSON_ERROR_SCAN;
            }

            // i2 is last character in string +1, index is length of extracted string
//            if (ostring.size() >= JSON_MAX_DATA)
//            {
//                ostring.resize(JSON_MAX_DATA-1);
//            }

            return iretn;
        }

    }

}
