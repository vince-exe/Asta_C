/*
 * Includere il file di intestazione `stdio.h`, che contiene le dichiarazioni
 * delle funzioni e delle macro utilizzate per l'input/output standard (I/O).
 */
#include <stdio.h>

/*
 * Includere il file di intestazione `stdlib.h`, che contiene le
 * definizioni delle funzioni di gestione della memoria dinamica.
 */
#include <stdlib.h>

/*
 * Include il file di intestazione `string.h`, che contiene le
 * definizioni delle funzioni di gestione delle stringhe.
 */
#include <string.h>

/*
 * Include il file di intestazione `structs.h`, che contiene le definizioni delle
 * costanti utilizzate dal programma.
 */
#include "./constants.h"

/*
 * Include il file di intestazione `structs.h`, che contiene le definizioni delle
 * strutture utilizzate dal programma.
 */
#include "./structs.h"

/*
 * Includere il file di intestazione `winsock2.h`, che contiene le dichiarazioni
 * delle funzioni e delle macro utilizzate per la programmazione delle socket in
 * Windows.
 */
#include <winsock2.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

/*
 * Funzione che pulisce il terminale.
 */
#define CLEAR_SCREEN() system("cls")

/**
 * @brief Funzione che controlla se si inserisce un intero.
 *
 * @param min: Numero piu' piccolo che si puo' inserire.
 * @param max: Numero piu' grande che si puo' inserire.
 * 
 * @return int: Il numero inserito.
 */
int get_int(const int min, const int max);

/**
 * @brief Funzione che stampa un messaggio e chiude il socket.
 *
 * @param socket: Socket da chiudere.
 * @param message: Messaggio da stampare.
 * @param file_: File.
 * @param line_: Linea di codice.
 */
void closeSocket(SOCKET socket, const char* message, const char* file_, const int line_);

/**
 * @brief Funzione che stampa un messaggio e chiude il socket.
 *
 * @param client: Il socket a cui verra' mandato intero.
 * @param integer: Stringa da mandare.
 * @param msgType_: Tipo del messaggio.
 *
 * @return 0: Se hanno fallito.
 * @return 1: Se e' andato tutto a buon fine.
 */
_Bool sendTo(SOCKET client, const char* message, int msgType_);

#endif // FUNCTIONS_H
