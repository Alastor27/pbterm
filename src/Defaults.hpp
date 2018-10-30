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


#if ! defined DEFAULTS_HPP_
#define DEFAULTS_HPP_


/******************************************
 * Default settings for all kinds of properties of the program
 ******************************************/


// Name used for this program (controls names of configuration and log file
// as well as selection of timer name. While there is no documented
// restriction on the length of the timer name the declaration of the
// itimer structure in inkview.h makes it look as if the name can't
// be longer than 16 character. So to stay on the safe side don't use
// an application name with more than 8 characters).

#define APP_NAME  "pbterm"


// Default screen orientation

#define DEFAULT_ORIENTATION  0


// Default time (in ms) between checks for output from shell

#define DEFAULT_CHECK_INTERVAL  100


// Default font size

#define DEFAULT_FONT_SIZE  24


// Step size to increase or decrease font size

#define FONT_STEP  1


// Minimum and maximum font size that can be used

#define MIN_FONT_SIZE   6
#define MAX_FONT_SIZE  72


// Additional spacing betwen lines (in pixel

#define LINE_SPACING 0


// Number of spaces a tab is expanded to

#define TAB_WIDTH  4


// Name of the shell to be used

#define SHELL_PATH  "/bin/ash"


// Name of the configuration file

#define CONFIG_FILE  "/mnt/ext1/system/config/" APP_NAME ".cfg"


// Default name of log file

#define DEFAULT_LOG_FILE  "/mnt/ext1/system/share/" APP_NAME "/" APP_NAME ".log"


// Default name of command file

#define DEFAULT_CMD_FILE  "/mnt/ext1/system/share/" APP_NAME "/" APP_NAME ".cmd"


// Maximum length of command a user may enter (including trailing '\0')

#define MAX_CMD_LEN      256


// Maximum length of history of commands

#define DEFAULT_MAX_CMD_HISTORY  50


// Maximum number of display lines to keep for scrolling

#define DEFAULT_MAX_DISPLAY_LINES  1024


// Radius (in pixel) the pointer must stay in to be recognized as a tap
// and not as a swipe guesture

#define MAX_EXCURSION     15


// Threshold of change in finget distance change in two-finger guestures
// for changing the fnt size by one 'font_step'

#define PINCH_DISTANCE  100


// Size of (square) on-screen button regions

#define ON_SCREEN_BUTTON_SIZE 75


// Width reserved for symbol at end of wrapped lines

#define CONTINUATION_SYMBOL_WIDTH  10


// x- and y-margins of display

#define X_MARGIN  10
#define Y_MARGIN  10


// Default name of the custom keyboard layout file 

#define KEYBOARD_FILE "/mnt/ext1/system/share/pbterm/pbterm.kbd"


// File to look for that contain commands put there by the user

#define USER_CMD_FILE "/mnt/ext1/system/share/pbterm/user.cmd"


#endif


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
