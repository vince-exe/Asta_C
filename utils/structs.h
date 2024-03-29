#include <windows.h>
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
    size_t counter;
} UserArray;

/* 
 * Definizione di un tipo chiamato 'AstaVariables' che rappresenta le variabili di un'asta.
 * num_clients: Numero client per avviare l'asta.
 * asta_import: Importo iniziale dell'asta.
 * asta_turn: Turno dell'asta.
 */
typedef struct AstaVariables {
    size_t num_clients;
    size_t asta_import;
    size_t asta_turn;
} AstaVariables;

/* 
 * Definizione di un tipo chiamato 'SendAsta' che rappresenta i parametri che il server passa al client.
 * import: Importo a cui sta attualmente l'asta.
 * nickname_turn: Nickname dell'utante a cui tocca impostare una nuova cifra.
 * message_turn: Messaggio del sever.
 */
typedef struct SendAsta {
    size_t import;
    char nickname_turn[255];
    char message_turn[255];
}SendAsta;

/* 
 * Definizione di un tipo chiamato 'HandleClientParams' che rappresenta i parametri del thread.
 * user: Puntatore al tipo 'User'.
 * user_array: Array di 'User'.
 * astaVariables: Puntatore al tipo 'AstaVariables'.
 * socketServer: Socket del server.
 */
typedef struct HandleClientParams {
    User* user;
    UserArray* user_array;
    AstaVariables* astaVariables;
    SOCKET socketServer;
}HandleClientParams;

/* 
 * Definizione di un tipo chiamato 'InputClient' che rappresenta i parametri di input del client.
 * msgType: Tipo del messaggio.
 * import: Importo inserito del client.
 */
typedef struct InputClient {
    int msgType;
    size_t import;
}InputClient;

#endif // STRUCTS_H