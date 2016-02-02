/*
 * Auteur : BUGE Pascal
 * 
 * Définition de la librairie MultiLangue
 * Référence : https://github.com/dakk/jquery-multilang
 * Permet de charger les textes affichés dans l'IHM en fonction de la langue
 * et en fonction de l'état d'un sous système
 */

// variable globale
var ml_langCode = "fr";

/**
 * Remplace tous les contenus HTML des elements avec un attibut 'st'
 * @param {String[String]} jsdata : Tableau de la forme "Référence" : "Valeur"
 * par ex: "btnQuitter" : "Quitter",
 * @returns {Void}
 */
var ml_translate = function (jsdata) {	// charge le json sous forme de tableau 
    $("[st]").each (function (index) { // pour chaque st du tableau, on leur attribue un index
        var id = $(this).attr ('id'); // récupère l'id de l'élément st actuel
        var etat = $(this).attr ('st'); // récupère l'état de l'id
        var ref = id;
        if (etat != "") // si l'état de l'élément id peut prendre différents états
            ref += "_" + etat; // on ajoute "_" + "état"
        var strTr = jsdata [ref]; // la variable strTr prend la valeur du nom de l'élément 
        $(this).text (strTr);  // écrit le texte associé à la variable strTr du tableau jsdata
    });
};

/**
 * Change la langue en changeant de fichier .json
 * Appelle ml_translate qui remplace tous les contenus HTML des elements avec un
 * attibut 'st'.
 * @param {String} langage : la langue de traduction "fr","us"
 * @returns {Void} 
 */
function ml_setLangue(langage) {
    ml_langCode = langage;
    $.getJSON('lang/' + ml_langCode + '.json', ml_translate); // Récupère un nouveau json
    // On change les drapeaux
    $("#imgLangue_P2").attr("src", "images/drapeau_" + ml_langCode + ".png"); // Récupère le drapeau associé
    $("#imgLangue_P2").attr("alt", ml_langCode); // Si l'image ne s'affiche pas on affiche la langue
    $("#imgLangue").attr("src", "images/drapeau_" + ml_langCode + ".png");
    $("#imgLangue").attr("alt", ml_langCode);
};

// Met à jour les id avec st. (plus utilisé)
function ml_update() { // Met a jour les id avec st.
    ml_setLangue(ml_langCode);
};

// remplace tous les contenus HTML des elements avec un
// attibut 'st' défini et non nul dans la langue courante
// A appeler après ml_setEtat(label, etat)
function ml_updateEtat() {
    $.getJSON('lang/' + ml_langCode + '.json', ml_setNewEtat);
};

/**
 * Remplace tous les contenus HTML des elements avec un
 * attibut 'st' défini dans le .json et non nul
 * @param {String[String]} jsdata : Tableau de la forme ("Référence" : "Valeur")
 * par ex: "btnQuitter" : "Quitter",
 * @returns {Void}
 */
var ml_setNewEtat = function (jsdata) {
    $("[st]").each (function (index) {
        var id = $(this).attr ('id');
        var etat = $(this).attr ('st');
        if (etat !== "") {
            var strTr = jsdata [id + "_" + etat];
            $(this).html (strTr);
        }
    });
};
