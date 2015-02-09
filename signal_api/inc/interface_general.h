/*******************************************************************************
 *
 * Project:
 *
 * Subsystem:
 *
 * Filename: interface_general.h
 *
 * Description: Declaration of all main functions of signal_api
 *              and all main internal structures.
 *
 * Author: Dmitry Durnov (dmitry.durnov at gmail.com)
 *
 * History:
 * -----------------------------------------------------------------------------
 * Version  Date      Author            Comment
 * -----------------------------------------------------------------------------
 * 0.1.0    20100522  Dmitry Durnov     Created.
 * 0.1.1    20100523  Dmitry Durnov     ClientData_t and ServerData_t ->
 *                                      InterfaceData_t. Boolean_t and Port_t
 *                                      have been introduced. (types_general.h)
 *                                      sigType_t -> SigType_t
 *                                      sigSize_t -> SigSize_t
 * 0.2.0    20100714  Dmitry Durnov     CONNECTION_UP and CONNECTION_DOWN signals
 *                                      registration has been added.
 *
 *
 ******************************************************************************/

#ifndef INTERFACE_GENERAL_H
#define INTERFACE_GENERAL_H

#include <pthread.h>
#include <sys/socket.h>

#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#include "types_general.h"

/* Location */

#define LOCATION_LENGTH (16)

typedef char Location_t [LOCATION_LENGTH];

#define LOCAL_CONNECTION "127.0.0.1"

/* Signal Header */

typedef unsigned short int SigType_t;
/* Special interface control signals */
#define GENERAL_CONNECTION_UP      (0xFF01)
#define GENERAL_CONNECTION_DOWN    (0xFF02)

/* To make Interface_connect by blocking call */
#define DISABLE_CONNECTION_UP      (0xFFFF)

typedef unsigned short int SigSize_t;

typedef struct SigHeader
{
    SigType_t type;
    SigSize_t size;
    int       sender;  /* To be filled by receiver */
} SigHeader_t;

/* Signal Queue */

typedef struct SignalQueueCell
{
    SigHeader_t            * header;
    struct SignalQueueCell * next;

} SignalQueueCell_t;

typedef struct SignalQueue
{
    SignalQueueCell_t * begin;
    SignalQueueCell_t * end;

    int                 size;

    pthread_mutex_t     mutex;
} SignalQueue_t;

typedef int ConnectionId_t;

/* Interface Data */

typedef struct InterfaceData
{
    pthread_t          thread;
    volatile Boolean_t is_running;

    SignalQueue_t *    queue_p;
    ConnectionId_t     fd;
    struct sockaddr_in addr;
    socklen_t          addr_size;

    SigType_t          connectionUp;
    SigType_t          connectionDown;

} InterfaceData_t;

/* Interface API */

SignalQueue_t * SignalQueue_init();
Boolean_t       SignalQueue_put(SignalQueue_t * sigQueue_p, SigHeader_t * signal_p);
void         *  SignalQueue_get(SignalQueue_t * sigQueue_p, SigType_t * type_p);

void         * alloc_sig(SigSize_t size, SigType_t type);
void           free_sig(void ** sig_p);
void           send_sig(int destId, void ** sig_p);
void         * receive_sig(SignalQueue_t * sigQueue_p, SigType_t * filter_p);

int            sender(void * sig_p);

Boolean_t Interface_publish(void ** data_p,
                            SignalQueue_t * sigQueue_p,
                            Location_t location,
                            Port_t port,
                            SigType_t connectionDown);

Boolean_t Interface_connect(void ** data_p,
                            SignalQueue_t * sigQueue_p,
                            Location_t location,
                            Port_t port,
                            SigType_t connectionUp,
                            SigType_t connectionDown);

Boolean_t Interface_disconnect(void ** data_p);

ConnectionId_t Interface_getServerId(void ** data_p);

#endif // INTERFACE_GENERAL_H
