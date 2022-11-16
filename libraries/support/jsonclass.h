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
                //        string name;
                Type type = Type::Undefined;
                string svalue;
                double nvalue;
                bool bvalue;
                std::map<string, Value> object;
                vector<Value> array;
                string::iterator begin;
                string::iterator end;
            };

            typedef pair<string, Value> Member;
            typedef std::map<string, Value> Object;
            typedef vector<Value> Array;

            //! JSON Member elements
            //! Structure containing the 2 basic parts of a JSON member, plus some supporting information.
            //    struct Member
            //    {
            //        string name;
            //        Value value;
            //    };

            //! JSON Object
            //! Complete JSON Object, containing both the string of characters and a heirarchical vector of values.
            //    struct Object
            //    {
            //        vector <Member> members;
            //    };

            Type contents_type(string name);
            string contents_string(string name);
            double contents_number(string name);
            bool contents_bool(string name);
            Object contents_object(string name);
            Array contents_array(string name);

            int32_t extract_contents(string json);
            //    int32_t extract_members(string::iterator begin, string::iterator end, vector<Member> &members);
            //    int32_t extract_values(string::iterator begin, string::iterator end, vector <Member> &members);
            int32_t extract_members(string::iterator &begin, string::iterator &end, Object &members);
            int32_t extract_values(string::iterator &begin, string::iterator &end, Array &values);
            int32_t extract_string(string::iterator &bit, string::iterator &eit, string &ostring);
            int32_t extract_name(string::iterator &bit, string::iterator &eit, string &name);
            int32_t extract_value(string::iterator &bit, string::iterator &eit, Value &value);
            int32_t extract_member(string::iterator &bit, string::iterator &eit, Member &value);
            //    int32_t extract_member(string::iterator &bit, string::iterator &eit, Member &value);
            int32_t skip_character(string::iterator &bit, string::iterator &eit, const char character);
            int32_t skip_to_character(string::iterator &bit, string::iterator &eit, string characters);
            int32_t skip_white(string::iterator &bit, string::iterator &eit);

            string Content;
            Type JType;
            Object ObjectContents;
            Array ArrayContents;
            //    vector <Member> Members;

        private:

            string::iterator begin;
            string::iterator end;
        };


    }
}
#endif // JSONCLASS_H
