/* --------------------------------------------------------------------------
                            Projet Insight

    Nom du fichier : etat.cpp
    Langage : CPP
    Description :
        Fabrication de la trame pour l'envoi des états aux clients IHM
    Organisme : Lycée International Victor Hugo Colomiers, BTS IRIS
    Chef de projet : J.Voyé, JP.Dumas
    Auteur 2013 : LAMAMY Bastien
    Version : v1.0 - 04/2013
    Auteur 2014 :
    Version : v1.1 - 02/2015
-----------------------------------------------------------------------------
 */

/*
--------------------------------------------------------------------------------
      Définition de la trame de transfert des états vers l'IHM
--------------------------------------------------------------------------------
La trame est une chaine de 103 caractères composée de 21 champs séparés par ';'
--------------------------------------------------------------------------------
AA;YY;EE;MM;CCCCCCCC;IIIIIIII;SS;ssssssss;WW;HH;PP;pppppppp;NN;BB;bbbbbbbb;
                                                   TTTT;RRRR;BBBB;OOOO;UUUU;ee;
--------------------------------------------------------------------------------
01 AA       : état de l'arrêt d'urgence
02 YY       : état globale du système
03 EE       : séquence en cours
04 MM       : mode dégradé
05 CCCCCCCC : code de compte rendu système
06 IIIIIIII : code de demande d'intervention IHM
07 SS       : situation du SEIS
08 ssssssss : compte rendu erreur SEIS
09 WW       : situation du WTS
10 HH       : situation du HP3
11 PP       : états des panneaux
12 pppppppp : code incident PANNEAUX
13 NN       : états de la pince
14 BB       : états du bras
15 bbbbbbbb : code d’information du système bras/pince
16 TTTT     : tension batteries
17 RRRR     : charges des batteries
18 BBBB     : courant dans les batteries
19 OOOO     : charge consommée
20 UUUU     : durée restante dans les batteries
21 ee       : code incident énergie
-----------------------------------------------------------------------------
 */

#include "../defs/Etats.h"
#include "../defs/Superviseur.h"

//---------------------------------------------------------------------------------------------------------
Etats::Etats()
{
	ptrListeMessages=NULL;
	pthread_mutex_init(&semEtat, NULL);
	NouveauMessage=false;
}

//---------------------------------------------------------------------------------------------------------
void Etats::initEtats(ListeMessages* ptrlistemessages)
{
	ptrListeMessages=ptrlistemessages;
}

void Etats::init(Superviseur* ptrsup)
{
	ptrSuperviseur=ptrsup;
}

void Etats::diffuserEtat()
{

    while(1)
    {
        pthread_mutex_lock(&semNouveauMessage);
        pthread_cond_wait (&NouveauMessageRecue, &semNouveauMessage);
        pthread_mutex_unlock(&semNouveauMessage);
        while(NouveauMessage)
        {
            pthread_mutex_lock(&semEtat);
            contruireTrame();
            NouveauMessage=false;
            pthread_mutex_unlock(&semEtat);
            usleep(DUREE_ENTRE_DEUX_ENVOIES);
        }
    }
}


/*
--------------------------------------------------------------------------------
                    Construction de la trame
La trame est une chaine de 103 caractères + 1 pour le 'e' initial.
AA;YY;EE;MM;CCCCCCCC;IIIIIIII;SS;ssssssss;WW;HH;PP;pppppppp;NN;BB;bbbbbbbb;
                                                   TTTT;RRRR;BBBB;OOOO;UUUU;ee;
--------------------------------------------------------------------------------
 */
void Etats::contruireTrame()
{
	ostringstream str_trame;
	str_trame.str("");
	str_trame << 'e' << hex << setfill('0')
			  << setw(2) << (int) orgARRU << ";"      // 01 AA : l'ARRU
			  << setw(2) << (int) etatSysteme << ";"  // 02 YY : état globale du systeme
			  << setw(2) << (int) SeqEnCours << ";"   // 03 EE : séquence En Cours
			  << setw(2) << (int) Mode << ";"         // 04 MM : Mode dégradé
			  << setw(8) << CompteRenduSysteme << ";" // 05 CCCCCCCC : code de compte rendu système
			  << setw(8) << demIHM << ";"             // 06 IIIIIIII : code de demande d'intervention IHM
			  << setw(2) << (int) Seis << ";"         // 07 SS : Situation du SEIS
			  << setw(8) << ErrSEIS << ";"            // 08 ssssssss  : compte rendu erreur SEIS
			  << setw(2) << (int) WTS << ";"          // 09 WW : Situation du WTS
			  << setw(2) << (int) HP3 << ";"          // 10 HH : Situation du HP3
			  << setw(2) << (int) EtatPanneauxIHM << ";" // 11 PP : États des panneaux
			  << setw(8) << IncidentPanneaux << ";"   // 12 pppppppp : Code incident PANNEAUX
			  << setw(2) << (int) EtatPince << ";"    // 13 NN : États de la pince
			  << setw(2) << (int) EtatBras << ";"     // 14 BB : États du bras
			  << setw(8) << CodeErrBrasPince << ";"   // 15 bbbbbbbb : code d’information du système bras/pince
			  << setw(4) << TensionBatteries << ";"   // 16 TTTT : TENSION_BATTERIES
			  << setw(4) << ChargeBatteries << ";"    // 17 RRRR : CHARGES_BATTERIES
			  << setw(4) << CourantBatteries << ";"   // 18 BBBB : COURANT_BATTERIES
			  << setw(4) << ChargeConsommee << ";"    // 19 OOOO : CHARGE_CONSOMMEE
			  << setw(4) << DureeRestante << ";"      // 20 UUUU : DUREE_RESTANTE
			  << setw(2) << (int) IncidentEnergie << ";" // 21 ee : Code incident énergie
			  << interConferencier<<"\n";
	Trame = str_trame.str();
	str_trame.str("");
	ptrListeMessages->ajouterMessage (Trame);
	ptrSuperviseur->v.verbEtat(Trame);
}
//---------------------------------------------------------------------------------------------------------

void Etats::setInterConferencier(string  val){
    if(interConferencier !=  val)
    {
        pthread_mutex_lock(&semEtat);
        interConferencier =  val;
        NouveauMessage=true;
        pthread_mutex_unlock(&semEtat);
        pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
    }
}

//---------------------------------------------------------------------------------------------------------
string Etats::getTrame()
{
    contruireTrame();
    return Trame;
};

//---------------------------------------------------------------------------------------------------------
void Etats::SetDemIHM(unsigned int val)
{
    if(demIHM !=  val)
    {
        pthread_mutex_lock(&semEtat);
        demIHM =  val;
        NouveauMessage=true;
        pthread_mutex_unlock(&semEtat);
        pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
    };
}

//---------------------------------------------------------------------------------------------------------
void Etats::SetflagDemIHM(unsigned int val)
{
    unsigned int newdemIHM = demIHM | val;
    if(demIHM != newdemIHM)
    {
        pthread_mutex_lock(&semEtat);
        demIHM =  newdemIHM;
        NouveauMessage=true;
        pthread_mutex_unlock(&semEtat);
        pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
    };
}

//---------------------------------------------------------------------------------------------------------
void Etats::UnSetflagDemIHM(unsigned int val)
{
    unsigned int newdemIHM= demIHM & ~val;
    if(demIHM != newdemIHM)
    {
        pthread_mutex_lock(&semEtat);
        demIHM =  newdemIHM;
        NouveauMessage=true;
        pthread_mutex_unlock(&semEtat);
        pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
    }
}

//---------------------------------------------------------------------------------------------------------
void Etats::setEtatSysteme(unsigned char val)
{
	if(etatSysteme !=  val)
	{
		pthread_mutex_lock(&semEtat);
		etatSysteme =  val;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}

//---------------------------------------------------------------------------------------------------------
void Etats::setSeqEnCours(unsigned char seq)
{
	if(SeqEnCours!=seq)
	{
		pthread_mutex_lock(&semEtat);
		SeqEnCours=seq;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}

//---------------------------------------------------------------------------------------------------------
void Etats::setOrgARRU(unsigned char val)
{
	if(orgARRU !=  val)
	{
		pthread_mutex_lock(&semEtat);
cout<<"ORG_ARRU="<<(int)val<<endl;
		orgARRU =  val;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}

//---------------------------------------------------------------------------------------------------------
void Etats::setMode(unsigned char flag)
{
	if(Mode!=flag)
	{
		pthread_mutex_lock(&semEtat);
		Mode=flag;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}

//---------------------------------------------------------------------------------------------------------
void Etats::SetFlagMode(unsigned char flag)
{
    unsigned char newmode=Mode|flag;
    if(Mode!=newmode)
    {
        pthread_mutex_lock(&semEtat);
        Mode=newmode;
        NouveauMessage=true;
        pthread_mutex_unlock(&semEtat);
        pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
    }
}

//---------------------------------------------------------------------------------------------------------
void Etats::UnsetFlagMode(unsigned char flag)
{
    unsigned char newmode=Mode & ~flag;
    if(Mode!=newmode)
    {
        pthread_mutex_lock(&semEtat);
        Mode=newmode;
         NouveauMessage=true;
        pthread_mutex_unlock(&semEtat);
        pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
    }
}

//---------------------------------------------------------------------------------------------------------
void Etats::setEtatSEIS(unsigned char val)
{
	if(Seis !=  val)
	{
		pthread_mutex_lock(&semEtat);
		Seis =  val;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}

///////////// debut ajout \\\\\\\\\\\\\\\\\\\/
//---------------------------------------------------------------------------------------------------------
void Etats::SetflagCompteRenduSysteme(unsigned int flag)
{
    unsigned int newflag=CompteRenduSysteme|flag;
    if(flag!=newflag)
    {
        pthread_mutex_lock(&semEtat);
        CompteRenduSysteme=newflag;
        NouveauMessage=true;
        pthread_mutex_unlock(&semEtat);
        pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
    }
}

//---------------------------------------------------------------------------------------------------------
void Etats::UnsetflagCompteRenduSysteme(unsigned int flag)
{
    unsigned int newflag=CompteRenduSysteme & ~flag;
    if(flag!=newflag)
    {
        pthread_mutex_lock(&semEtat);
        CompteRenduSysteme=newflag;
        NouveauMessage=true;
        pthread_mutex_unlock(&semEtat);
        pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
    }
}

//---------------------------------------------------------------------------------------------------------
void Etats::SetFlagEtatSEIS(unsigned char flag)
{
    unsigned char newetat=Seis|flag;
    if(Seis!=newetat)
    {
        pthread_mutex_lock(&semEtat);
        Seis=newetat;
        NouveauMessage=true;
        pthread_mutex_unlock(&semEtat);
        pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
    }
}

//---------------------------------------------------------------------------------------------------------
void Etats::UnsetFlagEtatSEIS(unsigned char flag)
{
    unsigned char newetat=Seis & ~flag;
    if(Seis!=newetat)
    {
        pthread_mutex_lock(&semEtat);
        Seis=newetat;
        NouveauMessage=true;
        pthread_mutex_unlock(&semEtat);
        pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
    }
}

//---------------------------------------------------------------------------------------------------------

void Etats::setEtatWTS(unsigned char val)
{
	if(WTS !=  val)
	{
		pthread_mutex_lock(&semEtat);
		WTS =  val;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}


void Etats::setEtatHP3(unsigned char val)
{
	if(HP3!=  val)
	{
		pthread_mutex_lock(&semEtat);
		HP3 =  val;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}

void Etats::setEtatPanneaux(unsigned char val)
{
	if(EtatPanneaux!=  val)
	{
		pthread_mutex_lock(&semEtat);
		EtatPanneaux=  val;

		switch(val)
		{
		case PANNEAUX_FERMES :

			if(IncidentPanneaux & PANNEAU_1_HORS_SERVICE) _Panneau1 = PANNEAU_1_HS;
			else _Panneau1 = PANNEAU_1_FERME;
			if(IncidentPanneaux & PANNEAU_2_HORS_SERVICE) _Panneau1 = PANNEAU_2_HS;
			else _Panneau2 = PANNEAU_2_FERME;
			break;
		case PANNEAUX_1_OUVERTURE_BRAS_EN_COURS		:
			_Panneau1 = PANNEAU_1_OUVERTURE_BRAS_EN_COURS;
			break;
		case PANNEAUX_1_OUVERTURE_VOLETS_EN_COURS	:
			_Panneau1 = PANNEAU_1_OUVERTURE_VOLETS_EN_COURS;
			break;
		case PANNEAUX_2_OUVERTURE_BRAS_EN_COURS		:
			_Panneau2 = PANNEAU_2_OUVERTURE_BRAS_EN_COURS;
			break;
		case PANNEAUX_2_OUVERTURE_VOLETS_EN_COURS	:
			_Panneau2 = PANNEAU_2_OUVERTURE_VOLETS_EN_COURS;
			break;
		case PANNEAUX_OUVERTS							:
			if(IncidentPanneaux & PANNEAU_1_HORS_SERVICE) _Panneau1 = PANNEAU_1_HS;
			else _Panneau1 = PANNEAU_1_OUVERT ;
			if(IncidentPanneaux & PANNEAU_2_HORS_SERVICE) _Panneau1 = PANNEAU_2_HS;
			else _Panneau2 = PANNEAU_2_OUVERT ;
			break;
		case PANNEAUX_1_FERMETURE_VOLETS_EN_COURS	:
			_Panneau1 = PANNEAU_1_FERMETURE_VOLETS_EN_COURS;
			break;
		case PANNEAUX_1_FERMETURE_BRAS_EN_COURS		:
			_Panneau1 = PANNEAU_1_FERMETURE_BRAS_EN_COURS;
			break;
		case PANNEAUX_2_FERMETURE_VOLETS_EN_COURS	:
			_Panneau2 = PANNEAU_2_FERMETURE_VOLETS_EN_COURS;
			break;
		case PANNEAUX_2_FERMETURE_BRAS_EN_COURS		:
			_Panneau2 = PANNEAU_2_FERMETURE_BRAS_EN_COURS;
			break;
		case INCIDENT_OUVERTURE_PANNEAU_1		:
			_Panneau1 = INCIDENT_DURANT_OUVERTURE_PANNEAU_1;
			break;
		case REPLI_PANNEAU_1_SUR_INCIDENT		:
			_Panneau1 = REPLI_PANNEAU_1_SUITE_A_INCIDENT;
			break;
		case INCIDENT_FERMETURE_PANNEAU_1		:
			_Panneau1 = INCIDENT_DURANT_FERMETURE_PANNEAU_1;
			break;
		case REOUVERTURE_PANNEAU_1_SUR_INCIDENT		:
			_Panneau1 = REOUVERTURE_PANNEAU_1_SUITE_A_INCIDENT;
			break;

		case INCIDENT_OUVERTURE_PANNEAU_2		:
			_Panneau2 = INCIDENT_DURANT_OUVERTURE_PANNEAU_2;
			break;
		case REPLI_PANNEAU_2_SUR_INCIDENT		:
			_Panneau2 = REPLI_PANNEAU_2_SUITE_A_INCIDENT;
			break;
		case INCIDENT_FERMETURE_PANNEAU_2		:
			_Panneau2 = INCIDENT_DURANT_FERMETURE_PANNEAU_2;
			break;
		case REOUVERTURE_PANNEAU_2_SUR_INCIDENT		:
			_Panneau2 = REOUVERTURE_PANNEAU_2_SUITE_A_INCIDENT;
			break;
		case PHASE_INIT + NON_INIT		:
			_Panneau1 = PANNEAU_1_NON_INIT;
			_Panneau2 = PANNEAU_2_NON_INIT;
			break;
		case PHASE_INIT + INIT_EN_COURS		:
			_Panneau1 = PANNEAU_1_INIT_EN_COURS;
			_Panneau2 = PANNEAU_2_INIT_EN_COURS;
			break;
		case PHASE_INIT + INIT_BON		:
			_Panneau1 = PANNEAU_1_FERME;
			_Panneau2 = PANNEAU_2_FERME;
			break;
		case PHASE_INIT + PANNEAU1_SEUL		:
			_Panneau2 = PANNEAU_2_HS;
			break;
		case PHASE_INIT + PANNEAU2_SEUL		:
			_Panneau1 = PANNEAU_1_HS;
			break;
		case INCIDENT_REDHIBITOIRE_PANNEAUX		:
			_Panneau1 = PANNEAU_1_HS;
			_Panneau2 = PANNEAU_2_HS;
			break;
		}
		EtatPanneauxIHM = _Panneau1 + _Panneau2 ;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
		stringstream ss;
		ss <<"recu en parametre :"<< (int)val<<" ( 0x"<< hex << setfill('0')<<setw(2)<< (int)val 
			<<") , donne 'Etat Panneaux' : " << dec << (int)EtatPanneauxIHM<<" (0x"<< hex << setfill('0')<<setw(2)<<")";
		ptrSuperviseur->v.verbMsg(Verbose::DBG,"ETATS::SET_ETAT_PANNEAUX",ss.str());
	}
}


void Etats::setEtatPince(unsigned char val)
{
	if(EtatPince!=  val)
	{
		pthread_mutex_lock(&semEtat);
		EtatPince=  val;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}


void Etats::setEtatBras(unsigned char etatBras)
{
	if(EtatBras!=etatBras)
	{
		pthread_mutex_lock(&semEtat);
		EtatBras=etatBras;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}

void Etats::setCompteRenduSysteme(unsigned int val)
{
	if(CompteRenduSysteme!=  val)
	{
		pthread_mutex_lock(&semEtat);
		CompteRenduSysteme =  val;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}

//---------------------------------------------------------------------------------------------------------
void Etats::UnsetflagIncidentPanneaux(unsigned int incident)
{
    unsigned int newflag=IncidentPanneaux & ~incident;
    if(incident!=newflag)
    {
        pthread_mutex_lock(&semEtat);
        IncidentPanneaux=newflag;
        NouveauMessage=true;
        pthread_mutex_unlock(&semEtat);
        pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
    }
}

//---------------------------------------------------------------------------------------------------------
void Etats::SetflagIncidentPanneaux(unsigned int incident)
{
    unsigned int newflag=IncidentPanneaux|incident;
    if(incident!=newflag)
    {
        pthread_mutex_lock(&semEtat);
        IncidentPanneaux=newflag;
        NouveauMessage=true;
        pthread_mutex_unlock(&semEtat);
        pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
    }
}

void Etats::setIncidentPanneaux(unsigned int val)
{
	if(IncidentPanneaux !=  val)
	{
		pthread_mutex_lock(&semEtat);
		IncidentPanneaux = val;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}

void Etats::setErreurSEIS(unsigned int val)
{
	if(ErrSEIS!=  val)
	{
		pthread_mutex_lock(&semEtat);
		ErrSEIS =  val;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}

void Etats::setCodeErreurBras(unsigned int val)
{
	if(CodeErrBrasPince!=  val)
	{
		pthread_mutex_lock(&semEtat);
		CodeErrBrasPince =  val;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}

void Etats::setTensionBatteries(short val)
{
	if(TensionBatteries!=  val)
	{
		pthread_mutex_lock(&semEtat);
		TensionBatteries =  val;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}

void Etats::setChargeBatteries(short val)
{
	if(ChargeBatteries!=  val)
	{
		pthread_mutex_lock(&semEtat);
		ChargeBatteries =  val;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}

void Etats::setCourantBatteries(short val)
{
	if(CourantBatteries !=  val)
	{
		pthread_mutex_lock(&semEtat);
		CourantBatteries =  val;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}

void Etats::setDureeRestanteBatteries(short val)
{
	if(DureeRestante !=  val)
	{
		pthread_mutex_lock(&semEtat);
		DureeRestante =  val;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}

void Etats::SetChargeConsommee(short val)
{
	if(ChargeConsommee !=  val)
	{
		pthread_mutex_lock(&semEtat);
		ChargeConsommee =  val;
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}
//---------------------------------------------------------------------------------------------------------
void Etats::SetflagIncidentBatteries(unsigned char val)
{
    int newflag = IncidentEnergie | val;
    if(IncidentEnergie != newflag )
    {
        pthread_mutex_lock(&semEtat);
        IncidentEnergie =  newflag ;
        NouveauMessage=true;
        pthread_mutex_unlock(&semEtat);
        pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
    }
}

//---------------------------------------------------------------------------------------------------------
void Etats::UnsetflagIncidentBatteries(unsigned char val)
{
    int newflag= IncidentEnergie & ~val;
    if(IncidentEnergie != newflag)
    {
        pthread_mutex_lock(&semEtat);
        IncidentEnergie =  newflag;
        NouveauMessage=true;
        pthread_mutex_unlock(&semEtat);
        pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
    }
}

//---------------------------------------------------------------------------------------------------------

void Etats::setIncidentBatteries(unsigned char val)
{
	if(IncidentEnergie !=  val)
	{
		pthread_mutex_lock(&semEtat);
		IncidentEnergie =  val;
		pthread_mutex_unlock(&semEtat);
		NouveauMessage=true;
		pthread_mutex_unlock(&semEtat);
		pthread_cond_signal (&NouveauMessageRecue); // on signale qu'il y a un changement d'etat
	}
}

//============= les GET ============================
unsigned char Etats::getOrgARRU()
{
    return orgARRU;
}

unsigned char Etats::getEtatSysteme()
{
    return etatSysteme;
}

unsigned char Etats::getEtatPanneaux()
{
    return EtatPanneaux;
}

unsigned char Etats::getEtatPince()
{
    return EtatPince;
}

unsigned char Etats::getEtatBras()
{
    return EtatBras;
}

unsigned char Etats::getSituationSEIS()
{
    return Seis;
}

unsigned char Etats::getSituationWTS()
{
    return WTS;
}

unsigned char Etats::getSituationHP3()
{
    return HP3;
}


unsigned char Etats::getMode()
{
    return Mode;
}

unsigned char Etats::getIncidentBatteries()
{
    return IncidentEnergie;
}

int Etats::GetDemIHM()
{
    return demIHM;
}




