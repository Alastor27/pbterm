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


#if ! defined CONFIG_HPP_
#define CONFIG_HPP_


#include <string>
#include <map>
#include <iostream>
#include "Inkview.hpp"


class Logger;


/******************************************
 * Class for reading the configuration file and afterwards
 * returning the configuraion settings
 ******************************************/

class Config
{
  public :

    // Constructor

    Config( Logger & logger );


    int
    default_orientation( ) const  { return m_default_orientation; }


    int
    check_interval( ) const  { return m_check_interval; }


    // Returns font name

    std::string const &
    font_name( ) const  { return m_font_name; }


    int
    default_font_size( ) const  { return m_default_font_size; }


    int
    font_step( ) const  { return m_font_step; }


    int
    line_spacing( ) const  { return m_line_spacing; }


    int
    tab_width( ) const  { return m_tab_width; }


    int
    max_history( ) const  { return m_max_history; }


    int
    max_lines( ) const  { return m_max_lines; }


    // Returns the name of the shell to be used

    std::string const &
    shell( ) const  { return m_shell; }


    // Returns the name of the file for reading and writing commands

    std::string const &
    cmd_file( ) const  { return m_cmd_file; }


    // Returns the name of the file with user defined commands

    std::string const &
    user_cmd_file( ) const  { return m_user_cmd_file; }


    // Returns the name of the keyboard layout file

    std::string const &
    keyboard_file( ) const  { return m_keyboard_file; }


    Logger &
    logger( )  { return m_logger; }


  private : 

    // Reads is and parses the configuration file

    void
    parse_cfg_file( );


    // Sets up the font name

    void
    checked_font( );


    // Sets up the shell to be used

    void
    checked_shell( );


    // Checks the name of file for reading and storing commands

    void
    checked_cmd_file( );


    // Checks the name of file with the keyboard layout

    void
    checked_keyboard_file( );


    // Checks the file name with default commands

    void
    checked_user_cmd_file( );


    // Switches logger to use a file for  logging

    void
    set_up_logger( );


    // Find and clean a string value in the configuration

    std::string
    get_cleaned_string( std::string const & key );


    // Checks and sets an integer property named 'name'

    void
    checked_int( std::string const & name,
                 int                 min,
                 int                 max,
                 int               & what );


    // Logger object

    Logger & m_logger;


    // Map for configuration key-value pairs

    std::map< std::string, std::string > m_cfg;


    // Default orientation

    int m_default_orientation;


    // Time between checks for shell output

    int m_check_interval;


    // Font name

    std::string m_font_name;


    // Default font size

    int m_default_font_size;


    // Step size for font size changes

    int m_font_step;


    // Distance between lines (in pixel)

    int m_line_spacing;


    // Into how any spaces a tab is to be expanded

    int m_tab_width;


    // Maximum length of the command history

    int m_max_history;


    // Maximum number of lines the Display object remembers

    int m_max_lines;


    // Shell to be used

    std::string m_shell;


    // Name of file for reading and storing commands

    std::string m_cmd_file;


    // Default log file name

    std::string m_log_file;


    // Keyboard layout file

    std::string m_keyboard_file;


    // File for commonly used commands

    std::string m_user_cmd_file;
};


#endif


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
