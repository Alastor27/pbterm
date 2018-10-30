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


#if ! defined ROTATION_HANDLER_HPP_
#define ROTATION_HANDLER_HPP_


class Messenger;
class Config;


/***************************************
 * Class that keeps care of letting the user request a new orientation
 ***************************************/

class Rotation_Handler
{
  public :

    Rotation_Handler( Messenger & mess,
                      Config    & config );


    void
    show( );


  private :

    static Rotation_Handler * s_handling_rotation;


    static void
    static_rotation_handler( int dir );


    void
    rotation_handler( int dir );


    Messenger & m_mess;
};


#endif


 /*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
