/*
 * protocole.h
 * Description : Définition des commandes envoyées au serveur
 * 
 * Date derniere modifs 10 decembre 2014; auteur JV
 */

#ifndef PROTOCOLE_H
#define PROTOCOLE_H

// --------------------- commandes normales --------------------------
#define 	C_OUVERTURE_PANNEAUX	"cpo"
#define 	C_FERMETURE_PANNEAUX	"cpf"	
#define		C_DEPOSE_SEIS		"csd"
#define 	C_REPRISE_SEIS		"csr" 
#define 	C_SEIS_LANCER_MESURES	"csm" 
#define 	C_SEIS_ARRETER_MESURES	"csa" 
#define		C_DEPOSE_WTS		"cdw"
#define		C_REPRISE_WTS		"crw"
#define		C_DEPOSE_HP3		"cdh"
#define		C_REPRISE_HP3		"crh"
#define		C_ARRET_SYSTEME		"cas"

// --------------------- commandes urgentes --------------------------
#define 	U_ARRU				"uar"	
#define 	U_FIN_ARRU			"uaf"	
#define   	U_PAS_DE_VERBOSE		"uv0"
#define   	U_VERBOSE_ERREURS_SEULES	"uv1"
#define   	U_VERBOSE_ERREURS_ET_ACTIONS	"uv2"
#define   	U_VERBOSE_DETAILLEE		"uv3"

// --------------------- commandes spéciales --------------------------
#define  	S_OUVERTURE_PINCE		"sop" 
#define   	S_FERMETURE_PINCE		"sfp"
#define   	S_BRAS_EN_POSITION_PANORAMIQUE	"sbp"
#define   	S_SEIS_ARRET_MESURES		"sam"
#define   	S_SEIS_LANCER_MESURES		"slm"
#define   	S_SEIS_REPLIER_PIEDS		"srp"
#define   	S_SEIS_LANCER_NIVEL		"sni"
#define   	S_METTRE_PANNEAUX_HS		"sph"
#define   	S_RINIT_PANNEAUX		"spi"
#define   	S_RELANCER_SUPERVISION		"srs"

// ------------------- confirmations de l'IHM -------------------------
#define 	CONF_OUVERTURE_PINCE	"fop"
#define 	CONF_POSE_HP3		"fph"
#define 	CONF_LACHER_WTS		"flw"
#define 	CONF_SAISIR_WTS		"fsw"
#define 	CONF_NIVELL_SEIS	"fns"
#define 	CONF_LACHER_SEIS	"fls"
#define 	CONF_SAISIR_SEIS	"fss"
#define 	CONF_LANCER_TESTS	"flt"
#define 	CONF_REPRISE_PB_PAN1	"fr1"
#define 	CONF_REPRISE_PB_PAN2	"fr2"
#define 	CONF_REMONTER_HP3_POUR_ARRET	"fha"
#define 	CONF_REMONTER_WTS_POUR_ARRET	"fwa"
#define 	REMONTER_SEIS_POUR_ARRET	"fsa"
#define 	CONF_FERMER_PANNEAUX_POUR_ARRET	"fpa"

// ------------------- décisions de l'IHM -----------------------------
#define 	DECISION_BATTERIE_FAIBLES_MAIS_CONTINUER 	"dbc"
#define 	DECISION_BATTERIE_FAIBLES_DONC_ARRETER 	 	"dba"
#define 	DECISION_SORTIE_TESTS_EST_CONTINUER		"dtc"
#define 	DECISION_SORTIE_TESTS_EST_ARRETER		"dta"
#define 	DECISION_SORTIE_INITIALISATION_EST_CONTINUER	"dic"
#define 	DECISION_SORTIE_INITIALISATION_EST_ARRETER	"dia"
#define 	DECISION_DEFAUT_DETECTES_MAIS_CONTINER		"ddc"
#define 	DECISION_DEFAUT_DETECTES_DONC_ARRETER		"dda"

// --------------------- nature des clients ---------------------------
#define 	CONFERENCIER	"confe"
#define 	OBSERVATEUR	"obser"

#endif //  PROTOCOLE_H
