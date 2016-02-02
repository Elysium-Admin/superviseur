/*
   Auteur : BUGE Pascal
   
   
   Classe CIhm pour la mise à jour de l'IHM
   
   Modifications :
   31/03/2015    
        activation/désactivation des boutons par le css "pointer-events"
   Version 0.32 - 25/04/2015    Auteur : BUGE
        ajout de la fonction setEtatEtCouleur()
   Version beta6 - 25/05/2015    Auteur : BUGE
        ajout de la fonction setEtatCamera()

    Beta7 - 28/04/2015    Auteur : BUGE Pascal
        Ajout d'une popup spécifique pour ARU matérielle

*/

/* global t_CamHS, adrCamera, adrImage */

// ---------- Fonction exécutée après le chargement de la page --------------
// Constructeur
function CIhm() {
    this.NORMAL = 0;                    // etat statique d'un sous système
    this.ACTIF  = 1;                    // etat en mouvement d'un sous système
    this.ERREUR = 2;                    // etat en erreur d'un sous système
};

/**
 * 
 * @param {Int} num
 * @returns {Void}
 */
CIhm.prototype.setMsgInfo = function(num) {
    $("#msgInfo").attr('st', num);
    ml_updateEtat();
};

/**
 * met l'icone de la camera verte si elle marche, rouge sinon
 * @param {boolean} marche
 * @returns {Void}
 */
CIhm.prototype.setEtatCamera = function(marche) {
    if (marche)
        $("#imgEtatCamera").attr("src", "images/cameraverte.png");
    else
        $("#imgEtatCamera").attr("src", "images/camerarouge.png");
};

/**
 * affiche la trame dans la barre d'état, uniquement pour le debug
 * @param {String} texte
 * @returns {Void}
 */
CIhm.prototype.setMsg = function(texte) {
    $("#msgInfo").text(texte);
};
    
/**
 * Change immédiatement la couleur d'un message
 * status possibles   : NORMAL, ACTIF, ERREUR 
 * couleurs associées :   vert  ,  marron,   rouge
 * @param {Int} id : id de l'élément dont il faut changer le statut
 * @param {Int} status : nouveau statut du label par exemple : ihm.ERREUR
 * @returns {Void}
 */
CIhm.prototype.setCouleur = function(id, status) {
    switch (status) {
        case this.NORMAL : $('#' + id).css("color", "green");
            break;
        case this.ACTIF  : $('#' + id).css("color", "#a05000");
            break;
        case this.ERREUR : $('#' + id).css("color", "red");
            break;
    }
};

/**
 * Change le code courant d'un label ou d'un message
 * Il faut appeler ml_updateEtat() pour que le message soit mis à jour
 * Remarque : il faut globaliser cet appel après plusieurs setEtat()
 * Affiche la page correspondante
 * @param {Int} id : id de l'élément dont il faut changer l'état
 * @param {Int} code : nouveau code du label
 * @returns {Void}
 */
CIhm.prototype.setEtat = function(id, code) {
    $("#" + id).attr('st', code);
};

/**
 * // Change le code courant d'un label ou d'un message
 * Il faut appeler ml_updateEtat() pour que le message soit mis à jour
 * Remarque : il faut globaliser cet appel après plusieurs setEtat()
 * @param {Int} id : id de l'élément dont il faut changer l'état
 * @param {Int} code : nouveau code du label
 * @param {String} status : nouveau status du label : NORMAL, ACTIF, ERREU
 * couleurs associées :                                vert, marron, rouge
 * @returns {Void}
 */CIhm.prototype.setEtatEtCouleur = function(id, code, status) {
    $("#" + id).attr('st', code);
    this.setCouleur(id, status);
};

/**
 * Récupère l'état de l'élément passé en paramètre
 * @param {String} id : identifiant de l'élément du st à récupérer
 * @returns {Void}
 */
CIhm.prototype.getEtat = function(id) {
    return parseInt($("#" + id).attr("st"));
};

// utilise la librairie multiLangage.js
CIhm.prototype.update = function() {
    ml_updateEtat();
};

/**
 * Indique si un élément est visible ou caché
 * @param {String} id : identifiant de l'élément du st à récupérer
 * @returns {boolean} : true si l'élément est visible
 */
CIhm.prototype.isCache = function(id) {
    return ($("#" + id).hasClass("hide"));
};

/**
 * Montre un élément caché
 * @param {String} id : id de l'élément à montrer
 * @returns {Void}
 */
CIhm.prototype.montrer = function(id) {
    $("#" + id).removeClass("hide").addClass("show");
};

/**
 * Cache un élément
 * @param {String} id : id de l'élément à cacher
 * @returns {Void}
 */
CIhm.prototype.cacher = function(id) {
    $("#" + id).removeClass("show").addClass("hide");
};

/**
 * Affiche la page correspondante
 * A la page 4 si la caméra est en panne on montre le film
 * @param {Int} numero : entier correspondant au numéro de la page de 1 à 5
 * @returns {Void}
 */
CIhm.prototype.montrerPage = function(numero) {
    var num = parseInt(numero);
    if (num !== 4)
        this.cacher("imgEtatCamera");
    if (num === 1)
        this.cacher("divBatterie");
    if (num >= 2)
        this.montrer("divBatterie");
    if (num === 2)
        this.removeAllTest();
    if (num === 3)
        this.removeAllInit();
    if (num >= 3) {
        this.montrer("imgEtatCamera");
        if (!t_CamHS) {     // si la caméra n'est pas HS, on la montre
            $("#imgCamera").attr("src", adrCamera);
            this.montrer("imgCamera");
            this.cacher("vidFilm");
        }
        else {
            this.montrer("vidFilm");
            this.cacher("imgCamera");
            $("#imgCamera").attr("src", adrImage);
        }
    }
    $.mobile.changePage("#page" + num);
};

/**
 * Active tous les boutons de commande
 * @returns {Void}
 */
CIhm.prototype.enableAllCmdButton = function() {
    $(".btnCmd").css("opacity", 1 );
    $(".btnCmd").css("pointer-events", "auto");
};

/**
 * Désactive tous les boutons de commande sauf l'aru
 * @returns {Void}
 */
CIhm.prototype.disableAllCmdButton = function() {
    $(".btnCmd").css("opacity", 0.5 );
    $(".btnCmd").css("pointer-events", "none");
};


/**
 * Active un bouton
 * @param {String} id
 * @returns {Void}
 */
CIhm.prototype.enableButton = function(id) {
    $("#" + id).css("opacity", 1 );
    $("#" + id).css("pointer-events", "auto");
};

/**
 * Désactive un bouton
 * @param {String} id 
 * @returns {Void}
 */
CIhm.prototype.disableButton = function(id) {
    $("#" + id).css("opacity", 0.5 );
    $("#" + id).css("pointer-events", "none");
};

/**
 * Vérifie si le bouton est activé
 * @param {String} id : id du bouton
 * @returns {Boolean}
 */
CIhm.prototype.isButtonEnable = function(id) {
    return (parseInt($("#" + id).css("opacity")) === 1);
};

/**
 * Ouvre une popup de confirmation
 * @param {Int} num : numéro de la popup de confrmation
 * @returns {Void}
 */
CIhm.prototype.dmdConfirmation = function(num) {
    $("#msgPopConfirm").attr('st', num);
    ml_updateEtat();
    this.cacher("btnPopPbNivellement");
    this.cacher("fldPopConfirm");
    this.openPopupBas("popConfirm");
};

/**
 * Ouvre une popup de confirmation avec option de dépannage
 * @param {Int} num : numéro de la popup de confrmation
 * @returns {Void}
 */
CIhm.prototype.dmdConfirmationAvecOption = function(num) {
    // rendre visible la non confirmation
    this.montrer("btnPopPbNivellement");
    $("#msgPopConfirm").attr('st', num);
    ml_updateEtat();
    this.openPopupBas("popConfirm");
};

/**
 * Ouvre une popup de décision
 * @param {Int} num : numéro de la popup de décision
 * @returns {Void}
 */
CIhm.prototype.dmdDecision = function(num) {
    $("#msgPopDecision").attr('st', num);
    ml_updateEtat();
    this.openPopupBas("popDecision");
};

/**
 * Ouvre une popup de décision pour le dépannage
 * @param {Int} num : numéro de la popup de décision
 * @returns {Void}
 */
CIhm.prototype.dmdDepannageDecision = function(num) {
    $("#msgDepannageDecision").attr('st', num);
    ml_updateEtat();
    this.openPopupBas("popDepannageDecision");
};

/**
 * affiche la popup d'Arrêt d'urgence
 * @param {Int} type : 0 (logiciel) ou 1 (matériel) 2 (logicielle sans bouton)
 * @returns {Void}
 */
CIhm.prototype.openARU = function(type) {
    if (type >= 1)         // ARU matériel, pas de fin d'ARU par bouton
        this.cacher("btnPopAruOK");
    else
        this.montrer("btnPopAruOK");
    $("#msgPopARU").attr('st', type);
    $("#txtPopARU").attr('st', type);
    ml_updateEtat();
    this.openPopup("popARU");
};

/**
 * Donne le nom de la popup active
 * @returns {String}    le nom de la popup active ou "" sinon
 */
CIhm.prototype.idPopupActive = function() {
    var trouve = false;
    if (this.isPopupActive()) {
        $("[data-role='popup']").each (function (index) {
            // A priori on ne peut appeler un proto dans un each()
            // remplacement de this.isIdPopupActive(this.id) par son traitement
            if ($( "#" + this.id + "-popup" ).hasClass("ui-popup-active")) {
                popupId = this.id;
                trouve = true;
            }
        });
    }
    if (trouve)
        return popupId;
    else
        return ""; 
};

/** 
 * teste si la popup donnée est active
 * @param {String} id la popup à tester
 * @returns {Boolean}   retourne true si cette popup est active
 */
CIhm.prototype.isIdPopupActive = function(id) {
    if ($( "#" + id + "-popup" ).hasClass("ui-popup-active"))
        return true;
    else
        return false;
};

/**
 * Teste si il y a une popup active
 * @returns {Boolean}   retourne true si une popup modale est active
 */
CIhm.prototype.isPopupActive = function() {
    if ($(".ui-popup-active").length > 0)
        return true;
    else
        return false;
};

/**
 * Teste si il y a une popup active et modale
 * @returns {Boolean}   retourne true si une popup modale est active
 */
CIhm.prototype.isPopupModaleActive = function() {
    if ((this.isPopupActive)
        && ($("#" + this.idPopupActive()).attr("data-dismissible") === "false"))
        return true;
    else
        return false;
};

/** 
 * Ouvre la popup donnée en paramètre
 * On ne peut pas ouvrir et fermer deux popup en même temps, 
 * Il faut un délai entre la fermeture et l'ouverture de la nouvelle popup.
 * Si une popup est déjà ouverte, forcer sa fermeture avant d'ouvrir l'autre.
 * On ne ferme pas une popup modale sauf si la popup à ouvrir c'est l'ARU !
 * @param {String} idToOpen   la popup à ouvrir sous les tests ou init
 * @returns {Void} 
 */
CIhm.prototype.openPopup = function(idToOpen) {
    if (!this.isPopupModaleActive() || (idToOpen === "popARU")) {
        if (this.isPopupActive()) {
            this.closePopup();
            setTimeout(function() { $("#" + idToOpen).popup("open"); }, 100);
        }
        else {
            $("#" + idToOpen).popup("open");
        }
    }
};

/** Ouvre une popup en bas et centré pour X
 * On ne peut pas ouvrir deux popups en même temps
 * Si une popup est déjà ouverte, forcer sa fermeture avant d'ouvrir l'autre
 * Il faut un délai entre la fermeture et l'ouverture de la nouvelle popup
 * @param {Int} idToOpen   la popup à ouvrir sous les tests ou init
 * @returns {Void} 
 */
CIhm.prototype.openPopupBas = function(idToOpen) {
    if (this.isPopupActive()) {
        this.closePopup();
        setTimeout(function() { $("#"+idToOpen).popup("open", { y: 520});}, 100);
    }
    else {
        $("#" + idToOpen).popup("open", { y: 520});
    }
};

// Si une popup est ouverte on retrouve son Id et on la ferme
CIhm.prototype.closePopup = function() {
    if (this.isPopupActive() && !this.isIdPopupActive("popARU")) {
        $("#" + this.idPopupActive()).popup("close");
    }
};

// Si une popup est ouverte on retrouve son Id et on la ferme
CIhm.prototype.closePopupARU = function() {
    if (this.isIdPopupActive("popARU"))
        $("#popARU").popup("close");
};

/**
 * ajoute une ligne de test au tableau des comptes rendus de test
 * @param {Int} codeTest  le code du test à ajouter
 * @param {Int} codeOK    le css met "0" en orange, "1" en vert et "2" en rouge
 *                        le multilangue met "1" à OK et "1" à "PB"
 * @returns {Void}
 */
CIhm.prototype.addTest = function(codeTest, codeOK) {
    var codeTest2 = codeTest;
    if (codeOK >= 1)
        codeTest2 +=10;
    $("#tbdTest_P2").append("<tr id='trTest" + codeTest + "'>"
        + "<td id='tdCodeTest' test='tdTest" + codeTest 
            + "' st='" + codeTest2 + "'></td>"
        + "<td  id='tdOK' test='tdTestOK" + codeTest 
            + "' st='" + codeOK + "'></td></tr>");
    ml_updateEtat();
};

/**
 * ajoute une ligne de test ou modifie le test si celui ci est déjà affiché
 * @param {Int} codeTest  le code du test à ajouter ou modifier
 * @param {Int} codeOK    le css met "0" en orange, "1" en vert et "2" en rouge
 *                        le multilangue met "1" à OK et "1" à "PB"
 * @returns {Void}
 */
CIhm.prototype.showTest = function(codeTest, codeOK) {
    var codeTest10 = codeTest;
    if (codeOK >= 1)
        codeTest10 +=10;
    if ($("#trTest" + codeTest).length > 0) {
        $("#tdCodeTest[test='tdTest" + codeTest + "']").attr('st', codeTest10);
        $("#tdOK[test='tdTestOK" + codeTest).attr('st', codeOK);
        ml_updateEtat();
    }
    else
        this.addTest(codeTest, codeOK);
};

/**
 * supprime le test du tableau des comptes rendus de test
 * @param {Int} codeTest    le code du test à supprimer
 * @returns {Void}
 */
CIhm.prototype.removeTest = function(codeTest) {
    $("#trTest" + codeTest).remove();
};

/**
 * supprime toutes les lignes d test au tableau des comptes rendus sauf l'entête
 * @returns {Void}
 */
CIhm.prototype.removeAllTest = function() {
    $("#tbdTest_P2 tr").remove();
};

/**
 * ajoute une ligne d'init au tableau des comptes rendus d'init
 * @param {Int} codeInit  le code de l'init à ajouter
 * @param {Int} codeOK    le css met "0" en orange, "1" en vert et "2" en rouge
 *                        le multilangue met "1" à OK et "1" à "PB"
 * @returns {Void}
 */
CIhm.prototype.addInit = function(codeInit, codeOK) {
    var codeInit2 = codeInit;
    if (codeOK >= 1)
        codeInit2 +=10;
    $("#tbdInit_P3").append("<tr id='trInit" + codeInit + "'>"
        + "<td id='tdCodeInit' init='tdInit" + codeInit + "' st='" + codeInit2 + "'></td>"
        + "<td  id='tdOK' init='tdInitOK" + codeInit + "' st='" + codeOK + "'></td></tr>");
    ml_updateEtat();
};

/**
 * ajoute une ligne d'init ou modifie l'init si celui-ci est déjà affiché
 * @param {Int} codeInit  le code de l'init à ajouter ou modifier (0,1,2)
 * @param {Int} codeOK    le css met "0" en orange, "1" en vert et "2" en rouge
 *                        le multilangue met "1" à OK et "1" à "PB"
 * @param {Int} hs        si le systeme est hs le code va en 20
 * @returns {Void}
 */
CIhm.prototype.showInit = function(codeInit, codeOK, hs) {
    var codeInit10 = codeInit;
    if (hs)
        codeInit10 +=20;                            // texte de 20 à 22
    else if (codeOK >= 1)
        codeInit10 +=10;                            // texte de 10 à 12
    if ($("#trInit" + codeInit).length > 0) {       // si le message existe
        $("#tdCodeInit[Init='tdInit" + codeInit + "']").attr('st', codeInit10);
        $("#tdOK[Init='tdInitOK" + codeInit).attr('st', codeOK);
        ml_updateEtat();
    }
    else
        this.addInit(codeInit, codeOK);
};

/**
 * supprime l'init du tableau des comptes rendus d'init
 * @param {Int} codeInit    le code de l'init à supprimer
 * @returns {Void}
 */
CIhm.prototype.removeInit = function(codeInit) {
    $("#trInit" + codeInit).remove();
};

/**
 * supprime toutes les lignes d'init au tableau des comptes rendus sauf l'entête
 * @returns {Void}
 */
CIhm.prototype.removeAllInit = function() {
    $("#tbdInit_P3 tr").remove();
};