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


#if ! defined POINTER_HANDLER_HPP_
#define POINTER_HANDLER_HPP_


#include "Inkview.hpp"


class Messenger;
class Config;


/******************************************
 * Class for handling pointer events from the reader
 ******************************************/

class Pointer_Handler
{
  public :

    Pointer_Handler( Messenger & mess,
                     Config    & )
        : m_mess( mess )
        , m_pointer_is_down( false )
        , m_in_pinch( false )
        , m_display_width( ScreenWidth( ) )
        , m_display_height( ScreenHeight( ) )
    { }


    // Handler function for all pointer events

    int
    handle( int type,
            int x,
            int y );


    void
    display_size_change( int width,
                         int height );


  private :

    // Handler function for pointer-down events

    int
    handle_pointer_down( int x,
                         int y );


    // Handler function for pointer-long events

    int
    handle_pointer_long( int x,
                         int y );


    // Handler function for pointer-move events

    int
    handle_pointer_move( int x,
                         int y );


    // Handle two-finger guestures

    int
    handle_pinch( int x,
                  int y );


    // Handler function for pointer-up events

    int
    handle_pointer_up( int x,
                       int y );


    // Handle end of a two-finger guesture

    int
    two_fingers_done( );


    // Calculates difference from start position

    int
    max_excursion( int x,
                   int y );


    int
    dist( int xs,
          int ys,
          int xe,
          int ye );


    // Messenger object to be notified about the results of events

    Messenger & m_mess;


    // Flag, set while the pointer is down

    bool m_pointer_is_down;


    // Flag, set while a two-finger guesture is going on

    bool m_in_pinch;


    // Position of pointer-at down event

    int m_start_x,
        m_start_y;


    // Start and end distance between the fingers in a two-finger guesture

    int m_pinch_start_dist,
        m_pinch_end_dist;

    // Maximum excursion from start point while pointer is down

    int m_max_excursion;


    // Current display width and height

    int m_display_width,
        m_display_height;
};


#endif


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
