/* 
 * elysiumPublic.js
 * 
 * Scripts en Javascript permettant le dynamisme du site INSIGHT
 * 
 * Syntaxe des sélecteurs $() en JQuery
 * http://www.w3schools.com/jquery/jquery_ref_selectors.asp
 * 
 * Pour tester le JS : http://jsfiddle.net + jQuery 1.11.0 + jQuery Mobile 1.4.4
 * 
 * Utilise 'CBatterie.js' & 'CIhm.js'
 * 
 * Version 0.27 - 02/04/2015    Auteur : BUGE Pascal / CAMPI Enzo
 *    Création de la version publique
 *    
 * Version 0.30 - 08/04/2015    Auteur : BUGE Pascal / CAMPI Enzo
 *    Ajout de 2 langues : Espagnol et Allemand
 *    Ajout de la vidéo et des séquences vidéo de dépose.
 *    Amélioration du CSS pour le placement à l'horizontale
 *
 * Version 0.31 - 15/04/2015    Auteur : BUGE Pascal
 *    Ajout du film de présentation sur popFilmElysium
 *    
 */

// déclaration de constantes globales pour Netbeans
/* global CONF_OUVERTURE_PINCE, CONF_POSE_HP3, CONF_LACHER_WTS, CONF_SAISIR_WTS, I_POPUP_FERMETURE, CONF_LANCER_TESTS, I_STOP_DATA, I_MARCHE_DATA, I_CHOISIR_3GRAPHES, I_CHOISIR_GRAPHEXYZ, I_CACHER_GRAPHE, I_VOIR_GRAPHE, I_CACHER_VIDEO, I_VOIR_VIDEO, I_RESET_DATA, CONF_NIVELL_SEIS, CONF_LACHER_SEIS, CONF_SAISIR_SEIS, CONF_REPRISE_PB_PAN1, CONF_REPRISE_PB_PAN2, CONF_REMONTER_HP3_POUR_ARRET, CONF_REMONTER_WTS_POUR_ARRET, REMONTER_SEIS_POUR_ARRET, CONF_FERMER_PANNEAUX_POUR_ARRET, DECISION_BATTERIE_FAIBLES_MAIS_CONTINUER, DECISION_BATTERIE_FAIBLES_DONC_ARRETER, DECISION_SORTIE_TESTS_ET_CONTINUER, DECISION_SORTIE_TESTS_ET_ARRETER, DECISION_SORTIE_INITIALISATION_ET_CONTINUER, DECISION_SORTIE_INITIALISATION_ET_ARRETER, DECISION_DEFAUT_DETECTES_MAIS_CONTINER, DECISION_DEFAUT_DETECTES_DONC_ARRETER, C_OUVERTURE_PANNEAUX, C_FERMETURE_PANNEAUX, C_DEPOSE_HP3, C_REPRISE_HP3, C_DEPOSE_SEIS, C_REPRISE_SEIS, C_DEPOSE_WTS, C_REPRISE_WTS, C_ARRET_SYSTEME, U_ARRU, U_FIN_ARRU, I_LANGUE_US, I_LANGUE_DE, I_LANGUE_ES, I_LANGUE_FR, I_POPUP_NASA, I_POPUP_CNES, I_POPUP_INSIGHT, I_POPUP_ACTOULOUSE, I_POPUP_BATTERIE, I_POPUP_VIDEO */

// ----------------- Déclaration de variable globales -------------------------
var SIMU = false;                // si on est sur le simulateur, false sinon

var PANNEAUX = 0;
var HP3 = 1;
var SEIS = 2;
var WTS = 3;
var PANNEAUX_O = 0;
var PANNEAUX_F = 1;
var HP3_D = 2;
var HP3_R = 3;
var SEIS_D = 4;
var SEIS_R = 5;
var WTS_D = 6;
var WTS_R = 7;
var QUITTER = 8;
var START = 2;
var STOP = 3;
var adrCamera = "http://10.5.128.3/ipcam/mjpeg.cgi";
var adrImage  = "images/lander.jpg";

var ihm = new CIhm();           // état fictif pour tester la classe 'CIhm'
var batterie = new CBatterie(); // pour tester la classe 'Batterie'
// ----------------------------------------------------------------------------

/**
 * ---------- Fonction exécutée après le chargement de la page ----------------
 */ 
$(function() {
    $( document ).ready(init);      // lancé quand le DOM est initialisé (prêt)
    // -------------------------- main page -----------------------------------
    // clicks d'infos sur les icones du HEADER
    $("#divBatterie").click(function() { openPopupInfo("popInfoBatterie"); });
    // ----- fermeture des popup d'info liées aux icones du HEADER ------------
    $("div[id^='popInfo']").click(function() { ihm.closePopup();});
    
    $("#imgLangue").click(function() { ihm.openPopup("popLangue"); });
    $("#divBatterie").click(function() { openPopupInfo("popInfoBatterie"); });
    
    // --------------- gestion des popups de vidéo/graphes --------------------
    $("#imgCamera").click(function() { imgCamera(); });
    $("#divGraphe").click(function() { divGraphe();});
    
    $("#btnPopCameraChangeVue").click(function() { btnPopVideoChangeVue(); });
    $("#btnPopCameraShowGraphe").click(function() { btnPopShowGraphe(); });
    $("#btnPopChangeVue").click(function() { btnPopChangeVue(); });
    $("#btnPopGrapheReset").click(function() { btnPopGrapheReset(); });
    $("#btnPopGrapheStart").click(function() { btnPopGrapheStart(); });
    $("#btnPopHideVideo").click(function() { btnPopShowVideo(); });
    
    // -------------------- gestion des langues -------------------------------
    $("#btnPopFrancais").click(function() { setLangue('fr'); });
    $("#btnPopAnglais").click(function() { setLangue('us'); });
    $("#btnPopEspagnol").click(function() { setLangue('es'); });
    $("#btnPopAllemand").click(function() { setLangue('de'); });
});

/**
 * fonction d'initialisation de l'IHM relancée à chaque actualisation de la page
 */ 
function init() {
    // empèche la sélection de texte par touch/drag (important)
    document.onselectstart = new Function ("return false");
    
    // initialisation des header, footer et popup globaux
    $("[data-role='header'], [data-role='footer']").toolbar();
    // ajout de data-tap-toggle="false" pour bloquer la visibilité du 'footer'
    $("[data-role='popup']").popup().enhanceWithin();  
    
    // installation de la connexion SSE
    sseConnexion();
    
    ml_setLangue("fr");
    //ihm.cacher("divBatterie");
    montrerVideo();                 // au départ on ne voit que la vidéo
}

function openPopupInfo(id) {
    ihm.openPopup(id);
}

//gestion locale
function btnPopGrapheStart() {          // start ou stop le graphique
    ihm.closePopup();   
    var etat = ihm.getEtat("btnPopGrapheStart");
    if (etat === 0)                     // le graphe est actif
        grapheStop();
    else
        grapheStart();
}

function btnPopVideoChangeVue() {   // montre la vidéo de la caméra ou la présentation
    ihm.closePopup();
    montreCamera();
}

function btnPopChangeVue() {    // changement de vue entre les graphiques
    ihm.closePopup();
    if ($("#div3Graphes").hasClass("hide")) 
        montreVue3Graphes();
    else
        montreVueGrapheXYZ();
}

function btnPopShowGraphe() {   // montre le graphique
    ihm.closePopup();
    montrerGraphe();
}

function btnPopShowVideo() {    // affiche la vidéo
    ihm.closePopup();
    montrerVideo();
}

function btnPopGrapheReset() {  // reset les données du graphique
    ihm.closePopup();
    g_resetDatas();
}

function setLangue(code) {  // définit la langue
    ihm.closePopup();
    ml_setLangue(code); 
}

function montrerGraphe() {  // cache la caméra et affiche le graphique
    ihm.cacher("imgCamera");
    ihm.montrer("divGraphe");
    g_refresh();            // pour avoir de suite le bon format des graphes
    ihm.setEtat("btnPopCameraShowGraphe", 0);
}

function montrerVideo() {   //cache le graphique et affiche la vidéo
    ihm.cacher("divGraphe");
    ihm.setEtat("btnPopCameraShowGraphe", 1);
    ihm.montrer("imgCamera");
    ml_updateEtat();
}

function grapheStop() {     // met en pause le graphique
    g_pause();
    // cacher le bouton de reset, pas de reset possible en pause
    ihm.cacher("btnPopGrapheReset");
    ihm.setEtat("btnPopGrapheStart", 1);    // on l'arrête
    ml_updateEtat();
}

function grapheStart() {    // démarre le graphique
    g_start();
    ihm.montrer("btnPopGrapheReset");       // montrer le bouton de reset
    ihm.setEtat("btnPopGrapheStart", 0);    // on le relance
    ml_updateEtat();
}

function montreVue3Graphes() {  // montre les trois graphiques
    ihm.montrer("div3Graphes");
    ihm.cacher("divGrapheXYZ");
    g_refresh();
}

function montreVueGrapheXYZ() { // montre le graphique réuni
    ihm.montrer("divGrapheXYZ");
    ihm.cacher("div3Graphes");
    g_refresh();
}

function imgCamera() {
    ihm.openPopup("popCamera");
}

function video() {
    ihm.openPopup("popVideo");
}

function divGraphe() {
    ihm.openPopup("popGraphes");
}
