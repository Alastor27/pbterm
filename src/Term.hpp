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


#if ! defined TERM_HPP_
#define TERM_HPP_


#include <string>
#include <vector>


class Messenger;
class Config;
class Logger;


/******************************************
 * Class that takes care of starting the shell, sending it
 * commands and reading the replies
 ******************************************/

class Term
{
  public :

    Term( Messenger & mess,
          Config    & config );


    ~Term( );


    bool
    using_pty( );


    void
    send_command( std::string const & cmd );


    void
    send_control( char crtl );


    std::vector< std::string > const &
    command_list( ) const;


    std::string
    last_command( ) const;


  private :

    static void
    static_timer_handler( );


    void
    timer_handler( );


    bool
    send( char const * data,
          int          len );


    void
    read_cmd_file( );


    void
    save_command( std::string const & cmd );


    void
    write_cmd_file( );


    pid_t
    start_shell( std::string const & shell );


    pid_t
    start_piped_shell( std::string const & shell );


    int
    open_master_pty( std::string & slave_name );


    void
    setup_child( std::string const & slave_name,
                 std::string const & shell );


    ssize_t
    get_shell_output( std::string & reply );


    // Messenger object we have to notify about new shell output

    Messenger & m_mess;


    // Time (in ms) between checks for shell output

    int m_check_interval;


    // File descriptors for reading and writing to the child running the shell.
    // If a pseudoterminal can be used they are the same.

    int m_write_fd,
        m_read_fd;


    // List of all commands already send to the shell

    std::vector< std::string > m_commands;


    // Object for logging

    Logger & m_logger;


    // Maximum number of commands in history

    std::size_t m_max_history;


    // Name of file to read histiry from and write it to

    std::string m_cmd_file;


    // Callback handling terminal

    static Term * s_handling_term;
};

#endif


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
