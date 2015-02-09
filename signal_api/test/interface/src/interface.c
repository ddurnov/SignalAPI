#include "interface.h"

unsigned char InterfaceA_initiateServiceReq(void * data_p, unsigned int clientId)
{
    InterfaceData_t * interface_p = data_p;

    InitializeServiceReq_t * sig_p = NULL;

    if (interface_p->fd == -1)
    {
        return FALSE;
    }

    sig_p = alloc_sig(sizeof(InitializeServiceReq_t), INIT_SERVICE_REQ);

    sig_p->clientId = clientId;

    send_sig(interface_p->fd, (void **)&sig_p);

    return TRUE;
}
