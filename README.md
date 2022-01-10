# TiCO

Timer per Camera Oscura

Caratteristiche principali:

* Timer per ingranditore lineare o fattoriale
* Provinatore lineare o F-STOP
* Timer per sviluppo a 4 bagni (configurabile)
* Metronomo
* Flusso di lavoro configurabile (es. passaggio automatico da modalità ingranditore a sviluppo)
* Statistiche sul numero di accensioni / tempo totale di funzionamento della lampada ingranditore

![TiCo Leggi il manuale utente per conoscere tutti i dettagli](https://github.com/sejerpz/tico/blob/main/UserManual.md)

# Bill of Materials

* 1 arduino nano (es. https://www.amazon.it/gp/product/B0716S43Q2/ref=ppx_yo_dt_b_asin_title_o02_s00?ie=UTF8&psc=1)
* 1 scheda display seriale 7 segmenti x 4, led rossi (**TM1637**) (es. https://www.amazon.it/gp/product/B08KT76Z3G/ref=ppx_yo_dt_b_asin_title_o04_s00?ie=UTF8&psc=1)
* 1 encoder rotativo con bottone (es. https://www.amazon.it/gp/product/B07SV5HHM5/ref=ppx_yo_dt_b_asin_title_o02_s00?ie=UTF8&psc=1)
* 2 pulsanti (es. https://www.amazon.it/gp/product/B0825RCZJS/ref=ppx_yo_dt_b_asin_title_o03_s00?ie=UTF8&psc=1)
* 1 scheda relè (es. https://www.amazon.it/gp/product/B06XHJ2PBJ/ref=ppx_yo_dt_b_asin_title_o07_s01?ie=UTF8&psc=1)
* 1 buzzer
* 1 connettore femmina da 3,5" per switch a pedale (aux switch - opzionale e **non testato**)

# Collegamenti

Schema di collegamento

![alt TiCo Fritzing Schema](https://github.com/sejerpz/tico/blob/main/docs/TiCo%20Schema.png?raw=true)

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

Buzzer

    const int buzzerPin = 12;

Relè

    const int rele1Pin = 13;

# Build

TiCO è sviluppato in c++ (arduino) utilizzando Visual Studio Code e PlatformIO

* Installare Visual Studio Code (https://code.visualstudio.com/)

* Da Visual Studio Code installare l'estensione PlatformIO che installerà anche le estensioni relative allo sviluppo c++ (https://platformio.org/install/ide?install=vscode)

* Aprire la cartella TiCO e fare il build della soluzione