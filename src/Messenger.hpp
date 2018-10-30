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


#if ! defined MESSENGER_HPP_
#define MESSENGER_HPP_


#include "Message.hpp"
#include "Request.hpp"
#include "Inkview.hpp"


class Logger;
class Display;
class Term;
class Keyboard_Handler;
class Menu_Handler;
class Button_Handler;
class Pointer_Handler;
class Rotation_Handler;


/******************************************
 * Central "switchboard" class: all subsystems send it messages ("do something")
 * or requests ("do something and give me back some information") and this
 * class takes care of dealing with them. Thus all components are completely
 * decoupled because all interaction happens only via messages and requests
 * send to this class.
 *
 * An alternative might be to use boost::signals2 to implement a signal/slot
 * mechanism but that would make building the program more complicated for
 * others (one needs to have boost installed and create a symbolic link in
 * one of the SDK's include directories to the boost header directory) and
 * it nearly doubles the size of the executable.
 ******************************************/

class Messenger
{
  public :

    // Constructor, assembles all subsystems

    Messenger( );


    // Destructor, deletes the subsystems

    ~Messenger( );


    // Templated methods for receiving messages (which never get modified,
    // so a const-qualified reference is expected)

    template < typename Mess >
    void
    send( Mess const & mess );


    // Templated methods for receiving requests (which need to be modified
    // for returning information to the sender)

    template < typename Req >
    Req &
    send( Req & req );


    // If available calls the GetMenuRect() function from libinkview

    bool
    GetMenuRect( imenu * menu,
                 irect & rect );


    // If available calls the GetTouchInfo() function from libinkview

    bool
    GetTouchInfo( iv_mtinfo mtinfo[ 2 ] );


  private :

    // Tries to load the GetMenuRect() and GetTouchInfo() functions from
    // libinkview

    void
    load_unsupported_functions( );


    Logger * m_logger;


    Display * m_display;


    Term * m_term;


    Keyboard_Handler * m_kbd_handler;


    Menu_Handler * m_menu_handler;


    Button_Handler   * m_button_handler;


    Pointer_Handler * m_pointer_handler;


    Rotation_Handler * m_rotation_handler;


    bool m_is_shutting_down;


    bool m_is_recording;


    int m_font_step;


    void * m_inkview_handle;


    GetMenuRect_t m_GetMenuRect;


    GetTouchInfo_t m_GetTouchInfo;
};


#endif


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
