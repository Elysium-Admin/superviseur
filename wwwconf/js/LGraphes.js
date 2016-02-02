/* 
 * Auteur : CAMPI Enzo
 * 
 * Création de 3 graphes X, Y, Z avec canvasJS
 * Nous ne sommes pas arrivé à en faire une classe js
 * surement à cause de la notion de callback sur événement SSE
 */

var dpsX = [];          // les points sur X
var dpsY = [];          // les points sur Y
var dpsZ = [];          // les points sur Z
// var NBVAL = 50;         // nb de points ajoutés à chaque intervalle de temps
// var INTERVAL = 500;     // durée d'un intervalle de temps en ms
var DATA_LENGHT = 3000; // nombre de points visibles dans le graphe (30s)
var chartXYZ;       // variable qui va stocker les graphs réunis
var chartX;         // premier graphique
var chartY;         // deuxième graphique
var chartZ;         // troisième graphique
var graphesReset = false;
var graphesPause = false;
var graphesPauseOld = false;

window.onload = function () {
    // ------------------ 3 graphes -----------------------
    chartX = new CanvasJS.Chart("divChartX", {  // on instancie le graphique par son ID
        axisX:{ valueFormatString: "hh:mm:ss"}, // format des valeurs sur l'axe des X
        axisY: { valueFormatString: "#0mg" },   // format des valeurs sur l'axe des Y
        includeZero: true,      // affiche le 0 à l'origine
        backgroundColor: "#fdf1e3",	// couleur de fond
        // data est un conteneur dans lequel on indique le type de graphique, 
        // sa couleur ainsi que le nom de la variable qui va stocker les points
        data: [{ type: "line", color: "orange", dataPoints: dpsX }]
    });
    chartY = new CanvasJS.Chart("divChartY", {
        axisX:{ valueFormatString: "hh:mm:ss"},
        axisY: {valueFormatString: "#0mg"},
        includeZero: true,
        backgroundColor: "#fdf1e3",		
        data: [{ type: "line", color: "green", dataPoints: dpsY }]
    });
    chartZ = new CanvasJS.Chart("divChartZ", {
        axisX:{ valueFormatString: "hh:mm:ss"},
        axisY: {valueFormatString: "#0mg"},
        includeZero: true,
        backgroundColor: "#fdf1e3",		
        data: [{ type: "line", color: "darkBlue", dataPoints: dpsZ }]
    });
    // ------------------ mono graphe --------------------
    // ici le but est de réunir les données des trois graphiques en un seul 
    // avec le zoom possible
    chartXYZ = new CanvasJS.Chart("divChartXYZ", {
        axisX:{ valueFormatString: "hh:mm:ss"},
        axisY: { valueFormatString: "#0mg" },
        includeZero: true,
        backgroundColor: "#fdf1e3",
        // zoomEnabled: true,
        // data va contenir les points des trois graphiques créés précedemment
        data: [
            { type: "line", color: "orange", dataPoints: dpsX },
            { type: "line", color: "green", dataPoints: dpsY },
            { type: "line", color: "darkBlue", dataPoints: dpsZ }
        ]
    });
};

/* ------------------ traiter la trame sismometre -----------------------------
 * Cette trame contient N datas sismométrique 
 * Une donnée sismométrique est constitue de : sXXXX,sYYYY,sZZZZ (17 caractères)
 * Les données sont séparées par ';'
 * Mots clef Requête mv (mesure vibration)
 * s : signe, XXXX : valeur des vibrations en X
 * t : signe, YYYY : valeur des vibrations en Y
 * u : signe, ZZZZ : valeur des vibrations en Z
 * Symbole de fin de trame ;
 * Par exemple : 
 *   +0000,+0000,+0000;+0003,+0001,+0001;+0012,+0003,+0002;+0007,+0001,+0000;
 *   Au max : 50 données par trame par 1/2s => 50x18 cars = 900 cars
 *   La trame se terminera par un zéro (null)
 ----------------------------------------------------------------------------- 
 */
function g_updateDonnees(trame) {       // gère la mise à jour des données dans les graphs
    // si on est en pause on ne fait rien
    if (graphesPause) {
        graphesPauseOld = true;
        return;
    }
    // si c'est une reprise après pause, on vide les anciennes données
    else if (graphesPauseOld) {
        graphesPauseOld = false;
        graphesReset = true;
        return;
    }
    else if (graphesReset) {
        graphesReset = false;
        // on vide toutes les données précédentes
        var nbDatas = dpsX.length;
        for (var i = 0; i < nbDatas; i++) {
            dpsX.shift(); dpsY.shift(); dpsZ.shift();
        }
    }
    // ------------ ici on ajoute les données de la trame reçue ---------------
    var date = new Date();
    var time = date.getTime();
    var donnees = trame.split(";");   // Je sépare les mesures (toutes les 10ms)
    var nbDonnees = donnees.length - 1; // une donnee de plus que prévu !

    for (var i = 0; i < nbDonnees; i++) {
        var valeurs = donnees[i].split(","); // je sépare les valeurs en X,Y,Z
        dpsX.push({x: date, y: parseInt(valeurs[0])});
        dpsY.push({x: date, y: parseInt(valeurs[1])});
        dpsZ.push({x: date, y: parseInt(valeurs[2])});
        time += 10;                     // ajout de 10ms, acquisition à 100 Hz
        date = new Date(time);
    }
    // pas plus de 30s de données
    if (dpsX.length > DATA_LENGHT) {
        nbEnTrop = dpsX.length - DATA_LENGHT;
        for (var i = 0; i < nbEnTrop; i++) {
            dpsX.shift();   dpsY.shift();   dpsZ.shift();   // retire le premier point du tableau
        }
    }
    g_refresh();
}

function g_resetDatas() {   // reset les données présentes dans les graphiques
    graphesReset = true;
}

function g_refresh() {
    chartXYZ.render();      // mise à jour du graphe de la vue monographe
    chartX.render();        // mise à jour des 3 graphes de la vue multigraphes
    chartY.render();
    chartZ.render();
}

function g_pause() {        // met en pause les graphiques
    graphesPause = true;
}

function g_start() {
    graphesPause = false;   // A mettre dés le début
}

