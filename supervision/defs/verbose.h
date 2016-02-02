#ifndef VERBOSE_H
#define VERBOSE_H

#include <iostream>
#include <string>
#include <ctime>
#include <cstdio>
#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#define NB_HISTO_LOG	10

using namespace std;


class Verbose
{

public:

	int maxLignes;

	Verbose();
	void init(int level, bool surEcran, string chemin, int nbHist=NB_HISTO_LOG);
	void init();
	bool verbMsg(int niveauVerb, string entete, string message);
	void verbEtat(string tramEtat);
	void setLevelVerbose(int lvel);
	int getLevelVerbose();
	void setLogEcran(bool versEcran);
	void setCheminLog(string chemin);
	void setNbHistoriques(int nbHist);
	enum NIVEAU_VERBOSE
	{
		RIEN 	= 0x0,
		ERREUR 	= 0x1,
		INFO	= 0x2,
		DBG 	= 0x3
	};

protected:
	string cheminLog;
	int lvlVerbose;
	bool initVerb;
	bool logSurEcran;
	ofstream fichier;
	int nombreDhistoriques;
//  ifstream log_1;
	pthread_mutex_t semVerb;
	struct timeval depart;
	struct timeval actuel;
	string textlv[4];
};

#endif

