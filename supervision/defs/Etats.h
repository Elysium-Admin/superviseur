//Date derniere modifs 10 decembre 2014; auteur JV
//
#ifndef ETAT_H
#define ETAT_H

#define TAILLE_TRAME	96

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

#include <unistd.h> // pour usleep()
#define DUREE_ENTRE_DEUX_ENVOIES	150000	//en micro secondes

#include "../defs/ListeMessages.h"

using namespace std;

class Etats
{
private:
	unsigned char orgARRU;
	unsigned char etatSysteme;
	unsigned char SeqEnCours;
	unsigned char Mode;
	unsigned char Seis;
	unsigned char WTS;
	unsigned char HP3;
	unsigned char EtatPanneaux;
	unsigned char EtatPanneauxIHM;
	unsigned char EtatPince;
	unsigned char EtatBras;
	unsigned int CompteRenduSysteme;
	unsigned int IncidentPanneaux;
	unsigned int ErrSEIS;
	unsigned int demIHM;
	unsigned int CodeErrBrasPince;
	short TensionBatteries;
	short ChargeBatteries;
	short CourantBatteries;
	short ChargeConsommee;
	short DureeRestante;
	unsigned char IncidentEnergie;
	void contruireTrame();
	string Trame;
	pthread_mutex_t semEtat;
	ListeMessages* ptrListeMessages;
	Superviseur* ptrSuperviseur;
	unsigned char _Panneau1;
	unsigned char _Panneau2;
	bool NouveauMessage;
	pthread_mutex_t semNouveauMessage;
	pthread_cond_t NouveauMessageRecue;

public:
    enum FLAG_COMPTE_RENDU_SYSTEME
    {
        PAS_INCIDENT_A_SIGNALER		= 0x00000000,
        RUPTURE_LIAISON_SEIS		= 0x00000001,
        RUPTURE_LIAISON_BRAS		= 0x00000002,
	RUPTURE_LIAISON_PANNEAUX	= 0x00000004,
        RUPTURE_LIAISON_CCGX		= 0x00000008,
        RUPTURE_LIAISON_CAMERA		= 0x00000010,
        RUPTURE_SERVICE_HTTP_CAMERA	= 0x00000020,
        COMMANDE_INCONNUE		= 0x80000000,
        CCGX_NON_TESTE			= 0x00000040,
        PANNEAUX_NON_TESTES		= 0x00000080,
        SEIS_NON_TESTE			= 0x00000100,
        CAMERA_NON_TESTE		= 0x00000200,
        BRAS_NON_TESTE			= 0x00000400,
        CCGX_TESTS_EN_COURS		= 0x00000800,
        PANNEAUX_TESTS_EN_COURS		= 0x00001000,
        SEIS_TESTS_EN_COURS		= 0x00002000,
        CAMERA_TESTS_EN_COURS		= 0x00004000,
        BRAS_TESTS_EN_COURS		= 0x00008000
    };

    enum FLAGS_ORG_ARRU
    {
        PAS_ARRU	= 0x00,
        ARRU_MATERIEL	= 0x01,
        ARRU_LOGICIEL	= 0x02
    };
    enum VAL_ETAT_SYSTEME
    {
        ATTENTE			=0,
        TESTS_MISE_EN_ROUTE	=1,
        INITILISATION		=2,
        COMMANDES		=3,
        CLOTURE			=4,
        ARRU_DEMANDEE		=5
    };
    enum VAL_ETAT_PINCE
    {
        PINCE_OUVERTE		= 0,
        PINCE_EN_COURS_OUVERTURE= 1,
        PINCE_FERMEE		= 2,
        PINCE_EN_COURS_FERMETURE= 3
    };
    enum VAL_SEQ_EN_COURS
    {
        RIEN			=0,
        DEPOSE_SEIS		=1,
        REPRISE_SEIS		=2,
        DEPOSE_WTS		=3,
        REPRISE_WTS		=4,
        DEPOSE_HP3		=5,
        REPRISE_HP3		=6,
        PANORAMIQUE		=7,
        ARRET_SYSTEM		=8,
        OUVERTURE_PANNEAUX 	=9,
        FERMETURE_PANNEAUX 	=10
    };
    enum FLAGS_DEM_IHM
    {
        PAS_DE_DEMANDE				=0x00000000,
        DEM_CONF_OUVERTURE_PINCE		=0x00000001,
        DEM_CONF_POSE_HP3			=0x00000002,
        DEM_CONF_LACHER_WTS			=0x00000004,
        DEM_CONF_SAISIR_WTS			=0x00000008,
        DEM_CONF_SEIS_COMMENCER_NIVELLEMENT	=0x00000010,
        DEM_CONF_LACHER_SEIS			=0x00000020,
        DEM_CONF_SAISIR_SEIS			=0x00000040,
        DEM_CONF_LANCER_TESTS			=0x00000080,
        DEM_CONF_FERMETURE_SUR_PB_OUVERT_PAN_1	=0x00000100,
        DEM_CONF_FERMETURE_SUR_PB_OUVERT_PAN_2	=0x00000200,
        DEM_CONF_ROUVERTURE_SUR_PB_FERMET_PAN_1	=0x00000400,
        DEM_CONF_ROUVERTURE_SUR_PB_FERMET_PAN_2	=0x00000800,
        DEM_CONF_REMONTER_HP3_POUR_ARRET	=0x00001000,
        DEM_CONF_REMONTER_WTS_POUR_ARRET	=0x00002000,
        DEM_CONF_REMONTER_SEIS_POUR_ARRET	=0x00004000,
        DEM_CONF_FERMER_PANNEAUX_POUR_ARRET	=0x00008000,

        DEM_DECISION_BATTERIES_FAIBLES		=0x00010000,
        DEM_DECISION_SORTIE_TEST		=0x00020000,
        DEM_DECISION_SORTIE_INITIALISATION	=0x00040000,
        DEM_DECISION_DEFAUT_DETECTE		=0x00080000
    };

    // en état systeme = ATTENTE, tous les modes doivent être  HS
    // en etat systeme = TEST au fur et a mesure queles test se déroulent
    //   avec succes le péripherique qitte son status "HS"
    enum FLAGS_MODE
    {
        TOUT_BON		=0x00,
        M_ALARME_BATTERIES	=0x01,
        M_BATTERIES_HS 		=0x02,
        M_PANNEAUX_HS 		=0x04,
        M_BRAS_HS 		=0x08,
        M_SEIS_HS 		=0x10,
        HTTP_CAM_HS 		=0x80
    };
    enum VAL_REGISTRE_PANNEAUX
    {
        PANNEAUX_FERMES				=0,
        PANNEAUX_1_OUVERTURE_BRAS_EN_COURS	=1,
        PANNEAUX_1_OUVERTURE_VOLETS_EN_COURS	=2,
        PANNEAUX_2_OUVERTURE_BRAS_EN_COURS	=3,
        PANNEAUX_2_OUVERTURE_VOLETS_EN_COURS	=4,
        PANNEAUX_OUVERTS			=8,
        PANNEAUX_1_FERMETURE_VOLETS_EN_COURS	=9,
        PANNEAUX_1_FERMETURE_BRAS_EN_COURS	=10,
        PANNEAUX_2_FERMETURE_VOLETS_EN_COURS	=11,
        PANNEAUX_2_FERMETURE_BRAS_EN_COURS	=12,
        INCIDENT_OUVERTURE_PANNEAU_1		=21,
        REPLI_PANNEAU_1_SUR_INCIDENT		=22,
        INCIDENT_FERMETURE_PANNEAU_1		=23,
        REOUVERTURE_PANNEAU_1_SUR_INCIDENT	=24,
        INCIDENT_OUVERTURE_PANNEAU_2		=41,
        REPLI_PANNEAU_2_SUR_INCIDENT		=42,
        INCIDENT_FERMETURE_PANNEAU_2		=43,
        REOUVERTURE_PANNEAU_2_SUR_INCIDENT	=44,
        PHASE_INIT				=64,
        INCIDENT_REDHIBITOIRE_PANNEAUX		=128
    };
    enum VAL_REG_INIT_PANNEAUX
    {
        NON_INIT		= 0,
        INIT_EN_COURS 		= 1,
        INIT_BON 		= 2,
        PANNEAU1_SEUL		= 3,
        PANNEAU2_SEUL 		= 4,
        INIT_PB 		=128
    };
    enum VAL_ETAT_BRAS
    {
        INCONNUE			= 0,
        POSITION_PANORAMIQUE		= 1,
        SEIS_HORS_PLATEAU		= 2,
        PRET_A_SAISIR_SEIS		= 3,
        WTS_AU_SOL			= 4,
        PRET_A_SAISIR_WTS		= 5,
        PRET_DEPOSE_HP3_PLATEAU		= 6,
        BRAS_POSITION_TRANSPORT		= 7,
        INIT_BRAS_EN_COURS		= 32,
        ACTION_EN_COURS			= 64,
        INCIDENT_REDHIBITOIRE_BRAS	= 128
    };
    enum VAL_ETAT_PANNEAUX
    {
        PANNEAU_1_FERME 			= 0x00,
        PANNEAU_1_OUVERTURE_BRAS_EN_COURS 	= 0x01,
        PANNEAU_1_OUVERTURE_VOLETS_EN_COURS 	= 0x02,
        PANNEAU_1_OUVERT 			= 0x03,
        PANNEAU_1_FERMETURE_VOLETS_EN_COURS 	= 0x04,
        PANNEAU_1_FERMETURE_BRAS_EN_COURS 	= 0x05,
        INCIDENT_DURANT_OUVERTURE_PANNEAU_1 	= 0x06,
        REPLI_PANNEAU_1_SUITE_A_INCIDENT 	= 0x07,
        INCIDENT_DURANT_FERMETURE_PANNEAU_1 	= 0x08,
        REOUVERTURE_PANNEAU_1_SUITE_A_INCIDENT	= 0x09,
        PANNEAU_1_NON_INIT			= 0x0a,
        PANNEAU_1_INIT_EN_COURS			= 0x0b,
        PANNEAU_1_HS 				= 0x0f,

        PANNEAU_2_FERME 			= 0x00,
        PANNEAU_2_OUVERTURE_BRAS_EN_COURS 	= 0x10,
        PANNEAU_2_OUVERTURE_VOLETS_EN_COURS 	= 0x20,
        PANNEAU_2_OUVERT 			= 0x30,
        PANNEAU_2_FERMETURE_VOLETS_EN_COURS 	= 0x40,
        PANNEAU_2_FERMETURE_BRAS_EN_COURS 	= 0x50,
        INCIDENT_DURANT_OUVERTURE_PANNEAU_2 	= 0x60,
        REPLI_PANNEAU_2_SUITE_A_INCIDENT 	= 0x70,
        INCIDENT_DURANT_FERMETURE_PANNEAU_2 	= 0x80,
        REOUVERTURE_PANNEAU_2_SUITE_A_INCIDENT 	= 0x90,
        PANNEAU_2_NON_INIT			= 0xa0,
        PANNEAU_2_INIT_EN_COURS			= 0xb0,
        PANNEAU_2_HS 				= 0xf0
    };

    enum FLAGS_SITUATION_APPAREILS
    {
        INDETERMINE		=0x00,
        SUR_PLATEAU		=0x01,
        DEPOSE_EN_COURS		=0x02,
        AU_SOL			=0x04,
        REPRISE_EN_COURS	=0x08,
        INDISPONIBLE		=0x10,
        NIVELLEMENT		=0x20,
        FIN_NIVELL		=0x40,
        MESURES_EN_COURS	=0x80
    };

    enum FLAGS_ALARMES_ENERGIE
    {
        PAS_D_ALARME		=0x00,
        ALARME_TENSION_BASSE	=0x01,
        ALARME_TENSION_HAUTE	=0x02,
        ALARME_TEMPERATURE	=0x04,
        ALARME_STATUS_RELAIS	=0x08,
        TENSION_BASSE		=0x10,
        CHARGE_BASSE		=0x20
    };

    enum FLAGS_INCIDENTS_PANNEAUX
    {
        PAS_D_INCIDENT_PANNEAUX	=0x00000000,
        PANNEAU_1_HORS_SERVICE	=0x00000080,
        PANNEAU_2_HORS_SERVICE	=0x00008000
    };

	Etats();

	void diffuserEtat();
	void initEtats(ListeMessages* ptrlistemessages);
	void init(Superviseur * ptrSup);
	void setOrgARRU(unsigned char val);
	void setEtatSysteme(unsigned char val);
	void setSeqEnCours(unsigned char seq);
	void setMode(unsigned char flag);
	void setEtatSEIS(unsigned char val);
	void setEtatWTS(unsigned char val);
	void setEtatHP3(unsigned char val);
	void setEtatPanneaux(unsigned char val);
	void setEtatPince(unsigned char val);
	void setEtatBras(unsigned char etatBras);
	void setCompteRenduSysteme(unsigned int val);
	void setIncidentPanneaux(unsigned int val);
	void setErreurSEIS(unsigned int val);
	void SetDemIHM(unsigned int flag);
	void setCodeErreurBras(unsigned int val);
	void setTensionBatteries(short val);
	void setChargeBatteries(short val);
	void setCourantBatteries(short val);
	void SetChargeConsommee(short val);
	void setDureeRestanteBatteries(short val);
	void setIncidentBatteries(unsigned char val);
	void setInterConferencier(string  val);

    void SetflagIncidentPanneaux(unsigned int incident);
    void UnsetflagIncidentPanneaux(unsigned int incident);
    void SetflagIncidentBatteries(unsigned char val);
    void UnsetflagIncidentBatteries(unsigned char val);
    void SetflagDemIHM(unsigned int val);
    void UnSetflagDemIHM(unsigned int val);
    void SetFlagMode(unsigned char flag);
    void UnsetFlagMode(unsigned char flag);
    void AnnulerConfirmIHM(unsigned char flag);
    void SetflagCompteRenduSysteme(unsigned int flag);
    void UnsetflagCompteRenduSysteme(unsigned int flag);
    void SetFlagEtatSEIS(unsigned char flag);
    void UnsetFlagEtatSEIS(unsigned char flag);

	unsigned char getMode();
	unsigned char getEtatPanneaux();
	unsigned char getEtatPince();
	unsigned char getEtatBras();
	unsigned char getSituationSEIS();
	unsigned char getSituationWTS();
	unsigned char getSituationHP3();
	unsigned char getOrgARRU();
	unsigned char getEtatSysteme();
	unsigned char getIncidentBatteries();
	int GetDemIHM();
	string getTrame();

	string interConferencier;

};

#endif // ETAT_H
