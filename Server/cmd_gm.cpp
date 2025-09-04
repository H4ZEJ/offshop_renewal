//add where you want
#ifdef ENABLE_IKASHOP_RENEWAL
#include "ikarus_shop.h"
#include "ikarus_shop_manager.h"
#endif

//add at the end of file
#ifdef ENABLE_IKASHOP_RENEWAL
ACMD(do_offshop_force_close_shop) 
{
	char arg1[50];
	argument = one_argument(argument, arg1, sizeof(arg1));
	if (arg1[0] != 0 && isdigit(arg1[0])) 
	{

		DWORD id = 0;
		str_to_number(id, arg1);

		if (id == 0) 
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "syntax : offshop_force_close_shop  <player-id>  ");
			return;
		}
		else 
		{
			auto pkShop = ikashop::GetManager().GetShopByOwnerID(id);
			if (!pkShop) 
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Cannot find shop by id %u ", id);
				return;
			}
			else 
			{
				ikashop::GetManager().SendShopForceCloseDBPacket(id, true);
				ch->ChatPacket(CHAT_TYPE_INFO, "shop closed successfully.");
			}
		}
	}
	else 
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "syntax : offshop_force_close_shop  <player-id>  ");
		return;
	}
}
#endif
