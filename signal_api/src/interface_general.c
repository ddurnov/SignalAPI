/*******************************************************************************
 *
 * Project:
 *
 * Subsystem:
 *
 * Filename: interface_general.c
 *
 * Description: Implementation of all main functions of signal_api.
 *
 * Author: Dmitry Durnov (dmitry.durnov at gmail.com)
 *
 * History:
 * -----------------------------------------------------------------------------
 * Version  Date      Author            Comment
 * -----------------------------------------------------------------------------
 * 0.1.0    20100522  Dmitry Durnov     Created.
 * 0.2.0    20100714  Dmitry Durnov     Connection down and connection up signals
 *                                      registration has been added.
 *
 ******************************************************************************/

#include "interface_general.h"

#define CLIENT_SOCKET_HANDLER_DELAY (10000)
#define SERVER_SOCKET_HANDLER_DELAY (10000)
#define RECEIVE_DELAY               (10000)
#define CONNECT_RETRY_DELAY         (100000)

#define MAX_PENDING (10)

void * alloc_sig(SigSize_t size, SigType_t type)
{
    SigHeader_t * sig_p = (SigHeader_t *)malloc(size);

    if (sig_p != NULL)
    {
        sig_p->type   = type;
        sig_p->size   = size;
        sig_p->sender = -1;
    }
    else
    {
        /* TODO: error message */
    }

    return (void *)sig_p;
}

void free_sig(void ** data_p)
{
    SigHeader_t ** sig_p = (SigHeader_t **)data_p;

    free(*sig_p);
    *sig_p = NULL;
}

void send_sig(int destId, void ** data_p)
{
    if (destId == -1)
    {
        /* TODO: error message */
        return;
    }

    int bytes_sent = 0;

    SigHeader_t ** sig_p = (SigHeader_t **) data_p;

    while(bytes_sent != (*sig_p)->size)
    {
        bytes_sent += send(destId, (*sig_p), (*sig_p)->size, 0);

        if (bytes_sent < 0)
        {
            /* TODO: destId is dead */
            break;
        }
    }

    /* printf("%d bytes have been sent\n", bytes_sent); */

    free_sig(data_p);

    return;
}

void * receive_sig(SignalQueue_t * sigQueue_p, SigType_t * filter_p)
{
    SigHeader_t * sig_p = NULL;

    while((sig_p = SignalQueue_get(sigQueue_p, filter_p)) == NULL) { usleep(RECEIVE_DELAY); }

    return (void *)sig_p;
}

int sender(void * data_p)
{
    SigHeader_t * sig_p = (SigHeader_t *)data_p;

    return sig_p->sender;
}

SignalQueue_t * SignalQueue_init()
{
    SignalQueue_t * queue_p = (SignalQueue_t *)malloc(sizeof(SignalQueue_t));

    pthread_mutexattr_t attr;

    if (queue_p != NULL)
    {
        queue_p->begin = NULL;
        queue_p->end   = NULL;
        queue_p->size  = 0;

        pthread_mutexattr_init(&attr);
        pthread_mutex_init(&queue_p->mutex, &attr);
    }

    return queue_p;
}

Boolean_t SignalQueue_put(SignalQueue_t * sigQueue_p, SigHeader_t * signal_p)
{
    SignalQueueCell_t * cell_p = NULL;

    if (sigQueue_p == NULL)
    {
        return FALSE;
    }

    pthread_mutex_lock(&sigQueue_p->mutex);

    cell_p = (SignalQueueCell_t *)malloc(sizeof(SignalQueueCell_t));

    if (cell_p == NULL)
    {
        pthread_mutex_unlock(&sigQueue_p->mutex);

        return FALSE;
    }

    cell_p->header = (SigHeader_t *)signal_p;
    cell_p->next   = NULL;

    if (sigQueue_p->size == 0)
    {
        sigQueue_p->begin = sigQueue_p->end = cell_p;
    }
    else
    {
        sigQueue_p->end->next = cell_p;
        sigQueue_p->end = cell_p;
    }

    sigQueue_p->size++;

    pthread_mutex_unlock(&sigQueue_p->mutex);

    return TRUE;
}

void * SignalQueue_get(SignalQueue_t * sigQueue_p, SigType_t * type_p)
{
    SignalQueueCell_t * curr_cell_p    = NULL;
    SignalQueueCell_t * prev_cell_p    = NULL;

    union Signal      * sig_p     = NULL;

    int i;

    int size = (int)type_p[0];

    unsigned char isMatch = FALSE;

    if (sigQueue_p == NULL)
    {
        return NULL;
    }

    pthread_mutex_lock(&sigQueue_p->mutex);

    if (sigQueue_p->size == 0)
    {
        pthread_mutex_unlock(&sigQueue_p->mutex);

        return NULL;
    }

    /* search for type */

    for (curr_cell_p = sigQueue_p->begin;
         curr_cell_p != NULL;
         prev_cell_p = curr_cell_p, curr_cell_p = curr_cell_p->next)
    {

        if (size == 0) /* Return first signal from queue */
        {
            isMatch = TRUE;
        }
        else if (size > 0) /* Get first equal signal */
        {
            for (i = 0; i < size; i++)
            {
                if (curr_cell_p->header->type == type_p[i + 1])
                {
                    isMatch = TRUE;
                    break; /* type_p array */
                }
            }
        }
        else /* size < 0 Get first not equal signal */
        {
            for (i = 0; i < (-size); i++)
            {
                if (curr_cell_p->header->type != type_p[i + 1])
                {
                    isMatch = TRUE;
                    break; /* type_p array */
                }
            }
        }

        if (isMatch == TRUE)
        {
            break; /* queue */
        }
    }

    if (isMatch == FALSE)
    {
        pthread_mutex_unlock(&sigQueue_p->mutex);

        return NULL;
    }

    if (curr_cell_p->next != NULL)
    {
        if (prev_cell_p == NULL)
        {
            sigQueue_p->begin = curr_cell_p->next;
        }
        else
        {
            prev_cell_p->next = curr_cell_p->next;
        }
    }
    else
    {
        if (prev_cell_p == NULL)
        {
            sigQueue_p->begin = NULL;
            sigQueue_p->end   = NULL;
        }
        else
        {
            prev_cell_p->next = NULL;
            sigQueue_p->end   = prev_cell_p;
        }
    }

    sigQueue_p->size--;

    sig_p = (union Signal *)curr_cell_p->header;

    free(curr_cell_p);

    pthread_mutex_unlock(&sigQueue_p->mutex);

    return sig_p;
}

void * handle_socket(void * data_p)
{
    InterfaceData_t ** client_p = (InterfaceData_t **)data_p;

    SigHeader_t * sig_p = NULL;

    SigHeader_t header;

    int header_bytes_read;
    int payload_bytes_read;

    printf("Starting handling of the client\n");

    while ((*client_p)->is_running)
    {
        header_bytes_read  = 0;
        payload_bytes_read = 0;

        /* printf("Client socket main loop\n"); */

        while(header_bytes_read < (int)sizeof(SigHeader_t))
        {
            header_bytes_read += recv((*client_p)->fd,
                                      &header + header_bytes_read,
                                      sizeof(SigHeader_t) - header_bytes_read,
                                      MSG_PEEK);

            if (header_bytes_read <= 0)
            {
                sig_p = alloc_sig(sizeof(SigHeader_t), (*client_p)->connectionDown);

                sig_p->sender = (*client_p)->fd;

                SignalQueue_put((*client_p)->queue_p, sig_p);

                pthread_exit(NULL);
            }
        }

        /* printf("Receiving message: type: %d size: %d\n", header.type, header.size); */

        sig_p = alloc_sig(header.size, header.type);

        while(payload_bytes_read < header.size)
        {
            payload_bytes_read += recv((*client_p)->fd,
                                       sig_p + payload_bytes_read,
                                       header.size - payload_bytes_read, 0);

            if (payload_bytes_read <= 0)
            {
                sig_p->type = (*client_p)->connectionDown;

                sig_p->sender = (*client_p)->fd;

                SignalQueue_put((*client_p)->queue_p, sig_p);

                pthread_exit(NULL);
            }

            /* printf("Received: %d of %d\n", payload_bytes_read, sig_p->size); */

        }

        sig_p->sender = (*client_p)->fd;

        /* printf("Received message: type: %d size: %d\n", sig_p->type, sig_p->size); */

        SignalQueue_put((*client_p)->queue_p, sig_p);

        usleep(CLIENT_SOCKET_HANDLER_DELAY);
    }

    return NULL;
}

void * connect_socket(void * data_p)
{
    InterfaceData_t ** client_p = (InterfaceData_t **)data_p;

    SigHeader_t * sig_p = NULL;

    if ((*client_p)->connectionUp != DISABLE_CONNECTION_UP)
    {
        /* Non blocking function call */

        /* Establish connection */
        while((connect((*client_p)->fd,
                       (struct sockaddr *) &(*client_p)->addr,
                       sizeof((*client_p)->addr)) < 0) && (*client_p)->is_running)
        {
            printf("Trying to connect to the server...\n");
            usleep(CONNECT_RETRY_DELAY);
        }

        sig_p = alloc_sig(sizeof(SigHeader_t), (*client_p)->connectionUp);
        
        sig_p->sender = (*client_p)->fd;
        
        SignalQueue_put((*client_p)->queue_p, sig_p);
    }

    return handle_socket(data_p);
}

void * listen_socket(void * data_p)
{
    InterfaceData_t ** server_p = (InterfaceData_t **)data_p;

    int fd;

    InterfaceData_t * client_p = NULL;

    printf("Starting server\n");

    while((*server_p)->is_running)
    {
        fd = accept((*server_p)->fd,
                    (struct sockaddr *)&(*server_p)->addr,
                    &(*server_p)->addr_size);

        printf("fd == %d\n", fd);

        if (fd != -1)
        {
            printf("Connecting new client\n");

            client_p = (InterfaceData_t *)malloc(sizeof(InterfaceData_t));

            client_p->connectionUp   = (*server_p)->connectionUp;
            client_p->connectionDown = (*server_p)->connectionDown;

            client_p->fd         = fd;
            client_p->queue_p    = (*server_p)->queue_p;
            client_p->is_running = TRUE;

            pthread_create(&client_p->thread, NULL, handle_socket, (void *)&client_p);
        }
        else
        {
            /* TODO: Print something... */
            perror("Accept failed.");
        }

        usleep(SERVER_SOCKET_HANDLER_DELAY);
    }

    return NULL;
}

Boolean_t Interface_publish(void ** data_p,
                            SignalQueue_t * queue_p,
                            Location_t location,
                            Port_t port,
                            SigType_t connDown)
{
    InterfaceData_t * server_p = NULL;

    *data_p = malloc(sizeof(InterfaceData_t));

    server_p = (InterfaceData_t *)(*data_p);

    server_p->queue_p = queue_p;

    if ((server_p->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Unable to publish interface: stage 1\n");
        free(*data_p);
        *data_p = NULL;
        return FALSE;
    }
    /* Allow socket descriptor to be reuseable */
    int on = 1;

    if (setsockopt(server_p->fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0) 
    {
        printf("Unable to publish interface: stage 2\n");
        free(*data_p);
        *data_p = NULL;
        return FALSE;
    }

    memset(&server_p->addr, 0, sizeof(server_p->addr));      /* Clear struct */
    server_p->addr.sin_family      = AF_INET;                /* Internet/IP */
    server_p->addr.sin_addr.s_addr = inet_addr(location);    /* Incoming addr */
    server_p->addr.sin_port        = htons(port);            /* Server port */

    /* Bind the server socket */
    if (bind(server_p->fd,
             (struct sockaddr *) &server_p->addr,
             sizeof(server_p->addr)) < 0)
    {
        printf("Unable to publish interface: stage 3\n");
        free(*data_p);
        *data_p = NULL;
        return FALSE;
    }

    /* Listen on the server socket */
    if (listen(server_p->fd, MAX_PENDING) < 0)
    {
        printf("Unable to publish interface: stage 4\n");
        free(*data_p);
        *data_p = NULL;
        return FALSE;
    }

    server_p->addr_size = sizeof(server_p->addr);

    server_p->connectionUp   = DISABLE_CONNECTION_UP; /* Unused */
    server_p->connectionDown = connDown;
    server_p->is_running     = TRUE;

    pthread_create(&server_p->thread, NULL, listen_socket, (void *)data_p);

    return TRUE;
}

Boolean_t Interface_connect(void ** data_p,
                            SignalQueue_t * queue_p,
                            Location_t location,
                            Port_t port,
                            SigType_t connUp,
                            SigType_t connDown)
{
    InterfaceData_t * client_p = NULL;

    *data_p = malloc(sizeof(InterfaceData_t));

    client_p = (InterfaceData_t *)(*data_p);

    client_p->queue_p = queue_p;

    if ((client_p->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Unable to connect to interface: stage 1\n");
        free(*data_p);
        *data_p = NULL;
        return FALSE;
    }

    /* Construct the server sockaddr_in structure */
    memset(&client_p->addr, 0, sizeof(client_p->addr));      /* Clear struct */
    client_p->addr.sin_family      = AF_INET;                /* Internet/IP */
    client_p->addr.sin_addr.s_addr = inet_addr(location);    /* IP address */
    client_p->addr.sin_port        = htons(port);            /* Server port */

    client_p->connectionUp   = connUp;
    client_p->connectionDown = connDown;
    client_p->is_running     = TRUE;

    if (client_p->connectionUp == DISABLE_CONNECTION_UP)
    {
        /* Blocking function call */

        /* Establish connection */
        while(connect(client_p->fd,
                      (struct sockaddr *) &client_p->addr,
                      sizeof(client_p->addr)) < 0)
        {
            printf("Trying to connect to the server...\n");
            usleep(CONNECT_RETRY_DELAY);
        }
    }

    pthread_create(&client_p->thread, NULL, connect_socket, (void *)data_p);

    return TRUE;
}

Boolean_t Interface_disconnect(void ** data_p)
{
    InterfaceData_t * client_p = (InterfaceData_t *)(*data_p);

    if (client_p != NULL)
    {
        /* Stop thread */
        client_p->is_running = FALSE;
        pthread_join(client_p->thread, NULL);

        close(client_p->fd);
        free(client_p);

        *data_p = NULL;
    }

    return TRUE;
}

ConnectionId_t Interface_getServerId(void ** data_p)
{
    InterfaceData_t * interface_p = (InterfaceData_t *)(*data_p);

    if (interface_p == NULL)
    {
        return -1;
    }

    return interface_p->fd;
}

