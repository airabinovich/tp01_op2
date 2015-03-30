/**
 * @author Ariel Iván Rabinovich
 * @date Mar 2015
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#define TAM 256
#define puertoTCP 6020
#define puertoUDP 6021

int main( int argc, char *argv[] ) {
	int sockfd, newsockfd, clilen, pid;
	char buffer[TAM], stimestamp[TAM];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	time_t timestamp = time(&timestamp);

	sockfd = socket( AF_INET, SOCK_STREAM, 0);
	if ( sockfd < 0 ) { 
		perror( " apertura de socket ");
		exit( 1 );
	}

	memset( (char *) &serv_addr, 0, sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons( puertoTCP );	//htons transforma little endian en big endian si es necesario

	if ( bind(sockfd, ( struct sockaddr *) &serv_addr, sizeof( serv_addr ) ) < 0 ) {
		perror( "ligadura" );
		exit( 1 );
	}

	printf( "Proceso: %d - socket disponible: %d\n", getpid(), ntohs(serv_addr.sin_port) );

	listen( sockfd, 5 );
	clilen = sizeof( cli_addr );

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
				printf("\nBucle Principal\n");
				memset( buffer, 0, TAM );
				n = read( newsockfd, buffer, TAM-1 );
				if ( n < 0 ) {
					perror( "lectura de socket" );
					exit(1);
				}
				printf( "PROCESO %d. ", getpid() );
				printf( "Recibí: %s", buffer );
				if (!strcmp("updatetime",buffer)){
					timestamp = time(&timestamp);
					sprintf( stimestamp, "%d" , (int)timestamp);
					n =  write(newsockfd, stimestamp , strlen(stimestamp));
					if(n <0){
						perror("escritura en socket");
						exit(1);
					}
				}else{ 
					//~ buffer[strlen(buffer)-1] = '\0';
					n = write( newsockfd, "Obtuve su mensaje", 18 );
					if ( n < 0 ) {
						perror( "escritura en socket" );
						exit( 1 );
					}
					// Verificación de si hay que terminar
					if( !strcmp( "fin", buffer ) ) {
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
