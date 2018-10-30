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


#include "Logger.hpp"


/******************************************
 ******************************************/

void
LogBuffer::switch_stream( std::streambuf * new_sb )
{
    m_sb = new_sb;
}


/******************************************
 ******************************************/

int
LogBuffer::overflow( int c )
{
    // This buffer has no buffer, instead every character "overflows"
    // and can be put directly into whatever the streambuffer points to

    if ( c == EOF || ! m_sb )
        return c;

    int ret = m_sb->sputc( c );
    m_sb->pubsync( );
    return ret == EOF ? EOF : c;
}


/******************************************
 * Destructor - close the log file if open
 ******************************************/

Logger::~Logger( )
{
    if ( m_fstream )
    {
        m_fstream->close( );
        delete m_fstream;
    }
}


/******************************************
 * At creation of the Logger instance the name of the log file isn't known,
 * so it gets opened by calling this method
 ******************************************/

bool
Logger::set_file( std::string const & file_name )
{
    // Give up (and keep the old stream) if the argument is bogus, the path
    // to the new file can't be used or the file can't be opened

    if ( file_name.empty( ) || file_name[ file_name.size( ) - 1 ] == '/' )
        return false;

    std::string path( Utils::prepare_file_creation( file_name ) );
    if ( path.empty( ) )
        return false;

    std::ofstream * new_fstream = new std::ofstream( file_name.c_str( ) );
    if ( ! new_fstream->is_open( ) )
        return false;

    // If this is a switch from one file to another close the file we were
    // using up until now after copying it format flags to the new one.

    if ( m_fstream )
    {
        new_fstream->flags( m_fstream->flags( ) );
        m_fstream->close( );
        delete m_fstream;
    }

    // If we're switching away from the stringstream write its contents to
    // the new file (and copy its format flags)

    if ( ! m_fstream )
    {
        *new_fstream << m_sstream.rdbuf( );
        new_fstream->flush( );
        new_fstream->flags( m_sstream.flags( ) );
    }

    // Set the new file "handle" as what we're going to log to in the future
    // and make sure it's unbuffered

    m_fstream = new_fstream;
    m_fstream->rdbuf( )->pubsetbuf( 0, 0 );
    m_buffer.switch_stream( m_fstream->rdbuf( ) );

    return true;
}


/******************************************
 * Write out "ERROR: " and return the ostream for the text to be logged
 ******************************************/

std::ostream &
Logger::error( )
{
    *this << "ERROR: ";
    return *this;
}


/******************************************
 * Write out "WARN: " and return the ostream for the text to be logged
 ******************************************/

std::ostream &
Logger::warn( )
{
    *this << "WARN: ";
    return *this;
}


/******************************************
 * Write out "INFO: " and return the ostream for the text to be logged
 ******************************************/

std::ostream &
Logger::info( )
{
    *this << "INFO: ";
    return *this;
}


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
