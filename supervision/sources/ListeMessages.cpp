#include "../defs/ListeMessages.h"
#include "../defs/Superviseur.h"
#include "../defs/commandes.h"

ListeMessages::ListeMessages ( )
{
	ptrSuperviseur=NULL;
	erreurCode=2;
	pthread_mutex_init(&semListeMsg, NULL);

}

ListeMessages::~ListeMessages ( )
{
}

//============================================================================
void ListeMessages::initListe(Superviseur* ptrsup)
{
	ptrSuperviseur=ptrsup;
}

//============================================================================
void ListeMessages::ajouterMessage (string message )
{
 	string *ptrmess=new string;
	*ptrmess=message;
	pthread_mutex_lock(&semListeMsg);
	listeMessages.push_back(ptrmess);
	pthread_mutex_unlock(&semListeMsg);
	pthread_cond_signal(&(ptrSuperviseur->nouveauMessage));
}

//============================================================================
bool ListeMessages::extraireMessage (string &message )
{
	string *ptrmess;
	if(ptrSuperviseur==NULL)
	{
		erreurCode=2;
		return false;
	}
	if (!listeMessages.empty())
	{
		pthread_mutex_lock(&semListeMsg);
		ptrmess= listeMessages.front();
		message=*ptrmess;
		listeMessages.erase(listeMessages.begin());
		delete ptrmess;
		pthread_mutex_unlock(&semListeMsg);
		return true;
	}
	return false;
}

//============================================================================

//-------FIN FICHIER------------------------------------------------------------

