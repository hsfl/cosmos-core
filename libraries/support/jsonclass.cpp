#include "jsonclass.h"

namespace Cosmos
{
    namespace Support
    {

        Json::Json(string json)
        {
            if (!json.empty())
            {
                extract_object(json);
            }
        }

        int32_t Json::extract_object(string json)
        {
            int32_t iretn = 0;

            Content = json;

            iretn = extract_members(json.begin(), json.end(), Members);
            return iretn;
        }

        //! Extract JSON values.
        //! Build JSON Object from values extracted from string containing JSON.
        //! \param json String containing JSON values.
        //! \return ::Object containing heirarchical vector of ::Member
        int32_t Json::extract_members(string::iterator begin, string::iterator end, vector <Member> &members)
        {
            skip_white(begin, end);
            skip_character(begin, end, '{');
            members.clear();
            while (*begin != '}' && begin != end)
            {
                Member tmember;
                int32_t iretn = extract_member(begin, end, tmember);
                if (iretn < 0)
                {
                    return iretn;
                }
                members.push_back(tmember);
                skip_white(begin, end);
            }
            skip_character(begin, end, '}');

            return members.size();
        }

        int32_t Json::extract_values(string::iterator begin, string::iterator end, vector <Member> &members)
        {
            members.clear();
            while (begin != end)
            {
                Member tmember;
                tmember.value.name = "";
                int32_t iretn = extract_value(begin, end, tmember.value);
                if (iretn < 0)
                {
                    return iretn;
                }
                members.push_back(tmember);
            }

            return 0;
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
            iretn = extract_name(begin, end, member.value.name);
            if (iretn < 0)
            {
                return iretn;
            }
            iretn = extract_value(begin, end, member.value);
            if (iretn < 0)
            {
                return iretn;
            }
            switch (member.value.type)
            {
            case Type::True:
            case Type::False:
            case Type::Null:
            case Type::Number:
            case Type::String:
                break;
            case Type::Undefined:
                return JSON_ERROR_SCAN;
                break;
            case Type::Object:
                extract_members(member.value.begin, member.value.end, member.object.members);
                break;
            case Type::Array:
                extract_values(member.value.begin, member.value.end, member.object.members);
                break;
            default:
                return JSON_ERROR_SCAN;
                break;
            }
            return iretn;
        }

        //! Extract next JSON value.
        //! Extract the next JSON value from the c++ string represented by the supplied iterator. Leave the iterator pointing
        //! to the beginning of the next value.
        //! \param it Reference to string iterator.
        //! \return A pointer the the discovered value, or a nullptr.
        int32_t Json::extract_value(string::iterator &begin, string::iterator &end, Value &value)
        {
            uint32_t count;
            int32_t iretn;

            iretn = skip_white(begin, end);
            if (iretn < 0)
            {
                return iretn;
            }

            value.begin = begin;
            switch (*begin)
            {
            case '{':
                count = 1;
                do
                {
                    if (++begin == end || *begin == 0)
                    {
                        return JSON_ERROR_EOS;
                    }
                    switch (*begin)
                    {
                    case '}':
                        --count;
                        break;
                    case '{':
                        ++count;
                        break;
                    }
                } while (count);
                value.type = Type::Object;
                break;
            case '[':
                count = 1;
                do
                {
                    if (++begin == end || *begin == 0)
                    {
                        return JSON_ERROR_EOS;
                    }
                    switch (*begin)
                    {
                    case ']':
                        --count;
                        break;
                    case '[':
                        ++count;
                        break;
                    }
                } while (count);
                value.type = Type::Array;
                break;
            case '"':
                value.svalue.clear();
                do
                {
                    if (++begin == end || *begin == 0)
                    {
                        return JSON_ERROR_EOS;
                    }
                    value.svalue.push_back(*begin);
                } while (*begin != '"');
                value.type = Type::String;
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

                break;
            default:
                value.svalue.clear();
                do
                {
                    if (++begin == end || *begin == 0)
                    {
                        return JSON_ERROR_EOS;
                    }
                    value.svalue.push_back(*begin);
                } while ((*begin>='0'&&*begin<='9')||*begin=='e'||*begin=='E'||*begin=='.'||*begin=='-');
                value.type = Type::Number;
                sscanf(value.svalue.c_str(), "%lf", value.nvalue);
                break;
            }

            ++begin;
            value.end = begin;

            return 0;
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
            int32_t iretn;

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
                return (JSON_ERROR_SCAN);
            }
            ++begin;

            return (0);
        }

        int32_t Json::extract_name(string::iterator &begin, string::iterator &end, string &name)
        {
            int32_t iretn = 0;

            if (begin == end || *begin == 0)
                return (JSON_ERROR_EOS);

            //Skip whitespace before name
            if ((iretn = skip_white(begin, end)) < 0)
            {
                return iretn;
            }

            //Parse name
            iretn = extract_string(begin, end, name);

            //Skip whitespace after name
            if ((iretn = skip_white(begin, end)) < 0)
            {
                return iretn;
            }

            if ((iretn = skip_character(begin, end, ':')) < 0)
                return iretn;

            //Skip whitespace after seperator
            if ((iretn = skip_white(begin, end)) < 0)
            {
                return iretn;
            }

            return iretn;
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
            if ((iretn = skip_character(begin, end, '"')) < 0)
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
