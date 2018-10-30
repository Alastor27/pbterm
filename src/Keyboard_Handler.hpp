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


#if ! defined KEYBOARD_HANDLER_HPP_
#define KEYBOARD_HANDLER_HPP_


#include "Defaults.hpp"
#include <string>


class Messenger;
class Config;


/******************************************
 * Class for dealing with the on-screen keyboard
 ******************************************/

class Keyboard_Handler
{
  public :

    Keyboard_Handler( Messenger & mess,
                      Config    & config );


    void
    show( std::string const & default_command );


    int
    active_keyboard( );


    void
    use_custom_keyboard( bool yes_no );


  private :

    static void
    static_kbd_handler( char * cmd );


    void
    kbd_handler( char * cmd );


    // Messenger object that gets notified about new input

    Messenger & m_mess;


    // Flag, set while keynoard is displayed

    bool m_keyboard_is_shown;


    // Buffer for keyboard input. Note: since UTF-8 characters seem to be
    // used internally and due to a bug in the keyboard code that alloes
    // entering one more glyph than the maximum length set via 'maxlen'
    // (which seems to be inntended to be the number of glyphs, not the
    // size of the buffer), we need a buffer that's a lot larger than
    // just 'MAX_CMD_LEN'!

    char m_cmd_buffer[ 4 * MAX_CMD_LEN + 1 ];


    // Name of file with keyboard layout

    std::string m_kbd_file;


    // Flag, set when custom keyboard is to be used

    bool m_use_custom;


    // Needed in static keyboard handler to call the real handler function

    static Keyboard_Handler * s_handling_keyboard;
};


#endif


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
