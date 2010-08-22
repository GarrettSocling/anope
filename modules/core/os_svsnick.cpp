/* OperServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandOSSVSNick : public Command
{
 public:
	CommandOSSVSNick() : Command("SVSNICK", 2, 2, "operserv/svsnick")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params[0];
		Anope::string newnick = params[1];
		User *u2;

		NickAlias *na;

		/* Truncate long nicknames to NICKMAX-2 characters */
		if (newnick.length() > Config->NickLen)
		{
			notice_lang(Config->s_OperServ, u, NICK_X_TRUNCATED, newnick.c_str(), Config->NickLen, newnick.c_str());
			newnick = params[1].substr(0, Config->NickLen);
		}

		/* Check for valid characters */
		if (newnick[0] == '-' || isdigit(newnick[0]))
		{
			notice_lang(Config->s_OperServ, u, NICK_X_ILLEGAL, newnick.c_str());
			return MOD_CONT;
		}
		for (unsigned i = 0, end = newnick.length(); i < end; ++i)
			if (!isvalidnick(newnick[i]))
			{
				notice_lang(Config->s_OperServ, u, NICK_X_ILLEGAL, newnick.c_str());
				return MOD_CONT;
			}

		/* Check for a nick in use or a forbidden/suspended nick */
		if (!(u2 = finduser(nick)))
			notice_lang(Config->s_OperServ, u, NICK_X_NOT_IN_USE, nick.c_str());
		else if (finduser(newnick))
			notice_lang(Config->s_OperServ, u, NICK_X_IN_USE, newnick.c_str());
		else if ((na = findnick(newnick)) && na->HasFlag(NS_FORBIDDEN))
			notice_lang(Config->s_OperServ, u, NICK_X_FORBIDDEN, newnick.c_str());
		else
		{
			notice_lang(Config->s_OperServ, u, OPER_SVSNICK_NEWNICK, nick.c_str(), newnick.c_str());
			ircdproto->SendGlobops(OperServ, "%s used SVSNICK to change %s to %s", u->nick.c_str(), nick.c_str(), newnick.c_str());
			ircdproto->SendForceNickChange(u2, newnick, time(NULL));
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config->s_OperServ, u, OPER_HELP_SVSNICK);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_OperServ, u, "SVSNICK", OPER_SVSNICK_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_OperServ, u, OPER_HELP_CMD_SVSNICK);
	}
};

class OSSVSNick : public Module
{
	CommandOSSVSNick commandossvsnick;

 public:
	OSSVSNick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		if (!ircd->svsnick)
			throw ModuleException("Your IRCd does not support SVSNICK");

		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandossvsnick);
	}
};

MODULE_INIT(OSSVSNick)