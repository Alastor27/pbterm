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


#include "Rotation_Handler.hpp"
#include "Messenger.hpp"
#include "Inkview.hpp"


// Definition of the static member used to find the Rotation_Handler
// instance from within static member functions

Rotation_Handler * Rotation_Handler::s_handling_rotation;


/***************************************
 ***************************************/

Rotation_Handler::Rotation_Handler( Messenger & mess,
                                    Config    & )
    : m_mess( mess )
{
    s_handling_rotation = this;
}


/***************************************
 * Shows the box that allows the user to select a new orientation
 ***************************************/

void
Rotation_Handler::show( )
{
    OpenRotateBox( &Rotation_Handler::static_rotation_handler );
}


/***************************************
 * Function that can be passed to C functions, redirects to the real handler
 ***************************************/

void
Rotation_Handler::static_rotation_handler( int dir )
{
    s_handling_rotation->rotation_handler( dir );
}


/***************************************
 * Handles a newly selected orientation
 ***************************************/

void
Rotation_Handler::rotation_handler( int dir )
{
    m_mess.send( message::Set_Orientation( dir ) );
}


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
