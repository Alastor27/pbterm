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


#include "Utils.hpp"
#include <limits>
#include <cerrno>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


namespace Utils {

/******************************************
 * Trims white-space from start of a line,
 * returning a new string instance
 ******************************************/

std::string
left_trim( std::string const & str )
{
    std::string s( str );
    left_trim( s );
    return s;
}


/******************************************
 * Trims white-space from start of line,
 * modifying the line
 ******************************************/

std::string &
left_trim( std::string & str )
{
    std::size_t pos = str.find_last_not_of( " \t\r\n" );
    if ( pos != std::string::npos )
        str = str.substr( 0, pos + 1 );
    return str;
}


/******************************************
 * Trims white-space from start of a line,
 * returning a new string instance
 ******************************************/

std::string
right_trim( std::string const & s )
{
    std::string str( s );
    right_trim( s );
    return s;
}


/******************************************
 * Trims white-space from start of line,
 * modifying the line
 ******************************************/

std::string &
right_trim( std::string & str )
{
    std::size_t pos = str.find_first_not_of( " \t\n\r" );
    if ( pos != std::string::npos )
        str = str.substr( pos );
    return str;
}


/******************************************
 * Trims white-space from start and end of line,
 * returning a new string instance
 ******************************************/

std::string
trim( std::string const & str )
{
    std::string s( str );
    trim( s );
    return s;
}


/******************************************
 * Trims white-space from start and end of line,
 * modifying the line
 ******************************************/

std::string &
trim( std::string & str )
{
    return left_trim( right_trim( str ) );
}


/******************************************
 * Converts a string to int, return true if this was possible
 * (i.e. the string consisted of digits only and only leading,
 * and trailing whitespace, the base, if specified, was between
 * 2 and 36 and the resulting number fits into and int) and
 * the result via the first argument, otherwise false. 
 ******************************************/

bool
to_int( int               & result,
        std::string const & s,
        int                 base )
{
    if ( base < 2 || base > 36 )
        return false;

    char *endp;
    errno = 0;
    long v = strtol( s.c_str( ), &endp, base );

    if (    errno
         || endp == s.c_str( )
         || *endp
         || v > std::numeric_limits< int >::max( )
         || v < std::numeric_limits< int >::min( ) )
        return false;

    result = v;
    return true;
}


/******************************************
 * Split a string at delimiters
 ******************************************/

std::vector< std::string >
split_string( std::string const & str,
              std::string const & delimiters )
{
    std::vector< std::string > comp;
    std::size_t start = 0;
    std::size_t pos;

    while ( ( pos = str.find_first_of( delimiters, start ) )
                                                        != std::string::npos )
    {
        comp.push_back( str.substr( start, pos - start ) );
        start = pos + 1;
    }


    if ( start <= str.size( ) - 1 )
        comp.push_back( str.substr( start ) );

    return comp;
}


/******************************************
 ******************************************/

static
std::vector< std::string >
split_abs_path( std::string const & name )
{
    std::string abs_path( name );

    // If we have relative path construct the absolute path, it's a bit messy

    if ( abs_path[ 0 ] != '/' )
    {
         long path_max = pathconf( "/", _PC_PATH_MAX );
        std::size_t size;

        char * prefix = 0;

        if ( path_max == -1 )
            size = 1024;

        while ( 1 )
        {
            if ( prefix )
                delete [ ] prefix;
            prefix = new char [ size ];
            if ( ! getcwd( prefix, size ) )
                size += 1024;
            else
                break;
        }

        abs_path = prefix + ( "/" + abs_path );

        delete [ ] prefix;
    }

    // The result may still contain stuff like "//", "/./" or "/../". To get
    // rid of split at each '/'

    std::vector< std::string > comp( split_string( abs_path, "/" ) );

    // Get rid of results of '//' and '/./'

    for ( std::size_t i = 0; i < comp.size( ); ++i )
        if ( comp[ i ].empty( ) || comp[ i ] == "." )
            comp.erase( comp.begin( ) + i-- );

    // Get rid of results of '/../' - they may even be at the start or bubble
    // up to it

    for ( int i = 0; i < static_cast< int >( comp.size( ) ); ++i )
        if ( i == 0 )
            while ( comp.front( ) == ".." )
                comp.erase( comp.begin( ) );
        else if ( comp[ i ] == ".." )
        {
            comp.erase( comp.begin( ) + i-- );
            comp.erase( comp.begin( ) + i-- );
        }

    return comp;
}


/******************************************
 ******************************************/

static
std::string
create_path( std::vector< std::string > const & comp )
{
    std::string path;

        // Just try to create a new directories. If this fails for any other
        // reason given than EEXIST we've got a problem. If also an EEXIST
        // failure is a bad sign it will be caught later on. No need for speed
        // here, just keep it simple, stupid!

    for ( std::vector< std::string >::const_iterator it = comp.begin( );
          it != comp.end( ) - 1; ++it )
    {
        errno = 0;
        if ( mkdir( ( path += "/" + *it ).c_str( ), 0777 ) && errno != EEXIST )
            return "";
    }

    return path + "/" + comp.back( );
}


/******************************************
 * Does everything needed for opening a file that nay beed creating:
 * expands to an absolute path and creates all directories needed. Returns
 * the absolute path of the file on success and an empty string on failure.
 ******************************************/

std::string
prepare_file_creation( std::string const & name )
{
    return create_path( split_abs_path( name ) );
}


} // namespace Utils

/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
