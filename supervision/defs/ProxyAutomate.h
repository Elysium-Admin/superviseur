#ifndef PROXYAUTOMATE_H
#define PROXYAUTOMATE_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdint.h>

#define ID_PROTO_MODBUS	0
#define ADRS_CLIENT_MODBUS	0xff
#define MODBUS_TCP_MAX_ADU_LENGTH 256

#include <pthread.h>

using namespace std;

//******************debut Ajout 25 Mars 2015****
class Superviseur;
//******************fin Ajout 25 Mars 2015****

//==================================================================================================================
//==================================================================================================================
class ProxyAutomate
{
public:
	ProxyAutomate( uint8_t unitId=ADRS_CLIENT_MODBUS);
	~ProxyAutomate();

	enum OFSETS_MODBUS_TCP
	{
		OFSET_ID_TRANSACTION	=0x00,
		OFSET_ID_PROTOCOLE	=0x02,
		OFSET_LONGUEUR		=0x04,
		OFSET_ID_APPAREIL	=0x06,
		OFSET_CODE_FONCTION	=0x07
	};
	enum CODES_FONCTIONS
	{
		READ_REGISTER			=03,
		READ_IMPUT_REGISTER		=04,
		WRITE_SINGLE_REGISTER		=06,
		WRITE_MULTIPLE_REGISTERS	=16,
		READWRITE_MULTIPLE_REGISTERS	=23,
		READ_EXEPTION_STATUS		=07
	};
	enum INCIDENT_LIAISON
	{
		PAS_DINCIDENT			=0,
		RUPTURE_DE_FLUX			=-1
	};
	enum ACTION
	{
		REG_LECTURE		=0,
		REG_ECRITURE		=1
	};

	bool connecterServeur(string Adrs,unsigned short port);
	bool reconnecterServeur();
	bool ecrireDansRegistre(int adrreg,uint16_t val);
	bool lireRegistres(int adrreg,int nbReg,uint16_t *buffer);
	bool estConnecte();
	void seDeconnecter();
	Superviseur* ptrSuperviseur;
//******************debut Ajout 25 Mars 2015****
	void init (Superviseur* ptrsup);
//******************fin Ajout 25 Mars 2015****

protected :
	string afficherTrameModBas( uint8_t *buffer ,int nboctets);
	int canal;
	string AdrsSrv;
	bool isConnecte;
	bool DejaInit;
	unsigned short portServeur;
	int IdTransaction;
	uint8_t Unit_Id;
	uint8_t requete[MODBUS_TCP_MAX_ADU_LENGTH];
	uint8_t reponse[MODBUS_TCP_MAX_ADU_LENGTH];
	pthread_mutex_t semRequeteLectureEnCours;


	pthread_mutex_t semRequeteEcritureEnCours;

};


#endif // PROXYAUTOMATE_H
