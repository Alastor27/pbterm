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


#include "Line.hpp"
#include "Lines.hpp"
#include "Inkview.hpp"
#include <algorithm>


/***************************************
 ***************************************/

Line::Line( std::string const & txt,
            Lines const       * parent )
    : m_parent( parent )
{
    store_detabbed( txt );
}


/***************************************
 * Appends some text to the line
 ***************************************/

void
Line::append( std::string const & txt )
{
    store_detabbed( m_txt + txt );
}


/***************************************
 * Draws the line (which can, due to wrapping, be split in several lines)
 * at the given y-position. Returns the y-position for the next line to be
 * drawn.
 ***************************************/

int
Line::redraw( int y_position ) const
{
    std::size_t num_lines = m_break_pos.size( );
    std::size_t cnt = 1;
    std::size_t start = 0;

    for ( std::vector< std::size_t >::const_iterator it = m_break_pos.begin( );
          it != m_break_pos.end( ); ++it )
    {
        // Draw (part of the) line

        DrawString( m_parent->x_margin( ), y_position,
                    m_txt.substr( start, *it - start ).c_str( ) );

        // Draw a small rectangle at end of the line if it wraps (and there's
        // enough space)
 
        if ( num_lines && cnt++ != num_lines )
        {
            int z = m_parent->continuation_symbol_width( ) - 2;
            if ( z > 4 )
            {
                int lh = m_parent->font_size( ) + m_parent->line_spacing( );
                int x = m_parent->screen_width( ) + m_parent->x_margin( ) - z;
                int y = y_position + ( lh + z )/ 2;
                FillArea( x, y, z, z, BLACK );
            }
        }

        y_position += m_parent->font_size( ) + m_parent->line_spacing( );
        start = *it;
    }     

    return y_position;
}


/***************************************
 * Checks where a line is to be split too fit on the screen and calculates
 * the height it will thus require
 ***************************************/

void
Line::recalc( )
{
    recalc_break_pos( );
    m_height =   m_break_pos.size( )
               * ( m_parent->font_size( ) + m_parent->line_spacing( ) );
}


/***************************************
 * Calculates where a line needs to be wrapped if it's longer than fits
 * onto the screen
 ***************************************/

void
Line::recalc_break_pos( )
{
    int width = StringWidth( m_txt.c_str( ) );
    int available = m_parent->screen_width( );
    std::size_t end = m_txt.size( );

    m_break_pos.clear( );

    // Nothing more to be done if the line fits into the availabe screen
    // width

    if ( width <= available )
    {
        m_break_pos.push_back( end );
        return;
    }

    // Start a guessing game where we've got to wrap

    std::size_t start = 0;

    available -= m_parent->continuation_symbol_width( );

    while ( start < end )
    {
        std::size_t guess =
                        static_cast< int >( (   ( double ) available / width )
                                              * ( end - start ) );

        bool last_try_was_ok = true;

        int last_good_width = 0;

        while( 1 )
        {
            int new_width =
                          StringWidth( m_txt.substr( start, guess ).c_str( ) );

            if ( new_width > available )
            {
                // Doesn't fit - reduce the guess (and remember that we failed)

                guess--;
                last_try_was_ok = false;
            }
            else if ( new_width < available )
            {
                // It fits: if we're already at the end of the line or the last
                // guess didn't fit we're done, otherwise raise the guess

                if ( guess >= end - start )
                {
                    last_good_width = new_width;
                     break;
                }

                if ( ! last_try_was_ok )
                    break;

                guess++;
                last_good_width = new_width;
            }
            else
            {
                last_good_width = new_width;
                break;
            }
        }

        start = std::min< int >( start + guess, end );
        width -= last_good_width; 
        m_break_pos.push_back( start );
    }

    if ( start < end )
        m_break_pos.push_back( end );
}
         

/***************************************
 * Stores the text of the line after removing tab characters and recalcutates
 * all information needed to display it properly
 ***************************************/

void
Line::store_detabbed( std::string const & txt )
{
    m_txt.clear( );

    std::size_t start = 0,
                pos;

    while ( ( pos = txt.find_first_of( "\t", start ) ) != std::string::npos )
    {
        m_txt.append( txt, start, pos - start );
        m_txt.append(   m_parent->tab_width( )
                      - m_txt.size( ) % m_parent->tab_width( ), ' ' );
        start = pos + 1;
    }

    if ( start != txt.size( ) )
        m_txt.append( txt, start, std::string::npos );

    recalc( );
}


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
