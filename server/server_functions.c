#include "server_functions.h"

void allocateUsername(UserArray* userArray, const char* username) {
    userArray->user_array[userArray->counter].username = malloc(sizeof(char) * strlen(username) + 1);
    strcpy(userArray->user_array[userArray->counter].username, username);
    userArray->user_array[userArray->counter].username[strlen((username)) + 1] = '\0';
}

void takeInput(int* serverPort, AstaVariables* AstaVariables) {
    CLEAR_SCREEN();
    printf("Inserisci la porta (%d, %d): ", MIN_PORT, MAX_PORT);
    (*serverPort) = get_int(MIN_PORT, MAX_PORT);
    CLEAR_SCREEN();

    printf("\nInserisci il numero di client (%d, %d): ", MIN_CLIENT, MAX_CLIENT);
    AstaVariables->num_clients = get_int(MIN_CLIENT, MAX_CLIENT);
    CLEAR_SCREEN();

    printf("\nInserisci l'importo di partenza dell'asta (min: %d): ", MIN_ASTA);
    AstaVariables->asta_import = get_int(MIN_ASTA, MAX_PORT);
    CLEAR_SCREEN();
}

int existUsername(UserArray* user_array, const char* newNickname) {
    for(size_t i = 0; i < user_array->counter; i++) {
        if(strcmp((user_array->user_array[i].username), newNickname) == 0) {
            return 1;
        }
    }

    return 0;
}

void closeAllSocket(UserArray* userArray){
    for(size_t i = 0; i < userArray->counter; i++){
        if (userArray->user_array[i].socket != INVALID_SOCKET) {
            closesocket(userArray->user_array[i].socket);
        }

        free(userArray->user_array[i].username);
    }
}

int sendToAllAsta(UserArray* userArray, SendAsta* data, const int msgType_) {
    int msgType = htonl(msgType_);

    for (size_t i = 0; i < userArray->counter; i++) {
        if ((send(userArray->user_array[i].socket, (char*)&msgType, sizeof(msgType), 0) < 0) ||
            (send(userArray->user_array[i].socket, (char*)data, sizeof(SendAsta), 0) < 0)) {
            return 0;
        }
    }

    return 1;
}

int sendToAll(UserArray* userArray, const char* message, const int msgType_) {
    int msgType = htonl(msgType_);

    for(size_t i = 0; i < userArray->counter; i++) {
        if((send(userArray->user_array[i].socket, (char*)&msgType, sizeof(msgType), 0) < 0) ||
           (send(userArray->user_array[i].socket, message, strlen(message) + 1, 0) < 0)) {
           return 0;
        };
    }
    
    return 1;
}

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

int shiftArray(UserArray* userArray, const char* username) {
    size_t pos = get_pos(userArray, username);

    if(pos == NOT_FOUND) {
        return 0;
    }

    for(size_t i = pos; i < userArray->counter; i++) {
        userArray->user_array[i] = userArray->user_array[i + 1];
    }

    userArray->counter--;
    return 1;
}

int handleTurn(AstaVariables* astaVariables, UserArray* user_array, SendAsta* sendAsta, int check) {    
    if(check) {
        if(astaVariables->asta_turn >= (user_array->counter - 1)) {
            astaVariables->asta_turn = 0;
        }
        else {
            astaVariables->asta_turn++;
        }
    }
    else {
        /* in caso l'ultimo quitta, parte dal primo */
        if(astaVariables->asta_turn >= (user_array->counter)) {
            astaVariables->asta_turn = 0;
        }
    }

    printf("\nTurno del giocatore: %d", astaVariables->asta_turn);

    sendAsta->import = astaVariables->asta_import;
    sprintf(sendAsta->message_turn, "Turno del giocatore [ %s ]", user_array->user_array[astaVariables->asta_turn].username);
    strncpy(sendAsta->nickname_turn, user_array->user_array[astaVariables->asta_turn].username, BUFFER_LEN);

    return sendToAllAsta(user_array, sendAsta, ASTA_MESSAGE);
}

void clientQuit(HandleClientParams* param, User* currentUser, SendAsta* sendAsta) {
    char tmp[BUFFER_LEN];

    sprintf(tmp, "Il giocatore %s ha abbandonato l'asta", currentUser->username);
    printf("\n%s", tmp);

    /* shiftare l'utente */
    if(!shiftArray(param->user_array, currentUser->username)){
        closeAllSocket(param->user_array);
        closeSocket(param->socketServer, "\nChiususa del server dovuta ad errore di incongruenza", __FILE__, __LINE__);
    }

    /* mandiamo il messaggio di quit */
    if(!sendToAll(param->user_array, tmp, GENERAL_MESSAGE)) {
        closeAllSocket(param->user_array);
        closeSocket(param->socketServer, "\nChiususa del server dovuta ad errore di spedizione ( client quit )", __FILE__, __LINE__);
        return;
    }
            
    if(param->user_array->counter == 1) {
        sprintf(tmp, ASTA_WON);
        if(!sendToAll(param->user_array, tmp, ASTA_STATUS)) {
            closeAllSocket(param->user_array);
            closeSocket(param->socketServer, "\nChiususa del server dovuta ad errore di spedizione ( client won )", __FILE__, __LINE__);
            return;
        }
    }

    if(!handleTurn(param->astaVariables, param->user_array, sendAsta, 0)) {
        closeAllSocket(param->user_array);
        closeSocket(param->socketServer, "\nIl server ha fallito nel mandare il messaggio di turno, There is nothing we can do...", __FILE__, __LINE__);
        return;
    }
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
            clientQuit(param, &currentUser, &sendAsta);
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
                if(!handleTurn(param->astaVariables, param->user_array, &sendAsta, 1)) {
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