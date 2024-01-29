/*
 * Include il file di intestazione `windows.h`, che contiene le definizioni delle
 * funzioni e delle macro utilizzate per la programmazione delle finestre in Windows.
 */
#include <windows.h>

/*
 * Includere il file di intestazione `winsock2.h`, che contiene le dichiarazioni
 * delle funzioni e delle macro utilizzate per la programmazione delle socket in
 * Windows.
 */
#include <winsock2.h>

#ifndef STRUCTS_H
#define STRUCTS_H

/* 
 * Definizione di un tipo chiamato 'User' che rappresenta un utente.
 * username: Username di un utente.
 * socket: Socket relativo ad un utente.
 */
typedef struct User {
    char* username;
    SOCKET socket;
} User;

/* 
 * Definizione di un tipo chiamato 'UserArray' che rappresenta un array di 'User'.
 * user_array: Array di 'User'.
 * counter: Contatore relativo ad 'user_array'.
 */
typedef struct UserArray {
    User* user_array;
    int counter;
} UserArray;

/* 
 * Definizione di un tipo chiamato 'AstaVariables' che rappresenta le variabili di un'asta.
 * min_clients: Numero minimo di client per avviare l'asta.
 * max_clients: Numero massimo di client per l'asta.
 * asta_import: Importo iniziale dell'asta.
 */
typedef struct AstaVariables {
    int max_clients;
    int asta_import;
    int asta_turn;
} AstaVariables;

/* 
 * Definizione di un tipo chiamato 'SendAsta' che rappresenta i parametri che il server passa al client.
 * import: Importo a cui sta attualmente l'asta.
 * nickname_turn: Nickname dell'utante a cui tocca impostare una nuova cifra.
 * message_turn: Messaggio del sever.
 */
typedef struct SendAsta {
    int import;
    char nickname_turn[255];
    char message_turn[255];
}SendAsta;

/* 
 * Definizione di un tipo chiamato 'HandleClientParams' che rappresenta i parametri del thread.
 * user: Puntatore al tipo 'User'.
 */
typedef struct HandleClientParams {
    User* user;
}HandleClientParams;

#endif // STRUCTS_H
