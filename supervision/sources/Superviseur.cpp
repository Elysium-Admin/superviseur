//Date derniere modifs 10 decembre 2014; auteur JV
//
#include "../defs/Superviseur.h"
#include <math.h>
#include <fcntl.h> // definitions de constantes pour configurer la socket en non bloquante
#include <cstring>
#include <string.h>
#include <unistd.h>
#include <errno.h> // pour EBUSY


//=====================================================================================================
Superviseur::Superviseur ( ) // version 2015
{
    stringstream strStream;
    // Initialisation des mutex
    pthread_mutex_init(&semDemIHM, NULL);
    pthread_mutex_init(&semAntiRun, NULL);
    pthread_mutex_init(&accesListeMessages, NULL);
    pthread_mutex_init(&accesListeClients, NULL);
    pthread_mutex_init(&mutexCmdSpeciale, NULL);
    pthread_mutex_init(&mutexCmdNormale, NULL);
    pthread_mutex_init(&mutexCamera, NULL);
    pthread_mutex_init(&mutexGestionARRU, NULL);

    //-----Initialisation variable-------------------
    SupervisionEnCours=false;
    testsOK=false;
    initOK=false;
    periodeARRU=3; // en secondes
    periodeEnergie=60; // en secondes
    periodeCamera=10; // en secondes
    nbMaxObs=0;
    nbObs=0;
    NbMesuresParPaquet=50;
    reponseIHM="";
    portCommandes=PORT_SERV_LB;
    tensionMinimum=220;// en dixeme de volts
    ChargeMinimum=40;//en dixieme d'ampere.heure
    //-----------------------------------------------
    int val;
    string chaine;

    lesMessages.initListe(this);
	icn.init(this);
	ics.init(this);
	etats.initEtats(&lesMessages);
	etats.init(this);
	autoBras.init(this);
	autoPanneaux.init(this);
	autoCCGX.init(this);
	empaqueteur.init(this);
	leSeis.initProxySeis(this);
    fstream fichier(CHEMIN_FICHIER_CONF);
    string ligne;
    string parametre;
    etats.setMode(Etats::M_BATTERIES_HS + Etats::M_BRAS_HS + Etats::M_PANNEAUX_HS + Etats::M_SEIS_HS);
    while( !fichier.eof() )
    {
        getline(fichier, ligne);
        strStream.clear();
	strStream.str("");
        parametre=ligne.substr (0, ligne.find_first_of('=',0));
        if(parametre=="ADRS_IP_SEIS")
        {
            adrsIpSeis=ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
        }
        else if(parametre=="PORT_SEIS")
        {
            strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
            strStream>>portSeis;
        }
        else if(parametre=="ADRS_IP_AUTOMATE_BRAS")
        {
            adrsIpAutomateBras=ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
        }
        else if(parametre=="PORT_AUTOMATE_BRAS")
        {
            strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
            strStream>>portAutomateBras;
        }
        else if(parametre=="ADRS_IP_AUTOMATE_PANNEAUX")
        {
            adrsIpAutomatePanneaux=ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
        }
        else if(parametre=="PORT_AUTOMATE_PANNEAUX")
        {
            strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
            strStream>>portAutomatePanneaux;
        }
        else if(parametre=="ADRS_IP_CCGX")
        {
            adrsIpCCGX=ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
        }
        else if(parametre=="PORT_CCGX")
        {
           strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
            strStream>>portCCGX;
        }
        else if(parametre=="ADRS_IP_CAMERA")
        {
            adrsIpCamera=ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
        }
        else if(parametre=="PORT_CAMERA")
        {
            strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
            strStream>>portCamera;
        }
        else if(parametre=="PERIODE_SURVEILLANCE_CAMERA")
        {
            strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
            strStream>>periodeCamera;
        }
        else if(parametre=="CAMERA_PING_TIMEOUT")
        {
            strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
            strStream>>camTestTimeout;
        }
        else if(parametre=="NOMBRE_MAX_PING")
        {
            strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
            strStream>>camTestMax;
        }
        else if(parametre=="NOMBRE_OBSERVATEURS_MAX")
        {
            strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
            strStream>>nbMaxObs;
        }
        else if(parametre=="PERIODE_SURVEILLANCE_ARRU")
        {
            strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
            strStream>>periodeARRU;
        }
        else if(parametre=="NB_MESURES_PAR_PAQUET")
        {
            strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
            strStream>>NbMesuresParPaquet;
        }
        else if(parametre=="VERBOSE")
        {
            strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
            strStream>>val;
	    v.setLevelVerbose(val);
        }
        else if(parametre=="PERIODE_SURVEILLANCE_ENERGIE")
        {
            strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
            strStream>>periodeEnergie;
        }
        else if(parametre=="TENSION_MINIMUM")
        {
            strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
            strStream>>tensionMinimum;
        }
        else if(parametre=="CHARGE_MINIMUM")
        {
            strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
            strStream>>ChargeMinimum;
        }
        else if(parametre=="LOG_SUR_ECRAN")
        {
            strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
            strStream>>val;
	    v.setLogEcran(val);
        }
        else if(parametre=="NB_HISTORIQUES_LOG")
        {
            strStream<<ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
            strStream>>val;
	    v.setNbHistoriques(val);
        }
        else if(parametre=="CHEMIN_LOG")
        {
	    chaine=ligne.substr (1+ligne.find_first_of('=',0),ligne.length());
	    v.setCheminLog(chaine);
        }
    }
    fichier.close();
    v.init();
    MesuresAEnvoyer.init(this, NbMesuresParPaquet, &lesMessages);
}

Superviseur::~Superviseur ( ) { }

//=====================================================================================================

void	Superviseur::DemandeIHM(Etats::FLAGS_DEM_IHM val)
{
    // a la reception, la reponse de l'IHM est placée dans l'attribut 'reponseIHM'
    pthread_mutex_lock(&semDemIHM);
    stringstream strStream;
    strStream <<" demande 0x"<<hex<< setfill('0')<<setw(8)<<val;
    v.verbMsg(Verbose::INFO," Superviseur::DemandeIHM()",strStream.str() );
    etats.SetflagDemIHM(val);
    v.verbMsg(Verbose::INFO,"SUPERVISEUR::DEMANDE_IHM" , " demande IHM");
    pthread_cond_wait (&confirmationIMHRecue, &semDemIHM);
    etats.UnSetflagDemIHM(val);
    v.verbMsg(Verbose::INFO,"SUPERVISEUR::DEMANDE_IHM" ,"confirmation IHM reçue");
    pthread_mutex_unlock(&semDemIHM);
}

//=====================================================================================================
void	Superviseur::DemandeIHM_Energie(Etats::FLAGS_DEM_IHM val)
{
    // a la reception, la reponse de l'IHM est placée dans l'attribut 'reponseIHM'
    pthread_mutex_lock(&semDemIHM_Energie);
    etats.SetflagDemIHM(val);
    v.verbMsg(Verbose::INFO,"SUPERVISEUR::DEMANDE_IHM_ENERGIE" , " demande IHM Energie");
    pthread_cond_wait (&decisionIMH_Energie_Recue, &semDemIHM_Energie);
    etats.UnSetflagDemIHM(val);
    v.verbMsg(Verbose::INFO,"SUPERVISEUR::DEMANDE_IHM_ENERGIE" , " decision IHM Energie reçue");
    pthread_mutex_unlock(&semDemIHM_Energie);
}

//=====================================================================================================
void	Superviseur::DemandeIHM_Incident(Etats::FLAGS_DEM_IHM val)
{
    // a la reception, la reponse de l'IHM est placée dans l'attribut 'reponseIHM'
    pthread_mutex_lock(&semDemIHM_Incident);
    etats.SetflagDemIHM(val);
    v.verbMsg(Verbose::INFO,"SUPERVISEUR::DEMANDE_IHM_INCIDENT" ," demande IHM Incident");
    pthread_cond_wait (&decisionIMH_Incident_Recue, &semDemIHM_Incident);
    etats.UnSetflagDemIHM(val);
    v.verbMsg(Verbose::INFO,"SUPERVISEUR::DEMANDE_IHM_INCIDENT" ," decision IHM Incident reçue");
    pthread_mutex_unlock(&semDemIHM_Incident);
}

//=====================================================================================================
string  Superviseur::AntiRun_str(int anti)
{
	switch (anti)
	{
		case RIEN	: return "RIEN" ;
		case ARRU_MATERIEL : return "ARRU_MATERIEL" ;
		case ARRU_LOGICIEL : return "ARRU_LOGICIEL" ;
		case PROBLEME_ENERGIE : return "PROBLEME_ENERGIE" ;
		case INCIDENT_PERIPH : return "INCIDENT_PERIPH" ;
		case ARRET_DEMANDE : return "ARRET_DEMANDE" ;
	};
	return "?";
}

//=====================================================================================================
void	Superviseur::kernel()  // version 2015
{
    // ici :initialiser tous les poiteurs des classes qui ont besoin de la référence vers le supervisuer
    SupervisionEnCours=true;
    v.verbMsg(Verbose::INFO,"SUPERVISEUR::KERNEL", " Supervision prete à être lancée ");
    while(SupervisionEnCours)
    {
        // etatSysteme = ATTENTE
        //----------------------
        while(!testsOK)
        {
            //----------------------
            // etatSysteme = ATTENTE
            //----------------------
            etats.setEtatSysteme(Etats::ATTENTE);
            leSeis.seDeconnecter();
            automateBras.seDeconnecter();
            automatePanneaux.seDeconnecter();
            CCGX.seDeconnecter();
            v.verbMsg(Verbose::INFO,"SUPERVISEUR::KERNEL", " ++++ On entre dans le mode ATTENTE ++++");
            NatureAntiRun = Superviseur::RIEN;
            etats.setInterConferencier("000");
            etats.setOrgARRU(Etats::PAS_ARRU);
            etats.setSeqEnCours(Etats::RIEN);
            etats.setIncidentPanneaux(0);
            etats.setErreurSEIS(0);
            etats.SetDemIHM(Etats::PAS_DE_DEMANDE);
            etats.setCodeErreurBras(0);
            etats.setEtatBras(Etats::INCONNUE);
            etats.setTensionBatteries(0);
            etats.setChargeBatteries(0);
            etats.SetflagIncidentBatteries(0);
            etats.setCourantBatteries(0);
            etats.SetChargeConsommee(0);
            etats.setDureeRestanteBatteries(0);
            etats.setMode(Etats::M_BATTERIES_HS | Etats::M_PANNEAUX_HS | Etats::M_BRAS_HS | Etats::M_SEIS_HS | Etats::HTTP_CAM_HS );
            etats.setEtatPanneaux(Etats::PHASE_INIT + Etats::NON_INIT);
            etats.setEtatBras(Etats::INCONNUE);
            etats.setEtatSEIS(Etats::SUR_PLATEAU);
            etats.setEtatWTS(Etats::SUR_PLATEAU);
            etats.setEtatHP3(Etats::SUR_PLATEAU);
            etats.setCompteRenduSysteme(Etats::CCGX_NON_TESTE | Etats::PANNEAUX_NON_TESTES | Etats::SEIS_NON_TESTE | Etats::CAMERA_NON_TESTE | Etats::BRAS_NON_TESTE );
            v.verbMsg(Verbose::INFO,"SUPERVISEUR::KERNEL", "  etats ATTENTE initialisé");

            initOK=false;
            etats.SetflagIncidentBatteries(Etats::PAS_D_ALARME);

            v.verbMsg(Verbose::INFO,"SUPERVISEUR::KERNEL", "  setEtatSysteme=Etats::ATTENTE ");
            DemandeIHM(Etats::DEM_CONF_LANCER_TESTS); // demande bloquante

            //----------------------------------
            // etatSysteme = TESTS_MISE_EN_ROUTE
            //----------------------------------
            etats.setEtatSysteme(Etats::TESTS_MISE_EN_ROUTE);
            v.verbMsg(Verbose::INFO,"SUPERVISEUR::KERNEL", "  setEtatSysteme=Etats::MISE_EN_ROUTE ");
            testerPeriph();
            DemandeIHM(Etats::DEM_DECISION_SORTIE_TEST);// demande bloquante
            if(reponseIHM==DECISION_SORTIE_TESTS_EST_CONTINUER)
            {
                testsOK=true;
            }
            else
            {
                testsOK=false;
            }
        }
        while( testsOK && !initOK )
        {
            //----------------------------
            // etatSysteme = INITILISATION
            //----------------------------
            etats.setEtatSysteme(Etats::INITILISATION);
            v.verbMsg(Verbose::INFO,"SUPERVISEUR::KERNEL", "  ++++++++++++  On entre dans le mode INITILISATION");
            initOK=InitPeriph();
            if( (NatureAntiRun == ARRU_LOGICIEL) || (NatureAntiRun == Superviseur::ARRU_MATERIEL ) )
            {
		while(etats.getEtatSysteme()==Etats::ARRU_DEMANDEE)
                {
                   sleep(1);
                };
                NatureAntiRun = Superviseur::RIEN;
            }
            else if(NatureAntiRun ==  Superviseur::PROBLEME_ENERGIE )
	    {
                DemandeIHM_Energie(Etats::DEM_DECISION_BATTERIES_FAIBLES);// demande bloquante
                if(reponseIHM_Energie==DECISION_BATTERIE_FAIBLES_DONC_ARRETER)
                {
                    ArretPropreDesPeriph();
                };
            }
            else
            {
                DemandeIHM(Etats::DEM_DECISION_SORTIE_INITIALISATION);// demande bloquante
                if(reponseIHM==DECISION_SORTIE_INITIALISATION_EST_CONTINUER)
                {
                    v.verbMsg(Verbose::INFO,"SUPERVISEUR::KERNEL", " decision de sortie init est :"+reponseIHM);
                    testsOK=true;
                    initOK=true;
                }
                else
                {
                    v.verbMsg(Verbose::INFO,"SUPERVISEUR::KERNEL", " retour test car decision de sortie init est :"+reponseIHM);
                    ArretPropreDesPeriph();
                }
            }
        }
        while(SupervisionEnCours & testsOK & initOK)
        {
            // etatSysteme = COMMANDES
            //----------------------------
            etats.setEtatSysteme(Etats::COMMANDES);
            v.verbMsg(Verbose::INFO,"SUPERVISEUR::KERNEL", " setEtatSysteme=Etats::COMMANDES ");
            pthread_mutex_lock(&semAntiRun);
            pthread_cond_wait (&SignalAntiRun, &semAntiRun);
            pthread_mutex_unlock(&semAntiRun);
	    v.verbMsg(Verbose::INFO,"SUPERVISEUR::KERNEL", " reçu signal anti-run Superviseur::"+AntiRun_str(NatureAntiRun));
            switch(NatureAntiRun)
            {
            case Superviseur::ARRU_MATERIEL :
            case Superviseur::ARRU_LOGICIEL :
              while(etats.getEtatSysteme()==Etats::ARRU_DEMANDEE)
                {
                   sleep(1);
                };
                NatureAntiRun = Superviseur::RIEN;
                break;
            case Superviseur::PROBLEME_ENERGIE :
                DemandeIHM_Energie(Etats::DEM_DECISION_BATTERIES_FAIBLES);// demande bloquante
                if(reponseIHM_Energie==DECISION_BATTERIE_FAIBLES_DONC_ARRETER)
                {
                    ArretPropreDesPeriph();
                }
                break;
            case Superviseur::INCIDENT_PERIPH :
                DemandeIHM_Incident(Etats::DEM_DECISION_DEFAUT_DETECTE);// demande bloquante
                if(reponseIHM_Incident==DECISION_DEFAUT_DETECTES_DONC_ARRETER)
                {
                    ArretPropreDesPeriph();
                }
                break;
            case Superviseur::ARRET_DEMANDE :
                ArretPropreDesPeriph();
                break;

            default:
                break;
            }
        }
    }
}

//=====================================================================================================
// testerPeriph() est chargée de faire :
//		se connecter à la CCGX  et verifier si pas de problème d'énergie
// 		se connecter au SEIS et lui fait faire la série de tests
//		se connecter aux automates du Bras et des panneaux
//		puis se connecter à la caméra
// 		aucun mouvement n'est effectué dans ce test
//-----------------------------------------------------------------------------------------------------
bool	Superviseur::testerPeriph()
{
    stringstream strStream;
    unsigned char Mode=etats.getMode();
    unsigned int resutatstests;
    //------------------------------
    // test de la CCGX (energie)
    //------------------------------
    v.verbMsg(Verbose::DBG,"SUPERVISEUR::testerPeriph()"," Test de la CCGX (energie)" );
    etats.SetflagCompteRenduSysteme(Etats::CCGX_TESTS_EN_COURS);
    etats.UnsetflagCompteRenduSysteme(Etats::CCGX_NON_TESTE);
    if(Mode & Etats::M_BATTERIES_HS )
    {
        if( !CCGX.connecter(this, adrsIpCCGX, portCCGX))
        {
            v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::testerPeriph()", " echec connexion CCGX  (Batteries)");
        }
        else
        {
            etats.UnsetFlagMode(Etats::M_BATTERIES_HS);
        };
    };
    etats.UnsetflagCompteRenduSysteme(Etats::CCGX_TESTS_EN_COURS);
    // -----------------
    // tests du seis
    // -----------------
    v.verbMsg(Verbose::DBG,"SUPERVISEUR::testerPeriph()"," Tests du seis" );
    etats.SetflagCompteRenduSysteme(Etats::SEIS_TESTS_EN_COURS);
    etats.UnsetflagCompteRenduSysteme(Etats::SEIS_NON_TESTE);
    if(Mode & Etats::M_SEIS_HS)
    {
        if( !leSeis.SeConnecter(adrsIpSeis,portSeis) )
        {
            v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::testerPeriph()", "  echec connexion SEIS ");
        }
        else
        {
            if(!leSeis.ProxySEIS::tester(resutatstests))
            {
                v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::testerPeriph()", " echec durant les tests ");
            }
            else
            {
                etats.UnsetFlagMode(Etats::M_SEIS_HS);
                etats.setErreurSEIS(resutatstests);
            };
        };
    };
    etats.UnsetflagCompteRenduSysteme(Etats::SEIS_TESTS_EN_COURS);
    // ------------------------------
    // connexion à l'automate des panneaux
    // -----------------------------------
    v.verbMsg(Verbose::DBG,"SUPERVISEUR::testerPeriph()"," Test des panneaux" );
    etats.SetflagCompteRenduSysteme(Etats::PANNEAUX_TESTS_EN_COURS);
    etats.UnsetflagCompteRenduSysteme(Etats::PANNEAUX_NON_TESTES);
    if(Mode & Etats::M_PANNEAUX_HS)
    {
        if(!automatePanneaux.connecter(this, adrsIpAutomatePanneaux,portAutomatePanneaux))
        {
            v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::testerPeriph()" , " echec connexion aux PANNEAUX ");
        }
        else
        {
            etats.UnsetFlagMode(Etats::M_PANNEAUX_HS);
            automatePanneaux.ecrireCmdPanneaux(AutomatePanneaux::RIEN);
	    automatePanneaux.ecrireARRU(AutomatePanneaux::PAS_ARRU);
        };
    };
    etats.UnsetflagCompteRenduSysteme(Etats::PANNEAUX_TESTS_EN_COURS);
    // ------------------------------
    //connexion à l'automate du bras
    // ------------------------------
    v.verbMsg(Verbose::DBG,"SUPERVISEUR::testerPeriph()" ," Test du bras" );
    etats.SetflagCompteRenduSysteme(Etats::BRAS_TESTS_EN_COURS);
    etats.UnsetflagCompteRenduSysteme(Etats::BRAS_NON_TESTE);
    if(Mode & Etats::M_BRAS_HS)
    {
        if(!automateBras.connecter(this, adrsIpAutomateBras,portAutomateBras))
        {
            v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::testerPeriph()" , " echec connexion au BRAS");
        }
        else
        {
            etats.UnsetFlagMode(Etats::M_BRAS_HS);
	    automateBras.ecrireARRU(AutomateBras::PAS_ARRU);
	    automateBras.lancerSequence( AutomateBras::RIEN );
            automateBras.ecrireCmdActionBras(AutomateBras::AUCUNE);
        };
    };
    etats.UnsetflagCompteRenduSysteme(Etats::BRAS_TESTS_EN_COURS);

    // ------------------------------
    // Test de la connexion à la caméra - Sylvain VISTE
    // ------------------------------
    etats.SetflagCompteRenduSysteme(Etats::CAMERA_TESTS_EN_COURS);
    etats.UnsetflagCompteRenduSysteme(Etats::CAMERA_NON_TESTE);
    if(Mode & Etats::HTTP_CAM_HS)
    {
        v.verbMsg(Verbose::DBG,"SUPERVISEUR::testerPeriph()" ," Test de la caméra" );

        ICMP icmp; // Création de l'objet
        camTest = 0;
        int sockCamera = sockClientTCPtoutesIF();
        struct sockaddr_in addrCamera = {};
        addrCamera.sin_family = AF_INET;
        addrCamera.sin_addr.s_addr = inet_addr( adrsIpCamera.c_str() );
        addrCamera.sin_port = htons( portCamera );
	while(camTest<camTestMax){
	    camTest++;
       	    if( icmp.ping(adrsIpCamera, camTestTimeout) == ICMP::PING_OK )
            {
            	if( connect(sockCamera, (struct sockaddr*)&addrCamera, sizeof(addrCamera)) != -1 )
            	{
                	v.verbMsg(Verbose::INFO,"SUPERVISEUR::TESTER_PERIPH", " Caméra en ligne");
                	etats.UnsetFlagMode(Etats::HTTP_CAM_HS);
                	camTest = camTestMax;
            	}
	    }
        }
        close( sockCamera );
        v.verbMsg(Verbose::INFO,"SUPERVISEUR::TESTER_PERIPH", " Test de la caméra terminé");
    };

    etats.UnsetflagCompteRenduSysteme(Etats::CAMERA_TESTS_EN_COURS);
    pthread_cond_signal(&conditionCamera);
    v.verbMsg(Verbose::INFO,"SUPERVISEUR::TESTER_PERIPH", " fin de la phase de tests");

    return true;
}
//==================================================================================================================================



//**********************************************************************************************************************************
// InitPeriph()  est chargée d'initialiser les automates panneaux et bras, ce sont les parties mobiles
// =============
// 	si pas de PB, en fin d'initiatilation le bras doit être en position "panoramique"
//	si pas de PB, en fin d'initilisation les panneaux sont fermés (et les bras décolés)
//**********************************************************************************************************************************/

bool	Superviseur::InitPeriph()
//-------------------------------
{
    stringstream strStream;
    unsigned char Mode=etats.getMode();
    uint16_t EtatInitPeriph;
    uint16_t etatdubras;
    uint16_t etatpince;
    int erreur;
    int duree=0;
    //=============================================================================================
    // Si on a pu se connecter à l'automate des paneaux durant les test on lance l'init des paneaux
    //=============================================================================================
    if(!(Mode & Etats::M_PANNEAUX_HS) )
    {
        try
        {
            v.verbMsg(Verbose::INFO,"SUPERVISEUR::INIT_PERIPH"," lancement l'init panneaux");
            // on lance l'initialisation des panneaux
            automatePanneaux.ecrireInitPanneaux(AutomatePanneaux::TESTER);
            v.verbMsg(Verbose::DBG,"SUPERVISEUR::INIT_PERIPH"," Init panneaux demandée");
            //=================================================================================
            // on verifie en attente active que la commande soit prise en compte
            //=================================================================================
            do  // cas ou il y  aurait une rupture  flux
            {
		if(etats.getOrgARRU()!= Etats::PAS_ARRU)
		{
                    throw erreur=ARRU_DEMANDE;
		}
		if(! automatePanneaux.lireEtatInitPanneaux(EtatInitPeriph))   // rupture de flux
                {
                    throw erreur=RUPTURE_DE_FLUX_PANNEAUX;
                }
                else
                {
                    sleep (1); // pour que l'attente active ne consomme pas trop de CPU
		    if(duree++> DUREE_MAX_INIT_PANNEAUX ) throw erreur=TIMEOUT_PANNEAUX;
                };
		Mode=etats.getMode();
           }
            while( (EtatInitPeriph == AutomatePanneaux::NON_INIT)  && !(Mode & Etats::M_PANNEAUX_HS) );
            //=================================================================================
            // on efface la commande de l'entrée automate pour qu'elle ne soit pas retraitée
            //=================================================================================
            v.verbMsg(Verbose::DBG,"SUPERVISEUR::INIT_PERIPH"," init panneaux en cours");
            automatePanneaux.ecrireInitPanneaux(AutomatePanneaux::PAS_TEST);
            //=================================================================================
            // attente active que la commande soit terminée
            //=================================================================================
            while( (EtatInitPeriph == AutomatePanneaux::INIT_EN_COURS )  && !(Mode & Etats::M_PANNEAUX_HS) )
            {
 		if(etats.getOrgARRU()!= Etats::PAS_ARRU)
		{
                    throw erreur=ARRU_DEMANDE;
		}
		if(! automatePanneaux.lireEtatInitPanneaux(EtatInitPeriph))   // rupture de flux
                {
                    throw erreur=RUPTURE_DE_FLUX_PANNEAUX;
                }
                else
                {
                    sleep (1); // pour que l'attente active ne consomme pas trop de CPU
		    if(duree++> DUREE_MAX_INIT_PANNEAUX ) throw erreur=TIMEOUT_PANNEAUX;
                };
		Mode=etats.getMode();
            };
            //==========================================================================================
            // lecture d'état des panneaux pour positionner convenablement la variable d'etat (pour l'IHM)
            //==========================================================================================
            automatePanneaux.lireEtatPanneaux();
            v.verbMsg(Verbose::INFO,"SUPERVISEUR::INIT_PERIPH"," Init panneaux terminée");
        }
        catch( int & e )
        {
           if(e==RUPTURE_DE_FLUX_PANNEAUX)
            {
                v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::INIT_PERIPH", " panneaux flux coupé");
                etats.setEtatPanneaux(Etats::INCIDENT_REDHIBITOIRE_PANNEAUX);
                etats.SetflagCompteRenduSysteme(Etats::RUPTURE_LIAISON_PANNEAUX);
                EtatInitPeriph = Etats::INCIDENT_REDHIBITOIRE_PANNEAUX;
            };
            if(e==ARRU_DEMANDE)
            {
                v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::INIT_PERIPH", " arrêt de l'init car ARRU demandé");
                return false;
            };
            if(e==TIMEOUT_PANNEAUX)
            {
                v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::INIT_PERIPH", " Time out sur l'init des panneaux => ils sont mis HS");
		etats.SetFlagMode(Etats::M_PANNEAUX_HS);
            };
        };

        //=======================================================================================
        // Si on a pas pu se connecter à l'automate des panneaux on laisse une trace dans le log
        //=======================================================================================
    }
    else
    {
        v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::INIT_PERIPH", " Les panneaux étant HS donc non initialisés");
    };
    //=====================================================================================================
    // Si on a pu se connecter à l'automate du bras durant les test alors on lance l'init du bras
    //=====================================================================================================
    if(!(Mode & Etats::M_BRAS_HS) )
    {
        try
        {
	    // on efface la commande
            etats.setEtatBras(Etats::INIT_BRAS_EN_COURS);
            automateBras.lancerSequence( AutomateBras::RIEN );
            //===============================================================================================
            // il se peut qu'on soit ici après un arrêt d'urgence, et donc avec le bras et/ou la pince dans un
            // état indéterminé ; si la pince est fermée avec un appareil au bout, il faut demander au
            //  conferencier s'il est pret à récuprer l'appareil en bout de pince, d'où la demande de
            //  confirmation pour l'ouverture de la pince
            //===============================================================================================
	    v.verbMsg(Verbose::INFO,"SUPERVISEUR::INIT_PERIPH"," debut d'init Bras");
            if(!automateBras.lireEtatPince(etatpince)){
                	throw erreur=RUPTURE_DE_FLUX_BRAS;
	    }
            if(etatpince != Etats::PINCE_OUVERTE)
            {
		v.verbMsg(Verbose::DBG,"SUPERVISEUR::INIT_PERIPH","  zut! la pince n'est pas bien ouverte ");
                do
                {
		    if(etats.getOrgARRU()!= Etats::PAS_ARRU)
		    {
			throw erreur=ARRU_DEMANDE;
		    };
                    DemandeIHM(Etats::DEM_CONF_OUVERTURE_PINCE);// demande bloquante
                }
                while(reponseIHM!=CONF_OUVERTURE_PINCE);
 
               if(!automateBras.actionPince(AutomateBras::OUVRIR_PINCE))
		{
                	throw erreur=RUPTURE_DE_FLUX_BRAS;
		};
		v.verbMsg(Verbose::DBG,"SUPERVISEUR::INIT_PERIPH"," Ouverture de la pince");
                do
                {
                    sleep(1); // Repos cpu
 		    if(etats.getOrgARRU()!= Etats::PAS_ARRU)
		    {
			throw erreur=ARRU_DEMANDE;
		    };
		    if(!automateBras.lireEtatPince(etatpince)){
                	throw erreur=RUPTURE_DE_FLUX_BRAS;
		    };
                }
                while(etatpince == Etats::PINCE_FERMEE);
		// on efface la commande pour que l'automate ne cherchepas a l'executer de nouveau
                if(!automateBras.actionPince(AutomateBras::PAS_NOUVELLE_ACTION))
		{
                	throw erreur=RUPTURE_DE_FLUX_BRAS;
		};
                do
                {
                    sleep(1); // Repos cpu
		    if(etats.getOrgARRU()!= Etats::PAS_ARRU)
		    {
			throw erreur=ARRU_DEMANDE;
		    };
                    if(!automateBras.lireEtatPince(etatpince)){
                	throw erreur=RUPTURE_DE_FLUX_BRAS;
		    }
                }
                while(etatpince != Etats::PINCE_OUVERTE);
            }
            //=================================================================================
            // la pince étant ouverte on lance l'initialisation du bras
            //=================================================================================
	    v.verbMsg(Verbose::INFO,"SUPERVISEUR::INIT_PERIPH","  pince ouverte, on lancement l'init Bras");
            if(! automateBras.ecrireInitBras(AutomateBras::TESTER) )   // rupture de flux
            {
                throw erreur=RUPTURE_DE_FLUX_BRAS;
            }
            else
            {
                v.verbMsg(Verbose::DBG,"SUPERVISEUR::INIT_PERIPH"," Init bras en cours");
                //=================================================================================
                // on verifie en attente active que la commande soit prise en compte
                //=================================================================================
                do
                {
		    if(etats.getOrgARRU()!= Etats::PAS_ARRU)
		    {
 			throw erreur=ARRU_DEMANDE;
		    };
                    if(! automateBras.lireEtatInitBras(EtatInitPeriph))   // rupture de flux
                    {
                      throw erreur=RUPTURE_DE_FLUX_BRAS;
                    }
                    else
                    {
                        sleep (1); // pour que l'attente active ne consomme pas trop de CPU
                    };
                }
                while(EtatInitPeriph == AutomateBras::NON_INIT);   // on atdend que la commande d'init soit prise en compte
                //=================================================================================
                // on efface la commande de l'entrée automate pour qu'elle ne soit pas retraitée
                //=================================================================================
                v.verbMsg(Verbose::DBG,"SUPERVISEUR::INIT_PERIPH","  init Bras est en cours on efface la commande");
                automateBras.ecrireInitBras(AutomateBras::PAS_TEST); // on efface la commande de l'entrée automate
                //=================================================================================
                // on attend en attente active que la commande soit terminée
                //=================================================================================
                while( EtatInitPeriph == AutomateBras::INIT_EN_COURS )
                {
 		    if(etats.getOrgARRU()!= Etats::PAS_ARRU)
		    {
        		throw erreur=ARRU_DEMANDE;
		    };
                   if(! automateBras.lireEtatInitBras(EtatInitPeriph))   // rupture de flux
                    {
                        throw erreur=RUPTURE_DE_FLUX_BRAS;
                    }
                    else
                    {
                        sleep (1); // pour que l'attente active ne consomme pas trop de CPU
			strStream.clear();
			strStream.str("");
			strStream<< " initialisation du bras : etat du bras  ="<<etatdubras ;
			v.verbMsg(Verbose::INFO,"SUPERVISEUR::INIT_PERIPH",strStream.str() );
                    };
                };
                //==========================================================================================
                // lecture d'état du bras pour positioner convenablement la variable d'etat (pour l'IHM)
                //==========================================================================================
                do
                {
 		   if(etats.getOrgARRU()!= Etats::PAS_ARRU)
		   {
			throw erreur=ARRU_DEMANDE;
		   }
                   if( !automateBras.lireEtatBras(etatdubras)) throw erreur=RUPTURE_DE_FLUX_BRAS;
		   v.verbMsg(Verbose::DBG,"SUPERVISEUR::INIT_PERIPH","  Bras pas encore en panoramique");
		   sleep (1);
                }
                while ( etatdubras != Etats::POSITION_PANORAMIQUE );
                v.verbMsg(Verbose::INFO,"SUPERVISEUR::INIT_PERIPH"," Fin Init");
            };
        }
        catch( int & e )
        {
              if(e==RUPTURE_DE_FLUX_BRAS)
            {
                v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::INIT_PERIPH"," bras flux coupé");
                etats.setEtatBras(Etats::INCIDENT_REDHIBITOIRE_BRAS);
                etats.SetflagCompteRenduSysteme(Etats::RUPTURE_LIAISON_BRAS);
                EtatInitPeriph = Etats::INCIDENT_REDHIBITOIRE_BRAS;
            };
            if(e==ARRU_DEMANDE)
            {
                v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::INIT_PERIPH", " arrêt de l'init car ARRU demandé");
                return false;
            };
        };
        //=======================================================================================
        // Si on a pas pu se connecter a l'automate du bras on laisse une trace dans le log
        //=======================================================================================
    }
    else
    {
        v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::INIT_PERIPH"," le bras étatnt HS il n'a pas été initialisé");
    };
    return true;
}
//==================================================================================================================================



//**********************************************************************************************************************************
// ArretPropreDesPeriph()   lance la procédure de fin de conférence en prenant soin de tout mettre en position transport
// ======================
// 	Arrete le systeme et met tout en position transport . Mais pour que cet ordre soit exécuter proprement il faut vérifier que
//  tout les appraeil soient remontés sur le plateau. DAns le cas contraire cette fonction demande au conferencier le rangemetn de
//  chaque appareil. En tant normale l'IHM ne peux accepter le rangement que si tout a été remonté, cepedant d'arrêt sollicité suite
//  à un incident, cette procedure analyse ce qui n'est pas remonté, ou replié , ce qui peut etre  remonté ou replier et  propose au
// conférencier de remonter ou replier chaque élément encore accessible et dans l'ordre adéquate.
//**********************************************************************************************************************************/
void Superviseur::ArretPropreDesPeriph()
//--------------------------------------
{
    unsigned char Mode=etats.getMode();
    etats.setEtatSysteme(Etats::CLOTURE);
    //==============================================================================
    // si le bras est opérationnel
    //==============================================================================
    v.verbMsg(Verbose::INFO,"SUPERVISEUR::ARRET_PROPRE_PERIPH"," debut de la procedure d'arrêt propre");
    if(!(Mode & Etats::M_BRAS_HS) )
    {
        uint16_t etatbras;
        // et si le HP3 n'est pas sur le plateau
        if(etats.getSituationHP3()!=Etats::SUR_PLATEAU)
        {
            // alors on demande au conferencier de le remonter
            DemandeIHM(Etats::DEM_CONF_REMONTER_HP3_POUR_ARRET); // demande bloquante
            icn.remonterHP3();
        };
        // ou si le WTS (bouclier) n'est pas sur le plateau
        if(etats.getSituationWTS()!=Etats::SUR_PLATEAU)
        {
            // alors on demande au conferencier de le remonter
            DemandeIHM(Etats::DEM_CONF_REMONTER_WTS_POUR_ARRET); // demande bloquante
            icn.remonterWTS();
        }
        //==============================================================================
        // si le SEIS est opérationnel (sachant que ici le bras est, lui, oéprationel)
        //==============================================================================
        if(!(Mode & Etats::M_SEIS_HS) )
        {
            // et si le SEIS n'est pas sur le plateau
            if(etats.getSituationSEIS()!=Etats::SUR_PLATEAU)
            {
                // alors on demande au conferencier de le remonter
                DemandeIHM(Etats::DEM_CONF_REMONTER_SEIS_POUR_ARRET); // demande bloquante
                icn.remonterSEIS();
                leSeis.seDeconnecter();
            }
        }
        //==============================================================================
        // on peut alors demander au bras de se mettre en position transport
        //==============================================================================
        automateBras.lancerSequence( AutomateBras::ARRET_SYSTEM );
        do
        {
            // on attend que le bras ait fini de se mettre en position transport
            sleep(1);
            automateBras.lireEtatBras(etatbras);
        }
        while( etatbras !=  Etats::BRAS_POSITION_TRANSPORT );
        // on se deconecte de l'automate du BRAS
        automateBras.lancerSequence( AutomateBras::RIEN );
	sleep(1);
        automateBras.seDeconnecter();
    }
    //==============================================================================
    // si les Panneaux sont opérationnels
    //==============================================================================
    if(!(Mode & Etats::M_PANNEAUX_HS) )
    {
	uint16_t etatPanneaux;
        // et qu'ils ne sont pas fermés
        if(etats.getEtatPanneaux()!=Etats::PANNEAUX_FERMES)
        {
            // on demande au panneaux de se refermer
            DemandeIHM(Etats::DEM_CONF_FERMER_PANNEAUX_POUR_ARRET); // demande bloquante
            icn.fermerPanneaux();
        }
        automatePanneaux.ecrireCmdPanneaux(AutomatePanneaux::ARRET_SYSTEM);
        do
        {
            // on attend que le bras ait fini de se mettre en position transport
            sleep(1);
            automatePanneaux.lireEtatPanneaux(etatPanneaux);
        }
        while( etatPanneaux !=  Etats::PANNEAUX_FERMES );
        // on se deconecte de l'automate des panneaux
        automatePanneaux.ecrireCmdPanneaux(AutomatePanneaux::RIEN);
	sleep(1);
        automatePanneaux.deconnecter();
    }
    //on positonne les variables d'état qui précisent qu'on doit retourner dans l'état ATTENTE
    testsOK=false;
    initOK=false;
    v.verbMsg(Verbose::INFO,"SUPERVISEUR::ARRET_PROPRE_PERIPH"," fin de la procedure d'arrêt propre");
}
//=====================================================================================================



//********************************************************************************************************************************
// surveillanceARRU()      surveille l'avènement des arrêts d'urgence matériels
// ==================
//	La surveillance de l'arret d'urgence (ici materiel) ne se fait qu'à partir du moment ou les tests sont fait, c'est à dire
//  à partir du moment qu'il y a des mouvements de bras ou panneaux possibles.
//  	En cas de reception d'un état d'arrêt d'urgence de l'automate on détermine l'origine de l'ARRU pour savoir si c'est un
//  ètat concomitant à un ARRU logiciel ou si c'est effectivement un ARRU matériel.
//	Pour rappel, si l'ARRU est d'origne logiciel, seul le conférencier peut le dévalider, s'il est d'origine matériel seul une
//  action materielle sur le coup de poing peut dévérouiller l'ARRU
//
//	Cette fonction compose le corps du thread "SurveilleARRU"
//
//********************************************************************************************************************************/
void 	Superviseur::surveillanceARRU()
//--------------------------------------
{
    unsigned char mode,EtatARRU;
    uint16_t regARRUbras,regARRUpanneaux;

    //==================================================================================================
    // on attend que le thread du noyau d'application (kernel) soit pret
    //==================================================================================================
    while(!SupervisionEnCours)sleep(1); //attente que tout soit initialiser avant toute chose

    //===================================================================================================
    //  LANCEMENT DE LA SURVEILLANCE DES ARRETS D'URGENCE
    //===================================================================================================
    v.verbMsg(Verbose::INFO,"SUPERVISEUR::SURVEILLANCE_ARRU"," La surveillance des Arrets d'Urgences est active");
    while(1)
    {
	//
        sleep(periodeARRU);
        mode=etats.getMode();
	//===================================================================================================
	// On ne lit les registres D'ARRU des automates qu'une fois les tests passés pour être sûr que
	// l'information ait du sens
	//===================================================================================================
        if(testsOK)
        {
	    pthread_mutex_lock(&mutexGestionARRU);
            if( automateBras.estConnecte())
            {
                if(!automateBras.lireARRU(regARRUbras))
                {
                    etats.setMode(Etats::M_BRAS_HS);
		    v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::SURVEILLANCE_ARRU"," rupture de flux avec le bras => Etat bras HS");
                };
            };
            if(automatePanneaux.estConnecte() )
            {
                if(!automatePanneaux.lireARRU(regARRUpanneaux) )
                {
                    etats.setMode(Etats::M_PANNEAUX_HS);
		    v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::SURVEILLANCE_ARRU"," rupture de flux avec les Panneaux => Etat panneaux HS");
                };
            };

            EtatARRU=etats.getOrgARRU();
            //===================================================================================================
            // si un des automates a demandé un ARRU
            //===================================================================================================
            if((regARRUpanneaux==AutomatePanneaux::ARRU)||(regARRUbras==AutomateBras::ARRU))
            {
		// et que il n'y a aucune trace d'un ARRU passé alors c'est un ARRU matériel
        	if(EtatARRU==Etats::PAS_ARRU)
		{
                	    etats.setEtatSysteme(Etats::ARRU_DEMANDEE);
                            etats.setOrgARRU(Etats::ARRU_MATERIEL);
                            NatureAntiRun=Superviseur::ARRU_MATERIEL;
                            pthread_cond_signal (&SignalAntiRun);
		            v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::SURVEILLANCE_ARRU"," ARRU  materiel, Signal AntiRun envoyé");
		};
            }
            //===================================================================================================
            // si aucune trace d'état d'ARRU sur les automates
            //===================================================================================================
            else
            {
	           //et que le flag ARRU materiel est posé alors on l'annule
                   if(EtatARRU==Etats::ARRU_MATERIEL)
                   {
			testsOK=false;
			initOK=false;
			v.verbMsg(Verbose::INFO,"SUPERVISEUR::SURVEILLANCE_ARRU"," Arrêt de l'ARRU matériel");
                	etats.setOrgARRU(Etats::PAS_ARRU);
			etats.setEtatSysteme(Etats::ATTENTE);
 		   };
	    };
	    pthread_mutex_unlock(&mutexGestionARRU);
	}
	else if(etats.getEtatSysteme()==Etats::ATTENTE )
	{
		etats.setOrgARRU(Etats::PAS_ARRU);
		NatureAntiRun = Superviseur::RIEN;
	};
    }
}
//================================================================================================================



//*********************************************************************************************************************************
// surveillanceEnergie()		surveille l'énergie du système
// =====================
//	La surveillance de l'énergie s'effectue à partir du moment où on effectue les tests du système c'est à dire lorsque nous nous
//	connectons au matériel.
//	Cette méthode gère la lecture des variables de l'énergie (tension,courant,charge consommée,charge restante et la durée) ainsi
//	que les alarmes et les ruptures de flux avec la carte color control GX.
//	En cas d'alarme ( tension basse,relais ou charge basse) l'état du système est changé pour signaler au conférencier un incident
//	sur les batteries et une décision du conférencier est attendue pour continuer ( si on est sur secteur) ou alors fait un arrêt
//	propre du sustème.
//*********************************************************************************************************************************/
void 	Superviseur::surveillanceEnergie()
//--------------------------------------
{
	//==================================================================================================
	// on attend que le thread du noyau d'application (kernel) soit pret
	//==================================================================================================
	while(!SupervisionEnCours)sleep(1); //attente que tout soit initialiser avant toute chose
	//===================================================================================================
	//  Lancement de la surveillance énergie
	//===================================================================================================
	v.verbMsg(Verbose::INFO,"SUPERVISEUR::SURVEILLANCE_ENERGIE"," surveillance énergie active");

	unsigned short tension;
	short courant;
	short chargeconsommee;
	unsigned short chargerestante;
	unsigned short duree;
	int erreur;

	unsigned char oldsflags=Etats::PAS_D_ALARME;
	unsigned char newsflags,newflagdresse,newflagUndresse;
	bool DemanderConfirmationIHM;

	while(1)
	{
		while( ! CCGX.estConnecte() )
		{
			sleep(2);
		}
		try
		{
	//==============================================================================================================
	//  Lecture des variables de l'énergie et vérification si il y a des ruptures de flux et/ou incidents batteries
	//==============================================================================================================
			DemanderConfirmationIHM=false;
			//lecture qui met a jour les variables d'IHM en même temps
			if( ! CCGX.lireCourantBatterie( courant))
			{
				throw erreur=RUPTURE_DE_FLUX_CCGX;
			};
			if( ! CCGX.lireChargeConsommee( chargeconsommee))
			{
				throw erreur=RUPTURE_DE_FLUX_CCGX;
			};
			if( ! CCGX.lireChargeDureeRestante( duree))
			{
				throw erreur=RUPTURE_DE_FLUX_CCGX;
			};
 			if (! CCGX.lireTensionBatteries(tension) )
			{
				throw erreur=RUPTURE_DE_FLUX_CCGX;
			}
			else
			{
				if( (duree!=0) && (tension<tensionMinimum) )
				{
					etats.SetflagIncidentBatteries(Etats::TENSION_BASSE);
					v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::SURVEILLANCE_ENERGIE"," tension basse si tension<tensionMinimum et secteur non branché");
				}
				else
				{
					etats.UnsetflagIncidentBatteries(Etats::TENSION_BASSE);
				};
			};
			if (! CCGX.lireChargeBatterie( chargerestante) )
			{
				throw erreur=RUPTURE_DE_FLUX_CCGX;
			}
			else
			{
				if( (duree!=0) && (chargerestante<=ChargeMinimum) )
				{
					etats.SetflagIncidentBatteries(Etats::CHARGE_BASSE);
					v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::SURVEILLANCE_ENERGIE"," charge basse si chargerestante<=chargeMinimum et secteur non branché");
				}
				else
				{
					etats.UnsetflagIncidentBatteries(Etats::CHARGE_BASSE);
				};
			};
	//===================================================================================================
	//  si on a un incident de batteries
	//===================================================================================================
			newsflags=etats.getIncidentBatteries();
			newflagdresse=newsflags & (newsflags ^ oldsflags);
			if(newflagdresse)
			{
				v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::SURVEILLANCE_ENERGIE"," nouvel incident");
				DemanderConfirmationIHM=true;
			}
			newflagUndresse= oldsflags& (newsflags ^ oldsflags);
			if(newflagUndresse & Etats::ALARME_TENSION_BASSE)
			{
				v.verbMsg(Verbose::INFO,"SUPERVISEUR::SURVEILLANCE_ENERGIE"," fin alarme tension basse");
			};
			if(newflagUndresse & Etats::ALARME_TENSION_HAUTE)
			{
				v.verbMsg(Verbose::INFO,"SUPERVISEUR::SURVEILLANCE_ENERGIE"," fin alarme tension haute");
			};
			if(newflagUndresse & Etats::ALARME_TEMPERATURE)
			{
				v.verbMsg(Verbose::INFO,"SUPERVISEUR::SURVEILLANCE_ENERGIE"," fin alarme temperature");
			};
			if(newflagUndresse & Etats::ALARME_STATUS_RELAIS)
			{
				v.verbMsg(Verbose::INFO,"SUPERVISEUR::SURVEILLANCE_ENERGIE"," fin alarme relais");
			};
			if(newflagUndresse & Etats::TENSION_BASSE)
			{
				v.verbMsg(Verbose::INFO,"SUPERVISEUR::SURVEILLANCE_ENERGIE"," fin de la tension basse");
			};
			if(newflagUndresse & Etats::CHARGE_BASSE)
			{
				v.verbMsg(Verbose::INFO,"SUPERVISEUR::SURVEILLANCE_ENERGIE"," fin de charge basse");
			};
	//===================================================================================================
	//  si on a un incident sur les batteries on attend un confirmation IHM ( conférencier)
	//===================================================================================================
			if(DemanderConfirmationIHM)
			{
				DemandeIHM_Energie(Etats::DEM_DECISION_BATTERIES_FAIBLES);
				v.verbMsg(Verbose::INFO,"SUPERVISEUR::SURVEILLANCE_ENERGIE"," demande décision batteries faibles");
				if(reponseIHM_Energie == DECISION_BATTERIE_FAIBLES_MAIS_CONTINUER)
				{
					v.verbMsg(Verbose::INFO,"SUPERVISEUR::SURVEILLANCE_ENERGIE"," décision batteries faibles : continuer quand même");
					etats.UnsetFlagMode(Etats::M_ALARME_BATTERIES);
				}
				else
				{
					etats.SetFlagMode(Etats::M_ALARME_BATTERIES);
					v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::SURVEILLANCE_ENERGIE","  état d'alarme batteries");
					NatureAntiRun=Superviseur::ARRET_DEMANDE;
					pthread_cond_signal (&SignalAntiRun);
					v.verbMsg(Verbose::INFO,"SUPERVISEUR::SURVEILLANCE_ENERGIE"," on choisit d'arrêter le système");
				}
			}
			oldsflags=newsflags;
			sleep(periodeEnergie);
		}
		catch(int e)
		{
	//===================================================================================================
	//  Rupture de flux
	//===================================================================================================
			if(e == RUPTURE_DE_FLUX_CCGX)
			{
				v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::SURVEILLANCE_ENERGIE","  rupture de flux détectée");
				etats.SetFlagMode(Etats::M_BATTERIES_HS);
				NatureAntiRun=Superviseur::ARRET_DEMANDE;
				pthread_cond_signal (&SignalAntiRun);
				DemanderConfirmationIHM=true;
			}
		}
	}
}
//===============================================================================================================================



//********************************************************************************************************************************
// diffuser()   et chargé de diffuser les messages (trames d'état et mesures) auprès des clients
// ==========
//	Cette fonction tourne en parmanence elle compose le coeur du thread "diffusion". Son principe algorithmique est simple
// tant qu'il y a des messages dans la liste des message elle les extrait et les envoie à chacun des clients de la lite de
// diffusion. quand il n'y a plus de message a envoyer elle s'endort dans l'attente d'un signal d'une nouvelle mise en liste de'un
// message. ce mécanisme de signal est mis en oeuve à l'aide de la libération conditionelle de sémaphore posix
//
//	Pour rappel, un message est soit une trame d'état, soit un trame contenant un paquet de mesures
//
//********************************************************************************************************************************
void	Superviseur::diffuser()
//-----------------------------
{
    string message;
    int nbclients;
    int nbcar;
    char bufantibug[100];
    //==================================================================================================
    // on attend que le thread du noyau d'application (kernel) soit pret
    //==================================================================================================
    while(!SupervisionEnCours)sleep(1); //attente que tout soit initialiser avant toute chose

    //===================================================================================================
    //  LANCEMENT DU SERVICE DE DIFFSION DES MESSAGES
    //===================================================================================================
    v.verbMsg(Verbose::INFO,"SUPERVISEUR::DIFFUSER","le service de diffusion est actif");
    while(1)   // pas sur que ce test d'arret soit pertinent
    {
        // s'il n'y a pas de message dansla liste alors on lance le mécanisme d'endormissement
        if(!lesMessages.extraireMessage (message))
        {
            pthread_mutex_lock(&accesListeMessages);
            pthread_cond_wait (&nouveauMessage,&accesListeMessages);
            pthread_mutex_unlock(&accesListeMessages);
        }
        // sinon on traite la diffusion du message à tous les clients
        else
        {
            pthread_mutex_lock (&accesListeClients);
            nbclients=listeClients.size();
            for(int i=nbclients-1; i>=0; i--)
            {
                //verifion si le cliet est toujours là !!
                if((nbcar=read(listeClients[i]->canal,bufantibug,100))==0)
                {
                    // il n'est plus là mais verifions si c'était un observateur
                    if(! listeClients[i]->conferencier)
                    {
                        nbObs--; //on libere une place pour un nouvel observateur
                    }
                    close(listeClients[i]->canal);
                    listeClients.erase(listeClients.begin()+i);
                }
                // il est toujours la alors on lui envoie le message
                else
                {
                    write(listeClients[i]->canal,message.c_str(),message.length());
                }
            }
            pthread_mutex_unlock (&accesListeClients);
        }
    }
}
//===============================================================================================================================



//********************************************************************************************************************************
// inscriptionClients()   et chargé de l'inscription des clients à la liste de diffusion des messages, en fonction de sa qualité
// ====================
//    Lorsqu'un client veut recevoir l'interface IHM et la mise à jour en fonction des événemetns, les mesures de vibrations
//  sans oublier le flux de la caméra, il doit s'incrire auprès du service de diffusion. Lors de sa demande d'inscription le
//  client décline à son insu en fonction du serveur HTTP sollicité, sa qualité (CONFERENCIER ou OBSERVATEUR) car si le nombre de
//  conférenciers n'est pas limité (ce n'est pas utilie car la plateforme n'autorise que certains péripheriques préhalablement
//  répertoriés à se connecter sur le serveur 'conférencier') le nombre d'observateurs est en revanche limité vu qu'il n'y a
//  pas de limite d'acces au serveur 'observateur'. Lors d'une demande d'incription ,pour un observateur fonction d'inscription
//  commence par verifier si le nombre limite d'observateur est atteint auquel casla demande est refusée, sinon, comme pour le
//  cas d'un conferecnier, puis envoie une trame 'détat au cleint pour que son IHM se mette dans l'état actuel de la conference
//  c'est seulement après cette mise à jour que le client est mis dans la liste de diffusion
//
//	Cette fonction tourne en permanance et compose le coeur du thread "demonDiffusion". Elle n'est donc jamais appelée au
//  sens strict du terme mais elle recoit les demande d'incription dans un socket TCP attaché à l'interface 'loopback' ce qui
//  n'autorise l'accès qu'aux processus tournant sur la même plateforme que le démon, c'est le cas des CGI. les clients font
//  leur demande d'inscription par l'intermediaire du script CGI "cgiStream" qui va garder le lien permanent côté client à
//  l'aide de la technologie "event-stream" permise par l'HTML5 et coté supervisuer par le canal TCP qui va identifier le
//  client dans la liste de diffusion
//********************************************************************************************************************************
bool Superviseur::inscriptionClients()
//------------------------------------
{
    struct sockaddr_in exp;
    int can_service;
    int taille=sizeof(struct sockaddr);
    string NatureClient;
    int nbcar;
    char buffer[20];
    Client* prtClient;
    //===============================================================================================================
    // on attend que le thread du noyau d'application (kernel) soit pret
    //===============================================================================================================
    while(!SupervisionEnCours)sleep(1); //attente que tout soit initialiser avant toute chose

    //===============================================================================================================
    // création de la sockete TCP pour la reception des demandes d'inscription des clients. Cette socket est attachée
    // à l'interface "loopback" et ne peut donc être accessible que par des processus tournant sur la même plateforme
    // que le superviseur, ce qui est le cas des CGI
    //===============================================================================================================
    if ((canalDemonDiffusion =sockTCPspecLoopback(PORT_SERV_LB))==-1)
    {
        erreurCode=1;
        return false;
    }
    listen(canalDemonDiffusion ,2);

    //===================================================================================================
    //  LANCEMENT DU SERVICE D'ECOUTE DES DEMANDES D'INSCRIPTION
    //===================================================================================================
    v.verbMsg(Verbose::INFO,"SUPERVISEUR::INSCRIPTION_CLIENTS","le service d'inscription des Clients est actif");
    while(1)
    {
        if((can_service=accept(canalDemonDiffusion,(struct sockaddr*)&exp,(socklen_t *)&taille))>=0)
        {
            nbcar=read(can_service,(void *)buffer,(size_t)sizeof(buffer));
            v.verbMsg(Verbose::INFO,"SUPERVISEUR::INSCRIPTION_CLIENTS"," Nouveau client");
            if(nbcar>=0)buffer[nbcar]=0x00;
            NatureClient=(string)buffer;
            //=============================================================================================
            // LE CLIENT EST UN CONFERENCIER
            //=============================================================================================
            if(NatureClient==CONFERENCIER)
            {
                v.verbMsg(Verbose::DBG,"SUPERVISEUR::INSCRIPTION_CLIENTS"," Nature du client: CONFERENCIER");
                prtClient=new Client;
                prtClient->canal=can_service;
                prtClient->conferencier=true;
                //on rend la socket non bloquante en lecture pour tester son ouverture avant d'ecrire dedans
                fcntl(can_service, F_SETFL, fcntl(can_service,F_GETFL,0)| O_NONBLOCK);
                // récuperation de la tame d'état
                string aujusetat=etats.getTrame();
                // envoi la tame d'état au client
                write(can_service,aujusetat.c_str(),aujusetat.length());
                pthread_mutex_lock(&accesListeClients);
                // on  place le client dans la liste de diffusion
                listeClients.push_back(prtClient);
                v.verbMsg(Verbose::DBG,"SUPERVISEUR::INSCRIPTION_CLIENTS"," Ajout d'un CONFERENCIER dans la liste de diffusion");
                pthread_mutex_unlock(&accesListeClients);
            }
            //=============================================================================================
            // LE CLIENT EST UN OBERVATEUR
            //=============================================================================================
            else if(NatureClient==OBSERVATEUR)
            {
                // vérification que le nombre limite d'obseravteurs n'est pas atteint
                // ------------------------------------------------------------------
                if(nbObs<nbMaxObs)
                {
                    v.verbMsg(Verbose::INFO,"SUPERVISEUR::INSCRIPTION_CLIENTS","  Nature du client: OBSERVATEUR .. Accepté");
                    prtClient=new Client;
                    prtClient->canal=can_service;
                    prtClient->conferencier=false;
                    //on rend la socket non bloquante en lecture pour tester son ouverture avant d'ecrire dedans
                    fcntl(can_service, F_SETFL, fcntl(can_service,F_GETFL,0)| O_NONBLOCK);
                    // récuperation de la tame d'état
                    string aujusetat=etats.getTrame();
                    // envoi la tame d'état au client
                    write(can_service,aujusetat.c_str(),aujusetat.length());
                    pthread_mutex_lock(&accesListeClients);
                    // on incrémente le nombre d'obervateurs
                    nbObs++;
                    // on  place le client dans la liste de diffusion
                    listeClients.push_back(prtClient);
                    pthread_mutex_unlock(&accesListeClients);
                }
                else
                {
                    v.verbMsg(Verbose::INFO,"SUPERVISEUR::INSCRIPTION_CLIENTS"," Client OBSERVATEUR .. Refusé");
                    close(can_service);
                }
            }
            //=============================================================================================
            // ALORS là !!!  le client est un martien !!!!
            //=============================================================================================
            else
            {
                close(can_service);
                //ICI :gerrer le message d'erreurl profile inconu
            }
        };
    }
    return true;
}
//================================================================================================================================



//********************************************************************************************************************************
//  receptionCmds() est chargée de recevoir, filtrer et aiguiller les requêtes des clients (IHM)
// ================
//	Elle est lancée dans un thread 'demonCmds' à part et qui lui est dédié.
//	Les clients font leurs reqêtes au travers de sockets UDP ouverts par chaque script CGI de commande (cgiCommande.cgi).
//		Cette fonction étant chargée de recevoir les requêtes clientes, elle est en fait un serveur UDP dont la socket
//	 demon est attachée à l'interface loopback et ne peut donc être accessible que par des processus tournant sur la même
//	 plateforme que le superviseur, ce qui est le cas des CGI
//
//	Son action : trier les commandes, et en dehors des comandes dites "urgentes" qu'elle execute directement, elle aiguille
//	les autres requêtes vers les interpreteurs adéquats
//
//	le fichier "protocole.h" contient toutes les réquetes possibles
//
//********************************************************************************************************************************
bool Superviseur::receptionCmds()
//-------------------------------
{
	stringstream strStream;
	char cmd[15];
	int canalCMD;
	struct sockaddr_in expediteur;
	//===============================================================================================================
	// on attend que le thread du noyau d'application (kernel) soit pret
	//===============================================================================================================
	while(!SupervisionEnCours)sleep(1); //attente que tout soit initialiser avant toute chose

	//===============================================================================================================
	// création de la sockete UDP pour la reception des commandes, cette socket est attachée  à l'interface "loopback"
	// et ne peut donc être accessible que par des processus tournant sur la même plateforme que le superviseur, ce
	// qui est le cas des CGI
	//===============================================================================================================
	//ICI :signaler que on va creer les socket de commandes
	while((canalCMD = sockUDPspecLoopback(portCommandes)) < 0)
	{
		//"erreur de creation du socket\n" pas de 'cout'.. mais affecter un numero dans l'attribut 'erreurCode'
		return false;
	}

	//===================================================================================================
	//  LANCEMENT DU SERVICE D'ECOUTE DES COMMANDES
	//===================================================================================================
	v.verbMsg(Verbose::INFO,"SUPERVISEUR::RECEPTION_CMDS"," l'écoute commande active");
	while(1) /* Boucle infinie */
	{
		bzero(cmd,15); // efface toute trace d'ancienne commande
		v.verbMsg(Verbose::DBG,"SUPERVISEUR::RECEPTION_CMDS" ,"ancienne commande effacée");
		// reception de la nouvelle commande (lecture bloquante) :
		cmd[recvfrom(canalCMD, &cmd, sizeof(cmd), 0, (struct sockaddr*)&expediteur,(socklen_t*)sizeof(struct sockaddr) )]=0x00;
		v.verbMsg(Verbose::INFO,"SUPERVISEUR::RECEPTION_CMDS","nouvelle commande reçue");
		//===================================================================================
		//  LA REQUETE EST UNE COMMANDE URGNENTE :  TRAITEMENT DES COMMANDES URGENTES
		//===================================================================================
		if(cmd[0]=='u')  // c'est une commande urgente
		{
			cmdU=cmd;
			v.verbMsg(Verbose::DBG,"SUPERVISEUR::RECEPTION_CMDS" , " c'est une commande urgente");
			if(cmdU==U_ARRU)
			{
				pthread_mutex_lock(&mutexGestionARRU);
				etats.setOrgARRU(Etats::ARRU_LOGICIEL);
				etats.setEtatSysteme(Etats::ARRU_DEMANDEE);
				automateBras.ecrireARRU(AutomateBras::ARRU);
				automatePanneaux.ecrireARRU(AutomatePanneaux::ARRU);
				NatureAntiRun=Superviseur::ARRU_LOGICIEL;
				pthread_cond_signal (&SignalAntiRun);
//				etats.setEtatPanneaux(Etats::INCIDENT_REDHIBITOIRE_PANNEAUX);
//				etats.setEtatBras(Etats::INCIDENT_REDHIBITOIRE_BRAS);
				pthread_mutex_unlock(&mutexGestionARRU);
				v.verbMsg(Verbose::INFO,"SUPERVISEUR::RECEPTION_CMDS"," commande d'ARRU logiciel reçue");
			}
			else if(cmdU==U_FIN_ARRU)
			{
				v.verbMsg(Verbose::DBG,"SUPERVISEUR::RECEPTION_CMDS"," demmande de fin ARRU");
				pthread_mutex_lock(&mutexGestionARRU);
				if(etats.getOrgARRU()==Etats::ARRU_LOGICIEL)
				{
					automateBras.ecrireARRU(AutomateBras::PAS_ARRU);
					automatePanneaux.ecrireARRU(AutomatePanneaux::PAS_ARRU);
					etats.setOrgARRU(Etats::PAS_ARRU);
					v.verbMsg(Verbose::INFO,"SUPERVISEUR::RECEPTION_CMDS","fin ARRU");
					testsOK=false;
					initOK=false;
					etats.setEtatSysteme(Etats::ATTENTE);
				}
				pthread_mutex_unlock(&mutexGestionARRU);
			}
			else if(cmdU==U_PAS_DE_VERBOSE)
			{
				v.setLevelVerbose(Verbose::RIEN);
				v.verbMsg(Verbose::INFO,"SUPERVISEUR::RECEPTION_CMDS"," demande pour arrêt du verbose");
			}
			else if(cmdU==U_VERBOSE_ERREURS_SEULES)
			{
				v.setLevelVerbose(Verbose::ERREUR);
				v.verbMsg(Verbose::INFO,"SUPERVISEUR::RECEPTION_CMDS"," demande pour verbose que des erreurs");
			}
			else if(cmdU==U_VERBOSE_ERREURS_ET_ACTIONS)
			{
				v.setLevelVerbose(Verbose::INFO);
				v.verbMsg(Verbose::INFO,"SUPERVISEUR::RECEPTION_CMDS","demande pour verbose erreurs et informations");
			}
			else if(cmdU==U_VERBOSE_DETAILLEE)
			{
				v.setLevelVerbose(Verbose::DBG);
				v.verbMsg(Verbose::INFO,"SUPERVISEUR::RECEPTION_CMDS"," demande pour verbose détaillée");
			}
			else
			{
				//ICI en verbose : ecrire le contenu de "cmd" reçue en expliquant que cette commande urgente n'est pas reconnue
				v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::RECEPTION_CMDS"," cmdU '"+cmdU+"' non reconnue");
			}
		}
		//===================================================================================
		// LA REQUETE EST UNE INFORMATION INTERCONFERENCIERS (non traitée par le superviseur)
		//===================================================================================
		else if (cmd[0]=='0')   // c'est pas une commande mais un echange entre conferenciers
		{
			etats.setInterConferencier(cmd);
		}
		//===================================================================================
		//  LA REQUETE EST UNE CONFIRMATION CONFERENCIER  ENVOYEE PAR L'IHM
		//===================================================================================
		else if (cmd[0]=='f')   // c'est pas une commande mais une confirmation
		{
			//ICI en verbose : ecrire le contenu de "cmd" reçue
			strStream.clear();
			strStream.str("");
			strStream<<" confirmation '"<<cmd<<"' reçue";
			v.verbMsg(Verbose::DBG,"SUPERVISEUR::RECEPTION_CMDS",strStream.str());
			reponseIHM=cmd;
			pthread_cond_signal(&confirmationIMHRecue);// envoie au dernier demandeur
		}
		//===================================================================================
		//  LA REQUETE EST UNE DECISION CONFERENCIER  ENVOYEE PAR L'IHM
		//===================================================================================
		else if (cmd[0]=='d')   // c'est pas une commande mais une  decision IHM
		{
			//ICI en verbose : ecrire le contenu de "cmd" reçue
			strStream.clear();
			strStream.str("");
			strStream<<" décision  '"<<cmd<<"' reçue";
			v.verbMsg(Verbose::DBG,"SUPERVISEUR::RECEPTION_CMDS",strStream.str());
			if(cmd[1]=='b')
			{
				reponseIHM_Energie=cmd;
				pthread_cond_signal(&decisionIMH_Energie_Recue);// envoie au dernier demandeur
			}
			else if (cmd[1]=='d')
			{
				reponseIHM_Incident=cmd;
				pthread_cond_signal(&decisionIMH_Incident_Recue);// envoie au dernier demandeur
			}
			else
			{
				reponseIHM=cmd;
				pthread_cond_signal(&confirmationIMHRecue);// envoie au dernier demandeur
			};
		}
		//=========================================================================================================
		//  LA REQUETE EST UNE COMMANDE SPECIALE
		// ======================================
		//   Les commandes spéciales sont des commandes de maintenance et ne sont pas envoyées par le conférencier
		//   mais par l'intermediare d'un logiciel de maintenance nommé "simucgicmd". Ce logiciel simule un CGI de
		//   commandes (d'où son nom) car il rentre par la même porte UDP qui n'est accessible que par l'interface
		//   loopback, ce qui l'oblige à 'tourner' sur la plateforme du superviseur.
		//=========================================================================================================
		else if (cmd[0]=='s')   // c'est une commande spéciale
		{
			strStream.clear();
			strStream.str("");
			strStream<<" commande spéciale : '"<<cmd<<"' reçue";
			v.verbMsg(Verbose::DBG,"SUPERVISEUR::RECEPTION_CMDS",strStream.str());
			cmdS=cmd;
			pthread_mutex_lock(&mutexCmdSpeciale);
			pthread_mutex_unlock(&mutexCmdSpeciale);
			sleep(0.5); // pour attendrelaisser le temps a l'ICS de prednre le semaphore et de se mettre en 'wait'
			pthread_cond_signal (&conditionCmdS); // on signale qu'il y a une nouvelel commande spéciale à traiter
			// si c'est une commence normale
		}
		//===================================================================================================
		//  LA REQUETE EST UNE COMMANDE CONFERENCIER NORMALE  ENVOYEE PAR L'IHM
		//    les commadnes "normales" sont en fait les commandes qui vont lancer les séquences, ou demander
		//    l'arret de la conférence.
		//===================================================================================================
		else  if (cmd[0]=='c')   // c'est alors une commande normale
		{
			strStream.clear();
			strStream.str("");
			strStream<<" commande normale : '"<<cmd<<"' reçue";
			v.verbMsg(Verbose::DBG,"SUPERVISEUR::RECEPTION_CMDS",strStream.str());
			unsigned char cetat=etats.getEtatSysteme();
			if(cetat !=Etats::COMMANDES)
			{
				//ICI :signaler que l'etat du systeme ne permet pas la reception de commandes
				v.verbMsg(Verbose::DBG,"SUPERVISEUR::RECEPTION_CMDS"," état système != COMMANDES");
			}
			else   // alors on va analyser cette commande
			{
				cmdN=cmd;
				v.verbMsg(Verbose::DBG,"SUPERVISEUR::RECEPTION_CMDS"," on analyse la commande");
				pthread_cond_signal (&conditionCmdN); // on signale qu'il y a une nouvelle commande normale à traiter
				// si le thread d'interpratation des commandes normales n'est pas a l'écoute du signal (c'est à dire
				// qu'il exécute deja une commande normale alors cette commande sera perdue donc non exécutée
			}
			//===================================================================================================
			// alors là nous devrions jamais y arriver !! ce qui a été reçu est inatendu et hors protocole
			//===================================================================================================
		}
		else
		{
			strStream.clear();
			strStream.str("");
			strStream<<" commande '"<<cmd<<"' n'en n'est pas une";
			v.verbMsg(Verbose::DBG,"SUPERVISEUR::RECEPTION_CMDS",strStream.str());
		}
	}
	return true;
}
//================================================================================================================================



//********************************************************************************************************************************
//  interpreterComandesNormales()  vérifie la faisabilité d'une commande avant de la faire exécuter
//  ============================
//    Une commande noramle est une commande de séquence telle que "ouvrir les panneaux", deposer le SEIS", etc.. tout ce que le
//  conférencier a besoin de faire exécuter lors d'un fonctionement 'normale" d'une conférence y compris l'arrêt 'propre" de la
//  conférence". Cependant une commande ne peut être exécutée que si le contexte s'y prète (par exemple on ne dépose pas le SEIS
//  apres le bouclier), cette fonction a donc comme rôle de verifier le contexte de la commande avant de la faire exécuter.
//    A l'exception de la comande d'arrêt normale, les autres commandes ne sont pas traitées ici mais dans la classe ICN (pour
//  "Interpreteur de Commande Normale" situées dans le fichier commandes.cpp
//
//	Cette fonction tourne en permanance et compose le coeur du thread "interpreteurComandesNormales". Elle n'est donc jamais
//  appelée au sens strict du terme mais elle recoit un signal du démon de commandes l'avertissant qu'une commande normale a été
//  recue et placée dans la  varaible "cmdN".
//********************************************************************************************************************************
void Superviseur::interpreterComandesNormales()
//---------------------------------------------
{
	unsigned char Mode;
	unsigned char EtatPanneaux;
	unsigned char SituationSEIS;
	unsigned char SituationWTS;
	unsigned char SituationHP3;
	stringstream strStream;
	while(!SupervisionEnCours)sleep(1); //attente que tout soit initialiser avant toute chose
	v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" , " interpréteur de commandes normales actif");

	while(1)
	{
		//=========================================================================================================================
		// prise du sémaphore de reception de commande normale et libération conditionelle dans l'attente du signal "conditionCmdN"
		//=========================================================================================================================
		pthread_mutex_lock(&mutexCmdNormale);
		pthread_cond_wait (&conditionCmdN, &mutexCmdNormale);

		//=========================================================================================================================
		// le signal "conditionCmdS" a été reçu, récupération de la commande dans la variable "cmdS" pour l'analyser et l'exécuter
		//=========================================================================================================================
		v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" , " commande '"+cmdN+"' reçue");
		Mode=etats.getMode();
		//----------------------------------------------------------------------------------------------------
		//  	 COMMANDES NECESSITANT QUE AU MOINS UN DES PANNEAUX SOIT OPERATIONNEL
		//----------------------------------------------------------------------------------------------------
		if(cmdN[1]=='p')  // c'est une commande qui concerne les panneaux
		{
			if((Mode & Etats::M_PANNEAUX_HS)==0)  // les panneaux sont opérationnels
			{
				EtatPanneaux=etats.getEtatPanneaux();
				strStream.clear();
				strStream.str("");
				strStream<<" Etat panneaux : "<<(int)EtatPanneaux;
				v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" , strStream.str());
				//............................................................
				// 	CONTROLE VALIDITEE DE LA COMMANDE D'OUVERTURE PANNEAUX
				//............................................................
				if(cmdN[2]=='o')  // commande pour ouvrir les panneaux
				{
					if(EtatPanneaux==Etats::PANNEAUX_FERMES)
					{
						etats.setSeqEnCours(Etats::OUVERTURE_PANNEAUX);
						v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" , "On est entrain d'ouvrir les panneaux");
						if(!icn.ouvrirPanneaux())  // traite la rupture de flux
						{
							etats.setEtatPanneaux(Etats::INCIDENT_REDHIBITOIRE_PANNEAUX);
							etats.SetflagCompteRenduSysteme(Etats::RUPTURE_LIAISON_PANNEAUX);
							NatureAntiRun=Superviseur::INCIDENT_PERIPH;
							pthread_cond_signal (&SignalAntiRun);
							v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"rupture flux panneaux");
						};
					}
					else
					{
						v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" , "panneaux déjà ouverts");
					};
					//............................................................
					// 	CONTROLE VALIDITEE DE LA COMMANDE DE FERMETURE PANNEAUX
					//............................................................
				}
				else if(cmdN[2]=='f')  // commande pour fermer les panneaux
				{
					if(EtatPanneaux==Etats::PANNEAUX_OUVERTS)
					{
						etats.setSeqEnCours(Etats::FERMETURE_PANNEAUX);
						v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"on est entrain de fermer les panneaux");
						if(!icn.fermerPanneaux())  // traite la rupture de flux
						{
							etats.setEtatPanneaux(Etats::INCIDENT_REDHIBITOIRE_PANNEAUX);
							etats.SetflagCompteRenduSysteme(Etats::RUPTURE_LIAISON_PANNEAUX);
							NatureAntiRun=Superviseur::INCIDENT_PERIPH;
							pthread_cond_signal (&SignalAntiRun);
							v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"rupture flux panneaux");
						};
					}
					else
					{
						v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"panneaux déjà fermés");
					};

					//............................................................
					// 	COMMANDE INCOHERANTE
					//............................................................
				}
				else   // alors là!!!! nous ne devrions jamais y arriver
				{
					etats.SetflagCompteRenduSysteme(Etats::COMMANDE_INCONNUE);
					v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"commande '"+cmdN+"' inconnue");
				}
			}
			else
			{
				// ICI signaler que les PANNEAUX sont HS donc commande impossible à traiter
				v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ," panneaus HS => commande non traitée");
			}
			//----------------------------------------------------------------------------------------------------
			//  	 COMMANDES D'ARRET PROPRE DU SYSTEME
			//----------------------------------------------------------------------------------------------------
		}
		else if(cmdN[1]=='a')   // c'est la commande d'arret systeme
		{
			NatureAntiRun=Superviseur::ARRET_DEMANDE;
			pthread_cond_signal (&SignalAntiRun);
			v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"commande arrêt système");
			//----------------------------------------------------------------------------------------------------
			//  	 COMMANDES NECESSITANT QUE LE BRAS SOIT OPERATIONNEL
			//----------------------------------------------------------------------------------------------------
		}
		else   // c'est une commande qui necessite que le bras fonctionne
		{
			if((Mode & Etats::M_BRAS_HS)==0)  // le bras est opérationnel
			{
				v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"bras fonctionnel");
				if(cmdN[1]=='s')  // c'est une comande qui necessite que le SEIS fonctione
				{
					v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"SEIS fonctionnel");
					//--------------------------------------------------------------------------------------------
					// 		CONTROLE VALIDITEE DE LA COMMANDES NECESSITANT QUE LE SEIS SOIT OPERATIONNEL
					//--------------------------------------------------------------------------------------------
					if((Mode & Etats::M_SEIS_HS)==0)  // le SEIS est opérationnel
					{
						SituationSEIS=etats.getSituationSEIS();
						v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"commande nécessitant SEIS fonctionnel");
						//............................................................
						// 	CONTROLE VALIDITEE DE LA COMMANDE DE DEPOSE DU SEIS
						//............................................................
						if(cmdN[2]=='d')  // commande de dépose SEIS
						{
							v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"commande dépose SEIS");
							if(SituationSEIS==Etats::SUR_PLATEAU)
							{
								SituationWTS=etats.getSituationWTS();
								v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ," récup état WTS quand SEIS sur plateau");
								if(SituationWTS==Etats::SUR_PLATEAU)
								{
									etats.setSeqEnCours(Etats::DEPOSE_SEIS);
									v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"WTS sur plateau on dépose SEIS");
									if(!icn.deposerSEIS())
									{
										NatureAntiRun=Superviseur::INCIDENT_PERIPH;
										pthread_cond_signal (&SignalAntiRun);
										v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"incident periphérique");
									};
								}
								else
								{
									// ICI signaler que le WTS étant dejà au sol on ne peut déposer le SEIS
									v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"WTS au sol pas de dépose SEIS");
								}
							}
							else
							{
								// ICI signaler que le SEIs est dejà au sol
								v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"SEIS déjà au sol");
							}
							//............................................................
							// 	CONTROLE VALIDITEE DE LA COMMANDE DE REPRISE DU SEIS
							//............................................................
						}
						else if(cmdN[2]=='r')  // commande de reprise SEIS
						{
							v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"commande reprise SEIS");
							if( SituationSEIS & Etats::AU_SOL )
							{
								SituationWTS=etats.getSituationWTS();
								v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"récupération état WTS");
								if(SituationWTS==Etats::SUR_PLATEAU)
								{
									etats.setSeqEnCours(Etats::REPRISE_SEIS);
									v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"reprise SEIS en cours");
									if( !icn.remonterSEIS() )
									{
										NatureAntiRun=Superviseur::INCIDENT_PERIPH;
										pthread_cond_signal (&SignalAntiRun);
										v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"incident periphérique");
									};
								}
								else
								{
									// ICI signaler que le WTS étant encore au sol on ne peut remonter le SEIS
									v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"WTS au sol, pas de reprise SEIS");
								}
							}
							else
							{
								// ICI signaler que le SEIs est dejà sur le plateau
								v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"SEIS déjà sur le plateau");
							}
							//............................................................
							// 	COMMANDE INCOHERANTE
							//............................................................
						}
						else   // alors là!!!! nous ne devrions jamais y arriver
						{
							v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"commande inconnue");
						}
					}
					else
					{
						v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ," Seis HS, commande impossible à effectuer");
					}
					//--------------------------------------------------------------------------------------------
					// 		COMMANDES CONCERNANT LE BOUCLIER (WTS)
					//--------------------------------------------------------------------------------------------
				}
				else if(cmdN==C_DEPOSE_WTS)
				{
					SituationWTS=etats.getSituationWTS();
					v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"commande dépose WTS");
					if(SituationWTS==Etats::SUR_PLATEAU)
					{
						etats.setSeqEnCours(Etats::DEPOSE_WTS);
						v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"dépose WTS en cours");
						if( !icn.deposerWTS() )
						{
							// on fait remonter la rupture de flux
							v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"rupture flux bras");
							etats.SetflagCompteRenduSysteme(Etats::RUPTURE_LIAISON_BRAS);
							NatureAntiRun=Superviseur::INCIDENT_PERIPH;
							pthread_cond_signal (&SignalAntiRun);
							v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"rupture flux bras");
						};
					}
					else
					{
						// ICI signaler que le WTS est dejà au sol
						v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"WTS déjà au sol");
					}
				}
				else if(cmdN==C_REPRISE_WTS)
				{
					SituationWTS=etats.getSituationWTS();
					v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"commande reprise WTS");
					if(SituationWTS==Etats::AU_SOL)
					{
						etats.setSeqEnCours(Etats::REPRISE_WTS);
						v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"reprise WTS");
						if( !icn.remonterWTS() )
						{
							// on fait remonter la rupture de flux
							v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"rupture flux bras");
							etats.SetflagCompteRenduSysteme(Etats::RUPTURE_LIAISON_BRAS);
							NatureAntiRun=Superviseur::INCIDENT_PERIPH;
							pthread_cond_signal (&SignalAntiRun);
						};
					}
					else
					{
						// ICI signaler que le WTS est dèjà sur leplateau
						v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"WTS déjà sur le plateau");
					}
					//--------------------------------------------------------------------------------------------
					// 		COMMANDES CONCERNANT LE HP3
					//--------------------------------------------------------------------------------------------
				}
				else if(cmdN==C_DEPOSE_HP3)
				{
					SituationHP3=etats.getSituationHP3();
					v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"commande dépose HP3");
					if(SituationHP3==Etats::SUR_PLATEAU)
					{
						etats.setSeqEnCours(Etats::DEPOSE_HP3);
						v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"dépose HP3 en cours");
						if( !icn.deposerHP3() )
						{
							// on fait remonter la rupture de flux
							v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"rupture flux bras");
							etats.SetflagCompteRenduSysteme(Etats::RUPTURE_LIAISON_BRAS);
							NatureAntiRun=Superviseur::INCIDENT_PERIPH;
							pthread_cond_signal (&SignalAntiRun);
						};
						v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"dépose HP3 terminée");
					}
					else
					{
						// ICI signaler que le HP3 est dejà au sol
						v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"HP3 déjà au sol");
					}
				}
				else if(cmdN==C_REPRISE_HP3)
				{
					SituationHP3=etats.getSituationHP3();
					v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"commande reprise HP3");
					if(SituationHP3==Etats::AU_SOL)
					{
						etats.setSeqEnCours(Etats::REPRISE_HP3);
						v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"reprise HP3 en cours");
						if( !icn.remonterHP3() )
						{
							// on fait remonter la rupture de flux
							v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"rupture flux bras");
							etats.SetflagCompteRenduSysteme(Etats::RUPTURE_LIAISON_BRAS);
							NatureAntiRun=Superviseur::INCIDENT_PERIPH;
							pthread_cond_signal (&SignalAntiRun);
						};
					}
					else
					{
						// ICI signaler que le HP3 est dèjà sur leplateau
						v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"HP3 déjà sur le plateau");
					}
					//............................................................
					// 	COMMANDE INCOHERANTE
					//............................................................
				}
				else   // alors là!!!! nous ne devrions jamais y arriver
				{
						v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"commande inconnue");
				}
				//--------------------------------------------------------------------------------------------
				// 		COMMANDES IMPOSSIBLE CAR LE BRAS EST HS
				//--------------------------------------------------------------------------------------------
			}
			else
			{
				v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"commande impossible à traiter bras HS");
			}
		}
		etats.setSeqEnCours(Etats::RIEN);
		pthread_mutex_unlock(&mutexCmdNormale);
		v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_NORMALES" ,"aucune séquence en cours");

	}
}
//================================================================================================================================



//********************************************************************************************************************************
//  interpreterComandesSpeciales()  fait executer des commandes spécifiques à la maintenances
//  =============================
//    Une commande spéciale est une commande qui est hors du domaine d'exécution d'une conférence sans problèeme
//	ce sont des comandes sui permettent d'ouvrir ou fermer la pince, forcer le replie des pieds du seis, le lancemetn et arrêt
//   des mesures de vibrations
//    Les commandes ne sont pas traitées ici mais dans la classe ICS (pour"Interpreteur de Commande Spéciale") situées dans le
//   fichier commandes.cpp
//
//	Cette fonction tourne en permanance et compose le coeur du thread "interpreteurComandesSpéciales". Elle n'est donc jamais
//  appelée au sens strict du terme mais elle recoit un signal du démon de commandes l'avertissant qu'une commande speciale a été
//  recue et placée dans la  varaible "cmdS".
//********************************************************************************************************************************
void Superviseur::interpreterComandesSpeciales()
//---------------------------------------------
{
	unsigned char EtatPince;

	while(!SupervisionEnCours)sleep(1); //attente que tout soit initialiser avant toute chose
	v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_SPECIALES"," interpréteur commande spéciale actif");

	while(1)
	{
		//=========================================================================================================================
		// prise du sémaphore de reception de commande spéciale et libération conditionelle dans l'attente du signal "conditionCmdS"
		//=========================================================================================================================
		pthread_mutex_lock(&mutexCmdSpeciale);
		pthread_cond_wait (&conditionCmdS, &mutexCmdSpeciale);

		//=========================================================================================================================
		// le signal "conditionCmdS" a été reçu , récupreration la commande dans la variable "cmdS" pour l'analyser et l'exécuter
		//=========================================================================================================================
		if(cmdS==S_OUVERTURE_PINCE)  // commande pour essayer de forcer l'ouverture de la pince
			//--------------------------------------------------------------------------------------
		{
			EtatPince=etats.getEtatPince();
			if(EtatPince==Etats::PINCE_FERMEE)
			{
				ics.ouvrirPince();
				v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_SPECIALES"," commande ouverture pince");
			}
			else
			{
				// ICI signaler que la pince n'étant pas fermée nous ne pouvons l'ouvrir
				v.verbMsg(Verbose::DBG,"SUPERVISEUR::INTERPRETER_CMD_SPECIALES"," pince déjà ouverte");
			}
		}
		else if(cmdS==S_FERMETURE_PINCE)   // commande pour essayer de forcer la fermeture de la pince
			//--------------------------------------------------------------------------------------------
		{
			EtatPince=etats.getEtatPince();
			v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_SPECIALES"," commande fermeture pince");
			if(EtatPince==Etats::PINCE_OUVERTE)
			{
				ics.fermerPince();
				v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_SPECIALES"," fermeture pince");
			}
			else
			{
				// ICI signaler que la pince n'étant pas ouverte nous ne pouvons la fermer
				v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_SPECIALES"," pince déjà fermée");
			}
		}
		else if(cmdS==S_SEIS_LANCER_MESURES)  // commande pour que le SEIS lance les mesures
			//----------------------------------------------------------------------------------
		{
			ics.SeisLancerMesures();
			v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_SPECIALES"," lancement mesures SEIS");
		}
		else if(cmdS==S_SEIS_ARRET_MESURES)  // commande pour que le SEIS arrête  les mesures
			//-----------------------------------------------------------------------------------
		{
			ics.SeisArretMesures();
			v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_SPECIALES"," arrêt mesures SEIS");
		}
		else if(cmdS==S_SEIS_REPLIER_PIEDS)  // commande pour que le SEIS replie ses pieds
			//----------------------------------------------------------------------------------
		{
			ics.SeisReplierPieds();
			v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_SPECIALES"," replier pieds SEIS");
		}
		else if(cmdS==S_SEIS_LANCER_NIVEL)  // commande pour que le SEIS replie ses pieds
			//----------------------------------------------------------------------------------
		{
			ics.SeisNiveller();
			v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_SPECIALES"," replier pieds SEIS");
		}


		else if(cmdS==S_METTRE_PANNEAUX_HS)   // commande pour essayer de forcer la mise des panneaux HS
			//--------------------------------------------------------------------------------------
		{
			ics.MettrePanneauxHS();
			v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_SPECIALES"," mise HS des panneaux par le conferencier");
		}
		else // alors là nous ne devriosn jamais y arriver
		{
			// ICI signaler que la commande est incomprehensible
			v.verbMsg(Verbose::INFO,"SUPERVISEUR::INTERPRETER_CMD_SPECIALES"," commande '"+cmdS+"'incompréhensible");
		}
		pthread_mutex_unlock(&mutexCmdSpeciale);

	}
}
//================================================================================================================================



//********************************************************************************************************************************
//  ConstruireMessageDEtat()   fonction intermediaire interfaçant la méthode d'envoie rythmé "diffuserEtat()" de la classe Etat
//  ========================
//  Cette interfaçe est necessaire pour que le trhead "construteurDEtat"emmte en oeuvre cette difusion rythmée sans avoir besoin
//  de connaitre la classe Etat et son instanciation
// 	"superviseur.ConstruireMessageDEtat();" semble plus élégant que "superviseur.etats.diffuserEtat();"
//********************************************************************************************************************************
void Superviseur::ConstruireMessageDEtat()
//---------------------------------------------
{
    etats.diffuserEtat();
}
//================================================================================================================================



//********************************************************************************************************************************
// surveillanceCamera()
// ====================
//
//********************************************************************************************************************************
void Superviseur::surveillanceCamera()
{
    unsigned char Mode;
    ICMP icmp;
    int reponsePing;
    int camTest;
    struct sockaddr_in addrCamera = {};
    addrCamera.sin_family = AF_INET;
    addrCamera.sin_addr.s_addr = inet_addr( adrsIpCamera.c_str() );
    addrCamera.sin_port = htons( portCamera );
   //===============================================================================================================
    // on attend que le thread du noyau d'application (kernel) soit pret
    //===============================================================================================================
    while(!SupervisionEnCours)sleep(1); //attente que tout soit initialiser avant toute chose

   //===============================================================================================================
    // le thread du noyau de l'application est actif
    //===============================================================================================================
    while(1)
    {
	if(etats.getEtatSysteme()==Etats::ATTENTE )
	{
 	    pthread_mutex_lock(&mutexCamera);
    	    pthread_cond_wait(&conditionCamera, &mutexCamera);
 	    pthread_mutex_unlock(&mutexCamera);//libération apres dévérouillage
	};
	sleep(periodeCamera);
	Mode=etats.getMode();
	camTest = camTestMax;
 	while(camTest>0)
	{
	    reponsePing =  icmp.ping(adrsIpCamera, camTestTimeout);
	    if ( reponsePing == ICMP::PING_TIMEOUT ) camTest --;
	    else camTest=0;
	};
   	if( reponsePing != ICMP::PING_OK )
	{
       	    etats.SetFlagMode( Etats::HTTP_CAM_HS );
                v.verbMsg(Verbose::ERREUR,"SUPERVISEUR::surveillanceCamera()"," La caméra ne répond pas au  au ping");
 	}
	else if ( Mode & Etats::HTTP_CAM_HS )
	{
	    int sockCamera = sockClientTCPtoutesIF();
            if ( connect(sockCamera, (struct sockaddr*)&addrCamera, sizeof(addrCamera)) != -1 )
            {
                // La caméra est de nouveau en ligne
                v.verbMsg(Verbose::DBG,"SUPERVISEUR::surveillanceCamera()"," Caméra de nouveau en ligne");
                etats.UnsetFlagMode( Etats::HTTP_CAM_HS );
            }
	    else
	    {
                v.verbMsg(Verbose::ERREUR ,"SUPERVISEUR::surveillanceCamera()","Serveur caméra non demarré");
  	    };
            close( sockCamera ); 
	};
    };
}
//================================================================================================================================

void Superviseur::fermetureClients()
{
    close(canalDemonDiffusion);
}

//---------FIN FICHIER--------------------------------------------------------------------

