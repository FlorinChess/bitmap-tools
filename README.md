# Assignment 3: ASCII Paint

## Lernziel

Dynamischer Speicher, File I/O, Structs, Enums, Endianness

---
## Beschreibung

### Ascii Paint

In diesem Semester gilt es im 3. Assignment von ESP eine Art Paint für ASCII-Bilder zu programmieren.
Die Aufgabe ist in folgende Kategorien aufgeteilt:

- **Bild Einlesen**
- **Zeichnen**
- **Bonus: Terminal-Farben**

---

## Inhaltsverzeichnis
<summary>

---

</summary>

[[_TOC_]]


## Bild Einlesen

Es gibt im Rahmen dieses Assignments zwei Arten von Bildern zu verstehen:

- ASCII-Bilder
- BMP-Bilder

---

### ASCII-Bilder
Diese sind einfache Textdateien, deren ASCII-Inhalt 1:1 eingelesen und gespeichert werden kann.

Beispiel: Dreieck
```c
  Z
 ZZZ
ZZZZZ
```

### BMP-Bilder
Im Rahmen dieses Assignments haben wir uns für BMP-Bilder zum Einlesen entschieden, da diese in ihrer Struktur geeignet erschienen.

(Wer sich genauer mit dem Aufbau von BMP-Bildern beschäftigen möchte, sei herzlich dazu eingeladen: https://en.wikipedia.org/wiki/BMP_file_format#Example_1)

Die nötigsten Informationen für dieses Assignment seien allerdings hier zusammengefasst:
- Alle Werte sind mit Little Endian gespeichert.
- Die ersten 2 Byte bestehen IMMER aus "BM" - ist das nicht gegeben, ist das File ungültig! ("Magic Number")
- Die Breite des Bildes beginnt bei Position 0x12 und ist über 4 Byte verteilt.
- Die Höhe des Bildes beginnt bei Position 0x16 und ist ebenfalls über 4 Byte verteilt.
- Die Position, an der das eigentliche Bild beginnt, ist bei Adresse 0x0A gespeichert und ebenfalls über 4 Byte verteilt.
- Das Pixel-Array ist im Format BGR statt RGB gespeichert.
- **Jede Zeile wird auf eine Länge gepadded sodass `Zeilenlänge mod 4 == 0`. Wenn eine Zeile 2 Pixel enthält, so besteht diese aus 8 Byte statt nur 6 für die 3 RGB Werte der Zeile.**
  In den meisten Fällen wird mit 00 gepadded. Eine Zeile, welche aus einem blauen und einem roten Pixel besteht würde folgendermaßen aussehen: `FF 00 00 00 00 FF 00 00`.

Wichtig ist außerdem noch zu wissen, dass der Bildinhalt von links nach rechts und von unten nach oben eingelesen werden muss!
Der erste Wert in der Datei entspricht also dem Pixel links unten im Bild.

| Länge in Byte | Position in der Datei              | Inhalt                                                               |
|---------------|------------------------------------|----------------------------------------------------------------------|
| 2             | 0                                  | Magic Number (Muss der ASCII-Text "BM" sein, ohne String-Terminator) |
| 4             | 0x0a                               | Start des Pixel-Arrays                                               |
| 4             | 0x12                               | Breite des Pixel-Arrays (Achtung zwecks padding!)                    |
| 4             | 0x16                               | Höhe des Pixel-Arrays                                                |
| N/A           | N/A (siehe Start des Pixel-Arrays) | Pixel-Array im Format B,G,R                                          |


Beispiel 10.bmp:
```c
42 4d 76 01 00 00 00 00   00 00 36 00 00 00 28 00
00 00 0a 00 00 00 0a 00   00 00 01 00 18 00 00 00
00 00 40 01 00 00 23 2e   00 00 23 2e 00 00 00 00
00 00 00 00 00 00 bb 76   31 bb 76 31 bb 76 31 bb
76 61 . . . . .
```
42 4d == Header (BM)\
36 00 00 00 == Start des Pixel-Array (54)\
0a 00 00 00 == Breite (10)\
0a 00 00 00  == Höhe (10)\
bb 76 31 bb 76 31 ... == Pixel Array

**Hinweis:
Die Werte können normalerweise in Linux problemlos eingelesen werden, hier muss nichts umgedreht werden (außer dass von unten nach oben eingelesen werden muss!)\
Für uns sieht es so aus als wäre die Zahl "verkehrt" gespeichert, dies liegt wie oben bereits erwähnt am Little Endian Encoding. `36 00 00 00` enstpricht `0x00000036`.**


### Mapping
Nachdem das Bild eingelesen wurde, müssen die Werte noch auf Ascii-Zeichen gemapped werden.\
Dies funktioniert in unserem Assignment so, dass zuerst die eingelesenen RGB-Werte in Grayscale-Werte umgewandelt und danach diese Grayscale-Werte in Ascii-Zeichen übersetzt werden.

Die RGB-Werte werden dabei wie folgt in Grayscale-Werte umgewandelt (die Reihenfolge der Addition ist dabei unbedingt zu beachten!):

```c
Grayscalewert = Redvalue * 0.2126 + Greenvalue * 0.7152 + Bluevalue * 0.0722
```

(Der Grayscalewert kann hierbei stets nur Werte zwischen 0 und 255 annehmen)

Um nun vom Grayscalewert auf das entsprechende Ascii-Zeichen zu kommen ist folgendes Mapping einzuhalten:

| Wertebereich  |           Ascii-Symbol                       |
|---------------|----------------------------------------------|
| 0 bis 19      | @                                            |
| 20 bis 39     | $                                            |
| 40 bis 59     | #                                            |
| 60 bis 79     | *                                            |
| 80 bis 99     | !                                            |
| 100 bis 119   | =                                            |
| 120 bis 139   | ;                                            |
| 140 bis 159   | :                                            |
| 160 bis 179   | ~                                            |
| 180 bis 199   | -                                            |
| 200 bis 219   | ,                                            |
| 220 bis 239   | .                                            |
| 240 bis 255   |  (WHITESPACE)                                |

### Hilfestellungen
- Zum Öffnen des BMP-Files ist der `rb` Modus zu verwenden
- `fseek()` sowie `fread()` sind sehr hilfreiche Funktionen, um Höhe, Breite usw. einzulesen

## Zeichenfeld
Hier wird das eigentliche Bild dargestellt.\
Außerdem hat ein Zeichenfeld immer eine gewisse Größe width x height (entspricht der Arbeitsfläche in Paint).\
Diese kann beispielsweise 100x100 chars betragen und kann während der Laufzeit beliebig vom Nutzer geändert werden.

Ein leeres Zeichenfeld besteht lediglich aus einem Rahmen aus '-' und '|'.\

Beispiel für ein Prokjekt mit einem 8x10 Zeichenfeld:

```c
----------
|        |
|        |
|        |
|        |
|        |
|        |
|        |
|        |
|        |
|        |
----------
>
```

## Programmablauf
Zu Beginn wird der Nutzer nach der gewünschten Größe für das Zeichenfeld gefragt. \
**Dabei sollen Größen bis zu maximal 18446744073709551615 x 18446744073709551615 unterstützt werden!**

Beispiel:
```c
> Please Enter the size of the Drawing Pane (width, height): 50 50
```
Hier hätte das Zeichenfeld nun eine Größe von 50x50 chars.

Danach können sämtliche Befehle in beliebiger Reihenfolge und Häufigkeit auftreten.

Dabei ist immer zuerst das Zeichenfeld auszugeben und danach eine Leerzeile.\
Außerdem ist darauf zu achten das gesamte Layout (inklusive Leerzeichen) genauestens einzuhalten!\
Man beachte außerdem den Command-Prompt (> ) vor dem Einlesen eines jeden Befehls.

Beispiel Programm-Start:

```c
./programname
Please Enter the size of the Drawing Pane (width, height): 8 10
----------
|        |
|        |
|        |
|        |
|        |
|        |
|        |
|        |
|        |
|        |
----------
>
```

## Befehle

Im folgenden werden alle validen Befehle aufgelistet, die das Programm behandeln können sollte.

**Sämtliche Befehle sowie Eingaben sind case-sensitive zu behandeln!**

---
#### Load
Laden eines Bildes
```c
load Picture
```
Hiermit soll ein BMP-Bild ins Programm geladen und als Hintergrund-Ebene hinzugefügt werden. Das Bild wird hierbei von der linken oberen Ecke des Zeichenfelds ausgehend platziert. \
Anhand der Magic Number ("BM") soll erkannt werden, ob das File valide ist.

Das Bild soll immer im Hintergrund bleiben. Außerdem ist zu beachten, dass es immer nur ein geladenes Bild geben kann, also überschreibt jeder neue Aufruf das vorherige Bild! Dieser Fall soll jederzeit eintreten können. \
Dabei bleiben jedoch alle anderen gezeichneten Elemente bestehen!
Hinweis: Es ist auch möglich, dass dieser Befehl kein einziges Mal im Programmablauf aufgerufen wird. Dann wird auf der leeren Zeichenfläche gezeichnet.

---

#### Resize
```c
> resize X Y
```
Hier soll die Größe des Zeichenfelds modifiziert werden, wobei x die neue Breite und y die neue Höhe ist.\
Sollten bereits Elemente auf dem Zeichenfeld existieren, bleiben diese unangerührt (auch, wenn diese womöglich abgeschnitten werden o.Ä.)!
Falls das Zeichenfeld wieder vergößert werden sollte, sollen die Inhalte wieder erscheinen.

---
#### Draw
```c
> draw Symbol X1,Y1 Direction Amount
```

Im Beispiel unten wird eine simple Linie aus dem angegebenen char Symbol gezeichnet. X,Y sind dabei der Startpunkt, Direction gibt die Richtung vom Startpunkt ausgehend an und Amount die Anzahl der Symbole. Direction wird hierbei als Winkel im Einheitskreis interpretiert (0 heißt 0° und entspricht der Richtung "rechts" im Bild); dabei sind nur Winkel von 0°-315° in 45°-Schritten möglich.

Dieser Befehl zeichnet eine Linie, bestehend aus 20 R, beginnend von den Koordinaten 3,2 nach rechts:
```c
> draw R 3,2 0 20
----------------------------
|                          |
|  RRRRRRRRRRRRRRRRRRRR    |
|                          |
|                          |
|                          |
----------------------------
>
```

Dieser Befehl zeichnet eine Linie beginnend von den Koordinaten 3,2 diagonal nach unten:
```c
> draw R 3,2 315 4
----------------------------
|                          |
|  R                       |
|   R                      |
|    R                     |
|     R                    |
|                          |
----------------------------
>
```
---
#### Fill
```c
> Fill X1,Y1 X2,Y2 Symbol
```

Im Beispiel unten wird eine simples Rechteck aus dem angegebenen char Symbol gezeichnet. X1,Y1 ist dabei der Startpunkt, X2,Y2 der Endpunkt.
Zwischen diesen beiden Punkten wird ein Rechteck aufgespannt, bestehend aus dem char Symbol.\

Dieser Befehl zeichnet ein Rechteck, bestehend aus 20 R, beginnend von den Koordinaten 3,2 nach rechts:
```c
> fill 3,2 6,3 R
----------------------------
|                          |
|  RRRR                    |
|  RRRR                    |
|                          |
|                          |
----------------------------
>
```

---
#### Rotate
```c
rotate
```
Dreht das letzte erstellte Element um 90° gegen den Uhrzeigersinn. Gedreht wird dabei immer um die Startkoordinate.

---
#### Undo
```c
undo
```
Macht den letzten ausgeführten Befehl rückgängig. Funktioniert nicht als erster Befehl, oder zwei mal hintereinander. Auch nach load muss undo nicht funktionieren.

**Bonus**: Undo funktioniert auch nach load.

---
#### Save
```c
save Filename
```
Dabei wird der Inhalt des Zeichenfelds in ein File namens Filename gespeichert.

Beispiel:
```c
> save ergebnis.txt
```

---
#### Quit
```c
quit
```
Beendet das Programm. Der Rückgabewert des Programms soll dabei 0 sein.

### BONUS:

---
#### Color
```
color name
```

Verändert die Farbe, in der gezeichnet wird. Hierzu werden ANSI-Escape-Sequenzen verwendet. Diese sind einfach in Strings hinzuzufügen und funktionieren wie folgt:
```
> "\033[31mDieser Satz wird mit roter Schrift ausgegeben.\n"
```
Um die Farbe wieder auf den Standard zurückzusetzen, müsste man nach \n noch die Reset-Sequenz anfügen. Im hier gezeigten Fall wäre jede kommende Ausgabe ebenfalls mit roter Schrift.

Die verwendeten Sequenzen sind der folgenden Tabelle zu entnehmen:

| Escape-Sequenz | Name            | Bedeutung/Farbe     |
|----------------|-----------------|---------------------|
| \033[0m        | -               | Reset-Sequenz       |
| \033[30m       | black           | Schwarz             |
| \033[31m       | red             | Rot                 |
| \033[32m       | green           | Grün                |
| \033[33m       | yellow          | Gelb                |
| \033[34m       | blue            | Blau (dunkel)       |
| \033[35m       | magenta         | Magenta/Violett     |
| \033[36m       | cyan            | Blau (hell)         |
| \033[37m       | white           | Weiß                |

Es sei angemerkt, dass das nur ein Auszug aus der langen Liste ist und man auch u.a. den Hintergrund des Textes einfärben könnte. \
Das würde allerdings den Rahmen der Aufgabe sprengen. Bei Interesse gibt es folgende Ressource als Startpunkt: https://de.wikipedia.org/wiki/ANSI-Escapesequenz \
Eine Liste der möglichen Sequenzen wäre unter anderem hier zu finden: https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences (Stand: 23.02.2023)

***WICHTIG:***
Chars, die farbig sind (also nicht der Standard-Reset-Farbe entsprechen!), sind wie folgt auszugeben: FARBE-CHAR-RESET.
Gerne dafür folgende Vorlage verwendet werden: `printf("%s%c%s", Farbe, Symbol, Reset)`
Für Standard-Farbige chars (= mit der Farbe Reset) darf folgender Befehl verwendet werden: `printf("%c", Symbol)`

**Hinweis: Unter Windows funktionieren diese Escape-Sequenzen möglicheweise nicht richtig!**

## Ergänzendes

### Sichtbarkeit - Hierarchie
Neuere Elemente sind über den alten zu zeichnen, das heißt, bei Überschneidung wird immer das neueste angelegte Element angezeigt.
Sollte das oben liegende Element gelöscht werden, wird natürlich das alte wieder angezeigt.

### Ursprung
Dieser Begriff steht stellvertretend für die Position eines Elements auf dem Zeichenfeld bzw. kennzeichnet dessen Koordinaten-Ursprung.

Der Ursprungspunkt ist immer das erste eingelesene Pixel bzw. der am weitest links liegende oberste Punkt des jeweiligen Bildobjekts.

### Out Of Bounds - Objekte

Natürlich kann es vorkommen, dass ein Bildobjekt mit dessen Ursprung nicht mehr vollständig auf das Zeichenfeld passt oder ganz außerhalb liegt.\
In diesem Fall wird es einfach abgeschnitten bzw. nicht angezeigt, existiert aber ganz normal weiterhin im Speicher!\
(Das gilt auch bei negativen Koordinaten/Werten, diese sind nicht als invalide einzuordnen!)

## Beispiel für vollständigen Programm Ablauf 

```
> Please Enter the size of the Drawing Pane (width, height): 10 10
------------
|          |
|          |
|          |
|          |
|          |
|          |
|          |
|          |
|          |
|          |
------------
> draw A 1,1 0 3
------------
|AAA       |
|          |
|          |
|          |
|          |
|          |
|          |
|          |
|          |
|          |
------------
> draw B 10,10 135 4
------------
|AAA       |
|          |
|          |
|          |
|          |
|          |
|      B   |
|       B  |
|        B |
|         B|
------------
> fill 2,2 5,5 C
------------
|AAA       |
| CCCC     |
| CCCC     |
| CCCC     |
| CCCC     |
|          |
|      B   |
|       B  |
|        B |
|         B|
------------
> draw D 8,6 0 10
------------
|AAA       |
| CCCC     |
| CCCC     |
| CCCC     |
| CCCC     |
|       DDD|
|      B   |
|       B  |
|        B |
|         B|
------------
> draw E 3,6 90 5
------------
|AAA       |
| CECC     |
| CECC     |
| CECC     |
| CECC     |
|  E    DDD|
|      B   |
|       B  |
|        B |
|         B|
------------
> resize 20 20
----------------------
|AAA                 |
| CECC               |
| CECC               |
| CECC               |
| CECC               |
|  E    DDDDDDDDDD   |
|      B             |
|       B            |
|        B           |
|         B          |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
----------------------
> draw F 12,12 0 3
----------------------
|AAA                 |
| CECC               |
| CECC               |
| CECC               |
| CECC               |
|  E    DDDDDDDDDD   |
|      B             |
|       B            |
|        B           |
|         B          |
|                    |
|           FFF      |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
----------------------
> rotate
----------------------
|AAA                 |
| CECC               |
| CECC               |
| CECC               |
| CECC               |
|  E    DDDDDDDDDD   |
|      B             |
|       B            |
|        B           |
|         B F        |
|           F        |
|           F        |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
----------------------
> rotate
----------------------
|AAA                 |
| CECC               |
| CECC               |
| CECC               |
| CECC               |
|  E    DDDDDDDDDD   |
|      B             |
|       B            |
|        B           |
|         B          |
|                    |
|         FFF        |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
----------------------
> rotate
----------------------
|AAA                 |
| CECC               |
| CECC               |
| CECC               |
| CECC               |
|  E    DDDDDDDDDD   |
|      B             |
|       B            |
|        B           |
|         B          |
|                    |
|           F        |
|           F        |
|           F        |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
----------------------
> rotate
----------------------
|AAA                 |
| CECC               |
| CECC               |
| CECC               |
| CECC               |
|  E    DDDDDDDDDD   |
|      B             |
|       B            |
|        B           |
|         B          |
|                    |
|           FFF      |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
----------------------
> fill 1,10 3,12 G
----------------------
|AAA                 |
| CECC               |
| CECC               |
| CECC               |
| CECC               |
|  E    DDDDDDDDDD   |
|      B             |
|       B            |
|        B           |
|GGG      B          |
|GGG                 |
|GGG        FFF      |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
----------------------
> rotate
----------------------
|AAA                 |
| CECC               |
| CECC               |
| CECC               |
| CECC               |
|  E    DDDDDDDDDD   |
|      B             |
|GGG    B            |
|GGG     B           |
|GGG      B          |
|                    |
|           FFF      |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
----------------------
> rotate
----------------------
|AAA                 |
| CECC               |
| CECC               |
| CECC               |
| CECC               |
|  E    DDDDDDDDDD   |
|      B             |
|G      B            |
|G       B           |
|G        B          |
|                    |
|           FFF      |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
----------------------
> rotate
----------------------
|AAA                 |
| CECC               |
| CECC               |
| CECC               |
| CECC               |
|  E    DDDDDDDDDD   |
|      B             |
|       B            |
|        B           |
|G        B          |
|G                   |
|G          FFF      |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
----------------------
> rotate
----------------------
|AAA                 |
| CECC               |
| CECC               |
| CECC               |
| CECC               |
|  E    DDDDDDDDDD   |
|      B             |
|       B            |
|        B           |
|GGG      B          |
|GGG                 |
|GGG        FFF      |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
----------------------
> load pictures/mona_lisa.bmp
----------------------
|AAA=================|
|=CECC===============|
|=CECC===============|
|=CECC===============|
|=CECC===============|
|=!E====DDDDDDDDDD===|
|=!====B=============|
|=!==!!=B!==;;;;;;;;;|
|=!==!!=!B==:::::::::|
|GGG=!!=!!B=:::::::::|
|GGG=!!=!!==:::::::::|
|GGG=!!=!!==FFF::::::|
|=!==!!=!!==:::::::::|
|=!==!!=!!==:::::::::|
|=!==!!=!!==:::::::::|
|=!==!!=!!==:::::::::|
|=!==!!=!!==:::::::::|
|=!==!!=!!==:::::::::|
|=!==!!=!!==:::::::::|
|=!==!!=!!==:::::::::|
----------------------
> resize 50 50
----------------------------------------------------
|AAA===============================================|
|=CECC=============================================|
|=CECC=============================================|
|=CECC=============================================|
|=CECC=============================================|
|=!E====DDDDDDDDDD=================================|
|=!====B===========================================|
|=!==!!=B!==;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;|
|=!==!!=!B==:::::::::::::::::::::::::::::::::::::::|
|GGG=!!=!!B=:::::::::::::::::::::::::::::::::::::::|
|GGG=!!=!!==:::::::::::::::::::::::::::::::::::::::|
|GGG=!!=!!==FFF::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::~~~~~~~~~~~~~~~~~~~~~~~~|
|=!==!!=!!==:::::::::::::::~-----------------------|
|=!==!!=!!==:::::::::::::::------------------------|
|=!==!!=!!==:::::::::::::::------------------------|
|=!==!!=!!==:::::::::::::::------------------------|
|=!==!!=!!==:::::::::::::::------------------------|
|=!==!!=!!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::-----------------~~:::::|
|=!==!!==!==:::::::::::::::-----------------~:;;;;;|
|=!==!!==!==:::::::::::::::------------------:;;;;;|
|==!=!!==!==:::::::::::::::-----------------~;;;;;;|
|==!=!!==!==:::::::::::::::-----------------~;;;;;;|
|==!=!!==!==:::::::::::::::-----------------~;;;;;;|
----------------------------------------------------
> fill 25,25 30,30 X
----------------------------------------------------
|AAA===============================================|
|=CECC=============================================|
|=CECC=============================================|
|=CECC=============================================|
|=CECC=============================================|
|=!E====DDDDDDDDDD=================================|
|=!====B===========================================|
|=!==!!=B!==;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;|
|=!==!!=!B==:::::::::::::::::::::::::::::::::::::::|
|GGG=!!=!!B=:::::::::::::::::::::::::::::::::::::::|
|GGG=!!=!!==:::::::::::::::::::::::::::::::::::::::|
|GGG=!!=!!==FFF::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::::::::::::::::::::::::::::|
|=!==!!=!!==:::::::::::::XXXXXX~~~~~~~~~~~~~~~~~~~~|
|=!==!!=!!==:::::::::::::XXXXXX--------------------|
|=!==!!=!!==:::::::::::::XXXXXX--------------------|
|=!==!!=!!==:::::::::::::XXXXXX--------------------|
|=!==!!=!!==:::::::::::::XXXXXX--------------------|
|=!==!!=!!==:::::::::::::XXXXXX--------------------|
|=!==!!=!!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::------------------------|
|=!==!!==!==:::::::::::::::-----------------~~:::::|
|=!==!!==!==:::::::::::::::-----------------~:;;;;;|
|=!==!!==!==:::::::::::::::------------------:;;;;;|
|==!=!!==!==:::::::::::::::-----------------~;;;;;;|
|==!=!!==!==:::::::::::::::-----------------~;;;;;;|
|==!=!!==!==:::::::::::::::-----------------~;;;;;;|
----------------------------------------------------
> resize 10 10
------------
|AAA=======|
|=CECC=====|
|=CECC=====|
|=CECC=====|
|=CECC=====|
|=!E====DDD|
|=!====B===|
|=!==!!=B!=|
|=!==!!=!B=|
|GGG=!!=!!B|
------------
> quit

```

## Fehlermeldungen
Falls beim Einlesen des Files ein Fehler auftritt, soll der Befehl sofort abgebrochen und das Program normal weiter ausgeführt werden!\
Dabei ist folgende Fehlermeldung auszugeben:

```c
Invalid File
```
Danach folgt in der nächsten Zeile wie gewohnt direkt der Command Prompt!

---

Falls beim Einlesen des Befehls ein Fehler auftritt, wird der Befehl sofort abgebrochen und das Program normal weiter ausgeführt!\
Dabei ist folgende Fehlermeldung auszugeben:

```c
Invalid Input
```
Danach folgt in der nächsten Zeile (wie gewohnt) direkt der Command Prompt!

**Hinweis: Es darf davon ausgegangen werden, dass die Anzahl und der Typ der mitgegebenen Parameter immer valide ist!**

---

Sollte ein ungültiger Aufruf eines gültigen Befehls erfolgen, ist folgende Fehlermeldung auszugeben:

```
Command can not be executed in this state!
```

Danach folgt, wie gewohnt, der Command-Prompt.

**Hinweis: Das kann ausschließlich bei rotate und undo vorkommen!**

---

Falls nicht mehr genügend Speicher zur Verfügung steht ODER die Zeichenfeldgröße ihr Maximum überschreitet, soll das Programm mit dem Returnwert 3 und folgender Fehlermeldung beendet werden:
```c
Out of Memory!
```

## Hinweise:
- Die Art der Implementation ist jedem selbst überlassen! Es gibt keine expliziten Spezifikationen (bis auf die erlaubten Libraries)! \
  Technisch zählt am Ende nur, dass der Output gemäß der Angabe korrekt ist (bestandene TCs, sofern nicht hardgecoded) UND dass keine Speicherfehler existieren.
- Allerdings gibt es für übermäßig unnötig allokierten Speicher, der nie genutzt wird, Punkteabzüge von bis zu 100%! (Lernziel nicht erreicht) \
  Zum Beispiel bei UNNÖTIG riesigen Arrays!
- Die Implementierung muss allenfalls sinnvoll beim AG argumentiert werden können, ansonsten drohen Punkteabzüge von bis zu 100%!

### Dennoch wollen wir euch folgende Empfehlung für die Implementierung ans Herz legen:
- Für Elemente ein Struct erstellen und die relevanten Informationen speichern
- Das Zeichenfeld als dynamisches 2D char-Array implementieren
- Meta-Informationen in der main() bzw. recht früh im Programmablauf anlegen und per Reference weitergeben; möglich ist auch hier ein Struct

### Pro-Tipp von Lukas:
Blind zu programmieren ist eines der schlimmsten Dinge die man tun kann, denn es bedeutet zu coden ohne einen Output zu sehen.\
Im schlimmsten Fall sieht man dabei erst sehr spät wenn eine Implementation keinen Sinn macht oder ein Denkfehler zugrunde liegt und die ganze Implementation verworfen werden kann.\
Das ist natürlich frustrierend, zumal es auch motivationstechnisch nicht von Vorteil ist, nie einen Output und somit einen wirklichen Fortschritt zu sehen.\
Daher lautet mein Tipp: Schau dass du so früh wie möglich dein Programm auf Korrektheit überprüfst indem du beispielsweise printf's einbaust und so einen Output erzeugen kannst mit dem du deine Implementation jederzeit Schritt für Schritt überprüfen kannst.\
Fang daher mit dem Zeichenfeld und ganz einfachen Ebenen (wie z.B. dem draw-Befehl) an! ;)

---
## Spezifikation

### Erlaubte Libraries
Alle C-Standardbibliotheken

Bei Verstößen drohen Punkteabzüge von bis zu 100%!

## Abgabe
* 22.05.2023, 23:59
* a3.c

## Bewertung

| Kategorie        | Punkte | 
|------------------|--------|
| Funktionaliät    | 22     |
| Doku und Stil    | 3      |
| Programmstruktur | 3,5    |
| Robustheit       | 3,5    |
|                  | 31     |
| Bonus            | 4      |
