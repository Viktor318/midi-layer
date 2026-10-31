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

A projekt Qt-alapú C++ alkalmazás (lásd `src/midiLayer.pro`), és a [RtMidi](https://www.music.mcgill.ca/~gary/rtmidi/) könyvtárat használja a MIDI kommunikációhoz (a `src/RtMidi` mappában található). A futtatáshoz szükséges adatok (demo bank, betűtípusok, alapbeállítások) a `dist` mappában vannak.

Fordításhoz szükséges:
- Qt 6 (Widgets, Network, Xml); Windowson a Qt 6.12 MinGW 64-bit változatával tesztelve
- C++ fordító (a platformodnak megfelelő: MSVC/MinGW Windows alatt, Clang/GCC macOS/Linux alatt)
- RtMidi (mellékelve a repóban)

```bash
cd src
qmake midiLayer.pro
make
```

Windows alatt a `src/midiLayer.pro` Qt Creatorral megnyitva és lefordítva is működik. A Release-fordításból a gyökérmappában lévő `deploy.bat` készít futtatható, hordozható csomagot (ZIP-pel együtt) a `deploy` mappába.

## Eredet

Az eredeti projekt: https://sourceforge.net/projects/midi-layer/
(szerző: António José Soares Maia)

A projektnek a SourceForge-on két forráskódtára van:

- **SVN** (`code`): a 2012–2016 közötti fejlesztés, az utolsó revízió az r89
  (1.0.0-s verzió, 2016). Ebben a forkban a `qt6-windows` ág erre épül.
- **Git** (`repo`): a szerző 2021 decemberében itt folytatta a fejlesztést, egészen
  az **1.1.0-alfa3** verzióig (Qt 6-ra átdolgozva, 4 virtuális MIDI-port, a MIDI
  Setup állapotának megőrzése, sok felületi javítás). Ebben a forkban az
  `upstream-2021` ág ennek változatlan másolata, a `qt6-windows-1.1` ág pedig erre
  épül, kiegészítve a Windowsos javításokkal.

A virtuális MIDI-portokat a mellékelt RtMidi Windowson nem támogatja, ez a funkció
csak Linuxon és macOS-en működik.

Ha a SourceForge-on új változás jelenne meg, a `git fetch sourceforge` után az
`upstream-2021` ág a `sourceforge/master`-re léptethető, és onnan a változások kézzel
átvehetők.

## Licenc

GNU General Public License v3.0 (GPLv3). Lásd a `LICENSE` fájlt.
