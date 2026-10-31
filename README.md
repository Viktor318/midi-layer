# MidiLayer

A MidiLayer egy egyszerű és könnyen kezelhető alkalmazás, amely élő zenei előadások támogatására készült.

Slotok és csatornák kezelése, a sebesség (velocity) és a hangerő állítása, a hangtartomány testreszabása, valamint modulációs kerekek és egyéb kontrollerek csatlakoztatása — mindezt a MidiLayer segítségével.

## Funkciók

- Akár 8 MIDI interfész-slot
- Egyszerű MIDI merge és MIDI thru
- MIDI routing interfész, csatorna vagy billentyűzet-tartomány szerint
- Billentyűzet-osztás (keyboard split) akár 64 részre, 1 csatornaforrással
- MIDI be- és kimeneti monitor eszköz
- Program change küldése minden eszköznek
- Vizuális kattintás (click) segédeszköz presetenként
- Esemény-szűrés billentyűzet-tartomány szerint
- Húzd-és-ejtsd preset-szervező
- SysEx küldése elemenként
- Szekvencer-vezérlő
- Presetek és elemek egyszerű kezelése fájlkezelőben

## Build

A projekt Qt-alapú C++ alkalmazás (lásd `midiLayer.pro`), és a [RtMidi](https://www.music.mcgill.ca/~gary/rtmidi/) könyvtárat használja a MIDI kommunikációhoz (a `RtMidi` mappában található).

Fordításhoz szükséges:
- Qt (a `.pro`/`.ui` fájlok alapján Qt Widgets)
- C++ fordító (a platformodnak megfelelő: MSVC/MinGW Windows alatt, Clang/GCC macOS/Linux alatt)
- RtMidi (mellékelve a repóban)

```bash
qmake midiLayer.pro
make
```

(Windows alatt Qt Creator-ral megnyitva és lefordítva is működik.)

## Eredet

Ez a repó a projekt eredeti SourceForge SVN forráskódjából lett importálva:
https://sourceforge.net/projects/midi-layer/

Az importált forrás az eredeti szerző 1.0.0-s verziója; a szerző a Windows- és
Mac-buildekhez a 89-es SVN-revíziót jelölte meg (`midiLayer.pro`). Az eredeti
fejlesztés 2016 óta nem folytatódott, ezért a fork nem szinkronizál automatikusan
a SourceForge-tárolóval. Ha ott mégis új változás jelenne meg, azt kézzel lehet
átvenni.

## Licenc

GNU General Public License v3.0 (GPLv3). Lásd a `LICENSE` fájlt.
