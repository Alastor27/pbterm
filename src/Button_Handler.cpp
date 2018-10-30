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


#include "Button_Handler.hpp"
#include "Messenger.hpp"


/******************************************
 * Handler for button events
 ******************************************/

int
Button_Handler::handle( int type,
                        int key,
                        int repeat )
{
    key = factor_in_orientation( key );

    switch ( type )
    {
        case EVT_KEYREPEAT :
            return handle_repeat( key, repeat );

        case EVT_KEYUP :
            return handle_release( key, repeat );
    }

    return 0;
}


/******************************************
 * Handler for repeat event, i.e. the user kept the button pressed
 * for a longer time
 ******************************************/

int
Button_Handler::handle_repeat( int key,
                               int repeat )
{
    // Don't react repeatedly, only on the first event

    if ( repeat != 1 )
        return 0;

    switch ( key )
    {
        case KEY_NEXT :
            m_mess.send( message::Change_Font_Size( 1 ) );
            return 1;


        case KEY_PREV :
            m_mess.send( message::Change_Font_Size( -1 ) );
            return 1;
    }

    return 0;
}


/******************************************
 * Handler for release event
 ******************************************/

int
Button_Handler::handle_release( int key,
                                int repeat )
{
    // If there was a repeat event everything has already been handled

    if ( repeat != 0 )
        return 0;

    switch ( key )
    {
        case KEY_MENU :
            m_mess.send( message::Show_Keyboard( ) );
            break;

        case KEY_NEXT :
            m_mess.send( message::Toggle_Recording( ) );
            break;

        case KEY_PREV :
            m_mess.send( message::Show_Menu( ) );
            break;

        default :
            return 0;
    }

    return 1;
}


/******************************************
 * The button marked with a triangle pointing to the right or up
 * should be treated as the "forward" button, while the other one
 * as the "backward" button. Of course, what is "up" or "to the
 * right" depends on the orientation - and the Pocketbook does
 * it's own mixing the return values up, exchanging KEY_NEXT
 * and KEY_PREV for the 90 and 180 ccw orientation.
 ******************************************/

int
Button_Handler::factor_in_orientation( int key )
{
    if (    ( key != KEY_NEXT && key != KEY_PREV )
         || m_orientation == ROTATE0
         || m_orientation == ROTATE180 )
        return key;

    return key == KEY_NEXT ? KEY_PREV : KEY_NEXT;
}


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
