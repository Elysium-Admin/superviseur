//
// pour animer un terminal http://nicofo.tuxfamily.org/index.php?post/2007/01/10/19-mouvement-du-curseur-dans-le-terminal
//
// Ligne de compilation hors makefile
//g++ -c ./src/Analyseur.cpp  -o ./lib/Analyseur.o


#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "../defs/Analyseur.h"
#include "../../supervision/defs/Etats.h"
#include "../../supervision/defs/ProxySEIS.h"
#include <string.h>

#define 	NB_CHAMPS_DANS_TRAME_ETAT	21


void AnalyseurDetat::analyserTrame(string trame)
{
	Trame=trame;
	if(oldTrame!=Trame)
	{
		// on efface les 8 1ère lignes
		cout << "\033[1;1H\033[K\033[1B\033[K\033[1B\033[K\033[1B\033[K\033[1B\033[K\033[1B\033[K\033[1B\033[K\033[1B\033[K";
		// on se positionne en debut de 1ere ligne en preparant la couleur du texte et celle du fond
		cout << "\033[1;1H\033[40;37;0m";
		cout<<"Mesures :\033[K"<<LastMesures<<endl;
		cout << "\033[9;1H";  // <-- positionne le curseur en ligne 9 colone 1
		string champ;
		string oldchamp;
		int lastpos;
		int newpos;
		cout<<"==============================================================================================================="<<endl;
		cout<<"Trame: "<<Trame<<endl;
		cout<<"---------------------------------------------------------------------------------------------------------------"<<endl;
		lastpos=Trame.find_first_of('e',0);
		for(int i=0; i<NB_CHAMPS_DANS_TRAME_ETAT; i++)
		{
			newpos=Trame.find_first_of(';',lastpos+1);
			champ=Trame.substr (lastpos+1,newpos-lastpos-1);
			change=false;
			if(oldTrame.length()== Trame.length())
			{
				oldchamp=oldTrame.substr (lastpos+1,newpos-lastpos-1);
				if(champ!=oldchamp)
				{
					change=true;
				}
			}
			interpreteChamp(champ,i+1);
			lastpos=newpos;
		}
		oldTrame=Trame;
	}
}

void AnalyseurDetat::afficheMesures(string mesures)
{
	LastMesures=mesures;
	cout << "\033[K\033[1;1H";
	cout<<"Mesures :\033[0;31;40m"<<LastMesures<<"\033[40;37;0m"<<endl;
}


void AnalyseurDetat::interpreteChamp(string champ, int num)
{
	switch (num)
	{
	case 1:
		_OrgARRU(champ);
		break;
	case 2:
		_EtatSysteme(champ);
		break;
	case 3:
		_SeqEnCours(champ);
		break;
	case 4:
		_Mode(champ);
		break;
	case 5:
		_CompteRenduSysteme(champ);
		break;
	case 6:
		_DemIHM(champ);
		break;
	case 7:
		_EtatSEIS(champ);
		break;
	case 8:
		_ErreurSEIS(champ);
		break;
	case 9:
		_EtatWTS(champ);
		break;
	case 10:
		_EtatHP3(champ);
		break;
	case 11:
		_EtatPanneaux(champ);
		break;
	case 12:
		_IncidentPanneaux(champ);
		break;
	case 13:
		_EtatPince(champ);
		break;
	case 14:
		_EtatBras(champ);
		break;
	case 15:
		_CodeErreurBras(champ);
		break;
	case 16:
		_TensionBateries(champ);
		break;
	case 17:
		_ChargeBateries(champ);
		break;
	case 18:
		_CourantBateries(champ);
		break;
	case 19:
		_ChargeConsommee(champ);
		break;
	case 20:
		_DureeRestanteBateries(champ);
		break;
	case 21:
		_IncidentBateries(champ);
		break;
	default:
		cout<<"??? champ "<<num<<" inconnu !!!"<<endl;
		break;
	}
}

void AnalyseurDetat::_OrgARRU(string champ)
{
	cout<<"\033[K\033[32;0m"<<champ<<"\033[34;1m"<<"\t  ... OrgARRU\t\t\t: ";
	if(change)cout<<"\033[1;47;31m";
	else cout<<"\033[40;37;0m";
	if(champ=="00") cout<<" PAS_ARRU";
	else if(champ=="01") cout<<"        ARRU_MATERIEL";
	else if(champ=="02") cout<<"                     ARRU_LOGICIEL";
	else cout<<"?????? valeur "<< champ<<" inattendue";
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_EtatSysteme(string champ)
{
	cout<<"\033[K\033[32;0m"<<champ<<"\033[34;1m"<<"\t  ... etatSysteme\t\t: ";
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	if(champ=="00") cout<<" ATTENTE";
	else if(champ=="01") cout<<"  .TESTS_MISE_EN_ROUTE";
	else if(champ=="02") cout<<"  ..INITILISATION";
	else if(champ=="03") cout<<"     COMMANDES";
	else if(champ=="04") cout<<"   CLOTURE";
	else if(champ=="05") cout<<"   *** ARRU_DEMANDEE ***";
	else cout<<"?????? valeur "<< champ<<" inattendue";
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_SeqEnCours(string champ)
{
	cout<<"\033[K\033[32;0m"<<champ<<"\033[34;1m"<<"\t  ... SeqEnCours\t\t: ";
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	if(champ=="00") cout<<" RIEN";
	else if(champ=="01") cout<<"        DEPOSE_SEIS";
	else if(champ=="02") cout<<"        REPRISE_SEIS";
	else if(champ=="03") cout<<"        DEPOSE_WTS";
	else if(champ=="04") cout<<"        REPRISE_WTS";
	else if(champ=="05") cout<<"        DEPOSE_HP3";
	else if(champ=="06") cout<<"        REPRISE_HP3";
	else if(champ=="07") cout<<"        PANORAMIQUE";
	else if(champ=="08") cout<<"        ARRET_SYSTEM";
	else if(champ=="09") cout<<"        OUVERTURE PANNEAUX";
	else if(champ=="0a") cout<<"        FERMETURE PANNEAUX";
	else cout<<"?????? valeur "<< champ<<" inattendue";
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_Mode(string champ)
{
	cout<<"\033[K\033[32;40;0m"<<champ<<"\033[34;40;1m"<<"\t  ... Mode\t\t\t: ";
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	if(champ=="00") cout<<" tout fonctionne";
	else
	{
		unsigned char modes;
		sscanf(champ.c_str(), "%x", &modes);
		string bilan=" ";
		if(modes&Etats::M_ALARME_BATTERIES)bilan+=" M_ALARME_BATTERIES ";
		if(modes&Etats::M_BATTERIES_HS)bilan+=" M_BATTERIES_HS ";
		if(modes&Etats::M_PANNEAUX_HS)bilan+=" M_PANNEAUX_HS ";
		if(modes&Etats::M_BRAS_HS)bilan+=" M_BRAS_HS ";
		if(modes&Etats::M_SEIS_HS)bilan+=" M_SEIS_HS ";
		if(modes&Etats::HTTP_CAM_HS)bilan+=" HTTP_CAM_HS ";
		cout<<bilan;
	}
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_CompteRenduSysteme(string champ)
{
	cout<<"\033[K\033[32;0m"<<champ<<"\033[34;1m"<<"  ... CompteRenduSysteme\t: ";
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	if(champ=="00000000") cout<<" R.A.S.";
	else
	{
		int valeur;
		string texte="";
		sscanf(champ.c_str(), "%x", &valeur);
		if( valeur & Etats::RUPTURE_LIAISON_SEIS) texte+=" Rupture liaison SEIS détectée ;";
		if( valeur & Etats::RUPTURE_LIAISON_BRAS) texte+=" Rupture liaison BRAS détectée ;";
		if( valeur & Etats::RUPTURE_LIAISON_PANNEAUX) texte+=" Rupture PANNEAUX détectée ;";
		if( valeur & Etats::RUPTURE_LIAISON_CCGX) texte+=" Rupture liaison CCGX détectée ;";
		if( valeur & Etats::RUPTURE_LIAISON_CAMERA) texte+=" Rupture liaison CAMERA détectée ;";
		if( valeur & Etats::COMMANDE_INCONNUE) texte+=" Commande inconnue ;";
		if( valeur & Etats::CCGX_NON_TESTE) texte+="CCGX_NON_TESTE ";
		if( valeur & Etats::PANNEAUX_NON_TESTES) texte+="PANNEAUX_NONTESTES ";
		if( valeur & Etats::SEIS_NON_TESTE) texte+=" SEIS_NONTESTE ";
		if( valeur & Etats::CAMERA_NON_TESTE) texte+="CAMERA_NONTESTEE ";
		if( valeur & Etats::BRAS_NON_TESTE) texte+="BRAS_NONTESTE ";
		if( valeur & Etats::CCGX_TESTS_EN_COURS) texte+="CCGX_TESTS_ENCOURS ";
		if( valeur & Etats::PANNEAUX_TESTS_EN_COURS) texte+="PANNEAUX_TESTS_ENCOURS ";
		if( valeur & Etats::SEIS_TESTS_EN_COURS) texte+="SEIS_TESTS_ENCOURS ";
		if( valeur & Etats::CAMERA_TESTS_EN_COURS) texte+="CAMERA_TESTS_ENCOURS ";
		if( valeur & Etats::BRAS_TESTS_EN_COURS) texte+="BRAS_TESTS_ENCOURS ";
		cout<<texte;
	}
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_DemIHM(string champ)
{
	cout<<"\033[K\033[36;0m"<<champ<<"\033[34;1m"<<"  ... DemIHM\t\t\t: ";
	if(champ=="00000000") cout<<"\033[40;37;0m PAS_DE_DEMANDE";
	else
	{
		int demandes;
		sscanf(champ.c_str(), "%x", &demandes);
		cout<<"\033[42;30;1m";
		if(demandes & 0x0000ffff)
		{
			string confim=" confirmation attendue : ";
			if(demandes&Etats::DEM_CONF_OUVERTURE_PINCE)confim+="DEM_CONF_OUVERTURE_PINCE ";
			if(demandes&Etats::DEM_CONF_POSE_HP3)confim+="DEM_CONF_POSE_HP3 ";
			if(demandes&Etats::DEM_CONF_LACHER_WTS)confim+="DEM_CONF_LACHER_WTS ";
			if(demandes&Etats::DEM_CONF_SAISIR_WTS)confim+="DEM_CONF_SAISIR_WTS ";
			if(demandes&Etats::DEM_CONF_SEIS_COMMENCER_NIVELLEMENT)confim+="DEM_CONF_SEIS_COMMENCER_NIVELLEMENT ";
			if(demandes&Etats::DEM_CONF_LACHER_SEIS)confim+="DEM_CONF_LACHER_SEIS ";
			if(demandes&Etats::DEM_CONF_SAISIR_SEIS)confim+="DEM_CONF_SAISIR_SEIS ";
			if(demandes&Etats::DEM_CONF_LANCER_TESTS)confim+="DEM_CONF_LANCER_TESTS ";
			if(demandes&Etats::DEM_CONF_FERMETURE_SUR_PB_OUVERT_PAN_1)confim+="DEM_CONF_FERMETURE_PB_OUVERTURE_PAN_1 ";
			if(demandes&Etats::DEM_CONF_FERMETURE_SUR_PB_OUVERT_PAN_2)confim+="DEM_CONF_FERMETURE_PB_OUVERTURE_PAN_2 ";
			if(demandes&Etats::DEM_CONF_ROUVERTURE_SUR_PB_FERMET_PAN_1)confim+="DEM_CONF_REOUVERTURE_PB_FERMETURE_PAN_1 ";
			if(demandes&Etats::DEM_CONF_ROUVERTURE_SUR_PB_FERMET_PAN_2)confim+="DEM_CONF_REOUVERTURE_PB_FERMETURE_PAN_2 ";
			if(demandes&Etats::DEM_CONF_REMONTER_HP3_POUR_ARRET)confim+="DEM_CONF_REMONTER_HP3_POUR_ARRET ";
			if(demandes&Etats::DEM_CONF_REMONTER_WTS_POUR_ARRET)confim+="DEM_CONF_REMONTER_WTS_POUR_ARRET ";
			if(demandes&Etats::DEM_CONF_REMONTER_SEIS_POUR_ARRET)confim+="DEM_CONF_REMONTER_SEIS_POUR_ARRET ";
			if(demandes&Etats::DEM_CONF_FERMER_PANNEAUX_POUR_ARRET)confim+="DEM_CONF_FERMER_PANNEAUX_POUR_ARRET ";
			cout<<confim;
		}
		else if(demandes & 0xffff0000)
		{
			string decide="    decision IHM sollicitée : ";
			if(demandes&Etats::DEM_DECISION_BATTERIES_FAIBLES)decide+="DEM_DECISION_BATTERIES_FAIBLES ";
			if(demandes&Etats::DEM_DECISION_SORTIE_TEST)decide+="DEM_DECISION_SORTIE_TEST ";
			if(demandes&Etats::DEM_DECISION_SORTIE_INITIALISATION)decide+="DEM_DECISION_SORTIE_INITIALISATION ";
			if(demandes&Etats::DEM_DECISION_DEFAUT_DETECTE)decide+="DEM_DECISION_DEFAUT_DETECTE ";
			cout<<decide;
		}
		else
		{
			cout<< "??? normalement c'est impossible d'en arriver là! champs reçu "<<champ;
		}
	}
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_IncidentBateries(string champ)
{
	cout<<"\033[32;0m"<<champ<<"\033[34;1m"<<"\t  ... IncidentBatteries\t\t: ";
	if(change)cout<<"\033[K\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	if(champ=="00") cout<<" PAS DE PROBLEME BATTERIE";
	else
	{
		unsigned char incidents;
		sscanf(champ.c_str(), "%x", &incidents);
		string bilan=" ";
		if(incidents & Etats::ALARME_TENSION_BASSE)bilan+=" ALARME_TENSION_BASSE ";
		if(incidents & Etats::ALARME_TENSION_HAUTE)bilan+=" ALARME_TENSION_HAUTE ";
		if(incidents & Etats::ALARME_TEMPERATURE)bilan+=" ALARME_TEMPERATURE ";
		if(incidents & Etats::ALARME_STATUS_RELAIS)bilan+=" ALARME_STATUS_RELAIS ";
		if(incidents & Etats::TENSION_BASSE)bilan+=" TENSION_BASSE ";
		if(incidents & Etats::CHARGE_BASSE)bilan+=" CHARGE_BASSE ";
		cout<<bilan;
	}
	cout<<"\033[40;37;0m\033[K"<<endl;
}


void AnalyseurDetat::_EtatSEIS(string champ)
{
	cout<<"\033[K\033[32;0m"<<champ<<"\033[34;1m"<<"\t  ... EtatSEIS\t\t\t: ";
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	if(champ=="00") cout<<" ... INDETERMINE...";
	else if(champ=="01") cout<<" SUR_PLATEAU";
	else if(champ=="02") cout<<" DEPOSE_EN_COURS";
	else if(champ=="04") cout<<" AU_SOL";
	else if(champ=="08") cout<<" REPRISE_EN_COURS";
	else if(champ=="10") cout<<"  ---- INDISPONIBLE ----";
	else if(champ=="20") cout<<" NIVELLEMENT EN  COURS ";
	else if(champ=="40") cout<<" SEIS PRET A ETRE LACHE ";
	else if(champ=="84") cout<<" MESURES_EN_COURS";
	else cout<<"?????? valeur "<< champ<<" inattendue";
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_ErreurSEIS(string champ)
{
	cout<<"\033[K\033[32;0m"<<champ<<"\033[34;1m"<<"  ... ErreurSEIS\t\t: ";
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	if(champ=="00000000") cout<<" PAS_D_ERREUR_SEIS ";
	else
	{
		unsigned char incidents;
		sscanf(champ.c_str(), "%x", &incidents);
		string bilan=" ";
		if(incidents & ProxySEIS::BATTERIE_SEIS_FAIBLE)bilan+=" BATTERIE_SEIS_FAIBLE ";
		if(incidents & ProxySEIS::CAPTEUR_PROMITE_1_EN_DEFAUT)bilan+=" CAPTEUR_PROMITE_1_EN_DEFAUT ";
		if(incidents & ProxySEIS::CAPTEUR_PROMITE_2_EN_DEFAUT)bilan+=" CAPTEUR_PROMITE_2_EN_DEFAUT ";
		if(incidents & ProxySEIS::CAPTEUR_PROMITE_3_EN_DEFAUT)bilan+=" CAPTEUR_PROMITE_3_EN_DEFAUT ";
		if(incidents & ProxySEIS::MOTEUR_1_ERREUR_EN_MONTEE)bilan+=" MOTEUR_1_ERREUR_EN_MONTEE ";
		if(incidents & ProxySEIS::MOTEUR_2_ERREUR_EN_MONTEE)bilan+=" MOTEUR_2_ERREUR_EN_MONTEE ";
		if(incidents & ProxySEIS::MOTEUR_3_ERREUR_EN_MONTEE)bilan+=" MOTEUR_3_ERREUR_EN_MONTEE ";
		if(incidents & ProxySEIS::MOTEUR_1_ERREUR_EN_DESCENTE)bilan+=" MOTEUR_1_ERREUR_EN_DESCENTE ";
		if(incidents & ProxySEIS::MOTEUR_2_ERREUR_EN_DESCENTE)bilan+=" MOTEUR_2_ERREUR_EN_DESCENTE ";
		if(incidents & ProxySEIS::MOTEUR_3_ERREUR_EN_DESCENTE)bilan+=" MOTEUR_3_ERREUR_EN_DESCENTE ";
		if(incidents & ProxySEIS::POTENTIOMETRE_1_DEFECTUEUX)bilan+=" POTENTIOMETRE_1_DEFECTUEUX ";
		if(incidents & ProxySEIS::POTENTIOMETRE_2_DEFECTUEUX)bilan+=" POTENTIOMETRE_2_DEFECTUEUX ";
		if(incidents & ProxySEIS::POTENTIOMETRE_3_DEFECTUEUX)bilan+=" POTENTIOMETRE_3_DEFECTUEUX ";
		if(incidents & ProxySEIS::ACCELEROMETRE_1_DEFECTUEUX)bilan+=" ACCELEROMETRE_1_DEFECTUEUX ";
		if(incidents & ProxySEIS::ACCELEROMETRE_2_DEFECTUEUX)bilan+=" ACCELEROMETRE_2_DEFECTUEUX ";
		if(incidents & ProxySEIS::ACCELEROMETRE_3_DEFECTUEUX)bilan+=" ACCELEROMETRE_3_DEFECTUEUX ";
		if(incidents & ProxySEIS::ECHEC_CONNEXION_SEIS)bilan+=" ECHEC_CONNEXION_SEIS ";
		if(incidents & ProxySEIS::ERREUR_PROTOCOLE)bilan+=" ERREUR_PROTOCOLE ";
		cout<<bilan;
	}
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_EtatWTS(string champ)
{
	cout<<"\033[K\033[32;0m"<<champ<<"\033[34;1m"<<"\t  ... EtatWTS\t\t\t: ";
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	if(champ=="00") cout<<" ... INDETERMINE...";
	else if(champ=="01") cout<<" SUR_PLATEAU";
	else if(champ=="02") cout<<" DEPOSE_EN_COURS";
	else if(champ=="04") cout<<" AU_SOL";
	else if(champ=="08") cout<<" REPRISE_EN_COURS";
	else if(champ=="10") cout<<"  ---- INDISPONIBLE ----";
	else cout<<"?????? valeur "<< champ<<" inattendue";
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_EtatHP3(string champ)
{
	cout<<"\033[K\033[32;0m"<<champ<<"\033[34;1m"<<"\t  ... EtatHP3\t\t\t: ";
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	if(champ=="00") cout<<" ... INDETERMINE...";
	else if(champ=="01") cout<<" SUR_PLATEAU";
	else if(champ=="02") cout<<" DEPOSE_EN_COURS";
	else if(champ=="04") cout<<" AU_SOL";
	else if(champ=="08") cout<<" REPRISE_EN_COURS";
	else if(champ=="10") cout<<"  ---- INDISPONIBLE ----";
	else cout<<"?????? valeur "<< champ<<" inattendue";
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_EtatPanneaux(string champ)
{
	cout<<"\033[K\033[32;0m"<<champ<<"\033[34;1m"<<"\t  ... EtatPanneaux\t\t: ";
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	switch (champ[1])
	{
	case '0' :
		cout<<"  PANNEAU_1_FERME   et  ";
		break;
	case '1' :
		cout<<"  PANNEAU_1_OUVERTURE_BRAS_EN_COURS   et  ";
		break;
	case '2' :
		cout<<"  PANNEAU_1_OUVERTURE_VOLETS_EN_COURS  et ";
		break;
	case '3' :
		cout<<"  PANNEAU_1_OUVERT   et  ";
		break;
	case '4' :
		cout<<"  PANNEAU_1_FERMETURE_VOLETS_EN_COURS  et ";
		break;
	case '5' :
		cout<<"  PANNEAU_1_FERMETURE_BRAS_EN_COURS   et  ";
		break;
	case '6' :
		cout<<"  INCIDENT_DURANT_OUVERTURE_PANNEAU_1   et  ";
		break;
	case '7' :
		cout<<"  REPLI_PANNEAU_1_SUITE_A_INCIDENT   et  ";
		break;
	case '8' :
		cout<<"  INCIDENT_DURANT_FERMETURE_PANNEAU_1   et  ";
		break;
	case '9' :
		cout<<"  REOUVERTURE_PANNEAU_1_SUITE_A_INCIDENT  et ";
		break;
	case 'a' :
		cout<<"  PANNEAU_1_NON_INIT   et  ";
		break;
	case 'b' :
		cout<<"  PANNEAU_1_INIT_EN_COURS   et  ";
		break;
	case 'f' :
		cout<<"  PANNEAU_1_HS    et  ";
		break;
	default  :
		cout<<"  ?? panneau 1 ??   et   ";
		break;
	}
	switch (champ[0])
	{
	case '0' :
		cout<<"  PANNEAU_2_FERME ";
		break;
	case '1' :
		cout<<"  PANNEAU_2_OUVERTURE_BRAS_EN_COURS ";
		break;
	case '2' :
		cout<<"  PANNEAU_2_OUVERTURE_VOLETS_EN_COURS ";
		break;
	case '3' :
		cout<<"  PANNEAU_2_OUVERT";
		break;
	case '4' :
		cout<<"  PANNEAU_2_FERMETURE_VOLETS_EN_COURS ";
		break;
	case '5' :
		cout<<"  PANNEAU_2_FERMETURE_BRAS_EN_COURS";
		break;
	case '6' :
		cout<<"  INCIDENT_DURANT_OUVERTURE_PANNEAU_2 ";
		break;
	case '7' :
		cout<<"  REPLI_PANNEAU_2_SUITE_A_INCIDENT ";
		break;
	case '8' :
		cout<<"  INCIDENT_DURANT_FERMETURE_PANNEAU_2 ";
		break;
	case '9' :
		cout<<"  REOUVERTURE_PANNEAU_2_SUITE_A_INCIDENT ";
		break;
	case 'a' :
		cout<<"  PANNEAU_2_NON_INIT";
		break;
	case 'b' :
		cout<<"  PANNEAU_2_INIT_EN_COURS ";
		break;
	case 'f' :
		cout<<"  PANNEAU_2_HS ";
		break;
	default  :
		cout<<"  ?? panneau 2 ??";
		break;
	}
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_IncidentPanneaux(string champ)
{
	cout<<"\033[K\033[32;0m"<<champ<<"\033[34;1m"<<"  ... IncidentPanneaux\t\t: ";
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	if(champ=="00000000") cout<<" PAS_ERREUR ";
	else
	{
		unsigned char incidents;
		sscanf(champ.c_str(), "%x", &incidents);
		string bilan=" ";
		if(incidents & Etats::PANNEAU_1_HORS_SERVICE)bilan+=" PANNEAU_1_HORS_SERVICE ";
		if(incidents & Etats::PANNEAU_2_HORS_SERVICE)bilan+=" PANNEAU_2_HORS_SERVICE ";
		cout<<bilan;
	}
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_EtatPince(string champ)
{
	cout<<"\033[K\033[32;40;0m"<<champ<<"\033[34;1m"<<"\t  ... EtatPince\t\t\t: ";
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	if(champ=="00") cout<<" PINCE_OUVERTE";
	else if(champ=="01") cout<<" ...PINCE_EN_COURS_OUVERTURE";
	else if(champ=="02") cout<<"         PINCE_FERMEE";
	else if(champ=="03") cout<<"  ....PINCE_EN_COURS_FERMETURE";
	else cout<<"?????? valeur "<< champ<<" inattendue";
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_EtatBras(string champ)
{
	cout<<"\033[K\033[32;0m"<<champ<<"\033[34;1m"<<"\t  ... EtatBras\t\t\t: ";
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	if(champ=="00") cout<<" INCONNUE";
	else if(champ=="01") cout<<" POSITION_PANORAMIQUE";
	else if(champ=="02") cout<<" HORS_PLATEAU";
	else if(champ=="03") cout<<" PRET_A_SAISIR_SEIS";
	else if(champ=="04") cout<<" WTS_AU_SOL";
	else if(champ=="05") cout<<" PRET_A_SAISIR_WTS";
	else if(champ=="06") cout<<" PRET_DEPOSE_HP3_PLATEAU";
	else if(champ=="07") cout<<" BRAS_POSITION_TRANSPORT";
	else if(champ=="20") cout<<" INIT_BRAS_EN_COURS";
	else if(champ=="40") cout<<" ACTION_EN_COURS";
	else if(champ=="80") cout<<" INCIDENT_REDHIBITOIRE_BRAS";
	else cout<<"?????? valeur "<< champ<<" inattendue";
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_CodeErreurBras(string champ)
{
	cout<<"\033[K\033[32;0m"<<champ<<"\033[34;1m"<<"  ... CodeErreurBras\t\t: ";
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	cout<<champ<<"    ( non encore interprété )";
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_TensionBateries(string champ)
{
	cout<<"\033[K\033[32;0m"<<champ<<"\033[34;1m"<<"\t  ... TensionBateries\t\t: ";
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	int valeur;// int et non short pour pas générer une erreur avec le sscanf
	sscanf(champ.c_str(), "%x", &valeur);
	cout <<valeur<< " en dixième de volts";
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_ChargeBateries(string champ)
{
	cout<<"\033[K\033[32;0m"<<champ<<"\033[34;1m"<<"\t  ... ChargeBatteries\t\t: ";
	int valeur;// int et non short pour pas générer une erreur avec le sscanf
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	sscanf(champ.c_str(), "%x", &valeur);
	cout <<valeur<< " en Ampères.heures ??? ou en dixième d'Ampère.Heure ???";
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_CourantBateries(string champ)
{
	cout<<"\033[K\033[32;0m"<<champ<<"\033[34;1m"<<"\t  ... CourantBatteries\t\t: ";
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	int valeur;// int et non short pour pas générer une erreur avec le sscanf
	sscanf(champ.c_str(), "%x", &valeur);
	cout <<valeur<< " en Ampères  ??? ou en dixième d'Ampère  ???";
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_ChargeConsommee(string champ)
{
	cout<<"\033[K\033[32;0m"<<champ<<"\033[34;1m"<<"\t  ... ChargeConsommée\t\t: ";
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	int valeur;// int et non short pour pas générer une erreur avec le sscanf
	sscanf(champ.c_str(), "%x", &valeur);
	cout <<valeur<< " en Ampères.heures ??? ou en dixième d'Ampère.Heure ???";
	cout<<"\033[40;37;0m\033[K"<<endl;
}

void AnalyseurDetat::_DureeRestanteBateries(string champ)
{
	cout<<"\033[K\033[32;0m"<<champ<<"\033[34;1m"<<"\t  ... DuréeRestanteBatteries\t: ";
	if(change)cout<<"\033[47;31;1m";
	else cout<<"\033[40;37;0m";
	int valeur; // int et non short pour pas générer une erreur avec le sscanf
	sscanf(champ.c_str(), "%x", &valeur);
	valeur=100*valeur;
	int jours=valeur/(3600*24);
	int secondes=valeur%(3600*24);
	int heures=secondes/3600;
	secondes=secondes%3600;
	int minutes=secondes/60;
	secondes=secondes%60;
	cout <<jours<< " jours + "<<heures<< " heures + "<<minutes<< " minutes + "<<secondes<< " secondes ";
	cout<<"\033[40;37;0m\033[K"<<endl;
}


