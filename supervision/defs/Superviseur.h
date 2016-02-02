//Date derniere modifs 10 decembre 2014; auteur JV
//

#ifndef SUPERVISEUR_H
#define SUPERVISEUR_H

#include <string>
#include <sstream>
#include <pthread.h>
#include <vector>
#include <stdexcept> 

#include "../defs/Etats.h"
#include "../defs/Empaqueteur.h"
#include "../defs/ProxySEIS.h"
#include "../defs/ListeMessages.h"
#include "../defs/Clients.h"
#include "../defs/automates.h"
#include "../defs/sockets.h"
#include "../defs/protocole.h"
#include "../defs/commandes.h"
#include "../defs/ICMP.h"
#include "../defs/verbose.h"

#define 	CHEMIN_FICHIER_CONF 		"./etc/_superviseur.conf"
#define 	PORT_SERV_LB			5000
#define 	DUREE_MAX_INIT_PANNEAUX		60 // en secondes

using namespace std;


class Superviseur
{
public:

	Superviseur ( );
	~Superviseur ( );
	void	diffuser();
	bool receptionCmds();
	bool inscriptionClients();
	void fermetureClients();
	void interpreterComandesNormales();
	void interpreterComandesSpeciales();
	void kernel();
	bool testerPeriph();
	bool InitPeriph();
	void ArretPropreDesPeriph();
	void surveillanceARRU();
	void surveillanceEnergie();
	void surveillanceCamera();
	void DemandeIHM(Etats::FLAGS_DEM_IHM val);
	void DemandeIHM_Energie(Etats::FLAGS_DEM_IHM val);
	void DemandeIHM_Incident(Etats::FLAGS_DEM_IHM val);
	void ConstruireMessageDEtat();
	bool SupervisionEnCours;
	bool testsOK;
	bool initOK;

	enum TYPE_MESSAGES
	{
		SEIS__VIBRATIONS='v',
		SEIS_ETATS='e'
	};
	ProxySEIS leSeis;
	AutomateBras automateBras;
	AutomatePanneaux automatePanneaux;
	AutomateCCGX CCGX;
	ListeMessages lesMessages;
	Etats etats;
	Empaqueteur MesuresAEnvoyer;
	Empaqueteur empaqueteur;
	Verbose v;
	ICN icn;
	ICS ics;
	AutomateBras autoBras;
	AutomatePanneaux autoPanneaux;
	AutomateCCGX autoCCGX;

	enum PROBLEMES_POSSIBLES
	{
		PAS_DE_RUPTURE_DE_FLUX		=	0x00,
		RUPTURE_DE_FLUX_PANNEAUX	=	0x01,
		RUPTURE_DE_FLUX_BRAS		=	0x02,
		RUPTURE_DE_FLUX_SEIS		=	0x03,
		RUPTURE_DE_FLUX_CCGX		=	0x04,
		ARRU_DEMANDE			=	0X06,
		TIMEOUT_PANNEAUX		=	0X08
	};

	string AntiRun_str(int anti);
	enum ANTI_RUN
	{
		RIEN		=0x00,
		ARRU_MATERIEL	=0x01,
		ARRU_LOGICIEL	=0x02,
		PROBLEME_ENERGIE=0x04,
		INCIDENT_PERIPH	=0x08,
		ARRET_DEMANDE	=0x10
	};
	pthread_mutex_t semAntiRun;
	pthread_cond_t SignalAntiRun;
	short NatureAntiRun;

// ------Mutex decision sur indicent péripherique
	pthread_mutex_t semDemIHM_Incident;
	pthread_cond_t decisionIMH_Incident_Recue;
	string reponseIHM_Incident;

// ------Mutex decision sur probelme energie
	pthread_mutex_t semDemIHM_Energie;
	pthread_cond_t decisionIMH_Energie_Recue;
	string reponseIHM_Energie;

// ------Mutex confiramtion pour les sequences normales
	pthread_mutex_t semDemIHM;
	pthread_cond_t confirmationIMHRecue;
	string reponseIHM;

// ------Mutex de protection à la liste de messages
	pthread_mutex_t accesListeMessages;
	pthread_cond_t nouveauMessage;

//------Mutex/Condition du thread de commande urgente-------
	pthread_mutex_t mutexCmdSpeciale;
	pthread_cond_t conditionCmdS;

//-----Mutex/Condition thread de commande normale--------
	pthread_mutex_t mutexCmdNormale;
	pthread_cond_t conditionCmdN;

//-----Mutex de gestion des ARRU--------
	pthread_mutex_t mutexGestionARRU;

	// Variables pour la caméra
	pthread_mutex_t mutexCamera;
	pthread_cond_t conditionCamera;
	int camTestTimeout;
	int camTestMax;
	int camTest;
	string adrsIpSeis;
	int portSeis;

	int canalDemonDiffusion;
protected:

	int portInscription;
	int portCommandes;
	string adrsIpAutomateBras;
	int portAutomateBras;
	string adrsIpAutomatePanneaux;
	int portAutomatePanneaux;
	string adrsIpCCGX;
	int portCCGX;
	string adrsIpCamera;
	int portCamera;
	int periodeARRU; // en secondes
	int periodeEnergie; //en secondes
	int periodeCamera; //en secondes
	int nbMaxObs;
	int nbObs;
	int NbMesuresParPaquet;
	std::vector<Client*> listeClients;
	pthread_mutex_t accesListeClients;
	string cmdS;
	string cmdU;
	string cmdN;
	unsigned short tensionMinimum;
	unsigned short ChargeMinimum;

private:
//	stringstream strStream;
	int erreurCode;


};

#endif // SUPERVISEUR_H
