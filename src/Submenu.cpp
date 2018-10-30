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


/***************************************
 * Note: the whole esubmenu stuff only will work safely if my assumption
 * that, while the 'text' member of the 'imenu' structure is declared as
 * 'char *', the OpenMenu() function won't modify the strings it gets passed.
 * I.e. that the declaration of the the 'text' member as 'char *' is an
 * oversight and should have been 'char const *' insteas (thus the need for
 * the awkward 'const_cast< char *>' cast). Otherwise we're in trouble and
 * would have to obtain and maintain memory for each of the menu item strings
 * somewhere...
 ***************************************/


#include "Submenu.hpp"


/***************************************
 * Constructor, expects the first libinkiew index to be used with the first
 * added entry and the direction it is to be shown (i.e. if the submenu
 * entries are to be shown in order of adding them or in the reverse order,
 * i.e. last added first)
 ***************************************/

Submenu::Submenu( int start_index,
                  int direction )
    : m_start_index( start_index )
    , m_direction( direction )
{
    // Put the sentinel element into the submenu, everything else will
    // inserted before it

    imenu tmp = { 0, 0, 0, 0 };
    m_entries.push_back( tmp );
}


/***************************************
 * Adds a new entry to the submenu, all we need to know is the type
 * and the text, the index is calculated automatically (but that
 * requires that this method isn't called intermixed with calls
 * of the method for other submenus!)
 ***************************************/

void
Submenu::add( short int    type,
              char const * text )
{
    imenu tmp = { type,
                  m_start_index + m_entries.size( ) - 1,
                  const_cast< char * >( text ),
                  0 };

    // If the menu is constructed "normally" insert the new entry just
    // before the sentinel entry at the end, otherwise (if it is con-
    // structed in reverse order) at the very start.

    if ( m_direction == Normal )
        m_entries.insert( m_entries.begin( ) + m_entries.size( ) - 1, tmp );
    else
        m_entries.insert( m_entries.begin( ), tmp );
}


/***************************************
 * Returns if a certain index (as returned by libinkview to the
 * handler function) exists in the submenu (assumes that they
 * are consecutive)
 ***************************************/

bool
Submenu::has( int index ) const
{
    return index >= m_start_index && index < next_free_index( );
}


/***************************************
 * Returns the position index in the submenu from the index
 * returned by libinkview to the handler function
 ***************************************/

int
Submenu::position( int index ) const
{
    if ( m_direction == Normal )
        return index - m_start_index;
    else
        return ( size( ) - 1 ) - ( index - m_start_index );
}


/***************************************
 * Returns the imenu at  the index returned by libinkview to the
 * handler function
 ***************************************/

imenu const &
Submenu::at( int index ) const
{
    return m_entries[ position( index ) ];
}


/***************************************
 * Returns the imenu at  the index returned by libinkview to the
 * handler function
 ***************************************/

imenu &
Submenu::at( int index )
{
    return m_entries[ position( index ) ];
}


/***************************************
 * Returns the address of the array of submenut entries (or, to be
 * precise, the address of the first entry) as it is required by the
 * linkview library for the 'submenu' member of the imenu structure.
 ***************************************/

imenu *
Submenu::addr( )
{
    return &m_entries.front( );
}


 /*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
