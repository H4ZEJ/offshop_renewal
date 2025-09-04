#include "StdAfx.h"
#ifdef ENABLE_IKASHOP_RENEWAL
#include "PythonNetworkStream.h"
#include "Packet.h"
#include "PythonIkarusShop.h"

bool CPythonNetworkStream::RecvOfflineshopPacket()
{
	TPacketGCNewOfflineshop pack;
	if(!Recv(sizeof(pack), &pack))
		return false;

	switch (pack.subheader)
	{
		case ikashop::SUBHEADER_GC_SHOP_OPEN:
			return RecvOfflineshopShopOpen();

		case ikashop::SUBHEADER_GC_SHOP_OPEN_OWNER:
			return RecvOfflineshopShopOpenOwner();

		case ikashop::SUBHEADER_GC_SHOP_OPEN_OWNER_NO_SHOP:
			return RecvOfflineshopShopOpenOwnerNoShop();

		case ikashop::SUBHEADER_GC_SHOP_EXPIRED_GUESTING:
			return RecvOfflineshopShopExpiredGuesting();

		case ikashop::SUBHEADER_GC_SHOP_REMOVE_ITEM_OWNER:
			return RecvOfflineshopShopRemoveItem(true);

		case ikashop::SUBHEADER_GC_SHOP_REMOVE_ITEM_GUEST:
			return RecvOfflineshopShopRemoveItem(false);

		case ikashop::SUBHEADER_GC_SHOP_EDIT_ITEM_GUEST:
			return RecvOfflineshopShopGuestEditItem();

		case ikashop::SUBHEADER_GC_SHOP_EDIT_ITEM_OWNER:
			return RecvOfflineshopShopOwnerEditItem();

		case ikashop::SUBHEADER_GC_SHOP_FILTER_RESULT:
			return RecvOfflineshopShopFilterResult();

		case ikashop::SUBHEADER_GC_SHOP_SAFEBOX_REFRESH:
			return RecvOfflineshopShopSafeboxRefresh();

		case ikashop::SUBHEADER_GC_SHOP_SAFEBOX_REMOVE_ITEM:
			return RecvOfflineshopShopSafeboxRemoveItem();

		case ikashop::SUBHEADER_GC_SHOP_SAFEBOX_ADD_ITEM:
			return RecvOfflineshopShopSafeboxAddItem();

		case ikashop::SUBHEDAER_GC_POPUP_MESSAGE:
			return RecvOfflineshopPopupMessage();

#ifdef ENABLE_IKASHOP_ENTITIES
		case ikashop::SUBHEADER_GC_INSERT_SHOP_ENTITY:
			return RecvOfflineshopInsertEntity();

		case ikashop::SUBHEADER_GC_REMOVE_SHOP_ENTITY:
			return RecvOfflineshopRemoveEntity();
#endif

		case ikashop::SUBHEADER_GC_BOARD_COUNTERS:
			return RecvOfflineshopBoardCounters();

#ifdef EXTEND_IKASHOP_PRO
		case ikashop::SUBHEADER_GC_NOTIFICATION_LIST:
			return RecvOfflineshopNotificationList();
#endif
#ifdef EXTEND_IKASHOP_ULTIMATE
		case ikashop::SUBHEADER_GC_PRICE_AVERAGE_RESPONSE:
			return RecvOfflineshopPriceAverageResponse();
#endif

		case ikashop::SUBHEADER_GC_SEARCH_RESULT_DELETE:
			return RecvOfflineshopSearchResultDelete();

		default:
			TraceError("UNKNOWN OFFLINESHOP SUBHEADER : %d ",pack.subheader);
			return false;
	}
}

bool CPythonNetworkStream::RecvOfflineshopShopOpen()
{
	ikashop::TSubPacketGCShopOpen subpack;
	if(!Recv(sizeof(subpack) , &subpack))
		return false;

#ifdef EXTEND_IKASHOP_ULTIMATE
	std::vector<ikashop::TShopItemView> items;
#else
	std::vector<ikashop::TShopItem> items;
#endif

	items.resize(subpack.shop.count);
	if(subpack.shop.count > 0)
		if(!Recv(sizeof(items[0])*items.size(), items.data()))
			return false;

	CPythonIkarusShop::Instance().OpenShop(subpack.shop, items);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopShopOpenOwner()
{
	ikashop::TSubPacketGCShopOpenOwner subpack;
	if(!Recv(sizeof(subpack) , &subpack))
		return false;

#ifdef EXTEND_IKASHOP_ULTIMATE
	std::vector<ikashop::TShopItemView> items;
#else
	std::vector<ikashop::TShopItem> items;
#endif
	items.resize(subpack.shop.count);
	
	if(subpack.shop.count!=0)
		if(!Recv(sizeof(items[0]) * subpack.shop.count, items.data()))
			return false;

	CPythonIkarusShop::Instance().OpenShopOwner(subpack.shop, items);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopShopOpenOwnerNoShop()
{
	CPythonIkarusShop::instance().OpenShopOwnerNoShop();
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopShopExpiredGuesting()
{
	ikashop::TSubPacketGCShopExpiredGuesting subpack;
	if(!Recv(sizeof(subpack), &subpack))
		return false;

	CPythonIkarusShop::instance().ShopExpiredGuesting(subpack.ownerid);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopShopRemoveItem(bool owner)
{
	ikashop::TSubPacketGCShopRemoveItem subpack;
	if(!Recv(sizeof(subpack), &subpack))
		return false;

	CPythonIkarusShop::instance().ShopRemoveItem(owner, subpack.itemid);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopShopGuestEditItem()
{
	ikashop::TSubPacketGCShopEditItem subpack;
	if (!Recv(sizeof(subpack), &subpack))
		return false;

	CPythonIkarusShop::instance().ShopGuestEditItem(subpack.itemid, subpack.price);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopShopOwnerEditItem()
{
	ikashop::TSubPacketGCShopEditItem subpack;
	if (!Recv(sizeof(subpack), &subpack))
		return false;

	CPythonIkarusShop::instance().ShopOwnerEditItem(subpack.itemid, subpack.price);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopShopFilterResult()
{
	ikashop::TSubPacketGCShopFilterResult subpack;
	if(!Recv(sizeof(subpack) , &subpack))
		return false;

	std::vector<ikashop::TSearchResultInfo> items(static_cast<size_t>(subpack.count));
	if(subpack.count != 0)
		if(!Recv(items.size() * sizeof(items[0]), items.data()))
			return false;

	CPythonIkarusShop::Instance().ShopFilterResult(items);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopShopSafeboxRefresh()
{
	ikashop::TSubPacketGCShopSafeboxRefresh subpack;
	if(!Recv(sizeof(subpack), &subpack))
		return false;

	std::vector<ikashop::TShopPlayerItem> items;
	items.resize(subpack.itemcount);

	if(subpack.itemcount != 0)
		if (!Recv(sizeof(ikashop::TShopPlayerItem) * items.size(), &items[0]))
			return false;

	CPythonIkarusShop::instance().SafeboxRefresh(subpack.valute, items);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopShopSafeboxRemoveItem()
{
	ikashop::TSubPacketGCShopSafeboxRemoveItem subpack{};
	if(!Recv(sizeof(subpack), &subpack))
		return false;

	CPythonIkarusShop::instance().SafeboxRemoveItem(subpack.itemid);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopShopSafeboxAddItem()
{
	ikashop::TSubPacketGCShoppingSafeboxAddItem subpack{};
	if (!Recv(sizeof(subpack), &subpack))
		return false;

	CPythonIkarusShop::instance().SafeboxAddItem(subpack.item);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopPopupMessage()
{
	// receiving string
	ikashop::TSubPacketGCPopupMessage pack;
	if (!Recv(sizeof(pack), &pack))
		return false;

	// safely terminating string
	pack.localeString[pack.MESSAGE_LEN-1] = 0;

	CPythonIkarusShop::instance().PopupMessage(pack.localeString);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopSearchResultDelete()
{
	// getting itemid
	DWORD itemid = 0;
	if (!Recv(sizeof(itemid), &itemid))
		return false;

	CPythonIkarusShop::instance().DeleteSearchResult(itemid);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopBoardCounters()
{
	ikashop::TSubPacketGCBoardCounters subpack;
	if (!Recv(sizeof(subpack), &subpack))
		return false;

#ifdef EXTEND_IKASHOP_PRO
	CPythonIkarusShop::Instance().RefreshBoardCounters(subpack.safebox, subpack.notification);
#else
	CPythonIkarusShop::Instance().RefreshBoardCounters(subpack.safebox);
#endif
	return true;
}

#ifdef EXTEND_IKASHOP_PRO
bool CPythonNetworkStream::RecvOfflineshopNotificationList()
{
	ikashop::TSubPacketGCNotificationList subpack;
	if (!Recv(sizeof(subpack), &subpack))
		return false;

	std::vector<ikashop::TNotificationInfo> notifications;
	notifications.resize(subpack.count);

	if(subpack.count != 0)
		if(!Recv(notifications.size() * sizeof(ikashop::TNotificationInfo), notifications.data()))
			return false;

	CPythonIkarusShop::Instance().SetNotificationList(notifications);
	return true;
}
#endif

#ifdef EXTEND_IKASHOP_ULTIMATE
bool CPythonNetworkStream::RecvOfflineshopPriceAverageResponse()
{
	long long average = 0;
	if(!Recv(sizeof(average), &average))
		return false;

	CPythonIkarusShop::instance().PriceAverageResponse(average);
	return true;
}
#endif

#ifdef ENABLE_IKASHOP_ENTITIES
bool CPythonNetworkStream::RecvOfflineshopInsertEntity()
{
	ikashop::TSubPacketGCInsertShopEntity subpack;
	if(!Recv(sizeof(subpack) , &subpack))
		return false;

	CPythonIkarusShop::Instance().InsertEntity(subpack.vid, subpack.type, subpack.name, subpack.x, subpack.y, subpack.z);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopRemoveEntity()
{
	ikashop::TSubPacketGCRemoveShopEntity subpack;
	if(!Recv(sizeof(subpack) , &subpack))
		return false;

	CPythonIkarusShop::Instance().RemoveEntity(subpack.vid);
	return true;
}

void CPythonNetworkStream::SendIkarusShopOnClickShopEntity(DWORD dwPickedShopVID)
{
	TPacketCGNewOfflineShop pack;
	pack.header = HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader = ikashop::SUBHEADER_CG_CLICK_ENTITY;
	pack.size = sizeof(pack) + sizeof(ikashop::TSubPacketCGShopClickEntity);

	ikashop::TSubPacketCGShopClickEntity subpack;
	subpack.vid = dwPickedShopVID;

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack",pack.subheader);
		return;
	}

	if(!Send(subpack))
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ", pack.subheader);
}

#ifdef EXTEND_IKASHOP_PRO
void CPythonNetworkStream::SendIkarusShopMoveShopEntity()
{
	TPacketCGNewOfflineShop pack;
	pack.header = HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader = ikashop::SUBHEADER_CG_MOVE_SHOP_ENTITY;
	pack.size = sizeof(pack);

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.subheader);
		return;
	}
}
#endif
#endif

void CPythonNetworkStream::SendIkarusShopCreateShop()
{
	TPacketCGNewOfflineShop pack{};
	pack.header = HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader	= ikashop::SUBHEADER_CG_SHOP_CREATE_NEW;
	pack.size = sizeof(pack);

	if (!Send(pack))
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.subheader);
}

void CPythonNetworkStream::SendIkarusShopForceCloseShop()
{
	TPacketCGNewOfflineShop pack;
	pack.header	= HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader	= ikashop::SUBHEADER_CG_SHOP_FORCE_CLOSE;
	pack.size		= sizeof(pack);

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack",pack.subheader);
		return;
	}
}

void CPythonNetworkStream::SendIkarusShopRequestShopList()
{
	TPacketCGNewOfflineShop pack;
	pack.header	= HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader	= ikashop::SUBHEADER_CG_SHOP_REQUEST_SHOPLIST;
	pack.size		= sizeof(pack);

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack",pack.subheader);
		return;
	}
}

void CPythonNetworkStream::SendIkarusShopOpenShop(DWORD dwOwnerID)
{
	TPacketCGNewOfflineShop pack;
	pack.header	= HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader	= ikashop::SUBHEADER_CG_SHOP_OPEN;
	pack.size		= sizeof(pack) + sizeof(ikashop::TSubPacketCGShopOpen);

	ikashop::TSubPacketCGShopOpen subpack;
	subpack.ownerid = dwOwnerID;

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack",pack.subheader);
		return;
	}

	if(!Send(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ",pack.subheader);
		return;
	}
}

void CPythonNetworkStream::SendIkarusShopOpenShopOwner()
{
	TPacketCGNewOfflineShop pack;
	pack.header	= HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader	= ikashop::SUBHEADER_CG_SHOP_OPEN_OWNER;
	pack.size		= sizeof(pack);

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack",pack.subheader);
		return;
	}
}


void CPythonNetworkStream::SendIkarusShopBuyItem(DWORD dwOwnerID, DWORD dwItemID, bool isSearch, long long TotalPriceSeen) //patch seen price check
{
	TPacketCGNewOfflineShop pack;
	pack.header = HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader = ikashop::SUBHEADER_CG_SHOP_BUY_ITEM;
	pack.size = sizeof(pack) + sizeof(ikashop::TSubPacketCGShopBuyItem);

	ikashop::TSubPacketCGShopBuyItem subpack;
	subpack.itemid = dwItemID;
	subpack.ownerid = dwOwnerID;
	subpack.searching = isSearch;

	//patch seen price check
	subpack.seenprice = TotalPriceSeen;

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.subheader);
		return;
	}

	if (!Send(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ", pack.subheader);
		return;
	}
}

void CPythonNetworkStream::SendIkarusShopAddItem(ikashop::TShopItemInfo& itemInfo)
{
	TPacketCGNewOfflineShop pack;
	pack.header	= HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader = ikashop::SUBHEADER_CG_SHOP_ADD_ITEM;
	pack.size = sizeof(pack) + sizeof(ikashop::TSubPacketCGAddItem);

	ikashop::TSubPacketCGAddItem subpack;
	subpack.pos = itemInfo.pos;
	subpack.price = itemInfo.price;
	subpack.destpos = itemInfo.destpos;

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack",pack.subheader);
		return;
	}

	if(!Send(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ",pack.subheader);
		return;
	}
}

void CPythonNetworkStream::SendIkarusShopRemoveItem(DWORD dwItemID)
{
	TPacketCGNewOfflineShop pack;
	pack.header	= HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader	= ikashop::SUBHEADER_CG_SHOP_REMOVE_ITEM;
	pack.size		= sizeof(pack) + sizeof(ikashop::TSubPacketCGRemoveItem);

	ikashop::TSubPacketCGRemoveItem subpack;
	subpack.itemid	= dwItemID;

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack",pack.subheader);
		return;
	}

	if(!Send(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ",pack.subheader);
		return;
	}
}

void CPythonNetworkStream::SendOfflineShopEditItem(DWORD dwItemID, const ikashop::TPriceInfo& price)
{
	TPacketCGNewOfflineShop pack;
	pack.header = HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader	= ikashop::SUBHEADER_CG_SHOP_EDIT_ITEM;
	pack.size = sizeof(pack) + sizeof(ikashop::TSubPacketCGEditItem);

	ikashop::TSubPacketCGEditItem subpack;
	subpack.itemid = dwItemID;
	subpack.price = price;

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack",pack.subheader);
		return;
	}

	if(!Send(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ",pack.subheader);
		return;
	}
}

void CPythonNetworkStream::SendIkarusShopFilterRequest(const ikashop::TFilterInfo& filter)
{
	TPacketCGNewOfflineShop pack;
	pack.header	= HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader	= ikashop::SUBHEADER_CG_SHOP_FILTER_REQUEST;
	pack.size		= sizeof(pack) + sizeof(ikashop::TSubPacketCGFilterRequest);

	ikashop::TSubPacketCGFilterRequest subpack;
	subpack.filter = filter;

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack",pack.subheader);
		return;
	}

	if(!Send(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ",pack.subheader);
		return;
	}
}

void CPythonNetworkStream::SendIkarusShopSearchFillRequest()
{
	TPacketCGNewOfflineShop pack;
	pack.header = HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader = ikashop::SUBHEADER_CG_SHOP_SEARCH_FILL_REQUEST;
	pack.size = sizeof(pack);

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.subheader);
		return;
	}
}

void CPythonNetworkStream::SendIkarusShopSafeboxOpen()
{
	TPacketCGNewOfflineShop pack;
	pack.header	= HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader	= ikashop::SUBHEADER_CG_SHOP_SAFEBOX_OPEN;
	pack.size		= sizeof(pack);

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack",pack.subheader);
		return;
	}
}

void CPythonNetworkStream::SendIkarusShopSafeboxGetItem(DWORD dwItemID)
{
	TPacketCGNewOfflineShop pack;
	pack.header	= HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader	= ikashop::SUBHEADER_CG_SHOP_SAFEBOX_GET_ITEM;
	pack.size = sizeof(pack) + sizeof(ikashop::TSubPacketCGShopSafeboxGetItem);

	ikashop::TSubPacketCGShopSafeboxGetItem subpack;
	subpack.itemid = dwItemID;

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack",pack.subheader);
		return;
	}

	if(!Send(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ",pack.subheader);
		return;
	}
}

void CPythonNetworkStream::SendIkarusShopSafeboxGetValutes()
{
	TPacketCGNewOfflineShop pack;
	pack.header	= HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader = ikashop::SUBHEADER_CG_SHOP_SAFEBOX_GET_VALUTES;
	pack.size = sizeof(pack);

	if (!Send(pack))
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack",pack.subheader);
}

void CPythonNetworkStream::SendIkarusShopSafeboxClose()
{
	TPacketCGNewOfflineShop pack;
	pack.header	= HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader	= ikashop::SUBHEADER_CG_SHOP_SAFEBOX_CLOSE;
	pack.size		= sizeof(pack);

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack",pack.subheader);
		return;
	}
}

void CPythonNetworkStream::SendIkarusMyShopCloseBoard()
{
	if(m_strPhase != "Game")
		return;

	TPacketCGNewOfflineShop pack;
	pack.header	= HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader = ikashop::SUBHEADER_CG_CLOSE_MY_SHOP_BOARD;
	pack.size = sizeof(pack);

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack",pack.subheader);
		return;
	}
}

void CPythonNetworkStream::SendIkarusCloseShopGuestBoard()
{
	if (m_strPhase != "Game")
		return;

	TPacketCGNewOfflineShop pack;
	pack.header = HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader = ikashop::SUBHEADER_CG_CLOSE_SHOP_GUEST;
	pack.size = sizeof(pack);

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.subheader);
		return;
	}
}

#ifdef EXTEND_IKASHOP_PRO
void CPythonNetworkStream::SendIkarusNotificationListRequest()
{
	TPacketCGNewOfflineShop pack;
	pack.header = HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader = ikashop::SUBHEADER_CG_NOTIFICATION_LIST_REQUEST;
	pack.size = sizeof(pack);

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.subheader);
		return;
	}
}

void CPythonNetworkStream::SendIkarusNotificationListClose()
{
	TPacketCGNewOfflineShop pack;
	pack.header = HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader = ikashop::SUBHEADER_CG_NOTIFICATION_LIST_CLOSE;
	pack.size = sizeof(pack);

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.subheader);
		return;
	}
}
#endif

#ifdef EXTEND_IKASHOP_ULTIMATE
void CPythonNetworkStream::SendPriceAverageRequest(DWORD vnum, DWORD count)
{
	TPacketCGNewOfflineShop pack;
	pack.header = HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader = ikashop::SUBHEADER_CG_PRICE_AVERAGE_REQUEST;
	pack.size = sizeof(pack) + sizeof(ikashop::TSubPacketCGPriceAverageRequest);

	ikashop::TSubPacketCGPriceAverageRequest subpack{};
	subpack.vnum = vnum;
	subpack.count = count;

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.subheader);
		return;
	}

	if (!Send(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.subheader);
		return;
	}
}

void CPythonNetworkStream::SendShopDecorationUsage(int index)
{
	TPacketCGNewOfflineShop pack;
	pack.header = HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader = ikashop::SUBHEADER_CG_SHOP_DECORATION_USE;
	pack.size = sizeof(pack) + sizeof(index);

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.subheader);
		return;
	}

	if (!Send(index))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.subheader);
		return;
	}
}

void CPythonNetworkStream::SendShopMoveItem(int srcPos, int destPos)
{
	TPacketCGNewOfflineShop pack;
	pack.header = HEADER_CG_NEW_OFFLINESHOP;
	pack.subheader = ikashop::SUBHEADER_CG_SHOP_MOVE_ITEM;
	pack.size = sizeof(pack) + sizeof(ikashop::TSubPacketCGShopMoveItem);

	ikashop::TSubPacketCGShopMoveItem subpack{};
	subpack.srcPos = srcPos;
	subpack.destPos = destPos;

	if (!Send(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.subheader);
		return;
	}

	if (!Send(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.subheader);
		return;
	}
}
#endif

#endif
