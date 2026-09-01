#!/bin/bash
grep -i "\/\/.* rtmidi.*version" RtMidi/RtMidi.h | sed 's/\/\/\s*rtmidi.*version\s*//gi'
