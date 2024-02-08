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

/*
 * Include il file di intestazione `server_functions.h`, che contiene le definizioni delle
 * funzioni utilizzate dal server.
 */
#include "server_functions.h"

/*
 * La libreria ws2_32.lib contiene le funzioni e le variabili necessarie per la programmazione
 * di rete in Windows utilizzando l'API Winsock.
 */
#pragma comment(lib,"ws2_32.lib") // Collega la libreria in fase di runtime.

_Bool handleTurn(HandleClientParams* param, SendAsta* sendAsta, _Bool check) {    
    if(param == NULL || sendAsta == NULL) {
        return (_Bool)0;
    }

    if(check) {
        if(param->astaVariables->asta_turn >= (param->user_array->counter - 1)) {
            param->astaVariables->asta_turn = 0;
        }
        else {
            param->astaVariables->asta_turn++;
        }
    }
    else {
        /* in caso l'ultimo quitta, parte dal primo */
        if(param->astaVariables->asta_turn >= (param->user_array->counter)) {
            param->astaVariables->asta_turn = 0;
        }
    }

    printf("\nTurno del giocatore: %d", param->astaVariables->asta_turn);

    sendAsta->import = param->astaVariables->asta_import;
    sprintf(sendAsta->message_turn, "Turno del giocatore [ %s ]", param->user_array->user_array[param->astaVariables->asta_turn].username);
    strncpy(sendAsta->nickname_turn, param->user_array->user_array[param->astaVariables->asta_turn].username, BUFFER_LEN);

    return sendToAllAsta(param->user_array, sendAsta, ASTA_MESSAGE);
}

DWORD WINAPI handleClient(LPVOID paramater) {
    int recv_size = 0, typeOfMsg = 0, clientImport = 0, isRunning = 1;
    char tmp[BUFFER_LEN];
    
    /* struttura dati che ci fa gestire i parametri passati al thread */
    HandleClientParams* param = (HandleClientParams*)paramater;
    /* struttura dati che conterra' l'input del socket */
    InputClient inputClient;
    SendAsta sendAsta;
    /* qui ci salviamo l'utente attuale essendo che paramater cambia ogni volta */
    User currentUser = *param->user;
    
    printf("\nStart listening thread on client %s\n", currentUser.username);
    while(isRunning) {
        /* ricevere la struttura */
        if((recv_size = recv(currentUser.socket, (char*)&inputClient, sizeof(InputClient), 0)) == SOCKET_ERROR) {
            sprintf(tmp, "Il giocatore %s ha abbandonato l'asta", currentUser.username);
            printf("\n%s", tmp);

            /* shiftare l'utente */
            if(!shiftArray(param->user_array, currentUser.username)){
                closeAllSocket(param->user_array);
                closeSocket(param->socketServer, "\nChiususa del server dovuta ad errore di incongruenza", __FILE__, __LINE__);
            }

            /* mandiamo il messaggio di quit */
            if(!sendToAll(param->user_array, tmp, GENERAL_MESSAGE)) {
                closeAllSocket(param->user_array);
                closeSocket(param->socketServer, "\nChiususa del server dovuta ad errore di spedizione ( client quit )", __FILE__, __LINE__);
                return 1;
            }
            
            if(param->user_array->counter == 1) {
                sprintf(tmp, ASTA_WON);
                if(!sendToAll(param->user_array, tmp, ASTA_STATUS)) {
                    closeAllSocket(param->user_array);
                    closeSocket(param->socketServer, "\nChiususa del server dovuta ad errore di spedizione ( client won )", __FILE__, __LINE__);
                    return 1;
                }
            }

            if(!handleTurn(param, &sendAsta, 0)) {
                closeAllSocket(param->user_array);
                closeSocket(param->socketServer, "\nIl server ha fallito nel mandare il messaggio di turno, There is nothing we can do...", __FILE__, __LINE__);
                return 1;
            }

            printf("\nEnd thread on client %s", currentUser.username);
            return 0;
        }

        switch(inputClient.msgType) {
            /* il server riceve l'importo dal client ( lo casta ad intero )*/
            case ASTA_IMPORT:
                sprintf(tmp, "Il giocatore %s ha puntato %d", currentUser.username, inputClient.import);
                printf("\n%s", tmp); 
                if(!sendToAll(param->user_array, tmp, GENERAL_MESSAGE)) {
                    closeAllSocket(param->user_array);
                    closeSocket(param->socketServer, "\nChiususa del server dovuta ad errore di spedizione", __FILE__, __LINE__);
                    return 1;
                }
                
                /* aggiorniamo l'importo dell'asta e mandiamo il prossimo turno */
                param->astaVariables->asta_import = inputClient.import;
                if(!handleTurn(param, &sendAsta, 1)) {
                    closeAllSocket(param->user_array);
                    closeSocket(param->socketServer, "\nIl server ha fallito nel mandare il messaggio di turno, There is nothing we can do...", __FILE__, __LINE__);
                    return 1;
                }

                break;
            
            case ASTA_WON_FROM_CLIENT:
                isRunning = 0;
                break;

            default:
                closeAllSocket(param->user_array);

                sprintf(tmp, "Messaggio dal client non identificato %d", typeOfMsg);
                closeSocket(param->socketServer, tmp, __FILE__, __LINE__);
                isRunning = 0;
                break;
        }
    }

    printf("\nEnd thread on client %s", currentUser.username);
    return 0;
}

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

    CLEAR_SCREEN();
    printf("Inserisci la porta (%d, %d): ", MIN_PORT, MAX_PORT);
    int server_port = get_int(MIN_PORT, MAX_PORT);
    CLEAR_SCREEN();

    printf("\nInserisci il numero di client (%d, %d): ", MIN_CLIENT, MAX_CLIENT);
    AstaVariables.num_clients = get_int(MIN_CLIENT, MAX_CLIENT);
    CLEAR_SCREEN();

    printf("\nInserisci l'importo di partenza dell'asta (min: %d): ", MIN_ASTA);
    AstaVariables.asta_import = get_int(MIN_ASTA, MAX_PORT);
    AstaVariables.asta_turn = 0;
    CLEAR_SCREEN();

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(server_port);

    if (bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        fprintf(stderr, "\nBind failed with error code : %d" , WSAGetLastError());
        WSACleanup();
        return 1;
    }
    
    CLEAR_SCREEN();
    printf("Server listening on port %d", server_port);
    /* inizializzo la struttura dati che contiene un array di user ed un counter */
    UserArray user_array = { malloc(sizeof(User) * AstaVariables.num_clients), 0 };
    /* array di thread */
    HANDLE *thread_array = malloc(sizeof(HANDLE) * AstaVariables.num_clients);

    /* il secondo argomento sono il numero di client che possono stare in coda di connessione prima di essere accettati.*/
    listen(serverSocket, AstaVariables.num_clients);

    int c = sizeof(struct sockaddr_in);
    int recv_size, msgType;
    char message[BUFFER_LEN];
    
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
            user_array.user_array[user_array.counter].username = malloc(sizeof(char) * strlen(message) + 1);
            strcpy(user_array.user_array[user_array.counter].username, message);
            user_array.user_array[user_array.counter].username[strlen((message)) + 1] = '\0';

            /* 
                stampa sul server il messaggio che il nuovo client si e' unito e lo manda anche a tutti i client
                automaticamente l'ultimo arrivato e' escluso dal messaggio perche' il counter ancora deve essere incrementato
            */
            printf("\nClient %s connesso (%d di %d)", message, user_array.counter + 1, AstaVariables.num_clients);

            char tmp[255];
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
    sprintf(sendAsta.message_turn, "Turno del giocatore [ %s ]", user_array.user_array[AstaVariables.asta_turn].username);
    strncpy(sendAsta.nickname_turn, user_array.user_array[AstaVariables.asta_turn].username, (size_t)BUFFER_LEN);
    
    if(!sendToAllAsta(&user_array, &sendAsta, ASTA_MESSAGE)) {
        closeAllSocket(&user_array);
        closeSocket(serverSocket, "Il server ha fallito nel mandare il messaggio di turno, There is nothing we can do..., ", __FILE__, __LINE__);
        system("START https://www.youtube.com/watch?v=F0Gkr4MBEO0?autoplay=1");
    }

    WaitForMultipleObjects(user_array.counter, thread_array, TRUE, INFINITE);
    printf("\nServer ha finito di aspettare per tutti i thread");
    for (int i = 0; i < user_array.counter; i++) {
        CloseHandle(thread_array[i]);
    }
    printf("\nAsta Finita");
    system("START https://www.youtube.com/watch?v=dvgZkm1xWPE?autoplay=1");
    
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}