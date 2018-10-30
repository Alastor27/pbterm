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


#include "Lines.hpp"
#include "Lines.hpp"
#include "Utils.hpp"


/***************************************
 * Adds new text to the lines. The text may contain embedded line-feeds,
 * which is used for splitting it into individual lines. If the resulting
 * set of lines requires more vertical space than is available on the
 * screen on redraw the newest line will always be shown.
 ***************************************/

void
Lines::add( std::string const & txt )
{
    // Split the input into lines at line feeds

    std::vector< std::string > lines = Utils::split_string( txt, "\n" );
    std::size_t line_count = lines.size( );

    // Add the new line, making sure that no more than a maximum number
    // of lines are kept.

    if ( line_count >= m_max_lines )
    {
        m_lines.clear( );
        for ( std::vector< std::string >::iterator it =
                                     lines.begin( ) + line_count - m_max_lines;
              it != lines.end( ); ++it )
            m_lines.push_back( Line( *it, this ) );
    }
    else
    {
        // The last time round something was added the last line may have
        // been missing a trailing newline. In this case append the first
        // line from the input to the last line.

        if ( m_lines.size( ) && m_is_unfinished_line )
        {
            m_lines[ m_lines.size( ) - 1 ].append( lines.front( ) );
            lines.erase( lines.begin( ) );
        }

        if ( lines.size( ) )
        {
            // If necessary remove existing lines to make room for the new
            // ones.

            std::size_t len = m_lines.size( ) + lines.size( );
            if ( len > m_max_lines )
                m_lines.erase( m_lines.begin( ),
                               m_lines.begin( ) + len - m_max_lines );

            for ( std::vector< std::string>::iterator it = lines.begin( );
                  it != lines.end( ); ++it )
                m_lines.push_back( Line( *it, this ) );
        }
    }

    // Check if the input ended in a line feed

    m_is_unfinished_line = txt[ txt.size( ) - 1 ] != '\n';

    // Let's see how long the complete new text is

    recalc_height( );
}


/***************************************
 * Function to inform the object about new screen dimensions (due to a
 * rotation). Induces a recalculation of all lines.
 ***************************************/

void
Lines::screen_dimensions_changed( )
{
    m_screen_width  = ScreenWidth(  ) - 2 * m_x_margin;
    m_screen_height = ScreenHeight( ) - 2 * m_y_margin;
    recalc( );
}


/***************************************
 * Function to inform the object about a new font size set.. Induces a
 * recalculation of all lines.
 ***************************************/

void
Lines::change_font_size( int new_font_size )
{
    m_font_size = new_font_size;
    recalc( );
}


/***************************************
 * Makes all lines recalculate themselves (might be due to a change of font
 * or the screen dimensions) and then determines the new total height.
 ***************************************/

void
Lines::recalc( )
{
    for ( std::vector< Line >::iterator it = m_lines.begin( );
          it != m_lines.end( ); ++it )
        it->recalc( );

    recalc_height( );
}


/***************************************
 * Computes the total height and recalculates the y-position
 ***************************************/

void
Lines::recalc_height( )
{
    int h = 0;

    for ( std::vector< Line >::iterator it = m_lines.begin( );
          it != m_lines.end( ); ++it )
        h += it->height( );
    
    m_height = h;

    if ( m_height > m_screen_height )
        m_y_position = m_height - m_screen_height;
    else
        m_y_position = 0;
}


/***************************************
 * Redraws everything unconditionally
 ***************************************/

void
Lines::redraw( ) const
{
    int h = - m_y_position;
    std::vector< Line >::const_iterator it = m_lines.begin( );

    // Skip lines that would appear above the top of the screen

    for ( ; it != m_lines.end( ) && h + it->height( ) < 0; ++it )
        h += it->height( );

    // Get all lines that are within the screen to redraw themselves

    for ( ; it != m_lines.end( ) && h < m_screen_height; ++it )
    {
        it->redraw( h + m_y_margin );
        h += it->height( );
    }     
}


/***************************************
 * Scrolls up or down by the given number of pixels (a positive number
 * moves the text downwards, a negative one upwards) a far as posible
 ***************************************/

void
Lines::shift( int shift )
{
    // If the screen isn't filled don't scroll

    if ( m_height <= m_screen_height )
        return;

    m_y_position -= shift;

    // Make sure we don't scroll beyond the start or end of the available
    // text

    if ( m_y_position < 0 )
        m_y_position = 0;
    else if ( m_y_position > m_height - m_screen_height )
        m_y_position = m_height - m_screen_height;
}


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
