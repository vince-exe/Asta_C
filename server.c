#include <stdio.h>
#include <winsock2.h>


/*
SOCK_STREAM: INT(1)
Tipo di socket che fornisce flussi di byte sequenziati, 
affidabili, bidirezionali e basati sulla connessione con un meccanismo di trasmissione dei dati OOB. 
Questo tipo di socket usa il protocollo TCP (Transmission Control Protocol) per la famiglia di indirizzi Internet (AF_INET o AF_INET6).

WSAStartup() INCL_WINSOCK_API_PROTOTYPES (Carica la versi)
socket()
*/
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "./functions/functions.h"

/*
 * Inizio parte dove si definiscono le costanti
*/

/*
 * Numero minimo di client per iniziare l'asta.
*/
#define MIN_ASTA 1

/*
 * Inizio costanti relative al numero dei client.
*/

/*
 * Numero minimo di client selezionabili.
*/
#define MIN_CLIENT 1

/*
 * Numero massimo di client selezionabili.
*/
#define MAX_CLIENT 100

/*
 * Fine costanti relative al numero dei client.
*/

/*
 * Lunghezza massima del buffer.
*/
#define BUFFER_LEN 1024

/*
 * Fine parte dove si definiscono le costanti.
*/

typedef struct User {
    char* username;
    SOCKET socket;
}User;

typedef struct UserArray {
    User* user_array;
    int counter;
}UserArray;

typedef struct AstaVariables {
    int max_clients;
    int min_clients_for_asta;
    int asta_import;
}AstaVariables;

int existUsername(UserArray* user_array, const char* newNickname) {
    for(int i = 0; i < user_array->counter; i++) {
        if(strcmp((user_array->user_array[i].username), newNickname) == 0) {
            return 1;
        }
    }

    return 0;
}

int main() {
    WSADATA wsa;
    SOCKET serverSocket;
    struct sockaddr_in server, client;
    
    AstaVariables AstaVariables;

    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
    
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d" , WSAGetLastError());
        return 1;
    }
    /*
    CLEAR_SCREEN();
    printf("Inserisci la porta (%d, %d): ", MIN_PORT, MAX_PORT);
    int server_port = get_int(MIN_PORT, MAX_PORT);
    CLEAR_SCREEN();

    printf("\nInserisci il numero di client (%d, %d): ", MIN_CLIENT, MAX_CLIENT);
    AstaVariables.max_clients = get_int(MIN_CLIENT, MAX_CLIENT);
    CLEAR_SCREEN();

    printf("\nInserisci il numero minimo di client per iniziare l'asta (%d, %d): ", MIN_CLIENT, MAX_CLIENT);
    AstaVariables.min_clients_for_asta = get_int(MIN_CLIENT, MAX_CLIENT);
    CLEAR_SCREEN();

    printf("\nInserisci l'importo di partenza dell'asta (min: %d): ", MIN_ASTA);
    AstaVariables.asta_import = get_int(MIN_ASTA, MAX_PORT);
    CLEAR_SCREEN();
    */
    
    int server_port = 8888;
    AstaVariables.max_clients = 5; AstaVariables.asta_import = 20; AstaVariables.min_clients_for_asta = 2; 
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(server_port);

    if (bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d" , WSAGetLastError());
        return 1;
    }
    
    printf("Server listening on port %d", server_port);
    /* inizializzo la struttura dati che contiene un array di user ed un counter */
    UserArray user_array = { malloc(sizeof(User) * AstaVariables.max_clients), 0 };

    /* il secondo argomento sono il numero di client che possono stare in coda di connessione prima di essere accettati.*/
    listen(serverSocket, AstaVariables.max_clients);

    int c = sizeof(struct sockaddr_in);
    int recv_size;
    char message[BUFFER_LEN];
    
    while(user_array.counter < AstaVariables.max_clients) {
        user_array.user_array[user_array.counter].socket = accept(serverSocket, (struct sockaddr *)&client, &c);

        if (user_array.user_array[user_array.counter].socket == INVALID_SOCKET) {
            printf("accept failed with error code : %d" , WSAGetLastError());
            return 1;
        }
        
        message[0] = '\0';
        if((recv_size = recv(user_array.user_array[user_array.counter].socket, message, sizeof(message), 0)) == SOCKET_ERROR) {
            printf("recv failed with error code : %d", WSAGetLastError());
        }
        message[recv_size] = '\0';
        /* controllo se il nickname esiste già*/
        if((user_array.counter != 0) && existUsername(&user_array, message)) {
            if(send(user_array.user_array[user_array.counter].socket, NICK_ALREADY_IN_USE, strlen(NICK_ALREADY_IN_USE), 0) < 0) {
                puts("Send Failed");
                return 1;
            }
        }
        /* altrimenti alloco l'username */
        else {
            user_array.user_array[user_array.counter].username = malloc(sizeof(char) * strlen(message) + 1);
            strcpy(user_array.user_array[user_array.counter].username, message);
            user_array.user_array[user_array.counter].username[strlen((message)) + 1] = '\0';
            printf("\nClient %s connesso (%d di %d)", message, user_array.counter + 1, AstaVariables.max_clients);
            user_array.counter++;
        }
        if(user_array.counter == AstaVariables.min_clients_for_asta) {
            /* dare il messaggio che l'asta è iniziata */
            printf("\nAsta Iniziata");
        }
        else {
            message[recv_size] = '\0';
            if(send(user_array.user_array[user_array.counter - 1].socket, WAIT_FOR_ASTA, strlen(WAIT_FOR_ASTA) + 1, 0) < 0) {
                puts("Send Failed");
                return 1;
            }
        }
    }   

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}