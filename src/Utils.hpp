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


#if ! defined UTILS_HPP_
#define UTILS_HPP_

#include <string>
#include <vector>


/******************************************
 * Collection of utility function used within the prgram
 ******************************************/

namespace Utils {

// Removes leading white-spaces from a string, returning a new string

std::string
left_trim( std::string const & str );


// Removes leading white-spaces from a string, modifing string

std::string &
left_trim( std::string & str );


// Removes leading white-spaces from a string, modifing string

std::string &
right_trim( std::string & str );


// Removes trailing white-spaces from a string, modifing string

std::string &
right_trim( std::string & str );


// Remove leading and trailing white-spaces from a string, returning a new
// string

std::string
trim( std::string const & str );


// Remove leading and trailing white-spaces from a string, modifying the string

std::string &
trim( std::string & str );


// Convert std::string to int (if possible)

bool
to_int( int               & result,
        std::string const & s,
        int                 base = 10 );


// Split a string at delimiters

std::vector< std::string >
split_string( std::string const & str,
              std::string const & delimiters );


std::string
prepare_file_creation( std::string const & name );

}


#endif


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
