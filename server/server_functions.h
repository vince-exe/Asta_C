#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>

#include "../utils/functions.h"
#include "../utils/constants.h"
#include "../utils/structs.h"

#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

/**
 * @brief funzione che alloca l'username del nuovo utente connesso all'interno dell'array di utenti
 * 
 * @param userArray array di utenti
 * @param username username da allocare
 */
void allocateUsername(UserArray* userArray, const char* username);

/**
 * @brief prende in input le informazioni necessaria per avviare l'asta ed il server
 * 
 * @param serverPort la porta sulla quale il server verrà hostato
 * @param AstaVariables struttura dati che gestisce le variabili dell'asta
 */
void takeInput(int* serverPort, AstaVariables* AstaVariables);

/**
 * @brief Funzione che controlla se l'username e' gia' presente tra i vari client.
 *
 * @param user_array: Array di 'User' dove controllare se e' presente 'newNickname'.
 * @param newNickname: Nickname da controllare.
 *
 * @return _Bool 0: Se il 'newNickname' non e' presente tra i nickanme di 'user_array'.
 * @return _Bool 1: Se il 'newNickname' e' presente tra i nickanme di 'user_array'.
 */
int existUsername(UserArray* user_array, const char* newNickname);

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
int sendToAllAsta(UserArray* userArray, SendAsta* data, const int msgType_);

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
int sendToAll(UserArray* userArray, const char* message, const int msgType_);

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
int shiftArray(UserArray* userArray, const char* username);

/**
 * @brief funzione che gestisce il singolo thread del client
 * 
 * @param paramater parametri del thread
 * @return DWORD 
 */
DWORD WINAPI handleClient(LPVOID paramater);

/**
 * @brief funzione che gestisce l'uscita di un client dall'asta
 * 
 * @param param i parametri del thread
 * @param currentUser l'utente attuale del thread
 * @param sendAsta struttura che gestisce le informazioni sull'asta
 */
void clientQuit(HandleClientParams* param, User* currentUser, SendAsta* sendAsta);

/**
 * @brief funzione che gestisce i turni dell'asta
 * 
 * @param astaVariables struttura che contiene le variabili dell'asta
 * @param user_array array di utenti
 * @param sendAsta struttura che contiene informazioni sull'asta
 * @param check variabile che gestisce l'incremento turni
 * @return 0: se fallisce 1: se è ok 
 */
int handleTurn(AstaVariables* astaVariables, UserArray* user_array, SendAsta* sendAsta, int check);

#endif //SERVER_FUNCTIONS_H