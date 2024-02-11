#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>

#include "../utils/functions.h"
#include "../utils/constants.h"
#include "../utils/structs.h"
#include "server_functions.h"

/*
 * La libreria ws2_32.lib contiene le funzioni e le variabili necessarie per la programmazione
 * di rete in Windows utilizzando l'API Winsock.
 */
#pragma comment(lib,"ws2_32.lib") // Collega la libreria in fase di runtime.

int main() {
    WSADATA wsa;
    SOCKET serverSocket;
    struct sockaddr_in server, client;
    AstaVariables AstaVariables;

    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        fprintf(stderr, "\nFailed. Error Code : %d",WSAGetLastError());
        return 1;
    }    
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        fprintf(stderr, "\nCould not create socket : %d" , WSAGetLastError());
        WSACleanup();
        return 1;
    }
    
    int serverPort;
    /* prende informazioni come: porta del server; importo iniziale e numeri di giocatori massimi */
    takeInput(&serverPort, &AstaVariables);
    AstaVariables.asta_turn = 0;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("0.0.0.0");
    server.sin_port = htons(serverPort);

    if (bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        fprintf(stderr, "\nBind failed with error code : %d" , WSAGetLastError());
        WSACleanup();
        return 1;
    }
    
    CLEAR_SCREEN();
    printf("Server listening on port %d", serverPort);
    /* inizializzo la struttura dati che contiene un array di user ed un counter */
    UserArray user_array = { malloc(sizeof(User) * AstaVariables.num_clients), 0 };
    /* array di thread */
    HANDLE *thread_array = malloc(sizeof(HANDLE) * AstaVariables.num_clients);
    
    /* il secondo argomento sono il numero di client che possono stare in coda di connessione prima di essere accettati.*/
    listen(serverSocket, AstaVariables.num_clients);

    int c = sizeof(struct sockaddr_in);
    int recv_size, msgType;
    char message[BUFFER_LEN], tmp[BUFFER_LEN];
    
    while(user_array.counter < AstaVariables.num_clients) {
        user_array.user_array[user_array.counter].socket = accept(serverSocket, (struct sockaddr *)&client, &c);
        if (user_array.user_array[user_array.counter].socket == INVALID_SOCKET) {
            fprintf(stderr, "\naccept failed with error code : %d" , WSAGetLastError());
            WSACleanup();
            return 1;
        }

        message[0] = '\0';
        if((recv_size = recv(user_array.user_array[user_array.counter].socket, message, sizeof(message), 0)) == SOCKET_ERROR) {
            fprintf(stderr, "\nrecv failed with error code : %d", WSAGetLastError());
            continue;
        }
        message[recv_size] = '\0';
        /* controllo se il nickname esiste gia' */
        if((user_array.counter != 0) && existUsername(&user_array, message)) {
            msgType = htonl(ASTA_STATUS);

            if ((send(user_array.user_array[user_array.counter].socket, (char*)&msgType, sizeof(msgType), 0) < 0) ||
                (send(user_array.user_array[user_array.counter].socket, NICK_ALREADY_IN_USE, strlen(NICK_ALREADY_IN_USE), 0) < 0)) {

                closeAllSocket(&user_array);
                closeSocket(serverSocket, "\nChiususa del server dovuta ad errore di spedizione", __FILE__, __LINE__);
                WSACleanup();
                return 1;
            }
        }
        /* altrimenti alloco l'username*/
        else {
            allocateUsername(&user_array, message);
            printf("\nClient %s connesso (%d di %d)", message, user_array.counter + 1, AstaVariables.num_clients);
            sprintf(tmp, "Giocatore %s connesso (%d di %d)", message, user_array.counter + 1, AstaVariables.num_clients);

            if(!sendToAll(&user_array, tmp, GENERAL_MESSAGE)) {
                closeAllSocket(&user_array);
                closeSocket(serverSocket, "\nChiususa del server dovuta ad errore di spedizione", __FILE__, __LINE__);
                WSACleanup();
                return 1;
            };
            /* dichiaro ed inizializzo ogni volta la variabile params che punta al singolo utente e poi la passo al thread*/
            HandleClientParams params = {&user_array.user_array[user_array.counter], &user_array, &AstaVariables, serverSocket};

            /* aggiungo al vettore di thread il singolo thread che gestira' il client */
            thread_array[user_array.counter] = CreateThread(NULL, 0, handleClient, &params, 0, NULL);
            if(thread_array[user_array.counter] == NULL) {
                closeAllSocket(&user_array);
                char tmp[BUFFER_LEN];
                sprintf(tmp, "\nErrore nella creazione del thread utente %s", user_array.user_array[user_array.counter].username);
                closeSocket(serverSocket, tmp, __FILE__, __LINE__);
                WSACleanup();
                return 1;
            }
            user_array.counter++;

            /* il server comunica a tutti i client che l'asta e' iniziata */
            if(user_array.counter == AstaVariables.num_clients) {
                if(!sendToAll(&user_array, ASTA_STARTED, ASTA_STATUS)) {
                    closeAllSocket(&user_array);
                    closeSocket(serverSocket, "\nChiusura del server dovuta ad errore di spedizione", __FILE__, __LINE__);
                }
            }
            /* altrimenti comunica che devono aspettare */
            else {
                msgType = htonl(ASTA_STATUS);

                /* counter - 1 perche' altrimenti andrebbe a scrivere in una posizione errata */
                if( (send(user_array.user_array[user_array.counter - 1].socket, (char*)&msgType, sizeof(msgType), 0) < 0) ||
                    (send(user_array.user_array[user_array.counter - 1].socket, WAIT_FOR_ASTA, strlen(WAIT_FOR_ASTA) + 1, 0) < 0)) {

                    closeAllSocket(&user_array);
                    closeSocket(serverSocket, "\nChiususa del server dovuta ad errore di spedizione", __FILE__, __LINE__);

                    closesocket(serverSocket);
                    WSACleanup();
                    return 1;
                }
            }
        }
    }
    SendAsta sendAsta;
    sendAsta.import = AstaVariables.asta_import;
    
    if(!handleTurn(&AstaVariables, &user_array, &sendAsta, 0)) {
        closeAllSocket(&user_array);
        closeSocket(serverSocket, "\nIl server ha fallito nel mandare il messaggio di turno, There is nothing we can do...", __FILE__, __LINE__);
        return 1;
    }

    WaitForMultipleObjects(user_array.counter, thread_array, TRUE, INFINITE);
    printf("\nServer ha finito di aspettare per tutti i thread");
    for (int i = 0; i < user_array.counter; i++) {
        CloseHandle(thread_array[i]);
    }
    printf("\nAsta Finita");
    
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}