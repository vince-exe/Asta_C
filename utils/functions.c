/*
 * Include il file di intestazione `function.h`, che contiene le definizioni delle
 * funzioni.
 */
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
    char buffer[10];
    int n;
    
    do {
        fgets(buffer, 10, stdin);
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
    printf("%s  %s  %d", message, file_, line_);
    closesocket(socket);
    WSACleanup();
    return;
}
