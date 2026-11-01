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
- **Bugfix**: Stuck notes after a preset change. The key release (Note Off) used to pass
  through the elements of the *new* preset, and if the note was outside their range, or
  the transpose or output channel differed, the note off was dropped or went to the
  wrong place. The program now records where each key sounded a note, and sends the
  release exactly there.
- **Bugfix**: Stuck sustain pedal after a preset change. The pedal (CC64) release is now
  sent to every channel the pedal press was sent to.
- **Bugfix**: Stuck pitch bend, modulation (CC1), breath (CC2) and channel aftertouch
  after a preset change. If the wheel was moved during the change, the old channel stayed
  detuned or modulated. Now, when the wheel (or pressure) returns to rest, the rest
  value is sent to every channel that previously received a different value.
- **Bugfix**: If the program started without a configured MIDI output and the output was
  selected afterwards, the instrument settings of the active preset (SysEx, bank,
  program, volume) never reached the synth, and everything played with the default
  sound until the next preset change. They are now resent when the output port opens.
- **Bugfix**: A Program Change higher than the number of presets in the bank
  deselected the current preset without selecting a new one, so everything went
  silent. Such Program Changes are now ignored and the current preset stays active.
- Preset switching by Program Change can be turned off on the "input/output options"
  tab of MIDI Setup (on by default). When it is off, all Program Changes are forwarded
  to the outputs.
- **Bugfix**: After reverting a preset, it stayed silent until it was selected again,
  because the reloaded elements did not receive incoming MIDI messages. Now the preset
  plays right after a revert, its instrument settings are resent, and the metronome
  gets the restored values too.
- **Bugfix**: After a crash or power failure the program refused to start ("Another
  instance is running") until the lock file left in the temp folder was deleted by hand.
  The program now detects when the instance that created the lock file is no longer
  running, and removes it automatically. The lock file is now called `MidiLayer.lock`
  instead of `MidiLayer.pid`.
- **Bugfix**: Renaming an element to the name of another element in the same preset
  silently overwrote the other element's settings on save. The program now warns that
  the name is already used and restores the old name (case-insensitive).
- **Bugfix**: After renaming a bank, the open preset and its elements kept saving under
  the old bank name: the program created a new folder with the old name, while the renamed
  bank kept the old state. Saving now goes to the renamed bank.
- **Bugfix**: After reverting an element, and for duplicated elements, the volume slider,
  the program and bank dials and the SysEx send buttons sent every message several times,
  and the ELEMENT label was created more than once. The controls are now connected only
  once per element.
- **Bugfix**: MIDI devices with letters outside Latin-1 in their names (e.g. ő, ű) were not
  restored to their slots on startup ("[not defined]"), because the program saved their
  names with those letters replaced by '?'. Names are now stored unchanged. The log file
  is now UTF-8 too. (A device saved incorrectly before has to be selected once more in
  MIDI Setup.)
- **Unsaved changes**: Until now, unsaved element changes were silently lost on preset
  change, while on exit and bank change the preset-level data (element order, metronome)
  was saved without asking. Now:
  - on a manual preset change (in the list), when creating a new preset, on bank change
    and on exit the program asks: Save / Discard / Cancel;
  - on a preset change by Program Change it does not ask (nobody can answer during a live
    performance) and does not save: the changes are lost, and this is logged;
  - in the preset list, a preset with unsaved changes is marked with an asterisk (*).
- **Bugfix – metronome**:
  - Tap tempo always gave too fast a tempo (the first tap was skipped and the sum was
    divided by the wrong number of intervals). It now uses the average interval of the
    last 8 taps at most.
  - When switched on, the first beat flashed twice (the timer started with 0 ms).
  - When switched off, the timer kept ticking (it now keeps running only if UDP sending is
    enabled).
  - On preset change the tempo dial only appeared to change: its internal value stayed the
    old one, and the next time the metronome was switched on or off, this old tempo was
    stored in the new preset.
- **Trash**: Deleted elements and presets now go to the Windows Recycle Bin, where they can
  be restored to their original location. If the Recycle Bin is not available (e.g. on a
  USB stick), the program copies them into a `trash` folder next to the data folder (by
  timestamp / bank / preset) and deletes only after a successful copy; if that fails too,
  nothing is deleted and a warning is shown. Bugs fixed: because of the old trash's
  relative path the copy could fail while the deletion still happened (the element was
  lost for good); deleting a preset did not put its elements into the trash; the trash
  showed up as a bank called "trash" in the Open Bank list (the old `data/trash` folder
  is now hidden).
- **Bugfix**: When the MIDI monitor was closed with Esc, its monitoring stayed on: the next
  time it was opened, every message appeared several times. Monitoring now stops whenever
  the window is closed (Esc included).
- **Bugfix**: Invalid SysEx (not starting with F0 or not ending with F7) was sent on element
  select and unselect too, and an unfinished one-digit byte was silently skipped, so a
  shifted message could be sent. Only valid SysEx is sent now; an invalid SysEx field is
  shown in red, and its tooltip explains the problem.
- **Bugfix**: On Windows, a bank, preset or element could not be renamed by changing only
  the letter case (e.g. "piano" → "Piano"): the program treated it as a name collision, or
  the rename failed. This now works.
- **Bugfix**: If there was no bank to open on startup (first start, deleted
  `midiLayer.ini`, or the last bank's folder no longer exists), the program started without
  a bank, and a new preset was created in the root of the data folder. Now the first
  existing bank is opened, or a new bank is created if there is none; presets cannot be
  created without a bank.
- Update check disabled: it watched the original author's SourceForge releases (over
  plain http), so it would have offered the original version to users of this fork. The
  setting ("Check for updates at startup") is hidden.
- On Windows the virtual MIDI ports are not created (not supported by the Windows MM API).
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
