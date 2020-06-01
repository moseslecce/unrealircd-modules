/*
 * ==================================================================
 * Module:			joindeop
 * Author:			Moses Lecce <info@aberrantsoftware.com>
 * Version:			5.0.1
 * Licence:			None
 * Description:	Automatically deops a user when joining a new channel (if user is not an oper)
 * ==================================================================
 */

 /* ChangeLog:
 *	01/06/2020 - 5.0.1
 *		- Initial version for UnrealIRCD v5
 */

#include "unrealircd.h"

ModuleHeader MOD_HEADER = {
        "third/joindeop",
        "5.0.1",
        "Automatically deop a user when joining a new channel",
        "Moses", // Author
        "unrealircd-5", // Modversion
};

MOD_INIT(joindeop)
{
  HookAdd(modinfo->handle, HOOKTYPE_LOCAL_JOIN, 0, joindeop_hook_join);
	return MOD_SUCCESS;
}

MOD_LOAD(joindeop)
{
	return MOD_SUCCESS;
}

MOD_UNLOAD(joindeop)
{
	return MOD_SUCCESS;
}

int joindeop_hook_join(aClient *cptr, aClient *sptr, aChannel *chptr, char *parv[])
{
	Member *cm;
	
	if (!IsOper(sptr))
	{
		//sendto_realops("%s is joining %s and not an oper", sptr->name, chptr->chname);
		
		/* appropriated from m_svsmode.c, why do_cmd didn't like me I have nfi.. */
		for (cm = chptr->members; cm; cm = cm->next)
		{
			//sendto_realops("Comparing %s to %s", cm->cptr->name, sptr->name);
			if (cm->cptr->name == sptr->name)
			{
				//sendto_realops("Match found.");
				if (cm->flags & CHFL_CHANOP)
				{
					Membership *mb;
					
					//sendto_realops("Match is opped, deopping.");
					mb = find_membership_link(cm->cptr->user->channel, chptr);

					sendto_channel_butserv(chptr, sptr, ":%s MODE %s %s %s", me.name, chptr->chname, "-o", cm->cptr->name);
					sendto_server(NULL, 0, 0, ":%s MODE %s %s %s", me.name, chptr->chname, "-o", cm->cptr->name);

					cm->flags &= ~CHFL_CHANOP;
					if (mb)
						mb->flags = cm->flags;
				}
			}
		}
	}
	return HOOK_CONTINUE;
}
