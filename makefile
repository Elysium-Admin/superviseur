SUPER_DIR=./supervision
OUTILS_DIR=./outils
CGI_DIR=LesCGI
CGICONF_DIR=$(CGI_DIR)/CGI-CONF
CGIOBS_DIR=$(CGI_DIR)/CGI-OBS
SUPVISR=$(SUPER_DIR)/super
OUTILS=$(OUTILS_DIR)outils
LESCGI=$(CGI_DIR)/lescgi

WEBCONF=./wwwconf
WEBOBS=./wwwobs

all: $(SUPVISR) $(OUTILS) $(LESCGI)

$(SUPVISR):
	@(cd $(SUPER_DIR) && $(MAKE))

$(OUTILS):
	@(cd $(OUTILS_DIR) && $(MAKE))

$(LESCGI):
	@(cd $(CGI_DIR) && $(MAKE))

install:
	sudo cp $(CGICONF_DIR)/cgiCommande $(WEBCONF)/cgi-bin/cgiCommande.cgi
	sudo cp $(CGICONF_DIR)/cgiStream $(WEBCONF)/cgi-bin/cgiStream.cgi
	sudo chown -R www-data:www-data $(WEBCONF)/cgi-bin/

	sudo cp $(CGIOBS_DIR)/cgiStream  $(WEBOBS)/cgi-bin/cgiStream.cgi
	sudo chown -R www-data:www-data $(WEBOBS)/cgi-bin/

