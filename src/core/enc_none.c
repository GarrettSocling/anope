/* Module for plain text encryption.
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * This program is free but copyrighted software; see the file COPYING for
 * details.
 */

#include "module.h"

class ENone : public Module
{
 public:
	ENone(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(ENCRYPTION);

		ModuleManager::Attach(I_OnEncrypt, this);
		ModuleManager::Attach(I_OnEncryptInPlace, this);
		ModuleManager::Attach(I_OnEncryptCheckLen, this);
		ModuleManager::Attach(I_OnDecrypt, this);
		ModuleManager::Attach(I_OnCheckPassword, this);
	}

	EventReturn OnEncrypt(const char *src,int len,char *dest,int size) 
	{
		if(size>=len) 
		{
			memset(dest,0,size);
			strncpy(dest,src,len);
			dest[len] = '\0';
			return EVENT_STOP; 
		}
		return EVENT_ERROR; 
	}

	EventReturn OnEncryptInPlace(char *buf, int size) 
	{
		return EVENT_STOP; 
	}

	EventReturn OnEncryptCheckLen(int passlen, int bufsize) 
	{
		if(bufsize>=passlen) {
			return EVENT_STOP;  
		}
		return EVENT_ALLOW; // return 1 
	}

	EventReturn OnDecrypt(const char *src, char *dest, int size) {
		memset(dest,0,size);
		strncpy(dest,src,size);
		dest[size] = '\0';
		return EVENT_ALLOW;
	}

	EventReturn OnCheckPassword(const char *plaintext, const char *password) {
		if(strcmp(plaintext,password)==0) 
		{
			return EVENT_ALLOW;
		}
		return EVENT_CONTINUE;
	}

};
/* EOF */


MODULE_INIT("enc_none", ENone)
