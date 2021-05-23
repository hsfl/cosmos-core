/********************************************************************
* Copyright (C) 2015 by Interstel Technologies, Inc.
*   and Hawaii Space Flight Laboratory.
*
* This file is part of the COSMOS/core that is the central
* module for COSMOS. For more information on COSMOS go to
* <http://cosmos-project.com>
*
* The COSMOS/core software is licenced under the
* GNU Lesser General Public License (LGPL) version 3 licence.
*
* You should have received a copy of the
* GNU Lesser General Public License
* If not, go to <http://www.gnu.org/licenses/>
*
* COSMOS/core is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3 of
* the License, or (at your option) any later version.
*
* COSMOS/core is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* Refer to the "licences" folder for further information on the
* condititons and terms to use this software.
********************************************************************/

/*! \file enumlib.cpp
    \brief Better Enum library source file
*/

#include "support/convertlib.h"
#include "enumlib.h"

namespace Cosmos {
    namespace Support {
        Enum::Enum()
        {

        }

        void Enum::Init(const vector<string> names, const vector<ptrdiff_t> values)
        {
            forward.clear();
            backward.clear();
            if (values.size() == 1)
            {
                ptrdiff_t value = values[0];
                for (string name : names)
                {
                    forward[name] = value;
                    backward[value] = name;
                    ++value;
                }
            }
            else if (values.size() == 2)
            {
                ptrdiff_t value = values[0];
                for (string name : names)
                {
                    forward[name] = value;
                    backward[value] = name;
                    value += (values[1] - values[0]);
                }
            }
            else if (values.size() == names.size())
            {
                for (size_t i=0; i<names.size(); ++i)
                {
                    forward[names[i]] = values[i];
                    backward[values[i]] = names[i];
                }
            }
            else
            {
                ptrdiff_t value = 0;
                for (string name : names)
                {
                    forward[name] = value;
                    backward[value] = name;
                    ++value;
                }
            }
        }

        void Enum::Extend(const vector<string> names, const vector<ptrdiff_t> values)
        {
            if (values.size() == 1)
            {
                ptrdiff_t value = values[0];
                for (string name : names)
                {
                    forward[name] = value;
                    backward[value] = name;
                    ++value;
                }
            }
            else if (values.size() == 2)
            {
                ptrdiff_t value = values[0];
                for (string name : names)
                {
                    forward[name] = value;
                    backward[value] = name;
                    value += (values[1] - values[0]);
                }
            }
            else if (values.size() == names.size())
            {
                for (size_t i=0; i<names.size(); ++i)
                {
                    forward[names[i]] = values[i];
                    backward[values[i]] = names[i];
                }
            }
            else
            {
                ptrdiff_t value = 0;
                for (string name : names)
                {
                    forward[name] = value;
                    backward[value] = name;
                    ++value;
                }
            }
        }

        bool Enum::Exists(string key)
        {
            if (forward.find(key) != forward.end())
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool Enum::Exists(ptrdiff_t value)
        {
            if (backward.find(value) != backward.end())
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        ptrdiff_t Enum::operator [] (const string name)
        {
            ForwardEnum::iterator eit;
            if ((eit=forward.find(name)) != forward.end())
            {
                return eit->second;
            }
            else
            {
                return PTRDIFF_MAX;
            }
        }

        string Enum::operator [] (const ptrdiff_t value)
        {
            BackwardEnum::iterator eit;
            if ((eit=backward.find(value)) != backward.end())
            {
                return eit->second;
            }
            else
            {
                return "";
            }
        }
    }
}
