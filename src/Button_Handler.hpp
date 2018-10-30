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


#if ! defined BUTTON_HANDLER_HPP_
#define BUTTON_HANDLER_HPP_


#include "Config.hpp"
#include "Inkview.hpp"


class Messenger;


/******************************************
 * Class for reacting to button presses
 ******************************************/

class Button_Handler
{
  public :

    Button_Handler( Messenger & mess,
                    Config    & config )
        : m_mess( mess )
        , m_orientation( config.default_orientation( ) )
    { }


    int
    handle( int type,
            int key,
            int repeat );


    void
    orientation_change( int orientation )  { m_orientation = orientation; }


  private :

    int
    handle_repeat( int key,
                   int repeat );


    int
    handle_release( int key,
                    int repeat );


    int
    factor_in_orientation( int key );


    // Messenger object that needs to be notified about results of button events

    Messenger & m_mess;


    // Current orientation

    int m_orientation;
};


#endif


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
