//================================================
//Date : 07/04/2015
//Nom du fichier : prgThread.cpp
//Auteur : DA CRUZ Thomas & GAYRAUD Alexandre
//Lieu : Lycée International Victor Hugo Colomiers
//================================================

#include "../defs/progI2C.h"
#include <pthread.h>
#include <sys/types.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../defs/sockets.h"
#include "../defs/LectureI2C.h"
#include <cstdlib>
#include <sstream>
#include <cstring>

using namespace std;

int main(void)
{
//Initialisation des variables
char buffer[40];
char mess[40];
stringstream strStream;
string trame;
string etat1;
string etat2;
string temp1;
string temp2;
int tempCap1;
int tempCap2;
socklen_t fromlen;
int resultrecv = 0;
int resultsendto = 0;
int sock = sockClientUDPspecLoopback();
struct sockaddr_in sin = {0};
sin.sin_port=htons(3200);
fromlen = sizeof sin;

while(1){
    system("clear");
    if(resultsendto = sendto(sock,&buffer,sizeof(buffer), 0, (sockaddr *)&sin,fromlen) < 0)
        {
        cerr<< "Erreur lors du sendto()" << endl;
        }
    if((resultrecv = recvfrom(sock,mess, sizeof(mess), 0, (sockaddr *)&sin, &fromlen)) < 0)
        {
        cerr<< "Erreur lors du recvfrom()" << endl;
        };
    mess[resultrecv]=0x00;
    trame = mess;
    if (trame.length() == 0) trame="A:99°C N:-1°C";
    //On troncature le message afin de récupérer les valeurs nécéssaires
    etat1 = trame.substr(trame.find_first_of(':',0) - 1,1);
    temp1 = trame.substr(1+ trame.find_first_of(':',1),trame.find_first_of('°',1) - 2);
    etat2 = trame.substr(trame.find_first_of(':',2) - 1,1);
    temp2 = trame.substr(1+ trame.find_first_of(':',2),trame.find_first_of('°',2) - 2);

    //Passage des string en int afin de ne pas avoir le symbole ° lors que la valeur
    // de la seconde température est inférieur à 10
    tempCap1 = atoi(temp1.c_str());
    tempCap2 = atoi(temp2.c_str());
    /*
    cout << "etat " << etat1 << endl;
    cout << "temp 1 " << temp1 << endl;
    cout << "etat2 " << etat2 << endl;
    cout << "temp 2 " << temp2 << endl;
    */
    //Affichage des valeurs sur le terminal
    cout<<"\033[1;1H";
    if(etat1 == "A")
        {
        cout << "\033[0;1;36;41mTempérature interieur du Lander: " << tempCap1 << " °C \033[40;37;0m" << endl;
        }
    else
        {
        cout << "\033[0;30;42mTempérature interieur du Lander: " << tempCap1 << " °C \033[40;37;0m" << endl;
        }
    if(etat2 == "A")
        {
        cout << "\033[0;1;36;41mTempérature WIFI sur Lander : " << tempCap2 << " °C \033[40;37;0m" << endl;
        }
    else
        {
        cout << "\033[0;30;42mTempérature du WIFI sur Lander : " << tempCap2 << " °C \033[40;37;0m" << endl;
        }
sleep(20);
cout << "\033[1;1H\033[K\033[1B\033[K";
    }

return 0;
}

