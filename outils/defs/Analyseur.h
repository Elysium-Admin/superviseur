//g++ AnalyseurTrame.cpp ../code/lib/Etats.o -o run2
//g++ AnalyseurTrame.cpp ../code/lib/Etats.o -o run2
#ifndef ANALYSEUR_H
#define ANALYSEUR_H


#include <string>

using namespace std;
class AnalyseurDetat
{

public:
//	AnalyseurDetat(){oldTrame="vide";};
	void analyserTrame(string trame);
	void afficheMesures(string mesures);

private:
	string Trame;
	string oldTrame;
	string LastMesures;
	bool change;
	void interpreteChamp(string champ, int num);

	void _OrgARRU(string champ);
	void _EtatSysteme(string champ);
	void _SeqEnCours(string champ);
	void _Mode(string champ);
	void _EtatSEIS(string champ);
	void _EtatWTS(string champ);
	void _EtatHP3(string champ);
	void _EtatPanneaux(string champ);
	void _EtatPince(string champ);
	void _EtatBras(string champ);
	void _CompteRenduSysteme(string champ);
	void _IncidentPanneaux(string champ);
	void _ErreurSEIS(string champ);
	void _DemIHM(string champ);
	void _CodeErreurBras(string champ);
	void _TensionBateries(string champ);
	void _ChargeBateries(string champ);
	void _CourantBateries(string champ);
	void _ChargeConsommee(string champ);
	void _DureeRestanteBateries(string champ);
	void _IncidentBateries(string champ);

};
#endif //ANALYSEUR_H
