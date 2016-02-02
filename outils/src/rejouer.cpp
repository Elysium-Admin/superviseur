//
//
// g++ rejouer.cpp -l pthread -o rejouer
//

#include <string.h>
#include <termios.h>
#include <pthread.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define   TAILLE_TAB_BUFFER	103
#define   TUBE	"tube"

using namespace std;

// varaibles globales
//--------------------
char cmd=0x20;
bool charbon;
pthread_mutex_t semClavier;
pthread_cond_t frappeClavierFaite;


//==================================================================================================================
char frpclav()
{
	static struct termios term, back;
	char ret=0xff;

	tcgetattr (0, &term); /* On recupere les info du terminal */
	memcpy (&back, &term, sizeof(term));

	term.c_lflag &= ~(ICANON|ECHO); /* On modifie les options du terminal */
	term.c_cc[VTIME] = 0; /* Temps d'attente infini */
	term.c_cc[VMIN] = 1; /* Nombre minimun de caractere ds le tampon pr débloquer la saisie*/

	tcsetattr(0, TCSANOW, &term); /* Modif des attribut de stdin */
	ret = getchar(); /* Attente d'un caractere */
	tcsetattr(0, TCSANOW, &back); /* Restauration des attributs */
        return ret;
};

//=========================================================================================================================
void analyserfrappe()
{
    char car;
    bool encore=true;
    cout<<"analyse frappe lancée"<<endl;
    while(encore){
	car =frpclav();
//	cout<<"<cmd=0x"<<hex<<(int)car<<endl;
	switch (car){
		case 0x46 :
		case 0x20 :
		case 0x42 :
		case 'q':
			cmd=car;
			pthread_cond_signal (&frappeClavierFaite);
			break;
	default :
//			cout<<"<cmd=0x"<<hex<<(int)car<<endl;
			break;
	}
    }
}

//=========================================================================================================================
//activité du thread
//------------------
void *commandesClavier(void*)
{
    analyserfrappe();
}

//=========================================================================================================================
int main(int argc, char *argv[])
{
    pthread_mutex_init(&semClavier, NULL);
    void *retour; /* utilisé pour la terminaison des thread */
    pthread_t saisieClavier;
    if (pthread_create (&saisieClavier, NULL, commandesClavier, NULL) < 0)
    {
        cerr<<"ERR Impossible de lancer le thread 'saisieClavier'"<<endl;
        return 0;
    };

    int versPlayeur;
    unlink(TUBE);
/* Creation fifos */
    if (mkfifo(TUBE, 0644) == -1) {
        cerr<<"Impossible creer fifos"<<endl;;
        return 0;
    }
    versPlayeur = open(TUBE, O_WRONLY);

    char *ptrBuffer;
    string ligne,debut,trame;
    fstream fichier(argv[1]);
    cout << "\033[1;1H"; // positionnement 1ere ligne 1ere colonne
    cout<<"\033[2J"; //effacer l'écran
    cout<< "Menu des commandes"<<endl;
    cout<< "------------------"<<endl;
    cout<<"appuyer sur  : la touche espace pour avancer d'un pas"<<endl;
    cout<<"             : la touche  <fleche vers le bas> pour défiler automatiquement jusqu'à l'état suivant"<<endl;
    cout<<"             : la  touche fin  pour défiler automatiquement  jusqu'à la fin du fichier"<<endl;
    cout<<"             : la  touche 'q'  quitter le programme"<<endl;
    cout<<"==========================================================================================================="<<endl;

    while( (!fichier.eof()) && (cmd != 'q') ){
	getline(fichier, ligne);
	cout<<ligne<<endl;
	if(ligne[0]=='e'){
		ptrBuffer=(char *)ligne.c_str();
	        write(versPlayeur,ptrBuffer,TAILLE_TAB_BUFFER);
	}else{
//		cout<<ligne<<endl;
	};
	switch (cmd){
		case 0x46 :
			usleep(500000);
			;break;
		case 0x20 :
			charbon=true;
			pthread_mutex_lock(&semClavier);
			pthread_cond_wait (&frappeClavierFaite, &semClavier);
			pthread_mutex_unlock(&semClavier);
			;break;
		case 0x42 :
			if(ligne[0]=='e'){
			    pthread_mutex_lock(&semClavier);
			    pthread_cond_wait (&frappeClavierFaite, &semClavier);
			    pthread_mutex_unlock(&semClavier);
			}else{
			    usleep(500000);
			}
 			break;
		case 'q' : charbon=false;
			   return 0;
			break;
	}
    };
// fermeture et liberation du tube
    close(versPlayeur);
    unlink(TUBE);

    (void)pthread_join (saisieClavier, &retour);

    return 0;
}

//==================================================================================================================

