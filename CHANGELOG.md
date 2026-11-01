# Változásnapló

A MidiLayer minden lényeges változása ebben a fájlban van dokumentálva.

## 1.1.0-alfa3 – Windows fork (Viktor318)

A szerző 2021-es, SourceForge git-tárolóbeli 1.1.0-alfa3 verziójára épül.

- Windowsos fordítás Qt 6.12-vel (MinGW 64-bit).
- **Hibajavítás**: A MIDI-portnevek végén □ jel jelent meg Windowson (a mellékelt
  RtMidi a portnév végén hagyta a lezáró nulla karaktert).
- **Hibajavítás**: Első indításkor (mentett ablakpozíció nélkül) a főablak a
  képernyőn kívülre került. Az alapértelmezett ablak most a fő képernyő közepére
  kerül, és a mentett pozíciót csak akkor használja, ha az látszik valamelyik
  képernyőn.
- **Hibajavítás**: Beragadó hangok preset-váltáskor. A billentyű felengedése (Note Off)
  eddig az *új* preset elemein ment át, és ha a hang kívül esett azok tartományán, vagy
  más volt a transzponálás vagy a kimeneti csatorna, a leállítás elmaradt vagy rossz
  helyre ment. Most a program nyilvántartja, melyik billentyű hová szólaltatott meg
  hangot, és a felengedést pontosan oda küldi.
- **Hibajavítás**: Beragadó kitartó pedál preset-váltáskor. A pedál (CC64) felengedése
  most minden olyan csatornára kimegy, ahová a lenyomása ment.
- **Hibajavítás**: Beragadó pitch bend, moduláció (CC1), fúvásvezérlő (CC2) és channel
  aftertouch preset-váltáskor. Ha váltáskor el volt húzva a kerék, a régi csatorna
  elhangolva vagy modulálva maradt. Most amikor a kerék (vagy a nyomás) visszatér
  nyugalmi állásba, a nyugalmi érték minden olyan csatornára kimegy, ahová korábban
  ettől eltérő érték ment.
- **Hibajavítás**: Ha a program beállított MIDI-kimenet nélkül indult, és a kimenetet
  utólag választottuk ki, az aktív preset hangszer-beállításai (SysEx, bank, program,
  hangerő) nem jutottak el a szintihez, és minden az alap hangszínen szólt a következő
  preset-váltásig. Most a kimeneti port megnyitásakor ezek újra kimennek.
- **Hibajavítás**: Ha egy Program Change nagyobb volt, mint a bank presetjeinek száma,
  a program leválasztotta az aktuális presetet, de újat nem választott, így minden
  elnémult. Most az ilyen Program Change-et figyelmen kívül hagyja, és az aktuális
  preset marad.
- A MIDI Setup „input/output options” lapján kikapcsolható a Program Change-re
  történő presetváltás (alapból bekapcsolva). Kikapcsolva minden Program Change
  továbbmegy a kimenetekre.
- **Hibajavítás**: A preset visszavonása (Revert) után a preset elnémult, amíg újra
  ki nem választottuk, mert az újratöltött elemek nem kapták meg a bejövő
  MIDI-üzeneteket. Most a visszavonás után a preset azonnal szól, a hangszer-beállítások
  újra kimennek, és a metronóm is a visszaállított értékeket kapja meg.
- **Hibajavítás**: Összeomlás vagy áramszünet után a program nem indult el („Another
  instance is running”), amíg kézzel ki nem töröltük az ideiglenes mappában maradt
  zárolófájlt. Most a program felismeri, ha a zárolófájlt létrehozó példány már nem fut,
  és magától eltávolítja. A zárolófájl neve `MidiLayer.pid` helyett `MidiLayer.lock`.
- **Hibajavítás**: Ha egy elemet a preset egy másik elemének nevére neveztünk át, a mentés
  szó nélkül felülírta a másik elem beállításait. Most a program figyelmeztet, hogy a név
  foglalt, és visszaállítja a régi nevet (a kis- és nagybetű nem számít).
- **Hibajavítás**: A bank átnevezése után a megnyitott preset és elemei a régi banknévvel
  mentettek: a program a régi néven új mappát hozott létre, az átnevezett bankban pedig a
  régi állapot maradt. Most az átnevezés után a mentés az új bankba kerül.
- Windowson a virtuális MIDI-portok nem jönnek létre (a Windows MM API nem támogatja).
- Fusion felületi stílus alapértelmezésként, mert a Windows 11-es stílus levágta a
  számmezők tartalmát (a `-style <név>` argumentummal felülbírálható).
- Az adományozásra felhívó gomb helyén a „Windows fork by Viktor318” link.
- `deploy.bat`: hordozható Windowsos csomag és ZIP készítése a Release-buildből.

## 1.1.0-alfa3 (az eredeti szerző, 2021. december, kiadatlan)

- 4 virtuális MIDI-port (Linux és macOS).
- **Hibajavítás**: A MIDI Setup elfelejtette az utolsó állapotot.
- **Hibajavítás**: Fordítási figyelmeztetések a naplózó függvényekben.
- Sok felületi javítás, átdolgozott elem-panel.

## 1.0.0 verzió (2016)

- Első valódi Mac-verzió, .dmg csomagban terjesztve.
- Preset eszköztár rendezése.
- Néhány modális ablak méretének igazítása Mac-en.
- MIDI monitorok automatikus meg-/elrejtésének eltávolítása (nem volt praktikus).
- A "portable" szó eltávolítása az ablak címsorából.
- **Hibajavítás**: Interfész nevek feloldása, ha speciális karaktereket tartalmaznak.
- **Hibajavítás**: Az új bank mentése és átnevezése hibázott.

## 0.4.3 verzió

- **Hibajavítás**: A preset eltűnt húzd-és-ejtsd művelet után a preset listában (csak Windows).
- Csak egy példány futtatása (lock fájl) és figyelmeztetés.
- Parancssor: lock fájl eltávolítása.
- Parancssor: súgó.
- Preset áttekintő.

## 0.4.2 verzió

- Metronóm tap-algoritmus javítása.
- Egyéni Control Change al-hangerőhöz.
- Elemek fül be/ki kapcsolóval és szerkesztési infóval.

## 0.4.1 verzió

- **Hibajavítás**: A program összeomlott több MIDI-beállítás használata után.
- **Hibajavítás**: Már megnyitott port-példányok eltávolítása a beállításokból (Linux ALSA driver).
- MIDI Thru port hozzáadása a Linux-fordításhoz (ALSA driver).
- A MIDI monitor már nem modális ablak.
- A program change tárcsa határértékeinek módosítása (0~127-ről 1~128-ra).
- CTRL+M billentyűkombináció a MIDI Monitor bezárásához.
- Gyorsbillentyűk a MIDI dock-monitorok meg-/bezárásához.

## 0.4.0 verzió

- **Hibajavítás**: Rossz csatorna csatorna-visszaállításnál.
- **Hibajavítás**: Nem lehetett módosítani a MIDI slotokat az első kiválasztás után.
- **Hibajavítás**: Felesleges karakter eltávolítása az interfész név végéről a Windows-buildben.
- Preset-váltási késleltetés optimalizálása.
- SysEx és program change problémák megoldása egyes hangmodulokon.
- Transzponálási tartomány -120 és 120 között, tartományon kívüli jelzéssel.
- Egyéni control change korlát hozzáadása.
- "Slot nincs meghatározva" infó a felületen.
- MIDI csatorna-monitor vizuális fejlesztések.
- Frissítés-értesítő fejlesztések.
- Metronóm-ütések küldése UDP csomagokban (működik az Android-alkalmazással).
- Néhány kisebb fejlesztés az elem-felületen:
  - Adományozás gomb hozzáadása.
  - Splash screen időtúllépés hozzáadása.
  - MidiLayer logó cseréje.
  - Logó cseréje.
  - LED-ek optimalizálása.

## 0.3.1 verzió

- Javítás a Windows-terjesztésben. Már fut Windows 8.x alatt is.
- Naplózás optimalizálása.
- MIDI-késleltetés csökkentése.
- Notes Off és Reset Controllers támogatása elem elhagyásakor.

## 0.3.0 verzió

- Qt 5.x használata.
- MIDI port-választó újraírva. Hatékonyabb lett.
- UDP csomagok küldése metronóm-kliensnek (Android/iPhone).
- Vezérlők visszaállításának eltávolítása preset-váltáskor.
- **Hibajavítás**: A bank change küldési sorrendje fordított volt.
- A bank change tartománya 0-127-re módosítva. Kompatibilisebb az eszközök kézikönyveivel.
- Bank change küldése érték-változáskor.
- Az elemek automatikusan menthetők vagy sem.
- Az ikonok támogatják a sötét rendszertémákat.
- A névjegy ablak a felület témaszíneit használja.
- Sok felhasználói felület fejlesztés.

## 0.2.1.31 verzió

- **Hibajavítás**: Program change küldése elem kikapcsolásakor és tárcsa-változtatáskor.
- **Hibajavítás**: Hangerő-változás küldése elem kikapcsolásakor és csúszka-változtatáskor.
- **Hibajavítás**: SysEx küldése elem kikapcsolásakor és a küldés gomb megnyomásakor.
- Menü hozzáadva a dock-ablakok meg-/elrejtéséhez.
- Program change fogadása minden interfész-slotról.
- Program change fogadása minden csatornáról.
- Szekvencer-vezérlő küldése minden slotnak.

## 0.2.0.30 verzió

- Fejlesztések a MIDI-információ továbbításában.
- MIDI Monitor, amely megmutatja a kimenő adatokat is.

## 0.1.0.28 verzió

- Egyszerre csak egy preset tárolása a memóriában. Kevesebb memóriahasználat (jelentős probléma volt).
- Szekvencer-vezérlő dock-widget hozzáadva.
- Metronóm dock-widget újratervezve.
- Csatorna-monitor widgetek újratervezve.
- MIDI monitor ablak újratervezve.
- Névjegy ablak módosítva.
- Beállítások ablak hozzáadva.
- Az ablakok állapota menthető.
- Új tab-sorrend meghatározva.
- Új gyorsbillentyűk meghatározva.
- "Megerősítés kilépéskor" funkció hozzáadva.
- Bank-, preset- és elemnevek védelme az operációs rendszer által nem támogatott fájlnevektől.
- Felület átstilizálva.
- Sok apró hiba javítva.

## 0.0.1.6 verzió

- **Hibajavítás**: Bank betöltésekor minden elem program change-t és hangerő-értéket küldött.
- **Hibajavítás**: Preset-váltáskor nem küldött program change-t.
- Kis jelölés hozzáadva a billentyűzet-grafika képéhez, a szabványos 60-billentyűs tartomány jelzésére.
- Metronóm-hangerő hozzáadva a preset-beállításokhoz.
- Metronóm-beállítás módosítva a preset fájlban.

## 0.0.1.0 verzió

- Kezdeti verzió.
