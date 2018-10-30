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


#if ! defined LINES_HPP_
#define LINES_HPP_


#include <string>
#include <vector>
#include "Line.hpp"
#include "Defaults.hpp"
#include "Inkview.hpp"


/***************************************
 * Class for storing all lines and drawing them.
 ***************************************/

class Lines
{
  public :

    // Constructor

    Lines( int font_size,
           int line_spacing,
           int tab_width,
           int x_margin,
           int y_margin,
           int max_lines ) 
        : m_screen_width( ScreenWidth( )   - 2 * x_margin )
        , m_screen_height( ScreenHeight( ) - 2 * y_margin )
        , m_font_size( font_size )
        , m_line_spacing( line_spacing )
        , m_tab_width( tab_width )
        , m_x_margin( x_margin )
        , m_y_margin( y_margin )
        , m_continuation_symbol_width( CONTINUATION_SYMBOL_WIDTH )
        , m_y_position( 0 )
        , m_max_lines( max_lines )
        , m_is_unfinished_line( false )

    { }


    // Adds a line (may contain embedded line breaks)

    void
    add( std::string const & txt );


    // Informs the object about new sceen dimensions

    void
    screen_dimensions_changed(  );


    // Informs the object that the font size has changed

    void
    change_font_size( int new_font_size );


    // Redraws everthing

    void
    redraw( ) const;


    // Scroll a number of pixels up or down

    void
    shift( int shift );


    // Returns the current screen width (minus padding at the sides)

    int
    screen_width( ) const  { return m_screen_width; }


    // Returns the current screen height (minus padding at top and bottom)

    int
    screen_height( ) const  { return m_screen_height; }


    // Returns the current font size

    int
    font_size( ) const  { return m_font_size; }


    // Returns the extra spacing between line

    int
    line_spacing( ) const  { return m_line_spacing; }


    // Returns into how many spaces a tab is to be expanded

    int
    tab_width( ) const  { return m_tab_width; }


    // Returns the x-margin to be used when drawing lines

    int
    x_margin( ) const  { return m_x_margin; }


    // Returns the y-margin to be used at the top of the screen

    int
    y_margin( ) const  { return m_y_margin; }


    // Returns the width of the "continuation symbol" drawn when lines must
    // be wrapped

    int
    continuation_symbol_width( ) const  { return m_continuation_symbol_width; }


    // Return the total height of all lines

    int
    height( ) const  { return m_height; }


  private :

    // Recalculates all lines and the resulting height

    void
    recalc( );


    // Recalculates the total height (and adjust y-position)

    void
    recalc_height( );


    // Screen width (reduced by x-margins)

    int m_screen_width;


    // Screen height (reduced by y-nargins

    int m_screen_height;


    // Font size

    int m_font_size;


    // Line spacing

    int m_line_spacing;


    // Into how many spaces a tab is expanded

    int m_tab_width;


    // x-margin

    int m_x_margin;


    // y_margin

    int m_y_margin;


    // Width of the "continuation symbol"

    int m_continuation_symbol_width;


    // Total height

    int m_height;


    // List of lines

    std::vector< Line > m_lines;


    // Distance (in pixel) of the upper border of the screen from the
    // the top of the lines (thus 0 means the very first line is shown
    // completely and the maximum value is the total height of all lines
    // minus the height of the screen)

    int m_y_position;


    // Maximum number of lines to keep in memory for scrolling

    std::size_t m_max_lines;


    // Flag, set when the last line added didn't end in a line-feed

    bool m_is_unfinished_line;
};


#endif


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
