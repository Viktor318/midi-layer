# Változásnapló

A MidiLayer minden lényeges változása ebben a fájlban van dokumentálva.

## 1.0.0 verzió (jelenlegi)

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
