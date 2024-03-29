#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#include "./utils/functions.h"

#pragma comment(lib,"ws2_32.lib") // linka automaticamente la libreria in fase di runtime.

int sendToServer(SOCKET client, InputClient* inputClient) {
    if((send(client, (char*)inputClient, sizeof(InputClient), 0) < 0)) {
        return 0;
    }
    
    return 1;
}

int checkNickname(SOCKET socketClient, const char* nickname) {
    char message[BUFFER_LEN];
    int recv_size, typeOfMsg;

    /* manda il nickname per verificare se esiste gia' */
    if(send(socketClient, nickname, strlen(nickname), 0) < 0) {
        puts("Send failed");
        return 0;
    }
    if((recv_size = recv(socketClient, (char*)&typeOfMsg, sizeof(typeOfMsg), 0)) == SOCKET_ERROR) {
        closeSocket(socketClient, "\nReceive Error", __FILE__, __LINE__);
        return 0;
    }
    // Normalizza l'ordine dei byte del numero per adeguarlo a una convenzione generalmente accettata
    typeOfMsg = ntohl(typeOfMsg);
    if(typeOfMsg != ASTA_STATUS) {
        closeSocket(socketClient, "\nFatal Error ( message != ASTA_MESSAGES)", __FILE__, __LINE__);
        return 0;
    }
    message[0] = '\0';
    if((recv_size = recv(socketClient, message, sizeof(message) , 0)) == SOCKET_ERROR) {
        closeSocket(socketClient, "\nReceive error", __FILE__, __LINE__);
        return 0;
    }
    message[recv_size] = '\0';

    if(strcmp(message, NICK_ALREADY_IN_USE) == 0) {
        printf("\nIl nickname %s e' utilizzato da un'altro client. ( Stai un po' fuori amico)", nickname);
        closeSocket(socketClient, "", __FILE__, __LINE__);
        return 0;
    }
    else if(strcmp(message, WAIT_FOR_ASTA) == 0) {
        printf("\nIn attesa di altri client per iniziare l'asta :).");
        return 1;
    }
    else if(strcmp(message, ASTA_STARTED) == 0) {
        printf("\nAsta iniziata. [ Che vinca l'amico che ha piu' soldi ]\n");
        return 1;
    }

    return 0;
}

void asta_client(SOCKET socketClient, const char* nickname) {
    char tmp[255];
    char message[BUFFER_LEN];
    int recv_size, typeOfMsg, astaImport;
    SendAsta sendAsta;
    InputClient inputClient;

    do {
        message[0] = '\0';
        /* ricevo il tipo di messaggio */
        if((recv_size = recv(socketClient, (char*)&typeOfMsg, sizeof(typeOfMsg), 0)) == SOCKET_ERROR) {
            printf("\n%d", WSAGetLastError());
            closeSocket(socketClient, "\nReceive error", __FILE__, __LINE__);
            return;
        }
        typeOfMsg = ntohl(typeOfMsg);

        if(typeOfMsg == ASTA_MESSAGE) {
            recv(socketClient, (char*)&sendAsta, sizeof(SendAsta), 0);
        }
        /* ricevo il messaggio char normale */
        else {    
            if((recv_size = recv(socketClient, message, sizeof(message) , 0)) == SOCKET_ERROR) {
                closeSocket(socketClient, "\nReceive error", __FILE__, __LINE__);
                return;
            }
            message[recv_size] = '\0';
        }

        switch(typeOfMsg) {
            case GENERAL_MESSAGE:
                printf("\n%s", message);
                break;

            case ASTA_STATUS:
                if(strcmp(message, ASTA_STARTED) == 0) {
                    printf("\nAsta iniziata. [ Che vinca l'amico che ha piu' soldi ]\n");
                }
                else if(strcmp(message, ASTA_WON) == 0) {
                    inputClient.import = sendAsta.import; 
                    inputClient.msgType = ASTA_WON_FROM_CLIENT;
                    
                    if(!sendToServer(socketClient, &inputClient)) {
                        closeSocket(socketClient, "Il client ha fallito nell'invio del messaggio al server", __FILE__, __LINE__);
                        return;
                    }
                    printf("\n\n[ Asta Vinta ]. Premio aggiudicato all'utente %s alla modica cifra di %d", nickname, sendAsta.import);
                    return;
                }
                break;

            case ASTA_MESSAGE:
                /* se tocca al client, puo' inserire l'importo */
                if(strcmp(sendAsta.nickname_turn, nickname) == 0) {
                    printf("\nTocca a te, inserisci l'importo (ultimo %d): ", sendAsta.import);
                    inputClient.import = get_int(sendAsta.import + 1, INT_MAX);
                    inputClient.msgType = ASTA_IMPORT;

                    if(!sendToServer(socketClient, &inputClient)) {
                        closeSocket(socketClient, "Il client ha fallito nell'invio del messaggio al server", __FILE__, __LINE__);
                        return;
                    }
                }
                else {
                    printf("\n%s", sendAsta.message_turn);
                }
                break;

            default:
                sprintf(tmp, "\nUnidentified message: %d", typeOfMsg);
                closeSocket(socketClient, tmp, __FILE__, __LINE__);
                return;
        }
    } while(1);
}

int main(int argc , char *argv[]) {
    WSADATA wsa;
    SOCKET socketClient;

    struct sockaddr_in server, address;
    char ipServer[BUFFER_LEN];
    int serverPort;
    
    CLEAR_SCREEN();
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
    if((socketClient = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET) {
        printf("Could not create socket : %d" , WSAGetLastError());
    }

    printf("\nInserisci l'ip del server: ");
    fgets(ipServer, sizeof(ipServer), stdin);
    ipServer[strlen(ipServer) - 1] = '\0';
    
    server.sin_addr.s_addr = inet_addr(ipServer); 
    server.sin_family = AF_INET;
 
    printf("\nInserisci la porta del server (%d, %d): ", MIN_PORT, MAX_PORT);
    serverPort = get_int(MIN_PORT, MAX_PORT);
    
    server.sin_port = htons(serverPort); 
    
    char nickname[BUFFER_LEN];
    int typeOfMsg;

    printf("\nInserisci username: ");
    fgets(nickname, sizeof(nickname), stdin);
    nickname[strlen(nickname) - 1] = '\0'; 

    if (connect(socketClient, (struct sockaddr *)&server, sizeof(server)) < 0) {
        closeSocket(socketClient, "Connect Error", __FILE__, __LINE__);
        return 1;
    }

    if(!checkNickname(socketClient, nickname)) {
        return 1;
    }

    asta_client(socketClient, nickname);

    closesocket(socketClient);
    WSACleanup();
    return 0;
}