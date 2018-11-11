/*
** Fichero: servidor.c
** Autores:
** David Flores Barbero DNI 70907575R
** Francisco Blázquez Matías DNI 70919093L
** Usuario: i0907575
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <net/if.h>


#define PUERTO 8713
#define INTERFAZ "eth0"
#define MULTICAST "ff15::33"
#define MAXLINE 100



/*--------------PROTOTIPOS---------------------*/
void manejadora(int s); //Registra la señal sigint y finaliza ordenadamente la aplicacion 
/*---------------------------------------------*/




/*---------------------------------------------*/
int main (int argc, char * argv[]){
/*---------------------------------------------*/


 //variable de uso de la manejadora
    struct sigaction ss;
    ss.sa_handler=manejadora;
    sigemptyset(&ss.sa_mask);
    ss.sa_flags = 0;

	int msgLen;
	int litenfd, connfd;
	socklen_t len;
	char buff[MAXLINE];
	struct sockaddr_in6 servaddr,recibiraddr;
	extern const struct in6_addr in6addr_any;
    char ipFuente[100];
	char interfaz[25];
	int puerto,s,addrlen;
	char mensaje[MAXLINE];
	struct ipv6_mreq ipv6mreq; //Estructura para el ADD_MEMBERSHIP que especifica la interfaz

	addrlen=sizeof(struct sockaddr_in6);


	//comprobacion de errores
     if(sigaction(SIGINT,&ss,NULL)==-1){
            perror("sigaction");
           
        }


    if(argc==4){
        
		sprintf(ipFuente,"%s",argv[1]);
        sprintf(interfaz,"%s",argv[2]);
		puerto=atoi(argv[3]);

	}else{

		sprintf(interfaz,"%s",INTERFAZ);
		sprintf(ipFuente,"%s",MULTICAST);
		puerto = PUERTO;	
	}

		bzero(&servaddr, sizeof(servaddr));
		

	if ( (s = socket(AF_INET6, SOCK_DGRAM, 0)) < 0){

		perror("Socket: error al crear el socket\n");
		exit (1);

	}
		
	 /* Inicialización */
	servaddr.sin6_family = AF_INET6;
	servaddr.sin6_addr = in6addr_any;
    servaddr.sin6_port = htons(puerto); //para el puerto 13 son necesarios permisos de administrador

    /* Asociación */
    if(bind(s, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1){
    	perror("Bind: Error al bindear en el receptor");
    	exit(1);
    }

	ipv6mreq.ipv6mr_interface = if_nametoindex(interfaz);
	
	if (inet_pton(AF_INET6, ipFuente, &ipv6mreq.ipv6mr_multiaddr) <= 0) {
		perror("inet_pton: error al convertir la direcciOn\n");
		exit (1);
	}
		
	/* Unirse al grupo multicast */

	if(setsockopt(s,IPPROTO_IPV6,IPV6_ADD_MEMBERSHIP,&ipv6mreq,sizeof(ipv6mreq))==-1){
		perror("Llamada setsockopt para multicast\n");
		exit(1);	

	}

	

	while(1){

		if((msgLen = (recvfrom(s,mensaje,MAXLINE,0,(struct sockaddr *)&recibiraddr,&addrlen)))==-1){
		
			perror("Recvfrom: Error en la recepción de mensajes");
			exit(1);
		}
		
		//fprintf(stdout,"la longitud: %d\n",msgLen);
		fflush(stdout);
		mensaje[msgLen]='\0';
		fprintf(stdout,"El mensaje es: %s\n",mensaje);
			

	}


}



/*---------------------------------------------*/
void manejadora(int signal){
/*---------------------------------------------*/
    
    printf("\tSe procede al cierre ordenado de la aplicacion\n");
    exit(0);


}


