/**
 * @mainpage Trabajo Práctico 1
 * @version 1.0
 * @file config_sockets.c
 * @author Ariel Rabinovich
 * @date Abil 2015
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

/**
 * @brief Esta función realiza la conexión del socket TCP en el puerto designado por port y en la direccion designada por srv_addr
 * @param (int port) Numero de puerto a conectar
 * @param (sockaddr_in *serv_addr) Puntero a una estructura donde retornar datos del servidor
 * @param (struct hostent *server) Puntero a una estructura que describe al servidor. Un servidor debe llamar con NULL
 * @return File descriptor id, -1 si hay error
 */
int configTCPsocket(int port, struct sockaddr_in *serv_addr, struct hostent *server){
	int sockfd = socket( AF_INET, SOCK_STREAM, 0 );	//sockfd es el file descriptor a retornar en caso de exito
	if ( sockfd < 0 ) {
		perror("Error en la apertura de socket");
		return (-1);
	}
	memset( (char *) serv_addr, 0, sizeof(*serv_addr) ); //borro el server_addr por si tenía info
	serv_addr->sin_family = AF_INET;
	if(server != NULL) memmove(&(serv_addr->sin_addr.s_addr),server->h_addr ,server->h_length );	//copio la direccion del server en la estructura serv_addr
	else serv_addr->sin_addr.s_addr = INADDR_ANY;	//si soy un servidor pido una dirección
	serv_addr->sin_port = htons(port);	//asigno el puerto
	
	return sockfd;
}

/**
 * @brief Esta función realiza la conexión del socket UDP en el puerto designado por port con el servidor descripto por server
 * @param (int port) Numero de puerto a conectar
 * @param (char srv_addr[]) Nombre de dominio o IP del servidor
 * @param (sockaddr_in *dest_addr) Puntero a una estructura donde retornar datos del destinatario
 * @param (struct hostent *server) Puntero a una estructura que describe al servidor. Un servidor debe llamar con NULL
 * @return File descriptor id, -1 si hay error
 */
int configUDPsocket(int port, struct sockaddr_in *dest_addr, struct hostent *server){
	int sockfd = socket( AF_INET, SOCK_DGRAM, 0 );
	if (sockfd < 0) {
		perror( "Error en la apertura de socket" );
		return (-1);
	}
	
	dest_addr->sin_family = AF_INET;
	dest_addr->sin_port = htons(port);
	if(server != NULL) dest_addr->sin_addr = *((struct in_addr *)server->h_addr);
	else dest_addr->sin_addr.s_addr = INADDR_ANY;
	memset( &(dest_addr->sin_zero), '\0', 8 );
	
	return sockfd;
}
