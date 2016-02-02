//=====================================================
// Projet Insight
//
// Description: simulateur du cgi de commande
// Nom du fichier: simucgicmd.cpp
// Auteur:
// Organisme : Lycée International Victor Hugo Colomiers, BTS IRIS
//
// Date: 13/12/2013
//======================================================
//
// g++ ./src/simucgicmd.cpp  ../supervision/lib/sockets.o -o simucgicmd

#include <iostream>
#include <string.h>
#include <termios.h>

#include <fstream>
#include <iomanip>
#include <sstream>

#include "../../supervision/defs/sockets.h"
#include "../../supervision/defs/protocole.h"

//#include <arpa/inet.h>
//#include <stdlib.h> // Ne pas oublier pour getenv()
//#include <stdio.h>
//#include <sys/types.h>
//#include <unistd.h>
//#include <cstring>

#include <string>
#include <vector>

// ports des services loopback du superviseur
#define 	PORT_SERV_LB			5000

using namespace std;

std::vector<string*> HistoirqueCmds;

void ihm();
void ihmCmdNormales();
void ihmCmdUrgentes();
void ihmCmdSpeciales();
void ihmConfirmations();
void ihmDecisions();
void ihmHistorique();

//==================================================================================================================
char frpclav()
{
	static struct termios term, back;
	char ret=0xff;

	tcgetattr (0, &term); /* On recupere les info du terminal */
	memcpy (&back, &term, sizeof(term));

	term.c_lflag &= ~(ICANON|ECHO); /* On modifie les options du terminal */
	term.c_cc[VTIME] = 0; /* Temps d'attente infini */
	term.c_cc[VMIN] = 1; /* Nombre minimun de caractere ds le tampon pr débloquer la saisie*/

	tcsetattr(0, TCSANOW, &term); /* Modif des attribut de stdin */
	ret = getchar(); /* Attente d'un caractere */
	tcsetattr(0, TCSANOW, &back); /* Restauration des attributs */
	return ret;
}
//==================================================================================================================
string cmd;

int main()
{

	int canal;
	struct sockaddr_in server_ad;

	server_ad.sin_family      = AF_INET;
	server_ad.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_ad.sin_port = htons(PORT_SERV_LB);
	if((canal = sockClientUDPspecLoopback()) < 0)
	{
		cerr<<"échec création socket"<<endl;
		return 0;
	}
	while(1)
	{
		cmd="";
		ihm();
		if(cmd!="")sendto(canal, cmd.c_str(), cmd.length(),0,(struct sockaddr*)&server_ad, sizeof(sockaddr_in));
	}
	return 0;
}

//___________________________________________________________________________________
// les IHM
//___________________________________________________________________________________
void ihm()
{
	char car;
	cout << "\033[2J\033[1;1H";  // <-- efface l'écran
	cout<<"Liste des commandes , confirmation et décision possibles à passer au superviseur "<<endl;
	cout<<"-------------------------------------------------------------------------------- "<<endl<<endl;
	cout<<"        commandes            lettre à taper"<<endl;
	cout<<"pour une commande normale      .....  'n'"<<endl;
	cout<<"pour une commande urgente      .....  'u'"<<endl;
	cout<<"pour une commande spéciale     .....  's'"<<endl;
	cout<<"pour une confirmation          .....  'c'"<<endl;
	cout<<"pour une décision              .....  'd'"<<endl<<endl;

//	cout<<"pour l' historique des commandes  : 'h'"<<endl;
	do
	{
		car=frpclav();
	}
	while( (car!='n')&&(car!='n')&&(car!='u')&&(car!='s')&&(car!='c')&&(car!='d')&&(car!='h')  );
	switch(car)
	{
	case 'n':
		ihmCmdNormales();
		break;
	case 'u':
		ihmCmdUrgentes();
		break;
	case 's':
		ihmCmdSpeciales();
		break;
	case 'c':
		ihmConfirmations();
		break;
	case 'd':
		ihmDecisions();
		break;
	case 'h':
		ihmHistorique();
		break;
	}
}

//___________________________________________________________________________________
void ihmCmdNormales()
{
	char car;
	cout << "\033[2J\033[1;1H";  // <-- efface l'écran
	cout<<"  menu des commandes normales"<<endl;
	cout<<"  ---------------------------"<<endl;
	cout<<"\tpour OUVERTURE_PANNEAUX    ..... '0'"<<endl;
	cout<<"\tpour FERMETURE_PANNEAUX    ..... '1'"<<endl;
	cout<<"\tpour DEPOSE_SEIS           ..... '2'"<<endl;
	cout<<"\tpour REPRISE_SEIS          ..... '3'"<<endl;
	cout<<"\tpour DEPOSE_WTS            ..... '4'"<<endl;
	cout<<"\tpour REPRISE_WTS           ..... '5'"<<endl;
	cout<<"\tpour DEPOSE_HP3            ..... '6'"<<endl;
	cout<<"\tpour REPRISE_HP3           ..... '7'"<<endl;
	cout<<"\tpour ARRET_SYSTEME         ..... 'a'"<<endl;

	cout<<endl<<"pour revenir au menu principal	: 'r'"<<endl;
	do
	{
		car=frpclav();
	}
	while( !( (car=='a')||(car=='r')|| ((car>='0')&&(car<='7')) ) );
	switch(car)
	{
	case '0':
		cmd=C_OUVERTURE_PANNEAUX;
		break;
	case '1':
		cmd=C_FERMETURE_PANNEAUX;
		break;
	case '2':
		cmd=C_DEPOSE_SEIS;
		break;
	case '3':
		cmd=C_REPRISE_SEIS;
		break;
	case '4':
		cmd=C_DEPOSE_WTS;
		break;
	case '5':
		cmd=C_REPRISE_WTS;
		break;
	case '6':
		cmd=C_DEPOSE_HP3;
		break;
	case '7':
		cmd=C_REPRISE_HP3;
		break;
	case 'a':
		cmd=C_ARRET_SYSTEME;
		break;
	default :
		break;
	}
}
//___________________________________________________________________________________
void ihmCmdUrgentes()
{
	char car;
	cout << "\033[2J\033[1;1H";  // <-- efface l'écran
	cout<<"  menu des commandes urgentes"<<endl;
	cout<<"  ---------------------------"<<endl;
	cout<<"\t ARRET  URGENCE                ..... '0'"<<endl;
	cout<<"\t PAS DE VERBOSE                ..... '1'"<<endl;
	cout<<"\t VERBOSE DES ERREURS_SEULES    ..... '2'"<<endl;
	cout<<"\t VERBOSE DES ERREURS_ET_ACTIONS..... '3'"<<endl;
	cout<<"\t VERBOSE_DETAILLEE             ..... '4'"<<endl;
	cout<<"\t FIN ARRU LOGICIEL             ..... '5'"<<endl;

	cout<<endl<<"pour revenir au menu principal	: 'r'"<<endl;
	do
	{
		car=frpclav();
	}
	while( ((car<'0')||(car>'5'))&&(car!='r') );
	switch(car)
	{
	case '0':
		cmd=U_ARRU;
		break;
	case '1':
		cmd=U_PAS_DE_VERBOSE;
		break;
	case '2':
		cmd=U_VERBOSE_ERREURS_SEULES;
		break;
	case '3':
		cmd=U_VERBOSE_ERREURS_ET_ACTIONS;
		break;
	case '4':
		cmd=U_VERBOSE_DETAILLEE;
		break;
	case '5':
		cmd=U_FIN_ARRU;
		break;
	default :
		break;
	}
}
//___________________________________________________________________________________
void ihmCmdSpeciales()
{
	char car;
	cout << "\033[2J\033[1;1H";  // <-- efface l'écran
	cout<<"  menu des commandes spéciales"<<endl;
	cout<<"  ----------------------------"<<endl;
	cout<<"\tOUVERTURE_PINCE       ..... '0'"<<endl;
	cout<<"\tFERMETURE_PINCE       ..... '1'"<<endl;
	cout<<"\tSEIS_ARRET_MESURES    ..... '2'"<<endl;
	cout<<"\tSEIS_LANCER_MESURES   ..... '3'"<<endl;
	cout<<"\tSEIS_REPLIER_PIEDS    ..... '4'"<<endl;
	cout<<"\tMETTRE_PANNEAUX_HS    ..... '5'"<<endl;

	cout<<endl<<"pour revenir au menu principal	: 'r'"<<endl;
	do
	{
		car=frpclav();
	}
	while(((car<'0')||(car>'5'))&&(car!='r') );
	switch(car)
	{
	case '0':
		cmd=S_OUVERTURE_PINCE;
		break;
	case '1':
		cmd=S_FERMETURE_PINCE;
		break;
	case '2':
		cmd=S_SEIS_ARRET_MESURES;
		break;
	case '3':
		cmd=S_SEIS_LANCER_MESURES;
		break;
	case '4':
		cmd=S_SEIS_REPLIER_PIEDS;
		break;
	case '5':
		cmd=S_METTRE_PANNEAUX_HS;
		break;
	default :
		break;
	};
}
//___________________________________________________________________________________
void ihmConfirmations()
{
	char car;
	cout << "\033[2J\033[1;1H";  // <-- efface l'écran
	cout<<"  menu des Confirmations"<<endl;
	cout<<"  ----------------------"<<endl;
	cout<<"\tconfirmation OUVERTURE_PINCE   ..... '0'"<<endl;
	cout<<"\tconfirmation POSE_HP3          ..... '1'"<<endl;
	cout<<"\tconfirmation LACHER_WTS        ..... '2'"<<endl;
	cout<<"\tconfirmation SAISIR_WTS        ..... '3'"<<endl;
	cout<<"\tconfirmation NIVELLEMENT SEIS  ..... '4'"<<endl;
	cout<<"\tconfirmation LACHER_SEIS       ..... '5'"<<endl;
	cout<<"\tconfirmation SAISIR_SEIS       ..... '6'"<<endl;
	cout<<"\tconfirmation LANCER_TESTS      ..... '7'"<<endl;
	cout<<"\tconfirmation RERPISE_PB_PAN1   ..... '8'"<<endl;
	cout<<"\tconfirmation RERPISE_PB_PAN2   ..... '9'"<<endl;
	cout<<"\tconfirmation REMONTER_HP3_POUR_ARRET   ..... 'a'"<<endl;
	cout<<"\tconfirmation REMONTER_WTS_POUR_ARRET   ..... 'b'"<<endl;
	cout<<"\tconfirmation REMONTER_SEIS_POUR_ARRET  ..... 'c'"<<endl;
	cout<<"\tconfirmation FERMER_PANNEAUX_POUR_ARRET..... 'd'"<<endl;

	cout<<endl<<"pour revenir au menu principal	: 'r'"<<endl;
	do
	{
		car=frpclav();
	}
	while( !( (car=='r')|| ((car>='0')&&(car<='9')) || ((car>='a')&&(car<='d')) ) );
	switch(car)
	{
	case '0':
		cmd=CONF_OUVERTURE_PINCE;
		break;
	case '1':
		cmd=CONF_POSE_HP3;
		break;
	case '2':
		cmd=CONF_LACHER_WTS;
		break;
	case '3':
		cmd=CONF_SAISIR_WTS;
		break;
	case '4':
		cmd=CONF_NIVELL_SEIS;
		break;
	case '5':
		cmd=CONF_LACHER_SEIS;
		break;
	case '6':
		cmd=CONF_SAISIR_SEIS;
		break;
	case '7':
		cmd=CONF_LANCER_TESTS;
		break;
	case '8':
		cmd=CONF_REPRISE_PB_PAN1;
		break;
	case '9':
		cmd=CONF_REPRISE_PB_PAN2;
		break;
	case 'a':
		cmd=CONF_REMONTER_HP3_POUR_ARRET;
		break;
	case 'b':
		cmd=CONF_REMONTER_WTS_POUR_ARRET;
		break;
	case 'c':
		cmd=REMONTER_SEIS_POUR_ARRET;
		break;
	case 'd':
		cmd=CONF_FERMER_PANNEAUX_POUR_ARRET;
		break;
	default :
		break;
	}
}
//___________________________________________________________________________________
void ihmDecisions()
{
	char car;
	cout << "\033[2J\033[1;1H";  // <-- efface l'écran
	cout<<"  menu des Décision"<<endl;
	cout<<"  -----------------"<<endl;
	cout<<"\tBATTERIE_FAIBLES_MAIS_CONTINUER    ..... '0'"<<endl;
	cout<<"\tBATTERIE_FAIBLES_DONC_ARRETER      ..... '1'"<<endl;
	cout<<"\tSORTIE_TESTS CONTINUER             ..... '2'"<<endl;
	cout<<"\tSORTIE_TESTS ARRETER               ..... '3'"<<endl;
	cout<<"\tSORTIE_INITIALISATION_ET_CONTINUER ..... '4'"<<endl;
	cout<<"\tSORTIE_INITIALISATION_ET_ARRETER   ..... '5'"<<endl;
	cout<<"\tDEFAUT_DETECTES_MAIS_CONTINER      ..... '6'"<<endl;
	cout<<"\tDEFAUT_DETECTES_DONC_ARRETER       ..... '7'"<<endl;

	cout<<endl<<"pour revenir au menu principal	: 'r'"<<endl;
	do
	{
		car=frpclav();
	}
	while( !( (car=='r')|| ((car>='0')&&(car<='7')) ) );
	switch(car)
	{
	case '0':
		cmd=DECISION_BATTERIE_FAIBLES_MAIS_CONTINUER;
		break;
	case '1':
		cmd=DECISION_BATTERIE_FAIBLES_DONC_ARRETER;
		break;
	case '2':
		cmd=DECISION_SORTIE_TESTS_EST_CONTINUER;
		break;
	case '3':
		cmd=DECISION_SORTIE_TESTS_EST_ARRETER;
		break;
	case '4':
		cmd=DECISION_SORTIE_INITIALISATION_EST_CONTINUER;
		break;
	case '5':
		cmd=DECISION_SORTIE_INITIALISATION_EST_ARRETER;
		break;
	case '6':
		cmd=DECISION_DEFAUT_DETECTES_MAIS_CONTINER;
		break;
	case '7':
		cmd=DECISION_DEFAUT_DETECTES_DONC_ARRETER;
		break;
	default :
		break;
	}

}
//___________________________________________________________________________________
void ihmHistorique()
{

}

