#include <stdio.h>

#include "interface.h"

union Signal {
    SigHeader_t             header;
    InitializeServiceReq_t  initializeServiceReq;
};

static SignalQueue_t * queue_p = NULL;

static void * server1 = NULL;

int main(int argc, char * argv[])
{
    (void)argc;
    (void)argv;

    queue_p = SignalQueue_init();

    union Signal * sig_p = NULL;
    SigType_t filter[] = {0};

    InitializeServiceCfm_t * sigCfm_p = NULL;

    if (Interface_publish(&server1,
                          queue_p,
                          LOCAL_CONNECTION,
                          INTERFACE_A_SERVER_PORT,
                          CLIENT_DOWN_IND) == FALSE)
    {
        printf("Unable to publish interface\n");
        exit(1);
    }

    while(TRUE)
    {
        printf("Main loop\n");

        sig_p = receive_sig(queue_p, filter);

        switch (sig_p->header.type)
        {
            case INIT_SERVICE_REQ:
                printf("RECV: INIT_SERVICE_REQ: clientId=%d from %d\n",
                                            sig_p->initializeServiceReq.clientId,
                                            sender(sig_p));

                sigCfm_p = alloc_sig(sizeof(InitializeServiceCfm_t), INIT_SERVICE_CFM);

                sigCfm_p->clientId = sig_p->initializeServiceReq.clientId;
                sigCfm_p->serverId = 888;
                sigCfm_p->result   = OK;

                send_sig(sender(sig_p), (void **)&sigCfm_p);

                break;

            case CLIENT_DOWN_IND:
                printf("RECV: CLIENT_DOWN_IND from %d\n", sig_p->header.sender);
                break;

            default:
                printf("Unknown signal: 0x%04X\n", sig_p->header.type);
        }

        free_sig((void **)&sig_p);
    }

    return 0;
}

