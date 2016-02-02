//
//
// g++ exTrames.cpp -o exTrames
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

//=========================================================================================================================
int main(int argc, char *argv[])
{

    char *ptrBuffer;
    string ligne,debut,trame;
    fstream fichier(argv[1]);

    while( !fichier.eof()){
	getline(fichier, ligne);
	if(ligne[0]=='e'){
		ptrBuffer=(char *)ligne.c_str();
		cerr<<ligne<<endl;
	};
   };

    return 0;
}

//==================================================================================================================

