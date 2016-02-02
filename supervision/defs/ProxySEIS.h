//_____________________________________________________________________________________________________
//
// ligne de compilation :
//       g++ -c ProxySEIS.cpp -o ProxySEIS.o
//_____________________________________________________________________________________________________
#ifndef PROXYSEIS_H
#define PROXYSEIS_H

#include <string>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unistd.h> // pour usleep()

#define FREQUENCE_ECHANTILLON_VIBRATION 100  // en hz
#define PERIODE_ECHANTILLON_TEMP 300  // en secondes
#define ATTENTE_MAX_REPONSE_SYNCHRONE	15 // en seconde
#define ATTENTE_MAX_ETABLISSEMENT_CONNEXION	20 // en seconde
#define ATTENTE_ENTRE_DEUX_ENVOIS_ARRET_MESURES	144000	//en micro secondes
#define NB_MAX_TENTATIVES_ARRET		20
#define NB_MAX_TENTATIVES		3

using namespace std;

class Superviseur;

class ProxySEIS
{
public:

	ProxySEIS();
	virtual ~ProxySEIS ( );
	void initProxySeis(Superviseur * ptrSup);
	void ReceptionEnvoiSEIS ();
	bool SeConnecter (string AdresseIP, unsigned short port );
	void seDeconnecter ( );
	bool replierPieds(); 				//envoi Synchrone
	bool detecterSol(string& detection);  	//envoi AsynSynchrone avec variable de réponse
	bool nivellement();  				//envoi Synchrone
	bool nivellement(string &chaineRep); 		//envoi AsynSynchrone avec variable de réponse
	bool lancerMesureVibrations(); //envoi Asynchrone // duree en dixieme de seconde
	void arretMesuresVibrations(); 			//envoi Asynchrone
	bool tester(unsigned int &resutats);
	bool isConnecte(){return connecte;};

	enum CODE_ERREUR_SEIS
	{
		PAS_ERREUR			=0x00000000,
		BATTERIE_SEIS_FAIBLE		=0x00000001,
		CAPTEUR_PROMITE_1_EN_DEFAUT	=0x00000002,
		CAPTEUR_PROMITE_2_EN_DEFAUT	=0x00000004,
		CAPTEUR_PROMITE_3_EN_DEFAUT	=0x00000008,
		MOTEUR_1_ERREUR_EN_MONTEE	=0x00000010,
		MOTEUR_1_ERREUR_EN_DESCENTE	=0x00000020,
		MOTEUR_2_ERREUR_EN_MONTEE	=0x00000040,
		MOTEUR_2_ERREUR_EN_DESCENTE	=0x00000080,
		MOTEUR_3_ERREUR_EN_MONTEE	=0x00000100,
		MOTEUR_3_ERREUR_EN_DESCENTE	=0x00000200,
		POTENTIOMETRE_1_DEFECTUEUX	=0x00000400,
		POTENTIOMETRE_2_DEFECTUEUX	=0x00000800,
		POTENTIOMETRE_3_DEFECTUEUX	=0x00001000,
		ACCELEROMETRE_1_DEFECTUEUX	=0x00002000,
		ACCELEROMETRE_2_DEFECTUEUX	=0x00004000,
		ACCELEROMETRE_3_DEFECTUEUX	=0x00008000,
		ECHEC_CONNEXION_SEIS		=0x00010000,
		ERREUR_PROTOCOLE		=0x00800000,
	};
	// les deux chiffre de poids forts sont prévu pour les ereur dites "spontanées" du seis
	// (voir document de définitions des interfaces

protected:
	pthread_mutex_t attenteReponse;
	pthread_cond_t reponseRecue;
	pthread_mutex_t attenteConnexion;
	pthread_cond_t SeisConnecte;
	string reponse;
	bool ModeMesure;
	bool RecuMesure;
	bool connecte;
	int canal;
	int erreurCode;
	ostringstream flux;
	bool demandesynchrone;
	Superviseur * ptrSuperviseur;

private:
	char FL[3];
	bool EnvoiSyn (string cmd );
	bool EnvoiAssyn (string cmd );
	bool EnvoiAssyn(string cmd ,string& strrep);
	string* ptrVarReponse;

public :
	string getReponse ( )
	{
		return reponse;
	}
	bool getModeMesure ( )
	{
		return ModeMesure;
	}
};
#endif // PROXYSEIS_H
