/**
 * @mainpage Trabajo Práctico 1
 * @version 1.0
 * @author Ariel Iván Rabinovich
 * @date Abril 2015
 * @file cli_i.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>
#include "config_sockets.c"
#define TAM 256
#define puertoTCP 6020
#define puertoUDP 6021

///@brief Cliente NTC. Llamar con la ip del server NTC. Si no se especifica se utiliza localhost
int main( int argc, char *argv[] ) {
	int sockTCPfd,n,sockUDPfd,u;
	struct sockaddr_in serv_addr,dest_addr;
	struct hostent *server;
	int 	terminar = 0,
			actualizar = 0;
	time_t 	timestamp = 0;
	FILE 	*log = fopen("cliente.log","w"),
			*reg = fopen("reg_tstmp.txt","r");
	char 	buffer[TAM],
			srvaddr[TAM] = "localhost",
			bufferUDP[TAM];
	
	if ( argc >= 2 ) {				//si me pasan un address
		strcpy(srvaddr,argv[1]);	//lo copio, sino por defecto se usa localhost
	}
	
	server = gethostbyname(srvaddr);
	if (server == NULL) {
		perror("Error, no existe el host" );
		exit( 0 );
	}
	
	if((sockTCPfd = configTCPsocket(puertoTCP, &serv_addr, server)) < 0){
		perror("Error en la conexión del socket TCP");
		exit(1);
	}
	if (connect( sockTCPfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr ) ) < 0 ) {	//trata de conectar al servidor
		perror("Error en la conexion");
		exit (1);
	}
	
	if((sockUDPfd = configUDPsocket(puertoUDP, &dest_addr, server)) < 0){
		perror("Error en la conexión del socket UDP\n");
		exit(1);
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
		fflush(log);
		//<\log>
		if(!strcmp("timestamp",buffer)){
			printf("%s",asctime(localtime(&timestamp)));
		}else{
			n = write( sockTCPfd, buffer, strlen(buffer) );
			if ( n < 0 ) {
				perror( "escritura de socket" );
				exit( 1 );
			}
			// Verificando si se escribió: updatetime
			else if(!strcmp("updatetime",buffer)){
				actualizar = 1;
			}
			// Verificando si se escribió: settime
			else if(!strcmp("settime",buffer)){
				while(!feof(reg)){
					memset(bufferUDP,'\0',TAM);
					u = fread(bufferUDP,sizeof(char),TAM,reg);
					bufferUDP[strlen(bufferUDP)] = '\0';
					if(u<0){
						printf("Error leyendo el archivo solicitado\n");
						break;
					}
					u = sendto( sockUDPfd, (void *)bufferUDP, TAM, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr) );	///@brief Se llama a sendto con parámetros (sockUDPfd, el file descriptor del socket),bufferUDP (buffer donde se encuentra la información a enviar), TAM (tamaño del buffer), 0 (banderas, valor por defecto, bloqueante), &dest_addr (dirección de la estructura del destinatario), sizeof(dest_addr) (tamaño de la estructura)
					if(u<0){
						printf("Error enviando el archivo solicitado\n");
						break;
					}
				}
			}
			// Verificando si se escribió: fin
			else if( !strcmp("fin",buffer) || !strcmp("exit",buffer)) {
				terminar = 1;
			}

			memset( buffer, '\0', TAM );
			n = read( sockTCPfd, buffer, TAM );
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
	fclose(reg);
	return EXIT_SUCCESS;
} 
