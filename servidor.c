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

void register_user(int s_local, char* user)
{
    printf("s> OPERATION REGISTER FROM %s\n",user);
    FILE *f;
    f = fopen("users.txt", "r");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    // Comprueba si el usuario ya está registrado
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        // Elimina el salto de línea al final de la línea
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, user) == 0) {
            // El usuario ya está registrado, envía '1' al cliente
            send(s_local, "1", 1, 0);
            fclose(f);
            return;
        }
    }
    fclose(f);

    // Registra al nuevo usuario y envía '0' al cliente
    f = fopen("users.txt", "a");
    fprintf(f, "%s\n", user);
    send(s_local, "0", 1, 0);
    fclose(f);
    return;
}

void unregister_user(int s_local, char* user)
{
    printf("s> OPERATION UNREGISTER FROM %s\n",user);
    FILE *f;
    f = fopen("users.txt", "r");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    // Comprueba si el usuario ya está registrado, si es así lo borra
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        // Elimina el salto de línea al final de la línea
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, user) == 0) {
            // El usuario ya está registrado, envía '0' al cliente
            send(s_local, "0", 1, 0);
            fclose(f);
            // Borra el usuario
            FILE *f2;
            f2 = fopen("users.txt", "r");
            FILE *f3;
            f3 = fopen("users2.txt", "w");
            while (fgets(line, sizeof(line), f2)) {
                // Elimina el salto de línea al final de la línea
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, user) != 0) {
                    fprintf(f3, "%s\n", line);
                }
            }
            fclose(f2);
            fclose(f3);
            remove("users.txt");
            rename("users2.txt", "users.txt");
            return;
        }
    }
    
}
void connect_user(int s_local, char* user)
{
    printf("s> OPERATION CONNECT FROM %s\n",user);
    FILE *f;
    f = fopen("users.txt", "r");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    // Comprueba si el usuario ya está registrado
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        // Elimina el salto de línea al final de la línea
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, user) == 0) {
            // El usuario ya está registrado, envía '0' al cliente
            send(s_local, "0", 1, 0);
            fclose(f);
            return;
        }
    }
    fclose(f);
    // El usuario no está registrado, envía '1' al cliente
    send(s_local, "1", 1, 0);
    return;
}



int tratar_peticion(void *sockfd)
{
    int *sockfd_int = (int *)sockfd;
    int err;
    int s_local;
    
    char buffer[256];

    pthread_mutex_lock(&mutex);
    s_local = (*sockfd_int);
    busy = false;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    
    err = recv(s_local, buffer, 256, 0); 
    if (err == -1)
    {
        printf("Error in reception\n");
        close(s_local);
        return NULL;
    }

    char *op = strtok(buffer, " ");
    char *user = strtok(NULL, " ");
    if (op && strcmp(op, "REGISTER") == 0){ //REGISTER
        if (user) {
            register_user(s_local, user);
        } else {
            printf("No user provided\n");
        }
    }
    else if (op && strcmp(op, "UNREGISTER") == 0){ //UNREGISTER
        if (user) {
            unregister_user(s_local, user);
        } else {
            printf("No user provided\n");
        }
    }
    else if (op && strcmp(op, "CONNECT") == 0){ //CONNECT
        if (user) {
            printf("ENTRA\n");
            connect_user(s_local, user);
        } else {
            printf("No user provided\n");
        }
    }
    else {
        printf("Invalid operation\n");
    }

    busy = true;
    return NULL;
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