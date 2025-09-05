//search
	Set(HEADER_CG_STATE_CHECKER, sizeof(BYTE), "ServerStateCheck", false);

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
	Set(HEADER_CG_NEW_OFFLINESHOP, sizeof(TPacketCGNewIkarusShop), "NewIkarusShop", false);
#endif