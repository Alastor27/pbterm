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


#if ! defined SUBMENU_HPP_
#define SUBMENU_HPP_


#include <vector>
#include "Inkview.hpp"


/***************************************
 * Class for a creating and dealing with submenus.
 ***************************************/

class Submenu
{
  public :

    // Enumeration for direction the submenu will be shown, i.e. last
    // inserted entry first or last

    enum Direction
    {
        Normal,
        Reverse
    };
        

    Submenu( int start_index,
             int direction = Normal );


    void
    add( short int    type,
         const char * text );


    int
    size( ) const  { return m_entries.size( ) - 1; }


    bool
    has( int index ) const ;


    int
    position( int index ) const;


    imenu const &
    at( int index ) const;


    imenu &
    at( int index );


    imenu const &
    operator [ ] ( int index ) const  { return at( index ); }


    imenu &
    operator [ ] ( int index )  { return at( index ); }


    imenu *
    addr( );


    int
    next_free_index( ) const  { return m_start_index + size( ); }


  private :

    std::vector< imenu > m_entries;


    int m_start_index;


    int m_direction;
};


#endif


 /*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
