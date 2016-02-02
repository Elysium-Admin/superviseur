CONFCGI_DIR=./CGI-CONF
OBSCGI_DIR=./CGI-OBS
CONF=$(CONFCGI_DIR)/confcgi
OBS=$(OBSCGI_DIR)/obscgi

all: $(CONF) $(OBS) 

$(CONF):
	@(cd $(CONFCGI_DIR) && $(MAKE))

$(OBS):
	@(cd $(OBSCGI_DIR) && $(MAKE))


