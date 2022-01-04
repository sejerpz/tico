# TiCO

Timer per Camera Oscura

# Bill of Materials

1 arduino nano
1 scheda display 7 segmenti x 4, led rossi (TM1637)
1 encoder rotativo con bottone
2 pulsanti
1 scheda relè
1 buzzer
1 connettore 3,5" per switch a pedale (aux switch - opzionale e non testato)

# Collegamenti

E' possibile configurare i pin relativi ai vari componenti nel file TiCO.h

L'encoder prevede 3 pin di collegamento 2 per signal / clock ed 1 per il bottone
const int encoderPin1 = 2;
const int encoderPin2 = 3;
const int button2Pin = 4; // encoder button

Push button Aux
const int buttonAuxPin = 5; // aux switch

Push button principale
const int button1Pin = 6; // start / stop

Push button secondario (Focus)
const int buttonFocusPin = 7; // focus

Display
const int displayClkPin = 8;
const int displayDIOPin = 9;

Serial Rx/Tx (solo per debug)
const int serial2Rx = 10;
const int serial2Tx = 11;

Buzzer
const int buzzerPin = 12;

Relè
const int rele1Pin = 13;

# Build

TiCO è sviluppato in c++ (arduino) utilizzando Visual Studio Code e PlatformIO

* Installare Visual Studio Code

* Da Visual Studio Code installare l'estensione PlatformIO che installerà anche le estensioni relative allo sviluppo c++

* Aprire la cartella TiCO e fare il build della soluzione