#ifndef ENUMLIB_H
#define ENUMLIB_H
/*! \file enumlib.h
    \brief enumlib include file
    A library providing a better approach to providing enums.
*/


namespace Cosmos {
    namespace Support {
        class Enum
        {
        public:
            Enum();
            void Init(const vector<string> names, const vector<ptrdiff_t> values={});
            void Extend(const vector<string> names, const vector<ptrdiff_t> values={});
            bool Exists(string key);
            bool Exists(ptrdiff_t value);
            typedef std::map<string, ptrdiff_t> ForwardEnum;
            typedef std::map<ptrdiff_t, string> BackwardEnum;
            ptrdiff_t operator [](const string name);
            string operator [](const ptrdiff_t value);

        private:
            ForwardEnum forward;
            BackwardEnum backward;
        };
    }
}

#endif // ENUMLIB_H
