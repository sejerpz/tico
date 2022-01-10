# Manuale TiCO Timer

![alt TiCo Front Panel](https://raw.githubusercontent.com/sejerpz/tico/main/docs/TiCo%20Front%20Panel.svg)

## Navigazione

||Azione|Descrizione|
|-------|------|--------|
|Bottone 3|Click|Cambio modalità timer: Ingranditore, Sviluppo, Provinatore, Metronomo|
||Click lungo|Accesso alle impostazioni|

## Segnali acustici

In tutte le modalità lo scorrere di un timer è sempre evidenziato da un segnale acustico ogni 30 sec. ed uno differente ogni minuto.

## Modalità **Ingranditore**

In questa modalità il timer è utilizzato per la stampa, attivando un ingranditore collegato all'uscita relé per il tempo di accensione specificato.

E' possibile configurare TiCO in modo che al termine della stampa passi automaticamente nella modalità sviluppo dopo un tempo di pausa specificato o dopo la pressione del **Bottone 1** (vedere i parametri `Dopo SVILUPPO` e `T. PAUSA`)

||Azione|Descrizione|
|-------|------|--------|
|Display||**I.** `tempo di accensione ingranditore in sec.`
|Encoder|Rotazione|Aumenta / diminuisce tempo accensione ingranditore|
|Bottone 1|Click|se timer fermo **avvia countdown**|
||Click|se timer avviato in countdown **pause / resume**|
||Click lungo|se timer avviato in countdown **cancel**|
|||se timer fermo **Focus ON**|
|Bottone 2|Click|Focus **ON / OFF**|


## Modalità **Sviluppo Fattoriale**

In questa modalità il timer è utilizzabile per lo sviluppo della carta nel classico processo a 3 bacinelle.

La particolarità dello sviluppo fattoriale consiste in due momenti specifici durante il primo bagno di sviluppo:

1. All'avvio il timer esegue un count up del tempo in attesa che l'immagine inizi a formarsi nella carta `tempo di comparsa`
1. A questo punto l'operatore premendo il **Bottone 1** avvia la secondo fase dello sviluppo il cui tempo è determinato dal `tempo di comparsa` x `il fattore moltiplicativo` impostato.

Ogni catena di sviluppo carta / developer ha un suo fattore moltiplicativo che deve essere determinato sperimentalmente dallo stampatore.

Al termine dello sviluppo il timer passerà ai bagni successivi se abilitati nelle impostazioni (Arresto, Fissaggio 1, Fissaggio 2).

||Azione|Descrizione|
|-------|------|--------|
|Display||**F.** `fattore moltiplicatore`
|Encoder|Rotazione|Aumenta / diminuisce moltiplicatore|
|Bottone 1|Click|se timer fermo **avvia countup**|
||Click|se timer avviato in countup **avvia countdown**|
||Click|se timer avviato in countdown **pause / resume**|
||Click lungo|se timer avviato in countdown **cancel**|
|Bottone 2|Click|Focus **ON / OFF**|

## Modalità **Sviluppo Lineare**

In questa modalità il timer è utilizzabile per lo sviluppo della carta nel classico processo a 3 bacinelle.

A differenza del metodo **fattoriale** questa modalità prevede un semplice timer count down dove sono impostati i secondi per il bagno di sviluppo.

Al termine il timer passerà ai bagni successivi se abilitati nelle impostazioni (Arresto, Fissaggio 1, Fissaggio 2).

||Azione|Descrizione|
|-------|------|--------|
|Display||**S.** `tempo di sviluppo in sec.`
|Encoder|Rotazione|Aumenta / diminuisce tempo di sviluppo|
|Bottone 1|Click|se timer fermo **avvia countdown**|
||Click|se timer avviato in countdown **pause / resume**|
||Click lungo|se timer avviato in countdown **cancel**|

## Modalità **Provinatore F-STOP**

Il provinatore F-Stop serve a realizzare il provino scalare e determinare il tempo per ottenere una stampa di lavoro.

La provinatura di solito avviene tramite l'ausilio di un provinatore provvisto di una finestrella oppure semplicemente facendo scorrere un cartoncino scuro coprendo il foglio che si sta stampando.

Nel primo caso il tempo di attivazione dell'ingranditore per ogni striscia è determinato dal **tempo base** più il tempo relativo al nr di provino ed allo scostamento in STOP impostato (vedere parametro  `Pr. F-STOP`).

Nel secondo caso il tempo e determinato come *delta* fra il tempo di esposizione della striscia precedente ed il **tempo base** più il tempo relativo al nr di provino ed allo scostamento in STOP impostato (vedere parametro  `Pr. F-STOP`).

Il nr di strisce per ogni provino può essere preimpostato oppure determinato dinamicamente interrompendo la procedura semplicemente con la pressione del **Bottone 1**.

||Azione|Descrizione|
|-------|------|--------|
|Display||**P.** `tempo base per striscia provino in sec.`
|Encoder|Rotazione|Aumenta / diminuisce tempo base|
|Bottone 1|Click|se timer fermo **avvia provinatura**|
||Click|se timer avviato **arresta la provinatura** (al termine del tempo)|
|Bottone 2|Click|Focus **ON / OFF**|

## Modalità **Metronomo**

In questa modalità il timer funziona come semplice metronomo emettendo da 1 a max 4 beep al secondo.

||Azione|Descrizione|
|-------|------|--------|
|Display||**b.** `nr beep al sec.`
|Encoder|Rotazione|Aumenta / diminuisce il nr. di beep al secondo da 1...4|
|Bottone 1|Click|se metronomo fermo **avvia metronomo**|
||Click|se metronomo avviato **arresta metronomo**|
|Bottone 2|Click|Focus **ON / OFF**|

# Impostazioni

## Navigazione

||Azione|Descrizione|
|-------|------|--------|
|Bottone 3|Click|Scorre i parametri delle impostazioni in sequenza|
||Click lungo|Salva ed esce dalla modalità impostazioni|

## Parametri

|Nome parametro|Descrizione parametro|Valore|Descrizione valore|
|--------------|---------------------|------|------------------|
|Pr. F-STOP|Provinatore F-STOP|Lin.|Lineare|
|||F. 1-1 ... F. 1-10|Incremento in stop del tempo per striscia provinata|
|Pr. Nr|Numero di strisce per provino|Man.|Manuale (Bottone 1 per interrompere)|
|||1 ... 20|Nr. di strice per provino (la provinatura si ferma automaticamente)|
|Pr. t.Calc|Modalità di calcolo del tempo per striscia|Sing.|Singola (viene usato un provinatore localizzato con una finestrella)|
|||Diff.|Come differenza dalla striscia precedente (la provinatura viene effettuata nascondendo progressivamente il foglio)|
|Pr. Pausa|Tempo di pausa in sec. fra una striscia e l'altra|OFF|Nessuna pausa|
|||Man.|Avanzamento manuale (Bottone 1 per avanzare)|
|||1 ... 10|Tempo in secondi|
|Dopo STAMPA|Quale modalità attivare al termine della stampa|STOP|Ferma il timer|
|||SVIL.|Entra in modalità SVILUPPO|
|Tipo SVILUPPO|Tipo sviluppo|FATT.|Fattoriale|
|||LIN.|Lineare|
|T. STOP|Tempo bagno di stop|OFF.|Non effettuare il bagno|
|||1 ... 999|Tempo in secondi|
|T. FISSAGGIO|Tempo bagno di fissaggio 1|OFF.|Non effettuare il bagno|
|||1 ... 999|Tempo in secondi|
|T. FISSAGGIO 2|Tempo bagno di fissaggio 2|OFF.|Non effettuare il bagno|
|||1 ... 999|Tempo in secondi|
|Dopo SVILUPPO|Quale modalità attivare al termine dello sviluppo|STOP|Ferma il timer e attiva la modalità iniziale|
|||CRON.|Avvia un cronometro per ulteriori bagni custom|
|T. PAUSA|Tempo di pausa fra un bagno e l'altro|OFF.|Disabilita la pausa|
|||MAN.|Avanzamento manuale (Bottone 1 per avanzare)|
|||1 ... 30|Tempo in secondi|
|BEEP|Avvisi acustici|SI|Avvisi acustici attivati|
|||NO|Nessun beep attivato|
|||PRIN.|Solo i beep principali|
|LUMINOSITA|Livello di luminosità del display|1 ... 7|Da meno a più luminoso|
|STATS|Visualizza alcuni dati statistici|M.|Nr. di minuti di accensione per la lampada|
|||C.|Numero di accensioni della lampada|
|RESET|Reset impostazioni e/o statistiche|NO|Non resettare|
|||PARA.|Resetta tutti le impostazioni ai valori di default|
|||STAT.|Azzera i contatori statistici|
|||TUTT.|Resetta sia i parametri che le statistiche|
||||Il **reset** avviene al salvataggio / uscita dalle impostazioni|



