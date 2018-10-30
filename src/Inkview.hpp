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


#if ! defined INKVIEW_HPP_
#define INKVIEW_HPP_


/******************************************
 * Include inkviw.h and define some stuff that's not in the include
 * file from the current SDK but seems to be implemented in the
 * versions of the libinkview distributed with newer devices (there
 * should be no harm if this additonal stuff isn't really available).
 ******************************************/

#include "inkview.h"


// Event to be evaluated during multi-touch guestures

#define EVT_MTSYNC  39


// Typedef for the signature of the GetMenuRect() function

typedef irect ( * GetMenuRect_t )( imenu const * );


// Structure returned by the GetTouchInfo() function

typedef struct iv_mtinfo_s {
    int active;
    int x;
    int y;
    int pressure;
    int rsv_1;
    int rsv_2;
} iv_mtinfo;


// Typedef for the signature of the GetTouchInfo() function

typedef iv_mtinfo * ( * GetTouchInfo_t )( void );


// ISPOINTEREVENT is missing two new types of events, so redefine it

#if defined ISPOINTEREVENT
#undef ISPOINTEREVENT
#endif

#define ISPOINTEREVENT( x )             \
   (    ( ( x ) >= 29 && ( x ) <= 31 )  \
     || ( ( x ) >= 34 && ( x ) <= 35 )  \
     || ( x ) == 39                     \
     || ( x ) == 44 )


// The T and TF macros in inkview.h are an abomination since their names are
// much too short not to clash with other stuff - e.g. many templates in C++
// use 'T'. Newer versions of inkview.h seem to have them commented out.

#if defined T
#undef T
#endif

#if defined TF
#undef TF
#endif


#endif


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
