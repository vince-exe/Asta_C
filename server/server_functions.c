/*
 * Include il file di intestazione `server_functions.h`, che contiene le definizioni delle
 * funzioni.
 */
#include "server_functions.h"

/**
 * @brief Funzione che controlla se l'username e' gia' presente tra i vari client.
 *
 * @param user_array: Array di 'User' dove controllare se e' presente 'newNickname'.
 * @param newNickname: Nickname da controllare.
 *
 * @return 0: Se il 'newNickname' non e' presente tra i nickanme di 'user_array'.
 * @return 1: Se il 'newNickname' e' presente tra i nickanme di 'user_array'.
 */
_Bool existUsername(UserArray* user_array, const char* newNickname) {
    for(size_t i = 0; i < user_array->counter; i++) {
        if(strcmp((user_array->user_array[i].username), newNickname) == 0) {
            return (_Bool)1;
        }
    }

    return (_Bool)0;
}

/**
 * @brief Procedura che chiude tutti i socket presenti all'interno di un 'UserArray'.
 * 
 * @param userArray: Array dove sono contenuti i socket da chiudere.
 */
void closeAllSocket(UserArray* userArray){
    for(size_t i = 0; i < userArray->counter; i++){
        if (userArray->user_array[i].socket != INVALID_SOCKET) {
            closesocket(userArray->user_array[i].socket);
        }

        free(userArray->user_array[i].username);
    }
}

/**
 * @brief Funzione che manda a tutti i socket presenti in 'userArray' un tipo 'SendAsta'
 * ed il tipo di messaggio 'msgType_'.
 *
 * @param userArray: Array di 'User' a cui mandare il messaggio.
 * @param data: Dati da inviare.
 * @param msgType_: Tipo del messaggio.
 *
 * @return 0: Se ci sono stati errori.
 * @return 1: Se non ci sono stati errori.
 */
_Bool sendToAllAsta(UserArray* userArray, SendAsta* data, const int msgType_) {
    int msgType = htonl(msgType_);

    for (size_t i = 0; i < userArray->counter; i++) {
        if ((send(userArray->user_array[i].socket, (char*)&msgType, sizeof(msgType), 0) < 0) ||
            (send(userArray->user_array[i].socket, (char*)data, sizeof(SendAsta), 0) < 0)) {
            return (_Bool)0;
        }
    }

    return (_Bool)1;
}

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
_Bool sendToAll(UserArray* userArray, const char* message, const int msgType_) {
    int msgType = htonl(msgType_);

    for(size_t i = 0; i < userArray->counter; i++) {
        if((send(userArray->user_array[i].socket, (char*)&msgType, sizeof(msgType), 0) < 0) ||
           (send(userArray->user_array[i].socket, message, strlen(message) + 1, 0) < 0)) {
           return (_Bool)0;
        };
    }
    
    return (_Bool)1;
}

/**
 * @brief Restituisce la posizione di un elemento in 'user_array' in base al 'username'.
 * 
 * @param userArray: Array di 'User'.
 * @param username: Lo 'username' da cercare.
 * 
 * @return int: Posizione di 'username'.
 */
size_t get_pos(UserArray* userArray, const char* username) {
    if (username == NULL || strlen(username) == 0) {
        return NOT_FOUND;
    }
    
    for(size_t i = 0; i < userArray->counter; i++) {
        if(strcmp(userArray->user_array[i].username, username) == 0) {
            return i;
        }
    }

    return NOT_FOUND;
}

/**
 * @brief Procedura che rimuove un 'User' da 'userArray'.
 * 
 * @param userArray: Array di 'User'.
 * @param username: Lo 'username' da eliminare.
 */
_Bool shiftArray(UserArray* userArray, const char* username) {
    size_t pos = get_pos(userArray, username);

    if(pos == NOT_FOUND) {
        return (_Bool)0;
    }

    for(size_t i = pos; i < userArray->counter; i++) {
        userArray->user_array[i] = userArray->user_array[i + 1];
    }

    userArray->counter--;
    return (_Bool)1;
}