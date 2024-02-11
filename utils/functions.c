#include "functions.h"

/**
 * @brief Funzione che controlla se si inserisce un intero.
 *
 * @param min: Numero piu' piccolo che si puo' inserire.
 * @param max: Numero piu' grande che si puo' inserire.
 * 
 * @return int: Il numero inserito.
 */
int get_int(const int min, const int max) {
    char buffer[11];
    int n = 0;

    do {
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strlen(buffer) - 1] = '\0';
        n = atoi(buffer);
    } while(n < min || n > max);
    
    return n;
}

/**
 * @brief Funzione che stampa un messaggio e chiude il socket.
 *
 * @param socket: Numero piu' piccolo che si puo' inserire.
 * @param message: Messaggio da stampare.
 * @param file_: File.
 * @param line_: Linea di codice.
 */
void closeSocket(SOCKET socket, const char* message, const char* file_, const int line_) {
    fprintf(stderr, "%s  %s  %d", message, file_, line_);
    closesocket(socket);
    WSACleanup();
    return;
}

/**
 * @brief Funzione che stampa un messaggio e chiude il socket.
 *
 * @param client: Il socket a cui verra' mandato intero.
 * @param integer: Stringa da mandare.
 * @param msgType_: Tipo del messaggio.
 *
 * @return 0: Se hanno fallito.
 * @return 1: Se e' andato tutto a buon fine.
 */
int sendTo(SOCKET client, const char* message, int msgType_) {
    int msgType = htonl(msgType_); 

    if((send(client, (char*)&msgType, sizeof(msgType), 0) < 0) ||
       (send(client, message, strlen(message) + 1, 0) < 0)) {
        return 0;
    };

    return 1;
}