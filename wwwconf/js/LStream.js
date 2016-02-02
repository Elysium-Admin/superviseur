/** --------------------------------------------------------------------------
 * Projet Insight
 * 
 * Nom du fichier : LStream.js
 * Langage : Javascript
 * Description : 
 * Scripts de communication par stream avec le serveur
 * pour réception des états du lander et des données sismométriques  
 * Organisme : Lycée International Victor Hugo Colomiers, BTS IRIS
 * Chef de projet : J.Voyé, JP.Dumas
 * Version 0.10 - 04/2013      Auteur : LAMAMY Bastien
 * Version 0.23 - 02/2015      Auteur : JP.Dumas/J.Voyé/DANTIN Benoit
 *     Réécriture du source et de la mise en page
 *        Changement de la structure de la trame
 * Version 0.27 - 02/04/2015   Auteur : JP.Dumas/DANTIN Benoit
 *     Ajout des commandes de mise à jour des IHM à distance
 * Version 0.30 - 08/04/2015    Auteur : JP.Dumas/DANTIN Benoit
 *    Prise en compte des commande d'extraits vidéo de dépose
 * Version 0.31 - 15/04/2015   Auteur : JP.Dumas/DANTIN Benoit
 *    Ajout de la gestion du film de présentation sur popFilmElysium
 * Version 0.32 - 25/04/2015    Auteur : DANTIN Benoit
 *    Gestion des états sur page5
 * Version beta1 - 06/05/2015    Auteur : DANTIN Benoit
 *    Conversion correcte du signed short tn_courantBatteries
 * ---------------------------------------------------------------------------- 
 * ----------------------------------------------------------------------------
 *                              ARCHITECTURE
 * ----------------------------------------------------------------------------
 * 
 *      Superviseur      TCP local              UDP distant 
 *  Service de diffusion    >>    cgiStream.cgi      >>      IHM
 *       Raspberry                  Raspberry            Tablette ou PC
 *        Thead                  Serveur Web Thread         Stream
 *         C++                      C++                   Javascript
 * ----------------------------------------------------------------------------
 */
/**
 * ----------------------------------------------------------------------------
 * Définition de la trame de transfert des états vers l'IHM
 * ----------------------------------------------------------------------------
 * La trame est une chaine de 103 caractères,
 * composée de 21 champs séparés par ';'
 * 00 01 02 03    04       05    06    07    08 09 10    11    12 13    14
 * AA;YY;EE;MM;CCCCCCCC;IIIIIIII;SS;ssssssss;WW;HH;PP;pppppppp;NN;BB;bbbbbbbb;
 * 
 *                                                  15   16   17   18   19  20
 *                                                 TTTT;RRRR;BBBB;OOOO;UUUU;ee;
 * ----------------------------------------------------------------------------
    AA       : état de l'arrêt d'urgence
    YY       : état globale du système
    EE       : séquence en cours
    MM       : modes dégradés
    CCCCCCCC : code de compte rendu système
    IIIIIIII : demande d'intervention IHM
    SS       : situation du SEIS
    ssssssss : compte rendu erreur SEIS
    WW       : situation du WTS
    HH       : situation du HP3
    PP       : état des panneaux
    pppppppp : code incident PANNEAUX
    NN       : état de la pince
    BB       : état du bras
    bbbbbbbb : code d’information bras/pince
    TTTT     : tension batteries (0,1V)
    RRRR     : charges des batteries (0,1%)
    BBBB     : courant dans les batteries (0,1A)
    OOOO     : charge consommée (0,1%)
    UUUU     : durée restante dans les batteries (100s)
    ee       : code incident énergie
    XXX      : commande d'action IHM
 --------------------------------------------------------------------------- 
 */
/* global batterie, ihm, SIMU, adrImage, adrCamera, I_RIEN, I_MONTRE_FILM */

var CGI_STREAM_FILE = "cgistream.cgi";
var CGI_CMD_FILE = "cgicommande.cgi";

// ------------------ variables globales ------------------------------------
var sourceEtat;
var sourceSEIS;
var initEtat = true;

// fixe les couleurs des messages d'état des panneaux (valeurs)
// NORMAL = 0, ACTIF = 1, ERREUR = 2; 
var couleurMsgPanneaux = new Array(0,1,1,0,1,1,2,2,2,2,0,1,0,0,0,2);
// fixe les couleurs des messages d'état des sous systèmes (bit)
var couleurMsgSys = new Array(2,0,1,0,1,2,1,0,2);
// fixe les couleurs des messages d'état du bras (valeurs complexes)
var couleurMsgBras = new Array(2,0,1,1,1,1,1,0,1,1,2,2);
// fixe les couleurs des messages d'état de la pince (valeurs + 1)
var couleurMsgPince = new Array(2,0,1,0,1);

// -------------- Anciens champs de la trame d'état en entier --------------- 
var t_aru = parseInt(0);                   // Arrêt d'urgence
var t_etatSysteme = parseInt(0);           // Etat du système
var t_sequenceEnCours = parseInt(0);       // séquence en cours
var t_modesDegrade = parseInt(0);          // modes dégradés
var t_compteRenduSysteme = parseInt(0);    // code de compte rendu système
var t_interventionIHM = parseInt(0);       // demande d'intervention IHM
var t_situationSEIS = parseInt(0);         // situation du SEIS
var t_erreurDuSEIS = parseInt(0);          // compte rendu erreur SEIS
var t_situationWTS = parseInt(0);          // situation du WTS
var t_situationHP3 = parseInt(0);          // situation du HP3
var t_etatPanneaux = parseInt(0);          // état des panneaux
var t_codeIncidentPanneaux = parseInt(0);   // code incident PANNEAUX
var t_etatPince = parseInt(0);             // état de la pince
var t_etatBras = parseInt(0);              // état du bras
var t_codeIncidentBrasPince = parseInt(0); // code d’information bras/pince
var t_tensionBatteries = parseInt(0);      // Tension batteries (en Volts)
var t_chargeBatteries = parseInt(0);       // charges des batteries (en %)
var t_courantBatteries = parseInt(0);      // Courant dans batteries (en Ampères)
var t_chargeConsommees = parseInt(0);      // Charge consommée (en %)
var t_dureeRestanteBatteries = parseInt(0);// Durée restante dans batteries (en s)
var t_codeIncidentEnergie = parseInt(0);   // code incident énergie
var t_cmdIHM = parseInt(0);                // code de la commande d'IHM

// ---------- Données distinctes nouvelles ------------
var t_etatPanneau1 = parseInt(0);          // Etat du panneau 1
var t_etatPanneau2 = parseInt(0);          // Etat du panneau 2
var t_demandeDecision = parseInt(0); 
var t_demandeConfirmation = parseInt(0); 
var t_seisMesuresEnCours = false;          // Le SEIS fournit des mesures
var t_alarmeBatterie = false;
var t_batteriesHS = false;
var t_panneauxHS = false;
var t_brasHS = false;
var t_SeisHS = false;
var t_CamHS = false;
var t_ruptureLiaisonSeis = false;
var t_ruptureLiaisonBras = false;
var t_ruptureLiaisonPanneaux = false;
var t_ruptureLiaisonCCGX = false;
var t_ruptureLiaisonCamera = false;
var t_ruptureSrvCamera = false;
// -------- Sous systèmes non testés ------------
var t_CCGXNonTeste = false;
var t_PanneauxNonTeste = false;
var t_SeisNonTeste = false;
var t_CameraNonTeste = false;
var t_BrasNonTeste = false;
// -------- Sous sytèmes en test ----------------
var t_CCGXTestEnCours = false;
var t_PanneauxTestEnCours = false;
var t_SeisTestEnCours = false;
var t_CameraTestEnCours = false;
var t_BrasTestEnCours = false;
var t_commandeInconnue = false;

// sémaphore de non réentrance pour le traitement de la trame courante
var occupe = false;

/* ---------------------- Connexion stream ----------------------------------
 Connexion au stream serveur 'cgiStream.cgi' pour demande d'Event Source SSE 
 et installe un listener pour l'événement 'etat'
 et un listener pour l'événement 'vibration'
 ----------------------------------------------------------------------------*/
function sseConnexion() {
    // Demande d'un stream serveur à 'cgistream.cgi' à partir d'un GET
    // IP_WEB   : 10.05.1.30 à voir. PORT_WEB : 2001 pour le conférencier
    // --------- Remplacé pour test par une Servlet Java sous Tomcat --------
    if (SIMU) {
        //Créer un nouvel objet EventSource
        sourceEtat = new EventSource("SSEServer");          // provoque un GET
        //permet d'assigner la gestionnaires d'un évènements sur 
        //un élément cible, ici sur l'event "etat"
        sourceEtat.addEventListener('etat', function (event) {
            traiterTrameEtat(event.data);
        }, false);
        //permet d'assigner la gestionnaires d'un évènements sur un élément cible
        //ici sur l'event "vibration"
        var sourceSEIS = new EventSource("SSE_SEIS");
        sourceSEIS.addEventListener('vibration', function (event) {
            traiterTrameSismo(event.data);
        }, false);
    }
    else {
          //Créer un nouvel objet EventSource avec le chemin du CGIStream
        source = new EventSource("cgi-bin/cgiStream.cgi");
        //permet d'assigner la gestionnaires d'un évènements sur un élément cible
        //ici sur l'event "etat"
        source.addEventListener('etat', function (event) {
            traiterTrameEtat(event.data);
        }, false);
        //permet d'assigner la gestionnaires d'un évènements sur un élément cible
        //ici sur l'event "Vibration"
        source.addEventListener('vibration', function (event) {
            traiterTrameSismo(event.data);
        }, false);
    }
}

/*
 * Ce déclenche quand la page se recharge
 * @param {type} trame
 * @returns {undefined}
 */
window.onbeforeunload = function() {
    if (SIMU) {
        ;   // rien
    }
    else {
        source.close();     // Ferme la connexion  au serveur 
        source.close();     // Ferme la connexion  au serveur 
    }
};

/* ------------------ traiter la trame sismometre -----------------------------
 * Cette trame contient N datas sismométrique 
 * Une donnée sismométrique est constitue de : sXXXX,sYYYY,sZZZZ (17 caractères)
 * Les données sont séparées par ';'
 * s : signe, XXXX : valeur des vibrations en X en mg (accélération en milli-g)
 * t : signe, YYYY : valeur des vibrations en Y en mg
 * u : signe, ZZZZ : valeur des vibrations en Z en mb
 * Symbole de fin de trame ';' (on connait la taille de la trame qui est fixe)
 * Par exemple : 
 *   +0000,+0000,+0000;+0003,+0001,+0001;+0012,+0003,+0002;+0007,+0001,+0000;
 *   Au max : 50 données par trame par 1/2s => 50x18 cars = 900 cars
 *   La trame se terminera par un zéro (null)
 ---------------------------------------------------------------------------- */
function traiterTrameSismo(trame) {
    if (occupe)
        return;
    occupe = true;
    g_updateDonnees(trame);                         // mise à jour de la trame
    occupe = false;
}

/* ------------------ traiter la trame d'état --------------------------------
 * 21 champs séparés par ';'
 * 0  1  2  3     4        5     6     7     8  9  10    11    12 13    14     15   16   17   18   19  20
 * AA;YY;EE;MM;CCCCCCCC;IIIIIIII;SS;ssssssss;WW;HH;PP;pppppppp;NN;BB;bbbbbbbb;TTTT;RRRR;BBBB;OOOO;UUUU;ee;
 ---------------------------------------------------------------------------- */
function traiterTrameEtat(trame) {                        // mise à jour de la trame
    // ihm.setMsg(trame);           // il faut aussi modifier ihm.setMsgInfo()
    // séparation des champs
    var champs = trame.split(';');
    // on convertit les champs hexadécimaux en nombres entier
    var tn_aru = parseInt(champs[0], 16);                    // Arrêt d'urgence
    var tn_etatSysteme = parseInt(champs[1], 16);            // Etat du système
    var tn_sequenceEnCours = parseInt(champs[2], 16);        // Séquence en cours
    var tn_modesDegrade = parseInt(champs[3], 16);           // Modes dégradés
    var tn_compteRenduSysteme = parseInt(champs[4], 16);     // Compte Rendu Système
    var tn_interventionIHM = parseInt(champs[5], 16);        // Demande IHM
    var tn_situationSEIS = parseInt(champs[6], 16);          // Situation du SEIS
    var tn_erreurDuSEIS = parseInt(champs[7], 16);           // Code Incident SEIS
    var tn_situationWTS = parseInt(champs[8], 16);           // Situation du WTS
    var tn_situationHP3 = parseInt(champs[9], 16);           // Situation du HP3
    var tn_etatPanneaux = parseInt(champs[10], 16);          // Etats Panneaux
    var tn_codeIncidentPanneaux = parseInt(champs[11], 16);  // Code Pb Panneaux
    var tn_etatPince = parseInt(champs[12], 16);             // Etat Pince
    var tn_etatBras = parseInt(champs[13], 16);              // Etat Bras
    var tn_codeIncidentBrasPince = parseInt(champs[14], 16); // Code Incident Bras
    var tn_tensionBatteries = parseInt(champs[15], 16)/100.0; // Tension batteries
    var tn_chargeBatteries  = parseInt(champs[16], 16)/10.0; // Charges batteries
    var tn_courantBatteries = parseInt(champs[17], 16);      // Courant batteries
    if (tn_courantBatteries >= 32768)       // la valeur est signée sur 2 octets
        tn_courantBatteries = tn_courantBatteries - 65536;
    tn_courantBatteries /= 10.0;
    var tn_chargeConsommees = parseInt(champs[18], 16)/10.0; // Charge consommée
    var tn_dureeRestanteBatteries = parseInt(champs[19], 16)*100;// Durée restante 
    var tn_codeIncidentEnergie = parseInt(champs[20], 16);   // Code Incident Bat.
    var tn_cmdIHM = parseInt(champs[21], 16);                // Cmd IHM

    // ------------------ AA:VAL_ORG_ARRU ------------------------
    if ((tn_aru !== t_aru) || initEtat) {
        t_aru = tn_aru; // met à jour la variable, pour prochaines comparaisons
        switch (tn_aru) {
            case 0x00 : // PAS_ARRU = 0x00 
                ihm.closePopupARU(); // ferme l'ARU si elle est ouverte
                initEtat = true;
                break; 
            case 0x01 : // ARRU_MATERIEL = 0x01
                ihm.openARU(1); // afficher la popup d'ARU matérielle
                return false;   // on ne traite rien d'autre si ARU
                break;
            case 0x02 : // ARRU_LOGICIEL = 0x02
                ihm.openARU(0); // afficher la popup d'ARU logicielle
                return false;   // on ne traite rien d'autre si ARU
                break;
        }
    }

    // --------- RECUPERE LES VALEURS D'ALIMENTATION ---------------------
    //tensions des batteries (en V)
    if ((tn_tensionBatteries !== t_tensionBatteries) || initEtat) {
        // mes a jours la variable , pour les prochaines comparaison
        t_tensionBatteries = tn_tensionBatteries;
        // on affiche la nouvelle tension des batteries
        batterie.setTension(tn_tensionBatteries);
        // mise à jour des données batterie de la popup
        $("#lblValeurTensionBatteries").html(batterie.tensionBatteries);
    }
    // Charges des batteries (en %)
    if ((tn_chargeBatteries !== t_chargeBatteries) || initEtat) {
        t_chargeBatteries = tn_chargeBatteries;
        // on affiche la nouvelle charge des batteries
        batterie.setCharge(tn_chargeBatteries);
        // mise à jour des données batterie de la popup
        $("#lblValeurChargeBatteries").html(batterie.chargeBatteries);
    }
    // Courant dans batteries (0,1A)
    if ((tn_courantBatteries !== t_courantBatteries) || initEtat) {
        t_courantBatteries = tn_courantBatteries;
        // on affiche le nouveau courant des batteries
        batterie.setCourant(tn_courantBatteries);
        // mise à jour des données batterie de la popup
        $("#lblValeurCourantBatteries").html(batterie.courantBatteries);
    }
    // Charge consommée (0,1%)
    if ((tn_chargeConsommees !== t_chargeConsommees) || initEtat) {
        // on affiche la nouvelle charge consommée des batteries
        batterie.setConsommee(tn_chargeConsommees);
        // mise à jour des données batterie de la popup
        $("#lblValeurChargeConsommees").html(batterie.chargeConsommees);
        t_chargeConsommees = tn_chargeConsommees;
    }
    // Durée restante dans batteries (100s)
    if ((tn_dureeRestanteBatteries !== t_dureeRestanteBatteries) || initEtat) {
        t_dureeRestanteBatteries = tn_dureeRestanteBatteries;
        // conversion en minute pour affichage sur IHM
        var dureeEnMn = tn_dureeRestanteBatteries / 60;
        batterie.setDuree(dureeEnMn);
        // affichage de la durée restante
        var hours = Math.floor(dureeEnMn / 60);
        var minutes = Math.floor(dureeEnMn % 60);
        // avec mise au format HHhMM de l'affichage
        var duree = ((hours < 10) ? '0' + hours : hours) + 'h' 
                                + ((minutes < 10) ? '0' + minutes : minutes);
        // mise à jour des données batterie de la popup
        $("#lblValeurDureeRestanteBatteries").html(duree);
    }
    // ------------------ ee:CODE INCIDENT ENERGIE ------------------------
    if ((tn_codeIncidentEnergie !== t_codeIncidentEnergie) || initEtat) {
        t_codeIncidentEnergie = tn_codeIncidentEnergie;
        //ouvre popup de decision
        majCodeIncidentEnergie(tn_codeIncidentEnergie);
    }
    // ------------------- YY:VAL_ETAT_SYSTEME -------------------- 
    // permet de montre la bonne page en fonction de l'état du système
    if ((tn_etatSysteme !== t_etatSysteme) || initEtat) {
        t_etatSysteme = tn_etatSysteme;
        majEtatSysteme(tn_etatSysteme);
    }
    // ---------------- EE:VAL_SEQ_EN_COURS -------------------- 
    if ((tn_sequenceEnCours !== t_sequenceEnCours) || initEtat) {
        t_sequenceEnCours = tn_sequenceEnCours;
        //modifie le texte du message avec l'id="msgInfo"
        ihm.setMsgInfo(tn_sequenceEnCours);
        if (tn_sequenceEnCours !== 0) {     // une séquence en cours ?
            ihm.disableAllCmdButton();      // on ne peut plus envoyer de cmd
        }
        else {
            // ----------- PAS DE SEQUENCE EN COURS !!! ---------------------
            var tn_positionSEIS = tn_situationSEIS & 0x7F;
            var tn_etatPanneau1 = parseInt(tn_etatPanneaux & 0x0F);
            var tn_etatPanneau2 = parseInt((tn_etatPanneaux & 0xF0) >> 4);
            var panneau1HS = (tn_etatPanneau1 === 0x0f);
            var panneau2HS = (tn_etatPanneau2 === 0x0f);
            var tn_panneauxHS = (tn_modesDegrade & 0x04 ? true : false);
            var tn_SeisHS = (tn_modesDegrade & 0x10 ? true : false);

            ihm.enableButton("btnHP3");
            if (tn_positionSEIS === 1) {         // SEIS sur plateau
                ihm.setEtat("btnSEIS", 0);
                ihm.disableButton("btnWTS");
            }
            else {     // SEIS au sol
                ihm.setEtat("btnSEIS", 1);
                ihm.enableButton("btnWTS");
            }
            if (tn_situationWTS === 1) {         // WTS sur plateau
                ihm.setEtat("btnWTS", 0);
                ihm.enableButton("btnSEIS");
            }
            else {
                ihm.setEtat("btnWTS", 1);
                ihm.disableButton("btnSEIS");
            }
            // gestion du bouton pour les panneaux
            if (((tn_etatPanneau1 === 0) || panneau1HS)     // panneaux fermés
                    && ((tn_etatPanneau2 === 0) || panneau2HS)) {
                ihm.setEtat("btnPanneaux", 0);
            }
            else if (((tn_etatPanneau1 === 3) || panneau1HS) // panneaux ouverts
                    && ((tn_etatPanneau2 === 3) || panneau2HS)) {
                ihm.setEtat("btnPanneaux", 1);
            }
            if (tn_panneauxHS)
                ihm.disableButton("btnPanneaux");    // commande OUVRIR autorisée
            else
                ihm.enableButton("btnPanneaux");
        
            // si tout est rangé on peut quitter
            if (((tn_etatPanneau1 === 0) || panneau1HS || tn_panneauxHS)
                && ((tn_etatPanneau2 === 0) || panneau2HS || tn_panneauxHS)
                && (tn_positionSEIS === 0x01)
                && (tn_situationHP3 === 0x01)
                && (tn_situationWTS === 0x01)) {
                ihm.enableButton("btnQuitter"); 
            }
        }
    }
    // -------------------- MM:FLAGS_MODE -----------------------
    if ((tn_modesDegrade !== t_modesDegrade) || initEtat) {
        t_modesDegrade = tn_modesDegrade;
        
        var tn_alarmeBatterie  = (tn_modesDegrade & 0x01 ? true : false);
        var tn_batteriesHS     = (tn_modesDegrade & 0x02 ? true : false);
        var tn_panneauxHS      = (tn_modesDegrade & 0x04 ? true : false);
        var tn_brasHS          = (tn_modesDegrade & 0x08 ? true : false);
        var tn_SeisHS          = (tn_modesDegrade & 0x10 ? true : false);
        var tn_CamHS           = (tn_modesDegrade & 0x80 ? true : false);

        if (tn_alarmeBatterie !== t_alarmeBatterie)
            t_alarmeBatterie = tn_alarmeBatterie;
        if ((tn_batteriesHS !== t_batteriesHS) || initEtat) {
            t_batteriesHS = tn_batteriesHS;
            batterie.setPanne(tn_batteriesHS);
        }
        else
            batterie.setPanne(0);
        if ((tn_panneauxHS !== t_panneauxHS) || initEtat) {
            t_panneauxHS = tn_panneauxHS;
            majPanneaux(tn_etatPanneaux);
        }
        if ((tn_brasHS !== t_brasHS) || initEtat) {
            t_brasHS = tn_brasHS;
            majBras(tn_etatBras, tn_situationSEIS, tn_situationWTS);
            majPince(tn_etatPince, t_brasHS);
        }
        if ((tn_SeisHS !== t_SeisHS) || initEtat) {
            t_SeisHS = tn_SeisHS;
            majSEIS(tn_situationSEIS);
        }
        if ((tn_CamHS !== t_CamHS) || initEtat) {
            t_CamHS = tn_CamHS;
            if (tn_CamHS) {     // remettre la photo si la camera était affichée
                ihm.setEtatCamera(false);
                $("#imgCamera").attr("src", adrImage);
            } 
            else {
                ihm.setEtatCamera(true);
                $("#imgCamera").attr("src", adrCamera);
            }
        }
    }
    // -------- CCCCCCCC : COMPTE_RENDU_SYSTEME -----------------
    // A LAISSER APRES LES MODES DEGRADES !!!
    // rupture de liaisons et tests
    if ((tn_compteRenduSysteme !== t_compteRenduSysteme) || initEtat) {
        t_compteRenduSysteme = tn_compteRenduSysteme;
        majCompteRenduSysteme(tn_compteRenduSysteme);
    }
    // ----------- IIIIIIII:FLAGS_DEM_IHM ----------------------- 
    if (tn_interventionIHM !== t_interventionIHM) {
        t_interventionIHM = tn_interventionIHM;
        majIHM(tn_interventionIHM);
    }
    // -------------------- SS:FLAGS_SITUATION SEIS -------------------- 
    if ((tn_situationSEIS !== t_situationSEIS) || initEtat) {
        t_situationSEIS = tn_situationSEIS;
        majSEIS(tn_situationSEIS);
    }
    // --------------- ssssssss: CODE ERREUR DU SEIS -----------------
    if ((tn_erreurDuSEIS !== t_erreurDuSEIS) || initEtat) {
        t_erreurDuSEIS = tn_erreurDuSEIS;
        // RIEN A TRAITER POUR L'INSTANT !
    }
    // -------------------- WW:FLAGS_SITUATION WTS --------------------
    if ((tn_situationWTS !== t_situationWTS) || initEtat) {
        t_situationWTS = tn_situationWTS;
        majWTS(tn_situationWTS);
    }
    // -------------------- HH:FLAGS_SITUATION HP3 -------------------- 
    if ((tn_situationHP3 !== t_situationHP3) || initEtat) {
        t_situationHP3 = tn_situationHP3;
        majHP3(tn_situationHP3);
    }
    // -------------------- PP:VAL_ETAT_PANNEAUX --------------------
    if ((tn_etatPanneaux !== t_etatPanneaux) || initEtat) {
        t_etatPanneaux = tn_etatPanneaux;
        majPanneaux(tn_etatPanneaux);
    }
    // ------------- pppppppp:CODE INCIDENT PANNEAUX ---------------
    if ((tn_codeIncidentPanneaux !== t_codeIncidentPanneaux) || initEtat) {
        t_codeIncidentPanneaux = tn_codeIncidentPanneaux;
        // RIEN POUR L'INSTANT (INUTILE)
    }
    // -------------------- BB:VAL_ETAT_BRAS -------------------- 
    if ((tn_etatBras !== t_etatBras) || initEtat) {
        t_etatBras = tn_etatBras;
        majBras(tn_etatBras, tn_situationSEIS, tn_situationWTS);
        // A traiter tn_actionEnCours & tn_incidentRedhibitoireBras !!!
    }
    // -------------------- NN:VAL_ETAT_PINCE -------------------- 
    if ((tn_etatPince !== t_etatPince) || initEtat) {
        t_etatPince = tn_etatPince;
        majPince(tn_etatPince, t_brasHS);
    }
    // -------------- bbbbbbbb:CODE INCIDENT BRAS-PINCE ---------------------
    if ((tn_codeIncidentBrasPince !== t_codeIncidentBrasPince) || initEtat) {
        t_codeIncidentBrasPince = tn_codeIncidentBrasPince;
        // RIEN A TRAITER POUR L'INSTANT !
    }
    // ------------------ XX:CODE COMMANDE IHM ------------------------
    if (tn_cmdIHM !== t_cmdIHM) {
        t_cmdIHM = tn_cmdIHM;
        majCmdIHM(tn_cmdIHM);
    }
    ml_updateEtat();                             // mettre à jour les labels
    initEtat = false;
};

/**
 * Mise a jour de l'affichage sur les IHM des observateur
 * @param {type} tn_cmdIHM
 * @returns {undefined}
 */
function majCmdIHM(tn_cmdIHM) {
    if (tn_cmdIHM !== 0) {
        switch (tn_cmdIHM) {
            case 0x01 : montrerGraphe();        break;  // I_VOIR_GRAPHE
            case 0x02 : cacherGraphe();         break;  // I_CACHER_GRAPHE
            case 0x03 : montrerVideo();         break;  // I_VOIR_VIDEO
            case 0x04 : cacherVideo();          break;  // I_CACHER_VIDEO
            case 0x05 : montreFilm();           break;  // I_MONTRE_FILM
            case 0x06 : montreCamera();         break;  // I_MONTRE_CAMERA
            case 0x07 : videoPanneaux(true);    break;  // I_VIDEO_PANNEAUX
            case 0x08 : videoSEIS(true);        break;  // I_VIDEO_SEIS
            case 0x09 : videoWTS(true);         break;  // I_VIDEO_WTS
            case 0x0a : videoHP3(true);         break;  // I_VIDEO_HP3
            case 0x0b : videoPanneaux(false);   break;  // I_CMD_VIDEO_PANNEAUX
            case 0x0c : videoSEIS(false);       break;  // I_CMD_VIDEO_SEIS
            case 0x0d : videoWTS(false);        break;  // I_CMD_VIDEO_WTS
            case 0x0e : videoHP3(false);        break;  // I_CMD_VIDEO_HP3
            case 0x0f : ihm.openPopupInfo("popFilmElysium"); break;
            case 0x10 : montreVue3Graphes();    break;  // I_MONTRE_3GRAPHES
            case 0x11 : montreVueGrapheXYZ();   break;  // I_MONTRE_GRAPHEXYZ
            case 0x12 : g_resetDatas();         break;  // I_RESET_DATA
            case 0x13 : grapheStop();           break;  // I_STOP_DATA
            case 0x14 : grapheStart();          break;  // I_MARCHE_DATA
            case 0x20 : ml_setLangue("fr");     break;  // I_LANGUE_FR
            case 0x21 : ml_setLangue("us");     break;  // I_LANGUE_US
            case 0x22 : ml_setLangue("de");     break;  // I_LANGUE_DE
            case 0x23 : ml_setLangue("es");     break;  // I_LANGUE_ES
            case 0x30 : ihm.openPopup("popInfoNasa");       break;  // I_POPUP_NASA
            case 0x31 : ihm.openPopup("popInfoCnes");       break;  // I_POPUP_CNES
            case 0x33 : ihm.openPopup("popFilmElysium");    break;  // I_POPUP_FILM
            case 0x34 : ihm.openPopup("popInfoAcToulouse"); break;  // I_POPUP_ACTOULOUSE
            case 0x35 : ihm.openPopup("popInfoBatterie");   break;  // I_POPUP_BATTERIE
            case 0x36 : ihm.closePopup();                   break;  // I_POPUP_FERMETURE
            case 0x40 : videoEchoPlay(true);                break;  // I_VIDEO_STOP
            case 0x41 : videoEchoStop(true);                break;  // I_VIDEO_PLAY
            case 0x42 : videoEchoPlay(false);               break;  // I_CMD_VIDEO_STOP
            case 0x43 : videoEchoStop(false);               break;  // I_CMD_VIDEO_PLAY
        }
    }
    c_envoi(I_RIEN);
}

/**
 * Mise a jour du code d'incident
 * @param {type} tn_codeIncidentEnergie
 * @returns {undefined}
 */
function majCodeIncidentEnergie(tn_codeIncidentEnergie) {
    batterie.setCode(tn_codeIncidentEnergie);
    $("#lblValeurCodeIncidentEnergie").html(tn_codeIncidentEnergie);
    // dmd si on continue ou non ?
    switch (tn_codeIncidentEnergie) {
        case 0x10:          // TENSION_BASSE
            ihm.dmdDecision(1); // msg batteries faibles
            break;
        // on ne fait rien pour les autres, pour l'instant
        case 0x00:  break;  // PAS_D_ALARME
        case 0x01:  break;  // ALARME_TENSION_BASSE
        case 0x02:  break;  // ALARME_TENSION_HAUTE
        case 0x04:  break;  // ALARME_TEMPERATURE
        case 0x08:  break;  // ALARME_STATUS_RELAIS
        case 0x20:  break;  // CHARGE_BASSE
    }
};

/**
 * on montre la bonne page fonction de l'état du système
 * @param {type} tn_etatSysteme
 * @returns {undefined}
 */
function majEtatSysteme(tn_etatSysteme) {
    // on montre la bonne page fonction de l'état du système
    if (tn_etatSysteme < 5)
        ihm.montrerPage(tn_etatSysteme + 1);
    // si c'est 5 c'est le Mode ARU, on ne fait rien !
    // c'est une popup qui va faire le boulot
};

/**
 * permet la gestion et la mise a jour de 
 * l'affichage des tests pendant la phase de test
 * @param {type} tn_compteRenduSysteme
 * @returns {undefined}
 */
function majCompteRenduSysteme(tn_compteRenduSysteme) {
    // rupture de service
    var tn_ruptureLiaisonSeis     = (tn_compteRenduSysteme & 0x01 ? true : false);
    var tn_ruptureLiaisonBras     = (tn_compteRenduSysteme & 0x02 ? true : false);
    var tn_ruptureLiaisonPanneaux = (tn_compteRenduSysteme & 0x04 ? true : false);
    var tn_ruptureLiaisonCCGX     = (tn_compteRenduSysteme & 0x08 ? true : false);
    var tn_ruptureLiaisonCamera   =  (tn_compteRenduSysteme & 0x010 ? true : false);
    var tn_ruptureSrvCamera       = (tn_compteRenduSysteme & 0x00000020 ? true : false);
    // Sous systèmes non testés
    var tn_CCGXNonTeste     = (tn_compteRenduSysteme & 0x00000040 ? true : false);
    var tn_PanneauxNonTeste = (tn_compteRenduSysteme & 0x00000080 ? true : false);
    var tn_SeisNonTeste     = (tn_compteRenduSysteme & 0x00000100 ? true : false);
    var tn_CameraNonTeste   = (tn_compteRenduSysteme & 0x00000200 ? true : false);
    var tn_BrasNonTeste     = (tn_compteRenduSysteme & 0x00000400 ? true : false);
    // Sous sytèmes en test
    var tn_CCGXTestEnCours      = (tn_compteRenduSysteme & 0x00000800 ? true : false);
    var tn_PanneauxTestEnCours  = (tn_compteRenduSysteme & 0x00001000 ? true : false);
    var tn_SeisTestEnCours      = (tn_compteRenduSysteme & 0x00002000 ? true : false);
    var tn_CameraTestEnCours    = (tn_compteRenduSysteme & 0x00004000 ? true : false);
    var tn_BrasTestEnCours      = (tn_compteRenduSysteme & 0x00008000 ? true : false);

    var tn_commandeInconnue = (tn_compteRenduSysteme & 0x80000000 ? true : false);

    // --------------- gestion des tests ----------------------
    if (t_etatSysteme === 1) {  // phase de test, après etatSysteme
        t_CCGXTestEnCours = tn_CCGXTestEnCours;
        gererTest(0, tn_CCGXTestEnCours, tn_CCGXNonTeste, t_batteriesHS);
        t_PanneauxTestEnCours = tn_PanneauxTestEnCours;
        gererTest(1, tn_PanneauxTestEnCours, tn_PanneauxNonTeste, t_panneauxHS);
        t_SeisTestEnCours = tn_SeisTestEnCours;
        gererTest(2, tn_SeisTestEnCours, tn_SeisNonTeste, t_SeisHS);
        t_CameraTestEnCours = tn_CameraTestEnCours;
        gererTest(3, tn_CameraTestEnCours, tn_CameraNonTeste, t_CamHS);
        t_BrasTestEnCours = tn_BrasTestEnCours;
        gererTest(4, tn_BrasTestEnCours, tn_BrasNonTeste, t_brasHS);
    }
    // les ruptures liaisons sont prises en compte dans les modes HS
    // donc pas ici !!!
    t_ruptureLiaisonSeis = tn_ruptureLiaisonSeis;
    t_ruptureLiaisonBras = tn_ruptureLiaisonBras;
    t_ruptureLiaisonPanneaux = tn_ruptureLiaisonPanneaux;
    t_ruptureLiaisonCCGX = tn_ruptureLiaisonCCGX;
    t_ruptureLiaisonCamera = tn_ruptureLiaisonCamera;
    t_commandeInconnue = tn_commandeInconnue;
};

/**
 * Mise a jours les demande de confirmation
 * et de decision de l'IHM
 * @param {type} tn_interventionIHM
 * @returns {undefined}
 */
function majIHM(tn_interventionIHM) {
    var tn_demandeConfirmation = tn_interventionIHM & 0x0000FFFF;
    if ((tn_demandeConfirmation !== t_demandeConfirmation) || initEtat) {
        t_demandeConfirmation = tn_demandeConfirmation;
        if (tn_demandeConfirmation !== 0x80) {
            if (tn_demandeConfirmation !== 0) {
                if (tn_demandeConfirmation !== 0x20)
                    ihm.dmdConfirmation(tn_demandeConfirmation);
                else
                    ihm.dmdConfirmationAvecOption(tn_demandeConfirmation);
            }
            else
                ihm.closePopup();           // on ferme la dmd de confirmation
        }
    }
    var tn_demandeDecision = (tn_interventionIHM & 0xFFFF0000) >> 16;
    if (tn_demandeDecision !== t_demandeDecision) {
        t_demandeDecision = tn_demandeDecision;
        if (tn_demandeDecision !== 0)
            ihm.dmdDecision(tn_demandeDecision);
        else
            ihm.closePopup();           // on ferme la dmd de décision
    }
};

/**
 * Permet la mise a jour des messages et états du SEIS
 * affiche ou cache les graphe
 * @param {type} tn_situationSEIS
 * @returns {undefined}
 */
function majSEIS(tn_situationSEIS) {
    if (t_SeisHS) {
        ihm.setEtatEtCouleur("msgSEIS", 256, ihm.ERREUR);   // msg : "Hors service"
        ihm.setEtatEtCouleur("msgSEIS_P5", 256, ihm.ERREUR);   // msg : "Hors service"
        cacherGraphe();
    }
    else {
        var tn_seisMesuresEnCours = ((tn_situationSEIS & 0x80) ? true : false);
        if ((tn_seisMesuresEnCours !== t_seisMesuresEnCours) || initEtat) {
            t_seisMesuresEnCours = tn_seisMesuresEnCours;
            if (tn_seisMesuresEnCours)
                montrerGraphe();
            else {
                cacherGraphe();
                g_resetDatas();
            }
        }
    }
    var tn_positionSEIS = tn_situationSEIS & 0x7F;
    ihm.setEtatEtCouleur("msgSEIS", tn_positionSEIS, couleurMsgSys[posBit(tn_positionSEIS)]);
    ihm.setEtatEtCouleur("msgSEIS_P5", tn_positionSEIS, couleurMsgSys[posBit(tn_positionSEIS)]);
    if (tn_positionSEIS === 1)           // SEIS sur plateau
        ihm.setEtat("btnSEIS", 0);
    else if (tn_positionSEIS === 4)
        ihm.setEtat("btnSEIS", 1);
};

/**
 *  Permet la mise a jour des messages et états du WTS
 * @param {type} tn_situationWTS
 * @returns {undefined}
 */
function majWTS(tn_situationWTS) {
    ihm.setEtatEtCouleur("msgWTS", tn_situationWTS, couleurMsgSys[posBit(tn_situationWTS)]);
    ihm.setEtatEtCouleur("msgWTS_P5", tn_situationWTS, couleurMsgSys[posBit(tn_situationWTS)]);
    if (tn_situationWTS === 1)           // WTS sur plateau
        ihm.setEtat("btnWTS", 0);
    else if (tn_situationWTS === 4)
        ihm.setEtat("btnWTS", 1);
};

/**
 *  Permet la mise a jour des messages et états du HP3
 * @param {type} tn_situationHP3
 * @returns {undefined}
 */
function majHP3(tn_situationHP3) {
    ihm.setEtatEtCouleur("msgHP3", tn_situationHP3, couleurMsgSys[posBit(tn_situationHP3)]);
    ihm.setEtatEtCouleur("msgHP3_P5", tn_situationHP3, couleurMsgSys[posBit(tn_situationHP3)]);
    if (tn_situationHP3 === 1)           // HP3 sur plateau
        ihm.setEtat("btnHP3", 0);
    else if (tn_situationHP3 === 4)
        ihm.setEtat("btnHP3", 1);
};

/**
 *  Permet la mise a jour des messages et états des Panneaux
 * @param {type} tn_etatPanneaux
 * @returns {undefined}
 */
function majPanneaux(tn_etatPanneaux) {
    var tn_etatPanneau1 = parseInt(tn_etatPanneaux & 0x0F);
    var tn_etatPanneau2 = parseInt((tn_etatPanneaux & 0xF0) >> 4);
    var panneau1HS = (tn_etatPanneau1 === 0x0f);
    var panneau2HS = (tn_etatPanneau2 === 0x0f);
    
    // --------- si c'est la phase d'initialisation --------------
    if (t_etatSysteme === 2) {
        if (!t_panneauxHS) {      // après traitement d'etatSysteme
            switch (tn_etatPanneau1) {
                case 0x0a : ihm.removeInit(0);  break;
                case 0x0b : ihm.showInit(0, 0, false); break;
                case 0x0f : ihm.showInit(0, 2, false); break;   // erreur init
                default   : ihm.showInit(0, 1, false); break;
            }
            switch (tn_etatPanneau2) {
                case 0x0a : ihm.removeInit(1);  break;
                case 0x0b : ihm.showInit(1, 0, false); break;
                case 0x0f : ihm.showInit(1, 2, false); break;   // erreur init
                default   : ihm.showInit(1, 1, false); break;
            }
        }
        else {  // si panneaux HS l'intialisation n'est pas bonne
            ihm.showInit(0, 2, true);
            ihm.showInit(1, 2, true);
        }
    }
    
    if (t_panneauxHS) {
        ihm.setEtatEtCouleur("msgPanneau1", 15, ihm.ERREUR);    // msg : "Hors service"
        ihm.setEtatEtCouleur("msgPanneau1_P5", 15, ihm.ERREUR); // msg : "Hors service"
        ihm.setEtatEtCouleur("msgPanneau2", 15, ihm.ERREUR);
        ihm.setEtatEtCouleur("msgPanneau2_P5", 15, ihm.ERREUR);
        ihm.disableButton("btnPanneaux");
    }
    else {      // si les panneaux ne sont pas HS
        if (t_sequenceEnCours === 0)
            ihm.enableButton("btnPanneaux");

        if ((tn_etatPanneau1 !== t_etatPanneau1) || initEtat) {
            t_etatPanneau1 = tn_etatPanneau1;
            ihm.setEtatEtCouleur("msgPanneau1", tn_etatPanneau1, couleurMsgPanneaux[tn_etatPanneau1]);
            ihm.setEtatEtCouleur("msgPanneau1_P5", tn_etatPanneau1, couleurMsgPanneaux[tn_etatPanneau1]);
        }
        if ((tn_etatPanneau2 !== t_etatPanneau2) || initEtat) {
            t_etatPanneau2 = tn_etatPanneau2;
            ihm.setEtatEtCouleur("msgPanneau2", tn_etatPanneau2, couleurMsgPanneaux[tn_etatPanneau2]);
            ihm.setEtatEtCouleur("msgPanneau2_P5", tn_etatPanneau2, couleurMsgPanneaux[tn_etatPanneau2]);
        }
        if (((tn_etatPanneau1 === 0) || panneau1HS)     // panneaux fermés
                && ((tn_etatPanneau2 === 0) || panneau2HS)) {
            ihm.setEtat("btnPanneaux", 0);
        }
        else if (((tn_etatPanneau1 === 3) || panneau1HS) // panneaux ouverts
                && ((tn_etatPanneau2 === 3) || panneau2HS)) {
            ihm.setEtat("btnPanneaux", 1);
        }
    }
};

/**
 * Met a jour les boutons en fonction de l'état du bras 
 * Met à jour l'état du bras
 * @param {type} tn_etatBras
 * @param {type} tn_situationSEIS
 * @param {type} tn_situationWTS
 * @returns {undefined}
 */
function majBras(tn_etatBras, tn_situationSEIS, tn_situationWTS) {
    var tn_positionSEIS = tn_situationSEIS & 0x7F;
    // --------------- phase d'initialisation ---------------
    if (t_etatSysteme === 2) {
        if (!t_brasHS) {       // après traitement d'etatSysteme
            switch (tn_etatBras) {
                case 0  : ihm.removeInit(2);  break;    // position inconnue
                case 32 : ihm.showInit(2, 0); break;    // init bras en cours
                case 1  : ihm.showInit(2, 1); break;    // position panoramique
                case 128 : ihm.showInit(2, 2); break;   // erreur d'init
            }
        }
        else {
            ihm.showInit(2, 2, true);
        }
    }          
    if (t_brasHS) {
        ihm.setEtatEtCouleur("msgBras", 256, ihm.ERREUR);    // msg : "Hors service"
        ihm.setEtatEtCouleur("msgBras_P5", 256, ihm.ERREUR); // msg : "Hors service"
        ihm.disableButton("btnHP3");  // masquer les boutons de commande
        ihm.disableButton("btnSEIS");
        ihm.disableButton("btnWTS");
    }
    else {
        // de 1 à 8 c'est une val, ensuite c'est un bit
        var k = tn_etatBras;
        if (k >= 16)
            k = posBit(tn_etatBras) + 2;
        ihm.setEtatEtCouleur("msgBras", tn_etatBras, couleurMsgBras[k]);
        ihm.setEtatEtCouleur("msgBras_P5", tn_etatBras, couleurMsgBras[k]);
        if (t_sequenceEnCours === 0) 
            ihm.enableButton("btnHP3");  // masquer les boutons de commande
        if (tn_positionSEIS === 0x01)       // SEIS sur plateau
            ihm.disableButton("btnWTS");
        else if (t_sequenceEnCours === 0)   // SEIS au sol
            ihm.enableButton("btnWTS"); 
        if (tn_situationWTS === 0x01)       // WTS sur plateau
            if (t_sequenceEnCours === 0) 
                ihm.enableButton("btnSEIS");
        else if (t_sequenceEnCours === 0) 
            ihm.disableButton("btnSEIS");
    }
};

/**
 * Met à jour l'état de la pince
 * @param {type} tn_etatPince
 * @param {type} t_brasHS
 * @returns {undefined}
 */
function majPince(tn_etatPince, t_brasHS) {
    // ajout en 0 d'un état inconnu identique à celui du bras, 
    // décale les autres états
    var etatPince = tn_etatPince + 1;
    if (t_brasHS)           // si bras dans état inconnu
        etatPince = 0;
    ihm.setEtatEtCouleur("msgPince", etatPince, couleurMsgPince[etatPince]);
    ihm.setEtatEtCouleur("msgPince_P5", etatPince, couleurMsgPince[etatPince]);
};

/**
 * permet l'affichage des tests pendant la phase de test
 * @param {type} code
 * @param {type} testEnCours
 * @param {type} nonTeste
 * @param {type} HS
 * @returns {undefined}
 */
function gererTest(code, testEnCours, nonTeste, HS) {
    if (testEnCours)
        ihm.showTest(code, 0);
    else if (nonTeste)        // donc il est testé
        ihm.removeTest(code);
    else if (HS)
        ihm.showTest(code, 2);
    else
        ihm.showTest(code, 1);
};

/**
 * retourne la position du premier bit non nul en partant de la droite
 * de 0 à 7
 * @param {type} valeur
 * @returns {Number}
 */
function posBit(valeur) {
    var pos = 1;
    if (valeur === 0) {     // tous les bits sont nuls
        return 0;
    }
    else {
        while (true) {      // on sort obligatoirement par le return
            if ((valeur & 0x01) === 0x01)
                return pos;
            else {
                valeur = valeur >> 1;
                pos++;
            }
        }
    }
};
