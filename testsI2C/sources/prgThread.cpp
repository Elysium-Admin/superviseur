//================================================
//Date : 07/04/2015
//Nom du fichier : prgThread.cpp
//Auteur : DA CRUZ Thomas & GAYRAUD Alexandre
//Lieu : Lycée International Victor Hugo Colomiers
//================================================

#include <pthread.h>
#include <sys/types.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../defs/sockets.h"
#include "../defs/LectureI2C.h"
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <fstream>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define		CHEMIN_FICHIER_CONF		"./etc/_i2c.conf"

using namespace std;

string mess2;
int valAlarme = 0;

void *thread_envoi(void *)
{
char buffer[40];
socklen_t fromlen;
int resultrecv = 0;
int resultsendto = 0;
int sock = sockUDPspecLoopback(3200);
struct sockaddr_in sin = {0};
fromlen = sizeof sin;
valTemp v;

    cout << "Lancement du thread envoi" << endl;
    if(sock == -1)
    {
	cerr<< "Erreur lors de la création de la socket" << endl;
	exit(1);
    }
    while(1){
        cout << "Thread envoi dans le while" << endl;
        if(resultrecv = recvfrom(sock,buffer, strlen(buffer), 0, (sockaddr *)&sin, &fromlen) < 0)
        {
        cerr<< "Erreur lors du recvfrom()" << endl;
        }
        cout << "Le message est : " << mess2 << endl;
       int test = mess2.length();
        cout << test << endl;
        if(resultsendto = sendto(sock,mess2.c_str(),test, 0, (sockaddr *)&sin,fromlen) < 0)
        {
            cerr<< "Erreur lors du sendto()" << endl;
        }

    }
}

void *thread_init(void *)
{
//Initialisation des variables
string nom_fichier;
string chemin_fichier;
string fichier;
valTemp v;
int temps_echantillonage = 0;
fstream conf(CHEMIN_FICHIER_CONF);
string ligne;
string mess;
string parametre;
stringstream strStream;

	while( !conf.eof() )
	{
		getline(conf, ligne);
		strStream.clear();
		parametre=ligne.substr(0, ligne.find_first_of('=',0));
		if(parametre=="CHEMIN_FICHIER_LOG")
		{
			chemin_fichier=ligne.substr (1+ligne.find_first_of('=',0),ligne.length());

		}
		else if(parametre=="NOM_FICHIER")
		{
			nom_fichier=ligne.substr (1+ligne.find_first_of('=',0),ligne.length());

			}
		else if(parametre=="TEMP_ALARME")
		{
			strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
			strStream>>valAlarme;

		}
		else if(parametre=="TEMPS_ECHANTILLONAGE")
		{
			strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
			strStream>>temps_echantillonage;

		}
	}

//Fermeture du fichier
conf.close();
//Association du chemin et du nom du fichier
fichier = chemin_fichier + nom_fichier;
//Création du fichier
cout << "Ouverture du fichier " << fichier << endl;
ostringstream str_trame;
str_trame.str("");
ofstream log_1 (fichier.c_str());
log_1 << "ETAT\t" << "Capteur 1\t" << "ETAT\t" << "Capteur 2\t" << endl;
while(true)
{
	v = lectureI2C();
	//printf("Temp 1 : %d \n",v.temp1);
	//printf("Temp 2 : %d \n",v.temp2);
	if(v.temp1 > valAlarme)
	{
	log_1 << "A\t" << v.temp1 << "°C\t\t";
    str_trame << "A:" << v.temp1 << "°C  ";
    }
	else
	{
	log_1 << "N\t" << v.temp1 << "°C\t\t";
	str_trame << "N:" << v.temp1 << "°C  ";
	}
	if(v.temp2 > valAlarme)
	{
	log_1 << "A\t" << v.temp2 << "°C\t\t" << endl;
    str_trame << "A:" << v.temp2 << "°C  ";
	}
	else
	{
	log_1 << "N\t" << v.temp2 << "°C\t\t" << endl;
	str_trame << "N:" << v.temp2 << "°C  ";
	}
	mess2 = str_trame.str();
	str_trame.str("");
    log_1.flush();
	sleep(10);
    }
}





int main()
{
	void *retour;
	pthread_t threadEnvoi;
	pthread_t threadInit;
	if(pthread_create (&threadInit, NULL, thread_init, NULL) < 0)
	{
		cerr<<"Impossible de créer le thread" << endl;
		exit(1);
	}
	if(pthread_create (&threadEnvoi, NULL, thread_envoi, NULL) < 0)
	{
		cerr<<"Impossible de créer le thread" << endl;
		exit(1);
	}
	cout << "Lancement des threads" << endl;
	(void)pthread_join(threadEnvoi, &retour);
	(void)pthread_join(threadInit, &retour);
	cout << "Après join" << endl;
	return 0;


}

