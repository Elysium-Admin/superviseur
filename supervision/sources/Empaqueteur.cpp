#include "../defs/Empaqueteur.h"
#include <string.h>
#include <stdlib.h>

//********************************************************************************************************
//  Contrusteur
//
//********************************************************************************************************
Empaqueteur::Empaqueteur() : nbMesuresActuelle(0)
//-----------------------------------------------
{
	ptrListeMessages=NULL;
}
//=======================================================================================================


//********************************************************************************************************
// init : fonction d'initilisation
// =====
///	Elle initialise des attributs de type pointeurs, ou d'attributs ayant besoin d'initialisation
//	tardive (post-constructeur) come c'estle caspor els attribut initlaisés suite a la lecture du
//	fichier de configuration
//********************************************************************************************************
void Empaqueteur::init(Superviseur * prtsup, int nbmesures, ListeMessages* ptrlistemessages)
//------------------------------------------------------------------------------------------
{
	ptrSuperviseur=prtsup;
	NbMesureParPaquet=nbmesures;
	nbMesuresActuelle=0;
	ptrMessage=(char *)malloc((NbMesureParPaquet*LG_MESURE)+3);
	*ptrMessage='v';
	ptrCourant=(char *)(ptrMessage+1);
	ptrListeMessages=ptrlistemessages;
}

void Empaqueteur::init(Superviseur * prtsup)
//------------------------------------------------------------------------------------------
{
	ptrSuperviseur=prtsup;	
}
//=======================================================================================================


//********************************************************************************************************
// ajouter  : fonction d'ajout d'un mesure (3 axes au paquet de mesure en cours de construction
// =======
//	Cette fonction ajoute mesurs apres mesures jusqu'a ce que la paquet atteigne 'NbMesureParPaquet'
// 	Lorsque ce quota est atteint le paquet est alors envoyé comme message dans la liste des messages
//	à diffuser, puis un nouveau paquet se recontruit.
//
//********************************************************************************************************
bool Empaqueteur::ajouter(char* mesure)
//-------------------------------------
{
	if(ptrListeMessages==NULL)return false;
	strncpy(ptrCourant,mesure,LG_MESURE);
	nbMesuresActuelle++;
	ptrCourant=(char *)(ptrCourant+LG_MESURE);
	if(nbMesuresActuelle>=NbMesureParPaquet)
	{
		*(ptrCourant++)='\n';
		*ptrCourant=0x00;
		Trame=(string)ptrMessage;
		ptrListeMessages->ajouterMessage (Trame);
		ptrCourant=(char *)(ptrMessage+1);
		nbMesuresActuelle=0;
	}
	return true;
}
//=======================================================================================================


