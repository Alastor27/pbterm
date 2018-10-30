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


#include "Menu_Handler.hpp"
#include "Messenger.hpp"
#include "Utils.hpp"
#include <string>
#include <vector>
#include <dlfcn.h>
#include <fstream>


// Definition of the static member used to find the Menu_Handler instance
// from within static member functions

Menu_Handler * Menu_Handler::s_handling_menu;


/***************************************
 * Constructor, creates the main menu's entries
 ***************************************/

Menu_Handler::Menu_Handler( Messenger & mess,
                            Config    & config )
    : m_mess( mess )
    , m_user_cmd( read_user_cmd_file( config.user_cmd_file( ) ) )
{
    s_handling_menu = this;

    // Create the main menu

    imenu tmp[ ] = { { ITEM_SUBMENU, 0,           "Prev. commands", 0 },
                     { ITEM_SUBMENU, 0,           "User commands",  0 },
                     { ITEM_SUBMENU, 0,           "Send CTRL",      0 },
                     { ITEM_SUBMENU, 0,           "Keyboard",       0 },
                     { ITEM_ACTIVE,  Menu_Rotate, "Rotate",         0 },
                     { ITEM_ACTIVE,  Menu_Exit,   "Exit",           0 },
                     { 0,            0,           0,                0 } };

    for ( std::size_t i = 0; i < sizeof tmp / sizeof *tmp; ++i )
        m_main_menu.push_back( tmp[ i ] );
}


/***************************************
 * Shows the menu with the upper left hand corner at x and y
 ***************************************/

void
Menu_Handler::show( int x,
                    int y )
{
    // Set up the submenus with the list of previos commands and the
    // control characters

    prepare_submenus( );

    // Try to get the width and height of the menu (for centering)

    int w = 0,
        h = 0;
    irect r;

    if ( m_mess.GetMenuRect( &m_main_menu.front( ), r ) )
    {
        w = r.w;
        h = r.h;
    }

    // Suspend screen updates while the menu is shown

    m_mess.send( message::Suspend_Display( true ) );

    // Ask for the menu to be shown on the display

    OpenMenu( &m_main_menu.front( ), 0, std::max< int >( 20, x - w / 2 ),
              y - h / 2, &Menu_Handler::static_menu_handler );
}


/***************************************
 ***************************************/

void
Menu_Handler::static_menu_handler( int index )
{
    s_handling_menu->menu_handler( index );
}


/***************************************
 * Handle what the menu returns
 ***************************************/

void
Menu_Handler::menu_handler( int index )
{
    // Re-enable screen updates

    m_mess.send( message::Suspend_Display( false ) );

    switch ( index )
    {
        case Menu_Rotate :
            m_mess.send( message::Show_Rotate_Box( ) );
            break;

        case Menu_Exit :
            m_mess.send( message::Close( ) );
            break;

        default :
            submenu_command( index );
    }

    // Get rid of most memory needed for the submenus

    m_submenus.clear( );    
}


/***************************************
 * Called for a return index from submenu (but also for the index returned
 * when the user closed the menu without making a selection)
 ***************************************/

void
Menu_Handler::submenu_command( int index )
{
    if ( m_submenus[ Menu_Cmd_List ].has( index ) )
        m_mess.send( message::Show_Keyboard(
                              m_submenus[ Menu_Cmd_List ][ index ].text ) );
    if ( m_submenus[ Menu_User_Cmd ].has( index ) )
    {
        std::string txt = m_submenus[ Menu_User_Cmd ][ index ].text;
        if ( txt[ txt.size( ) - 1 ] == '#' )
        {
            txt[ txt.size( ) - 1 ] = '\n';
            m_mess.send( message::New_Command( txt ) );
        }
        else
            m_mess.send( message::Show_Keyboard( txt ) );
    }
    else if ( m_submenus[ Menu_Send_Ctrl ].has( index ) )
        m_mess.send( message::New_Ctrl_Char(
                              m_submenus[ Menu_Send_Ctrl ][ index ].text ) );
    else if ( m_submenus[ Menu_Keyboard ].has( index ) )
        m_mess.send( message::Use_Custom_Keyboard(
                     m_submenus[ Menu_Keyboard ][ index ].text == "Pbterm" ) );
}


/***************************************
 * Creates the two submenus, one for the "Prev. command" entry of the
 * main menu and one for the "end CTRL" entry.
 ***************************************/

void
Menu_Handler::prepare_submenus( )
{
    m_submenus.clear( );

    // Create the submenu for editing previous commands (with the items
    // shown in reverse order of creation, i.e. with the most recent
    // commands being shown at the top of the submenu)

    m_submenus.push_back( Submenu( Menu_First_Unused, Submenu::Reverse ) );

    request::Get_Command_List gcl;
    std::vector< std::string > const * cmd_list = m_mess.send( gcl ).result;

    for ( std::vector< std::string >::const_iterator it = cmd_list->begin( );
          it != cmd_list->end( ); ++it )
        m_submenus.back( ).add( ITEM_ACTIVE, it->c_str( ) );

    // Create submenu with default commands

    m_submenus.push_back( Submenu( m_submenus.back( ).next_free_index( ) ) );

    for ( std::vector< std::string >::const_iterator it = m_user_cmd.begin( );
          it != m_user_cmd.end( ); ++it )
        m_submenus.back( ).add( ITEM_ACTIVE, it->c_str( ) );

    // Create the submenu for sending control characters

    char const * ctrl[ ] = { "^C", "^D", "^Z", "^[" };

    m_submenus.push_back( Submenu( m_submenus.back( ).next_free_index( ) ) );

    for ( std::size_t i = 0; i < sizeof ctrl / sizeof *ctrl; ++i )
        m_submenus.back( ).add( ITEM_ACTIVE, ctrl[ i ] );

    // Add submenu for keyboard selection

    request::Get_Active_Keyboard gak;
    m_mess.send( gak );

    char const * kbds[ ] = { "Pbterm", "Default" };

    m_submenus.push_back( Submenu( m_submenus.back( ).next_free_index( ) ) );

    for ( std::size_t i = 0; i < sizeof kbds / sizeof * kbds; ++i )
        m_submenus.back( ).add( static_cast< int >( i ) == gak.result ?
                                ITEM_BULLET : ITEM_ACTIVE, kbds[ i ] );

    // Set them up in the main menu - it makes no sense to have the command
    // submenu shown at all if there are no previos commands. And if the
    // communication with the shell is not via a pseudoterminal sending
    // control characters doesn't make sense.

    if ( m_submenus.front( ).size( ) )
    {
        m_main_menu[ Menu_Cmd_List ].type    = ITEM_SUBMENU;
        m_main_menu[ Menu_Cmd_List ].submenu =
                                           m_submenus[ Menu_Cmd_List ].addr( );
    }
    else
    {
        m_main_menu[ Menu_Cmd_List ].type    = ITEM_INACTIVE;
        m_main_menu[ Menu_Cmd_List ].submenu = 0;
    }

    //

    if ( m_user_cmd.size( ) )
    {
        m_main_menu[ Menu_User_Cmd ].type    = ITEM_SUBMENU;
        m_main_menu[ Menu_User_Cmd ].submenu =
                                           m_submenus[ Menu_User_Cmd ].addr( );
    }
    else
    {
        m_main_menu[ Menu_User_Cmd ].type    = ITEM_INACTIVE;
        m_main_menu[ Menu_User_Cmd ].submenu = 0;
    }

    // Enable submenu for keyboard selection if we're communicating with the
    // shell via a PTY

    request::Can_Use_Ctrl cuc;
    if ( m_mess.send( cuc ).result )
    {
        m_main_menu[ Menu_Send_Ctrl ].type    = ITEM_SUBMENU;
        m_main_menu[ Menu_Send_Ctrl ].submenu =
                                          m_submenus[ Menu_Send_Ctrl ].addr( );
    }
    else
    {
        m_main_menu[ Menu_Send_Ctrl ].type    = ITEM_INACTIVE;
        m_main_menu[ Menu_Send_Ctrl ].submenu = 0;
    }

    // Enable submenu for keyboard selection if a custom keyboard can be used

    if ( gak.result != -1 )
    {
        m_main_menu[ Menu_Keyboard ].type    = ITEM_SUBMENU;
        m_main_menu[ Menu_Keyboard ].submenu =
                                           m_submenus[ Menu_Keyboard ].addr( );
    }
    else
    {
        m_main_menu[ Menu_Keyboard ].type    = ITEM_INACTIVE;
        m_main_menu[ Menu_Keyboard ].submenu = 0;
    }
}


/***************************************
 * Tries to read in the file with default commands
 ***************************************/

std::vector< std::string >
Menu_Handler::read_user_cmd_file( std::string cmd_file )
{
    std::vector< std::string > cmd_list;

    // Try to open the file with default commands for reading

    std::ifstream ifs( cmd_file.c_str( ), std::ifstream::in );

    std::string  line;

    // Keep reading until the end of the file is reached

    while ( ifs.good( ) )
    {
        std::getline( ifs, line );

        if ( ifs.fail( ) )
            break;

        // Remove empty lines and comments (lines starting with a hash mark)

        size_t p;
        if (    ( p = line.find_first_not_of( " \t" ) ) == std::string::npos
             || line[ p ] == '#' )
            continue;

        cmd_list.push_back( line );
    }

    return cmd_list;
}


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
