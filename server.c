#include <stdio.h>
#include <winsock2.h>

#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "./utils/functions.h"
#include "./utils/constants.h"
#include "./utils/structs.h"

#pragma comment(lib,"ws2_32.lib") // linka automaticamente la libreria in fase di runtime.

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
    for(int i = 0; i < user_array->counter; i++) {
        if(strcmp((user_array->user_array[i].username), newNickname) == 0) {
            return (_Bool)1;
        }
    }

    return (_Bool)0;
}

/* manda un messaggio a tutti i client */
_Bool sendToAll(UserArray* user_array, const char* message, int msgType_) {
    int msgType = htonl(msgType_);
    printf("\n%d counter: ", user_array->counter);
    for(int i = 0; i < user_array->counter; i++) {
        if((send(user_array->user_array[i].socket, (char*)&msgType, sizeof(msgType), 0) < 0) ||
           (send(user_array->user_array[i].socket, message, strlen(message) + 1, 0) < 0)) {
           return (_Bool)0;
        };
        printf("\nSend to all: %s", user_array->user_array[i].username);
    }

    return (_Bool)1;
}

/* TO-DO */
DWORD WINAPI handleClient(LPVOID paramater) {
    int recv_size = 0;
    char message[BUFFER_LEN];
    HandleClientParams* param = (HandleClientParams*)paramater;

    printf("\nStart listening thread on client %s", param->user->username);
    while((recv_size = recv(param->user->socket, message, sizeof(message) , 0)) != SOCKET_ERROR) {
        ;
    }
}

void closeAllSocket(UserArray *ptr){
    for(unsigned int i = 0; i < ptr->counter; i++) closesocket(ptr->user_array[i].socket);
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
    /* NON CANCELLARE ROCCIA
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
    AstaVariables.max_clients = 3; AstaVariables.asta_import = 20;
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
    /* array di thread */
    
    /* il secondo argomento sono il numero di client che possono stare in coda di connessione prima di essere accettati.*/
    listen(serverSocket, AstaVariables.max_clients);

    int c = sizeof(struct sockaddr_in);
    int recv_size, msgType;
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
            continue;
        }
        message[recv_size] = '\0';
        /* controllo se il nickname esiste gia' */
        if((user_array.counter != 0) && existUsername(&user_array, message)) {
            msgType = htonl(ASTA_MESSAGES);

            if ((send(user_array.user_array[user_array.counter].socket, (char*)&msgType, sizeof(msgType), 0) < 0) ||
                (send(user_array.user_array[user_array.counter].socket, NICK_ALREADY_IN_USE, strlen(NICK_ALREADY_IN_USE), 0) < 0)) {

                closeAllSocket(&user_array);
                closeSocket(serverSocket, "Chiususa del server dovuta ad errore di spedizione", __FILE__, __LINE__);
                return 1;
            }
        }
        /* altrimenti alloco l'username*/
        else {
            user_array.user_array[user_array.counter].username = malloc(sizeof(char) * strlen(message) + 1);
            strcpy(user_array.user_array[user_array.counter].username, message);
            user_array.user_array[user_array.counter].username[strlen((message)) + 1] = '\0';

            /* stampa sul server il messaggio che il nuovo client si e' unito e lo manda anche a tutti i client
               automaticamente l'ultimo arrivato e' escluso dal messaggio perche' il counter ancora deve essere incrementato
            */
            printf("\nClient %s connesso (%d di %d)", message, user_array.counter + 1, AstaVariables.max_clients);
            
            char tmp[255];
            sprintf(tmp, "Giocatore %s connesso (%d di %d)", message, user_array.counter + 1, AstaVariables.max_clients);
            if(!sendToAll(&user_array, tmp, GENERAL_MESSAGE)) {
                closeAllSocket(&user_array);
                closeSocket(serverSocket, "Chiususa del server dovuta ad errore di spedizione", __FILE__, __LINE__);
                return 1;
            };
            user_array.counter++;
            /* il server comunica a tutti i client che l'asta e' iniziata */
            if(user_array.counter == AstaVariables.max_clients) {
                if(!sendToAll(&user_array, ASTA_STARTED, ASTA_MESSAGES)) {
                    closeAllSocket(&user_array);
                    closeSocket(serverSocket, "Chiusura del server dovuta ad errore di spedizione", __FILE__, __LINE__);
                }
            }
            else {
                msgType = htonl(ASTA_MESSAGES);
                
                /* counter - 1 perche' altrimenti andrebbe a scrivere in una posizione errata */
                if( (send(user_array.user_array[user_array.counter - 1].socket, (char*)&msgType, sizeof(msgType), 0) < 0) ||
                    (send(user_array.user_array[user_array.counter - 1].socket, WAIT_FOR_ASTA, strlen(WAIT_FOR_ASTA) + 1, 0) < 0)) {
                    
                    closeAllSocket(&user_array);
                    closeSocket(serverSocket, "Chiususa del server dovuta ad errore di spedizione", __FILE__, __LINE__);
                    return 1;
                }
            }
        }
    }
    while(1){
        ;
    }
    printf("\nServer uscito dwal ciclo..");
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}