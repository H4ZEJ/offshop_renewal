//search in function @@@ void CClientManager::QUERY_BOOT
	peer->EncodeWORD(0xffff);

//after add

#ifdef ENABLE_IKASHOP_RENEWAL
	SendIkarusShopTable(peer);
#endif

//search in function @@@ void CClientManager::QUERY_ITEM_SAVE
	if (p->window == SAFEBOX || p->window == MALL)
	{

//replace with
	if (p->window == SAFEBOX || p->window == MALL
#ifdef ENABLE_IKASHOP_RENEWAL
		|| p->window == IKASHOP_OFFLINESHOP || p->window == IKASHOP_SAFEBOX
#endif
	)
	{

//search
			case HEADER_GD_REQUEST_CHANNELSTATUS:
				RequestChannelStatus(peer, dwHandle);
				break;

//after add
#ifdef ENABLE_IKASHOP_RENEWAL
			case HEADER_GD_NEW_OFFLINESHOP:
				RecvIkarusShopPacket(peer, data);
				break;
#endif

//search in function @@@ int CClientManager::AnalyzeQueryResult
	if (!peer)
	{
		delete qi;
		return true;
	}

//replace with
	if (!peer)
	{
#ifdef ENABLE_IKASHOP_RENEWAL
		switch (qi->iType)
		{
		case QID_OFFLINESHOP_ADD_ITEM:
		case QID_OFFLINESHOP_EDIT_ITEM:
		case QID_OFFLINESHOP_DELETE_SHOP:
		case QID_OFFLINESHOP_DELETE_SHOP_ITEM:
		case QID_OFFLINESHOP_REMOVE_ITEM:
		case QID_OFFLINESHOP_UPDATE_SOLD_ITEM:
		case QID_OFFLINESHOP_CREATE_SHOP:
		case QID_OFFLINESHOP_SHOP_CHANGE_NAME:
		case QID_OFFLINESHOP_SHOP_UPDATE_DURATION:
		case QID_OFFLINESHOP_SAFEBOX_DELETE_ITEM:
		case QID_OFFLINESHOP_SAFEBOX_ADD_ITEM:
		case QID_OFFLINESHOP_SAFEBOX_UPDATE_VALUTES:
		case QID_OFFLINESHOP_SAFEBOX_INSERT_VALUTES:
		case QID_OFFLINESHOP_SAFEBOX_UPDATE_VALUTES_ADDING:
			IkarusShopResultQuery(peer, msg, qi);
			break;

		default: break;
		}
#endif
		//sys_err("CClientManager::AnalyzeQueryResult: peer not exist anymore. (ident: %d)", qi->dwIdent);
		delete qi;
		return true;
	}

//In same function search
		case QID_ITEMPRICE_LOAD:
			RESULT_PRICELIST_LOAD(peer, msg);
			break;

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
		case QID_OFFLINESHOP_ADD_ITEM:
		case QID_OFFLINESHOP_EDIT_ITEM:
		case QID_OFFLINESHOP_DELETE_SHOP:
		case QID_OFFLINESHOP_DELETE_SHOP_ITEM:
		case QID_OFFLINESHOP_CREATE_SHOP:
		case QID_OFFLINESHOP_SHOP_CHANGE_NAME:
		case QID_OFFLINESHOP_SAFEBOX_DELETE_ITEM:
		case QID_OFFLINESHOP_SAFEBOX_ADD_ITEM:
		case QID_OFFLINESHOP_REMOVE_ITEM:
		case QID_OFFLINESHOP_SAFEBOX_UPDATE_VALUTES:
		case QID_OFFLINESHOP_SAFEBOX_INSERT_VALUTES:
		case QID_OFFLINESHOP_SAFEBOX_UPDATE_VALUTES_ADDING:
			IkarusShopResultQuery(peer, msg, qi);
			break;
#endif

//search
		if (!(thecore_heart->pulse % (thecore_heart->passes_per_sec * 10)))
		{
			pt_log("QUERY: MAIN[%d] ASYNC[%d]", g_query_count[0], g_query_count[1]);
			g_query_count[0] = 0;
			g_query_count[1] = 0;
			/////////////////////////////////////////////////////////////////

			/////////////////////////////////////////////////////////////////
			pt_log("ITEM:%d\n", g_item_count);
			g_item_count = 0;
			/////////////////////////////////////////////////////////////////
		}

//add before
#ifdef ENABLE_IKASHOP_RENEWAL
		if ((thecore_heart->pulse % (thecore_heart->passes_per_sec * 60)) == thecore_heart->passes_per_sec*30) //to be async relative to the money log
			IkarusShopDurationProcess();
#endif