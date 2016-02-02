//
//
// g++ playeur.cpp ../lib/Analyseur.o -o playeur
//

#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include "../defs/Analyseur.h"

#define   TAILLE_TAB_BUFFER	103
#define   TUBE	"tube"


using namespace std;

int main()
{
    AnalyseurDetat analyseur;
    int canal;
    canal= open(TUBE, O_RDONLY);
    if (canal == -1) {
        cerr<<"Impossible ouvrir le tube "<<TUBE<<" , Lancer serveur d\'abord?"<<endl;
         return 0;
    }
    cout<<"canal="<<canal<<endl;
    char Buffer[TAILLE_TAB_BUFFER+1];
    string trame;
    Buffer[TAILLE_TAB_BUFFER]=0x00;
    int cmpt;
    while((cmpt=read(canal,Buffer,TAILLE_TAB_BUFFER))>0){
         trame=Buffer;
         analyseur.analyserTrame(trame);
 //	 cout<<trame<<endl;
    }
	cout<<"cmpt="<<cmpt<<endl;
// fermeture  du tube
    close(canal);

    return 0;
}

//==================================================================================================================

