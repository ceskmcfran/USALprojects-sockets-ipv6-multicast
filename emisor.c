/*
** Fichero: cliente.c
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
#include <net/if.h>
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


#define MAXLINE 100
#define PUERTO 8713
#define INTERFAZ "eth0"
#define MULTICAST "ff15::33"
#define SALTOS 10
#define MENSAJE "Hola que tal"



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


    //Declaracion de variables
	char nombreDifusor[20];
	char mensaje[MAXLINE];
	char ipserver[100];
	char interfaz[25];
	int Iinterfaz;
	int puerto;
	int saltos;

    int s, n, addrlen;
    struct sockaddr_in6 emaddr;
    struct sockaddr_in6 receptddr;

    addrlen=sizeof(struct sockaddr_in6);
   



    //comprobacion de errores
     if(sigaction(SIGINT,&ss,NULL)==-1){
            perror("sigaction");
           
        }


    if(argc==6){
        
  
        sprintf(mensaje,"%s",argv[1]);
		sprintf(ipserver,"%s",argv[2]);
		sprintf(interfaz,"%s",argv[3]);
		puerto = atoi(argv[4]);
		saltos = atoi(argv[5]);

	}else{

		sprintf(mensaje,"%s",MENSAJE);
		sprintf(interfaz,"%s",INTERFAZ);
		sprintf(ipserver,"%s",MULTICAST);
		puerto = PUERTO;
		saltos = SALTOS;
	
	}
		bzero(&emaddr, sizeof(emaddr));
		

        /* Creación */
        if( (s = socket(AF_INET6, SOCK_DGRAM, 0)) < 0){
			perror("Socket: error al crear el socket\n");
			exit(-1);
		}

    
        /* Inicialización */
    	emaddr.sin6_family = AF_INET6;
		emaddr.sin6_addr = in6addr_any;
        emaddr.sin6_port = 0; //para el puerto 13 son necesarios permisos de administrador

        


		/* Asociación */
        if(bind(s, (struct sockaddr *) &emaddr, sizeof(emaddr)) == -1){
        	perror("Bind: Error al bindear en el emisor");
        	exit(1);
        }
	
		/*Difundir por la interfaz determinada*/
		Iinterfaz = if_nametoindex(interfaz);
		
		if(setsockopt(s,IPPROTO_IPV6,IPV6_MULTICAST_IF,(char *)&Iinterfaz,sizeof(Iinterfaz))==-1)
		{							
			perror("Llamada setsockopt para especificar interfaz de difusión");
			exit(1);
		}
		

		/*Para el numero de saltos */
		if(setsockopt(s,IPPROTO_IPV6,IPV6_MULTICAST_HOPS, &saltos,sizeof(saltos))==-1)
		{
			perror("Llamada setsockopt para especificar los saltos de difusión");
			exit(1);
		}


		receptddr.sin6_family = AF_INET6;
		//receptddr.sin6_addr = ipserver;
        receptddr.sin6_port = htons(puerto); //para el puerto 13 son necesarios permisos de administrador


		
		/*IPv6 a bin*/
		if(inet_pton(AF_INET6, ipserver, &receptddr.sin6_addr) <= 0){
			perror("inet_pton: error al convertir la direccion\n");
			exit(1);

		}

		while(1){


			if(sendto(s,mensaje,strlen(mensaje),0,(struct sockaddr *)&receptddr,addrlen)==-1){
				perror("emisorSento");
				exit(1);
	
			}
			sleep(1);
			

		}







}

/*---------------------------------------------*/
void manejadora(int signal){
/*---------------------------------------------*/
    
    printf("\tSe procede al cierre ordenado de la aplicacion\n");
    exit(0);


}
