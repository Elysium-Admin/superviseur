/* --------------------------------------------------------------------------
                            Projet Insight

    Nom du fichier : cgiStream.cpp > cgiStream.cgi
    Langage : CPP
    Description :   
        Envoi du stream de mise à jour de l'IHM
        Deux évènements sont possibles :
            'etat' : pour la stream des états du lander
            'vibration' : pour la stream des datas du sismomètre (X,Y,Z)
        Ce thread est client du thread de diffusion
    Organisme : Lycée International Victor Hugo Colomiers, BTS IRIS
    Chef de projet : J.Voyé, JP.Dumas
    Auteur 2014 : LAMAMY Bastien
    Version : v1.0 - 04/2014
    Auteur 2015 : 
    Version : v1.1 - 02/2015
-----------------------------------------------------------------------------
 *                              ARCHITECTURE
-----------------------------------------------------------------------------
 * 
 *      Superviseur    (TCP local)  CGI-STREAM     (HTTP)  navigateur
 *  -----------------------------------------------------------------
 *  Service de diffusion    >>      cgiStream.cgi      >>      IHM
 *       Raspberry                  Raspberry            Tablette ou PC
 *        Thread           Processus fils Serveur Web      Stream
 *         C++                      C++                   Javascript
-----------------------------------------------------------------------------
 */
// g++ ./src/cgiStream.cpp  ../projet/lib/sockets.o  -o cgiStream

#include <unistd.h>
#include <string.h>
#include "../../../supervision/defs/sockets.h"
#include <iostream>
#include "../../../supervision/defs/protocole.h"

#define ADR_SERV_LB "127.0.0.1" // adresse locale (loopback)
#define PORT_SERV_LB    5000        // port du service de diffusion du superviseur

int envoyerEtat(char* texte);
int envoyerVibration(char* texte);
int envoyerEntete();

// entete des trames SSL pour event stream pour les évènements 'etat' et 'vibration'
//=================================================================================
char enteteHTTP[ ] = "HTTP/1.1 200 OK\nCache-Control: no-cache;\nTransfer-Encoding: chunked;\nContent-Type: text/event-stream;charset=UTF-8;\nAccess-Control--Allow-Origin: *;\n\n";
char enteteEtat[ ] = "event: etat\ndata: ";
char enteteVibration[ ] = "event: vibration\ndata: ";


//=================================================================================
// corps du CGI STREAM
//=================================================================================
int main(void) {
   struct sockaddr_in serv;
   int taille,cmpt=0,codeEnvoiTrame;
   int canal;
   char *ptrTrame,*prtCourant;
   char texte[1500];
//   AnalyseurDetat analyseur;
   string trame;
   int deb,fin;
//Connection au loopback=======================================

	//Handler Serveur
	struct sockaddr_in serveur;
	serveur.sin_family=AF_INET;
 	serveur.sin_addr.s_addr=inet_addr(ADR_SERV_LB);
  	serveur.sin_port=htons(PORT_SERV_LB);
	//Taille du Handler du Serveur
	taille = sizeof( struct sockaddr );

	//Création de la socket TCP
	canal = sockClientTCPspecLoopback();

	//Test de connexion
	if(connect(canal,(struct sockaddr*)&serveur,taille)==-1){
	  	cerr<<"Echec Connexion au serveur "<<endl;
		return -1;
	};
	string natureClient=OBSERVATEUR;
	write(canal,natureClient.c_str(),natureClient.length());

	envoyerEntete();

//=============================================================
//Tant que le caractère de fin de trame n'est pas lu faire:
    do{
        cmpt = read(canal, texte, sizeof(texte));
        if (cmpt == -1) {
            cerr << "Rupture de Connexion\n";
            close(0); close(1); close(canal);
            return -1;
        }
        else if (cmpt == 0) {
            cerr << "Fermeture de Connexion\n";
            close(0); close(1); close(canal);
            return -1;
        } 
        else {
            texte[cmpt] = 0x00;             // traduit en chaine de cars
            char *ptrDebutTrame = texte;
            char *prtCourant = texte;
            while (*prtCourant != 0x00) {   // il peut y avoir +s trames
                if (*prtCourant != '\n')    // j'ai pas fini la trame
                    prtCourant++;
                else {
                    codeEnvoiTrame = 0;
                    *prtCourant = 0x00;     // remplace '\n' par un 0x00 de fin
                    prtCourant++;           // pointe la trame suivante
                    if (*ptrDebutTrame == 'e')
                        codeEnvoiTrame = envoyerEtat(ptrDebutTrame + 1);
                    else if (*ptrDebutTrame == 'v')
                        codeEnvoiTrame = envoyerVibration(ptrDebutTrame + 1);
                    ptrDebutTrame = prtCourant; // trame suivante si il y en a
                    // teste si erreur d'envoi de trame
                    if (codeEnvoiTrame == -1) {
                        cerr << "Echec de l'envoi, fermeture du canal \n";
                        close(0); close(1); close(canal);
                        return -1;
                    }
                }
            }
        }
    } while (1);        // on sort par les return 
    close(canal);       // Fermeture du canal
    return 0;
}

int envoyerEntete() {
    if (write(1, enteteHTTP, strlen(enteteHTTP)) == -1)
        return -1;
    return 0;
}

//=================================================================================
// Envoi Fin de trame des états au client
// Comme c'est un CGI le canal 1 envoi le texte au client
//=================================================================================
int envoyerEtat(char* texte) {
    char CRLF[5] = {0x0a, 0x0d, 0x0a, 0x0d, 0x00};
    if (write(1, enteteEtat, strlen(enteteEtat)) == -1)
        return -1;
    if (write(1, texte, strlen(texte)) == -1)
        return -1;
    if (write(1, CRLF, strlen(CRLF)) == -1)
        return -1;
    return 0;
}

//=================================================================================
// Envoi Fin de trame des données (X,Y,Z) du sismomètre au client
//=================================================================================
int envoyerVibration(char* texte) {
    char CRLF[5] = {0x0a, 0x0d, 0x0a, 0x0d, 0x00};
    if (write(1, enteteVibration, strlen(enteteVibration)) == -1)
        return -1;
    if (write(1, texte, strlen(texte)) == -1)
        return -1;
    if (write(1, CRLF, strlen(CRLF)) == -1)
        return -1;
    return 0;
}
