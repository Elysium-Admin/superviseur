#include <pthread.h>
#include <sys/types.h>
#include <iostream>
#include <signal.h>

#include "../defs/Superviseur.h"

Superviseur superviseur;

void interruptionCallback(int sigid)
{
    cout << endl;
    superviseur.fermetureClients();
    exit(0);
}

void *demonDiffusion(void*)
{
	superviseur.inscriptionClients();
}

void *demonCmds(void*)
{
	superviseur.receptionCmds();
}

void *receptionSeis(void*)
{
	superviseur.leSeis.ReceptionEnvoiSEIS ();
}

void *interpreteurComandesSpeciales(void*)
{
	superviseur.interpreterComandesSpeciales();
}

void *interpreteurComandesNormales(void*)
{
	superviseur.interpreterComandesNormales();
}

void *diffusion(void*)
{
	superviseur.diffuser();
}

void *supervision(void*)
{
	superviseur.kernel();
}

void *surveillantEnergie(void*)
{
	superviseur.surveillanceEnergie();
}

void *surveillantARRU(void*)
{
	superviseur.surveillanceARRU();
}

void *surveillantCamera(void*)
{
	superviseur.surveillanceCamera();
}

void *construteurDEtat(void*)
{
	superviseur.ConstruireMessageDEtat();
}


int main()
{
    signal(SIGINT, interruptionCallback);

	void *retour; /* utilisé pour la terminaison des thread */
	pthread_t demonClients;
	pthread_t demonCommandes;
	pthread_t reponsesSeis;
	pthread_t commandesNormales;
	pthread_t commandesSpeciales;
	pthread_t messageDiffusion;
	pthread_t SurveilleErnegie;
	pthread_t SurveilleARRU;
	pthread_t SurveilleCamera;
	pthread_t Surpervision;
//************debut Ajout  18 Mars 2015*********
	pthread_t FaireMessageDEtat;
//************fin Ajout  18 Mars 2015*********

	if (pthread_create (&Surpervision, NULL, supervision, NULL) < 0)
	{
		cerr<<"ERR prgDeSupervision Impossible de créer le thread 'supervision'"<<endl;
		exit (1);
	};
	if (pthread_create (&commandesSpeciales, NULL, interpreteurComandesSpeciales, NULL) < 0)
	{
		cerr<<"ERR prgDeSupervision Impossible de créer le thread 'commandesSpeciales'"<<endl;
		exit (1);
	};
	if (pthread_create (&SurveilleErnegie, NULL,surveillantEnergie, NULL) < 0)
	{
		cerr<<"ERR prgDeSupervision Impossible de créer le thread 'SurveilleErnegie'"<<endl;
		exit (1);
	};
	if (pthread_create (&SurveilleARRU, NULL,surveillantARRU, NULL) < 0)
	{
		cerr<<"ERR prgDeSupervision Impossible de créer le thread 'SurveilleARRU'"<<endl;
		exit (1);
	};
	if (pthread_create (&SurveilleCamera, NULL, surveillantCamera, NULL) < 0)
	{
		cerr<<"ERR prgDeSupervision Impossible de créer le thread 'SurveilleCamera'"<<endl;
		exit (1);
	};
	if (pthread_create (&commandesNormales, NULL, interpreteurComandesNormales, NULL) < 0)
	{
		cerr<<"ERR prgDeSupervision Impossible de créer le thread 'commandesNormales'"<<endl;
		exit (1);
	};
	if (pthread_create (&reponsesSeis, NULL, receptionSeis, NULL) < 0)
	{
		cerr<<"ERR prgDeSupervision Impossible de créer le thread 'reponsesSeis'"<<endl;
		exit (1);
	};
	if (pthread_create (&demonCommandes, NULL, demonCmds, NULL) < 0)
	{
		cerr<<"ERR prgDeSupervision Impossible de créer le thread 'demonCommandes'"<<endl;
		exit (1);
	};
	if (pthread_create (&demonClients, NULL,demonDiffusion, NULL) < 0)
	{
		cerr<<"ERR prgDeSupervision Impossible de créer le thread 'demonClients'"<<endl;
		exit (1);
	};
	if (pthread_create (&messageDiffusion, NULL,diffusion, NULL) < 0)
	{
		cerr<<"ERR prgDeSupervision Impossible de créer le thread 'diffusion'"<<endl;
		exit (1);
	};
//************debut Ajout 18 Mars 2015*********
	if (pthread_create (&FaireMessageDEtat, NULL,construteurDEtat, NULL) < 0)
	{
		cerr<<"ERR prgDeSupervision Impossible de créer le thread 'FaireMessageDEtat'"<<endl;
		exit (1);
	};
//************fin Ajout  18 Mars 2015*********

	(void)pthread_join (Surpervision, &retour);
	(void)pthread_join (commandesSpeciales, &retour);
	(void)pthread_join (commandesNormales, &retour);
	(void)pthread_join (reponsesSeis, &retour);
	(void)pthread_join (demonCommandes, &retour);
	(void)pthread_join (demonClients, &retour);
	(void)pthread_join (messageDiffusion, &retour);
	(void)pthread_join (SurveilleErnegie, &retour);
	(void)pthread_join (SurveilleARRU, &retour);
	pthread_join(SurveilleCamera, &retour);
//************debut Ajout 18 Mars 2015*********
	(void)pthread_join (FaireMessageDEtat, &retour);
//************fin Ajout  18 Mars 2015*********

	return 0;
}

