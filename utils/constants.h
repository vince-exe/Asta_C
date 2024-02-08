#ifndef CONSTANTS_H
#define CONSTANTS_H

/*
 * Include il file di intestazione `limits.h`, che contiene le definizioni delle
 * costanti utilizzate.
 */
#include <limits.h>

/**
 * @brief Numero minimo di client per iniziare l'asta.
 */
#define MIN_ASTA 1

// **Inizio zona dove si definiscono le costanti relative al numero di client.**

/**
 * @brief Numero minimo di client selezionabili.
 */
#define MIN_CLIENT 1

/**
 * @brief Numero massimo di client selezionabili.
 */
#define MAX_CLIENT 128

// **Fine zona dove si definiscono le costanti relative al numero di client.**

// **Inizio zona dove si definiscono le costanti relative al numero di porte.**

/**
 * @brief Numero di porta piu piccolo che si puo selezionare.
 */
#define MIN_PORT 1024

/**
 * @brief Numero di porta piu grande che si puo selezionare.
 */
#define MAX_PORT 49151

// **Fine zona dove si definiscono le costanti relative al numero di porte.**

// **Inizio zona dove si definiscono le costanti relative ai codici di output del server.**

/**
 * @brief Messaggio inviato dal server al client per indicare che il nickname e' gia' in uso.
 */
#define NICK_ALREADY_IN_USE "NICK_ALREADY_IN_USE"

/**
 * @brief Messaggio inviato dal server al client per notificarlo dell'attesa dell'inizio dell'asta.
 */
#define WAIT_FOR_ASTA "WAIT_FOR_ASTA"

/**
 * @brief Codice assegnato al messaggio riferito all'inizio dell'asta.
 */
#define ASTA_STARTED "ASTA_STARTED"

/**
 * @brief Messaggio inviato dal server al client per indicare che l'asta e' conclusa.
 */
#define ASTA_CLOSED "ASTA_CLOSED"

// **Fine zona dove si definiscono le costanti relativi ai codici Output dell server**

// **Inizio zona dove si definiscono le costanti relative ai codici dei messaggi del server.**

/**
 * @brief Codice assegnato al messaggio riferito ad un messaggio generale.
 */
#define GENERAL_MESSAGE 2

/**
 * @brief Codice assegnato al messaggio riferito all'asta.
 */
#define ASTA_STATUS 3

/**
 * @brief Codice assegnato al messaggio riferito ai messaggi per quando l'asta ha inizio
 */
#define ASTA_MESSAGE 4

#define ASTA_IMPORT 5

/**
 * @brief Codice assegnato al messaggio riferito ad un messaggio di testing.
 */
#define TEST_MESSAGE 7

// **Fine zona dove si definiscono le costanti relative ai codici dei messaggi del server.**

/**
 * @brief Lunghezza massima del buffer.
 */
#define BUFFER_LEN 1024

#define NOT_FOUND UINT_MAX

#endif // CONSTANTS_H
