#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>
#define TAM 256
#define puertoTCP 6020
#define puertoUDP 6021

int main( int argc, char *argv[] ) {
	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	int terminar = 0, actualizar = 0;
	time_t timestamp = 0;
	FILE *log = fopen("cliente.log","w");
	char buffer[TAM];
	
	if ( argc < 2 ) {
		fprintf( stderr, "Uso %s host address\n", argv[0]);
		exit( 0 );
	}

	sockfd = socket( AF_INET, SOCK_STREAM, 0 );	//sockfd es un file descriptor
	if ( sockfd < 0 ) {
		perror( "ERROR apertura de socket" );
		exit( 1 );
	}

	server = gethostbyname( argv[1] );
	if (server == NULL) {
		fprintf( stderr,"Error, no existe el host\n" );
		exit( 0 );
	}
	memset( (char *) &serv_addr, 0, sizeof(serv_addr) ); //cambie '0' por 0
	serv_addr.sin_family = AF_INET;
	bcopy( (char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length );
	serv_addr.sin_port = htons( puertoTCP );
	if ( connect( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr ) ) < 0 ) {
		perror( "conexion" );
		exit( 1 );
	}

	while(1) {
		printf( ">: " );
		memset( buffer, '\0', TAM );
		fgets( buffer, TAM-1, stdin );
		buffer[strlen(buffer)-1] = '\0';
		
		//<log>
		time_t tiempolocal = time(&tiempolocal);
		char *stiempolocal = asctime(localtime(&tiempolocal));
		stiempolocal[strlen(stiempolocal)-1]='\0';
		fprintf(log,"%s: %s\n",stiempolocal,buffer);
		//<\log>
		
		if(!strcmp("timestamp",buffer)){
			printf("%s",asctime(localtime(&timestamp)));
		}else{
			n = write( sockfd, buffer, strlen(buffer) );
			if ( n < 0 ) {
				perror( "escritura de socket" );
				exit( 1 );
			}
			// Verificando si se escribió: updatetime
			if(!strcmp("updatetime",buffer)){
				actualizar = 1;
			}
			// Verificando si se escribió: fin
			if( !strcmp( "fin", buffer ) ) {
				terminar = 1;
			}

			memset( buffer, '\0', TAM );
			n = read( sockfd, buffer, TAM );
			if ( n < 0 ) {
				perror( "lectura de socket" );
				exit( 1 );
			}
			if(actualizar){
				timestamp = atoi(buffer);
				printf("Timestamp actualizado\nNuevo timestamp: %s",asctime(localtime(&timestamp)));
				actualizar = 0;
			}else{
				printf( "Respuesta: %s\n", buffer );
			}
			if( terminar ) {
				printf( "Finalizando ejecución\n" );
				exit(0);
			}
		}
	}
	fclose(log);
	return 0;
} 
