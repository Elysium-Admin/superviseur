//Date derniere modifs 10 decembre 2014; auteur JV
//
#ifndef COMMANDES_H
#define COMMANDES_H
#include <stdint.h>

#include <string>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime> // pour le chien de garde
#include <sys/time.h>

using namespace std;

class Superviseur;

#define RUPTURE_BRAS_SEUL	10
#define RUPTURE_BRAS_AVEC_OBJET	11
#define RUPTURE_SEIS_SEUL	12
#define SIGNAL_ARRU		20
#define RUPTURE_CONNEXION_PANNEAUX	30
#define PANNEAUX_HS			31
#define TIME_OUT_PANNEAUX		32

#define TEMPS_REACTION_PINCE			5	// en secondes
#define CHIEN_DE_GARDE_MOUVEMENT_PINCE		12	// en secondes
#define CHIEN_DE_GARDE_MOUVEMENT_PANNEAUX	180	// en secondes

//=================================================================================================================================
class ICN
{
public:
	ICN();
	void init(Superviseur * ptrsuperviseur);
	bool ouvrirPanneaux();
	bool fermerPanneaux();
	bool deposerSEIS();
	bool remonterSEIS();
	bool deposerWTS();
	bool remonterWTS();
	bool deposerHP3();
	bool remonterHP3();
	
protected:
	bool initChienDeGarde();
	bool ChienDeGardeDepasse(int duree);
	struct timeval depart;
	struct timeval actuel;
	Superviseur* ptrSuperviseur;
};

class ICS
{
public:
	ICS();
	void init(Superviseur * ptrsuperviseur);
	bool ouvrirPince();
	bool fermerPince();
	void positionnerBrasEnPanoramique();
	void SeisArretMesures();
	void SeisLancerMesures();
	void SeisReplierPieds();
	void SeisNiveller();
	void MettrePanneauxHS();

// ------Mutex decision sur indicent péripherique
//	pthread_mutex_t semCmdSpePince;
//	pthread_cond_t condSemSpePince;

protected:
	Superviseur* ptrSuperviseur;
	
};



#endif // COMMANDES_H
