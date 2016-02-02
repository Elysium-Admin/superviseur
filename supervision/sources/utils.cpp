#include "../defs/utils.h"

//=========================================================================================
bool ishexadec(char c)
{
	if ( ((c>='0')&&(c<='9')) || ((c>='a')&&(c<='f')) || ((c>='A')&&(c<='F')) ) return true;
	return false;
}


//=========================================================================================
bool IsChaineNumeric(string chaine)
{
	int lg=chaine.length();
	for(int i = 0; i<lg; i++)
	{
		if ((chaine[i]<'0')||(chaine[i]>'9'))return false;
	}
	return true;
}

//=========================================================================================
bool IsAdrsEthernet(string chaine)
{
	if(chaine.length()!=LG_ASCII_ETH)return false;
	if(! ishexadec(chaine[0]))return false;
	if(! ishexadec(chaine[1]))return false;
	if((chaine[2]!=':')&&(chaine[2]!='-'))return false;
	else chaine[2]=':';
	if(! ishexadec(chaine[3]))return false;
	if(! ishexadec(chaine[4]))return false;
	if((chaine[5]!=':')&&(chaine[5]!='-')) return false;
	else chaine[5]=':';
	if(! ishexadec(chaine[6]))return false;
	if(! ishexadec(chaine[7]))return false;
	if((chaine[8]!=':')&&(chaine[8]!='-')) return false;
	else chaine[8]=':';
	if(! ishexadec(chaine[9]))return false;
	if(! ishexadec(chaine[10]))return false;
	if((chaine[11]!=':')&&(chaine[11]!='-')) return false;
	else chaine[11]=':';
	if(! ishexadec(chaine[12]))return false;
	if(! ishexadec(chaine[13]))return false;
	if((chaine[14]!=':')&&(chaine[14]!='-')) return false;
	else chaine[14]=':';
	if(! ishexadec(chaine[15]))return false;
	if(! ishexadec(chaine[16]))return false;
	return true;
}

//=========================================================================================
bool  convAsciiToEther(string source, u_char * ptrCible)
{
	string s_octet;
	int lastpos=-1;
	int newpos;
	istringstream iss;
	int val;
	if(! IsAdrsEthernet(source))return false;
	for(int i =0; i< ETH_HW_ADDR_LEN ; i++)
	{
		newpos=source.find_first_of(':',lastpos+1);
		s_octet=source.substr (lastpos+1,newpos-lastpos-1);
		iss.clear();
		iss.str(s_octet.c_str());
		iss >>hex>>val;
		ptrCible[i]=0x000000ff&val;
		lastpos=newpos;
	}
	return true;
}
//=========================================================================================


void  convEtherToAscii(u_char * ptrSource, string & Cible)
{
	ostringstream str_hdrs;
	str_hdrs.str("");
	str_hdrs<<hex<<setfill('0')<<setw(2)<<(int)ptrSource[0];
	str_hdrs<<":"<<hex<<setfill('0')<<setw(2)<<(int)ptrSource[1];
	str_hdrs<<":"<<hex<<setfill('0')<<setw(2)<<(int)ptrSource[2];
	str_hdrs<<":"<<hex<<setfill('0')<<setw(2)<<(int)ptrSource[3];
	str_hdrs<<":"<<hex<<setfill('0')<<setw(2)<<(int)ptrSource[4];
	str_hdrs<<":"<<hex<<setfill('0')<<setw(2)<<(int)ptrSource[5];
	Cible= str_hdrs.str();

}
//=========================================================================================

bool  convAsciiToIp(string source, u_char * ptrCible)
{
	string s_octet;
	int lastpos=-1;
	int newpos;
	istringstream iss;
	int val;
	for(int i =0; i< IP_ADDR_LEN  ; i++)
	{
		newpos=source.find_first_of('.',lastpos+1);
		s_octet=source.substr (lastpos+1,newpos-lastpos-1);
		if(! IsChaineNumeric(s_octet))return false;
		iss.clear();
		iss.str(s_octet.c_str());
		iss >>val;
		if((val>=0)&&(val<=255))ptrCible[i]=val;
		else return false;
		lastpos=newpos;
	}
	return true;
}
//=========================================================================================

void  convIPToAscii(u_char * ptrSource, string & Cible)
{
	ostringstream str_ipadr;
	str_ipadr.str("");
	str_ipadr<<(int)ptrSource[0];
	str_ipadr<<"."<<(int)ptrSource[1];
	str_ipadr<<"."<<(int)ptrSource[2];
	str_ipadr<<"."<<(int)ptrSource[3];
	Cible= str_ipadr.str();
}
//=========================================================================================
string lireShortHexa(u_short * src)
{
	u_short inverse;
	ostringstream stresult;
	stresult.str("");
	inverse=((src&&0xff00)>>8)+((src&&0x00ff)<<8);
	stresult<<"0x"<<hex<<setfill('0')<<setw(4)<<inverse;
	return stresult.str();
}
