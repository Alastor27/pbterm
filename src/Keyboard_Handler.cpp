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


#include "Keyboard_Handler.hpp"
#include "Messenger.hpp"
#include "Config.hpp"
#include "Utils.hpp"
#include "Defaults.hpp"
#include "Inkview.hpp"


// Definition of the static member used to find the Keyboard_Handler
// instance from within static member functions

Keyboard_Handler * Keyboard_Handler::s_handling_keyboard;


/******************************************
 * Constructor
 ******************************************/

Keyboard_Handler::Keyboard_Handler( Messenger & mess,
                                    Config    & config )
    : m_mess( mess )
    , m_keyboard_is_shown( false )
    , m_kbd_file( config.keyboard_file( ) )
    , m_use_custom(    ! m_kbd_file.empty( )
                    && ! access( m_kbd_file.c_str( ), R_OK ) )
{
    s_handling_keyboard = this;
}


/******************************************
 * Gets the keyboard displayed to let the user enter a new command.
 ******************************************/

void
Keyboard_Handler::show( std::string const & default_command )
{
    if ( m_keyboard_is_shown )
        return;

    // Set up the buffer with the default command (make sure the buffer
    // doesn't overflow, e.g. because the user manually modified the
    // command file and added a command that's too long)

    strncpy( m_cmd_buffer, default_command.c_str( ), MAX_CMD_LEN - 1 );
    m_cmd_buffer[ MAX_CMD_LEN - 1 ] = '\0';

    m_keyboard_is_shown = true;

    // Suspend screen updates while the keyboard is shown

    m_mess.send( message::Suspend_Display( true ) );

    // If there's a usable custom keyboard layput file use that, otherwise
    // whatever the system treats as the default.
    // Note: we use 'MAX_CMD_LEN - 1' since due to a bug in libinkview
    // the user may enter one more glyph than that argument.

    if ( active_keyboard( ) == 0 )
        OpenCustomKeyboard( m_kbd_file.c_str( ),
                            "Enter shell command (or 'exit' to quit)",
                            m_cmd_buffer, MAX_CMD_LEN - 1, 0,
                            &Keyboard_Handler::static_kbd_handler );
    else
        OpenKeyboard( "Enter shell command (or 'exit' to quit)",
                      m_cmd_buffer, MAX_CMD_LEN - 1, 0,
                      &Keyboard_Handler::static_kbd_handler );
}


/******************************************
 * Returns -1 if there's no custom keyboard available, 0 if the custom
 * keyboard is in use and 1 if the default keyboard is used.
 ******************************************/

int
Keyboard_Handler::active_keyboard( )
{
    if ( m_kbd_file.empty( ) || access( m_kbd_file.c_str( ), R_OK ) )
        return -1;

    return ! m_use_custom;
}


/******************************************
 * If called with a 'true' value sets use of custom keyboard (if one
 * is available), otherwise switches it off
 ******************************************/

void
Keyboard_Handler::use_custom_keyboard( bool yes_no )
{
    if ( ! m_kbd_file.empty( ) && ! access( m_kbd_file.c_str( ), R_OK ) )
        m_use_custom = yes_no;
}


/******************************************
 * Helper function that can be called from C, redirects to the
 * real handler function
 ******************************************/

void
Keyboard_Handler::static_kbd_handler( char * cmd )
{
    s_handling_keyboard->kbd_handler( cmd );
}


/******************************************
 * Called when the user pressed the keyboards <ENTER> button (or closed
 * it via the "Close" button)
 ******************************************/

void
Keyboard_Handler::kbd_handler( char * cmd )
{
    m_keyboard_is_shown = false;

    // Re-enable screen updates

    m_mess.send( message::Suspend_Display( false ) );

    // A NULL pointer seems to be delivered when the user closed the
    // keyboard via the "Close" button

    if ( ! cmd )
        return;

    // If there's at least one non-white-space character in the input send
    // it to the messenger after attaching a newline characters (as we would
    // get it from a "normal" keyboard)

    std::string s( Utils::trim( cmd ) );
    if ( s.empty( ) )
        return;

    m_mess.send( message::New_Command( s + "\n" ) );
}


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
