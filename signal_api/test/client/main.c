#include <stdio.h>

#include "interface.h"

union Signal {
    SigHeader_t             header;
    InitializeServiceCfm_t  initializeServiceCfm;
};

static SignalQueue_t * queue_p = NULL;

int main(int argc, char * argv[])
{
    (void)argc;
    (void)argv;

    queue_p = SignalQueue_init();

    union Signal * sig_p;
    SigType_t filter[] = {0};

    void * interface1;

    if (Interface_connect(&interface1,
                          queue_p,
                          LOCAL_CONNECTION,
                          INTERFACE_A_SERVER_PORT,
                          SERVER_UP_IND, /* DISABLE_CONNECTION_UP */
                          SERVER_DOWN_IND) == FALSE)
    {
        printf("Unable to connect to interface1\n");
        exit(1);
    }

    while(TRUE)
    {
        printf("Main loop\n");

        sig_p = receive_sig(queue_p, filter);

        switch (sig_p->header.type)
        {
            case INIT_SERVICE_CFM:
                printf("RECV: INIT_SERVICE_CFM: clientId=%d serverId=%d result=%d from %d\n",
                                                sig_p->initializeServiceCfm.clientId,
                                                sig_p->initializeServiceCfm.serverId,
                                                sig_p->initializeServiceCfm.result,
                                                sender(sig_p));

                InterfaceA_initiateServiceReq(interface1, 123);

                break;

            case SERVER_UP_IND:
                printf("RECV: SERVER_UP_IND from %d\n", sender(sig_p));

                if (Interface_getServerId(&interface1) == sender(sig_p))
                {
                    InterfaceA_initiateServiceReq(interface1, 123);
                }

                break;

            case SERVER_DOWN_IND:
                printf("RECV: SERVER_DOWN_IND from %d\n", sender(sig_p));

                Interface_disconnect(&interface1);

                if (Interface_connect(&interface1,
                                      queue_p,
                                      LOCAL_CONNECTION,
                                      INTERFACE_A_SERVER_PORT,
                                      SERVER_UP_IND, /* DISABLE_CONNECTION_UP */
                                      SERVER_DOWN_IND) == FALSE)
                {
                    printf("Unable to connect to interface1\n");
                    exit(1);
                }

                break;

        default:
                printf("Unknown signal\n");
        }

        free_sig((void **)&sig_p);
    }

    return 0;
}

