#include "../defs/automates.h"
#include "../defs/Superviseur.h"
#include "../defs/verbose.h"


//===============================================================================================================
//		AUTOMATE DE LA CARTE CCGX (énergie)
//===============================================================================================================

void AutomateCCGX::init(Superviseur * ptrSup)
{
	ptrSuperviseur=ptrSup;
}

bool AutomateCCGX::_lire(int adrreg,short *buffer)
{
	relaterDansLog(adrreg, (uint16_t) *buffer);
	if(!lireRegistres(adrreg,1,(uint16_t *)buffer))
	{
		if(isConnecte )
		{
			if (Unit_Id != UNIT_ID_CCGX1)
			{
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"AUTOMATE_CCGX::LIRE_CCGX",
							"changement de borne : tentative sur la première borne");
				Unit_Id = UNIT_ID_CCGX1;
			}
			else
			{
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"AUTOMATE_CCGX::LIRE_CCGX",
							 "changement de borne : echec sur la 1 on essaye sur la borne 2");
				Unit_Id = UNIT_ID_CCGX2;
			};
			if(lireRegistres(adrreg,1,(uint16_t *)buffer))
			{
				relaterDansLog(adrreg, (uint16_t) *buffer);
				return true;
			};
			if (Unit_Id != UNIT_ID_CCGX2)
			{
				ptrSuperviseur->v.verbMsg(Verbose::INFO,"AUTOMATE_CCGX::LIRE_CCGX",
							"changement de borne : tentative sur la deuxieme borne");
				Unit_Id = UNIT_ID_CCGX2;
			};
			if(lireRegistres(adrreg,1,(uint16_t *)buffer))
			{
				relaterDansLog(adrreg, (uint16_t) *buffer);
				return true;
			};
			ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"AUTOMATE_CCGX::LIRE_CCGX",
					"aucune des connections VE.direct de la CCGX ne semble active, verifier la connexion entre la BMV608 et la carte CCGX");
			seDeconnecter();
			ptrSuperviseur->etats.SetFlagMode(Etats::M_BATTERIES_HS);
			return false;
		};
		// verbose : rupture de flux (pas forcemetn à faire ici)
		ptrSuperviseur->etats.SetFlagMode(Etats::M_BATTERIES_HS);
		return false;
	};
	return true;
}
//---------------------------------------------------------------------------------------------------------------

bool AutomateCCGX::connecter(Superviseur * ptrsuperviseur,string Adrs,unsigned short port)
{
	ptrSuperviseur=ptrsuperviseur;
	if(!connecterServeur(Adrs,port) )
	{
		ptrSuperviseur->etats.SetFlagMode(Etats::M_BATTERIES_HS);
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"AUTOMATE_CCGX::CONNECTER"," connexion échouée CCGX, batterie hs");
		return false;
	};
	ptrSuperviseur->etats.UnsetFlagMode(Etats::M_BATTERIES_HS);
	ptrSuperviseur->v.verbMsg(Verbose::INFO,"AUTOMATE_CCGX::LIRE_CCGX"," connexion CCGX réussie");
	return true;
}

//---------------------------------------------------------------------------------------------------------------
bool AutomateCCGX::lireTensionBatteries(unsigned short &tension)
{
	if ( _lire(REG_ETAT_TENSION_BATTERIES,(short*)&tension) )
	{
		ptrSuperviseur->etats.setTensionBatteries(tension);
		return true;
	};
	return false;
}

//---------------------------------------------------------------------------------------------------------------
bool  AutomateCCGX::lireCourantBatterie(short & courant)
{
	if ( _lire(REG_ETAT_COURANT_BATTERIES,&courant) )
	{
		ptrSuperviseur->etats.setCourantBatteries(courant);
		return true;
	};
	return false;
}

//---------------------------------------------------------------------------------------------------------------
bool  AutomateCCGX::lireChargeConsommee(short & chargeconsommee)
{
	if ( _lire(REG_ETAT_CHARGE_CONSOMMEE,&chargeconsommee) )
	{
		ptrSuperviseur->etats.SetChargeConsommee(chargeconsommee);
		return true;
	};
	return false;
}

//---------------------------------------------------------------------------------------------------------------
bool  AutomateCCGX::lireChargeBatterie(unsigned short & chargerestante)
{
	if ( _lire(REG_ETAT_CHARGES_BATTERIES,(short*)&chargerestante) )
	{
		ptrSuperviseur->etats.setChargeBatteries(chargerestante);
		return true;
	};
	return false;
}

//---------------------------------------------------------------------------------------------------------------
bool  AutomateCCGX::lireChargeDureeRestante(unsigned short & duree)
{
	if ( _lire(REG_ETAT_DUREE_RESTANTE,(short*)&duree) )
	{
		ptrSuperviseur->etats.setDureeRestanteBatteries(duree);
		return true;
	};
	return false;
}

//---------------------------------------------------------------------------------------------------------------
bool  AutomateCCGX::lireStatusAlarmes()
{
	uint16_t buffer,incident;
	unsigned char flags = ptrSuperviseur->etats.getIncidentBatteries();
	_lire(REG_ETAT_STATUS_ALARMES,(short *)&buffer);
	if(buffer==AutomateCCGX::INCIDENT)
	{
		if ( (_lire(REG_ALARME_TENTION_BASSE,(short *)&incident)) && ( incident == AutomateCCGX::INCIDENT) )
		{
			ptrSuperviseur->etats.SetflagIncidentBatteries(Etats::ALARME_TENSION_BASSE);
		}
		else
		{
			ptrSuperviseur->etats.UnsetflagIncidentBatteries(Etats::ALARME_TENSION_BASSE);
		}
		if( (_lire(REG_ALARME_TENTION_HAUTE,(short *)&incident)) && ( incident== AutomateCCGX::INCIDENT) )
		{
			ptrSuperviseur->etats.SetflagIncidentBatteries(Etats::ALARME_TENSION_HAUTE);
		}
		else
		{
			ptrSuperviseur->etats.UnsetflagIncidentBatteries(Etats::ALARME_TENSION_HAUTE);
		}
		if( (_lire(REG_ALARME_TEMPERATURE,(short *)&incident)) && ( incident== AutomateCCGX::INCIDENT) )
		{
			ptrSuperviseur->etats.SetflagIncidentBatteries(Etats::ALARME_TEMPERATURE);
		}
		else
		{
			ptrSuperviseur->etats.UnsetflagIncidentBatteries(Etats::ALARME_TEMPERATURE);
		}
		if( (_lire(REG_STATUS_RELAIS,(short *)&incident)) && ( incident== AutomateCCGX::INCIDENT) )
		{
			ptrSuperviseur->etats.SetflagIncidentBatteries(Etats::ALARME_STATUS_RELAIS);
		}
		else
		{
			ptrSuperviseur->etats.UnsetflagIncidentBatteries(Etats::ALARME_STATUS_RELAIS);
		}
		return true;
	}
	else
	{
		ptrSuperviseur->etats.setIncidentBatteries(flags & (Etats::TENSION_BASSE + Etats::CHARGE_BASSE));
	};
	return false;
}

//---------------------------------------------------------------------------------------------------------------
void AutomateCCGX::deconneter()
{
	seDeconnecter();
	ptrSuperviseur->v.verbMsg(Verbose::INFO,"AUTOMATE_CCGX::DECONNECTER_CCGX","déconnexion CCGX réussie");
	ptrSuperviseur->etats.SetFlagMode(Etats::M_BATTERIES_HS);
}

//---------------------------------------------------------------------------------------------------------------

void AutomateCCGX::relaterDansLog(int reg, uint16_t val)
{
	string message;
//	ostringstream str_msg;
//	str_msg.str("");
	stringstream str_msg;
	str_msg.clear();
	str_msg <<"lecture du registre ("<<reg<<")"<< nomDuRegistre(reg)<<" valeur lue : ("<<val<<") 0x"<< hex << setfill('0')<<setw(4) <<val;
	message = str_msg.str();
	ptrSuperviseur->v.verbMsg(Verbose::DBG,"AUTOMATE_CCGX::LIRE_CCGX",message);

}
//---------------------------------------------------------------------------------------------------------------
string AutomateCCGX::nomDuRegistre(int reg)
{
	switch(reg)
	{
	case 	259 :
		return "REG_ETAT_TENSION_BATTERIES";
	case 	261 :
		return "REG_ETAT_COURANT_BATTERIES";
	case 	265 :
		return "REG_ETAT_CHARGE_CONSOMMEE";
	case 	266 :
		return "REG_ETAT_CHARGES_BATTERIES";
	case 	267 :
		return "REG_ETAT_STATUS_ALARMES";
	case 	301 :
		return "REG_ETAT_DUREE_RESTANTE";
	case 	268 :
		return "REG_ALARME_TENTION_BASSE";
	case 	269 :
		return "REG_ALARME_TENTION_HAUTE";
	case 	274 :
		return "REG_ALARME_TEMPERATURE";
	case 	280 :
		return "REG_STATUS_RELAIS";
	default :
		break;
	};
	return "registre inconnu";
}
//---------------------------------------------------------------------------------------------------------------

//===============================================================================================================
//  AUTOMATE DES PANNEAUX
//===============================================================================================================
// methodes protected
//********************
void AutomatePanneaux::init(Superviseur * ptrSup)
{
	ptrSuperviseur=ptrSup;
}

bool AutomatePanneaux::_ecrire(int adrreg, uint16_t val)
//------------------------------------------------------
{
	relaterDansLog(adrreg, val, ProxyAutomate::REG_ECRITURE);
	if(!ecrireDansRegistre(adrreg, val))
	{
		if(isConnecte )seDeconnecter();
		ptrSuperviseur->etats.SetFlagMode(Etats::M_PANNEAUX_HS);
		ptrSuperviseur->etats.setIncidentPanneaux(0xffffffff);
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------------------------------------------

bool AutomatePanneaux::_lire(int adrreg,uint16_t *buffer)
{
	relaterDansLog(adrreg, (uint16_t) *buffer, ProxyAutomate::REG_LECTURE);
	if(!lireRegistres(adrreg,1,buffer)) // rupture de liaison
	{
		ptrSuperviseur->etats.SetFlagMode(Etats::M_PANNEAUX_HS);
		ptrSuperviseur->etats.setIncidentPanneaux(0xffffffff);
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------------------------------------------

string AutomatePanneaux::nomDuRegistre(int reg)
{
	switch(reg)
	{
	case 	101 :
		return "REG_ARRU";
	case 	120 :
		return "REG_CMD_PANNEAUX";
	case 	102 :
		return "REG_CMD_TESTS_INIT";
	case 	103 :
		return "REG_ETAT_ARRU";
	case 	121 :
		return "REG_ETAT_PANNEAUX";
	case 	122 :
		return "REG_ETAT_INIT_PANNEAUX";
	case 	125 :
		return "REG_DEFAUTS_PANNEAUX";
	default :
		break;
	};
	return "registre inconnu";
}

//---------------------------------------------------------------------------------------------------------------
// methodes publics
//*****************


void AutomatePanneaux::relaterDansLog(int reg, uint16_t val, int sens)
{
	string message;
	ostringstream str_msg;
	str_msg.str("");
	if (sens==ProxyAutomate::REG_LECTURE)
	{
		str_msg <<"lecture du registre ("<<reg<<")"<< nomDuRegistre(reg)<<" valeur lue : ("<<val<<") 0x"<< hex << setfill('0')<<setw(4) <<val;
		message = str_msg.str();
		ptrSuperviseur->v.verbMsg(Verbose::DBG,"AUTOMATE_PANNEAUX::LIRE_PANNEAUX",message);
	}
	else
	{
		str_msg <<"écriture dans le registre ("<<reg<<")"<< nomDuRegistre(reg)<<" de la valeur : ("<<val<<") 0x"<< hex << setfill('0')<<setw(4) <<val;
		message = str_msg.str();
		ptrSuperviseur->v.verbMsg(Verbose::DBG,"AUTOMATE_PANNEAUX::ECRIRE_PANNEAUX",message);
	}

}
//---------------------------------------------------------------------------------------------------------------

bool AutomatePanneaux::connecter(Superviseur * ptrsuperviseur,string Adrs,unsigned short port)
{
	ptrSuperviseur=ptrsuperviseur;
	if(!connecterServeur(Adrs,port) )
	{
		ptrSuperviseur->etats.SetFlagMode(Etats::M_PANNEAUX_HS);
		ptrSuperviseur->etats.setIncidentPanneaux(0xffffffff);
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"AUTOMATE_PANNEAUX::CONNECTER_PANNEAUX"," connexion échouée , panneaux hs");
		return false;
	};
	ptrSuperviseur->etats.UnsetFlagMode(Etats::M_PANNEAUX_HS);
	ptrSuperviseur->v.verbMsg(Verbose::INFO,"AUTOMATE_PANNEAUX::CONNECTER_PANNEAUX"," connexion aux panneaux réussie ");
	return true;
}
//---------------------------------------------------------------------------------------------------------------

bool AutomatePanneaux::lireARRU(uint16_t & arru)
{
	if (!_lire(REG_ETAT_ARRU,&arru))return false;// rupture de liaison
	return true;
}

//---------------------------------------------------------------------------------------------------------------
bool AutomatePanneaux::lireEtatPanneaux()
{
	unsigned char flags;
	uint16_t val;
	if (!_lire(REG_ETAT_PANNEAUX,&val))return false; // rupture de liaison
	flags=val & 0xff;
	lireDefautsPanneaux();
	ptrSuperviseur->etats.setEtatPanneaux(flags);
	if(flags == Etats::INCIDENT_REDHIBITOIRE_PANNEAUX)
	{
		ptrSuperviseur->etats.SetFlagMode(Etats::M_PANNEAUX_HS);
	}
	return true;
}

//---------------------------------------------------------------------------------------------------------------
bool AutomatePanneaux::lireEtatPanneaux(uint16_t & lue)
{
	bool retour = lireEtatPanneaux();
	lue = ptrSuperviseur->etats.getEtatPanneaux();
	return retour;
}

//---------------------------------------------------------------------------------------------------------------

bool AutomatePanneaux::lireEtatInitPanneaux(uint16_t & lue)
{
	stringstream strStream;
	if (!_lire(REG_ETAT_INIT_PANNEAUX,&lue))return false;// rupture de liaison
	if (!lireDefautsPanneaux())return false;// rupture de liaison
	if( lue == AutomatePanneaux::INIT_PB )
	{
		ptrSuperviseur->etats.SetFlagMode(Etats::M_PANNEAUX_HS);
		ptrSuperviseur->etats.setEtatPanneaux(Etats::INCIDENT_REDHIBITOIRE_PANNEAUX);
	}
	else
	{
		stringstream str_msg;
		//str_msg.clear();
		strStream<<" Etats::PHASE_INIT + lue ="<< hex << Etats::PHASE_INIT + lue <<endl;
		ptrSuperviseur->v.verbMsg(Verbose::INFO,"AUTOMATE_PANNEAUX::INIT_PANNEAUX",strStream.str());
		ptrSuperviseur->etats.setEtatPanneaux(Etats::PHASE_INIT + lue);
	}
	return true;
}

//---------------------------------------------------------------------------------------------------------------
bool AutomatePanneaux::lireDefautsPanneaux()
{
	uint16_t bufferH,bufferB;
	if (!_lire(REG_DEFAUTS_PANNEAUX,&bufferH))return false;// rupture de liaison
	if (!_lire(REG_DEFAUTS_PANNEAUX+1,&bufferB))return false;// rupture de liaison
	defauts=(bufferH<<16)+bufferB;
	ptrSuperviseur->etats.setIncidentPanneaux(defauts);
	return true;
}
//---------------------------------------------------------------------------------------------------------------

void AutomatePanneaux::deconnecter()
{
	seDeconnecter();
	ptrSuperviseur->etats.SetFlagMode(Etats::M_PANNEAUX_HS);
	ptrSuperviseur->etats.setIncidentPanneaux(0xffffffff);
	ptrSuperviseur->v.verbMsg(Verbose::INFO,"AUTOMATE_PANNEAUX::DECONNECTER_PANNEAUX" ," deconnexion réussie panneaux");
}
//---------------------------------------------------------------------------------------------------------------

bool AutomatePanneaux::ecrireCmdPanneaux(uint16_t val)
{
	return _ecrire(REG_CMD_PANNEAUX,val);
}
//---------------------------------------------------------------------------------------------------------------

bool AutomatePanneaux::ecrireARRU(uint16_t val)
{
	return _ecrire(REG_ARRU,val);
}
//---------------------------------------------------------------------------------------------------------------

bool AutomatePanneaux::ecrireInitPanneaux(uint16_t val)
{
	return _ecrire(REG_CMD_TESTS_INIT,val);
}
//---------------------------------------------------------------------------------------------------------------
unsigned int AutomatePanneaux::getDefauts()
{
	return defauts ;
}
//---------------------------------------------------------------------------------------------------------------

//===============================================================================================================
//  AUTOMATE DU BRAS
//===============================================================================================================
// methodes protected
//********************
void AutomateBras::init(Superviseur * ptrSup)
{
	ptrSuperviseur=ptrSup;
}

bool AutomateBras::_ecrire(int adrreg, uint16_t val)
{
	relaterDansLog(adrreg, val, ProxyAutomate::REG_ECRITURE);
	if(!ecrireDansRegistre(adrreg, val))
	{
		ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
		ptrSuperviseur->etats.setCodeErreurBras(0xffffffff);
		return false;
	}
	return true;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AutomateBras::_lire(int adrreg,uint16_t *buffer)
{
	relaterDansLog(adrreg, (uint16_t) *buffer, ProxyAutomate::REG_LECTURE);
	if(!lireRegistres(adrreg,1,buffer))
	{
		if(isConnecte )seDeconnecter();
		ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
		ptrSuperviseur->etats.setCodeErreurBras(0xffffffff);
		return false;
	}
	return true;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

string AutomateBras::nomDuRegistre(int reg)
{
	switch(reg)
	{
	case 	101 :
		return "REG_ARRU";
	case 	102 :
		return "REG_CMD_TESTS_INIT";
	case 	105 :
		return "REG_CMD_ACTION_BRAS";
	case 	106 :
		return "REG_CMD_SEQ";
	case 	107 :
		return "REG_CMD_ACTION_PINCE";
	case 	103 :
		return "REG_ETAT_ARRU";
	case 	108 :
		return "REG_ETAT_BRAS";
	case 	109 :
		return "REG_ETAT_SEQ_ENCOURS";
	case 	110 :
		return "REG_ETAT_PINCE";
	case 	111 :
		return "REG_ETAT_INIT_BRAS";
	case 	116 :
		return "REG_DEFAUTS_BRAS";
	default :
		break;
	};
	return "registre inconnu";
}
//---------------------------------------------------------------------------------------------------------------
// methodes publics
//*****************


void AutomateBras::relaterDansLog(int reg, uint16_t val, int sens)
{
	string message;
	ostringstream str_msg;
	str_msg.str("");
	if (sens==ProxyAutomate::REG_LECTURE)
	{
		str_msg <<"lecture du registre ("<<reg<<")"<< nomDuRegistre(reg)<<" valeur lue : ("<<val<<") 0x"<< hex << setfill('0')<<setw(4) <<val;
		message = str_msg.str();
		ptrSuperviseur->v.verbMsg(Verbose::DBG,"AUTOMATE_BRAS::LIRE_REG_BRAS",message);
	}
	else
	{
		str_msg <<"écriture dans le registre ("<<reg<<")"<< nomDuRegistre(reg)<<" de la valeur : ("<<val<<") 0x"<< hex << setfill('0')<<setw(4) <<val;
		message = str_msg.str();
		ptrSuperviseur->v.verbMsg(Verbose::DBG,"AUTOMATE_BRAS::ECRIRE_REG_BRAS",message);
	}

}
//---------------------------------------------------------------------------------------------------------------
bool AutomateBras::connecter(Superviseur * ptrsuperviseur, string Adrs,unsigned short port)
{
	ptrSuperviseur=ptrsuperviseur;
	if(!connecterServeur(Adrs,port) )
	{
		ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
		ptrSuperviseur->etats.setCodeErreurBras(0xffffffff);
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"AUTOMATE_BRAS::CONNECTER_BRAS" , " connexion échouée : bras hs");
		return false;
	};
	ptrSuperviseur->etats.UnsetFlagMode(Etats::M_BRAS_HS);
	ptrSuperviseur->v.verbMsg(Verbose::INFO,"AUTOMATE_BRAS::CONNECTER_BRAS" , "connexion au bras réussie ");
	return true;
}
//---------------------------------------------------------------------------------------------------------------
bool AutomateBras::lireEtatInitBras(uint16_t & lue)
{

	if (!_lire(REG_ETAT_INIT_BRAS,&lue))
	{
		return false;// rupture de liaison
	};
	if (!lireDefautsBras())
	{
		return false;// rupture de liaison
	};
	if( lue == AutomateBras::INIT_PB )
	{
		ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
		ptrSuperviseur->etats.setEtatBras(Etats::INCIDENT_REDHIBITOIRE_BRAS);
	}
	else
	{
		ptrSuperviseur->etats.setEtatBras(Etats::INIT_BRAS_EN_COURS);
	}
	return true;
}

//---------------------------------------------------------------------------------------------------------------

bool AutomateBras::ecrireInitBras(uint16_t val)
{
	relaterDansLog(REG_CMD_TESTS_INIT , val , ProxyAutomate::REG_ECRITURE);
	return _ecrire(REG_CMD_TESTS_INIT,val);
}
//---------------------------------------------------------------------------------------------------------------

bool AutomateBras::actionPince(uint16_t action)
{
	relaterDansLog(REG_CMD_ACTION_PINCE , action , ProxyAutomate::REG_ECRITURE);
	return _ecrire(REG_CMD_ACTION_PINCE,action);
}

//---------------------------------------------------------------------------------------------------------------

bool AutomateBras::ecrireCmdActionBras(uint16_t action)
{
	relaterDansLog(REG_CMD_ACTION_BRAS , action , ProxyAutomate::REG_ECRITURE);
	return _ecrire(REG_CMD_ACTION_BRAS,action);
}
//---------------------------------------------------------------------------------------------------------------

bool AutomateBras::sequenceEncours(int &exception)
{
	uint16_t buffer;
	exception = ProxyAutomate::PAS_DINCIDENT;
	if( !_lire(REG_ETAT_SEQ_ENCOURS,&buffer) )
	{
		exception = ProxyAutomate::RUPTURE_DE_FLUX;
		return false; //rupture de flux
	};
	if( buffer==AutomateBras::AUCUNE_SEQUENCE_COURANTE )
	{
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------------------------------------------

bool AutomateBras::lancerSequence(uint16_t seq)
{
	relaterDansLog(REG_CMD_SEQ , seq , ProxyAutomate::REG_ECRITURE);
	return _ecrire(REG_CMD_SEQ,seq);
}

//---------------------------------------------------------------------------------------------------------------

bool AutomateBras::lireEtatPince(uint16_t& val)
{
	unsigned char flags;
	if (!_lire(REG_ETAT_PINCE,&val))return false;// rupture de liaison
	flags=val & 0xff;
	ptrSuperviseur->etats.setEtatPince(flags);
	return true;
}
//---------------------------------------------------------------------------------------------------------------

bool AutomateBras::lireARRU(uint16_t & arru)
{
	if (!_lire(REG_ETAT_ARRU,&arru))return false;// rupture de liaison
	return true;
}
//---------------------------------------------------------------------------------------------------------------

bool AutomateBras::ecrireARRU(uint16_t val)
{
	return _ecrire(REG_CMD_ARRU,val);
}
//---------------------------------------------------------------------------------------------------------------

bool AutomateBras::lireEtatBras(uint16_t& val)
{
	unsigned char flags;
	if (!_lire(REG_ETAT_BRAS,&val))return false;// rupture de liaison
	flags=val & 0xff;
	ptrSuperviseur->etats.setEtatBras(flags);
	if (!lireDefautsBras())return false;// rupture de liaison
	if(flags == Etats::INCIDENT_REDHIBITOIRE_BRAS)ptrSuperviseur->etats.SetFlagMode(Etats::M_BRAS_HS);
	return true;
}
//---------------------------------------------------------------------------------------------------------------

bool AutomateBras::lireDefautsBras()
{
	uint16_t bufferH,bufferB;
	if (!_lire(REG_DEFAUTS_BRAS,&bufferH))return false;// rupture de liaison
	if (!_lire(REG_DEFAUTS_BRAS+1,&bufferB))return false;// rupture de liaison
	defauts=(bufferH<<16)+bufferB;
	ptrSuperviseur->etats.setCodeErreurBras(defauts);
	return true;
}
//---------------------------------------------------------------------------------------------------------------

unsigned int AutomateBras::getDefauts()
{
	return defauts ;
}
//---------------------------------------------------------------------------------------------------------------
// cette methode n'est a utiliser que "hors sequence", dans les commandes "spéciales" de la pince
//===============================================================================================
bool AutomateBras::commandeSpecialePince(uint16_t action)
{
	if ((action==EFFACE_ACTION) || (action==OUVRERTURE_PINCE) || (action==FERMETURE_PINCE) ){
		return _ecrire(REG_CMD_ACTION_BRAS,action);
	}
	return false;
}
//---------------------------------------------------------------------------------------------------------------

