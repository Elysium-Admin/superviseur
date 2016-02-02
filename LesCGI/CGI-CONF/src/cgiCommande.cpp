/* --------------------------------------------------------------------------
                            Projet Insight

    Nom du fichier : cgiCommande.cpp > cgiCommande.cgi
    Langage : CPP
    Description : Reception de la trame de commande en provenance de l'IHM en HTTP
                  Envoi de la commande en local au superviseur en UDP sur loopback
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
 *      navigateur       (HTTP)      CGI        (TCP local)    Superviseur
 *  -----------------------------------------------------------------------
 *        IHM            >>      cgiCommande.cgi     >>    démon de commande
 *     Tablette ou PC  		    Raspberry                Raspberry        
 *       GET HTTP          Processus fils Serveur Web        datagramme
 *       Javascript                   C++                       C++            
----------------------------------------------------------------------------- 
 */
// g++ ./src/cgiCommande.cpp  ../projet/lib/sockets.o  -o cgiCommande

#include <iostream>
#include <arpa/inet.h>
#include <stdlib.h>                         // Ne pas oublier pour getenv()
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h> 
#include <cstring>
#include <stdlib.h>
#include "strings.h"

#include "../../../supervision/defs/sockets.h"
#include "../../../supervision/defs/protocole.h"

using namespace std;

// pour la socket UDP vers le serveur
#define PORT_SERV_LB	5000
#define ADR_SERV_LB     "127.0.0.1"

//===============================================================================================
// Fonction d'envoi (en UDP sur loopback) d'une commande sur le demon de commandes du superviseur 
//===============================================================================================
int envoiCmd(string message) {
    struct sockaddr_in server_ad;       //  Déclaration structure de la socket
    // Identification du demon du superviseur à qui envoyer la cmd
    server_ad.sin_addr.s_addr = inet_addr(ADR_SERV_LB);
    server_ad.sin_port = htons(PORT_SERV_LB);
    server_ad.sin_family = AF_INET;
    // Création de la socket loopback qui nous donne la requète HTTP
    int canal = sockClientUDPspecLoopback();
    if (canal < 0) {
        return 0;                       // création impossible
    }
    // Envoi notre commande au demon du superviseur
    sendto (canal, message.c_str(), message.length(), 0,
                            (struct sockaddr*) &server_ad, sizeof(sockaddr_in));
    close(canal);
    return 0;
}

//===============================================================================================
// Fonction  qui relance le programme de supervision
void relanceSuperviseur()
{
	system("/home/pi/Elysium/outils/scripts/stopSuperviseur 2> /home/pi/Elysium/supervision/logs/cgicommande.error");
	sleep(5);
	system("nohup /home/pi/Elysium/outils/supervise&");
}


//=================================================================================
// corps du CGI
//=================================================================================
int main() {
    char* data = getenv("QUERY_STRING");    // récupération de la requete GET dans stdin
    string dt = (string)data;               // conversion en string
    // Vérifie si la cmd existe dans les paramètres de la requète
    size_t pos = dt.find("cmd=");
    if (pos != string::npos) {  // si la commande existe on l'envoi au demon
        string cmd = dt.substr (pos + 4, 3);
	if (cmd == S_RELANCER_SUPERVISION ){
		relanceSuperviseur();
	}else{
        	envoiCmd(cmd);          // ex:"cmd=cpo" ici on extrait 3 lettres "cpo"
	};
    }
    return 0;
}
//=================================================================================

