/* 
 * Nom : elysium.js
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
 * Version 0.23 - 02/2015       Auteur : BUGE Pascal
 *      Ajout de l'état de la pince
 *      Déplacement de l'icône batterie dans l'entête
 *      Correction bug incohérence entre état des sous-systèmes et les boutons
 * 
 * Version 0.27 - 02/04/2015    Auteur : BUGE Pascal / DANTIN Benoit
 *    Ajout des commandes de mise à jour des IHM à distance
 *    
 * Version 0.30 - 08/04/2015    Auteur : BUGE Pascal / CAMPI Enzo / Professeurs de langue
 *    Ajout de 2 langues : Espagnol et Allemand
 *    Ajout de la vidéo et des séquences vidéo de dépose.
 *    
 * Version 0.31 - 15/04/2015    Auteur : CAMPI Enzo
 *    Ajout du film de présentation sur popFilmElysium
 *    
 * Version beta3 - 25/04/2015    Auteur : BUGE Pascal
 *    Ajout d'un panel de dépannage sur la popup de la Nasa
 *    
 * Version beta6 - 25/05/2015    Auteur : BUGE Pascal
 *    Ajout de la gestion de imgEtatCamera
 */

// déclaration de constantes globales pour Netbeans
/* global CONF_OUVERTURE_PINCE, CONF_POSE_HP3, CONF_LACHER_WTS, CONF_SAISIR_WTS, I_POPUP_FERMETURE, CONF_LANCER_TESTS, I_STOP_DATA, I_MARCHE_DATA, I_CHOISIR_3GRAPHES, I_CHOISIR_GRAPHEXYZ, I_CACHER_GRAPHE, I_VOIR_GRAPHE, I_CACHER_VIDEO, I_VOIR_VIDEO, I_RESET_DATA, CONF_NIVELL_SEIS, CONF_LACHER_SEIS, CONF_SAISIR_SEIS, CONF_REPRISE_PB_PAN1, CONF_REPRISE_PB_PAN2, CONF_REMONTER_HP3_POUR_ARRET, CONF_REMONTER_WTS_POUR_ARRET, REMONTER_SEIS_POUR_ARRET, CONF_FERMER_PANNEAUX_POUR_ARRET, DECISION_BATTERIE_FAIBLES_MAIS_CONTINUER, DECISION_BATTERIE_FAIBLES_DONC_ARRETER, DECISION_SORTIE_TESTS_ET_CONTINUER, DECISION_SORTIE_TESTS_ET_ARRETER, DECISION_SORTIE_INITIALISATION_ET_CONTINUER, DECISION_SORTIE_INITIALISATION_ET_ARRETER, DECISION_DEFAUT_DETECTES_MAIS_CONTINER, DECISION_DEFAUT_DETECTES_DONC_ARRETER, C_OUVERTURE_PANNEAUX, C_FERMETURE_PANNEAUX, C_DEPOSE_HP3, C_REPRISE_HP3, C_DEPOSE_SEIS, C_REPRISE_SEIS, C_DEPOSE_WTS, C_REPRISE_WTS, C_ARRET_SYSTEME, U_ARRU, U_FIN_ARRU, I_LANGUE_US, I_LANGUE_DE, I_LANGUE_ES, I_LANGUE_FR, I_POPUP_NASA, I_POPUP_CNES, I_POPUP_INSIGHT, I_POPUP_ACTOULOUSE, I_POPUP_BATTERIE, I_POPUP_FILM, I_VIDEO_SEIS, I_VIDEO_HP3, I_VIDEO_WTS, I_VIDEO_PANNEAUX, I_MONTRE_CAMERA, I_MONTRE_FILM, I_MONTRE_3GRAPHES, I_MONTRE_GRAPHEXYZ, I_CMD_VIDEO_PANNEAUX, vidPopFilm, I_CMD_VIDEO_SEIS, I_CMD_VIDEO_WTS, I_CMD_VIDEO_HP3, t_CamHS, S_OUVERTURE_PINCE, S_SEIS_ARRET_MESURES, S_SEIS_LANCER_MESURES, S_SEIS_REPLIER_PIEDS, S_METTRE_PANNEAUX_HS, I_VIDEO_STOP, I_VIDEO_PLAY, I_CMD_VIDEO_STOP, I_CMD_VIDEO_PLAY, S_SEIS_LANCER_NIVEL */

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
var adrImage = "images/lander.jpg";
var timerVideo;

var ihm = new CIhm();           // état fictif pour tester la classe 'CIhm'
var batterie = new CBatterie(); // pour tester la classe 'Batterie'
// ----------------------------------------------------------------------------

/**
 * ---------- Fonction exécutée après le chargement de la page ----------------
 */
$(function () {
    $(document).ready(init);      // lancé quand le DOM est initialisé (prêt)
    //
    $("#btnMain_P1").click(function () {
        btnMain_P1();
    });
    $("#btnQuitter_P1").click(function () {
        btnQuitter_P1();
    });
    // -------------------------- main page -----------------------------------
    // clicks d'infos sur les icones du HEADER
    $("#imgNasa").click(function () {
        openPopupInfo("popInfoNasa");
    });
    $("#imgCnes").click(function () {
        openPopupInfo("popInfoCnes");
    });
    $("#imgElysium").click(function () {
        imgElysium();
    });
    $("#imgAcToulouse").click(function () {
        openPopupInfo("popInfoAcToulouse");
    });
    $("#divBatterie").click(function () {
        openPopupInfo("popInfoBatterie");
    });
    // ----- fermeture des popup d'info liées aux icones du HEADER ------------
    $("div[id^='popInfo']").click(function () {
        ihm.closePopup();
    });
    $("#popFilmElysium").click(function () {
        popFilmElysium();
    });
    // fermeture des popups non modales
    $("div[id^='popInfo']").on("popupafterclose", function () {
        c_envoi(I_POPUP_FERMETURE);
    });
    $("#popFilmElysium").on("popupafterclose", function () {
        ihm.cacher("cmdFilm");          // on cache le menu contextuel
        c_envoi(I_POPUP_FERMETURE);
    });

    $("#imgLangue").click(function () {
        ihm.openPopup("popLangue");
    });
    
    $("#btnPanneaux").click(function () {
        btnCommande(PANNEAUX);
    });
    $("#btnSEIS").click(function () {
        btnCommande(SEIS);
    });
    $("#btnWTS").click(function () {
        btnCommande(WTS);
    });
    $("#btnHP3").click(function () {
        btnCommande(HP3);
    });
    $("#btnQuitter").click(function () {
        btnCommande(QUITTER);
    });

    // --------------- gestion des popups de commandes ------------------------

    $("#btnPopCommandeOK").click(function () {
        btnPopCommande(true);
    });
    $("#btnPopCommandeNO").click(function () {
        btnPopCommande(false);
    });

    $("#btnPopConfirm").click(function () {
        btnPopConfirm();
    });
    
    $("#btnPopPbNivellement").click(function () {
        btnPopPbNivellement();
    });
    
    $("#btnPopRepliPieds").click(function () {
        btnPopRepliPieds();
    });
    
    $("#btnPopNivellement").click(function () {
        btnPopNivellement();
    });
    
    $("#btnPopDecisionOK").click(function () {
        btnPopDecision(true);
    });
    $("#btnPopDecisionNO").click(function () {
        btnPopDecision(false);
    });
    
    // ----------- debut beta4 -----------
    $("#imgOutils").click(function () { 
        btnDepannage(); 
    });
    
    $("#btnDepannageDecisionOK").click(function () { 
        btnDepannageDecision(true); 
    });
    
    $("#btnDepannageDecisionNO").click(function () { 
        btnDepannageDecision(false); 
    });
    // ----------- fin beta4 -----------

    // --------------- gestion des popups de vidéo/graphes --------------------
    $("#imgCamera").click(function () {
        imgCamera();
    });
    $("#vidFilm").click(function () {
        vidFilm();
    });
    $("#btnPopCameraChangeVue").click(function () {
        btnPopVideoChangeVue();
    });
    $("#btnPopCameraShowGraphe").click(function () {
        btnPopVideoShowGraphe();
    });
    $("#btnPopCameraHideVideo").click(function () {
        btnPopVideoHideVideo();
    });
    $("#divGraphe").click(function () {
        divGraphe();
    });
    $("#btnPopHideVideo").click(function () {
        btnPopHideVideo();
    });
    $("#btnPopVideoHideVideo").click(function () {
        btnPopVideoHideVideo();
    });
    $("#btnPopVideoChangeVue").click(function () {
        btnPopVideoChangeVue();
    });
    $("#btnPopVideoPanneaux").click(function () {
        btnPopVideoPanneaux(true);
    });
    $("#btnPopVideoSEIS").click(function () {
        btnPopVideoSEIS(true);
    });
    $("#btnPopVideoWTS").click(function () {
        btnPopVideoWTS(true);
    });
    $("#btnPopVideoHP3").click(function () {
        btnPopVideoHP3(true);
    });

    $("#btnCmdVideoPanneaux").click(function () {
        btnPopVideoPanneaux(false);
    });
    $("#btnCmdVideoSEIS").click(function () {
        btnPopVideoSEIS(false);
    });
    $("#btnCmdVideoWTS").click(function () {
        btnPopVideoWTS(false);
    });
    $("#btnCmdVideoHP3").click(function () {
        btnPopVideoHP3(false);
    });

    $("#btnPopVideoShowGraphe").click(function () {
        btnPopVideoShowGraphe();
    });
    $("#btnPopHideGraphe").click(function () {
        btnPopHideGraphe();
    });
    $("#btnPopChangeVue").click(function () {
        btnPopChangeVue();
    });
    $("#btnPopGrapheReset").click(function () {
        btnPopGrapheReset();
    });
    $("#btnPopGrapheStart").click(function () {
        btnPopGrapheStart();
    });

    // -------------------- gestion des langues -------------------------------
    $("#btnPopFrancais").click(function () {
        setLangue('fr');
    });
    $("#btnPopAnglais").click(function () {
        setLangue('us');
    });
    $("#btnPopEspagnol").click(function () {
        setLangue('es');
    });
    $("#btnPopAllemand").click(function () {
        setLangue('de');
    });

    // -------------------- gestion de l'ARU ----------------------------------
    $("#btnARU").click(function () {
        btnARU();
    });
    $("#btnARU_P3").click(function () {
        btnARU();
    });
    $("#btnARU_P5").click(function () {
        btnARU();
    });
    $("#btnPopAruOK").click(function () {
        btnPopAruOK();
    });
    
    // -------------------- gestion de la video -------------------------------
    $("#vidFilm").on("playing", function () { 
        c_envoi(I_VIDEO_PLAY);
    });
    
    $("#vidPopFilm").on("playing", function () {
        c_envoi(I_CMD_VIDEO_PLAY);
    });
    
    $("#vidFilm").on("pause", function () {
        c_envoi(I_VIDEO_STOP); 
        clearInterval(timerVideo);
    });

    $("#vidPopFilm").on("pause", function () { 
        c_envoi(I_CMD_VIDEO_STOP); 
        clearInterval(timerVideo);
    });
});

/**
 * fonction d'initialisation de l'IHM relancée à chaque actualisation de la page
 */
function init() {
    // empèche la sélection de texte par touch/drag (important)
    document.onselectstart = new Function("return false");

    // initialisation des header, footer et popup globaux
    $("[data-role='header'], [data-role='footer']").toolbar();
    // ajout de data-tap-toggle="false" pour bloquer la visibilité du 'footer'
    $("[data-role='popup']").popup().enhanceWithin();

    // installation de la connexion SSE
    sseConnexion();

    ml_setLangue("fr");           // définit le français comme langue de base
    cacherGraphe();               // au départ on ne voit que la vidéo
    ihm.disableButton("btnWTS");  // seul ce bouton n'est pas accessible
    ihm.cacher("divBatterie");    // au départ la batterie n'est pas initialisée
    ihm.cacher("imgEtatCamera");  // au départ la caméra n'est pas initialisée

}

// permet d'afficher la vidéo de présentation pour la page4
function popFilmElysium() {         
    if (ihm.isCache("cmdFilm"))
        ihm.montrer("cmdFilm");
    else
        ihm.cacher("cmdFilm");
}

// permet d'afficher la vidéo de présentation, sauf sur la page4
function imgElysium() {
    if ($('#page4').is(':hidden')) { // si on est pas sur la page principale
        openPopupInfo("popFilmElysium");
    }
}

// Page1 : permet de lancer les tests
function btnMain_P1() {
    c_envoi(CONF_LANCER_TESTS);
}

// Page1 : montre la page 6 pour quitter l'application
function btnQuitter_P1() {
    ihm.montrerPage(6);
}

// Permet ds une popup de voir la partie de la vidéo traitant les panneaux
function btnPopVideoPanneaux(close) {   
    if (close) {
        ihm.closePopup();
        c_envoi(I_VIDEO_PANNEAUX);
        videoPanneaux(true);
    }
    else {
        c_envoi(I_CMD_VIDEO_PANNEAUX);
        videoPanneaux(false);
    }
}

// Permet ds une popup de voir la partie de la vidéo traitant du SEIS
function btnPopVideoSEIS(close) {  
    if (close) {
        ihm.closePopup();
        c_envoi(I_VIDEO_SEIS);
        videoSEIS(true);
    }
    else {
        c_envoi(I_CMD_VIDEO_SEIS);
        videoSEIS(false);
    }
}

// Permet ds une popup de voir la partie de la vidéo traitant du WTS
function btnPopVideoWTS(close) {  
    if (close) {
        ihm.closePopup();
        c_envoi(I_VIDEO_WTS);
        videoWTS(true);
    }
    else {
        c_envoi(I_CMD_VIDEO_WTS);
        videoWTS(false);
    }
}

// Permet ds une popup de voir la partie de la vidéo traitant du HP3
function btnPopVideoHP3(close) {   
    if (close) {
        ihm.closePopup();
        c_envoi(I_VIDEO_HP3);
        videoHP3(true);
    }
    else {
        c_envoi(I_CMD_VIDEO_HP3);
        videoHP3(false);
    }
}

// ouverture d'une popup avec envoi de l'info sur la trame des commandes
// pour écho sur l'autre PC
// id : id de la popup à ouvrir
function openPopupInfo(id) {
    if (id === "popInfoNasa")
        c_envoi(I_POPUP_NASA);
    else if (id === "popInfoCnes")
        c_envoi(I_POPUP_CNES);
    else if (id === "popFilmElysium")
        c_envoi(I_POPUP_FILM);
    else if (id === "popInfoAcToulouse")
        c_envoi(I_POPUP_ACTOULOUSE);
    else if (id === "popInfoBatterie")
        c_envoi(I_POPUP_BATTERIE);
    ihm.openPopup(id);
}

// gestion locale
function btnPopGrapheStart() {        // start ou stop le graphiques
    ihm.closePopup();
    var etat = ihm.getEtat("btnPopGrapheStart");
    if (etat === 0) {                           // le graphe est actif
        c_envoi(I_STOP_DATA);
        grapheStop();
    }
    else {
        c_envoi(I_MARCHE_DATA);
        grapheStart();
    }
}

function btnPopVideoChangeVue() {   // montre la vidéo ou la présentation
    ihm.closePopup();
    if (ihm.isCache("imgCamera")) {
        c_envoi(I_MONTRE_CAMERA);
        montreCamera();
    }
    else {
        c_envoi(I_MONTRE_FILM);
        montreFilm();
    }
}

function btnPopChangeVue() {        // changement de vue entre les graphiques
    ihm.closePopup();
    if (ihm.isCache("div3Graphes")) {
        c_envoi(I_MONTRE_3GRAPHES);
        montreVue3Graphes();
    }
    else {
        c_envoi(I_MONTRE_GRAPHEXYZ);
        montreVueGrapheXYZ();
    }
}

function btnPopVideoShowGraphe() {        // gestion locale
    ihm.closePopup();
    var etat = ihm.getEtat("btnPopVideoShowGraphe");
    if (etat === 0) {
        c_envoi(I_CACHER_GRAPHE);
        cacherGraphe();
    }
    else {
        c_envoi(I_VOIR_GRAPHE);
        montrerGraphe();
    }
}

function btnPopHideGraphe() {        // masque le graphique
    ihm.closePopup();
    c_envoi(I_CACHER_GRAPHE);
    cacherGraphe();
}

function btnPopHideVideo() {       // masque ou montre la vidéo
    ihm.closePopup();
    var etat = ihm.getEtat("btnPopHideVideo");
    if (etat === 0) {
        c_envoi(I_CACHER_VIDEO);
        cacherVideo();
    }
    else {
        c_envoi(I_VOIR_VIDEO);
        montrerVideo();
    }
}

function btnPopVideoHideVideo() {
    ihm.closePopup();
    c_envoi(I_CACHER_VIDEO);
    cacherVideo();
}

function btnPopGrapheReset() {        // reset les données du graphique
    ihm.closePopup();
    c_envoi(I_RESET_DATA);
    g_resetDatas();
}

function setLangue(code) {  // définit la langue 
    ihm.closePopup();
    if (code === "fr")
        c_envoi(I_LANGUE_FR);
    else if (code === "us")
        c_envoi(I_LANGUE_US);
    else if (code === "de")
        c_envoi(I_LANGUE_DE);
    if (code === "es")
        c_envoi(I_LANGUE_ES);
    ml_setLangue(code);
    // on déplace certains messages (panneaux)
    if (code === "fr") {
        $("#msgPanneau1").css({left: "125px"});
        $("#msgPanneau2").css({left: "125px"});
        $("#lblSEIS").css({left: "440px"});
        $("#msgSEIS").css({left: "500px"});
        $("#lblWTS").css({left: "440px"});
        $("#msgWTS").css({left: "500px"});
    }
    else {
        $("#msgPanneau1").css({left: "90px"});
        $("#msgPanneau2").css({left: "90px"});
        $("#lblSEIS").css({left: "400px"});
        $("#msgSEIS").css({left: "460px"});
        $("#lblWTS").css({left: "400px"});
        $("#msgWTS").css({left: "460px"});
    }
}

function montreCamera() {   // cache le film et montre la vidéo de la caméra
    ihm.cacher("vidFilm");
    ihm.montrer("imgCamera");
    if (t_CamHS)      // remettre le film si la camera était affichée
        $("#imgCamera").attr("src", adrImage);
    else
        $("#imgCamera").attr("src", adrCamera);
}

function montreFilm() {     // cache la vidéo de la caméra et montre le film
    ihm.montrer("vidFilm");
    ihm.cacher("imgCamera");
}

function montrerGraphe() {
    $("#imgCamera").css({left: "10px"});    // mettre la vidéo à gauche
    $("#imgCamera").css({width: "675px"});
    $("#vidFilm").css({left: "10px"});      // mettre la vidéo à gauche
    $("#vidFilm").css({width: "675px"});
    $("#divGraphe").css({left: "705px"});   // mettre le graphe à droite
    $("#divGraphe").css({width: "560px"});  // redimensionner les graphes
    ihm.montrer("divGraphe");
    ihm.montrer("btnPopVideoHideVideo");
    ihm.montrer("btnPopCameraHideVideo");
    fermerPopupCentrale();
    g_refresh();            // pour avoir de suite le bon format des graphes
    ihm.setEtat("btnPopVideoShowGraphe", 0);
    ihm.setEtat("btnPopCameraShowGraphe", 0);
    ml_updateEtat();
}

// A modifier : montrer au moins une video
// Cacher une popup possible
function cacherGraphe() {
    $("#imgCamera").css({left: "240px"});
    $("#imgCamera").css({width: "800px"});
    $("#vidFilm").css({left: "240px"});
    $("#vidFilm").css({width: "800px"});
    if (ihm.isCache("imgCamera"))
        ihm.montrer("vidFilm");
    else
        ihm.montrer("imgCamera");
    fermerPopupCentrale();
    ihm.cacher("divGraphe");
    ihm.cacher("btnPopVideoHideVideo");
    ihm.cacher("btnPopCameraHideVideo");
    ihm.setEtat("btnPopVideoShowGraphe", 1);
    ihm.setEtat("btnPopCameraShowGraphe", 1);
    ml_updateEtat();
}

function montrerVideo() {
    $("#divGraphe").css({left: "705px"});       // mettre le graphe à droite
    $("#divGraphe").css({width: "560px"});      // redimensionner les graphes
    if (ihm.isCache("imgCamera"))
        ihm.montrer("vidFilm");
    else
        ihm.montrer("imgCamera");
    fermerPopupCentrale();
    g_refresh();            // pour avoir de suite le bon format des graphes
    ihm.setEtat("btnPopHideVideo", 0);
    ihm.montrer("btnPopHideGraphe");
    ml_updateEtat();
}

function cacherVideo() {
    $("#divGraphe").css({left: "10px"});         // mettre le graphe à gauche
    $("#divGraphe").css({width: "1255px"});      // redimensionner les graphes
    if (ihm.isCache("imgCamera")) {
        ihm.cacher("vidFilm");
        videoEchoStop(true);
    }
    else {
        ihm.cacher("imgCamera");
        videoEchoStop(false);
    }
    fermerPopupCentrale();
    g_refresh();            // pour avoir de suite le bon format des graphes
    ihm.setEtat("btnPopHideVideo", 1);
    ihm.cacher("btnPopHideGraphe");
    ml_updateEtat();
}

// si changement d'affichage central, il faut fermer tout
// menu contextuel ouvert
function fermerPopupCentrale() {
    if (ihm.isPopupActive() && (ihm.idPopupActive() === "popGraphes"))
         $("#popGraphes").popup("close");
    else if (ihm.isPopupActive() && (ihm.idPopupActive() === "popFilm"))
         $("#popFilm").popup("close");
    else if (ihm.isPopupActive() && (ihm.idPopupActive() === "popCamera"))
         $("#popCamera").popup("close");
}

function cameraStop() {     // masque la vidéo de la caméra et affiche une image
    $("#imgCamera").attr("src", adrImage);
}

function cameraStart() {    // masque l'image et affiche la vidéo de la caméra
    $("#imgCamera").attr("src", adrCamera);
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

function montreVueGrapheXYZ() { // montre le graphique unique
    ihm.montrer("divGrapheXYZ");
    ihm.cacher("div3Graphes");
    g_refresh();
}

function btnPopConfirm() {
    ihm.cacher("btnPopPbNivellement");
    ihm.cacher("fldPopConfirm");
    var numConfirmation = ihm.getEtat("msgPopConfirm");
    switch (numConfirmation) {
        case  0x01   :
            c_envoi(CONF_OUVERTURE_PINCE);
            break;
        case  0x02   :
            c_envoi(CONF_POSE_HP3);
            break;
        case  0x04   :
            c_envoi(CONF_LACHER_WTS);
            break;
        case  0x08   :
            c_envoi(CONF_SAISIR_WTS);
            break;
        case  0x010  :
            c_envoi(CONF_NIVELL_SEIS);
            break;
        case  0x020  :
            c_envoi(CONF_LACHER_SEIS);
            break;
        case  0x040  :
            c_envoi(CONF_SAISIR_SEIS);
            break;
        case  0x080  :
            c_envoi(CONF_LANCER_TESTS);
            break;
        case  0x100  :
            c_envoi(CONF_REPRISE_PB_PAN1);
            break;
        case  0x200  :
            c_envoi(CONF_REPRISE_PB_PAN2);
            break;
        case  0x400  :
            c_envoi(CONF_REPRISE_PB_PAN1);
            break;
        case  0x800  :
            c_envoi(CONF_REPRISE_PB_PAN2);
            break;
        case  0x1000 :
            c_envoi(CONF_REMONTER_HP3_POUR_ARRET);
            break;
        case  0x2000 :
            c_envoi(CONF_REMONTER_WTS_POUR_ARRET);
            break;
        case  0x4000 :
            c_envoi(REMONTER_SEIS_POUR_ARRET);
            break;
        case  0x8000 :
            c_envoi(CONF_FERMER_PANNEAUX_POUR_ARRET);
            break;
    }
}

function btnPopPbNivellement() {
    ihm.cacher("btnPopPbNivellement");
    ihm.montrer("fldPopConfirm");
}

function btnPopRepliPieds() {
    c_envoi(S_SEIS_REPLIER_PIEDS);
}

function btnPopNivellement() {
    c_envoi(S_SEIS_LANCER_NIVEL);
}

function btnPopDecision(ok) {
    var numDecision = ihm.getEtat("msgPopDecision");
    switch (numDecision) {
        case 0x01 :
            if (ok)
                c_envoi(DECISION_BATTERIE_FAIBLES_MAIS_CONTINUER);
            else
                c_envoi(DECISION_BATTERIE_FAIBLES_DONC_ARRETER);
            break;
        case 0x02 :
            if (ok)
                c_envoi(DECISION_SORTIE_TESTS_ET_CONTINUER);
            else
                c_envoi(DECISION_SORTIE_TESTS_ET_ARRETER);
            break;
        case 0x04 :
            if (ok)
                c_envoi(DECISION_SORTIE_INITIALISATION_ET_CONTINUER);
            else
                c_envoi(DECISION_SORTIE_INITIALISATION_ET_ARRETER);
            break;
        case 0x08 :
            if (ok)
                c_envoi(DECISION_DEFAUT_DETECTES_MAIS_CONTINER);
            else
                c_envoi(DECISION_DEFAUT_DETECTES_DONC_ARRETER);
            break;
    }
}

function btnCommande(sousSysteme) {
    var etat, id, st;
    switch (sousSysteme) {
        case PANNEAUX :
            id = "btnPanneaux";
            st = PANNEAUX_O;
            break;
        case HP3 :
            id = "btnHP3";
            st = HP3_D;
            break;
        case SEIS :
            id = "btnSEIS";
            st = SEIS_D;
            break;
        case WTS :
            id = "btnWTS";
            st = WTS_D;
            break;
        case QUITTER :
            id = "btnQuitter";
            st = QUITTER;
            break;
    }
    if (ihm.isButtonEnable(id)) {   // le bouton est t'il actif ?
        etat = ihm.getEtat(id);
        if (etat === 0)     // il faut déposer le sous système
            ihm.setEtat("msgPopCommande", st);
        else                // il faut reprendre le sous système
            ihm.setEtat("msgPopCommande", st + 1);
        ml_updateEtat();
        ihm.openPopup("popCommande");        // gestion locale
    }
}

function btnPopCommande(OK) {
    if (OK) {
        var sousSysteme = ihm.getEtat("msgPopCommande");
        switch (sousSysteme) {
            case PANNEAUX_O :
                c_envoi(C_OUVERTURE_PANNEAUX);
                break;
            case PANNEAUX_F :
                c_envoi(C_FERMETURE_PANNEAUX);
                break;
            case HP3_D :
                c_envoi(C_DEPOSE_HP3);
                break;
            case HP3_R :
                c_envoi(C_REPRISE_HP3);
                break;
            case SEIS_D :
                c_envoi(C_DEPOSE_SEIS);
                break;
            case SEIS_R :
                c_envoi(C_REPRISE_SEIS);
                break;
            case WTS_D :
                c_envoi(C_DEPOSE_WTS);
                break;
            case WTS_R :
                c_envoi(C_REPRISE_WTS);
                break;
            case QUITTER :
                c_envoi(C_ARRET_SYSTEME);
                break;
        }
    }
    ihm.closePopup();
    ml_updateEtat();
}

function imgCamera() {
    ihm.openPopup("popCamera");          // gestion locale
}

function vidFilm() {
    ihm.openPopup("popFilm");          // gestion locale
}

function divGraphe() {
    ihm.openPopup("popGraphes");        // gestion locale
}

function btnARU() {
    c_envoi(U_ARRU);
}

function btnPopAruOK() {
    c_envoi(U_FIN_ARRU);
}

function btnDepannage() {
    ihm.openPopup("popDepannage");
}

function btnDepannageDecision(ok) {
    var numDecision = ihm.getEtat("msgDepannageDecision");
    if (ok) {
        switch (numDecision) {  // Décisions d'envoi de commande pour dépannage
            case 1 : c_envoi(C_ARRET_SYSTEME); break;
            case 2 : c_envoi(S_SEIS_ARRET_MESURES); break;
            case 3 : c_envoi(S_SEIS_LANCER_MESURES); break;
            case 4 : c_envoi(S_SEIS_REPLIER_PIEDS); break;
            case 5 : c_envoi(S_METTRE_PANNEAUX_HS); break;
            case 6 : c_envoi("sni"); break;         // Fin Nivellement SEIS
        }
    }
    ihm.closePopup();
}

// on joue la vidéo de l'ouverture des panneaux de 38 à 45
// element : id de la video à voir
function videoPanneaux(film) {
    if (film)
        videoPlay("vidFilm", 38, 45, 0.25);
    else
        videoPlay("vidPopFilm", 38, 45, 0.25);
}

// on joue la vidéo du déchargement du SEIS de 45 à 61
// element : id de la video à voir
function videoSEIS(film) {
    if (film)
        videoPlay("vidFilm", 45, 63, 0.4);
    else
        videoPlay("vidPopFilm", 45, 63, 0.4);
}

// on joue la vidéo du déchargement du WTS de 62 à 78.2
// element : id de la video à voir
function videoWTS(film) {
    if (film)
        videoPlay("vidFilm", 64, 81, 0.4);
    else
        videoPlay("vidPopFilm", 64, 81, 0.4);
}

// on joue la vidéo du déchargement du HP3 de 78.3 à 93
// element : id de la video à voir
function videoHP3(film) {
    if (film)
        videoPlay("vidFilm", 82, 97, 0.4);
    else
        videoPlay("vidPopFilm", 82, 97, 0.4);
}

// joue une video entre start et stop à la vitesse donnée
// element : id de la video à voir
// start :  réel positif en secondes
// stop : réel positif en secondes
// vitesse : entre 0.1 et 1.0
function videoPlay(element, start, stop, vitesse) { // 
    var video = document.getElementById(element);
    video.pause();
    video.currentTime = start;      // place le point de lecture de la vidéo 
    video.playbackRate = vitesse;   // vitesse de lecture de la vidéo
    var duree = Math.abs((stop - start) / vitesse);   // durée restante de la vidéo
    // correction d'une erreur de gestion du paramètre
    timerVideo = setTimeout(function() {videoStop(video);}, duree * 1000);  // lance videoStop à la fin de la durée restante
    video.play();
}

// arrêt de la vidéo donnée
// video : id de la video à voir
function videoStop(video) {   // met la vidéo en pause
    video.pause();
    clearInterval(timerVideo);
    video.defaultPlaybackRate = 1.0;    // vitesse de lecture de la vidéo
}

// joue une video où elle se trouve
function videoEchoPlay(film) {
    var video;
    if (film)
        video = document.getElementById("vidFilm");
    else
        video = document.getElementById("vidPopFilm");
    video.play();
}

// arrête une video où elle se trouve
function videoEchoStop(film) { 
    var video;
    if (film)
        video = document.getElementById("vidFilm");
    else
        video = document.getElementById("vidPopFilm");
    video.pause();
}
