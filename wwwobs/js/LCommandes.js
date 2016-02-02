/* ----------------------------------------------------------------------------
                            Projet Insight

    Nom du fichier : LCommande.js
    Langage : Javascript
    Description : 
        Envoi des commandes de l'IHM vers le serveur
    Organisme : Lycée International Victor Hugo Colomiers, BTS IRIS
    Chef de projet : J.Voyé, JP.Dumas
    Version 0.10 - 04/2013      Auteur : BLASCO Adrien
    Version 0.23 - 02/2015      Auteur : DANTIN Benoit
    Version 0.27 - 02/04/2015   Auteur : JP.Dumas/DANTIN Benoit
        Ajout des commandes de mise à jour des IHM à distance
        c_envoi() modifie 't_cmdIHM' dans 'LStream.js'
    Version 0.31 - 15/04/2015   Auteur : JP.Dumas/DANTIN Benoit
        Ajout de la gestion du film de présentation sur popFilmElysium
-------------------------------------------------------------------------------
 */

/*--------------------------------------------------------------------
 *                  Définition des commandes
 * ------------------------------------------------------------------- 
 */

/* global SIMU */

// --------------------- commandes normales --------------------------
var C_OUVERTURE_PANNEAUX    = "cpo";
var C_FERMETURE_PANNEAUX    = "cpf";	
var C_DEPOSE_SEIS           = "csd";
var C_REPRISE_SEIS          = "csr";
var C_SEIS_LANCER_MESURES   = "csm";
var C_SEIS_ARRETER_MESURES  = "csa";
var C_DEPOSE_WTS            = "cdw";
var C_REPRISE_WTS           = "crw";
var C_DEPOSE_HP3            = "cdh";
var C_REPRISE_HP3           = "crh";
var C_ARRET_SYSTEME         = "cas";

// --------------------- commandes urgentes --------------------------
var U_ARRU                          = "uar";
var U_FIN_ARRU                      = "uaf";
var U_PAS_DE_VERBOSE                = "uv0";
var U_VERBOSE_ERREURS_SEULES        = "uv1";
var U_VERBOSE_ERREURS_ET_ACTIONS    = "uv2";
var U_VERBOSE_DETAILLEE             = "uv3";

// --------------------- commandes spéciales --------------------------
var S_OUVERTURE_PINCE               = "sop";
var S_FERMETURE_PINCE               = "sfp";
var S_BRAS_EN_POSITION_PANORAMIQUE  = "sbp";
var S_SEIS_ARRET_MESURES            = "sam";
var S_SEIS_LANCER_MESURES           = "slm";
var S_SEIS_REPLIER_PIEDS            = "srp";
var S_METTRE_PANNEAUX_HS            = "sph";
var S_SEIS_LANCER_NIVEL             = "sni";

// ------------------- confirmations de l'IHM -------------------------
var CONF_OUVERTURE_PINCE            = "fop";
var CONF_POSE_HP3                   = "fph";
var CONF_LACHER_WTS                 = "flw";
var CONF_SAISIR_WTS                 = "fsw";
var CONF_NIVELL_SEIS                = "fns";
var CONF_LACHER_SEIS                = "fls";
var CONF_SAISIR_SEIS                = "fss";
var CONF_LANCER_TESTS               = "flt";
var CONF_REPRISE_PB_PAN1            = "fr1";
var CONF_REPRISE_PB_PAN2            = "fr2";
var CONF_REMONTER_HP3_POUR_ARRET    = "fha";
var CONF_REMONTER_WTS_POUR_ARRET    = "fwa";
var REMONTER_SEIS_POUR_ARRET        = "fsa";
var CONF_FERMER_PANNEAUX_POUR_ARRET = "fpa";

// ------------------- décisions de l'IHM -----------------------------
var DECISION_BATTERIE_FAIBLES_MAIS_CONTINUER 	= "dbc";
var DECISION_BATTERIE_FAIBLES_DONC_ARRETER 	= "dba";
var DECISION_SORTIE_TESTS_ET_CONTINUER		= "dtc";
var DECISION_SORTIE_TESTS_ET_ARRETER		= "dta";
var DECISION_SORTIE_INITIALISATION_ET_CONTINUER	= "dic";
var DECISION_SORTIE_INITIALISATION_ET_ARRETER	= "dia";
var DECISION_DEFAUT_DETECTES_MAIS_CONTINER	= "ddc";
var DECISION_DEFAUT_DETECTES_DONC_ARRETER	= "dda";
// --------------------------------------------------------------------

var CONFERENCIER    = "confe";
var OBSERVATEUR     = "obser";

// --------------------------------------------------------------------
//              Champ pour écho des action IHM locales
//          00x : correspond à des actions sur l'IHM
//          01x : correspond à l'affichage/fermeture des popup d'info
// --------------------------------------------------------------------
var I_RIEN               = "000";
var I_VOIR_GRAPHE        = "001";
var I_CACHER_GRAPHE      = "002";
var I_VOIR_VIDEO         = "003";
var I_CACHER_VIDEO       = "004";
var I_MONTRE_FILM        = "005";
var I_MONTRE_CAMERA      = "006";
var I_VIDEO_PANNEAUX     = "007";
var I_VIDEO_SEIS         = "008";
var I_VIDEO_WTS          = "009";
var I_VIDEO_HP3          = "00a";
var I_CMD_VIDEO_PANNEAUX = "00b";
var I_CMD_VIDEO_SEIS     = "00c";
var I_CMD_VIDEO_WTS      = "00d";
var I_CMD_VIDEO_HP3      = "00e";
var I_MONTRE_3GRAPHES    = "010";
var I_MONTRE_GRAPHEXYZ   = "011";
var I_RESET_DATA         = "012";
var I_STOP_DATA          = "013";
var I_MARCHE_DATA        = "014";
var I_LANGUE_FR          = "020";
var I_LANGUE_US          = "021";
var I_LANGUE_DE          = "022";
var I_LANGUE_ES          = "023";
var I_POPUP_NASA         = "030";
var I_POPUP_CNES         = "031";
var I_POPUP_FILM         = "033";
var I_POPUP_ACTOULOUSE   = "034";
var I_POPUP_BATTERIE     = "035";
var I_POPUP_FERMETURE    = "036";
var I_VIDEO_PLAY         = "040";
var I_VIDEO_STOP         = "041";
var I_CMD_VIDEO_PLAY     = "042";
var I_CMD_VIDEO_STOP     = "043";

/* --------------------------------------------------------------------
 * Envoi en Ajax de la commande demandée par l'IHM
 * cmd : la commande à envoyer au format textuel, sur 3 caractères
 */ 
function c_envoi(cmd) {
    // on va aussi recevoir cette commande donc on la mémorise pour ne pas
    // la traiter à nouveau dans LStream.js
    if (cmd.charAt(0) === '0') {
        t_cmdIHM = parseInt(cmd, 16);
    }
    $.ajax( {
        url  : (SIMU ? "test/commandes.jsp" : "cgi-bin/cgiCommande.cgi"),
        type : 'GET',                          // car on poste des datas
        data : "cmd=" + cmd,
        dataType: "html"
    } );
}
