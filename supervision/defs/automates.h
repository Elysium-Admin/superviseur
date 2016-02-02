#ifndef AUTOMATES_H
#define AUTOMATES_H

#include "../defs/ProxyAutomate.h"


//class Superviseur;

//==================================================================================================================
//==================================================================================================================
class AutomateBras : public ProxyAutomate
{
public:
	bool connecter(Superviseur * ptrsuperviseur, string Adrs,unsigned short port);
	void init(Superviseur * ptrSup);
	// Registres de commandes
	//=======================
	enum REGISTRES_DE_COMMANDES
	{
		REG_CMD_ARRU 		=101,
		REG_CMD_TESTS_INIT 	=102,
		REG_CMD_ACTION_BRAS 	=105,
		REG_CMD_SEQ 		=106,
		REG_CMD_ACTION_PINCE 	=107
	};

	// commandes
	//====================
	enum ARRU
	{
		PAS_ARRU	= 0,
		ARRU		= 1
	};

	enum CMD_ACTIONS_PINCE
	{
		PAS_NOUVELLE_ACTION	= 0,
		OUVRIR_PINCE		= 1,
		FERMER_PINCE		= 2
	};

	enum CMD_SPECIALES_PINCE
	{
		EFFACE_ACTION		= 0,
		OUVRERTURE_PINCE	= 60,
		FERMETURE_PINCE		= 61
	};

	enum CMD_ACTIONS_BRAS
	{
		AUCUNE					= 0,
		ARRET_BRAS				= 1,
		REMONTER_SEIS				= 2,
		FINIR_SEQ_REPRISE_WTS			= 3,
		RETOUR_POSITION_PANORAMIQUE		= 4,
		FINIR_SEQ_POSE_HP3			= 5,
		LANCER_CHIEN_DE_GARDE_NIVELLEMENT	= 6,
		ARRET_CHIEN_DE_GARDE_NIVELLEMENT	= 7,
		FINIR_SEQ_POSE_WTS			= 8
	};

	enum CMD_SEQUENCE
	{
		RIEN		=0,
		DEPOSE_SEIS	=1,
		REPRISE_SEIS	=2,
		DEPOSE_WTS	=3,
		REPRISE_WTS	=4,
		DEPOSE_HP3	=5,
		REPRISE_HP3	=6,
		PANORAMIQUE	=7,
		ARRET_SYSTEM	=8
	};
	enum CMD_TEST_INIT
	{
		PAS_TEST	= 0,
		TESTER		= 1
	};

	// Registres d'etats
	//====================
	enum REGISTRES_DETATS
	{
		REG_ETAT_ARRU		=103,
		REG_ETAT_BRAS		=108,
		REG_ETAT_SEQ_ENCOURS	=109,
		REG_ETAT_PINCE		=110,
		REG_ETAT_INIT_BRAS	=111,
		REG_DEFAUTS_BRAS	=116
	};
	enum ETAT_INIT_BRAS
	{
		NON_INIT 	= 0,
		INIT_EN_COURS 	= 1,
		INIT_BON	= 2,
		INIT_PB 	= 128
	};

	enum ETAT_SEQUENCE_COURANTE
	{
		AUCUNE_SEQUENCE_COURANTE	=0,
		UNE_SEQUENCE_EST_COURS		=1
	};

	enum INCIDENTS_REDHIBITOIRES_BRAS
	{
		BUTEE_DEPOSE=0x00000008,
	};

	bool lireEtatInitBras(uint16_t & lue);
	bool ecrireARRU(uint16_t val);
	bool lancerSequence(uint16_t seq);
	bool actionPince(uint16_t action);
	bool commandeSpecialePince(uint16_t action);
	bool sequenceEncours(int &execption);
	bool lireDefautsBras();
	bool lireEtatBras(uint16_t& val);
	bool lireEtatPince(uint16_t& val);
	bool lireARRU(uint16_t & arru);
	bool ecrireCmdActionBras(uint16_t action);
	bool ecrireInitBras(uint16_t val);
	unsigned int getDefauts();
	void relaterDansLog(int reg, uint16_t val, int sens);


protected:
	unsigned int defauts;
	bool _ecrire(int adrreg ,uint16_t val);// traite les ruptures de connexion
	string nomDuRegistre(int reg);
	bool _lire(int adrreg, uint16_t *buffer);// traite les ruptures de connexion

// déplacemnet du poiteur de supervisuer vers la classe parent fait le 25 Mars 2015

};

//==================================================================================================================
//==================================================================================================================
class AutomatePanneaux : public ProxyAutomate
{
public:
	bool connecter(Superviseur * ptrsuperviseur, string Adrs,unsigned short port);
	void deconnecter();
	void init(Superviseur * ptrSup);

	enum REGISTRES_DE_COMMANDES
	{
		REG_ARRU		=101,
		REG_CMD_PANNEAUX	=120,
		REG_CMD_TESTS_INIT	=102
	};
	bool ecrireCmdPanneaux(uint16_t val);
	bool ecrireARRU(uint16_t val);
	bool ecrireInitPanneaux(uint16_t val);

	enum REGISTRES_DETATS
	{
		REG_ETAT_ARRU 		=103,
		REG_ETAT_PANNEAUX	=121,
		REG_ETAT_INIT_PANNEAUX	=122,
		REG_DEFAUTS_PANNEAUX	=125
	};
	bool lireARRU(uint16_t & arru);
	bool lireEtatPanneaux(uint16_t & lue);
	bool lireEtatPanneaux();
	bool lireEtatInitPanneaux(uint16_t & lue);
	bool lireDefautsPanneaux();
	unsigned int getDefauts();
	void relaterDansLog(int reg, uint16_t val, int sens);


	enum ETAT_INIT_PANNEAUX
	{
		NON_INIT 	= 0,
		INIT_EN_COURS 	= 1,
		INIT_BON	= 2,
		PANNEAU1_SEUL	= 3,
		PANNEAU2_SEUL 	= 4,
		INIT_PB 	= 128
	};

	// commandes
	//====================
	enum ARRU
	{
		PAS_ARRU	= 0,
		ARRU		= 1
	};
	enum CMD_PANNEAUX
	{
		RIEN 				= 0,
		OUVRIR_PANNEAUX 		= 1,
		FERMER_PANNEAUX 		= 2,
		ARRET_SYSTEM			= 3,
		REPLIER_PANNEAU_EN_PANNE 	= 4,
		REOUVRIR_PANNEAU_EN_PANNE 	= 5
	};
	enum CMD_TEST_INIT
	{
		PAS_TEST	= 0,
		TESTER		= 1
	};
protected:
	string nomDuRegistre(int reg);
	bool _ecrire(int adrreg ,uint16_t val);// traite les ruptures de connexion
	bool _lire(int adrreg, uint16_t *buffer);// traite les ruptures de connexion
	unsigned int defauts;


// déplacemnet du poiteur de supervisuer vers la classe parent fait le 25 Mars 2015

};


//==================================================================================================================
#define		UNIT_ID_CCGX1	247
#define		UNIT_ID_CCGX2	245
//==================================================================================================================

class AutomateCCGX : public ProxyAutomate
{
public:
	AutomateCCGX():ProxyAutomate(UNIT_ID_CCGX1) {};
	bool connecter(Superviseur * ptrsuperviseur, string Adrs,unsigned short port);
	void deconneter();
	void init(Superviseur * ptrSup);
	
	enum REGISTRES_DETATS
	{
		REG_ETAT_TENSION_BATTERIES	=259,
		REG_ETAT_COURANT_BATTERIES 	=261,
		REG_ETAT_CHARGE_CONSOMMEE	=265,
		REG_ETAT_CHARGES_BATTERIES	=266,
		REG_ETAT_STATUS_ALARMES		=267,
		REG_ETAT_DUREE_RESTANTE		=301
	};

	enum REGISTRES_DALARMES
	{
		REG_ALARME_TENTION_BASSE	=268,
		REG_ALARME_TENTION_HAUTE	=269,
		REG_ALARME_TEMPERATURE		=274,
		REG_STATUS_RELAIS		=280
	};

	enum INCIDENT_BATTERIE
	{
		PAS_DINCIDENT		=0x00,
		INCIDENT		=0x02
	};

	bool  lireTensionBatteries(unsigned short &tension);
	bool  lireCourantBatterie(short &courant);
	bool  lireChargeConsommee(short &chargeconsommee);
	bool  lireChargeBatterie(unsigned short &chargerestante);
	bool  lireChargeDureeRestante(unsigned short &duree);
	bool lireStatusAlarmes();
	void  relaterDansLog(int reg, uint16_t val);

protected:
	string nomDuRegistre(int reg);
	bool _lire(int adrreg, short *buffer);// traite les ruptures de connexion

// déplacemnet du poiteur de supervisuer vers la classe parent fait le 25 Mars 2015

};
//==================================================================================================================

#endif // AUTOMATES_H
