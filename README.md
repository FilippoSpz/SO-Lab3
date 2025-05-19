# Progetto: Parallel

## Descrizione
Il progetto **Parallel** è una versione semplificata del comando GNU `parallel`, implementata in linguaggio C. Questo programma consente di eseguire in parallelo una sequenza di comandi Bash, sfruttando le capacità di un sistema multicore. 

## Funzionalità
Il programma accetta tre argomenti:
1. **Nome del file**: Un file di testo contenente, una per riga, i parametri che verranno utilizzati per eseguire i comandi.
2. **Numero di processi**: Un numero intero che specifica quanti processi in parallelo devono essere eseguiti.
3. **Comando da eseguire**: Una stringa che rappresenta il comando Bash da eseguire. La stringa deve contenere il carattere `%`, che verrà sostituito con i parametri letti dal file.

Il programma legge i parametri dal file, li sostituisce al carattere `%` nella stringa del comando e distribuisce i comandi tra i processi figli, che li eseguono in parallelo.

## Dettagli di Implementazione
- **Creazione dei processi**: Il programma utilizza la funzione `fork()` per creare un numero di processi figli pari al numero specificato dall'utente.
- **Comunicazione tramite pipe**: Il processo padre utilizza delle pipe per inviare i comandi ai processi figli. Ogni processo figlio legge i comandi dalla pipe a lui associata.
- **Esecuzione dei comandi**: I processi figli eseguono i comandi ricevuti utilizzando la funzione `system()`.
- **Gestione delle pipe**: Le pipe vengono chiuse correttamente per evitare blocchi. Quando il padre termina di inviare i comandi, chiude le estremità di scrittura, permettendo ai figli di rilevare la fine dei dati.
- **Sincronizzazione**: Il processo padre attende la terminazione di tutti i processi figli utilizzando la funzione `wait()`.

## Struttura del Codice
- **`main.c`**: Contiene l'intera implementazione del programma, inclusa la gestione degli argomenti, la creazione dei processi, la comunicazione tramite pipe e l'esecuzione dei comandi.
- **Macro definite**:
  - `MAXPROCESSES`: Numero massimo di processi supportati (256).
  - `MAXCMD`: Lunghezza massima di un comando (256 caratteri).

## Requisiti
- **Linguaggio**: C (standard C11).
- **Compilatore**: GCC o compatibile.
- **CMake**: Utilizzato per configurare il progetto.

## Configurazione del Progetto
Il file `CMakeLists.txt` è configurato per compilare il progetto:
```cmake
cmake_minimum_required(VERSION 3.30)
project(untitled1 C)

set(CMAKE_C_STANDARD 11)

add_executable(untitled1 main.c)
