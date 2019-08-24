#ifndef JSONCLASS_H
#define JSONCLASS_H

#include "support/configCosmos.h"
#include "support/cosmos-errno.h"

namespace Cosmos
{
    namespace Support
    {
    class Json
{
public:
    Json(string json="");

    //! JSON Basic Types
    //! enum defining the basic types of value to be found in a JSON member
    enum class Type : uint16_t
        {
        Undefined,
        Null,
        False,
        True,
        Number,
        String,
        Array,
        Object
        };

    //! JSON Value
    //! Structure containg the Value portion of a JSON Member
    struct Value
    {
        string name;
        Type type = Type::Undefined;
        string svalue;
        double nvalue;
        string::iterator begin;
        string::iterator end;
    };

    struct Member;

    //! JSON Object
    //! Complete JSON Object, containing both the string of characters and a heirarchical vector of values.
    struct Object
    {
        vector <Member> members;
    };

    //! JSON Member elements
    //! Structure containing the 2 basic parts of a JSON member, plus some supporting information.
    struct Member
    {
        Value value;
        Object object;
    };

    int32_t extract_object(string json);
    int32_t extract_members(string::iterator begin, string::iterator end, vector<Member> &members);
    int32_t extract_values(string::iterator begin, string::iterator end, vector <Member> &members);
    int32_t extract_string(string::iterator &bit, string::iterator &eit, string &ostring);
    int32_t extract_name(string::iterator &bit, string::iterator &eit, string &name);
    int32_t extract_value(string::iterator &bit, string::iterator &eit, Value &value);
    int32_t extract_member(string::iterator &bit, string::iterator &eit, Member &value);
    int32_t skip_character(string::iterator &bit, string::iterator &eit, const char character);
    int32_t skip_white(string::iterator &bit, string::iterator &eit);

    private:

    string Content;
    vector <Member> Members;
    string::iterator begin;
    string::iterator end;
};


    }
}
#endif // JSONCLASS_H
