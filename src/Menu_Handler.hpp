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


#if ! defined MENU_HANDLER_HPP_
#define MENU_HANDLER_HPP_


#include "Submenu.hpp"
#include "Config.hpp"
#include <vector>
#include "Inkview.hpp"


class Messenger;
class Config;


/***************************************
 * Class for dealing with the on-screen menu
 ***************************************/

class Menu_Handler
{
  public :

    Menu_Handler( Messenger & mess,
                  Config    & config );


    void
    show( int x,
          int y );


  private :

    static Menu_Handler * s_handling_menu;


    static void
    static_menu_handler( int index );


    void
    menu_handler( int index );


    void
    submenu_command( int index );


    void
    prepare_submenus( );


    void
    get_menu_size( int & w,
                   int & h );


    std::vector< std::string >
    read_user_cmd_file( std::string cmd_file );


    // Messenger object that gets notified on menu selections

    Messenger & m_mess;


    // Enumeration for the enries indexes in the main menu (the sentinel
    // at the end is used as the starting index for submenus)

    enum Menu_Items {
        Menu_Cmd_List,
        Menu_User_Cmd,
        Menu_Send_Ctrl,
        Menu_Keyboard,
        Menu_Rotate,
        Menu_Exit,
        Menu_First_Unused
    };


    // Storage for the main menu

    std::vector< imenu > m_main_menu;


    // Storage for the submenus

    std::vector< Submenu > m_submenus;


    // List of commands for the "Default commands" submenu


    std::vector< std::string > m_user_cmd;
};


#endif


 /*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
