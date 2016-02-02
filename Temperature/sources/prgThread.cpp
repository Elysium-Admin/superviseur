//================================================
//Date : 07/04/2015
//Nom du fichier : prgThread.cpp
//Auteur : DA CRUZ Thomas & GAYRAUD Alexandre
//Lieu : Lycée International Victor Hugo Colomiers
//================================================

#include <pthread.h>
#include <sys/types.h>
#include <iostream>
#include <iomanip>
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

#define	CHEMIN_FICHIER_CONF "/home/pi/Elysium/Temperature/etc/_i2c.conf"

using namespace std;

string dernieresMesures;

//========================================================================================
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
//        cout << "Thread envoi dans le while" << endl;
        if(resultrecv = recvfrom(sock,buffer, strlen(buffer), 0, (sockaddr *)&sin, &fromlen) < 0)
        {
            cerr<< "Erreur lors du recvfrom()" << endl;
        }
//        cout << "Le message est : " << dernieresMesures << endl;
	int test = dernieresMesures.length();
//        cout << test << endl;
        if(resultsendto = sendto(sock,dernieresMesures.c_str(),test, 0, (sockaddr *)&sin,fromlen) < 0)
        {
            cerr<< "Erreur lors du sendto()" << endl;
        }

    }
}

//========================================================================================
void *thread_init(void *)
{
    //Initialisation des variables
    string nom_fichier;
    string fichier;
    int nombreDhistoriques;
    string chemin_fichier;
    valTemp v;
    fstream conf(CHEMIN_FICHIER_CONF);
    string ligne;
    string mess;
    string parametre;
    stringstream strStream;
    int valAlarme = 0;
    int PeriodeSurveillance = 30;// en minutes

    while( !conf.eof() )
    {
	getline(conf, ligne);
	strStream.clear();
	parametre=ligne.substr(0, ligne.find_first_of('=',0));
	if(parametre=="CHEMIN_FICHIER_DATAS")
	{
		chemin_fichier=ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
	}
	else if(parametre=="NOMBRE_HISTORIQUES")
	{
		strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
		strStream>>nombreDhistoriques;
	}
	else if(parametre=="TEMPERATURE_ALARME")
	{
		strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
		strStream>>valAlarme;
	}
	else if(parametre=="PERIODE_DE_SURVEILLANCE")
	{
		strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
		strStream>>PeriodeSurveillance;
	};
    }
    //Fermeture du fichier
    conf.close();
    //Association du chemin et du nom du fichier
    fichier = chemin_fichier + nom_fichier;

	strStream.clear();
	string nom_base=chemin_fichier + "/TemperaturesLander";
	string str1,str2;
	strStream << nom_base<<"-"<<setfill('0') << setw(2) << nombreDhistoriques << ".datas";
	strStream >> str1;
	remove(str1.c_str());
	for(int i=nombreDhistoriques-1 ; i>0 ; i--){
		str2=str1;
        	strStream.clear();
		strStream.str("");
		strStream << nom_base<<"-"<<setfill('0') << setw(2) << i << ".datas";
		strStream >> str1;
		rename(str1.c_str(), str2.c_str());
	}
        strStream.clear();
	strStream.str("");
	strStream << nom_base<<".datas";
	strStream >> nom_base;
	rename(nom_base.c_str(), str1.c_str());


    //Création du fichier
    cout << "Ouverture du fichier " << fichier << endl;
    ostringstream str_trame;
    str_trame.str("");
    ofstream Datas (nom_base.c_str());
    Datas << "ETAT\t" << "Capteur 1\t" << "ETAT\t" << "Capteur 2\t" << endl;
    while(true)
    {
	v = lectureI2C();
	if(v.temp1 > valAlarme)
	{
		Datas << "A\t" << v.temp1 << "°C\t\t";
    		str_trame << "A:" << v.temp1 << "°C  ";
    	}
	else
	{
		Datas << "N\t" << v.temp1 << "°C\t\t";
		str_trame << "N:" << v.temp1 << "°C  ";
	};
	if(v.temp2 > valAlarme)
	{
		Datas << "A\t" << v.temp2 << "°C\t\t" << endl;
		str_trame << "A:" << v.temp2 << "°C  ";
	}
	else
	{
		Datas << "N\t" << v.temp2 << "°C\t\t" << endl;
		str_trame << "N:" << v.temp2 << "°C  ";
	};
	dernieresMesures = str_trame.str();
	str_trame.str("");
	Datas.flush();
	sleep(PeriodeSurveillance*60); // pour traduire en secondes
    }
}


//========================================================================================
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

