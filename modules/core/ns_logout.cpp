/* NickServ core functions
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

class CommandNSLogout : public Command
{
 public:
	CommandNSLogout() : Command("LOGOUT", 0, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string nick = !params.empty() ? params[0] : "";
		Anope::string param = params.size() > 1 ? params[1] : "";
		User *u2;

		if (!u->Account()->IsServicesOper() && !nick.empty())
			this->OnSyntaxError(u, "");
		else if (!(u2 = (!nick.empty() ? finduser(nick) : u)))
			notice_lang(Config.s_NickServ, u, NICK_X_NOT_IN_USE, nick.c_str());
		else if (!nick.empty() && u2->Account() && !u2->Account()->IsServicesOper())
			notice_lang(Config.s_NickServ, u, NICK_LOGOUT_SERVICESADMIN, nick.c_str());
		else
		{
			if (!nick.empty() && !param.empty() && param.equals_ci("REVALIDATE"))
				validate_user(u2);

			u2->isSuperAdmin = 0; /* Dont let people logout and remain a SuperAdmin */
			Alog() << Config.s_NickServ << ": " << u->GetMask() << " logged out nickname " << u2->nick;

			/* Remove founder status from this user in all channels */
			if (!nick.empty())
				notice_lang(Config.s_NickServ, u, NICK_LOGOUT_X_SUCCEEDED, nick.c_str());
			else
				notice_lang(Config.s_NickServ, u, NICK_LOGOUT_SUCCEEDED);

			ircdproto->SendAccountLogout(u2, u2->Account());
			u2->RemoveMode(NickServ, UMODE_REGISTERED);
			ircdproto->SendUnregisteredNick(u2);

			u2->Logout();

			/* Send out an event */
			FOREACH_MOD(I_OnNickLogout, OnNickLogout(u2));
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		if (u->Account() && u->Account()->IsServicesOper())
			notice_help(Config.s_NickServ, u, NICK_SERVADMIN_HELP_LOGOUT);
		else
			notice_help(Config.s_NickServ, u, NICK_HELP_LOGOUT);

		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_NickServ, u, "LOGOUT", NICK_LOGOUT_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_LOGOUT);
	}
};

class NSLogout : public Module
{
 public:
	NSLogout(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, new CommandNSLogout());
	}
};

MODULE_INIT(NSLogout)