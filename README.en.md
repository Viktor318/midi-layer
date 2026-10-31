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

## Build

This is a Qt-based C++ application (see `midiLayer.pro`), using the [RtMidi](https://www.music.mcgill.ca/~gary/rtmidi/) library for MIDI communication (bundled in the `RtMidi` folder).

Requirements:
- Qt (Qt Widgets, based on the `.pro`/`.ui` files)
- A C++ compiler (MSVC/MinGW on Windows, Clang/GCC on macOS/Linux)
- RtMidi (included in this repository)

```bash
qmake midiLayer.pro
make
```

(On Windows, opening and building the project in Qt Creator also works.)

## Origin

This repository was imported from the project's original SourceForge SVN source:
https://sourceforge.net/projects/midi-layer/

The imported source is the original author's version 1.0.0; the author marked
SVN revision 89 for the Windows and Mac builds (`midiLayer.pro`). The original
development has not continued since 2016, so this fork is not automatically
synchronized with the SourceForge repository. Should any new change appear there,
it can be merged manually.

## License

GNU General Public License v3.0 (GPLv3). See the `LICENSE` file.
