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


#include "Messenger.hpp"
#include "Inkview.hpp"


/******************************************
 * Handler function that gets invoked for all libinkview events,
 * mostly passing them on to a Messenger class instace for dealing
 * with them. Exceptions are the EVT_INIT and EVT_EXIT events that
 * create and destroy the Messenger class instance.
 ******************************************/

static int
inkview_handler( int type,
                 int par1,
                 int par2 )
{
    static Messenger * mess = 0;

    switch ( type )
    {
        case EVT_INIT :
            mess = new Messenger( );
            return 1;

        case EVT_EXIT :
            delete mess;
            return 1;

        default :
            request::Handle_Event he( type, par1, par2 );
            return mess->send( he ).result;
    }

    return 0;
}


/******************************************
 * Here the program starts. All to be done is passing control
 * on to the inkview library that deals with the event loop.
 ******************************************/

int
main( )
{
    InkViewMain( inkview_handler );
    return 0;
}


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
