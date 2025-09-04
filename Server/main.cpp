//add where you want
#ifdef ENABLE_IKASHOP_RENEWAL
#include "ikarus_shop.h"
#include "ikarus_shop_manager.h"
#endif

//search
	quest::CQuestManager quest_manager;

//add before
#ifdef ENABLE_IKASHOP_RENEWAL
	ikashop::CShopManager offshopManager;
#endif