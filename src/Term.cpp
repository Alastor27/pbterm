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


#include "Term.hpp"
#include "Messenger.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include "Defaults.hpp"
#include <cerrno>
#include <fstream>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>


// Definition of the static member used to find the Term instance from
// within static member functions

Term * Term::s_handling_term;


/******************************************
 * Constructor, starts the shell and tries to read in stored commands
 * from a previous session
 ******************************************/

Term::Term( Messenger & mess,
            Config    & config )
    : m_mess( mess )
    , m_check_interval( config.check_interval( ) )
    , m_write_fd( -1 )
    , m_read_fd( -1 )
    , m_logger( config.logger( ) )
    , m_max_history( config.max_history( ) )
    , m_cmd_file( config.cmd_file( ) )
{
    s_handling_term = this;

    // Start the shell, abort on any failures

    if ( start_shell( config.shell( ) ) <= 0 )
    {
        m_mess.send( message::Close( ) );
        return;
    }

    // Read in the file with the command history

    read_cmd_file( );

    // Start looking for messages from the shell

    timer_handler( );
}


/***************************************
 * Destructor, stores the command history and closes connections to shell,
 * this in turn will make it quit (it's going to receive a SIGPIPE)
 ***************************************/

Term::~Term( )
{
    if ( m_write_fd >= 0 )
    {
        close( m_write_fd );
        if ( m_write_fd != m_read_fd )
            close( m_read_fd );
        write_cmd_file( );
    }
}


/******************************************
 * Returns if communication with the shell uses a pseudoterminal
 ******************************************/

bool
Term::using_pty( )
{
    return m_write_fd == m_read_fd;
}


/******************************************
 * Stores a command at the end of the list of commands and erases earlier
 * entries of the same command. Keeps the list from getting longer than
 * is set by 'm_max_history'.
 ******************************************/

void
Term::save_command( std::string const & cmd )
{
    for ( std::size_t i = 0; i < m_commands.size( ); ++i )
        if ( cmd == m_commands[ i ] )
        {
            m_commands.erase( m_commands.begin( ) + i );
            break;
        }

    while ( m_commands.size( ) >= m_max_history )
        m_commands.erase( m_commands.begin( ) );

    m_commands.push_back( cmd );
}


/******************************************
 * Returns list of all commands in the "history"
 ******************************************/

std::vector< std::string > const &
Term::command_list( ) const
{
    return m_commands;
}


/******************************************
 * Returns the last command in the "history" (or the
 * empty string if there's nothing in the history)
 ******************************************/

std::string
Term::last_command( ) const
{
    return m_commands.empty( ) ? "" : m_commands.back( );
}


/******************************************
 * Sends a string to the shell and stores it in the history
 * (if sending the string did succeed)
 ******************************************/

void
Term::send_command( std::string const & cmd )
{
    if ( send( cmd.c_str( ), cmd.size( ) ) )
        save_command( cmd.substr( 0, cmd.size( ) - 1 ) );
}


/******************************************
 * Sends a single (control character) to the shell
 ******************************************/

void
Term::send_control( char ctrl )
{
    send( &ctrl, 1 );
}


/******************************************
 * Helper function that does the actual sending
 ******************************************/

bool
Term::send( char const * data,
            int          len )
{
    int cnt = 0;

    while ( ( len -= cnt ) > 0 )
    {
        cnt = write( m_write_fd, data, len );

        if ( cnt < 0 )
        {
            m_logger.error( ) << "write() to pty failed: "
                              << strerror( errno ) << std::endl;
            m_mess.send( message::Close( ) );
            return false;
        }

        data += cnt;
    }

    return true;
}


/***************************************
 * We need a static function since we must pass it to a C function, so
 * we have this "dummy" function that we can use with C and that just
 * redirects to the real function for dealing with the timer event.
 ***************************************/

void
Term::static_timer_handler( )
{
    s_handling_term->timer_handler( );
}


/***************************************
 * Handler for timer events, checks if the shell send any new data
 ***************************************/

void
Term::timer_handler( )
{
    // Ok, let's see if there's something to be obtained from the shell

    std::string txt;
    int retval = get_shell_output( txt );

    // A negative return value indicates that the shell closed its output
    // channels (probably because it stopped working) or due to some other
    // serious error. A return value of 0 is to be expected when the shell
    // didn't send anything.

    if ( retval == -1 )
    {
        m_mess.send( message::Close( ) );
        return;
    }
    else if ( retval > 0 )
    {
        // Pass on any new text to the display and, if recording is on,
        // write it to the log file

        m_mess.send( message::New_Text( txt ) );
    }

    // (Re)start the timer that gets us back here for more

    SetWeakTimer( APP_NAME "_timer", &Term::static_timer_handler,
                  m_check_interval );
}


/***************************************
 * Tries to read in the file with commands used in a previous session
 ***************************************/

void
Term::read_cmd_file( )
{
    // Try to open the commamd file for reading

    std::ifstream ifs( m_cmd_file.c_str( ), std::ifstream::in );

    std::string  line;

    // Keep reading until the end of the file is reached or as many
    // commands have been found as fit into the history

    while ( ifs.good( ) )
    {
        std::getline( ifs, line );

        if ( ifs.fail( ) )
            break;

        // Remove empty lines and comments (lines starting with a hash mark)

        if ( Utils::left_trim( line ).empty( ) || line[ 0 ] == '#' )
            continue;

        save_command( line );
    }
}


/***************************************
 * Tries to write out a file with all commands stored in this session
 ***************************************/

void
Term::write_cmd_file( )
{
    // Create all necessary drectories (if possible)

    std::string path( Utils::prepare_file_creation( m_cmd_file ) );

    if ( path.empty( ) )
        return;

    // Try to open the commandd file for writing

    std::ofstream ofs( path.c_str( ), std::ofstream::out );

    if ( ofs.bad( ) )
        return;

    // Write out all commands to the file

    for ( std::vector< std::string >::const_iterator it = m_commands.begin( );
          it != m_commands.end( ); ++it )
        ofs << *it << std::endl;
}


/******************************************                                     
 * Recreates a pseudoterminal and set it up and spawns the shell with all
 * standard file descriptors redirected to the slave part of the pseudo-
 * terminal.
 ******************************************/

pid_t
Term::start_shell( std::string const & shell )
{
    // Open the master side of the pseudterminal and get the name of the
    // slave part's file.

    std::string slave_name;

    if ( ( m_write_fd = m_read_fd = open_master_pty( slave_name ) ) == -1 )
        return -1;

    // A return value of -2 indicates that opening the pseudoterminal failed
    // due to permission issues, so let's try it with pipes instead

    if ( m_write_fd == -2 )
        return start_piped_shell( shell );

    // Now that we have the master and the slave name (or a set of pipes)
    // we can start the child process

    pid_t child_pid = fork( );
    switch ( child_pid )
    {
        case -1 :
            m_logger.error( ) << "fork() failed: "
                              << strerror( errno ) << std::endl;
            close( m_write_fd );
            m_write_fd = m_read_fd = -1;
            return -1;

        case 0 :                                 // Child
            close( m_write_fd );
            setup_child( slave_name, shell );
            exit( 1 );
    }

    return child_pid;
}


/******************************************                                     
 * Opens the master pseudoterminal, sets it up and 
 ******************************************/

int
Term::open_master_pty( std::string & slave_name )
{
    int fd;

    // Open the master part of the pseudoterminal. Set it to non-blocking
    // to avoid having to call select() or poll() each time we check for
    // new data from the shell.

    if ( ( fd = posix_openpt( O_RDWR | O_NOCTTY | O_NONBLOCK ) ) == -1 )
    {
        if ( errno == EACCES )
        {
            m_logger.info( ) <<  "posix_openpt() failed due to permission "
                             << "issues, trying to use pipes" << std::endl;
            return -2;
        }

        m_logger.error( ) <<  "posix_openpt() failed: "
                          << strerror( errno ) << std::endl;
        return -1;
    }

    // According to TLPI calling grantpt() actually is not becessary on Linux,
    // but we do i anyway;-)

    if ( grantpt( fd ) != 0 )
    {
        m_logger.error( ) << "grantpt() failed: "
                          << strerror( errno ) << std::endl;
        close( fd );
        return -1;
    }

    // unlockpt() must be called before opening the slave

    if ( unlockpt( fd ) != 0 )
    {
        m_logger.error( ) << "unlockpt() failed: "
                          << strerror( errno ) << std::endl;
        close( fd );
        return -1;
    }

    // Get the file name for the slave part

    char *sn;
    if ( ! ( sn = ptsname( fd ) ) )
    {
        m_logger.error( ) << "ptsname() failed: "
                          << strerror( errno ) << std::endl;
        close( fd );
        return -1;
    }

    slave_name = sn;

    // Switch off echoing, otherwise we would read back everything we're
    // sending to the pseudoterminal. And we don't want a carriage added
    // to each newline we receive.

    struct termios tp;

    if ( tcgetattr( fd, &tp ) == -1 )
    {
        m_logger.error( ) << "tcgetattr() failed: "
                          << strerror( errno ) << std::endl;
        close( fd );
        return -1;
    }

    tp.c_lflag &= ~ ECHO;
    tp.c_oflag &= ~ ONLCR;

    if ( tcsetattr( fd, TCSANOW, &tp ) == -1 )
    {
        m_logger.error( ) << "tcsetattr() failed: "
                          << strerror( errno ) << std::endl;
        close( fd );
        return -1;
    }

    return fd;
}


/******************************************                                     
 * Set up the slave pseudoterminal, redirect all standard file descriptors
 * to it, close all other files and finally replace the child by the shell.
 ******************************************/

void
Term::setup_child( std::string const & slave_name,
                   std::string const & shell )
{
    // Run child in a new session, making it the session leader

    if ( setsid( ) < 0 )
    {
        m_logger.error( ) << "setsid failed: "
                          << strerror( errno ) << std::endl;
        return;
    }

    // Open the slave

    int slave_fd = open( slave_name.c_str( ), O_RDWR );
    if ( slave_fd == -1 )
    {
        m_logger.error( ) << "open() for slave failed: "
                          << strerror( errno ) << std::endl;
        return;
    }

    // According to TLPI this is necessary on BSD to acquire a controlling
    // terminal

#ifdef TIOCSCTTY
    if ( ioctl( slave_fd, TIOCSCTTY, ( char * ) 0 ) == -1 )
    {
        close( slave_fd );
        m_logger.error( ) << "ioctl() with TIOCSCTTY failed: "
                          << strerror( errno ) << std::endl;
        return;
    }
#endif

    // Redirect the three standard file descritors to the slave

    if (   dup2( slave_fd, STDIN_FILENO  ) != STDIN_FILENO
        || dup2( slave_fd, STDOUT_FILENO ) != STDOUT_FILENO
        || dup2( slave_fd, STDERR_FILENO ) != STDERR_FILENO )
    {
        close( slave_fd );
        m_logger.error( ) << "dup2() failed: "
                          << strerror( errno ) << std::endl;
        return;
    }

    // Close all files above stderr

    for ( int i = STDERR_FILENO + 1; i < getdtablesize( ); i++ )
        close( i );

    // Finally replace the process by the shell

    execlp( shell.c_str( ), shell.c_str( ), "-i", ( char * ) 0 );
}


/******************************************                                     
 * Methd invoked when opening a pseudoterminal failed due to missing
 * permissions to open the master file. In this case the communication
 * with the shell must be done via a set of pipes.
 ******************************************/

pid_t
Term::start_piped_shell( std::string const & shell )
{
    int fdes[ 2 ];

    // Create pipe set for sending data from the child to the parent

    if ( pipe( fdes ) < 0 )
    {
        m_logger.error( ) << "Failed to create pipe set: "
                          << strerror( errno ) << std::endl;
        return -1;
    }

    m_read_fd = fdes[ 0 ];
    int child_out = fdes[ 1 ];

    // Create another pipe set for sending data from the parent to the child

    if ( pipe( fdes ) < 0 )
    {
        m_logger.error( ) <<  "Failed to create second pipe set: "
                          << strerror( errno ) << std::endl;
        return -1;
    }

    int child_in = fdes[ 0 ];
    m_write_fd = fdes[ 1 ];

    pid_t child_pid;

    if ( ( child_pid = fork( ) ) < 0 )
    {
        m_logger.error( ) << "Can't fork: " << strerror( errno ) << std::endl;
        return -1;
    }
    else if ( child_pid == 0 )
    {
        // Run the child in its own session

        if ( setsid( ) < 0 )
        {
            m_logger.error( ) << "Failed to set process group: "
                              << strerror( errno ) << std::endl;
            return -1;
        }

        // Redirct childs standard channels to the pipes

        if (    dup2( child_in,  STDIN_FILENO  )  != STDIN_FILENO
             || dup2( child_out, STDOUT_FILENO  ) != STDOUT_FILENO
             || dup2( child_out, STDERR_FILENO  ) != STDERR_FILENO )
        {
            m_logger.error( ) << "dup2() failed: "
                              << strerror( errno ) << std::endl;
            exit( 1 );
        }

        // Close all other open files

        for ( int i = STDERR_FILENO + 1; i < getdtablesize( ); i++ )
            close( i );

        // Start the shell, replacing the child process

        execlp( shell.c_str( ), shell.c_str( ), "-i", NULL );
        exit( 1 );
    }

    // Close the child side pipe file handles

    close( child_in );
    close( child_out );

    // Set the non-blocking flag on the pipe end we're going to read from

    int flags = fcntl( m_read_fd, F_GETFL );

    if (    flags == -1
         || fcntl( m_read_fd, F_SETFL, flags | O_NONBLOCK ) == -1 )
    {
        m_logger.error( )  << "Failed to unblock read end of pipe: "
                           << strerror( errno ) << std::endl;
        return -1;
    }

    return child_pid;
}


/******************************************                                     
 * Tries to read as many data as are available from the shell
 ******************************************/

ssize_t
Term::get_shell_output( std::string & reply )
{
    static int const buffer_len = 1024;
    ssize_t cnt = 0,
            retval;

    // Keep reading until we get less than a full buffers worth of data

    do
    {
        char tmp_buffer[ buffer_len + 1 ];

        errno = 0;
        retval = read( m_read_fd, tmp_buffer, buffer_len );

        // Since the PTY is in non-blocking mode failure with EAGAIN is ok,
        // it just means that no data are available at the moment while a
        // zero return value means that the shell closed its output (and
        // that means its dead for all our purposes)

        if ( retval <= 0 )
            return errno == EAGAIN || errno == EWOULDBLOCK ? cnt : -1;

        tmp_buffer[ retval ] = '\0';
        reply += tmp_buffer;
        cnt   += retval;
    } while ( retval == buffer_len );

    return cnt;
}


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
