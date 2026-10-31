# Changelog

All notable changes to MidiLayer are documented in this file.

## 1.1.0-alfa3 – Windows fork (Viktor318)

Based on the author's 2021 version 1.1.0-alfa3 from the SourceForge git repository.

- Windows build with Qt 6.12 (MinGW 64-bit).
- **Bugfix**: A box character appeared at the end of MIDI port names on Windows (the
  bundled RtMidi left the terminating null character at the end of the name).
- **Bugfix**: On first start (without a saved window position) the main window was
  placed off screen. The default window is now centered on the primary screen, and
  a saved position is used only if it is visible on one of the screens.
- Fusion widget style by default, because the Windows 11 style clipped the contents
  of the number fields (can be overridden with `-style <name>`).
- The donation button is replaced by a "Windows fork by Viktor318" link.
- `deploy.bat`: builds a portable Windows package and ZIP from the Release build.

## 1.1.0-alfa3 (original author, December 2021, unreleased)

- 4 virtual MIDI ports (Linux and macOS).
- **Bugfix**: MIDI Setup forgot its last state.
- **Bugfix**: Compiler warnings in the logging functions.
- Many visual improvements, reworked element panel.

## Version 1.0.0 (2016)

- First real version for Mac, distributed in .dmg package.
- Arrange preset tool bar.
- Adjust some modal windows sizes on Mac.
- Remove auto show/hide MIDI monitors (not practical).
- Remove "portable" word in window title.
- **Bugfix**: Resolve interface names when they have special chars.
- **Bugfix**: New bank save and rename actions fails.

## Version 0.4.3

- **Bugfix**: Preset disappears after drag and drop in preset list (Windows only).
- Run only one instance (lock file) and warn.
- Command line: remove lock file.
- Command line: help.
- Preset overview.

## Version 0.4.2

- Improve metronome tap algorithm.
- Custom Control Change to sub-volume.
- Elements tab with on/off and edited info.

## Version 0.4.1

- **Bugfix**: Program crashes after multiple MIDI setup usages.
- **Bugfix**: Remove already open port instances from setup (Linux ALSA driver).
- Include MIDI Thru port on Linux compilation (ALSA driver).
- MIDI monitor no longer a modal dialog.
- Change program change dial limits (was 0~127, now is 1~128).
- Use keys CTRL+M to close MIDI Monitor.
- Use shortcut keys to open/close MIDI dock monitors.

## Version 0.4.0

- **Bugfix**: Wrong channel on channel reset.
- **Bugfix**: Not possible to change MIDI slots after first selection.
- **Bugfix**: Remove an extra char at the end of interface name on Windows build.
- Optimization of preset change delay.
- Resolve problem for SysEx and program changes in some sound modules.
- Transpose range from -120 to 120 with out-of-bounds indication.
- Add a custom control change limit.
- Slot-not-defined information on GUI.
- MIDI channel monitor visual improvements.
- Improvements to the update notifier.
- Send metronome beats in UDP packets (works with the Android app).
- Some small improvements to the element GUI:
  - Add donate button.
  - Add splash screen timeout.
  - Change MidiLayer logo.
  - Change logo.
  - LEDs optimization.

## Version 0.3.1

- Correction on Windows distribution. Now runs on Windows 8.x.
- Optimization of logs.
- Reduce MIDI latency.
- Support Notes Off and Reset Controllers on leave elements.

## Version 0.3.0

- Use Qt 5.x.
- MIDI port selector recoded. Now more efficient.
- Ability to send UDP packets to a metronome client (Android/iPhone).
- Remove reset controllers on change preset.
- **Bugfix**: Bank change send order was inverted.
- Bank change range changed to 0-127. More compatible with equipment manuals.
- Send bank change on value change.
- Elements can be autosaved or not.
- Icons support dark OS themes.
- About dialog uses GUI theme colors.
- Many user interface improvements.

## Version 0.2.1.31

- **Bugfix**: Program change was sent on element off and dial change.
- **Bugfix**: Volume change was sent on element off and slider change.
- **Bugfix**: Send SysEx was sent on element off and press send button.
- Added menu for Show/Hide dock windows.
- Can receive program change from all interface slots.
- Can receive program change from all channels.
- Can send sequencer controller to all slots.

## Version 0.2.0.30

- Improvements in the propagation of MIDI information.
- MIDI Monitor with the possibility to show what is coming out.

## Version 0.1.0.28

- Keep in memory only one preset at once. Uses less memory (big issue).
- Added a sequencer controller dock widget.
- Redesigned metronome dock widget.
- Redesigned channel monitor widgets.
- Redesigned MIDI monitor dialog.
- Changed about dialog.
- Added preferences dialog.
- Can save window state.
- Defined new tab stop order.
- Defined new keyboard shortcuts.
- Added "confirm on exit" functionality.
- Prevents bank, preset and element names from unsupported filenames in OS.
- Restyled GUI.
- Corrected many small bugs.

## Version 0.0.1.6

- **Bugfix**: On bank load action, all elements were sending program change and volume.
- **Bugfix**: On preset change, program changes were not being sent.
- Added a small annotation on the image in the keyboard graphic, to represent the standard 60-key keyboard range.
- Metronome volume added to preset settings.
- Changed metronome setting in preset file.

## Version 0.0.1.0

- Initial version.
