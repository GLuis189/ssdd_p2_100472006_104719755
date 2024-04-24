#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include "lines.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int busy = true;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void register_user(char* user)
{
    FILE *f;
    f = fopen("users.txt", "a");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    fprintf(f, "%s\n", user);
    fclose(f);
    return;
}

int tratar_peticion(void *sockfd)
{
    int *sockfd_int = (int *)sockfd;
    int err;
    int s_local;
    
    char op[256];

    pthread_mutex_lock(&mutex);
    s_local = (*sockfd_int);
    busy = false;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    err = recv(s_local, op, 256, 0); 
    if (err == -1)
    {
        printf("Error in reception op\n");
        close(s_local);
        return NULL;
    }
    if (op == "REGISTER\0"){
        char user[256];
        
        err = recv(s_local, user, 256, 0);
        if (err == -1)
        {
            printf("Error in reception user\n");
            close(s_local);
            return NULL;
        }
        register_user(user);
    }
    
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: servidor.c -p P\n");
        return 0;
    }
    struct sockaddr_in server_addr, client_addr;
    socklen_t size;
    int sd, sc;
    int err;
    pthread_t thid;
    pthread_attr_t attr;

    // Inicializar el atributo
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // Crear el socket
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("SERVER: Error en el socket");
        return (0);
    }

    // Permite que se reuse el socket
    int val = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(int));

    // Inicializar
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind
    err = bind(sd, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1)
    {
        printf("Error en bind\n");
        return -1;
    }

    // Listen
    err = listen(sd, SOMAXCONN);
    if (err == -1)
    {
        printf("Error en listen\n");
        return -1;
    }

    size = sizeof(client_addr);

    printf("s> init server <local IP>: <%d>\n", atoi(argv[2]));

    // Aceptar la conexión en sí
    while (1)
    {
        // Nuevo socket
        sc = accept(sd, (struct sockaddr *)&client_addr, (socklen_t *)&size);
        if (sc == -1)
        {
            printf("Error en accept\n");
            return -1;
        }
        pthread_create(&thid, &attr, tratar_peticion, (void *)&sc);
        printf("s> new connection from %s\n", inet_ntoa(client_addr.sin_addr));

        while (busy)
        {
            pthread_cond_wait(&cond, &mutex);
            busy = true;
            pthread_mutex_unlock(&mutex);
        }
    }
    close(sd);
    return 0;
}