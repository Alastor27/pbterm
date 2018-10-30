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


#include "Display.hpp"
#include "Messenger.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include "Defaults.hpp"


/******************************************
 * Constructor
 ******************************************/

Display::Display( Messenger & mess,
                  Config    & config )
    : m_font_name( config.font_name( ) )
    , m_font_size( config.default_font_size( ) )
    , m_orientation( GetOrientation( ) )
    , m_initial_orientation( m_orientation )
    , m_width( ScreenWidth( ) )
    , m_font( 0 )
    , m_lines( m_font_size, config.line_spacing( ), config.tab_width( ),
               X_MARGIN, Y_MARGIN, config.max_lines( ) )
    , m_is_output_suspended( false )
    , m_is_redraw_needed( false )
    , m_is_recording( false )
{
    OpenScreen( );

    if ( ! ( m_font = OpenFont( m_font_name.c_str( ), m_font_size, 1 ) ) )
    {
        config.logger( ).error( ) << "Can't open font '" << m_font_name
                                  << "' with size of " << m_font_size
                                  << std::endl;
        mess.send( message::Close( ) );
        return;
    }

    // Set orientation only after font has been set, the font is needed in
    // the rotate() method

    rotate( config.default_orientation( ) );
}


/******************************************
 * Destructor, switches back to original orientation and "frees" font
 ******************************************/

Display::~Display( )
{
    if ( m_orientation != m_initial_orientation )
        SetOrientation( m_initial_orientation );

    if ( m_font )
        CloseFont( m_font );
}

    
/******************************************
 * Redraws the display
 ******************************************/

void
Display::redraw( )
{
    // While output is suspended just record that a redraw will be necessary
    // when redrawing becomes possible again

    if ( m_is_output_suspended )
    {
        m_is_redraw_needed = true;
        return;
    }

    ClearScreen( );
    SetFont( m_font, BLACK );

    // Get all (visible) lines to redraw themselves

    m_lines.redraw( );

    // Draw a little triangle in the upper right hand corner while recording
    // is switched on

    if ( m_is_recording )
    {
        int size   = 20,
            offset = 10;

        int x  = m_width - offset - size,
            y1 = offset,
            y2 = offset + size;

        while ( y1 < y2 )
        {
            DrawLine( x, y1,   x, y2, BLACK );
            x++;
            DrawLine( x, y1++, x, y2--, BLACK );
            x++;
        }
    }

    SoftUpdate( );
}


/******************************************
 * Adds text to be shown on the display
 ******************************************/

void
Display::add_text( std::string const & str )
{
    // Note: need to set font here first, it may have been chenged behind our
    // back which would screw up the calculations done for the widths of the
    // newly added lines

    SetFont( m_font, BLACK );
    m_lines.add( str );
    Repaint( );
}


/***************************************
 * Called when the user makes a swipe gesture to scroll up or down
 ***************************************/

void
Display::shift( int amount )
{
    m_lines.shift( amount );
    Repaint( );
}
    

/***************************************
 * Called when the user asks for a different orientation
 ***************************************/

void
Display::rotate( int dir )
{
    if ( dir == m_orientation )
        return;

    SetOrientation( m_orientation = dir );

    m_width = ScreenWidth( );

    SetFont( m_font, BLACK );

    m_lines.screen_dimensions_changed( );

    Repaint( );
}


/***************************************
 * Called when the user requests a larger or smaller font size (argument
 * is the difference to the current font size)
 ***************************************/

void
Display::change_font_size( int incr )
{
    int new_font_size  = m_font_size + incr;

    // Put some rstrictions on the minimum and maximum font size

    if ( new_font_size < MIN_FONT_SIZE )
        new_font_size = MIN_FONT_SIZE;

    if ( new_font_size > MAX_FONT_SIZE )
        new_font_size = MAX_FONT_SIZE;

    if ( new_font_size == m_font_size )
        return;

    ifont *new_font = OpenFont( m_font_name.c_str( ), new_font_size, 1 );

    if ( ! new_font )
        return;

    CloseFont( m_font );

    m_font = new_font;
    m_font_size = new_font_size;

    SetFont( m_font, BLACK );
    m_lines.change_font_size( m_font_size );
    Repaint( );
}


/******************************************
 * Called when the user switches recording on or off
 ******************************************/

void
Display::recording_state_change( bool state )
{
    m_is_recording = state;
    redraw( );
}


/******************************************
 * In some situations, e.g. when the menu or keyboard is shown, screen
 * updates need to be suspended. This happens when this method is called
 * with a 'true' value. When it's later called again with a 'false' value
 * updates get re-enabled and, if there were attempts to update in between,
 * an immediate update is triggered.
 ******************************************/

void
Display::suspend_redraws( bool stop )
{
    if (    stop != m_is_output_suspended
         && ! ( m_is_output_suspended = stop )
         && m_is_redraw_needed )
    {
        m_is_redraw_needed = false;
        Repaint( );
    }
}


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
