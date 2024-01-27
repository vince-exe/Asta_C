/*
 * Includere il file di intestazione `stdio.h`, che contiene le dichiarazioni
 * delle funzioni e delle macro utilizzate per l'input/output standard (I/O).
 */
#include <stdio.h>

/*
 * Include il file di intestazione `string.h`, che contiene le
 * definizioni delle funzioni di gestione delle stringhe.
 */
#include <string.h>

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
#include "./utils/functions.h"

#pragma comment(lib,"ws2_32.lib") // linka automaticamente la libreria in fase di runtime.

int main(int argc , char *argv[]) {
    WSADATA wsa;
    SOCKET socketClient;

    struct sockaddr_in server, address;
    char message[BUFFER_LEN];
    char ipServer[BUFFER_LEN];
    int serverPort;
    int recv_size;
    
    CLEAR_SCREEN();
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
    if((socketClient = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET) {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
    /* NON CANCELLARE
    printf("\nInserisci l'ip del server: ");
    fgets(ipServer, sizeof(ipServer), stdin);
    ipServer[strlen(ipServer) - 1] = '\0';
    */
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    server.sin_family = AF_INET;
    /* NON CANCELLARE
    printf("\nInserisci la porta del server (%d, %d): ", MIN_PORT, MAX_PORT);
    serverPort = get_int(MIN_PORT, MAX_PORT);
    */
    server.sin_port = htons(8888); 
    
    char nickname[BUFFER_LEN];
    int typeOfMsg;

    printf("Inserisci username: ");
    fgets(nickname, sizeof(nickname), stdin);
    nickname[strlen(nickname) - 1] = '\0'; 

    if (connect(socketClient, (struct sockaddr *)&server, sizeof(server)) < 0) {
        closeSocket(socketClient, "Connect Error", __FILE__, __LINE__);
        return 1;
    }
    /* manda il nickname per verificare se esiste gia' */
    if(send(socketClient, nickname, strlen(nickname), 0) < 0) {
        puts("Send failed");
        return 1;
    }
    message[0] = '\0';
    if((recv_size = recv(socketClient, (char*)&typeOfMsg, sizeof(typeOfMsg), 0)) == SOCKET_ERROR) {
        closeSocket(socketClient, "Receive Error", __FILE__, __LINE__);
        return 1;
    }
    // Normalizza l'ordine dei byte del numero per adeguarlo a una convenzione generalmente accettata
    typeOfMsg = ntohl(typeOfMsg);
    if(typeOfMsg != ASTA_MESSAGES) {
        closeSocket(socketClient, "Fatal Error ( message != ASTA_MESSAGES)", __FILE__, __LINE__);
        return 1;
    }
    if(typeOfMsg == ASTA_MESSAGES) {
        if((recv_size = recv(socketClient, message, sizeof(message) , 0)) == SOCKET_ERROR) {
            closeSocket(socketClient, "Receive error", __FILE__, __LINE__);
            return 1;
        }
        message[recv_size] = '\0';

        if(strcmp(message, NICK_ALREADY_IN_USE) == 0) {
            printf("\nIl nickname %s \x86 utilizzato da un'altro client.", nickname);
            closeSocket(socketClient, "Nickname already in use", __FILE__, __LINE__);
            return 1;
        }
        else if(strcmp(message, WAIT_FOR_ASTA) == 0) {
            printf("\nIn attesa di altri client per iniziare l'asta :).");
        }
    }

    do {
        message[0] = '\0';
        /* ricevo il tipo di messaggio */
        if((recv_size = recv(socketClient, (char*)&typeOfMsg, sizeof(typeOfMsg), 0)) == SOCKET_ERROR) {
            closeSocket(socketClient, "Receive error", __FILE__, __LINE__);
            return 1;
        }
        typeOfMsg = ntohl(typeOfMsg);
        /* ricevo il messaggio */
        if((recv_size = recv(socketClient, message, sizeof(message) , 0)) == SOCKET_ERROR) {
            closeSocket(socketClient, "Receive error", __FILE__, __LINE__);
            return 1;
        }
        message[recv_size] = '\0';

        switch(typeOfMsg) {
            case GENERAL_MESSAGE:
                printf("\n%s", message);
                break;

            case ASTA_MESSAGES:
                if(strcmp(message, ASTA_CLOSED) == 0) {
                    /* TO-DO */
                }
                puts("Messaggio riguardante l'asta");
                break;

            default:
                closeSocket(socketClient, "Unidentified message", __FILE__, __LINE__);
                return 1;
        }
    } while(1);

    closesocket(socketClient);
    WSACleanup();
    return 0;
}