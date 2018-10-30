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


#if ! defined LOGGER_HPP_
#define LOGGER_HPP_


#include "Utils.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <streambuf>


/******************************************
 * Helper class for logging
 ******************************************/

class LogBuffer : public std::streambuf
{
  public:

    LogBuffer( std::streambuf * sb )
        : m_sb( sb )
    { }


    void
    switch_stream( std::streambuf * new_sb );


  private:

    int
    overflow( int c );


    std::streambuf * m_sb;
};



/******************************************
 * Class for logging
 ******************************************/

class Logger : public std::ostream
{
  public :

    Logger( )
        : std::ostream( &m_buffer )
        , m_fstream( 0 )
        , m_buffer( m_sstream.rdbuf( ) )
    { }


    ~Logger( );


    bool
    set_file( std::string const & file_name );


    bool
    can_log( ) const  { return m_fstream != 0; }


    std::ostream &
    error( );


    std::ostream &
    warn( );


    std::ostream &
    info( );


  private :

    std::stringstream m_sstream;


    std::ofstream * m_fstream;


    LogBuffer m_buffer;
};


#endif


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
