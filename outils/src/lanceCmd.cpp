// Auteur : Joel Voyé	; date 03/01/2015
//=======================================
// démon pour les clients 'clientMkcmd' et 'clientCmd'  
//
// il y a juste a le lancer sur la machine cible et elle execute des action des deux clients
//
// ligne de compilation :  g++ ./sources/lanceCmd.cpp ./lib/sockets.o -o lanceCmd


#include <unistd.h>
#include "../../supervision/defs/sockets.h"
#include <iostream>
#include <errno.h> 
#include <stdlib.h>
#include <string.h>

#include <sys/wait.h>

#define PORT_SERV	12346
#define LGCMD		255
#define 	MKSCRIPT	"mkscript"


using namespace std;


int main()
{
    struct sockaddr_in exp;
    int can_service;
    int canalDemon;
    int canal_in,canal_out;
    int cmpt;
    int taille=sizeof(struct sockaddr);
    pid_t cpid;
    int status;
    char *arguments[] = { NULL, NULL  };
     if ((canalDemon =sockTCPtoutesIF(PORT_SERV))==-1){
	return 0;
    }
    listen(canalDemon ,2);
    while(1){
	if((can_service=accept(canalDemon,(struct sockaddr*)&exp,(socklen_t *)&taille))>=0){

	   do {
    		cpid = fork();
    	   } while ((cpid == -1) && (errno == EAGAIN));
	   switch (cpid) {
		/* Si on a une erreur irrémédiable (ENOMEM dans notre cas) */
		case -1:
			cerr<<"erreur fork :"<<errno<<endl;
			return 0;
			break;
		case 0:
		/* on est dans le fils */
			arguments[0]=(char *)malloc(LGCMD);
	    		cmpt=read(can_service,arguments[0],LGCMD); 
			*(char*)(arguments[0]+cmpt)=0x00;
			close(0);dup(can_service);
			close(1);dup(can_service);
	
	cout<<"lancemant de "<< (string) arguments[0]<<" dans 1 seconde"<<endl;
	sleep(1);

			if (execv(arguments[0], arguments) == -1) {
    				cerr<<"erreur :"<<errno<<" lors du lancement de "<<arguments[0] <<endl;
	   		};

//			system(arguments[0]);
	cout<<" "<< (string) arguments[0]<<" est lancé fermeture canal dans 1 seconde"<<endl;
	sleep(1);
			free(arguments[0]);
			close(can_service);
	abort ();
			return 0;
			break;
    		default:
		/* on est dans le père */
//			wait (&status);
 			close(can_service);
			break;
	   }
	}
    }
    return 0;
}
