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


#if ! defined LINE_HPP_
#define LINE_HPP_


#include <string>
#include <vector>


class Lines;


/***************************************
 * Class for dealing with a single line to be shown on the display -
 * stores the data and knows how to display them
 ***************************************/

class Line
{
  public :

    Line( std::string const & txt,
          Lines const       * parent );


    // Appends more text to the end of the line

    void
    append( std::string const & txt );


    // Redraws the line at a given y-position

    int
    redraw( int y_offset ) const;


    // Recalculates how the line is to be displayed when any display
    // settings change (i.e. font size or orientation)

    void
    recalc( );


    // Returns the height the line needs on the screen

    int
    height( ) const  { return m_height; }


  private :

    // Calculates at which positions in the line wrapping is needed

    void
    recalc_break_pos( );


    // Expand tabs and store the data of the line

    void
    store_detabbed( std::string const & txt );


    // Complete height needed for drawing the line

    int m_height;


    // The Lines class instance the line belongs to

    Lines const * m_parent;


    // Text of the line

    std::string m_txt;


    // Vector of indices into the lines text where wrapping must be done

    std::vector< std::size_t > m_break_pos;
};


#endif

/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
