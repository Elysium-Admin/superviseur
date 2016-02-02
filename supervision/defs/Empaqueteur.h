#ifndef EMPAQUETEUR_H
#define EMPAQUETEUR_H

#include <pthread.h>
#include <iostream>

#include "../defs/ListeMessages.h"

#define		LG_MESURE			18	//exemple +0012,-1023,+1245;

using namespace std;

class Superviseur;

class Empaqueteur
{

private:

	pthread_mutex_t semMesures;	
	int NbMesureParPaquet;
	int nbMesuresActuelle;
	char * ptrMessage;
	char * ptrCourant;
	ListeMessages* ptrListeMessages;
	string Trame;
	Superviseur* ptrSuperviseur;

public:

	Empaqueteur();
	void init(Superviseur * prtsup, int nbmesures , ListeMessages* ptrlistemessages);
	void init(Superviseur * prtsup);
	bool ajouter(char* mesure);
	

};

#endif //  EMPAQUETEUR_H
