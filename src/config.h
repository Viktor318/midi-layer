/*
    MidiLayer
    Copyright (C) 2012-2022, António José Soares Maia <maia.ajs@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONFIG_H
#define CONFIG_H

#define APP_NAME                    "MidiLayer"
#define APP_ORGANIZATION            "aMaia"

#define APP_SETTINGS_FILE           "midiLayer.ini"
#define APP_DATA_DIR                "data"
#define APP_TRASH_DIR               "data/trash"
#define APP_DATA_BANK_EXTENSION     ".mlb"
#define APP_DATA_PRESET_EXTENSION   ".mlp"
#define APP_DATA_ELEMENT_EXTENSION  ".mle"
#define APP_MIDI_INTERFACE_NAME     "MidiLayer"
#define APP_LOCK_FILE_NAME          "MidiLayer.pid"
#define APP_OBJECT_NAME_SEPARATOR   "|"

#define APP_FONTS_DIR               "fonts"

#define STR_VIRTUAL_IN              "MidiLayer_VOut"
#define STR_VIRTUAL_OUT             "MidiLayer_VIn"

//#define NO_CHANNEL_MONITOR

#define MIDI_SLOTS                  8
#define VIRTUAL_PORTS               4
#define SEND_UDP_METRONOME

#define _LED_COLOR_OFF_     "#CCCCCC"
#define _LED_COLOR_IN_      "#FFFF00"
#define _LED_COLOR_ACCEPT_  "#00FF00"
#define _LED_COLOR_REJECT_  "#FF0000"

// PLEASE DO NOT CHANGE NEXT LINE
#define DONATE_URL                  "https://www.paypal.com/bt/webapps/mpp/send-money-online"

#endif // CONFIG_H

