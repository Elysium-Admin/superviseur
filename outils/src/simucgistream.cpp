//=====================================================
// Projet Insight
//
// Description: simule un cgi stream
// Nom du fichier: simucgistream.cgi
// Auteur:
// Organisme : Lycée International Victor Hugo Colomiers, BTS IRIS
//
// Date: 13/12/2014
//======================================================
//
// g++ ./src/simucgistream.cpp  ../supervision/lib/sockets.o ./lib/Analyseur.o -o simucgistream


#include <unistd.h>
#include <string.h>
#include <iostream>
#include "../defs/Analyseur.h"
#include "../../supervision/defs/sockets.h"
#include "../../supervision/defs/Clients.h"
#include "../../supervision/defs/protocole.h"

// ports des services loopback du superviseur
#define 	PORT_SERV_LB			5000

using namespace std;

//=============================================================
int main(void)
{
	struct sockaddr_in serv;
	int taille,cmpt=0,code;
	int canal;
	char *ptrTrame,*prtCourant;
	char texte[1500];
	AnalyseurDetat analyseur;
	string trame;
	int deb,fin;
//Connection au loopback=======================================

	//Handler Serveur
	struct sockaddr_in serveur;
	serveur.sin_family=AF_INET;
	serveur.sin_addr.s_addr=inet_addr("127.0.0.1");
	serveur.sin_port=htons(PORT_SERV_LB);
	//Taille du Handler du Serveur
	taille = sizeof( struct sockaddr );

	//Création de la socket TCP
	canal = sockClientTCPspecLoopback();

	//Test de connexion
	if(connect(canal,(struct sockaddr*)&serveur,taille)==-1)
	{
		cerr<<"Echec Connexion au serveur "<<endl;
		return -1;
	};
	string natureClient=CONFERENCIER;
	write(canal,natureClient.c_str(),natureClient.length());

//=============================================================
//Tant que le caractère de fin de trame n'est pas lu faire:
	do
	{
		if((cmpt=read(canal,texte,sizeof(texte)))==-1)
		{
			strcpy(texte,":( rupture de Connexion :(\n");
			close(0);
			close(1);
			close(canal);
			return -1;
		}
		else if(cmpt==0)
		{
			strcpy(texte,":( fermeture de Connexion :( \n");
			close(0);
			close(1);
			close(canal);
			return -1;
		}
		else
		{
			texte[cmpt]=0x00;
			ptrTrame=texte;
			prtCourant=texte;
			while(*prtCourant!=0x00)
			{
				if(*prtCourant!='\n')prtCourant++;
				else
				{
					*prtCourant=0x00;
					prtCourant++;
					if (*ptrTrame=='e')
					{
						trame=ptrTrame;
						analyseur.analyserTrame(trame);
						ptrTrame=prtCourant;
					}
					else if (*ptrTrame=='v')
					{
						trame=ptrTrame;
						analyseur.afficheMesures(trame);
						ptrTrame=prtCourant;
					}
				}

			}
		}
		if(code==-1)
		{
			strcpy(texte,":( échec de l'envoie, fermeture du canal :(\n");
			close(0);
			close(1);
			close(canal);
			return -1;
		}

	}
	while(1);
	return 0;
}


