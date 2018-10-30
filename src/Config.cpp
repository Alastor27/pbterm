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


#include "Config.hpp"
#include "Logger.hpp"
#include "Defaults.hpp"
#include "Utils.hpp"
#include <fstream>
#include <limits>
#include <algorithm>
#include "Inkview.hpp"


/******************************************
 ******************************************/

Config::Config( Logger & logger )
    : m_logger( logger )
    , m_default_orientation( DEFAULT_ORIENTATION       )
    , m_check_interval(      DEFAULT_CHECK_INTERVAL    )
    , m_font_name(           DEFAULTFONTM              )
    , m_default_font_size(   DEFAULT_FONT_SIZE         )
    , m_font_step(           FONT_STEP                 )
    , m_line_spacing(        LINE_SPACING              )
    , m_tab_width(           TAB_WIDTH                 )
    , m_max_history(         DEFAULT_MAX_CMD_HISTORY   )
    , m_max_lines(           DEFAULT_MAX_DISPLAY_LINES )
    , m_shell(               SHELL_PATH                )
    , m_cmd_file(            DEFAULT_CMD_FILE          )
    , m_log_file(            DEFAULT_LOG_FILE          )
    , m_keyboard_file(       KEYBOARD_FILE             ) 
    , m_user_cmd_file(       USER_CMD_FILE             )
{
    // Try to read in the configuration file

    parse_cfg_file( );

    set_up_logger( );
    checked_font( );
    checked_shell( );
    checked_cmd_file( );
    checked_keyboard_file( );
    checked_user_cmd_file( );

    // Check for integer settings from the configuration file

    checked_int( "orientation", 0, 3, m_default_orientation );
    checked_int( "check_interval", 50, 10000, m_check_interval );
    checked_int( "font_size", 6, 72, m_default_font_size );
    checked_int( "font_step", 1, 10, m_font_step );
    checked_int( "line_spacing", - m_default_font_size, 100,
                 m_line_spacing );
    checked_int( "tab_width", 2, 8, m_tab_width );
    checked_int( "max_history", 0, std::numeric_limits< int >::max( ),
                 m_max_history );
    checked_int( "max_lines", 20, std::numeric_limits< int >::max( ),
                 m_max_lines );

    for ( std::map< std::string, std::string >::iterator it = m_cfg.begin( );
          it != m_cfg.end( ); ++it )
        m_logger.warn( ) << "Unsupported keyword '" << it->first
                         << "' found in configuration file" << std::endl;
}


/******************************************
 ******************************************/

void
Config::parse_cfg_file( )
{
    // Try to open the configuration file for reading

    std::ifstream ifs( CONFIG_FILE, std::ifstream::in );

    std::string  line;
    int cnt = 0;

    while ( ifs.good( ) )
    {
        std::getline( ifs, line );

        if ( ifs.fail( ) )
            break;

        cnt++;

        // Remove comments (starting with a hash '#')

        std::size_t pos = line.find( '#' );

        if ( pos != std::string::npos )
            line = line.substr( 0, pos );

        if ( Utils::trim( line ).empty( ) )
            continue;

        // Split into key and value at the first colon

        std::size_t n = line.find( ':' );
        if ( n == std::string::npos )
        {
            m_logger.warn( ) << "Line " << cnt << " of configuration file "
                             << "is invalid (missing colon)" << std::endl;
            continue;
        }

        std::string key( Utils::trim( line.substr( 0, n ) ) );
        std::string val( Utils::trim( line.substr( n + 1 ) ) );

        if ( key.empty( ) )
        {
            m_logger.warn( ) << "Missing keyword on line " << cnt
                             << " of configuration file" << std::endl;
            continue;
        }

        // We want keywords in all lowercase but accept them in whatever
        // the user prefers, so convert to lowercase now.

        std::transform( key.begin(), key.end(),
                        key.begin(), ::tolower );

        if ( m_cfg.find( key ) != m_cfg.end( ) )
        {
            m_logger.warn( ) << "Redefinition of keyword '" << key
                             << " on line " << cnt
                             << " of configuration file, skipped" << std::endl;
        }

        m_cfg[ key ] = val;
    }
}


/******************************************
 ******************************************/

void
Config::checked_font( )
{
    std::string font_name( get_cleaned_string( "font_name" ) );

    if ( ! font_name.empty( ) )
    {
        ifont * f = OpenFont( font_name.c_str( ), m_default_font_size, 0 );

        if ( ! f )
            m_logger.warn( ) << "Can't use font '" << font_name
                             << "' requested in configuartion file"
                             << std::endl;
        else
        {
            CloseFont( f );
            m_font_name = font_name;
        }
    }

    m_cfg.erase( "font_name" );
}


/******************************************
 ******************************************/

void
Config::checked_cmd_file( )
{
    std::string cmd_file( get_cleaned_string( "command_file" ) );

    if ( ! cmd_file.empty( ) )
    {
        if ( access( cmd_file.c_str( ), R_OK ) )
            m_logger.warn( ) << "Can't access command file '" << cmd_file
                             << "' requested in configuration file"
                             << std::endl;
        else
            m_cmd_file = cmd_file;
    }

    m_cfg.erase( "command_file" );
}


/******************************************
 ******************************************/

void
Config::checked_keyboard_file( )
{
    if ( m_cfg.find( "keyboard_file" ) == m_cfg.end( ) )
        return;

    m_keyboard_file = get_cleaned_string( "keyboard_file" );
    m_cfg.erase( "keyboard_file" );
}


/******************************************
 ******************************************/

void
Config::checked_shell( )
{
    std::string shell( get_cleaned_string( "shell" ) );

    // Check that the name is ok, it exists and we're allowed to execute it

    if ( ! shell.empty( ) )
    {
        if ( access( shell.c_str( ), X_OK ) )
            m_logger.warn( ) << "Can't use shell '" << shell
                             << "' requested in configuration file"
                             << std::endl;
        else
            m_shell = shell;
    }

    m_cfg.erase( "shell" );
}


/******************************************
 ******************************************/

void
Config::checked_user_cmd_file( )
{
    std::string user_cmd_file( get_cleaned_string( "user_cmd_file" ) );

    if ( ! user_cmd_file.empty( ) )
    {
        if ( access( user_cmd_file.c_str( ), R_OK ) )
            m_logger.warn( ) << "Can't access default commands file '"
                             << user_cmd_file << "' requested in "
                             << "configuration file" << std::endl;
        else
            m_user_cmd_file = user_cmd_file;
    }

    m_cfg.erase( "user_cmd_file" );
}


/******************************************
 ******************************************/

void
Config::set_up_logger( )
{
    std::string log_file( get_cleaned_string( "log_file" ) );

    // If user defined log file can't be opened use the default one

    if ( log_file.empty( ) || ! m_logger.set_file( log_file ) )
    {
        if ( ! log_file.empty( ) )
            m_logger.warn( ) << "Can't open log file '" << log_file
                             << "' requested in configuration file"
                             << std::endl;
        m_logger.set_file( m_log_file );
    }

    m_cfg.erase( "log_file" );
}       


/******************************************
 ******************************************/

std::string
Config::get_cleaned_string( std::string const & key )
{
    std::map< std::string, std::string >::iterator it = m_cfg.find( key );

    if ( it == m_cfg.end( ) )
        return "";

    // Remove enclosing parentheses

    if (    ! it->second.empty( )
         && it->second.at( 0 ) == '"'
         && it->second.at( it->second.size( ) - 1  ) == '"' )
        return it->second.substr( 1, it->second.size( ) - 2 );

    return it->second;
}


/******************************************
 ******************************************/

void
Config::checked_int( std::string const & name,
                     int                 min,
                     int                 max,
                     int               & what )
{
    std::map< std::string, std::string >::iterator it = m_cfg.find( name );
    int tmp;

    if ( it == m_cfg.end( ) )
        return;

    if ( it->second.empty( ) )
    {
        m_cfg.erase( name );
        m_logger.warn( ) << "Missing value for keyword '" << name
                         << "' in configuration file" << std::endl;
        return;
    }
        
    if ( ! Utils::to_int( tmp, it->second ) )
    {
        m_cfg.erase( name );
        m_logger.warn( ) << "Value for keyword '" << name
                         << "' in configuration file isn't an integer"
                         << std::endl;
        return;
    }
        
    m_cfg.erase( name );

    if ( tmp < min || tmp > max )
    {
        m_logger.warn( ) << "Value for keyword '" << name
                         << "' in configuration file isn't in the range ["
                         << min << ", " << max << "]" << std::endl;
        return;
    }

    what = tmp;
}


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
