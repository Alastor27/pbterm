/*
 *  Copyright (C) 2013 Jens Thoms Toerring <jt@toerring.de.de>
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
#include "Logger.hpp"
#include "Config.hpp"
#include "Display.hpp"
#include "Term.hpp"
#include "Keyboard_Handler.hpp"
#include "Menu_Handler.hpp"
#include "Button_Handler.hpp"
#include "Pointer_Handler.hpp"
#include "Rotation_Handler.hpp"
#include <dlfcn.h>


/******************************************
 * Constructor, creates all the subsystems needed
 ******************************************/

Messenger::Messenger( )
    : m_logger( 0 )
    , m_display( 0 )
    , m_term( 0 )
    , m_kbd_handler( 0 )
    , m_menu_handler( 0 )
    , m_button_handler( 0 )
    , m_pointer_handler( 0 )
    , m_is_shutting_down( false )
    , m_is_recording( false )
    , m_inkview_handle( 0 )
    , m_GetMenuRect( 0 )
    , m_GetTouchInfo( 0 )
{
    // Newer versions of the libinkview library have several functions not
    // supported by the current SDK. Try to load them anyway.

    load_unsupported_functions( );

    // Create the object for logging messages

    m_logger = new Logger( );

    // Read in and parse the configuration file

    Config config( *m_logger );

    m_font_step = config.font_step( );

    // Initialize the display subsytem

    m_display = new Display( *this, config );

    if ( m_is_shutting_down )
        return;

    // Prepare the terminal dealing with the shell

    m_term = new Term( *this, config );

    if ( m_is_shutting_down )
        return;

    // Create subsystems for handling button and pointer events and for
    // dealing with the on-screen keyboard, menu and orientation selector.

    m_button_handler  = new Button_Handler( *this, config );
    m_pointer_handler = new Pointer_Handler( *this, config );

    m_kbd_handler      = new Keyboard_Handler( *this, config );
    m_menu_handler     = new Menu_Handler( *this, config );
    m_rotation_handler = new Rotation_Handler( *this, config );
}


/******************************************
 * Destructor - get rid of the subsystem objects
 ******************************************/

Messenger::~Messenger( )
{
    delete m_rotation_handler;
    delete m_pointer_handler;
    delete m_button_handler;
    delete m_menu_handler;
    delete m_kbd_handler;
    delete m_term;
    delete m_display;
    delete m_logger;

    if ( m_inkview_handle )
        dlclose( m_inkview_handle );
}
    

/******************************************
 * Receives the "Close" message, asking libinkview to shut down the program
 * (which then will send at least an EVT_EXIT event)
 ******************************************/

template < >
void
Messenger::send< message::Close >( message::Close const & )
{
    m_is_shutting_down = true;
    CloseApp( );
}


/******************************************
 * Receives the "Text" message, asking for new text to be put on the screen
 ******************************************/

template < >
void
Messenger::send< message::New_Text >( message::New_Text const & mess )
{
    if ( m_is_recording )
        *m_logger << mess.text;

    m_display->add_text( mess.text );
}


/******************************************
 * Receives the "Command" messages, sent when the user entered a new command
 * Sends it to the shell, displays on on the screen and logs it if appropriate.
 ******************************************/

template < >
void
Messenger::send< message::New_Command >( message::New_Command const & mess )
{
    m_term->send_command( mess.cmd );
    m_display->add_text( mess.cmd );
    if ( m_is_recording )
        *m_logger << mess.cmd;
}


/******************************************
 * Receives the "New Ctrl_Char" message, sent when the user asked for a
 * control char being sent to the shell
 ******************************************/

template < >
void
Messenger::send< message::New_Ctrl_Char >( message::New_Ctrl_Char const & mess )
{
    if (    ! mess.ctrl
         || mess.ctrl[ 0 ] != '^'
         || mess.ctrl[ 1 ] < 'A'
         || mess.ctrl[ 1 ] > '[' )
        return;

    m_term->send_control( mess.ctrl[ 1 ] - 'A' + 1 );

    // A "^C" is normally shown on terminals

    if ( mess.ctrl[ 1 ] == 'C' )
    {
        m_display->add_text( mess.ctrl );

        if ( m_is_recording )
            *m_logger << mess.ctrl;
    }
}


/******************************************
 * Receives the "Shift Display" message, sent when the user asked for the
 * display to be scrolled up or down
 ******************************************/

template < >
void
Messenger::send< message::Shift_Display >( message::Shift_Display const & mess )
{
    m_display->shift( mess.dist );
}


/******************************************
 * Receives the "Togge Recording" message, sent when the user asked to switch
 * recording on or off
 ******************************************/

template < >
void
Messenger::send< message::Toggle_Recording >(
                                            message::Toggle_Recording const & )
{
    // Also the display subsystem needs to know since it draws some indicator
    // while recording is on

    if ( m_logger->can_log( ) )
        m_display->recording_state_change( m_is_recording = ! m_is_recording );
}


/******************************************
 * Received the "Change Font" message when the user asks for an
 * increased or decreased font size
 ******************************************/

template < >
void
Messenger::send< message::Change_Font_Size >(
                                       message::Change_Font_Size const & mess )
{
    m_display->change_font_size( mess.difference * m_font_step );
}


/******************************************
 * Receives the "Suspend Display" message to suspend or re-enable display
 * updates, issued when e.g. while the menu or keyboard is shown to keep
 * it from being overdrawn by new data coming from the shell
 ******************************************/

template < >
void
Messenger::send< message::Suspend_Display >(
                                        message::Suspend_Display const & mess )
{
    m_display->suspend_redraws( mess.stop );
}


/******************************************
 * Receives the "Set Display Orientation", requesting a change of the
 * display orientation. The pointer and button handling subsystem need
 * to be made aware of the resulting changes.
 ******************************************/

template < >
void
Messenger::send< message::Set_Orientation >(
                                        message::Set_Orientation const & mess )
{
    m_display->rotate( mess.dir );
    m_pointer_handler->display_size_change( ScreenWidth( ),
                                            ScreenHeight( ) );
    m_button_handler->orientation_change( GetOrientation( ) );
}


/******************************************
 * Receives the "Show Rotate Box" message, resulting in the rotation selector
 * being shown (which, in turn, will result in a "Set Orientation" message).
 ******************************************/

template < >
void
Messenger::send< message::Show_Rotate_Box >( message::Show_Rotate_Box const & )
{
    m_rotation_handler->show( );
}


/******************************************
 * Receives the "Show Keyboard" message when the keyboard is to be displayed
 * on the screen (may, in turn, result in a "Command" message).
 ******************************************/

template < >
void
Messenger::send< message::Show_Keyboard >( message::Show_Keyboard const & mess )
{
    m_kbd_handler->show( mess.txt.empty( ) ?
                         m_term->last_command( ) : mess.txt );
}


/******************************************
 * Receives the "Show Menu" message when the menu is to be displayed
 * on the screen (may result in a number of other messages)
 ******************************************/

template < >
void
Messenger::send< message::Show_Menu >( message::Show_Menu const & mess )
{
    m_menu_handler->show( mess.x, mess.y );
}


/******************************************
 * Receives the "Use Custom Keyboard" message when the user wants to switch
 * use of the custom keyboard on or off
 ******************************************/

template < >
void
Messenger::send< message::Use_Custom_Keyboard >(
                                    message::Use_Custom_Keyboard const & mess )
{
    m_kbd_handler->use_custom_keyboard( mess.yes_no );
}


/******************************************
 * Receives the "Handle Event" request sent for libinkview events.
 ******************************************/

template < >
request::Handle_Event &
Messenger::send< request::Handle_Event >( request::Handle_Event & req )
{
    if ( m_is_shutting_down )
        return req;

    if ( req.type == EVT_SHOW )
    {
        m_display->redraw( );
        req.result = 1;
    }
    else if ( ISKEYEVENT( req.type ) )
        req.result = m_button_handler->handle( req.type, req.par1, req.par2 );
    else if ( ISPOINTEREVENT( req.type ) )
        req.result = m_pointer_handler->handle( req.type, req.par1, req.par2 );
    return req;
}


/******************************************
 * Receives the "Get Command List" request to obtain a list of all
 * commands in the history
 ******************************************/

template < >
request::Get_Command_List &
Messenger::send< request::Get_Command_List >( request::Get_Command_List & req )
{
    req.result = &m_term->command_list( );
    return req;
}


/******************************************
 * Receives the "Can_Use_Crtl" request to inquire if control characters
 * can be sent to the shell
 ******************************************/

template < >
request::Can_Use_Ctrl &
Messenger::send< request::Can_Use_Ctrl >( request::Can_Use_Ctrl & req )
{
    req.result = m_term->using_pty( );
    return req;
}


/******************************************
 * Receives the "Get Active Keyboard" request to obtain the type of the
 * active keyboard
 ******************************************/

template < >
request::Get_Active_Keyboard &
Messenger::send< request::Get_Active_Keyboard >(
                                            request::Get_Active_Keyboard & req )
{
    req.result = m_kbd_handler->active_keyboard( );
    return req;
}


/******************************************
 * If available in libinkview call GetMenuRect() to find out how
 * large a menu will be. Return if calling GetMenuRect() was possible.
 ******************************************/

bool
Messenger::GetMenuRect( imenu * menu,
                        irect & rect )
{
    if ( m_GetMenuRect )
    {
        rect = m_GetMenuRect( menu );
        return true;
    }

    return false;
}


/******************************************
 * If available in libinkview call GetTouchInfo() to find out were the two
 * fingers are at the moment. Return if calling GetTouchInfo() was possible.
 ******************************************/

bool
Messenger::GetTouchInfo( iv_mtinfo mtinfo[ 2 ] )
{
    iv_mtinfo * mti;

    if ( ! m_GetTouchInfo || ! ( mti = m_GetTouchInfo( ) ) )
        return false;

    memcpy( mtinfo, mti, 2 * sizeof *mti );
    return true;
}


/******************************************
 * A number of functions actually available in newer libinkview versions
 * aren't suppported by the current SDK. So we need to use sone tricks to
 * get at them anyway, i.e. via dlopen() and dlsym().
 ******************************************/

void
Messenger::load_unsupported_functions( )
{
    if ( ! ( m_inkview_handle = dlopen( "libinkview.so", RTLD_LAZY ) ) )
        return;

    m_GetMenuRect = reinterpret_cast< GetMenuRect_t >(
                                   dlsym( m_inkview_handle, "GetMenuRect" ) );
    m_GetTouchInfo = reinterpret_cast< GetTouchInfo_t >(
                                   dlsym( m_inkview_handle, "GetTouchInfo" ) );
}


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
