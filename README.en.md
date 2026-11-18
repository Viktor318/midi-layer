# MidiLayer

MidiLayer is a simple and easy to use application designed to enhance your live performances.

Manage slots and channels, adjust the velocity and the volume, customize the note range and connect with modulation wheels and other types of controllers, all with the help of MidiLayer.

## Features

- Up to 8 MIDI interface slots
- Easy MIDI merge and MIDI thru
- MIDI routing by interface, channel or keyboard region
- Keyboard split up to 64 parts with 1 channel source
- MIDI in and out monitor utility
- Send program changes for all equipment
- Visual click utility by preset
- Event filter by keyboard region
- Drag and drop preset organizer
- Send SysEx by element
- Sequencer controller
- Easy to manage presets and elements in file explorer

## Download and use (Windows)

The ready-made Windows package can be downloaded from the GitHub **Releases** page
(`MidiLayer-<version>-win64.zip`).

1. Extract it to a folder the user can write to (e.g. `C:\MidiLayer` or under
   Documents) – **not** under `Program Files`.
2. Start `MidiLayer.exe`.
3. Assign the MIDI devices to the slots in the *MIDI Setup* window.

The program runs in portable mode: banks, presets and settings are kept in the
`data` folder and the `midiLayer.ini` file next to the program, so the whole folder
can be copied to another computer. No installation is needed.

## Build

This is a Qt-based C++ application (see `src/midiLayer.pro`), using the [RtMidi](https://www.music.mcgill.ca/~gary/rtmidi/) library for MIDI communication (bundled in the `src/RtMidi` folder). The runtime data (demo bank, fonts, default settings) is in the `dist` folder.

Requirements:
- Qt 6 (Widgets, Network, Xml); on Windows tested with Qt 6.12 MinGW 64-bit
- A C++ compiler (MSVC/MinGW on Windows, Clang/GCC on macOS/Linux)
- RtMidi (included in this repository)

```bash
cd src
qmake midiLayer.pro
make
```

On Windows, opening and building `src/midiLayer.pro` in Qt Creator also works. From the Release build, `deploy.bat` in the root folder creates a runnable, portable package (plus a ZIP) in the `deploy` folder.

## Origin

The original project: https://sourceforge.net/projects/midi-layer/
(author: António José Soares Maia)

The project has two source repositories on SourceForge:

- **SVN** (`code`): development from 2012 to 2016, last revision r89
  (version 1.0.0, 2016). In this fork the first Qt 6 port based on it is kept
  under the `qt6-windows-1.0.0` tag (no longer developed).
- **Git** (`repo`): the author continued development here in December 2021, up to
  version **1.1.0-alfa3** (reworked for Qt 6, 4 virtual MIDI ports, MIDI Setup
  state kept, many visual improvements). In this fork the `upstream-2021` branch
  is an unmodified copy of it, and the `qt6-windows-1.1` branch builds on it with
  the Windows fixes.

The bundled RtMidi does not support virtual MIDI ports on Windows, so that feature
works only on Linux and macOS.

Should a new change appear on SourceForge, after `git fetch sourceforge` the
`upstream-2021` branch can be moved to `sourceforge/master`, and the changes merged
manually from there.

## License

GNU General Public License v3.0 (GPLv3). See the `LICENSE` file.
