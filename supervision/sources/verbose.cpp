#include <iostream>
#include <string>
#include <ctime>
#include <cstdio>
#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <pthread.h>

#include "../defs/verbose.h"
#include "../defs/Superviseur.h"

using namespace std;

//==============================================================================================================
//
//***************************************************************************************************************
Verbose::Verbose() : initVerb(false)
//---------------------------------------------------------------------------------------------------------------
{
	if(gettimeofday(&depart, NULL)==-1){
		cerr<<"erreur gettimeofday"<<endl;
	};
	textlv[RIEN]="";
	textlv[ERREUR]="ERR**";
	textlv[INFO]="INFO ";
	textlv[DBG]="DBG  ";
}


//==============================================================================================================
//
//***************************************************************************************************************
void Verbose::init(int level, bool surEcran, string chemin, int nbHist)
//---------------------------------------------------------------------------------------------------------------
{
	cheminLog=chemin;
	lvlVerbose=level;
	logSurEcran=surEcran;
	nombreDhistoriques=nbHist;
	init();
}

//==============================================================================================================
//
//***************************************************************************************************************
void Verbose::setCheminLog(string chemin)
{
	cheminLog=chemin;
}


//==============================================================================================================
//
//***************************************************************************************************************
void Verbose::setNbHistoriques(int nbHist)
{
	nombreDhistoriques=nbHist;
}



//==============================================================================================================
//
//***************************************************************************************************************
void Verbose::init()
//---------------------------------------------------------------------------------------------------------------
{
    	stringstream strStream;
	string nom_base=cheminLog + "Insight";
	string str1,str2;
	strStream << nom_base<<"-"<<setfill('0') << setw(2) << nombreDhistoriques << ".log";
	strStream >> str1;
	remove(str1.c_str());
	for(int i=nombreDhistoriques-1 ; i>0 ; i--){
		str2=str1;
        	strStream.clear();
		strStream.str("");
		strStream << nom_base<<"-"<<setfill('0') << setw(2) << i << ".log";
		strStream >> str1;
		rename(str1.c_str(), str2.c_str());
	}
        strStream.clear();
	strStream.str("");
	strStream << nom_base<<".log";
	strStream >> nom_base;
	rename(nom_base.c_str(), str1.c_str());

	fichier.open(nom_base.c_str(), fstream::app);
	if(fichier){
		initVerb=true;
		cout<<"info [Verbose::init] : ouverture du fichier log réussie"<<endl;
	}else{
		initVerb=false;
		cout<<"***echec*** [Verbose::init] : ouverture du fichier log échouée"<<endl;
	}
}

//==============================================================================================================
//
//***************************************************************************************************************
bool Verbose::verbMsg(int niveauVerb, string entete, string message)
//---------------------------------------------------------------------------------------------------------------
{
	if(!initVerb){
		cerr<<"Verbose::verbMsg  : verbose non intialisé"<<endl;
		return false;
	}
	int _heures,_minutes,_secondes;
	if(niveauVerb<=lvlVerbose)
	{
		pthread_mutex_lock(&semVerb);
		int ecart_us;
		int ecart_s;
		// Temps passé depuis le lancement
		stringstream StreamLigne;
  		gettimeofday(&actuel, NULL);
		ecart_us = actuel.tv_usec - depart.tv_usec;
		ecart_s = actuel.tv_sec - depart.tv_sec;
		if(ecart_us<0){
			ecart_s--;
			ecart_us = 1000000 + ecart_us;
		};
		_heures = ecart_s / 3600;
		_secondes = ecart_s % 3600;
		_minutes = _secondes / 60;
		_secondes = _secondes % 60;
		StreamLigne<< setfill('0') << setw(2) << _heures<<"h"<< setw(2) <<_minutes<<"mn"<< setw(2) <<_secondes<<"s"
					<< setw(6)<<ecart_us<<"us [ "<<textlv[niveauVerb]  << entete <<" ] " << message ;

		fichier << StreamLigne.str()<< endl;
//		fichier.flush();
		if(logSurEcran){
			cout<< StreamLigne.str()<< endl;
		};
		pthread_mutex_unlock(&semVerb);
	}
	return true;
}

//==============================================================================================================
//
//***************************************************************************************************************
void Verbose::verbEtat(string tramEtat)
//---------------------------------------------------------------------------------------------------------------
{
	pthread_mutex_lock(&semVerb);
	fichier << tramEtat << endl;
	fichier.flush();
	if(logSurEcran){
		cout << tramEtat<< endl;
	}
	pthread_mutex_unlock(&semVerb);
}

//==============================================================================================================
//
//***************************************************************************************************************
void Verbose::setLevelVerbose(int lvel)
//---------------------------------------------------------------------------------------------------------------
{
	lvlVerbose=lvel;
//cout<<"lvlVerbose="<<lvlVerbose<<endl;

}

//==============================================================================================================
//
//***************************************************************************************************************
void Verbose::Verbose::setLogEcran(bool versEcran)
//---------------------------------------------------------------------------------------------------------------
{
	logSurEcran = versEcran;
}

//==============================================================================================================
//
//***************************************************************************************************************
int Verbose::getLevelVerbose()
{
	return lvlVerbose;
}

//==============================================================================================================





