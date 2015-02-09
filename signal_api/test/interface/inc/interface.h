#ifndef INTERFACE_H
#define INTERFACE_H

#include "interface_general.h"

#define INTERFACE_A_SERVER_PORT (10002)

#define INIT_SERVICE_REQ (100)
#define INIT_SERVICE_CFM (101)

#define CLIENT_DOWN_IND  (102)

#define SERVER_UP_IND    (103)
#define SERVER_DOWN_IND  (104)

#define MAX_MSG_SIZE (128)

typedef unsigned char serviceResult_t;
#define OK  (0)
#define NOK (1)

typedef struct InitializeServiceReq
{
    SigHeader_t  header;
    unsigned int clientId;
} InitializeServiceReq_t;

typedef struct InitializeServiceCfm
{
    SigHeader_t     header;
    unsigned int    clientId;
    unsigned int    serverId;
    serviceResult_t result;
} InitializeServiceCfm_t;

unsigned char InterfaceA_initiateServiceReq(void * interfaceData, unsigned int clientId);

#endif // INTERFACE_H
