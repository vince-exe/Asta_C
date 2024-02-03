#include <stdio.h>
#include <winsock2.h>

#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "./utils/functions.h"
#include "./utils/constants.h"
#include "./utils/structs.h"

#pragma comment(lib,"ws2_32.lib") // linka automaticamente la libreria in fase di runtime.


void closeAllSocket(UserArray *ptr){
    for(size_t i = 0; i < ptr->counter; i++){
        closesocket(ptr->user_array[i].socket);
        free(ptr->user_array[i].username);
    }
}

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

_Bool sendToAllAsta(UserArray* user_array, SendAsta* dati, int msgType_) {
    int msgType = htonl(msgType_);

    for (int i = 0; i < user_array->counter; i++) {
        if ((send(user_array->user_array[i].socket, (char*)&msgType, sizeof(msgType), 0) < 0) ||
            (send(user_array->user_array[i].socket, (char*)dati, sizeof(SendAsta), 0) < 0)) {
            free(dati->nickname_turn); // Libera la memoria allocata per nickname_turn
            return (_Bool)0;
        }
    }

    //free(dati->nickname_turn); // Libera la memoria allocata per nickname_turn
    return (_Bool)1;
}

/* manda un messaggio a tutti i client */
_Bool sendToAll(UserArray* user_array, const char* message, int msgType_) {
    int msgType = htonl(msgType_);

    for(int i = 0; i < user_array->counter; i++) {
        if((send(user_array->user_array[i].socket, (char*)&msgType, sizeof(msgType), 0) < 0) ||
           (send(user_array->user_array[i].socket, message, strlen(message) + 1, 0) < 0)) {
           return (_Bool)0;
        };
    }
    
    return (_Bool)1;
}

DWORD WINAPI handleClient(LPVOID paramater) {
    int recv_size = 0, typeOfMsg = 0, clientImport = 0, isRunning = 1;
    char tmp[BUFFER_LEN];
    HandleClientParams* param = (HandleClientParams*)paramater;
    InputClient inputClient;

    /* qui ci salviamo l'utente attuale essendo che paramater cambia ogni volta */
    User currentUser = *param->user;
    
    printf("\nStart listening thread on client %s\n", currentUser.username);
    while(isRunning) {
        /* ricevere la struttura */
        recv(param->user->socket, (char*)&inputClient, sizeof(InputClient), 0);

        switch(inputClient.msgType) {
            /* il server riceve l'importo dal client ( lo casta ad intero )*/
            case ASTA_IMPORT:
                sprintf(tmp, "Il giocatore %s ha puntato %d", currentUser.username, inputClient.import);
                printf("\n%s", tmp); 
                sendToAll(param->user_array, tmp, GENERAL_MESSAGE);

                /* TODO: mandare il prossimo turno*/
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
    AstaVariables.max_clients = 3; AstaVariables.asta_import = 20; AstaVariables.asta_turn = 0;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(server_port);

    if (bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        fprintf(stderr, "\nBind failed with error code : %d" , WSAGetLastError());
        return 1;
    }
    
    CLEAR_SCREEN();
    printf("Server listening on port %d", server_port);
    /* inizializzo la struttura dati che contiene un array di user ed un counter */
    UserArray user_array = { malloc(sizeof(User) * AstaVariables.max_clients), 0 };
    /* array di thread */
    HANDLE *thread_array = malloc(sizeof(HANDLE) * AstaVariables.max_clients);

    /* il secondo argomento sono il numero di client che possono stare in coda di connessione prima di essere accettati.*/
    listen(serverSocket, AstaVariables.max_clients);

    int c = sizeof(struct sockaddr_in);
    int recv_size, msgType;
    char message[BUFFER_LEN];
    
    while(user_array.counter < AstaVariables.max_clients) {
        user_array.user_array[user_array.counter].socket = accept(serverSocket, (struct sockaddr *)&client, &c);

        if (user_array.user_array[user_array.counter].socket == INVALID_SOCKET) {
            fprintf(stderr, "\naccept failed with error code : %d" , WSAGetLastError());
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
            printf("\nClient %s connesso (%d di %d)", message, user_array.counter + 1, AstaVariables.max_clients);

            char tmp[255];
            sprintf(tmp, "Giocatore %s connesso (%d di %d)", message, user_array.counter + 1, AstaVariables.max_clients);
            if(!sendToAll(&user_array, tmp, GENERAL_MESSAGE)) {
                closeAllSocket(&user_array);
                closeSocket(serverSocket, "\nChiususa del server dovuta ad errore di spedizione", __FILE__, __LINE__);
                return 1;
            };
            /* dichiaro ed inizializzo ogni volta la variabile params che punta al singolo utente e poi la passo al thread*/
            HandleClientParams params = {&user_array.user_array[user_array.counter], &user_array, serverSocket};

            /* aggiungo al vettore di thread il singolo thread che gestira' il client */
            thread_array[user_array.counter] = CreateThread(NULL, 0, handleClient, &params, 0, NULL);
            if(thread_array[user_array.counter] == NULL) {
                closeAllSocket(&user_array);
                char tmp[BUFFER_LEN];
                sprintf(tmp, "\nErrore nella creazione del thread utente %s", user_array.user_array[user_array.counter].username);
                closeSocket(serverSocket, tmp, __FILE__, __LINE__);
                return 1;
            }
            user_array.counter++;

            /* il server comunica a tutti i client che l'asta e' iniziata */
            if(user_array.counter == AstaVariables.max_clients) {
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
                    return 1;
                }
            }
        }
    }
    /* TEMP TEST 
        - FARLO CON I PUNTATORI DINAMICI 
    */
    SendAsta sendAsta;

    sendAsta.import = AstaVariables.asta_import;
    sprintf(sendAsta.message_turn, "Turno del giocatore [ %s ]", user_array.user_array[AstaVariables.asta_turn].username);
    strcpy(sendAsta.nickname_turn, user_array.user_array[AstaVariables.asta_turn].username);
    
    sendToAllAsta(&user_array, &sendAsta, ASTA_MESSAGE);

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