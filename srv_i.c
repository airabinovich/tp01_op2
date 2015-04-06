/**
 * @mainpage Trabajo Práctico 1
 * @version 1.0
 * @author Ariel Iván Rabinovich
 * @date Abril 2015
 * @file srv_i.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include "config_sockets.c"
#define TAM 256
#define puertoTCP 6020
#define puertoUDP 6021

int main( int argc, char *argv[] ) {
	int sockfd, newsockfd, clilen, pid, sockUDPfd;
	char buffer[TAM], stimestamp[TAM], bufferUDP[TAM];
	struct sockaddr_in serv_addr, cli_addr;
	int n,u;
	time_t timestamp = time(&timestamp);

	//<socket TCP>
	if((sockfd = configTCPsocket(puertoTCP, &serv_addr, NULL)) < 0){
		perror("Error en la creación del socket TCP");
		exit(1);
	}

	if ( bind(sockfd, ( struct sockaddr *) &serv_addr, sizeof( serv_addr ) ) < 0 ) {
		perror( "ligadura" );
		exit( 1 );
	}

	printf( "Proceso: %d - socket disponible: %d\n", getpid(), ntohs(serv_addr.sin_port) );

	listen( sockfd, 5 );
	clilen = sizeof( cli_addr );
	//<\socket TCP>
	//<socket UDP>
	if((sockUDPfd = configUDPsocket(puertoUDP, &serv_addr , NULL)) < 0 ){
		perror("Error en la creación del socket UDP");
		exit(1);
	}
	if( bind( sockUDPfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr) ) < 0 ) {
		perror( "ERROR en binding" );
		exit( 1 );
	}
	
	int tamano_direccion = sizeof( struct sockaddr );
	//<\socket UDP>
	while( 1 ) {
		newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, &clilen );
		if ( newsockfd < 0 ) {
			perror( "accept" );
			exit( 1 );
		}

		pid = fork(); 
		if ( pid < 0 ) {
			perror( "fork" );
			exit( 1 );
		}

		if ( pid == 0 ) {  // Proceso hijo
			close( sockfd );

			while ( 1 ) {
				memset( buffer, 0, TAM );
				n = read( newsockfd, buffer, TAM-1 );
				if ( n < 0 ) {
					perror( "lectura de socket" );
					exit(1);
				}
				printf( "PROCESO %d. ", getpid() );
				printf( "Recibí: %s\n", buffer );
				if (!strcmp("updatetime",buffer)){
					timestamp = time(&timestamp);
					sprintf( stimestamp, "%d" , (int)timestamp);
					n =  write(newsockfd, stimestamp , strlen(stimestamp));
					if(n <0){
						perror("escritura en socket");
						exit(1);
					}
				}else if(!strcmp("settime",buffer)){
						u = recvfrom( sockUDPfd, bufferUDP, TAM-1, 0, (struct sockaddr *)&serv_addr, &tamano_direccion );
						if(u<0){
							printf("Error recibiendo el archivo\n");
							break;
						}
						//~ printf("string recibido por UDP %s\n",bufferUDP);
						timestamp = atoi(bufferUDP);
						printf("Nuevo timestamp asignado al servidor: %s",asctime(localtime(&timestamp)));
						n = write( newsockfd, "Timestamp Actualizado en el Servidor", 37 );
						if ( n < 0 ) {
							perror( "escritura en socket" );
							exit( 1 );
						}
				}else{
					n = write( newsockfd, "Obtuve su mensaje", 18 );
					if ( n < 0 ) {
						perror( "escritura en socket" );
						exit( 1 );
					}
					// Verificación de si hay que terminar
					if( !strcmp( "fin", buffer ) || !strcmp("exit",buffer) ) {
						printf( "PROCESO %d. Como recibí 'fin', termino la ejecución.\n\n", getpid() );
						exit(0);
					}
					
				}
			}
		}
		else {	//proceso actual (servidor)
			printf( "SERVIDOR: Nuevo cliente, que atiende el proceso hijo: %d\n", pid );
			close( newsockfd );
		}
	}
	return 0; 
} 
