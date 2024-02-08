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
 * Includere il file di intestazione `windows.h`, che contiene le dichiarazioni
 * delle funzioni e delle macro utilizzate per la programmazione in Windows.
 */
#include <windows.h>

/*
 * Includere il file di intestazione `winsock2.h`, che contiene le dichiarazioni
 * delle funzioni e delle macro utilizzate per la programmazione delle socket in
 * Windows.
 */
#include <winsock2.h>

/*
 * Include il file di intestazione `functions.h`, che contiene le definizioni delle
 * funzioni utilizzate dal programma.
 */
#include "../utils/functions.h"

/*
 * Include il file di intestazione `constants.h`, che contiene le definizioni delle
 * constantnti utilizzate dal programma.
 */
#include "../utils/constants.h"

/*
 * Include il file di intestazione `structs.h`, che contiene le definizioni delle
 * strutture utilizzate dal programma.
 */
#include "../utils/structs.h"

#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

/**
 * @brief Funzione che controlla se l'username e' gia' presente tra i vari client.
 *
 * @param user_array: Array di 'User' dove controllare se e' presente 'newNickname'.
 * @param newNickname: Nickname da controllare.
 *
 * @return _Bool 0: Se il 'newNickname' non e' presente tra i nickanme di 'user_array'.
 * @return _Bool 1: Se il 'newNickname' e' presente tra i nickanme di 'user_array'.
 */
_Bool existUsername(UserArray* user_array, const char* newNickname);

/**
 * @brief Procedura che chiude tutti i socket presenti all'interno di un 'UserArray'.
 * 
 * @param userArray: Array dove sono contenuti i socket da chiudere.
 */
void closeAllSocket(UserArray* userArray);

/**
 * @brief Funzione che manda a tutti i socket presenti in 'userArray' un tipo 'SendAsta'
 * ed il tipo di messaggio 'msgType_'.
 *
 * @param userArray: Array di 'User' a cui mandare il messaggio.
 * @param data: Dati da inviare.
 * @param msgType_: Tipo del messaggio.
 *
 * @return _Bool 0: Se ci sono stati errori.
 * @return _Bool 1: Se non ci sono stati errori.
 */
_Bool sendToAllAsta(UserArray* userArray, SendAsta* data, const int msgType_);

/**
 * @brief Funzione che manda a tutti i socket presenti in 'userArray' un messaggio.
 * 
 * @param userArray: Array di 'User' a cui mandare il messaggio.
 * @param message: Messaggio da inviare.
 * @param msgType_: Tipo del messaggio.
 * 
 * @return _Bool 0: Se ci sono stati errori.
 * @return _Bool 1: Se non ci sono stati errori.
 */
_Bool sendToAll(UserArray* userArray, const char* message, const int msgType_);

/**
 * @brief Restituisce la posizione di un elemento in 'user_array' in base al 'username'.
 * 
 * @param userArray: Array di 'User'.
 * @param username: Lo 'username' da cercare.
 * 
 * @return size_t: Posizione di 'username'.
 */
size_t get_pos(UserArray* userArray, const char* username);

/**
 * @brief Procedura che rimuove un 'User' da 'userArray'.
 * 
 * @param userArray: Array di 'User'.
 * @param username: Lo 'username' dello 'User' da eliminare.
 * 
 * @return _Bool 0: Se non è stato eliminato nessuno.
 * @return _Bool 1: Se è stato eliminato il giosto 'User'.
 */
_Bool shiftArray(UserArray* userArray, const char* username);

#endif //SERVER_FUNCTIONS_H