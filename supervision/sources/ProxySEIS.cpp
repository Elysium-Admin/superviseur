#include "../defs/ProxySEIS.h"
#include "../defs/sockets.h"
#include "../defs/Superviseur.h"
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h> // pour ETIMEDOUT et EBUSY

#define BUFFER_RECEPTION 1500

//===========================================================================================
ProxySEIS::ProxySEIS()
	: canal(-1), connecte(false),ModeMesure(false),ptrSuperviseur(NULL),demandesynchrone(false)
{
	pthread_mutex_init(&attenteReponse, NULL);
	pthread_mutex_init(&attenteConnexion, NULL);
	pthread_cond_init(&reponseRecue, NULL);
}

//===========================================================================================
ProxySEIS::~ProxySEIS ( ) { }

//===========================================================================================
void ProxySEIS::initProxySeis(Superviseur * ptrSup)
{
	ptrSuperviseur=ptrSup;
}
//===========================================================================================
// fonction de connexion au SEIS; necessite l'adresse IP et le port du SEIS en parametres
//-------------------------------------------------------------------------------------------
bool ProxySEIS::SeConnecter (string AdresseIP, unsigned short port )
{
	stringstream strStream;
	struct sockaddr_in adresse;
	erreurCode=0;
	ptrSuperviseur->v.verbMsg(Verbose::DBG,"ProxySEIS::SeConnecter" , "  Début de la procédure");
	if ((canal=sockClientTCPtoutesIF())==-1)
	{
		erreurCode=1;
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ProxySEIS::SeConnecter" , " Impossible de créer la socket");
		return false; // à faire : gestion du code et message d'erreur
	}
	if (ptrSuperviseur==NULL)
	{
		erreurCode=2;
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ProxySEIS::SeConnecter" , " ptrSuperviseur=NULL ");
		return false; // à faire : gestion du code et message d'erreur
	}
	adresse.sin_family=AF_INET;
	adresse.sin_addr.s_addr=inet_addr(AdresseIP.c_str());
	adresse.sin_port=htons(port);
	int tentatives=0;
	bool presenceSeis=false;
	while(!presenceSeis)
	{
		if(connect(canal,( struct sockaddr *)&adresse , sizeof(struct sockaddr))==-1) // à decommenter avec le vrai seis
		{
			tentatives++;
 	    		strStream.clear();
			strStream <<" ... tentative n° "<<tentatives;
			ptrSuperviseur->v.verbMsg(Verbose::INFO," ProxySEIS::::SeConnecter()",strStream.str() );
			if(tentatives>=NB_MAX_TENTATIVES) // on limite le nombre de tentatives de connexion
			{
 	    			strStream.clear();
				strStream <<" IMPOSSIBLE de se connecter au serveur du SEIS "<<AdresseIP<<":"<<port;
				ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::SeConnecter()",strStream.str());
				return false;
			}
		}
		else
		{
 	    		strStream.clear();
			strStream <<" connecté au serveur du SEIS "<<AdresseIP<<":"<<port;
			ptrSuperviseur->v.verbMsg(Verbose::INFO," ProxySEIS::SeConnecter()",strStream.str());
			connecte=true;
			presenceSeis=true;
		}
	}
	pthread_cond_signal(&SeisConnecte);
	return true;
}

//===========================================================================================
// fonction de deconnexion au SEIS
//-------------------------------------------------------------------------------------------
void ProxySEIS::seDeconnecter()
{
	if(ModeMesure)
	{
		ModeMesure=false;
		arretMesuresVibrations();
	};
	shutdown(canal,2);
	connecte=false;
	ptrSuperviseur->v.verbMsg(Verbose::DBG,"ProxySEIS::seDeconnecter()" , "  Deconnexion du SEIS faite");
}

//===========================================================================================

// fonction d'envoie synchrone connexion au SEIS
//  cette fonction attend une réponse en retour; elle NE PEUT être utilisée pour des commandes
//  aynchrones telles que les demandes de mesure
//  elle DOIT etre utilisée par exemple lorsque on attend une reponse  comme pour le deploiement
//  des pieds du SEIS
//-------------------------------------------------------------------------------------------
bool ProxySEIS::EnvoiSyn(string cmd )
{
	static struct timespec AttenteMaxReponse = {0, 0};
	FL[0] = 0x0D;
	FL[1] = 0x0A;
	FL[2] = 0x00;
	cmd = cmd + FL;
	reponse="";
	int retcode;
	if(!connecte)
	{
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::EnvoiSyn()"," NON connecté au serveur du SEIS ");
		reponse="SEIS_NON_CONNECTE";
		return false;
	};
	pthread_mutex_lock(&attenteReponse);
	demandesynchrone=true;
	if(ModeMesure)
	{
		ModeMesure=false;
		arretMesuresVibrations();
	};
	// initialisation du chien de garde
	AttenteMaxReponse.tv_sec = time(NULL) + ATTENTE_MAX_REPONSE_SYNCHRONE;
	// lancement du chien de garde
	if(write(canal,cmd.c_str(),cmd.length())<1)
	{
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::EnvoiSyn()"," echec à l'envoie de commande au SEIS ");
		reponse="ECHEC_ENVOIE_SEIS";
		return false;
	}
	retcode=pthread_cond_timedwait (&reponseRecue,&attenteReponse, &AttenteMaxReponse);
	demandesynchrone=false;
	pthread_mutex_unlock(&attenteReponse);
	if( retcode == ETIMEDOUT)   //timeout
	{
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::EnvoiSyn()", " durée d'attente réponse dépasée");
		reponse="TIME_OUT";
		return false;
	}
	cout<<"good :)"<<endl;
	return true;
}

//===========================================================================================
bool ProxySEIS::tester(unsigned int &resutats)
{
	stringstream strStream;
	if(!connecte)
	{
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::tester()","NON connecté au serveur du SEIS");
		return false;
	}
	else
	{
		resutats=0;
		reponse="";
		if (EnvoiSyn((string)"$eth;"))
		{
			if(reponse!="$EthOK;")
			{
 				strStream.clear();
				strStream<<" réponse recue :"<<reponse<<" réponse attendue :$EthOK;";
				ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::tester()",strStream.str() );
				resutats += ECHEC_CONNEXION_SEIS ;
			};
		}
		else
		{
			erreurCode=8;
			return false;
		};
		reponse="";
		if (EnvoiSyn((string)"$tcon;"))
		{
			if(reponse!="$BAT0;")
			{
 				strStream.clear();
				strStream<<" réponse recue :"<<reponse<<" réponse attendue :$BAT0;";
				ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::tester()",strStream.str() );
				resutats += BATTERIE_SEIS_FAIBLE ;
			};
		}
		else
		{
			erreurCode=8;
			return false;
		}
		reponse="";
		if (EnvoiSyn((string)"$tcapt;"))  // reponse normale $CAP000; sinon
		{
			if(reponse!="$CAP000;")
			{
				strStream.clear();
				strStream<<" réponse recue :"<<reponse<<" réponse attendue :$CAP000;";
				ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::tester()",strStream.str() );
				if(reponse[4]=='1')resutats += CAPTEUR_PROMITE_1_EN_DEFAUT ;
				if(reponse[5]=='1')resutats += CAPTEUR_PROMITE_2_EN_DEFAUT ;
				if(reponse[6]=='1')resutats += CAPTEUR_PROMITE_3_EN_DEFAUT ;
			};
		}
		else
		{
			erreurCode=8;
			return false;
		}
		reponse="";
		if (EnvoiSyn((string)"$tmot;"))  // reponse normale $MOT000; sinon
		{
			if(reponse!="$MOT000;")
			{
				strStream.clear();
				strStream<<" réponse recue :"<<reponse<<" réponse attendue :$MOT000;";
				if(reponse[4]=='1')resutats += CAPTEUR_PROMITE_1_EN_DEFAUT ;
				if(reponse[5]=='1')resutats += CAPTEUR_PROMITE_2_EN_DEFAUT ;
				if(reponse[6]=='1')resutats += CAPTEUR_PROMITE_3_EN_DEFAUT ;
			};
		}
		else
		{
			erreurCode=8;
			return false;
		}
		reponse="";
		if (EnvoiSyn((string)"$tpot;"))  // reponse normale $POT000; sinon
		{
			if(reponse!="$POT000;")
			{
				strStream.clear();
				strStream<<" réponse recue :"<<reponse<<" réponse attendue :$POT000;";
				if(reponse[4]=='1')resutats += CAPTEUR_PROMITE_1_EN_DEFAUT ;
				if(reponse[5]=='1')resutats += CAPTEUR_PROMITE_2_EN_DEFAUT ;
				if(reponse[6]=='1')resutats += CAPTEUR_PROMITE_3_EN_DEFAUT ;
			};
		}
		else
		{
			erreurCode=8;
			return false;
		}
		reponse="";
		if (EnvoiSyn((string)"$tacc;"))  // reponse normale $ACC000; sinon
		{
			if(reponse!="$ACC000;")
			{
				strStream.clear();
				strStream<<" réponse recue :"<<reponse<<" réponse attendue :$ACC000;";
				if(reponse[4]=='1')resutats += CAPTEUR_PROMITE_1_EN_DEFAUT ;
				if(reponse[5]=='1')resutats += CAPTEUR_PROMITE_2_EN_DEFAUT ;
				if(reponse[6]=='1')resutats += CAPTEUR_PROMITE_3_EN_DEFAUT ;
			};
		}
		else
		{
			erreurCode=8;
			return false;
		}
		reponse="";
	}
	return true;
}

//===========================================================================================
bool  ProxySEIS::replierPieds() // synchrone
{
	stringstream str_msg;
	string rep;
	erreurCode=0;
	if(!connecte)
	{
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::replierPieds()","NON connecté au serveur du SEIS");
		return false;
	};
	if(ModeMesure==true)
	{
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::replierPieds()"," replie des pieds Impossible tant qu'on est en mode mesure");
	}
	else
	{
	    for(int i = 0 ; i <NB_MAX_TENTATIVES ;i++)
            {
		if (EnvoiSyn((string)"$Crepli;"))
		{
//			if(reponse=="$!frep0;")
			if(reponse=="$!frepli;")
			{
				ptrSuperviseur->v.verbMsg(Verbose::INFO," ProxySEIS::replierPieds()"," replie des pieds effectué");
				return true;
			}
			else
			{
//				str_msg <<" reponse attendue '$!frep0;' du seis mais reçu "<<reponse;
				str_msg <<" reponse attendue '$!frepli;' du seis mais reçu "<<reponse;
				ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::replierPieds()",str_msg.str());
				return false;
			};
		}
		else
		{
		   if(i<NB_MAX_TENTATIVES){
			i++;
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::replierPieds()"," Time out , on relance la commande");
		   }
		   else{
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::replierPieds()"," Echec envoie synchrone");
			return false;
		   }
		}
	    }
	}
	ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::replierPieds()"," Echec envoie synchrone apres trois tentatives");
	return false;
}


//===========================================================================================
bool  ProxySEIS::nivellement()  // synchrone
{
	stringstream str_msg;
	bool bonnereponse=false;
	erreurCode=0;
	if(!connecte)
	{
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::nivellement()","NON connecté au serveur du SEIS");
		return false;
	};
	if(ModeMesure==true)
	{
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::nivellement()"," replie des pieds Impossible tant qu'on est en mode mesure");
	}
	else
	{
		do
		{
			if (EnvoiSyn((string)"$Cnivel;"))
			{
				if(reponse=="$!fniv0;")
				{
					bonnereponse=true;
					ptrSuperviseur->v.verbMsg(Verbose::INFO," ProxySEIS::nivellement()","Nivellement = True") ;
				}
				else
				{
					str_msg <<" reponse attendue '$Cnivel;' du seis mais reçu "<<reponse;
					ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::nivellement()",str_msg.str());
				}
			}
			else
			{
				ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::nivellement()", " échec de nivellement ");
				return false;
			}
		}
		while (!bonnereponse);
	}
	return bonnereponse;
}


//===========================================================================================
// fonction d'envoie Asynchrone connexion au SEIS
//  cette focntion n'attend pas de réponse en retour;
//  elle NE PEUT etre utilisée lorsque on attend une reponse comme dans le cas du deploiement
//  des pieds du SEIS
//  elle DOIT être utilisée pour des commandes qui n'attendent PAS de réponse
//  telles que les demandes de mesures
//-------------------------------------------------------------------------------------------

bool ProxySEIS::EnvoiAssyn (string cmd )
{
	FL[0] = 0x0D;
	FL[1] = 0x0A;
	FL[2] = 0x00;
	cmd = cmd +(string)FL;
	if(!connecte)
	{
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::EnvoiAssyn" ," pas connecté au SEIS");
		return false;
	}
	if(write(canal,cmd.c_str(),cmd.length())<1)
	{
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::EnvoiAssyn" ,"  erreur d'écriture");
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------
bool ProxySEIS::EnvoiAssyn(string cmd ,string & strrep)
{
	ptrVarReponse=&strrep;
	return EnvoiAssyn (cmd );
}

//===========================================================================================
bool  ProxySEIS::detecterSol(string &chaineRep)  // Asynchrone mais avec variable réponse
{
	if(ModeMesure==false)
	{
		if( EnvoiAssyn((string)"$Capp;",chaineRep) )
		{
			return true;
		};
	};
	ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::detecterSol()" ,"  erreur durant la detection sol");
	return false;
}
//===========================================================================================
bool  ProxySEIS::nivellement(string &chaineRep)  // Asynchrone mais avec variable réponse
{
	erreurCode=0;
	if(ModeMesure==false)
	{
		if( EnvoiAssyn((string)"$Cnivel;",chaineRep) )
		{
			return true;
		};
	};
	ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::nivellement(...)" ,"  erreur durant le nivellement");
	return false;
}
//===========================================================================================
bool  ProxySEIS::lancerMesureVibrations() //asynchrone mais sans reponse
{
	ModeMesure=true;
	ptrSuperviseur->v.verbMsg(Verbose::INFO," ProxySEIS::lancerMesureVibrations()" , "demande de mesure de vibrations");
	return EnvoiAssyn ((string)"$Cme;");
}

//===========================================================================================
void  ProxySEIS::arretMesuresVibrations()//asynchrone mais sans reponse
{
	stringstream strStream;
	for(int i=0; i<NB_MAX_TENTATIVES_ARRET; i++)
	{
 	    	strStream.clear();
		EnvoiAssyn ((string)"A");
		strStream <<" ... tentative n°"<<i<<" de demande d'arret";
		usleep(ATTENTE_ENTRE_DEUX_ENVOIS_ARRET_MESURES);
		EnvoiAssyn ((string)"A");
		usleep(ATTENTE_ENTRE_DEUX_ENVOIS_ARRET_MESURES);
		EnvoiAssyn ((string)"A");
		ptrSuperviseur->v.verbMsg(Verbose::INFO," ProxySEIS::arretMesuresVibrations()",strStream.str() );
		RecuMesure=false;
		usleep(ATTENTE_MAX_ETABLISSEMENT_CONNEXION);
		if(!RecuMesure)i=NB_MAX_TENTATIVES_ARRET;
	}
	ModeMesure=false;
}

//===========================================================================================


//===========================================================================================
void ProxySEIS::ReceptionEnvoiSEIS ()
{
	stringstream strStream;
	char buffer[BUFFER_RECEPTION];
	int nbcarlus;
	string chaine;
	while(ptrSuperviseur==NULL)sleep(1);
	while(!ptrSuperviseur->SupervisionEnCours)sleep(1);
	ptrSuperviseur->v.verbMsg(Verbose::INFO," ProxySEIS::ReceptionEnvoiSEIS" , " l'écoute du SEIS est active");
	while(1)
	{
		if(!connecte)
		{
			ptrSuperviseur->v.verbMsg(Verbose::INFO," ProxySEIS::ReceptionEnvoiSEIS" , " Attente de la connexion au SEIS");
			pthread_mutex_lock(&attenteConnexion);
			pthread_cond_wait (&SeisConnecte,&attenteConnexion);
			pthread_mutex_unlock(&attenteConnexion);
			ptrSuperviseur->v.verbMsg(Verbose::INFO," ProxySEIS::ReceptionEnvoiSEIS" , " connexion SEIS établie");
		}
		if((nbcarlus=read(canal,buffer,BUFFER_RECEPTION))>0)
		{
			buffer[nbcarlus-2]=0x00; // -2 pour eliminer CR, LF
			// -------------------------------
			// probleme de protocole incompris
			// -------------------------------
			if((strcmp(buffer,"$QUOI?"))==0)
			{
				ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::ReceptionEnvoiSEIS" , " le SEIS a envoyer 'QUOI?'");
/*				if(ptrVarReponse!=NULL)
				{
					*ptrVarReponse=(string)buffer;
					ptrVarReponse=NULL;
				};
				reponse=(string)buffer;
				pthread_cond_signal(&reponseRecue);
*/			}
			// ----------------------------------------------------------------------------
			// réponse a une commande ou reception d'une mesure ou trame d'erreur spontanée
			// ----------------------------------------------------------------------------
			else if( (buffer[0]=='$')&&(buffer[1]=='!') )
			{
				// ----------------------
				// reception d'une mesure
				// ----------------------
				if( buffer[2]=='m')
				{
					if( buffer[3]=='v')
					{
						RecuMesure=true;
						ptrSuperviseur->MesuresAEnvoyer.ajouter(&(buffer[5]));
					};
				}
				// ----------------------
				// reponse à une commande
				// ----------------------
				else
				{
 	    				strStream.clear();
					strStream <<"reçu par le SEIS :"<<(string)buffer;
					ptrSuperviseur->v.verbMsg(Verbose::DBG,"ProxySEIS::ReceptionEnvoiSEIS()",strStream.str() );
					if( buffer[2]=='f')
					{
						if(!ModeMesure)
						{
							if(demandesynchrone)
							{
 	    							strStream.clear();
								strStream <<"reponse a la demande synchrone "<<(string)buffer;
								ptrSuperviseur->v.verbMsg(Verbose::INFO,"ProxySEIS::ReceptionEnvoiSEIS()",strStream.str() );
								pthread_mutex_lock(&attenteReponse);
								pthread_mutex_unlock(&attenteReponse);
								reponse=(string)buffer;
								pthread_cond_signal(&reponseRecue);
							}
							else
							{
								if(ptrVarReponse!=NULL)
								{
 	    								strStream.clear();
									strStream <<"reponse a la demande Asynchrone "<<(string)buffer;
									ptrSuperviseur->v.verbMsg(Verbose::INFO,"ProxySEIS::ReceptionEnvoiSEIS()",strStream.str() );
									*ptrVarReponse=(string)buffer;
									ptrVarReponse=NULL;
								}else{

 	    								strStream.clear();
									strStream <<"reponse a la demande Asynchrone "<<(string)buffer << "mais il n'y aucune variable pour recevoir la réponse";
									ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ProxySEIS::ReceptionEnvoiSEIS()",strStream.str() );
								}
							}
						}
						// ???????????????????????????????????????????????????????????????????????
						// pas normale d'avoir une réponse a une commande si on est en mode mesure
						// .......................................................................
						else
						{
							ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxySEIS::ReceptionEnvoiSEIS()"," replie des pieds Impossible tant qu'on est en mode mesure");
						};
						// ______________________________
						// compte rendu spontané d'erreur
						// ''''''''''''''''''''''''''''''
					}
					else
					{
						if( (buffer[2]=='E')&&(buffer[3]=='R')&&(buffer[3]=='R') )
						{
							//une trame d'erreur spontanée
 	    						strStream.clear();
							strStream <<"reception d'uen erreur spontanée  "<<(string)buffer;
							ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ProxySEIS::ReceptionEnvoiSEIS()",strStream.str() );
							unsigned int erreur= ((buffer[4]-'0')*10)+(buffer[5]-'0');
							ptrSuperviseur->etats.setErreurSEIS(erreur);
							ptrSuperviseur->etats.SetFlagMode(Etats::M_SEIS_HS);
							ptrSuperviseur->NatureAntiRun=Superviseur::INCIDENT_PERIPH;
							pthread_cond_signal (&(ptrSuperviseur->SignalAntiRun));

						}
						else
						{
 	    						strStream.clear();
							strStream <<"reponse  "<<(string)buffer << " non identifiée";
							ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ProxySEIS::ReceptionEnvoiSEIS()",strStream.str() );
						};
					};
				};
				// -----------------
				// réponse à un test
				// -----------------
			}
			else if(buffer[0]=='$')   // reponse au test
			{
 	    			strStream.clear();
				strStream.str("");
				strStream <<"reception d'uen reponse de test  "<<(string)buffer ;
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"ProxySEIS::ReceptionEnvoiSEIS()",strStream.str() );
				reponse=(string)buffer;
				pthread_cond_signal(&reponseRecue);
			}
			// -------------------------------------------
			// alors la !! il y a un probleme de protocole
			// -------------------------------------------
			else
			{
 	    			strStream.clear();
				strStream <<"reponse  "<<(string)buffer <<" inconnue dans le protovcole du SEIS";
				ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ProxySEIS::ReceptionEnvoiSEIS()",strStream.str() );
			}
		}
		// -----------------------------------------
		// rupture de flux, la connection est coupée
		// ------------------------------------------
		else
		{
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"ProxySEIS::ReceptionEnvoiSEIS()", " rupture de Flux avec le SEIS" );
			seDeconnecter();
		}
	}
}

//===========================================================================================


