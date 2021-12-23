Midi Layer - Release Notes
==========================


Version 1.0.0 (current)
--------------------------------
First real version for Mac distributed in .dmg package.
Arrange preset tool bar
Adjust some modal windows sizes on Mac
Remove auto show/hide midi monitors (not pratical)
Remove portable word in window title.
BugFix - Resolve interfaces names when has specials chars.
BugFix - New bank save and rename actions fails.


Version 0.4.3
--------------------------------
BugFix - Preset desappear after drag and drop in preset list (Windows only).
Run only one instance (lock file) and warn.
Command line remove lock file.
Command line help.
Preset overview.


Version 0.4.2
--------------------------------
Improve metronome tap algorith.
Custom Control Change to sub-volume.
Elements tab with on/off and edited info.


Version 0.4.1
--------------------------------
BugFix - Program crashes after multiple MIDI setup usages.
BugFix - Remove already open port instances from setup (Linux ALSA driver).
Include MIDI Thru port on Linux compilation (ALSA driver).
Midi monitor no longer a modal dialog.
Change program change dial limits (was 0~127, now is 1~128).
Use keys CTRL+M to close MIDI Monitor.
Use shortcut keys to open/close MIDI dock monitors.


Version 0.4.0
--------------------------------
BugFix - Wrong channel on channel reset
Bugfix - Not possible change MIDI slots after fist selection.
Bugfix - Remove a extra char at the end of interface name on windows build.
Optimization of preset change delay.
Resolve problem for SysEx and program changes in some sound modules.
Transpose range from -120 to 120 with out of bounds indication
Add a custom control change limit.
Slot not defined information on GUI
Midi channel monitor visual improvements
Improvements at update notifier
Send metronome beats in UDP packages (work with android app)
Some small improvements on element GUI.
- Add donate button
- Add splashScreen time out
- Change MidiLayer logo.
- Change logo.
- Led's optimization.


Version 0.3.1
--------------------------------
Correction on Windows distribution. Now runs in Windows 8.x
Optimization of logs.
Reduce MIDI latency.
Suport Notes Off and Reset Controllers on leave elements.


Version 0.3.0
--------------------------------
Use QT 5.x
MIDI port selector recoded. Now is more efficient.
Ability to send UDP packets to a metronome client (Android/iPhone).
Remove reset controllers on change preset.
Bugfix - Bank Change send order was inverted.
Bank change range change to 0 to 127. More compatible with equipment manuals
Send bank change on value change.
Elements can be or not autosaved.
Icons support dark OS themes.
About dialog have GUI theme colors.
Many user interface improvements.


Version 0.2.1.31
----------------
Bugfix - Program change was sent on element off and dial change
Bugfix - Volume change was sent on element off and slider change
Bugfix - Send SysEx was sent on element off and press send button
Added menu for Show/Hide dock windows.
Can receive program change from all inteface slots
Can receive program change from all channels
Can send sequencer transport to all slots


Version 0.2.0.30
--------------------------
Improvements in the propagation of MIDI information.
MIDI Monitor with possibility to show what is coming out.


Version 0.1.0.28
----------------
Keep in memory only one preset at once. Use less memory (big issue).
Added a sequencer transport dock widget.
Redesigned metronome dock widget.
Redesigned channels monitors widgets.
Redesigned MIDI monitor dialog.
Changed about dialog.
Added preferences dialog.
Can save windows state.
Defined new tab stop order.
Defined new key shortcuts.
Added "confirm on exit" fuctionality.
Prevents bank, preset and elements names from unsupported filenemes in OS.
Restyled GUI.
Corrected many small bugs.


Version 0.0.1.6
---------------
BugFix - At bank load action, all elements were sending program change a volume.
BugFix - On preset change were not sending program changes.
Added a small annotation on image in keyboard graphic, for represent standart 60 keys keyboard range.
Metronome volume addded to preset settings.
Changed metronome setting in preset file.


Version 0.0.1.0
---------------
Initial version.
