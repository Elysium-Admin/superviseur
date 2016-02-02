/*
 * Auteur : BUGE Pascal
 * 
 * 
 * Classe CBatterie pour la mise à jour de l'affichage de la batterie
 */

// Constructeur
function CBatterie() {
    // définition des attributs de la classe
    this.seuilChargeRouge = 10.0;       // Seuil où la charge batterie est rouge
    this.seuilChargeOrange = 20.0;      // Seuil où la charge batterie est orange
    this.tensionBatteries = 0.0;        // Tension batteries (en Volts)
    this.chargeBatteries = 0.0;         // Charges des batteries (en %)
    this.courantBatteries = 0.0;        // Courant dans les batteries (en Ampères)
    this.chargeConsommees = 0.0;        // Charge consommée (en %)
    this.dureeRestanteBatteries = 0;    // Durée restante dans batteries (en mn)
    this.codeIncidentEnergie = 0;       // CODE D'INCIDENT ENERGIE
};

/**
 * Indique que la batterie est en panne
 * @param {Boolean} yes : true si on a un problème grave avec la batterie
 * @returns {Void}
 */
CBatterie.prototype.setPanne = function(yes) {
    if (yes) {
        $("#divBatterieFond").css("backgroundColor", "red");
        $("#divBatterieCharge").removeClass("show").addClass("hide");
    }
    else {
        $("#divBatterieFond").css("backgroundColor", "#feedd8");
        $("#divBatterieCharge").removeClass("hide").addClass("show");
    }
};

/**
 * Change la tension en volt
 * @param {Double} tension : réel en volt
 * @returns {Void}
 */
CBatterie.prototype.setTension = function(tension) {
    this.tensionBatteries = tension;
};

/**
 * Change le niveau de la batterie en % et la couleur (vert, orange, rouge)
 * @param {Double} charge : réel entre 0 et 100 %
 * @returns {Void}
 */
CBatterie.prototype.setCharge = function(charge) {
    if (charge < 0)
        charge = 0;
    else if (charge > 100)
        charge = 100;
    this.chargeBatteries = charge;
    var longueur = Math.floor(this.chargeBatteries*58/100);// pour que le carré vert s'adapte au pourcentage de batterie
    $("#divBatterieCharge").css("width", longueur + "px");
    if (charge < this.seuilChargeRouge)
        $("#divBatterieCharge").css("backgroundColor", "red");
    else if (charge < this.seuilChargeOrange)
        $("#divBatterieCharge").css("backgroundColor", "orange");
    else 
        $("#divBatterieCharge").css("backgroundColor", "green");
};

/**
 * Change la valeur du courant en Ampère
 * @param {double} courant : réel exprimé en ampère
 * @returns {void}
 */
CBatterie.prototype.setCourant = function(courant) {
    this.courantBatteries = courant;
};

/**
 * Change la charge consommée en %
 * @param {Double} consommee : réel exprimée en %
 * @returns {Void}
 */
CBatterie.prototype.setConsommee = function(consommee) {
    this.chargeConsommees = consommee;
};

/**
 * Change la durée restante de la batterie (en minute)
 * @param {Int} duree : heure exprimée en minute
 * @returns {Void}
 */
CBatterie.prototype.setDuree = function(duree) {
    if (duree < 0)
        duree = 0;
    this.dureeRestanteBatteries = duree;
    var hours = Math.floor(this.dureeRestanteBatteries / 60);
    var minutes = Math.floor(this.dureeRestanteBatteries % 60);
    // avec mise au format HHhMM de l'affichage
    var res = ((hours < 10) ? '0' + hours : hours) + 'h' 
                                + ((minutes < 10) ? '0' + minutes : minutes);
    $("#lblBatterie").html(res);
};

/**
 * Change le code d'incident
 * @param {Int} code : entier exprimant un code
 * @returns {Void}
 */
CBatterie.prototype.setCode = function(code) {
    this.codeIncidentEnergie = code;
};
