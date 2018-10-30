/*
 *  Copyright (C) 2013 Jens Thoms Toerring <jt@toerring.de>
 *
 *  This file is part of the pbterm program.
 *
 *  pbterm is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  pbterm is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with pbterm.  If not, see <http://www.gnu.org/licenses/>.
 */


#if ! defined REQUEST_HPP_
#define REQUEST_HPP_


/******************************************
 * Declaration of all requests that can be sent by all components to
 * the Messenger class. On a request a return value must be passed
 * back, thus every object has some member(s) for this. Another aspect
 * is that no const-qualified request eferences can be sent (in contrast
 * to messages).
 ******************************************/


#include <string>
#include <vector>


namespace request 
{
    // Request sent when libinkview sends an event, the request returns
    // if the program dealt with the event.

    struct Handle_Event
    {
        Handle_Event( int type,
                      int par1,
                      int par2 )
            : type( type )
            , par1( par1 )
            , par2( par2 )
            , result( 0 )
        { }

        int type,
            par1,
            par2,
            result;
    };


    // Request sent to obtain a list of all stored commands

    struct Get_Command_List
    {
        std::vector< std::string > const * result;
    };


    // Request sent to inquire if Control characters can be sent to the shell

    struct Can_Use_Ctrl
    {
        bool result;
    };


    // Request sent to obtain the type of the active keyboard

    struct Get_Active_Keyboard
    {
        int result;
    };
};


#endif


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
