//
//g++ ProxyAutomate.cpp -c -o ProxyAutomate.o

//
#include "../defs/ProxyAutomate.h"
#include <arpa/inet.h>
#include "../defs/sockets.h"
#include <unistd.h>
#include "../defs/Superviseur.h"

#include <iomanip>


//===================================================================================================
ProxyAutomate::ProxyAutomate( uint8_t unitId)
//------------------------------------------
	: canal(-1),isConnecte(false)
{
	requete[ProxyAutomate::OFSET_ID_PROTOCOLE]=ID_PROTO_MODBUS;
	requete[1+ProxyAutomate::OFSET_ID_PROTOCOLE]=ID_PROTO_MODBUS;
	Unit_Id=unitId;
	requete[ProxyAutomate::OFSET_ID_APPAREIL]=Unit_Id;
	pthread_mutex_init(&semRequeteLectureEnCours, NULL);
	pthread_mutex_init(&semRequeteEcritureEnCours, NULL);
	DejaInit=false;
}

//===================================================================================================
void ProxyAutomate::init (Superviseur* ptrsup)
//-------------------------------------------
{
	ptrSuperviseur = ptrsup ;
}

//===================================================================================================
ProxyAutomate::~ProxyAutomate()
//------------------------------------------
{
}

//===================================================================================================
bool ProxyAutomate::connecterServeur(string Adrs,unsigned short port)
//------------------------------------------
{
	struct sockaddr_in adresse;
	if(ptrSuperviseur ==NULL)
	{
		// pas de verbose possible car le pointeur vers le superviseur est NULL donc un cerr classqiue
		cerr<<" *** ERREUR *** proxyAutomate::connecterServeur  :  ptrSuperviseur == NULL" <<endl;
		return false;
	};
	if((canal=sockClientTCPtoutesIF())<0)
	{
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxyAutomate::connecterServeur()"," échec attachement socket");
		return false;
	};
	adresse.sin_family=AF_INET;
	adresse.sin_addr.s_addr=inet_addr(Adrs.c_str());
	adresse.sin_port=htons(port);
	if(connect(canal,(struct sockaddr*)&adresse,sizeof(struct sockaddr))!=-1)
	{
		isConnecte=true;
		AdrsSrv=Adrs;
		portServeur=(unsigned short)port;
		IdTransaction=1;
	}
	DejaInit=true;
	return isConnecte;
}

//===================================================================================================
bool ProxyAutomate::reconnecterServeur()
//------------------------------------------
{
	if (!DejaInit) return false;
	return connecterServeur(AdrsSrv,portServeur);
}

//===================================================================================================
void ProxyAutomate::seDeconnecter()
//------------------------------------------
{
	if (isConnecte)
	{
		isConnecte=false;
		close(canal);
		AdrsSrv="";
		portServeur=0;
	}
}

//===================================================================================================
bool ProxyAutomate::ecrireDansRegistre(int adrreg,uint16_t val)
//-------------------------------------------------------------
{
//******************debut modif 11 avril 2015****
    pthread_mutex_lock(&semRequeteEcritureEnCours);
    if(!isConnecte){
		stringstream str_msg;
		str_msg <<" ecriture impossible car non connecté au "<<AdrsSrv<<":"<<portServeur;
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxyAutomate::ecrireDansRegistre()",str_msg.str());
    }else{
	requete[ProxyAutomate::OFSET_ID_TRANSACTION]=(uint8_t)(IdTransaction>>8);;
	requete[1+ProxyAutomate::OFSET_ID_TRANSACTION]=(uint8_t)(IdTransaction&0x00ff);
	IdTransaction++;
	requete[ProxyAutomate::OFSET_LONGUEUR]=0;//poid fort de 0x0006 (longueurs des champs qui suivent)
	requete[1+ProxyAutomate::OFSET_LONGUEUR]=6;//poid faible de 0x0006 (longueurs des champs qui suivent)
	requete[ProxyAutomate::OFSET_ID_APPAREIL]=Unit_Id;
	requete[ProxyAutomate::OFSET_CODE_FONCTION]=ProxyAutomate::WRITE_SINGLE_REGISTER;

	requete[1+ProxyAutomate::OFSET_CODE_FONCTION]=(uint8_t)(adrreg>>8);;
	requete[2+ProxyAutomate::OFSET_CODE_FONCTION]=(uint8_t)(adrreg&0x00ff);

	requete[3+ProxyAutomate::OFSET_CODE_FONCTION]=(uint8_t)(val>>8);;
	requete[4+ProxyAutomate::OFSET_CODE_FONCTION]=(uint8_t)(val&0x00ff);
	write(canal,requete,12);
	int nblu=read(canal,reponse,sizeof(reponse));
	if(nblu<=0)seDeconnecter();
    };
    pthread_mutex_unlock(&semRequeteEcritureEnCours);
    return isConnecte;
//******************fin Modif 11 avril Mars 2015****
}

//===================================================================================================
bool ProxyAutomate::lireRegistres(int adrregDebut,int nbReg,uint16_t *buffer)
//----------------------------------------------------------------------------
{
	pthread_mutex_lock(&semRequeteLectureEnCours);
	stringstream str_msg;
	if(!isConnecte){
		str_msg <<" ecriture impossible car non connecté au "<<AdrsSrv<<":"<<portServeur;
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR," ProxyAutomate::lireDansRegistre()",str_msg.str());
	}else{
	int lg=(2*nbReg)+4;
	requete[ProxyAutomate::OFSET_ID_TRANSACTION]=(uint8_t)(IdTransaction>>8);
	requete[1+ProxyAutomate::OFSET_ID_TRANSACTION]=(uint8_t)(IdTransaction&0x00ff);
	IdTransaction++;
	requete[ProxyAutomate::OFSET_LONGUEUR]=(uint8_t)(lg>>8);
	requete[1+ProxyAutomate::OFSET_LONGUEUR]=(uint8_t)(lg&0x00ff);
	requete[ProxyAutomate::OFSET_ID_APPAREIL]=Unit_Id;
	requete[ProxyAutomate::OFSET_CODE_FONCTION]=ProxyAutomate::READ_REGISTER;

	requete[1+ProxyAutomate::OFSET_CODE_FONCTION]=(uint8_t)(adrregDebut>>8);;
	requete[2+ProxyAutomate::OFSET_CODE_FONCTION]=(uint8_t)(adrregDebut&0x00ff);

	requete[3+ProxyAutomate::OFSET_CODE_FONCTION]=(uint8_t)(nbReg>>8);;
	requete[4+ProxyAutomate::OFSET_CODE_FONCTION]=(uint8_t)(nbReg&0x00ff);

	write(canal,requete,12);
	int nblu=read(canal,reponse,sizeof(reponse));
	if(nblu<=0)
	{
		str_msg.clear();
		str_msg <<" pas de Flux "<<AdrsSrv<<":"<<portServeur;
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"  ProxyAutomate::lireRegistres()",str_msg.str());
		seDeconnecter();
	}
	else if(nblu<(1+ProxyAutomate::OFSET_CODE_FONCTION))
	{
		str_msg.clear();
		str_msg <<" reponse ModBus malformatée de "<<AdrsSrv<<":"<<portServeur<<" = "<< afficherTrameModBas(reponse ,nblu);
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"  ProxyAutomate::lireRegistres()",str_msg.str());
		seDeconnecter();
	}
	else if(reponse[ProxyAutomate::OFSET_CODE_FONCTION]!=ProxyAutomate::READ_REGISTER)
	{
		str_msg.clear();
		str_msg <<" reponse ModBus d'exception de "<<AdrsSrv<<":"<<portServeur<<" = "<< afficherTrameModBas(reponse ,nblu);
		ptrSuperviseur->v.verbMsg(Verbose::ERREUR,"  ProxyAutomate::lireRegistres()",str_msg.str());
		seDeconnecter();
	}
	else
	{
		nblu=(int)reponse[1+ProxyAutomate::OFSET_CODE_FONCTION]/2;
		for(int i=0; i<nblu; i++)
		{
			buffer[i]= (reponse[2*(i+1)+ProxyAutomate::OFSET_CODE_FONCTION]<<8)+
					   reponse[1+(2*(i+1))+ProxyAutomate::OFSET_CODE_FONCTION];
		}
	}
    }
    pthread_mutex_unlock(&semRequeteLectureEnCours);
    return isConnecte;
}

//===================================================================================================
string ProxyAutomate::afficherTrameModBas( uint8_t *buffer ,int nboctets)
{
	ostringstream str_trame;
	str_trame.str("");
	str_trame << "\""<< hex << setfill('0');
	for(int i= 0; i<nboctets; i++)
	{
		str_trame << setw(2) << (int) buffer[i] << " ";    // exemple 'f2 '
	}
	str_trame << "\"";
	return str_trame.str();

}

//===================================================================================================
bool ProxyAutomate::estConnecte()
//-------------------------------
{
	return isConnecte;
}

//===================================================================================================



