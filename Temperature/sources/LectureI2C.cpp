//================================================
//Date : 07/04/2015
//Nom du fichier : LectureI2C.cpp
//Auteur : DA CRUZ Thomas & GAYRAUD Alexandre
//Lieu : Lycée International Victor Hugo Colomiers
//================================================

#include "../defs/LectureI2C.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

valTemp lectureI2C(){

	//Initialisation des variables
	int file;
	valTemp v;
	v.temp1 = 10;
	v.temp2 = 60;
	char filename[] = "/dev/i2c-1";
	float temp2;
	if ((file = open(filename, O_RDWR)) < 0)
	{
		perror("Echec de l'ouverture du bus I2C");
		return v;
	}
	//Initialisation de la communication
	int addr1 = 0x1C;
	if (ioctl(file, I2C_SLAVE, addr1) < 0)
	{
		printf("Echec lors de l'acquisition d'un accès bus et/ou de 		communication. \n");
		return v;
	}
	//Lecture sur le bus
	unsigned char buf[10] = {0};
	buf[0] = 5; 
	//Lancement de la mesure
	write(file,buf,1);

	if (read(file,buf,2) !=2)
	{
		printf("Echec de lecture depuis le bus I2C : %s. \n", strerror(errno));
	}
	else
	{
		//Suppresion des flags 
		buf[0] = buf[0]&0x1F;
		/**********Passage en décimal*************/
		//Met la température dans temp1
		if ((buf[0] & 0x10) == 0x10){
			buf[0] = buf[0] & 0x0F; //Nettoyer le flag SIGN
			v.temp1 = 256 - (buf[0] * 16 + buf[1] / 16);
		}
		else
		{
			v.temp1 = (buf[0] * 16 + buf[1] / 16);
			printf("Température LectureI2C.cpp : %d\n",v.temp1);
		}
	}
	//Second capteur
	int addr2 = 0x1A;
	if (ioctl(file, I2C_SLAVE, addr2) < 0)
	{
		printf("Echec lors de l'acquisition d'un accès bus et/ou de 		communication. \n");
		return v;
	}
	//Lecture sur le bus
	unsigned char buf1[10] = {0};
	buf1[0] = 5; 
	//Lancement de la mesure
	write(file,buf1,1);

	if (read(file,buf1,2) !=2)
	{
		printf("Echec de lecture depuis le bus I2C : %s. \n", strerror(errno));
	}
	else
	{
		//Suppresion des flags 
		buf1[0] = buf1[0]&0x1F;
		/**********Passage en décimal*************/
		//Met la température dans temp1
		if ((buf1[0] & 0x10) == 0x10){
			buf1[0] = buf1[0] & 0x0F; //Nettoyer le flag SIGN
			v.temp2 = 256 - (buf1[0] * 16 + buf1[1] / 16);
		}
		else
		{
			v.temp2 = (buf1[0] * 16 + buf1[1] / 16);
		}
		printf("La température est de %d°C \n",v.temp2);
	}
	return v;
}

