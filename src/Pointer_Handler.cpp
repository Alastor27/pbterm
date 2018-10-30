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


#include "Pointer_Handler.hpp"
#include "Messenger.hpp"
#include "Defaults.hpp"
#include <cmath>
#include <algorithm>


/******************************************
 * Handler for all pointer events
 ******************************************/

int
Pointer_Handler::handle( int type,
                         int x,
                         int y )
{
    switch ( type )
    {
        case EVT_POINTERDOWN :
            return handle_pointer_down( x, y );

        case EVT_POINTERLONG :
            return handle_pointer_long( x, y );

        case EVT_POINTERMOVE :
            return handle_pointer_move( x, y );

        case EVT_POINTERUP :
            return handle_pointer_up( x, y );

        case EVT_MTSYNC :
            return handle_pinch( x, y );
    }

    return 0;
}


/******************************************
 * Handler for pointer down events
 ******************************************/

int
Pointer_Handler::handle_pointer_down( int x,
                                      int y )
{
    if ( m_pointer_is_down )
        return 0;

    m_pointer_is_down = true;
    m_in_pinch = false;
    m_start_x = x;
    m_start_y = y;
    m_max_excursion = 0;

    return 1;
}


/******************************************
 * Handler for pointer long events (i.e. the pointer remained
 * in the same place for a longer time)
 ******************************************/

int
Pointer_Handler::handle_pointer_long( int x,
                                      int y )
{
    if ( ! m_pointer_is_down || m_in_pinch )
        return 0;

    m_max_excursion = max_excursion( x, y );

    if ( m_max_excursion > MAX_EXCURSION )
        return 0;

    m_pointer_is_down = false;
    m_mess.send( message::Show_Menu( x, y ) );

    return 1;
}


/******************************************
 * Handler for pointer move events
 ******************************************/

int
Pointer_Handler::handle_pointer_move( int x,
                                      int y )
{
    if ( ! m_pointer_is_down || m_in_pinch )
        return 0;

    m_max_excursion = max_excursion( x, y );

    return 1;
}


/******************************************
 * Called on EVT_MTSYNC events to check if a two-fungure guesture is
 * starting, going on or ends. Depends on the availability of the
 * GetTouchInfo() function in the libinkview used (otherwise nothing
 * happens). On start of a two-finger guesture the start distance
 * between the fingers is recorded, during such a guesture the new
 * distanc is stored, and at the end the font size is increased or
 * decreased according to how far the fingers were moved apart or
 * together.
 ******************************************/

int
Pointer_Handler::handle_pinch( int,
                               int cnt )
{
    // Nothing to be done while we aren't in pointer down mode or we didn't
    // detect a two-finger guesture yet and there's just a single finger on
    // the touch screen

    if (    ! m_pointer_is_down
         || ( ! m_in_pinch && cnt < 2 ) )
        return 0;

    // Try to get the coordinates of the fingers - this requires the
    // GetTouchInfo() function to be availalble.

    iv_mtinfo mtinfo[ 2 ];

    if ( ! m_mess.GetTouchInfo( mtinfo ) )
        return 0;

    // There are three interesting cases
    // a) we didn't detect a two-finger guesture yet and it's starting now,
    //    so store the start distance
    // b) we're in a two-fingure guesture and the distance between the fingers
    //    changed, store the new distance
    // c) the two-finger guesture ended, so change the font size according to
    //    how far the fingers moved apart or together since the start of the
    //    guesture (and don't bother with any other movements of the fingers
    //    anymore until they have been taken away)

    if ( ! m_in_pinch && cnt == 2 )
    {
        m_pinch_start_dist = m_pinch_end_dist =
                                         dist( mtinfo[ 0 ].x, mtinfo[ 0 ].y,
                                               mtinfo[ 1 ].x, mtinfo[ 1 ].y );
        m_in_pinch = true;
    }
    else if ( m_in_pinch && cnt == 2 )
        m_pinch_end_dist = dist( mtinfo[ 0 ].x, mtinfo[ 0 ].y,
                                 mtinfo[ 1 ].x, mtinfo[ 1 ].y );
    else if ( m_in_pinch && cnt == 1 )
    {
        m_pointer_is_down = false;
        return two_fingers_done( );
    }

    return 1;
}


/******************************************
 * Handler for pointer up events
 ******************************************/

int
Pointer_Handler::handle_pointer_up( int x,
                                    int y )
{
    if ( ! m_pointer_is_down )
        return 0;

    m_pointer_is_down = false;

    // If we were coming from a two-finger guesture change the font size

    if ( m_in_pinch )
        return two_fingers_done( );
 
    // Otherwise this was for scrolling

   m_max_excursion = max_excursion( x, y );

    // If the excursion wasn't too large treat it as a tap and either show
    // the keyboard or toggle recording (if the tap was in the upper right
    // hand corner) or close the program (if the tap was in the upper left
    // hand corner). If this was a swipe instead shift the screen content.

    if ( m_max_excursion <= MAX_EXCURSION )
    {
        if (    m_start_x >= m_display_width - ON_SCREEN_BUTTON_SIZE
             && m_start_y <= ON_SCREEN_BUTTON_SIZE )
            m_mess.send( message::Toggle_Recording( ) );
        else if (    m_start_x <= ON_SCREEN_BUTTON_SIZE
                  && m_start_y <= ON_SCREEN_BUTTON_SIZE )
            m_mess.send( message::Close( ) );
        else if (    m_start_x <= m_display_width / 4
                  && m_start_y >= 3 * m_display_height / 4 )
            m_mess.send( message::Show_Menu( m_start_x, m_start_y ) );
        else
            m_mess.send( message::Show_Keyboard( ) );
    }
    else
        m_mess.send( message::Shift_Display( y - m_start_y ) );

    return 1;
}


/******************************************
 * Informs the object that the display size changed
 ******************************************/

void
Pointer_Handler::display_size_change( int width,
                                      int height )
{
    m_display_width  = width;
    m_display_height = height;
}


/******************************************
 * Called on the end of a two-finger guesture. Change the font size
 * proportional to the amount the fingers were moved apart or together.
 ******************************************/

int
Pointer_Handler::two_fingers_done( )
{
    m_in_pinch = false;

    int steps = ( m_pinch_end_dist - m_pinch_start_dist ) / PINCH_DISTANCE;
    
    if ( steps != 0 )
        m_mess.send( message::Change_Font_Size( steps ) );

    return 1;
}


/******************************************
 * Calculates the maximum excursion of the pointer from the start position.
 ******************************************/

int
Pointer_Handler::max_excursion( int x,
                                int y )
{
    return std::max< int >( m_max_excursion,
                            dist( x, y, m_start_x, m_start_y ) );
}


/******************************************
 * Calculates the distance between two points (rounded down to the next
 * integer)
 ******************************************/

int
Pointer_Handler::dist( int xs,
                       int ys,
                       int xe,
                       int ye )
{
    return static_cast< int >( sqrt(    ( xe - xs ) * ( xe - xs )
                                      + ( ye - ys ) * ( ye - ys ) ) );
}


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
