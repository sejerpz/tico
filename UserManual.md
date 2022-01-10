# Manuale TiCO Timer

![alt TiCo Front Panel](https://raw.githubusercontent.com/sejerpz/tico/main/docs/TiCo%20Front%20Panel.svg)

||Azione|Descrizione|
|-------|------|--------|
|Bottone 3|Click|Cambio modalità timer: Ingranditore, Sviluppo, Provinatore, Metronomo|
||Click lungo|Accesso alle impostazioni|


## Modalità **Ingranditore**

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

||Azione|Descrizione|
|-------|------|--------|
|Display||**S.** `tempo di sviluppo in sec.`
|Encoder|Rotazione|Aumenta / diminuisce tempo di sviluppo|
|Bottone 1|Click|se timer fermo **avvia countdown**|
||Click|se timer avviato in countdown **pause / resume**|
||Click lungo|se timer avviato in countdown **cancel**|

## Modalità **Provinatore F-STOP**

||Azione|Descrizione|
|-------|------|--------|
|Display||**P.** `tempo base per striscia provino in sec.`
|Encoder|Rotazione|Aumenta / diminuisce tempo base|
|Bottone 1|Click|se timer fermo **avvia provinatura**|
||Click|se timer avviato **arresta la provinatura** (al termine del tempo)|
|Bottone 2|Click|Focus **ON / OFF**|

## Modalità **Metronomo**

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



