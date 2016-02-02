//Date derniere modifs 31/01/2015
//

#include "../defs/commandes.h"
#include "../defs/Etats.h"
#include "../defs/Superviseur.h"
#include <unistd.h>


//********************************************
//**************	ICS	**************
//********************************************
//=================================================================================================================================
ICN::ICN()
{
}

//=================================================================================================================================
void ICN::init(Superviseur * ptrsuperviseur)
//******************************************
{
	ptrSuperviseur=ptrsuperviseur;
}

//=================================================================================================================================
bool ICN::initChienDeGarde()
//**************************
{
	if(gettimeofday(&depart, NULL)==-1){
		return false;
	};
	return true;
}

//=================================================================================================================================
bool ICN::ChienDeGardeDepasse(int duree)
//**************************************
{
	gettimeofday(&actuel, NULL);
	int ecart_s;
	ecart_s = actuel.tv_sec - depart.tv_sec;
	if(ecart_s > duree){
		return true;
	};
	return false;
}


//=================================================================================================================================
bool ICN::ouvrirPanneaux()
//************************
{
    uint16_t val;
    unsigned char Mode;
    int codeException;

    try
    {
	if ( !initChienDeGarde()){
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::OUVERTURE PANNEAUX"," Echec d'initialisation du chien de garde");
		throw codeException=TIME_OUT_PANNEAUX;
	};
	if( !ptrSuperviseur->automatePanneaux.lireEtatPanneaux(val) ) 
	{
		throw codeException=RUPTURE_CONNEXION_PANNEAUX;
	};
	if(val==Etats::PANNEAUX_FERMES)
	{
            ptrSuperviseur->automatePanneaux.ecrireCmdPanneaux(AutomatePanneaux::OUVRIR_PANNEAUX);
	    ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::OUVRIR_PANNEAUX","Ouverture panneaux lancée");
            do
            {
		if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL) || (ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
			throw codeException=SIGNAL_ARRU;
		};
		if(ChienDeGardeDepasse(CHIEN_DE_GARDE_MOUVEMENT_PANNEAUX)){
			throw codeException=TIME_OUT_PANNEAUX;
		};
       		if(!ptrSuperviseur->automatePanneaux.lireEtatPanneaux(val))
      		{
			throw codeException=RUPTURE_CONNEXION_PANNEAUX;
		}; // rupture de flux
		sleep(1);
		Mode=ptrSuperviseur->etats.getMode();
	    }
            while((val==Etats::PANNEAUX_FERMES)&&(val!=Etats::INCIDENT_REDHIBITOIRE_PANNEAUX ) && !(Mode & Etats::M_PANNEAUX_HS ));

            ptrSuperviseur->v.verbMsg(Verbose::DBG,"ICN::OUVRIR_PANNEAUX"," Ouverture lancée on efface la commande");
            ptrSuperviseur->automatePanneaux.ecrireCmdPanneaux(AutomatePanneaux::RIEN );
	    ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::OUVRIR_PANNEAUX"," On attend la fin de la commande d'ouverture");
	    if(val==Etats::INCIDENT_REDHIBITOIRE_PANNEAUX )
            {
		throw codeException=PANNEAUX_HS;
	    };
            while((val!=Etats::PANNEAUX_OUVERTS) &&(val!=Etats::INCIDENT_REDHIBITOIRE_PANNEAUX) && !(Mode & Etats::M_PANNEAUX_HS ))
            {
		if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL) || (ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
			throw codeException=SIGNAL_ARRU;
		};
		if(ChienDeGardeDepasse(CHIEN_DE_GARDE_MOUVEMENT_PANNEAUX)){
			throw codeException=TIME_OUT_PANNEAUX;
		};
		if(!ptrSuperviseur->automatePanneaux.lireEtatPanneaux(val))
	        {
			throw codeException=RUPTURE_CONNEXION_PANNEAUX;
		};
		if(val==Etats::INCIDENT_OUVERTURE_PANNEAU_1)
		{
		    ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::OUVRIR_PANNEAUX","Incident lors de l'ouverture panneau 1");

		    // ---------------------------------------------
		    // gestion des incident d'ouverture du panneau1
		    // ---------------------------------------------
                    do 
                	{
			ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::OUVRIR_PANNEAUX","Demande IHM de confirmation reprise problème panneaux 1");
			ptrSuperviseur->DemandeIHM(Etats::DEM_CONF_FERMETURE_SUR_PB_OUVERT_PAN_1);// demande bloquante
                    }
		    while(ptrSuperviseur->reponseIHM!=CONF_REPRISE_PB_PAN1);

                    ptrSuperviseur->automatePanneaux.ecrireCmdPanneaux(AutomatePanneaux::REPLIER_PANNEAU_EN_PANNE);
		    ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::OUVRIR_PANNEAUX","On replit le panneau en panne");
		    ptrSuperviseur->etats.SetflagIncidentPanneaux(Etats::PANNEAU_1_HORS_SERVICE);
		}
	 	else if(val== Etats::INCIDENT_OUVERTURE_PANNEAU_2)
            	{
		    ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::OUVRIR_PANNEAUX","Incident lors de l'ouverture panneau 2");

		    // ---------------------------------------------
		    // gestion des incident d'ouverture du panneau2
		    // ---------------------------------------------
                   do
                    {
			ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::OUVRIR_PANNEAUX","Demande IHM de confirmation reprise problème panneaux 2");
              		ptrSuperviseur->DemandeIHM(Etats::DEM_CONF_FERMETURE_SUR_PB_OUVERT_PAN_2);// demande bloquante
                    }
		    while (ptrSuperviseur->reponseIHM!=CONF_REPRISE_PB_PAN2);

                    ptrSuperviseur->automatePanneaux.ecrireCmdPanneaux(AutomatePanneaux::REPLIER_PANNEAU_EN_PANNE);
		    ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::OUVRIR_PANNEAUX","On replit le panneau en panne");
		    ptrSuperviseur->etats.SetflagIncidentPanneaux(Etats::PANNEAU_2_HORS_SERVICE);
		}
		sleep(1);
		Mode=ptrSuperviseur->etats.getMode();
	    };
	};
	ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::OUVRIR_PANNEAUX","Panneaux ouverts");
    }
    catch(int e)
    {
		//traitement des exceptions
		switch (e)
		{
		case  TIME_OUT_PANNEAUX :
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::OUVERTURE PANNEAUX"," Time out de la sequence d'ouverture des panneaux");
			ptrSuperviseur->etats.SetFlagMode(Etats::M_PANNEAUX_HS);
			return false;
			break;
		case  PANNEAUX_HS :
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::OUVERTURE PANNEAUX"," defaut redhibitoire detecté dans les paneaux");
			return false;
			break;
		case  RUPTURE_CONNEXION_PANNEAUX :
			ptrSuperviseur->etats.SetflagCompteRenduSysteme(Etats::RUPTURE_LIAISON_PANNEAUX);
			ptrSuperviseur->etats.SetFlagMode(Etats::M_PANNEAUX_HS);
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::OUVERTURE PANNEAUX","Rupture de flux avec l'automate des panneaux");
			return false;
			break;
		case  SIGNAL_ARRU :
			ptrSuperviseur->etats.SetflagCompteRenduSysteme(Etats::COMMANDE_INCONNUE);
			ptrSuperviseur->etats.SetFlagMode(Etats::M_PANNEAUX_HS);
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::OUVERTURE PANNEAUX"," demande d'ARRU dectectée");
			return false;
			break;
		default :
			break;
		}
    }
    return true;
}

//=================================================================================================================================
bool ICN::fermerPanneaux()
//***********************
{
    uint16_t val;
    unsigned char Mode;
    int codeException;

   try
    {
	if ( !initChienDeGarde()){
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::OUVERTURE PANNEAUX"," Echec d'initialisation du chien de garde");
		throw codeException=TIME_OUT_PANNEAUX;
	};
	if( !ptrSuperviseur->automatePanneaux.lireEtatPanneaux(val) ) 
	{
		throw codeException=RUPTURE_CONNEXION_PANNEAUX;
	};
	if(val==Etats::PANNEAUX_OUVERTS)
	{
	    ptrSuperviseur->automatePanneaux.ecrireCmdPanneaux(AutomatePanneaux::FERMER_PANNEAUX);
	    ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::FERMER_PANNEAUX"," Fermeture panneaux lancée");
	    do
	    {
	    	if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
			throw codeException=SIGNAL_ARRU;
		};
		if(ChienDeGardeDepasse(CHIEN_DE_GARDE_MOUVEMENT_PANNEAUX)){
			throw codeException=TIME_OUT_PANNEAUX;
		};
       		if(!ptrSuperviseur->automatePanneaux.lireEtatPanneaux(val))
      		{
			throw codeException=RUPTURE_CONNEXION_PANNEAUX;
		}; // rupture de flux
		sleep(1);
		Mode=ptrSuperviseur->etats.getMode();
	    }
	    while( (val == Etats::PANNEAUX_OUVERTS) && (val != Etats::INCIDENT_REDHIBITOIRE_PANNEAUX ) && !(Mode & Etats::M_PANNEAUX_HS ) );

	    ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::FERMER_PANNEAUX","Fermeture lancée on efface la commande");
	    ptrSuperviseur->automatePanneaux.ecrireCmdPanneaux(AutomatePanneaux::RIEN );
	    ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::FERMER_PANNEAUX"," Attente de la fin de la commande de fermeture");
	    if(val==Etats::INCIDENT_REDHIBITOIRE_PANNEAUX )
	    if(val==Etats::INCIDENT_REDHIBITOIRE_PANNEAUX )
            {
		throw codeException=PANNEAUX_HS;
	    };
            while((val!=Etats::PANNEAUX_FERMES) &&(val!=Etats::INCIDENT_REDHIBITOIRE_PANNEAUX) && !(Mode & Etats::M_PANNEAUX_HS ))
            {
		if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL) || (ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
			throw codeException=SIGNAL_ARRU;
		};
		if(ChienDeGardeDepasse(CHIEN_DE_GARDE_MOUVEMENT_PANNEAUX)){
			throw codeException=TIME_OUT_PANNEAUX;
		};
		if(!ptrSuperviseur->automatePanneaux.lireEtatPanneaux(val))
	        {
			throw codeException=RUPTURE_CONNEXION_PANNEAUX;
		};
		if(val==Etats::INCIDENT_OUVERTURE_PANNEAU_1)
		{
		    ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::FERMER_PANNEAUX","Incident lors de la fermeture panneau 1");

		// ------------------------------------------------
		// gestion les incidents de fermeture du panneaux 1
		// ------------------------------------------------
                do
                {
		    ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::FERMER_PANNEAUX","Demande IHM de confirmation reprise problème panneaux");
                    ptrSuperviseur->DemandeIHM(Etats::DEM_CONF_ROUVERTURE_SUR_PB_FERMET_PAN_1);// demande bloquante
                }
		while (ptrSuperviseur->reponseIHM!=CONF_REPRISE_PB_PAN1);

                ptrSuperviseur->automatePanneaux.ecrireCmdPanneaux(AutomatePanneaux::REOUVRIR_PANNEAU_EN_PANNE);
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::FERMER_PANNEAUX","On reouvre le panneau en panne");

		    ptrSuperviseur->etats.SetflagIncidentPanneaux(Etats::PANNEAU_1_HORS_SERVICE);
		}
	 	else if(val== Etats::INCIDENT_OUVERTURE_PANNEAU_2)
            	{
		    ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::FERMER_PANNEAUX","Incident lors de la fermeture panneau 2");

		// ------------------------------------------------
		// gestion les incidents de fermeture du panneaux 2
		// ------------------------------------------------
                do
                {
		    ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::FERMER_PANNEAUX","Demande IHM de confirmation reprise problème panneaux");
                    ptrSuperviseur->DemandeIHM(Etats::DEM_CONF_ROUVERTURE_SUR_PB_FERMET_PAN_2);// demande bloquante
                }
		while (ptrSuperviseur->reponseIHM!=CONF_REPRISE_PB_PAN2);
                ptrSuperviseur->automatePanneaux.ecrireCmdPanneaux(AutomatePanneaux::REOUVRIR_PANNEAU_EN_PANNE);
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::FERMER_PANNEAUX","On reouvre le panneau en panne");

		    //nouvel algo
		    ptrSuperviseur->etats.SetflagIncidentPanneaux(Etats::PANNEAU_2_HORS_SERVICE);
		}
		sleep(1);
		Mode=ptrSuperviseur->etats.getMode();
	    };
        };
        ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::FERMER_PANNEAUX","Panneaux fermés");
    }
    catch(int e)
    {
		//traitement des exceptions
		switch (e)
		{
		case  TIME_OUT_PANNEAUX :
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::FERMETURE PANNEAUX"," Time out de la sequence dd fermeture des panneaux");
			ptrSuperviseur->etats.SetFlagMode(Etats::M_PANNEAUX_HS);
			return false;
			break;
		case  PANNEAUX_HS :
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::FERMETURE PANNEAUX"," defaut redhibitoire detecté dans les paneaux");
			return false;
			break;
		case  RUPTURE_CONNEXION_PANNEAUX :
			ptrSuperviseur->etats.SetflagCompteRenduSysteme(Etats::RUPTURE_LIAISON_PANNEAUX);
			ptrSuperviseur->etats.SetFlagMode(Etats::M_PANNEAUX_HS);
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::FERMETURE PANNEAUX","Rupture de flux avec l'automate des panneaux");
			return false;
			break;
		case  SIGNAL_ARRU :
			ptrSuperviseur->etats.SetflagCompteRenduSysteme(Etats::COMMANDE_INCONNUE);
			ptrSuperviseur->etats.SetFlagMode(Etats::M_PANNEAUX_HS);
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::FERMETURE PANNEAUX"," demande d'ARRU dectectée");
			return false;
			break;
		default :
			break;
		}
    }
    return true;
}



//=================================================================================================================================
bool ICN::deposerSEIS()
//*********************
{
	uint16_t etatBras;
	uint16_t etatBrasPrecedant=Etats::INCONNUE;
	uint16_t etatPince;
	uint16_t etatPincePrecedant;
	string reponseSeis="";
	bool historique;
	int exception;
	int codeException;
	ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS","Début de la dépose du seis");
	try
	{
                if(! ptrSuperviseur->automateBras.ecrireCmdActionBras(AutomateBras::AUCUNE))// rupture de flux bras
                {
                        throw codeException=RUPTURE_BRAS_AVEC_OBJET;
                };
		while(ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL) ||
									(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			}
			if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
			sleep(1);
		};
		// on s'assure que la pince soit bien ouverte
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS"," vérifie si la pince est ouverte");
		if( !ptrSuperviseur->automateBras.lireEtatPince(etatPincePrecedant) )  // rupture de flux bras
		{
			throw codeException=RUPTURE_BRAS_SEUL;
		};
		// pour le cas (anormale) ou la pince serait fermée
		if(etatPincePrecedant==Etats::PINCE_FERMEE)
		{
			ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS"," la pince étant fermée nous la faisons s'ouvrir");
			if( !ptrSuperviseur->automateBras.actionPince(AutomateBras::OUVRIR_PINCE) )  // rupture de flux bras
			{
				throw codeException=RUPTURE_BRAS_AVEC_OBJET;
			};
			do
			{
				if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL) ||
									(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
					throw codeException=SIGNAL_ARRU;
				}
				if( !ptrSuperviseur->automateBras.lireEtatPince(etatPince) )  // rupture de flux bras
				{
					throw codeException=RUPTURE_BRAS_AVEC_OBJET;
				};
				if( (etatPincePrecedant==Etats::PINCE_FERMEE) && (etatPince!=Etats::PINCE_FERMEE) ){
					ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS",
							" la pince commence son ouverture on efface la commande d'ouverture");
					if (!ptrSuperviseur->automateBras.actionPince(AutomateBras::PAS_NOUVELLE_ACTION)){
						throw codeException=RUPTURE_BRAS_AVEC_OBJET;
					}
				}
				etatPincePrecedant=etatPince;
				if( etatPince != Etats::PINCE_OUVERTE )sleep(1);
			}
			while( etatPince != Etats::PINCE_OUVERTE );
		}
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS","Pince ouverte, lancement de la séquence");
		// ça y est on lance la séquence
		if( !ptrSuperviseur->automateBras.lancerSequence(AutomateBras::DEPOSE_SEIS) )  // rupture de flux bras
		{
			throw codeException=RUPTURE_BRAS_SEUL;
		};
		//on attend quel'automate ait bien pris a commande en compte
		ptrSuperviseur->v.verbMsg(Verbose::DBG,"ICN::::DEPOSER_SEIS","On commance la séquence");
		while(! ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			}
			if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
			sleep(1);
		};
		if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS","La séquence est en cours on efface la commande ");
		// la commande ayant été prise en compte on l'efface pour que l'automate ne
		// cherche pas à l'exécuter une nouvelle fois apres la fin de cette séquence
		if( !ptrSuperviseur->automateBras.lancerSequence(AutomateBras::RIEN) )  // rupture de flux bras
		{
			throw codeException=RUPTURE_BRAS_SEUL;
		};
		// maintenant on traite le suivi de la séquence
		ptrSuperviseur->v.verbMsg(Verbose::DBG,"ICN::::DEPOSER_SEIS","On traite le suivi de la séquence");
		while(ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			}
			sleep(1); // pour ne pas consommer trop de cpu
			//lecture de l'etat du bras pour le traitemnt en boucle
			if(!ptrSuperviseur->automateBras.lireEtatBras(etatBras) )  // rupture de flux bras
			{
				throw codeException=RUPTURE_BRAS_AVEC_OBJET;
			};
			//lecture de l'etat de la pince pour le traitemnt en boucle
			if(!ptrSuperviseur->automateBras.lireEtatPince(etatPince) )  // rupture de flux bras
			{
				throw codeException=RUPTURE_BRAS_AVEC_OBJET;
			};
			// si le seis vient d'etre saisie on indique a l'ihm qu'il est en cours de dépose
			if( (etatPincePrecedant==Etats::PINCE_OUVERTE) && (etatPince==Etats::PINCE_FERMEE) )
			{
				etatPincePrecedant=Etats::PINCE_FERMEE;
				ptrSuperviseur->etats.setEtatSEIS(Etats::DEPOSE_EN_COURS);
			};
			// si le seis arrive  hors plateau on va lancer la detection du sol
			if( (etatBras&Etats::SEIS_HORS_PLATEAU) && !(etatBrasPrecedant&Etats::SEIS_HORS_PLATEAU) )
			{
				etatBrasPrecedant=Etats::SEIS_HORS_PLATEAU;
				ptrSuperviseur->v.verbMsg(Verbose::DBG,"ICN::::DEPOSER_SEIS","Le seis arrive hors plateau on va lancer la détection du sol");
				if( !(ptrSuperviseur->leSeis.detecterSol(reponseSeis)) )  // rupture de flux SEIS
				{
					ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::::DEPOSER_SEIS","Erreur détection sol");
					throw codeException=RUPTURE_BRAS_AVEC_OBJET;
				};
			};
			// le seis vient de détecter le sol
			if(reponseSeis=="$!fapp;")
			{
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS","Fin approche '$!fapp;' reçue");
				reponseSeis="";// on efface la réponse pour ne pas la retraiter dans la foulée
				// on arrete le bras
				ptrSuperviseur->etats.setEtatSEIS(Etats::NIVELLEMENT); // indique a l'ihm qu'on est en phase de nivellement
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS"," avant delancer le nivelement on arrete le bras");
				if( !ptrSuperviseur->automateBras.ecrireCmdActionBras(AutomateBras::ARRET_BRAS) )  // rupture de flux bras
				{
					throw codeException=RUPTURE_BRAS_AVEC_OBJET;
				};
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS",
									" Le bras est arrêté, on va demander confirmation au conférencier");
				//demande au conferencier de verifier visuellement la bonne position du seis avant de lancer du nivellement
				while(ptrSuperviseur->reponseIHM!=CONF_NIVELL_SEIS)
				{
					if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL) ||
										(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
						throw codeException=SIGNAL_ARRU;
					};
					ptrSuperviseur->DemandeIHM(Etats::DEM_CONF_SEIS_COMMENCER_NIVELLEMENT); // demande bloquante
				};
				// le conferecier a confirmé la bonne position du seis , on peut lancer le nivellement
				// le nivellement est une procedure assynchrone avec varaible de reponse
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS",
									" confirmation du conférencier , on lance le nivellement pour le seis");

				if( !(ptrSuperviseur->leSeis.nivellement(reponseSeis)) )  // rupture de flux du seis
				{
					ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::::DEPOSER_SEIS"," Erreur durant le nivellement");
					throw codeException=RUPTURE_BRAS_AVEC_OBJET;
				};
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS"," Déclenchement du chien de garde de l'automate bras");
				if(! ptrSuperviseur->automateBras.ecrireCmdActionBras(AutomateBras::LANCER_CHIEN_DE_GARDE_NIVELLEMENT) )// rupture de flux bras
				{
					throw codeException=RUPTURE_BRAS_AVEC_OBJET;
				};
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS",
											"on attend la fin du nivellement ou le chien de garde du bras");
			};
			if(reponseSeis=="$!fniv0;")
			{
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS"," fin nivellement '$!fniv0;' reçu");
				reponseSeis=""; // on efface la réponse pour ne pas la retraiter dans la foulée
				if(! ptrSuperviseur->automateBras.ecrireCmdActionBras(AutomateBras::ARRET_CHIEN_DE_GARDE_NIVELLEMENT))// rupture de flux bras
				{
					throw codeException=RUPTURE_BRAS_AVEC_OBJET;
				};
				ptrSuperviseur->etats.setEtatSEIS(Etats::FIN_NIVELL);

				// on demande au conférencier de confirmer visuellement qu'il n'y a pas de probleme pour lacher le seis
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS"," demande ouverture pince");
				while(ptrSuperviseur->reponseIHM!=CONF_LACHER_SEIS)
				{
					if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL) ||
										(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
						throw codeException=SIGNAL_ARRU;
					};
					ptrSuperviseur->DemandeIHM(Etats::DEM_CONF_LACHER_SEIS); // demande bloquante
				}
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS",
										" demande ouverture pince confirmée par le conferncier");
				// le conférencier a confirmé on peut alors ouvrir la pince
				if(!ptrSuperviseur->automateBras.actionPince(AutomateBras::OUVRIR_PINCE) )  // rupture de flux bras
				{
					throw codeException=RUPTURE_BRAS_AVEC_OBJET;
				};
				// boucle d'attente que la pince soit ouverte
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS"," pince en cours d'ouverture");
				historique=false;
				do
				{
					if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL) ||
										(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
						throw codeException=SIGNAL_ARRU;
					};
					if(!ptrSuperviseur->automateBras.lireEtatPince(etatPince) )  // rupture de flux bras
					{
						throw codeException=RUPTURE_BRAS_AVEC_OBJET;
					};
					if( (!historique) && (etatPince!=Etats::PINCE_FERMEE) )
					{
						historique=true;
						// on  remet la commande a pas d'action , ca occupe le temps que la pince s'ouvre
						if (!ptrSuperviseur->automateBras.actionPince(AutomateBras::PAS_NOUVELLE_ACTION)){
							throw codeException=RUPTURE_BRAS_AVEC_OBJET;
						}
					};
					//et on s'endort 1 seconde pour que l'attente ne consomme pas trops de CPU
					sleep(1);
				}
				while( etatPince!=Etats::PINCE_OUVERTE);
				// maintenant on peut declarer que le seis est au sol
				ptrSuperviseur->etats.setEtatSEIS(Etats::AU_SOL);
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS"," pince ouverte, on met le SEIS en mode 'mesure'");
				// et donc le mettre en mode mesure de vibrations
				ptrSuperviseur->leSeis.lancerMesureVibrations();
				ptrSuperviseur->etats.SetFlagEtatSEIS(Etats::MESURES_EN_COURS);
				// l'ouverture de la pince lance automatiquement, sur l'automate, le replie du bras donc pas besoin de le commander
			};
			if(reponseSeis=="$!fniv1;")  // incident de nivellement
			{
				reponseSeis="";
				ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::::DEPOSER_SEIS",
										"$!fniv1; reçu => probleme de nivellement détecté par le SEIS");
				ptrSuperviseur->etats.SetFlagMode(Etats::M_SEIS_HS);
				ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
				return false;  // car il n'y a pas de sequence degradée prevue sur l'automate
			};
			if(reponseSeis=="$QUOI?")  // incident de nivellement
			{
				reponseSeis="";
				ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::::DEPOSER_SEIS", "$QUOI?; reçu => le seis n'a pas compris la requête");
//				ptrSuperviseur->etats.SetFlagMode(Etats::M_SEIS_HS);
//				ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
//				return false;  // car il n'y a pas de sequence degradée prevue sur l'automate
			};
			// on veille quand même à ce quele bras retourne sans probleme dans la position canonique
			if(etatBras==Etats::INCIDENT_REDHIBITOIRE_BRAS)
			{
				stringstream ss;
				ss <<"défault bras = 0x"<< hex << setfill('0')<<setw(8)<<ptrSuperviseur->automateBras.getDefauts(); 
				ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::::DEPOSER_SEIS",ss.str());
				if( etatPincePrecedant!=Etats::PINCE_OUVERTE)ptrSuperviseur->etats.setEtatSEIS(Etats::INDISPONIBLE);
				ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);// car il n'y a pas de sequence degradée prevue surl'automate
				return false;  // car il n'y a pas de sequence degradée prevue sur l'automate
			};
//			sleep(1); // Repos
			usleep(200000); // Repos
		}
		if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS", " on attend que le bras soit retourné en positon panoramique");
		while(etatBras != Etats::POSITION_PANORAMIQUE)
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			};
			sleep(1);
			if( !ptrSuperviseur->automateBras.lireEtatBras(etatBras) )  // rupture de flux bras
			{
				throw codeException=RUPTURE_BRAS_SEUL;
			};
		};
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSER_SEIS", " Fin de sequence");
	}
	catch(int e)
	{
		//traitement des exceptions
		switch (e)
		{
		case  RUPTURE_BRAS_AVEC_OBJET :
			ptrSuperviseur->etats.setEtatSEIS(Etats::INDISPONIBLE); // pas de break car c'est un ajout a ce qui suit
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::::DEPOSER_SEIS","Rupture de flux bras avec objet");
		case  RUPTURE_BRAS_SEUL :
			ptrSuperviseur->etats.SetflagCompteRenduSysteme(Etats::RUPTURE_LIAISON_BRAS);
			ptrSuperviseur->etats.setEtatBras(Etats::INCIDENT_REDHIBITOIRE_BRAS);
			ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::::DEPOSER_SEIS","Rupture de flux avec l'automate du bras");

			return false;
			break;
		case  SIGNAL_ARRU :
			ptrSuperviseur->etats.SetflagCompteRenduSysteme(Etats::COMMANDE_INCONNUE);
			ptrSuperviseur->etats.setEtatBras(Etats::INCONNUE);
			ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::DEPOSER_SEIS"," demande d'ARRU dectectée");
			return false;
			break;
		default :
			break;
		}
	}
	return true;
}

//=================================================================================================================================
bool ICN::remonterSEIS()
//**********************
{
	uint16_t etatBras;
	uint16_t etatPince;
	uint16_t etatPincePrecedant;
	int exception;
	int codeException;
	bool histopince;
	ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::REPRISE_SEIS","Début de reprise du seis");
	try
	{
                if(! ptrSuperviseur->automateBras.ecrireCmdActionBras(AutomateBras::AUCUNE))// rupture de flux bras
                {
                        throw codeException=RUPTURE_BRAS_AVEC_OBJET;
                };
 		while(ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			}
			if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
			sleep(1);
		}
		if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
		// on s'assure que la pince soit bien ouverte
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::REPRISE_SEIS"," vérifie si la pince est ouverte");
		if( !ptrSuperviseur->automateBras.lireEtatPince(etatPincePrecedant) )  // rupture de flux bras
		{
			throw codeException=RUPTURE_BRAS_SEUL;
		};
		// pour le cas (anormale) ou la pince serait fermée
		if(etatPincePrecedant==Etats::PINCE_FERMEE)
		{
			ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::REPRISE_SEIS","  la pince étant fermée on commande son ouverture");
			if( !ptrSuperviseur->automateBras.actionPince(AutomateBras::OUVRIR_PINCE) )  // rupture de flux bras
			{
				throw codeException=RUPTURE_BRAS_AVEC_OBJET;
			};
			do
			{
				if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL) ||
										(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
					throw codeException=SIGNAL_ARRU;
				}
				if( !ptrSuperviseur->automateBras.lireEtatPince(etatPince) )  // rupture de flux bras
				{
					throw codeException=RUPTURE_BRAS_AVEC_OBJET;
				};
				if( (etatPincePrecedant==Etats::PINCE_FERMEE) && (etatPince!=Etats::PINCE_FERMEE) ){
					ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::REPRISE_SEIS",
									" la pince commence son ouverture on efface la commande d'ouverture");
					if (!ptrSuperviseur->automateBras.actionPince(AutomateBras::PAS_NOUVELLE_ACTION)){
						throw codeException=RUPTURE_BRAS_AVEC_OBJET;
					}
				}
				etatPincePrecedant=etatPince;
				if( etatPince != Etats::PINCE_OUVERTE )sleep(1);
			}
			while( etatPince != Etats::PINCE_OUVERTE );
		}
		// ça y est la pince est ouverte, on lance la séquence
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::REPRISE_SEIS","  début de la sequence de recupération du seis");
		if( !ptrSuperviseur->automateBras.lancerSequence(AutomateBras::REPRISE_SEIS) )  // rupture de flux bras
		{
			throw codeException=RUPTURE_BRAS_SEUL;
		};
		//on attend que l'automate ait bien pris la commande en compte
		while(! ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			}
			if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
			sleep(1);
		}
		// la commande ayant été prise en compte on l'efface pour quel'automate ne
		// cherche pas à l'exécuter une nouvelle fois apres la fin de cette séquence
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::REPRISE_SEIS"," la sequence a débutée on efface la commande");
		if( !ptrSuperviseur->automateBras.lancerSequence(AutomateBras::RIEN) )  // rupture de flux bras
		{
			throw codeException=RUPTURE_BRAS_SEUL;
		};
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::REPRISE_SEIS"," debut de boucle de suivi de la commande");
		// maintenant on traite le suivi de la séquence de reprise du seis
		while(ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			}
			sleep(1); // pour ne pas consommer trop de cpu
			//lecture de l'etat du bras pour le traitemnt en boucle
			if( !ptrSuperviseur->automateBras.lireEtatBras(etatBras) )  // rupture de flux bras
			{
				throw codeException=RUPTURE_BRAS_SEUL;
			};
			//lecture de l'etat de la pince pour le traitemnt en boucle
			if( !ptrSuperviseur->automateBras.lireEtatPince(etatPince) )  // rupture de flux bras
			{
				throw codeException=RUPTURE_BRAS_SEUL;
			};
			// le bras est pret à saisir le seis au sol
			if(etatBras==Etats::PRET_A_SAISIR_SEIS)
			{
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::REPRISE_SEIS"," demande au conferencier la confirmation pour saisir le SEIS");
				// on demande  au conférencier de verifier visuellement que c'est bon avant de lancer la saisie du seis
				while(ptrSuperviseur->reponseIHM!=CONF_SAISIR_SEIS)
				{
					if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
						throw codeException=SIGNAL_ARRU;
					};
					ptrSuperviseur->DemandeIHM(Etats::DEM_CONF_SAISIR_SEIS); // demande bloquante
				}
				// le conferencier a confirmé qu'on peut saisir le seis
				// on comande l'arret des mesures de vibration
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::REPRISE_SEIS","  confirmation faite on commence par arreter le mode mesure du SEIS");
				ptrSuperviseur->leSeis.arretMesuresVibrations();
				ptrSuperviseur->etats.UnsetFlagEtatSEIS(Etats::MESURES_EN_COURS);
				// on commande au bras de fermer la pince
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::REPRISE_SEIS"," mesures arretées, on ferme la pince pour le saisir");
				if( !ptrSuperviseur->automateBras.actionPince(AutomateBras::FERMER_PINCE) )  // rupture de flux bras
				{
					throw codeException=RUPTURE_BRAS_AVEC_OBJET;
				};
				ptrSuperviseur->etats.setEtatSEIS(Etats::REPRISE_EN_COURS);
				// on va surveiller la fermeture de la pince
				histopince=false;
				do
				{
					if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL) ||
									(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
						throw codeException=SIGNAL_ARRU;
					};
					sleep(1); // pour ne pas consommer trop de cpu
					if( !ptrSuperviseur->automateBras.lireEtatPince(etatPincePrecedant) )  // rupture de flux bras
					{
						throw codeException=RUPTURE_BRAS_AVEC_OBJET;
					};
					if( !histopince && (etatPincePrecedant != Etats::PINCE_OUVERTE) )
					{
						histopince=true;
						// desque la pince commence a se fermer on efface  la commande et comme on vient de faire
						if (!ptrSuperviseur->automateBras.actionPince(AutomateBras::PAS_NOUVELLE_ACTION)){
							throw codeException=RUPTURE_BRAS_AVEC_OBJET;
						}
					};
				}
				while( etatPincePrecedant != Etats::PINCE_FERMEE );
				// le seis est saisie on va lui faire remonter les pieds
				// c'est une commande synchrone donc bloquante
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::REPRISE_SEIS",
										" La pince est fermée on lance le replie des pieds du SEIS");
				if( !(ptrSuperviseur->leSeis.replierPieds()) )  //requete synchrone donc bloquante mais avec chien de garde
				{
					//le nivellement s'est arrété avec un probleme
					ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::::REPRISE_SEIS","Erreur lors du repliement des pieds");
					throw  codeException=RUPTURE_SEIS_SEUL ;
				};
				// les pieds sont remontés on  demande au bras d'aller déposer le seis sur le plateau
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::REPRISE_SEIS",
								" les pieds du SEIS sont repliés on lance l'action pour remonter sur le plateau");
				if( !ptrSuperviseur->automateBras.ecrireCmdActionBras(AutomateBras::REMONTER_SEIS) )  // rupture de flux bras
				{
					throw codeException=RUPTURE_BRAS_AVEC_OBJET;
					return false;
				};
			};
			// on surveille la reouverture de la pince pour indiquer quele seis est posé sur le plateau
			if( (etatPincePrecedant == Etats::PINCE_FERMEE) && (etatPince == Etats::PINCE_OUVERTE) )
			{
				etatPincePrecedant = Etats::PINCE_OUVERTE;
				ptrSuperviseur->etats.setEtatSEIS(Etats::SUR_PLATEAU);
			};
		}
		if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::REPRISE_SEIS"," on attente que le bras soit en positon panoramique");
		while(etatBras != Etats::POSITION_PANORAMIQUE)
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			};
			sleep(1);
			if( !ptrSuperviseur->automateBras.lireEtatBras(etatBras) )  // rupture de flux bras
			{
				throw codeException=RUPTURE_BRAS_SEUL;
			};
		};
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::REPRISE_SEIS"," séquence terminée");
	}
	catch(int e)
	{
		//traitement des exceptions
		switch (e)
		{
		case  RUPTURE_BRAS_AVEC_OBJET :
			ptrSuperviseur->etats.setEtatSEIS(Etats::INDISPONIBLE); // pas de break car c'est un ajout a ce qui suit
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::::REPRISE_SEIS","  Rupture de flux bras avec objet");
		case  RUPTURE_BRAS_SEUL :
			ptrSuperviseur->etats.SetflagCompteRenduSysteme(Etats::RUPTURE_LIAISON_BRAS);
			ptrSuperviseur->etats.setEtatBras(Etats::INCIDENT_REDHIBITOIRE_BRAS);
			ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::::REPRISE_SEIS","Rupture de flux avec l'automate du bras");

			return false;
			break;
		case RUPTURE_SEIS_SEUL :
			ptrSuperviseur->etats.SetflagCompteRenduSysteme(Etats::RUPTURE_LIAISON_BRAS);
			ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::::REPRISE_SEIS","Rupture de flux avec le seis");
			break;
		case  SIGNAL_ARRU :
			ptrSuperviseur->etats.SetflagCompteRenduSysteme(Etats::COMMANDE_INCONNUE);
			ptrSuperviseur->etats.setEtatBras(Etats::INCONNUE);
			ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::REPRISE_SEIS"," demande d'ARRU dectectée");
			return false;
			break;
		default :
			break;
		}
	}
	return true;
}

//=================================================================================================================================
bool ICN::deposerWTS()
//**********************
{
	uint16_t etatBras;
	uint16_t etatPince;
	uint16_t etatPincePrecedant;
	bool WTSaisi=false;
	bool conf=false;
	int exception;
	int codeException;
	try
	{
 		while(ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			}
			if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
			sleep(1);
		}
		if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
		// on s'assure que la pince soit bien ouverte
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSE_WTS"," vérifie si la pince est ouverte");
		if( !ptrSuperviseur->automateBras.lireEtatPince(etatPincePrecedant) )  // rupture de flux bras
		{
			throw codeException=RUPTURE_BRAS_SEUL;
		};
		// pour le cas (anormale) ou la pince serait fermée
		if(etatPincePrecedant==Etats::PINCE_FERMEE)
		{
			ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSE_WTS","  la pince étant fermée on commande son ouverture");
			if( !ptrSuperviseur->automateBras.actionPince(AutomateBras::OUVRIR_PINCE) )  // rupture de flux bras
			{
				throw codeException=RUPTURE_BRAS_AVEC_OBJET;
			};
			do
			{
				if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
					throw codeException=SIGNAL_ARRU;
				};
				if( !ptrSuperviseur->automateBras.lireEtatPince(etatPince) )  // rupture de flux bras
				{
					throw codeException=RUPTURE_BRAS_AVEC_OBJET;
				};
				if( (etatPincePrecedant==Etats::PINCE_FERMEE) && (etatPince!=Etats::PINCE_FERMEE) ){
					ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSE_WTS",
								" la pince commence son ouverture on efface la commande d'ouverture");
					if (!ptrSuperviseur->automateBras.actionPince(AutomateBras::PAS_NOUVELLE_ACTION)){
						throw codeException=RUPTURE_BRAS_AVEC_OBJET;
					}
				}
				etatPincePrecedant=etatPince;
				if( etatPince != Etats::PINCE_OUVERTE )sleep(1);
			}
			while( etatPince != Etats::PINCE_OUVERTE );
		}
		// ça y est la pince est ouverte, on lance la séquence
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSE_WTS","  la pince est ouverte on peut debuter la sequence");
		if( !ptrSuperviseur->automateBras.lancerSequence(AutomateBras::DEPOSE_WTS) )  // rupture de flux bras
		{
			throw codeException=RUPTURE_BRAS_SEUL;
		};
                if(! ptrSuperviseur->automateBras.ecrireCmdActionBras(AutomateBras::AUCUNE))// rupture de flux bras
                {
                        throw codeException=RUPTURE_BRAS_AVEC_OBJET;
                };
		//on attend que l'automate ait bien pris la commande en compte
		while(! ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			};
			if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
			sleep(1);
		}
		// la commande ayant été prise en compte on l'efface pour quel'automate ne
		// cherche pas à l'exécuter une nouvelle fois apres la fin de cette séquence
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSE_WTS"," la sequence a débutée on efface la commande");
		if( !ptrSuperviseur->automateBras.lancerSequence(AutomateBras::RIEN) )  // rupture de flux bras
		{
			throw codeException=RUPTURE_BRAS_SEUL;
		};
		// mainteant on traite le suivi de la séquence
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSE_WTS"," commande effacée, suivit de la sequence ");
		while(ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			}
			sleep(1); // pour ne pas consommer trop de cpu
			//lecture de l'etat du bras pour le traitemnt en boucle
			if( !ptrSuperviseur->automateBras.lireEtatBras(etatBras) )  // rupture de flux bras
			{
				throw codeException=RUPTURE_BRAS_AVEC_OBJET;
			};
			//lecture de l'etat de la pince pour le traitemnt en boucle
			if( !ptrSuperviseur->automateBras.lireEtatPince(etatPince) )  // rupture de flux bras
			{
				if( etatPincePrecedant==Etats::PINCE_FERMEE ) ptrSuperviseur->etats.setEtatWTS(Etats::INDISPONIBLE);
				throw codeException=RUPTURE_BRAS_SEUL;
			};
			if( (!WTSaisi) && (etatPince==Etats::PINCE_FERMEE) )
			{
				WTSaisi=true;
				ptrSuperviseur->etats.setEtatWTS(Etats::DEPOSE_EN_COURS);
			};
			if( !conf && (etatBras==Etats::WTS_AU_SOL) )
			{
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSE_WTS"," demande au conférencier la confirmation de lacher le WTS");
				while(ptrSuperviseur->reponseIHM!=CONF_LACHER_WTS)
				{
					ptrSuperviseur->DemandeIHM(Etats::DEM_CONF_LACHER_WTS); // demande bloquante
				};
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSE_WTS"," confirmation dépose WTS est reçue, le WTS est posé sur le SEIS");
				ptrSuperviseur->automateBras.ecrireCmdActionBras(AutomateBras::FINIR_SEQ_POSE_WTS);
				conf=true;
			};
			if( (WTSaisi) && (etatPince==Etats::PINCE_OUVERTE) )
			{
				WTSaisi=false;
				ptrSuperviseur->etats.setEtatWTS(Etats::AU_SOL);
			};
		}
		if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSE_WTS"," on attente que le bras soit en positon panoramique");
		while(etatBras != Etats::POSITION_PANORAMIQUE)
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			};
			sleep(1);
			if( !ptrSuperviseur->automateBras.lireEtatBras(etatBras) )  // rupture de flux bras
			{
				throw codeException=RUPTURE_BRAS_SEUL;
			};
		};
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSE_WTS"," séquence terminée");
	}
	catch(int e)
	{
		//traitement des exceptions
		switch (e)
		{
		case  RUPTURE_BRAS_AVEC_OBJET :
			ptrSuperviseur->etats.setEtatWTS(Etats::INDISPONIBLE); // pas de break car c'est un ajout a ce qui suit
		case  RUPTURE_BRAS_SEUL :
			ptrSuperviseur->etats.SetflagCompteRenduSysteme(Etats::RUPTURE_LIAISON_BRAS);
			ptrSuperviseur->etats.setEtatBras(Etats::INCIDENT_REDHIBITOIRE_BRAS);
			ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::::DEPOSE_WTS","  Rupture de flux avec l'automate du bras");
			return false;
			break;
		case  SIGNAL_ARRU :
			ptrSuperviseur->etats.SetflagCompteRenduSysteme(Etats::COMMANDE_INCONNUE);
			ptrSuperviseur->etats.setEtatBras(Etats::INCONNUE);
			ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::DEPOSE_WTS"," demande d'ARRU dectectée");
			return false;
			break;

		default :
			break;
		}
	};
	return true;
}

//=================================================================================================================================
bool ICN::remonterWTS()
//**********************
{
	uint16_t etatBras;
	uint16_t etatPince;
	uint16_t etatPincePrecedant;
	bool WTSaisi=false;
	bool conf=false;
	int exception;
	int codeException;
	try
	{
 		while(ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			}
			if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
			sleep(1);
		}
		if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
		// on s'assure que la pince soit bien ouverte
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::REPRISE_WTS"," vérifie si la pince est ouverte");
		if( !ptrSuperviseur->automateBras.lireEtatPince(etatPincePrecedant) )  // rupture de flux bras
		{
			throw codeException=RUPTURE_BRAS_SEUL;
		};
		// pour le cas (anormale) ou la pince serait fermée
		if(etatPincePrecedant==Etats::PINCE_FERMEE)
		{
			ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_WTS","  la pince étant fermée on commande son ouverture");
			if( !ptrSuperviseur->automateBras.actionPince(AutomateBras::OUVRIR_PINCE) )  // rupture de flux bras
			{
				throw codeException=RUPTURE_BRAS_AVEC_OBJET;
			};
			do
			{
				if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
					throw codeException=SIGNAL_ARRU;
				};
				if( !ptrSuperviseur->automateBras.lireEtatPince(etatPince) )  // rupture de flux bras
				{
					throw codeException=RUPTURE_BRAS_AVEC_OBJET;
				};
				if( (etatPincePrecedant==Etats::PINCE_FERMEE) && (etatPince!=Etats::PINCE_FERMEE) ){
					ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_WTS",
							" la pince commence son ouverture on efface la commande d'ouverture");
					if (!ptrSuperviseur->automateBras.actionPince(AutomateBras::PAS_NOUVELLE_ACTION)){
						throw codeException=RUPTURE_BRAS_AVEC_OBJET;
					}
				}
				etatPincePrecedant=etatPince;
				if( etatPince != Etats::PINCE_OUVERTE )sleep(1);
			}
			while( etatPince != Etats::PINCE_OUVERTE );
		}
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_WTS"," la pince est ouverte on lance la séquence de reprise du WTS");
                if(! ptrSuperviseur->automateBras.ecrireCmdActionBras(AutomateBras::AUCUNE))// rupture de flux bras
                {
                        throw codeException=RUPTURE_BRAS_AVEC_OBJET;
                };
		// ça y est la pince est ouverte, on lance la séquence
		ptrSuperviseur->automateBras.lancerSequence(AutomateBras::REPRISE_WTS);
		//on attend que l'automate ait bien pris la commande en compte
		while(! ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			};
			if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
			sleep(1);
		}
		// la commande ayant été prise en compte on l'efface pour quel'automate ne
		// cherche pas à l'exécuter une nouvelle fois apres la fin de cette séquence
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_WTS"," la sequence a débutée on efface la commande");
		if( !ptrSuperviseur->automateBras.lancerSequence(AutomateBras::RIEN) )  // rupture de flux bras
		{
			throw codeException=RUPTURE_BRAS_SEUL;
		};
		// mainteant on traite le suivi de la séquence
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_WTS"," commande effacée, suivit de la sequence ");
		while(ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			}
			sleep(1); // pour ne pas consommer trop de cpu
			//lecture de l'etat du bras pour le traitemnt en boucle
			if( !ptrSuperviseur->automateBras.lireEtatBras(etatBras) )  // rupture de flux bras
			{
				if( etatPincePrecedant==Etats::PINCE_FERMEE ) ptrSuperviseur->etats.setEtatWTS(Etats::INDISPONIBLE);
				throw codeException=RUPTURE_BRAS_SEUL;
			};
			//lecture de l'etat de la pince pour le traitemnt en boucle
			if( !ptrSuperviseur->automateBras.lireEtatPince(etatPince) )  // rupture de flux bras
			{
				if( etatPincePrecedant==Etats::PINCE_FERMEE ) ptrSuperviseur->etats.setEtatWTS(Etats::INDISPONIBLE);
				throw codeException=RUPTURE_BRAS_SEUL;
			};
			if( !conf && (etatBras==Etats::PRET_A_SAISIR_WTS) )
			{
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::REPRISE_WTS"," demande au conférencier la confirmation pour saisir le WTS");
				while(ptrSuperviseur->reponseIHM!=CONF_SAISIR_WTS)
				{
					ptrSuperviseur->DemandeIHM(Etats::DEM_CONF_SAISIR_WTS); // demande bloquante
				};
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_WTS"," la confirmation de saisie WTS est reçue");
				ptrSuperviseur->automateBras.ecrireCmdActionBras(AutomateBras::FINIR_SEQ_REPRISE_WTS);
				conf=true;
			};
			if( (!WTSaisi) && (etatPince==Etats::PINCE_FERMEE) )
			{
				WTSaisi=true;
				ptrSuperviseur->etats.setEtatWTS(Etats::REPRISE_EN_COURS);
			};
			if( (WTSaisi) && (etatPince==Etats::PINCE_OUVERTE) )
			{
				WTSaisi=false;
				ptrSuperviseur->etats.setEtatWTS(Etats::SUR_PLATEAU);
			};
		}
		if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_WTS","  attente que le bras soit en positon panoramique ");
		while(etatBras != Etats::POSITION_PANORAMIQUE)
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			}
			sleep(1);
			if( !ptrSuperviseur->automateBras.lireEtatBras(etatBras) )  // rupture de flux bras
			{
				throw codeException=RUPTURE_BRAS_SEUL;
			};
		};
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_WTS"," séquence terminée ");
	}
	catch(int e)
	{
		//traitement des exceptions
		switch (e)
		{
		case  RUPTURE_BRAS_AVEC_OBJET :
			ptrSuperviseur->etats.setEtatWTS(Etats::INDISPONIBLE); // pas de break car c'est un ajout a ce qui suit
		case  RUPTURE_BRAS_SEUL :
			ptrSuperviseur->etats.SetflagCompteRenduSysteme(Etats::RUPTURE_LIAISON_BRAS);
			ptrSuperviseur->etats.setEtatBras(Etats::INCIDENT_REDHIBITOIRE_BRAS);
			ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::REPRISE_WTS"," Rupture de flux avec l'automate du bras");
			return false;
			break;
		case  SIGNAL_ARRU :
			ptrSuperviseur->etats.SetflagCompteRenduSysteme(Etats::COMMANDE_INCONNUE);
			ptrSuperviseur->etats.setEtatBras(Etats::INCONNUE);
			ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::DEPOSE_WTS"," demande d'ARRU dectectée");
			return false;
			break;

		default :
			break;
		}
	}
	return true;
}

//=================================================================================================================================
bool ICN::deposerHP3()
//********************
{
	uint16_t etatBras;
	uint16_t etatPince;
	uint16_t etatPincePrecedant;
	bool HP3plateau=true;
	int exception;
	int codeException;
	try
	{
                if(! ptrSuperviseur->automateBras.ecrireCmdActionBras(AutomateBras::AUCUNE))// rupture de flux bras
                {
                        throw codeException=RUPTURE_BRAS_AVEC_OBJET;
                };
 		while(ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			}
			if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
			sleep(1);
		}
		if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
		// on s'assure que la pince soit bien ouverte
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::DEPOSE_HP3"," vérifie si la pince est ouverte");
		if( !ptrSuperviseur->automateBras.lireEtatPince(etatPincePrecedant) )  // rupture de flux bras
		{
			throw codeException=RUPTURE_BRAS_SEUL;
		};
		// pour le cas (anormale) ou la pince serait fermée
		if(etatPincePrecedant==Etats::PINCE_FERMEE)
		{
			ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::DEPOSE_HP3","  la pince étant fermée on commande son ouverture");
			if( !ptrSuperviseur->automateBras.actionPince(AutomateBras::OUVRIR_PINCE) )  // rupture de flux bras
			{
				throw codeException=RUPTURE_BRAS_AVEC_OBJET;
			};
			do
			{
				if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
					throw codeException=SIGNAL_ARRU;
				};
				if( !ptrSuperviseur->automateBras.lireEtatPince(etatPince) )  // rupture de flux bras
				{
					throw codeException=RUPTURE_BRAS_AVEC_OBJET;
				};
				if( (etatPincePrecedant==Etats::PINCE_FERMEE) && (etatPince!=Etats::PINCE_FERMEE) ){
					ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::DEPOSE_HP3",
								" la pince commence son ouverture on efface la commande d'ouverture");
					if (!ptrSuperviseur->automateBras.actionPince(AutomateBras::PAS_NOUVELLE_ACTION)){
						throw codeException=RUPTURE_BRAS_AVEC_OBJET;
					}
				}
				etatPincePrecedant=etatPince;
				if( etatPince != Etats::PINCE_OUVERTE )sleep(1);
			}
			while( etatPince != Etats::PINCE_OUVERTE );
		}
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::DEPOSE_HP3"," la pince est ouverte on lance la séquence de dépose HP3");
		// ça y est la pince est ouverte, on lance la séquence
		ptrSuperviseur->automateBras.lancerSequence(AutomateBras::DEPOSE_HP3);
		//on atend quel'automate ait bien pris a commande en compte
		while(! ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
			sleep(1);
		}
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::DEPOSE_HP3"," la séquence est lancée, on va effacer la commande");
		// la commande ayant été prise en compte on l'efface pour quel'automate ne
		// cherche pas à l'exécuter une nouvelle fois apres la fin de cette séquence
		if( !ptrSuperviseur->automateBras.lancerSequence(AutomateBras::RIEN) )  // rupture de flux bras
		{
			throw codeException=RUPTURE_BRAS_SEUL;
		};
		// mainteant on traite le suivi de la séquence
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::DEPOSE_HP3"," commande effacée on fait le suivit de la sequence");
		while(ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			}
			sleep(1); // pour ne pas consommer trop de cpu
			//lecture de l'etat du bras pour le traitemnt en boucle
			if( !ptrSuperviseur->automateBras.lireEtatBras(etatBras) )  // rupture de flux bras
			{
				if( etatPincePrecedant==Etats::PINCE_FERMEE ) ptrSuperviseur->etats.setEtatHP3(Etats::INDISPONIBLE);
				throw codeException=RUPTURE_BRAS_SEUL;
			};
			//lecture de l'etat de la pince pour le traitemnt en boucle
			if( !ptrSuperviseur->automateBras.lireEtatPince(etatPince) )  // rupture de flux bras
			{
				if( etatPincePrecedant==Etats::PINCE_FERMEE ) ptrSuperviseur->etats.setEtatHP3(Etats::INDISPONIBLE);
				throw codeException=RUPTURE_BRAS_SEUL;
			};

			// si le HP3 vient d'etre saisie on indique a l'ihm qu'il est en cours de dépose
			if( (etatPincePrecedant==Etats::PINCE_OUVERTE) && (etatPince==Etats::PINCE_FERMEE) )
			{
				etatPincePrecedant=Etats::PINCE_FERMEE;
				ptrSuperviseur->etats.setEtatHP3(Etats::DEPOSE_EN_COURS);
				HP3plateau=false;
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::DEPOSE_HP3"," le HP3 a quitté le plateau");
			}
			if( (!HP3plateau) && (etatPincePrecedant==Etats::PINCE_FERMEE) && (etatPince==Etats::PINCE_OUVERTE) )
			{
				etatPincePrecedant=Etats::PINCE_OUVERTE;
				ptrSuperviseur->etats.setEtatHP3(Etats::AU_SOL);
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::DEPOSE_HP3"," le HP3 est au sol");
			}
		}
		if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::DEPOSE_HP3"," attente que le bras soit en positon panoramique ");
		while(etatBras != Etats::POSITION_PANORAMIQUE)
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			}
			sleep(1);
			if( !ptrSuperviseur->automateBras.lireEtatBras(etatBras) )  // rupture de flux bras
			{
				throw codeException=RUPTURE_BRAS_SEUL;
			};
		};
		ptrSuperviseur->etats.setEtatHP3(Etats::AU_SOL); // securité car ca devrait être deja positionné
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::DEPOSE_HP3"," séquence terminée ");
	}
	catch(int e)
		//traitement des exceptions
	{
		switch (e)
		{
		case  RUPTURE_BRAS_AVEC_OBJET :
			ptrSuperviseur->etats.setEtatWTS(Etats::INDISPONIBLE); // pas de break car c'est un ajout a ce qui suit
		case  RUPTURE_BRAS_SEUL :
			ptrSuperviseur->etats.SetflagCompteRenduSysteme(Etats::RUPTURE_LIAISON_BRAS);
			ptrSuperviseur->etats.setEtatBras(Etats::INCIDENT_REDHIBITOIRE_BRAS);
			ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::DEPOSE_HP3"," Rupture de flux avec l'automate du bras");
			return false;
			break;
		case  SIGNAL_ARRU :
			ptrSuperviseur->etats.SetflagCompteRenduSysteme(Etats::COMMANDE_INCONNUE);
			ptrSuperviseur->etats.setEtatBras(Etats::INCONNUE);
			ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::DEPOSE_HP3"," demande d'ARRU dectectée");
			return false;
			break;

		default :
			break;
		}
	}
	return true;
}

//=================================================================================================================================
bool ICN::remonterHP3()
//**********************
{
	uint16_t etatBras;
	uint16_t etatPince;
	uint16_t etatPincePrecedant;
	bool HP3Sol=true;
	bool conf=false;
	int exception;
	int codeException;
	try
	{
                if(! ptrSuperviseur->automateBras.ecrireCmdActionBras(AutomateBras::AUCUNE))// rupture de flux bras
                {
                        throw codeException=RUPTURE_BRAS_AVEC_OBJET;
                };
 		while(ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			}
			if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
			sleep(1);
		}
		if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
		// on s'assure que la pince soit bien ouverte
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::::REPRISE_HP3"," vérifie si la pince est ouverte");
		if( !ptrSuperviseur->automateBras.lireEtatPince(etatPincePrecedant) )  // rupture de flux bras
		{
			throw codeException=RUPTURE_BRAS_SEUL;
		};
		// pour le cas (anormale) ou la pince serait fermée
		if(etatPincePrecedant==Etats::PINCE_FERMEE)
		{
			ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_HP3","  la pince étant fermée on commande son ouverture");
			if( !ptrSuperviseur->automateBras.actionPince(AutomateBras::OUVRIR_PINCE) )  // rupture de flux bras
			{
				throw codeException=RUPTURE_BRAS_AVEC_OBJET;
			};
                	if(! ptrSuperviseur->automateBras.ecrireCmdActionBras(AutomateBras::AUCUNE))// rupture de flux bras
                	{
                        	throw codeException=RUPTURE_BRAS_AVEC_OBJET;
               		};
			do
			{
				if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL) ||
									(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
					throw codeException=SIGNAL_ARRU;
				};
				if( !ptrSuperviseur->automateBras.lireEtatPince(etatPince) )  // rupture de flux bras
				{
					throw codeException=RUPTURE_BRAS_AVEC_OBJET;
				};
				if( (etatPincePrecedant==Etats::PINCE_FERMEE) && (etatPince!=Etats::PINCE_FERMEE) ){
					ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_HP3",
								" la pince commence son ouverture on efface la commande d'ouverture");
					if (!ptrSuperviseur->automateBras.actionPince(AutomateBras::PAS_NOUVELLE_ACTION)){
						throw codeException=RUPTURE_BRAS_AVEC_OBJET;
					}
				}
				etatPincePrecedant=etatPince;
				if( etatPince != Etats::PINCE_OUVERTE )sleep(1);
			}
			while( etatPince != Etats::PINCE_OUVERTE );
		}
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_HP3"," la pince est ouverte on lance la séquence ");
		// ça y est la pince est ouverte, on lance la séquence
		ptrSuperviseur->automateBras.lancerSequence(AutomateBras::REPRISE_HP3);
		//on atend quel'automate ait bien pris a commande en compte
		while(! ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
			sleep(1);
		}
		// la commande ayant été prise en compte on l'efface pour quel'automate ne
		// cherche pas à l'exécuter une nouvelle fois apres la fin de cette séquence
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_HP3"," la séquence est lancée , on peut effacer la commande");
		if( !ptrSuperviseur->automateBras.lancerSequence(AutomateBras::RIEN) )  // rupture de flux bras
		{
			throw codeException=RUPTURE_BRAS_SEUL;
		};
		// mainteant on traite le suivi de la séquence
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_HP3"," commande effacée, on fait le suivit de la sequence");
		while(ptrSuperviseur->automateBras.sequenceEncours(exception))
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			}
			sleep(1); // pour ne pas consommer trop de cpu
			//lecture de l'etat du bras pour le traitemnt en boucle
			if( !ptrSuperviseur->automateBras.lireEtatBras(etatBras) )  // rupture de flux bras
			{
				if( etatPincePrecedant==Etats::PINCE_FERMEE ) ptrSuperviseur->etats.setEtatHP3(Etats::INDISPONIBLE);
				throw codeException=RUPTURE_BRAS_SEUL;
			};
			//lecture de l'etat de la pince pour le traitemnt en boucle
			if( !ptrSuperviseur->automateBras.lireEtatPince(etatPince) )  // rupture de flux bras
			{
				if( etatPincePrecedant==Etats::PINCE_FERMEE ) ptrSuperviseur->etats.setEtatHP3(Etats::INDISPONIBLE);
				throw codeException=RUPTURE_BRAS_SEUL;
			};

			// si le HP3 vient d'etre saisie on indique a l'ihm qu'il est en cours de dépose
			if( (etatPincePrecedant==Etats::PINCE_OUVERTE) && (etatPince==Etats::PINCE_FERMEE) )
			{
				etatPincePrecedant=Etats::PINCE_FERMEE;
				ptrSuperviseur->etats.setEtatHP3(Etats::REPRISE_EN_COURS);
				HP3Sol=false;
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_HP3"," le HP3 a quitté le sol");
			}
			if( !conf && (etatBras==Etats::PRET_DEPOSE_HP3_PLATEAU) )
			{
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_HP3"," demande au conférencier , l'autorisato de poser le HP3 sur le plateau");
				while(ptrSuperviseur->reponseIHM!=CONF_POSE_HP3)
				{
					ptrSuperviseur->DemandeIHM(Etats::DEM_CONF_POSE_HP3); // demande bloquante
				};
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_HP3","  confirmation  reçue");;
				ptrSuperviseur->automateBras.ecrireCmdActionBras(AutomateBras::FINIR_SEQ_POSE_HP3);
				conf=true;
			};
			if( (!HP3Sol) && (etatPincePrecedant==Etats::PINCE_FERMEE) && (etatPince==Etats::PINCE_OUVERTE) )
			{
				etatPincePrecedant=Etats::PINCE_OUVERTE;
				ptrSuperviseur->etats.setEtatHP3(Etats::SUR_PLATEAU);
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_HP3"," le HP3 est sur le plateau");;
			};

		}
		if(exception==ProxyAutomate::RUPTURE_DE_FLUX)throw codeException=RUPTURE_BRAS_SEUL;
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_HP3"," attente que le bras soit en positon panoramique ");
		while(etatBras != Etats::POSITION_PANORAMIQUE)
		{
			if((ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_LOGICIEL)||(ptrSuperviseur->NatureAntiRun==Superviseur::ARRU_MATERIEL)){
				throw codeException=SIGNAL_ARRU;
			}
			sleep(1);
			if( !ptrSuperviseur->automateBras.lireEtatBras(etatBras) )  // rupture de flux bras
			{
				throw codeException=RUPTURE_BRAS_SEUL;
			};
		};
		ptrSuperviseur->etats.setEtatHP3(Etats::SUR_PLATEAU); // securité car l'etat devrait déjà être positionné plus haut
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"ICN::REPRISE_HP3"," séquence terminée ");
	}
	catch(int e)
	{
		//traitement des exceptions
		switch (e)
		{
		case  RUPTURE_BRAS_AVEC_OBJET :
			ptrSuperviseur->etats.setEtatWTS(Etats::INDISPONIBLE); // pas de break car c'est un ajout a ce qui suit
		case  RUPTURE_BRAS_SEUL :
			ptrSuperviseur->etats.SetflagCompteRenduSysteme(Etats::RUPTURE_LIAISON_BRAS);
			ptrSuperviseur->etats.setEtatBras(Etats::INCIDENT_REDHIBITOIRE_BRAS);
			ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::REPRISE_HP3"," Rupture de flux avec l'automate du bras");
			return false;
			break;
		case  SIGNAL_ARRU :
			ptrSuperviseur->etats.SetflagCompteRenduSysteme(Etats::COMMANDE_INCONNUE);
			ptrSuperviseur->etats.setEtatBras(Etats::INCONNUE);
			ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICN::REPRISE_HP3"," demande d'ARRU dectectée");
			return false;
			break;

		default :
			break;
		}
	}
	return true;
}

//=================================================================================================================================

//********************************************
//**************	ICS	**************
//********************************************
//=================================================================================================================================
ICS::ICS()
{
}

//=================================================================================================================================
void ICS::init(Superviseur * ptrsuperviseur)
//***********************
{
	ptrSuperviseur=ptrsuperviseur;
}

//=================================================================================================================================
bool ICS::ouvrirPince()
//*********************
{
	uint16_t etatPince;
	int tempsEcoule=0;
 	ptrSuperviseur->automateBras.commandeSpecialePince(AutomateBras::OUVRERTURE_PINCE);
	ptrSuperviseur->automateBras.lireEtatPince(etatPince);
	while( (etatPince == Etats::PINCE_FERMEE) && (tempsEcoule < TEMPS_REACTION_PINCE) )
	{
		sleep(1);
		tempsEcoule++;
		ptrSuperviseur->automateBras.lireEtatPince(etatPince);
	};
	ptrSuperviseur->automateBras.commandeSpecialePince(AutomateBras::EFFACE_ACTION);
	if (tempsEcoule == TEMPS_REACTION_PINCE){
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICS::ouvrirPince()"," Temps de reaction d'ouverture pince dépasée ");
		return false;
	};
	tempsEcoule=0;
	while( (etatPince != Etats::PINCE_OUVERTE) &&  (tempsEcoule < CHIEN_DE_GARDE_MOUVEMENT_PINCE) )
	{
		sleep(1);
		tempsEcoule++;
		ptrSuperviseur->automateBras.lireEtatPince(etatPince);
	};
	if (tempsEcoule == CHIEN_DE_GARDE_MOUVEMENT_PINCE){
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICS::ouvrirPince()"," chien de garde tombé ");
		return false;
	};
	return true;
}

//=================================================================================================================================
bool ICS::fermerPince()
//*********************
{
	uint16_t etatPince;
	int tempsEcoule=0;
	ptrSuperviseur->automateBras.commandeSpecialePince(AutomateBras::FERMETURE_PINCE);
	ptrSuperviseur->automateBras.lireEtatPince(etatPince);
	while( (etatPince == Etats::PINCE_OUVERTE ) && (tempsEcoule < TEMPS_REACTION_PINCE) )
	{
		sleep(1);
		tempsEcoule++;
		ptrSuperviseur->automateBras.lireEtatPince(etatPince);
	};
	ptrSuperviseur->automateBras.commandeSpecialePince(AutomateBras::EFFACE_ACTION);
	if (tempsEcoule == TEMPS_REACTION_PINCE){
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICS::fermerPince()"," Temps de reaction d'ouverture pince dépasée ");
		return false;
	};
	tempsEcoule=0;
	while( ( etatPince != Etats::PINCE_FERMEE ) &&  (tempsEcoule < CHIEN_DE_GARDE_MOUVEMENT_PINCE) )
	{
		sleep(1);
		tempsEcoule++;
		ptrSuperviseur->automateBras.lireEtatPince(etatPince);
	};
	if (tempsEcoule == CHIEN_DE_GARDE_MOUVEMENT_PINCE){
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ICS::fermePince()"," chien de garde tombé ");
		return false;
	};
	return true;
}

//=================================================================================================================================
void ICS::SeisLancerMesures()
//***************************
{
    bool dejaConnecte=ptrSuperviseur->leSeis.isConnecte();
    if(!dejaConnecte)ptrSuperviseur->leSeis.SeConnecter(ptrSuperviseur->adrsIpSeis,ptrSuperviseur->portSeis);
    if(ptrSuperviseur->leSeis.isConnecte()){
	if(ptrSuperviseur->leSeis.lancerMesureVibrations())
	{
		ptrSuperviseur->etats.SetFlagEtatSEIS(Etats::MESURES_EN_COURS);
	};
	if(!dejaConnecte)ptrSuperviseur->leSeis.seDeconnecter ( );
    }
}

//=================================================================================================================================
void ICS::SeisArretMesures()
//**************************
{
    bool dejaConnecte=ptrSuperviseur->leSeis.isConnecte();
    if(!dejaConnecte)ptrSuperviseur->leSeis.SeConnecter(ptrSuperviseur->adrsIpSeis,ptrSuperviseur->portSeis);
    if(ptrSuperviseur->leSeis.isConnecte()){
	ptrSuperviseur->leSeis.arretMesuresVibrations();
	ptrSuperviseur->etats.UnsetFlagEtatSEIS(Etats::MESURES_EN_COURS);
	if(!dejaConnecte)ptrSuperviseur->leSeis.seDeconnecter ( );
    }
}

//=================================================================================================================================
void ICS::SeisReplierPieds()
//**************************
{
    bool dejaConnecte=ptrSuperviseur->leSeis.isConnecte();
    if(!dejaConnecte)ptrSuperviseur->leSeis.SeConnecter(ptrSuperviseur->adrsIpSeis,ptrSuperviseur->portSeis);
    if(ptrSuperviseur->leSeis.isConnecte()){
	ptrSuperviseur->leSeis.replierPieds();
	if(!dejaConnecte)ptrSuperviseur->leSeis.seDeconnecter ( );
    }
}

//=================================================================================================================================
void ICS::SeisNiveller()
//**********************
{
    bool dejaConnecte=ptrSuperviseur->leSeis.isConnecte();
    if(!dejaConnecte)ptrSuperviseur->leSeis.SeConnecter(ptrSuperviseur->adrsIpSeis,ptrSuperviseur->portSeis);
    if(ptrSuperviseur->leSeis.isConnecte()){
	ptrSuperviseur->leSeis.nivellement();
	if(!dejaConnecte)ptrSuperviseur->leSeis.seDeconnecter ( );
    }
}


//=================================================================================================================================
void ICS::MettrePanneauxHS()
//**************************
{
	ptrSuperviseur->etats.SetFlagMode(Etats::M_PANNEAUX_HS);
}

//===== FIN FICHIER

