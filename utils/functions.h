#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#include "./constants.h"
#include "./structs.h"

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
int sendTo(SOCKET client, const char* message, int msgType_);

#endif // FUNCTIONS_H