/**
 * @file event.h
 * @brief Commanding class
 * 
 * Copyright (C) 2024 by Interstel Technologies, Inc. and Hawaii Space Flight
 * Laboratory.
 * 
 * This file is part of the COSMOS/core that is the central module for COSMOS.
 * For more information on COSMOS go to <http://cosmos-project.com>
 * 
 * The COSMOS/core software is licenced under the GNU Lesser General Public
 * License (LGPL) version 3 licence.
 * 
 * You should have received a copy of the GNU Lesser General Public License. If
 * not, go to <http://www.gnu.org/licenses/>
 * 
 * COSMOS/core is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 * 
 * COSMOS/core is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 * 
 * Refer to the "licences" folder for further information on the conditions and
 * terms to use this software.
 */

#ifndef COSMOS_COMMAND_H
#define COSMOS_COMMAND_H

/**
 * @ingroup support
 * @defgroup agentlib Agent Server and Client Library
 * %Agent Server and Client.
 */

#include "support/configCosmos.h"
#include "support/jsonlib.h"
#include "support/jsonobject.h"
#include "support/timelib.h"

/** @todo Document this. */
#define MAXCOMMANDWORD 20

namespace Cosmos
{
    /** @todo Remove commented-out code? */
//    namespace Support
//    {

        /**
         * @brief Class to manage %Event information
         * 
         * An %Event is a collection of information related to a single 
         * occurrence.
         */
        class Event
        {
            /** @todo consider private? */
        public:
            /**	%Event start time (Modified Julian Date) */
            double		mjd;
            /** 
             * %Event execution time (Coordinated Universal Time)
             * JIMNOTE: but appears to be using MJD in the code? should it be 
             * named mjdexec? 
             */
            double		utcexec=0.;
            /**	%Event name */
            string		name;
            /** %Event type */
            uint32_t	type;
            /** %Event flag */
            uint32_t	flag;
            /** %Event data */
            string		data;
            /** %Event condition */
            string		condition;
            /**	%Event run indicators */
            bool		already_ran;
            /** @todo Document this. */
            uint32_t true_count=0;
            /** %Event information stored as a JSON string */
            string		event_string;

        public:
            Event();
            ~Event();

            string generator(
                    string name,
                    string data,
                    double utc,
                    string condition,
                    uint32_t flag,
                    uint16_t type
                    );
            string generator(eventstruc event);
            friend ::std::ostream& operator<<(::std::ostream& out, const Event& cmd);
            friend bool operator==(const Event& cmd1, const Event& cmd2);
            void	set_command(string jstring);
            string	get_event_string();
            bool condition_true(cosmosstruc *cinfo);

            /**
             * @brief Set the utcexec object
             * 
             * Sets Event::utcexec to current time
             * 
             * This function is called after event execution and updates the 
             * execution time.
             * 
             * @todo Move to .cpp file
             */
            void	set_utcexec()	{	utcexec = currentmjd(0.);	}

            /**
             * @brief Set the actual object
             * 
             * Sets Event::flag to indicate the event has actually executed 
             * (i.e. EVENT_FLAG_ACTUAL)
             * 
             * This function is called after event execution and updates the 
             * execution status.
             * 
             * @todo Move to .cpp file
             */
            void	set_actual()	{	flag |= EVENT_FLAG_ACTUAL;	}

            /**
             * @brief Get the name object
             * 
             * Retrieves Event::name
             * 
             * @return string representing %Event name
             * 
             * @todo Move to .cpp file
             */
            string  get_name()		{	return name; }

            /**
             * @brief Get the Utc object
             * 
             * Retrieves Event::mjd
             * 
             * @return double representing %Event start time
             * 
             * @todo Move to .cpp file
             */
            double	getUtc()		{	return mjd;	}

            /**
             * @brief Get the Time object
             * 
             * Retrieves Event::mjd
             * 
             * @return string representing %Event start time
             * 
             * @todo Move to .cpp file
             */
            string	getTime()		{	return mjd2iso8601( getUtc() ); }

            /**
             * @brief Get the Utc Exec object
             * 
             * Retrieves Event::utcexec
             * 
             * @return double representing %Event execution time
             * 
             * @todo Move to .cpp file
             */
            double	getUtcExec()	{	return utcexec;	}

            /**
             * @brief Get the data object
             * 
             * Retrieves Event::data
             * 
             * @return string representing %Event data
             * 
             * @todo Move to .cpp file
             */
            string	get_data()		{	return data; }

            /**
             * @brief Determines if it is time for the %Event to execute
             * 
             * @return true if %Event is ready to execute
             * @return false otherwise false
             * 
             * @todo Move to .cpp file
             */
            bool	is_ready()		{	return (mjd <= currentmjd(0.)); }

            /**
             * @brief Determines if the %Event repeatable
             * 
             * @return true if %Event is repeatable
             * @return false otherwise false
             * 
             * @todo Move to .cpp file
             */
            bool	is_repeat()		{	return (flag & EVENT_FLAG_REPEAT);	}

            /**
             * @brief Determines if the %Event is a command
             * 
             * @return true if %Event is a command
             * @return false otherwise false
             * 
             * @todo Move to .cpp file
             */
            bool	is_command()	{	return (type & EVENT_TYPE_COMMAND || type & EVENT_TYPE_REQUEST);	}

            /** @todo Remove commented-out code? */
//            bool	is_request()	{	return (type & EVENT_TYPE_REQUEST);	}

            /**
             * @brief Determines if the %Event is a conditional command
             * 
             * @return true if %Event is a conditional command
             * @return false otherwise
             * 
             * @todo Move to .cpp file
             */
            bool	is_conditional(){	return (flag & EVENT_FLAG_CONDITIONAL);	}

            /**
             * @brief Determines if the %Event is a solo command
             * 
             * @return true if %Event is a solo command
             * @return false otherwise
             * 
             * @todo Move to .cpp file
             */
            bool	is_solo(){	return (flag & EVENT_FLAG_SOLO);	}

            /**
             * @brief Determines if the %Event has already run
             * 
             * @return true if %Event has already run
             * @return false otherwise
             * 
             * @todo Move to .cpp file
             */
            bool	is_alreadyrun(){	return (flag & EVENT_FLAG_TRUE);	}

            /**
             * @brief Set the already run object
             * 
             * @param value 
             * 
             * @todo Document this.
             */
            void    set_alreadyrun(bool value){ if (value) flag |= EVENT_FLAG_TRUE; else flag &= ~EVENT_FLAG_TRUE; }
        }; // end of class Event
        /** @todo Remove commented-out code? */
//    } // end of namespace Support
} // end of namespace Cosmos

#endif // COSMOS_COMMAND_H
