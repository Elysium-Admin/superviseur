#ifndef LISTEMESSAGES_H
#define LISTEMESSAGES_H

#include <string>
#include <vector>

using namespace std;


class Superviseur;

class ListeMessages
{
public:

	ListeMessages ( );
	virtual ~ListeMessages ( );
	void initListe(Superviseur* ptrsup);
	void ajouterMessage (string message );
	bool extraireMessage (string &message );	

protected:
	std::vector<string*> listeMessages;	
	int erreurCode;
	pthread_mutex_t semListeMsg;
	Superviseur* ptrSuperviseur;
};

#endif // LISTEMESSAGES_H
