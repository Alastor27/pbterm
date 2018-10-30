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


#if ! defined DISPLAY_HPP_
#define DISPLAY_HPP_


#include <string>
#include <vector>
#include "Lines.hpp"
#include "Inkview.hpp"


class Config;
class Messenger;
class Logger;


/******************************************
 * Class that takes care of what happens on the display
 ******************************************/

class Display
{
  public :

    Display( Messenger & mess,
             Config    & config );


    ~Display( );


    void
    redraw( );


    void
    add_text( std::string const & str );


    void
    shift( int amount );


    void
    rotate( int dir );


    void
    change_font_size( int incr );

    void
    suspend_redraws( bool stop );


    void
    recording_state_change( bool state );


  private :

    // Name of the font to use

    std::string m_font_name;


    // Size of that font

    int m_font_size;


    // Cyurrent orientation

    int m_orientation;


    // Original orientation

    int m_initial_orientation;


    // Current width of display

    int m_width;


    // Font currently in use

    ifont * m_font;


    // Object containing all lines

    Lines m_lines;


    // Flag, set when no redraws are to done

    bool m_is_output_suspended;


    // Flag, set when during suspended a redraws a redraw is requested

    bool m_is_redraw_needed;


    // Flag, set while recording is switched on

    bool m_is_recording;
};


#endif


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
