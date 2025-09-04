#include "stdafx.h"
#ifdef ENABLE_IKASHOP_RENEWAL
#include "Main.h"
#include "Config.h"
#include "DBManager.h"
#include "QID.h"
#include "Peer.h"
#include "IkarusShopCache.h"
#include "ClientManager.h"


std::string CreateIkarusShopSelectShopItemsQuery();

template <class T>
const T& Decode(const char*& data){
	auto obj = reinterpret_cast<const T*>(data);
	data += sizeof(T);
	return *obj;
}


bool CClientManager::InitializeIkarusShopTable()
{
	if(!InitializeIkarusShopShopTable())
		return false;

	if(!InitializeIkarusShopShopItemsTable())
		return false;

	return true;
}

bool CClientManager::InitializeIkarusShopShopTable()
{
	// executing select query on shops table
	auto msg = CDBManager::instance().DirectQuery("SELECT s.`owner` , s.`duration`, p.`name`"
#ifdef ENABLE_IKASHOP_ENTITIES
		", s.`map`, s.`x`, s.`y`"
#endif
#ifdef EXTEND_IKASHOP_ULTIMATE
		", s.`decoration`, s.`decoration_time`, s.`lock_index`"
#endif
		" FROM `ikashop_offlineshop` s INNER JOIN `player` p ON s.`owner` = p.`id`;");

	// handling sql errors
	if (msg->uiSQLErrno != 0 || !msg->Get()) {
		sys_err("CANNOT LOAD ikashop_offlineshop TABLE , errorcode %d sql->Get() = %p", msg->uiSQLErrno, msg->Get());
		return false;
	}

	// fetching results
	DWORD owner = 0;
	DWORD duration = 0;

	for (uint32_t i = 0; i < msg->Get()->uiNumRows; i++)
	{
		auto row = mysql_fetch_row(msg->Get()->pSQLResult);
		int col = 0;

		str_to_number(owner, row[col++]);
		str_to_number(duration, row[col++]);
		auto name = row[col++];

#ifdef ENABLE_IKASHOP_ENTITIES
		ikashop::TShopSpawn spawn{};
		str_to_number(spawn.map, row[col++]);
		str_to_number(spawn.x, row[col++]);
		str_to_number(spawn.y, row[col++]);		
#endif
#ifdef EXTEND_IKASHOP_ULTIMATE
		int decoration{};
		int decoration_time{};
		str_to_number(decoration, row[col++]);
		str_to_number(decoration_time, row[col++]);

		int lock_index{};
		str_to_number(lock_index, row[col++]);
		if(lock_index < OFFSHOP_LOCK_INDEX_INIT)
			lock_index = OFFSHOP_LOCK_INDEX_INIT;
#endif

		if (!m_offlineshopShopCache.PutShop(owner, duration, name
#ifdef ENABLE_IKASHOP_ENTITIES
			, spawn
#endif
#ifdef EXTEND_IKASHOP_ULTIMATE
			, decoration, decoration_time, lock_index
#endif
		))
			sys_err("cannot execute putsShop -> double shop id?! %u ", owner);
	}

	return true;
}

bool CClientManager::InitializeIkarusShopShopItemsTable()
{
	// executing query selecting items on window IKASHOP_OFFLINESHOP
	std::string query = CreateIkarusShopSelectShopItemsQuery();
	auto msg = CDBManager::instance().DirectQuery(query.c_str());

	// handling sql errors
	if (msg->uiSQLErrno != 0 || !msg->Get()){
		sys_err("CANNOT LOAD SHOP ITEM TABLE , errorcode %d  sql %p", msg->uiSQLErrno, msg->Get());
		return false;
	}

	// fetching data
	for(uint32_t i=0; i < msg->Get()->uiNumRows; i++)
	{
		auto row = mysql_fetch_row(msg->Get()->pSQLResult);
		int col = 0;

		// fetching item info
		ikashop::CShopCache::TShopCacheItemInfo item{};
		str_to_number(item.id, row[col++]);
		str_to_number(item.owner, row[col++]);
		str_to_number(item.pos, row[col++]);
		str_to_number(item.vnum, row[col++]);
		str_to_number(item.count, row[col++]);
		// fetching sockets
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
			str_to_number(item.alSockets[i], row[col++]);
		// fetching attributes
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++) {
			str_to_number(item.aAttr[i].bType, row[col++]);
			str_to_number(item.aAttr[i].sValue, row[col++]);
		}

#ifdef ENABLE_CHANGELOOK_SYSTEM
		str_to_number(item.dwTransmutation, row[col++]);
#endif
		// fetching json
		std::string jsonStr = row[col++];
		auto jsonOptional = str_to_json(jsonStr.c_str());
		if (!jsonOptional) {
			sys_err("INVALID JSON ON SHOP ITEM %d (owner %d)  json value (%s)", item.id, item.owner, jsonStr.c_str());
			continue;
		}

		auto& json = jsonOptional.value();

		// fetching price yang
		auto priceYang = jsonHelper::getValue<int64_t>(json, "yang");
		if (!priceYang) {
			sys_err("INVALID JSON ON SHOP ITEM %d (owner %d) CANNOT EXTRACT PRICE YANG   json value (%s)", item.id, item.owner, jsonStr.c_str());
			continue;
		}
		item.price.yang = *priceYang;

		// fetching price cheque
#ifdef ENABLE_CHEQUE_SYSTEM
		auto priceCheque = jsonHelper::getValue<int>(json, "cheque");
		if (!priceCheque) {
			sys_err("INVALID JSON ON SHOP ITEM %d (owner %d) CANNOT EXTRACT PRICE CHEQUE   json value (%s)", item.id, item.owner, jsonStr.c_str());
			continue;
		}
		item.price.cheque = *priceCheque;
#endif

		// fetching expiration
		auto expiration = jsonHelper::getValue<int64_t>(json, "expiration");
		item.expiration = static_cast<ikashop::ExpirationType>(expiration ? *expiration : 0);
		
		if (!m_offlineshopShopCache.PutItem(item.owner, item.id, item/*, isSold*/))
			sys_err("cannot execute putitem !? owner %d , item %d", item.owner, item.id);
	}

	return true;
}

void CClientManager::SendIkarusShopTable(CPeer* peer)
{
	peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0,
		sizeof(TPacketDGNewIkarusShop) + sizeof(ikashop::TSubPacketDGLoadTables) +
		sizeof(ikashop::TShopInfo)	* m_offlineshopShopCache.GetCount() + sizeof(ikashop::TShopItem) * m_offlineshopShopCache.GetItemCount()
#ifdef EXTEND_IKASHOP_ULTIMATE
		 + sizeof(DWORD) + sizeof(ikashop::TSaleHistory) * m_offlineshopAveragePriceCache.GetCount()
#endif
	);


	TPacketDGNewIkarusShop pack{};
	pack.bSubHeader	= ikashop::SUBHEADER_DG_LOAD_TABLES;

	ikashop::TSubPacketDGLoadTables subPack{};
	subPack.shopcount = m_offlineshopShopCache.GetCount();

	peer->Encode(&pack, sizeof(pack));
	peer->Encode(&subPack, sizeof(subPack));

	

	m_offlineshopShopCache.EncodeCache(peer);

#ifdef EXTEND_IKASHOP_ULTIMATE
	m_offlineshopAveragePriceCache.Encode(peer);
#endif
}

void CClientManager::RecvIkarusShopPacket(CPeer* peer,const char* data)
{
	const auto& pack = Decode<TPacketDGNewIkarusShop>(data);

	bool success=false;

	switch (pack.bSubHeader)
	{
	case ikashop::SUBHEADER_GD_BUY_ITEM:
		success=RecvIkarusShopBuyItemPacket(data);
		break;
	case ikashop::SUBHEADER_GD_BUY_LOCK_ITEM:
		success=RecvIkarusShopLockBuyItem(peer,data);
		break;
	case ikashop::SUBHEADER_GD_EDIT_ITEM:
		success=RecvIkarusShopEditItemPacket(data);
		break;
	case ikashop::SUBHEADER_GD_REMOVE_ITEM:
		success=RecvIkarusShopRemoveItemPacket(peer, data);
		break;
	case ikashop::SUBHEADER_GD_ADD_ITEM:
		success=RecvIkarusShopAddItemPacket(data);
		break;

	case ikashop::SUBHEADER_GD_SHOP_FORCE_CLOSE:
		success=RecvIkarusShopForceClose(peer, data);
		break;
	case ikashop::SUBHEADER_GD_SHOP_CREATE_NEW:
		success=RecvIkarusShopCreateNew(data);
		break;
	case ikashop::SUBHEADER_GD_SHOP_CHANGE_NAME:
		success=RecvIkarusShopChangeName(data);
		break;

	case ikashop::SUBHEADER_GD_SAFEBOX_GET_ITEM:
		success=RecvIkarusShopSafeboxGetItem(peer, data);
		break;
	case ikashop::SUBHEADER_GD_SAFEBOX_GET_VALUTES:
		success=RecvIkarusShopSafeboxGetValutes(data);
		break;
	case ikashop::SUBHEADER_GD_SAFEBOX_ADD_ITEM:
		success=RecvIkarusShopSafeboxAddItem(data);
		break;
	case ikashop::SUBHEADER_GD_SAFEBOX_GET_TIME_FEEDBACK:
		success=RecvIkarusShopSafeboxGetItemFeedback(data);
		break;

#ifdef EXTEND_IKASHOP_PRO
	case ikashop::SUBHEADER_GD_NOTIFICATION_SEEN:
		success=RecvIkarusShopNotificationSeen(data);
		break;
	case ikashop::SUBHEADER_GD_SHOP_RESTORE_DURATION:
		success=RecvIkarusShopRestoreDuration(data);
		break;
#ifdef ENABLE_IKASHOP_ENTITIES
	case ikashop::SUBHEADER_GD_MOVE_SHOP_ENTITY:
		success=RecvIkarusShopMoveShopEntity(data);
		break;
#endif
#endif


#ifdef EXTEND_IKASHOP_ULTIMATE
	case ikashop::SUBHEADER_GD_SHOP_DECORATION_USE:
		success=RecvIkarusShopDecorationUse(data);
		break;

	case ikashop::SUBHEADER_GD_SHOP_MOVE_ITEM:
		success=RecvIkarusShopMoveItem(data);
		break;

	case ikashop::SUBHEADER_GD_SHOP_UNLOCK_CELL:
		success=RecvIkarusShopUnlockCell(data);
		break;
#endif

	default:
		sys_err("UNKNOW NEW OFFLINESHOP SUBHEADER GD %d",pack.bSubHeader);
		break;
	}


	if(!success)
		sys_err("maybe some error during recv offline shop subheader %d ",pack.bSubHeader);
}

bool CClientManager::RecvIkarusShopLockBuyItem(CPeer* peer, const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketGDLockBuyItem>(data);
	
	// searching shop
	auto shop = m_offlineshopShopCache.Get(subpack.ownerid);
	if(!shop)
	{
		sys_err("cannot find buy target item %u (owner %u , buyer %u) ", subpack.itemid, subpack.ownerid, subpack.guestid);
		return false;
	}

	// searching item
	auto itemIter = shop->itemsmap.find(subpack.itemid);
	if(itemIter == shop->itemsmap.end())
	{
		sys_err("cannot find buy target item %u (owner %u , buyer %u) ", subpack.itemid, subpack.ownerid, subpack.guestid);
		return false;
	}

	auto item = itemIter->second;

	// finally selling it
	if (m_offlineshopShopCache.LockSellItem(subpack.ownerid, subpack.itemid, subpack.seenprice)){
#ifdef ENABLE_IKASHOP_LOGS
		IkarusShopLog(subpack.guestid, subpack.itemid, "LOCK_ITEM_TO_BUY", subpack.ownerid, "", item->vnum, item->count, item->price.yang
#ifdef ENABLE_CHEQUE_SYSTEM
			, item->price.cheque
#endif
		);
#endif

		SendIkarusShopBuyLockedItemPacket(peer, subpack.ownerid, subpack.guestid, subpack.itemid);
#ifdef EXTEND_IKASHOP_PRO
		std::string itemPrice = fmt::format("{}", item->price.yang);
#ifdef ENABLE_CHEQUE_SYSTEM
		itemPrice += fmt::format("|{}", item->price.cheque);
#endif
		AppendIkarusShopNotification(
			ikashop::ENotificationType::SELLER_SOLD_ITEM, subpack.ownerid, item->vnum, "", itemPrice);
#endif
#ifdef EXTEND_IKASHOP_ULTIMATE
		ikashop::TSaleHistory data{};
		data.account = subpack.accountid;
		data.count = item->count;
		data.vnum = item->vnum;
		data.datetime = std::time(nullptr);
		data.price = subpack.normalizedPrice;

		if (m_offlineshopAveragePriceCache.RegisterSelling(data))
		{
			TPacketDGNewIkarusShop pack;
			pack.bSubHeader = ikashop::SUBHEADER_DG_SALE_HISTORY_REGISTER;

			for (auto& peer : m_peerList)
			{
				if (peer->GetChannel() != 0)
				{
					peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(data));
					peer->Encode(pack);
					peer->Encode(data);
				}
			}
		}
#endif
	}

	else
		sys_err("cannot find buy target item %u (owner %u , buyer %u) ", subpack.itemid, subpack.ownerid, subpack.guestid);
	return true;
}

bool CClientManager::RecvIkarusShopBuyItemPacket(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketGDBuyItem>(data);

	DWORD dwOwner = subpack.ownerid;
	DWORD dwItem  = subpack.itemid;
	bool success = subpack.success;	

	auto shopCache = m_offlineshopShopCache.Get(subpack.ownerid);
	if(!shopCache)
		return false;

	auto it = shopCache->itemsmap.find(dwItem);
	if(shopCache->itemsmap.end() == it)
		return false;

	// unlocking locked item
	if(!success){
		m_offlineshopShopCache.UnlockSellItem(dwOwner, dwItem);
		return true;
	}

	const auto& price = it->second->price;

	ikashop::TValutesInfo valute;
	valute.yang = price.GetTotalYangAmount();

	m_offlineshopSafeboxCache.AddValutes(subpack.ownerid, valute);
	SendIkarusShopSafeboxAddValutes(subpack.ownerid, valute);

	//ikashop-updated 04/08/19
	SendIkarusShopBuyItemPacket(dwOwner, subpack.guestid, dwItem);

#ifdef ENABLE_IKASHOP_LOGS
	auto& item = it->second;
	IkarusShopLog(subpack.guestid, subpack.itemid, "BUY_ITEM", subpack.ownerid, "", item->vnum, item->count, item->price.yang
#ifdef ENABLE_CHEQUE_SYSTEM
		, item->price.cheque
#endif
	);
#endif

	if(!m_offlineshopShopCache.SellItem(dwOwner, dwItem))
		sys_err("some problem with sell : %u %u ",dwOwner, dwItem);

	return true;
}

bool CClientManager::RecvIkarusShopEditItemPacket(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketGDEditItem>(data);
	
	if(m_offlineshopShopCache.EditItem(subpack.ownerid, subpack.itemid, subpack.price)){
		SendIkarusShopEditItemPacket(subpack.ownerid, subpack.itemid, subpack.price);
#ifdef ENABLE_IKASHOP_LOGS
		auto item = m_offlineshopShopCache.GetItem(subpack.ownerid, subpack.itemid);
		IkarusShopLog(subpack.ownerid, subpack.itemid, "EDIT_PRICE", subpack.ownerid, "", item->vnum, item->count, item->price.yang
#ifdef ENABLE_CHEQUE_SYSTEM
			, item->price.cheque
#endif
		);
#endif
	}

	else
		sys_err("cannot edit item %u , shop %u ",subpack.itemid , subpack.ownerid);
	return true;
}

bool CClientManager::RecvIkarusShopRemoveItemPacket(CPeer* peer, const char* data) //patchme
{
	const auto& subpack = Decode<ikashop::TSubPacketGDRemoveItem>(data);
	
	// getting shop
	auto shopCache = m_offlineshopShopCache.Get(subpack.ownerid);
	if (!shopCache)
	{
		
		return false;
	}

	// getting item cache
	auto it = shopCache->itemsmap.find(subpack.itemid);
	if (it == shopCache->itemsmap.end())
	{
		
		return false;
	}

#ifdef ENABLE_IKASHOP_LOGS
	auto item = it->second;
	IkarusShopLog(subpack.ownerid, subpack.itemid, "REMOVE_ITEM", subpack.ownerid, "", item->vnum, item->count, item->price.yang
#ifdef ENABLE_CHEQUE_SYSTEM
		, item->price.cheque
#endif
	);
#endif

	//topatch
	auto copy = it->second;
	if (m_offlineshopShopCache.RemoveItem(subpack.ownerid, subpack.itemid))
		SendIkarusShopRemoveItemPacket(subpack.ownerid, subpack.itemid, peer);

	else
		sys_err("cannot remove item %u shop %u item ?!", subpack.ownerid, subpack.itemid);

	return true;
}

bool CClientManager::RecvIkarusShopAddItemPacket(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketGDAddItem>(data);
	ikashop::CShopCache::TShopCacheItemInfo item{subpack.sitem};
	item.lock = false;

#ifdef ENABLE_IKASHOP_LOGS
	IkarusShopLog(subpack.ownerid, item.id, "ADD_ITEM", subpack.ownerid, "", item.vnum, item.count, item.price.yang
#ifdef ENABLE_CHEQUE_SYSTEM
		, item.price.cheque
#endif
	);
#endif

	m_offlineshopShopCache.AddItem(item);
	return true;
}

bool CClientManager::RecvIkarusShopForceClose(CPeer* peer, const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketGDShopForceClose>(data);

	auto shopCache = m_offlineshopShopCache.Get(subpack.ownerid);
	if(!shopCache)
		return false;

	for(auto& [id, item] : shopCache->itemsmap)
		if(item->lock)
			return true;

#ifdef ENABLE_IKASHOP_LOGS
	IkarusShopLog(subpack.ownerid, 0, subpack.gm ? "GM_CLOSE_SHOP" : "CLOSE_SHOP", subpack.ownerid);
#endif

#ifdef EXTEND_IKASHOP_ULTIMATE
	auto items { shopCache->itemsmap };
	for(auto& [id, item] : items)
	{
#ifdef ENABLE_IKASHOP_LOGS
		IkarusShopLog(subpack.ownerid, item->id, "REMOVE_ITEM_CLOSE_SHOP", subpack.ownerid, "", item->vnum, item->count, item->price.yang
#ifdef ENABLE_CHEQUE_SYSTEM
			, item->price.cheque
#endif
		);
#endif
		if (m_offlineshopShopCache.RemoveItem(subpack.ownerid, item->id))
			SendIkarusShopRemoveItemPacket(subpack.ownerid, item->id, peer);
	}

	IkarusShopExpiredShop(subpack.ownerid, true);
#else
	// caching item list
	auto items{ shopCache->itemsmap };

	SendIkarusShopForceClose(subpack.ownerid);

	// putting item into safebox
	for (auto [id, item] : items)
		m_offlineshopSafeboxCache.AddItem(subpack.ownerid, id, *item);
#endif
	return true;
}

bool CClientManager::RecvIkarusShopCreateNew(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketGDShopCreateNew>(data);
	
#ifdef ENABLE_IKASHOP_ENTITIES
	m_offlineshopShopCache.CreateShop(subpack.shop.ownerid , subpack.shop.duration , subpack.shop.name, subpack.shop.spawn);
#else
	m_offlineshopShopCache.CreateShop(subpack.shop.ownerid , subpack.shop.duration , subpack.shop.name);
#endif

#ifdef ENABLE_IKASHOP_LOGS
	IkarusShopLog(subpack.shop.ownerid, 0, "CREATE_SHOP", subpack.shop.ownerid);
#endif

	return true;
}

bool CClientManager::RecvIkarusShopChangeName(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketGDShopChangeName>(data);
	if(m_offlineshopShopCache.ChangeShopName(subpack.ownerid, subpack.name))
		SendIkarusShopChangeName(subpack.ownerid, subpack.name);
	return true;
}

bool CClientManager::RecvIkarusShopSafeboxGetItem(CPeer* peer, const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketGDSafeboxGetItem>(data);
	if(m_offlineshopSafeboxCache.LockItem(subpack.ownerid , subpack.itemid))
	{
#ifdef ENABLE_IKASHOP_LOGS
		auto safebox = m_offlineshopSafeboxCache.Get(subpack.ownerid);
		auto& item = safebox->lockedmap.find(subpack.itemid)->second;
		IkarusShopLog(subpack.ownerid, subpack.itemid, "SHOPPING_SAFEBOX_GET_ITEM", 0, "", item.vnum, item.count);
#endif
		SendIkarusShopSafeboxGetItemConfirm(peer, subpack.ownerid, subpack.itemid);
		return true;
	}
	return false;
}

bool CClientManager::RecvIkarusShopSafeboxGetValutes(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketGDSafeboxGetValutes>(data);
	if(m_offlineshopSafeboxCache.RemoveValutes(subpack.ownerid , subpack.valute)){
#ifdef ENABLE_IKASHOP_LOGS
		IkarusShopLog(subpack.ownerid, 0, "SHOPPING_SAFEBOX_GET_VALUTES", 0, "", 0, 0, subpack.valute.yang);
#endif
		return true;
	}
	return false;
}

bool CClientManager::RecvIkarusShopSafeboxAddItem(const char* data) {
	const auto& subpack = Decode<ikashop::TSubPacketGDSafeboxAddItem>(data);
	if (!m_offlineshopSafeboxCache.AddItem(subpack.ownerid, subpack.itemid, subpack.item))
		return false;
#ifdef ENABLE_IKASHOP_LOGS
	IkarusShopLog(subpack.ownerid, subpack.itemid, "SHOPPING_SAFEBOX_ADD_ITEM", 0, "", subpack.item.vnum, subpack.item.count);
#endif
	return true;
}

bool CClientManager::RecvIkarusShopSafeboxGetItemFeedback(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketGDSafeboxGetItemFeedback>(data);

#ifdef ENABLE_IKASHOP_LOGS
	ikashop::TShopPlayerItem* item = nullptr;
	if (auto safebox = m_offlineshopSafeboxCache.Get(subpack.ownerid))
		if (auto iter = safebox->itemsmap.find(subpack.itemid); iter != safebox->itemsmap.end())
			item = &iter->second;
	IkarusShopLog(subpack.ownerid, subpack.itemid, "SHOPPING_SAFEBOX_GET_ITEM_RESULT", 0, subpack.result ? "SUCCESS" : "FAILED", item ? item->vnum : 0, item ? item->count : 0);
#endif

	return subpack.result ?
		m_offlineshopSafeboxCache.RemoveLockedItem(subpack.ownerid, subpack.itemid) :
		m_offlineshopSafeboxCache.UnlockItem(subpack.ownerid, subpack.itemid);
}

#ifdef EXTEND_IKASHOP_PRO
bool CClientManager::RecvIkarusShopNotificationSeen(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketGDNotificationSeen>(data);
	auto query = fmt::format("DELETE FROM `ikashop_notification` WHERE `id` = {}", subpack.id);
	CDBManager::instance().AsyncQuery(query.c_str());
	return true;
}

bool CClientManager::RecvIkarusShopRestoreDuration(const char* data)
{
	const auto& owner = Decode<DWORD>(data);
	auto shop = m_offlineshopShopCache.Get(owner);
	if(!shop 
#ifndef EXTEND_IKASHOP_ULTIMATE
		|| shop->duration != 0
#endif
		)
		return false;

	// updating on cache
	shop->duration = OFFLINESHOP_DURATION_MAX_MINUTES;
	
	// updating on table
	auto query = fmt::format("UPDATE `ikashop_offlineshop` SET `duration` = {} WHERE `owner` = {}", shop->duration, owner);
	CDBManager::instance().AsyncQuery(query.c_str());
	
	// forwarding to cores
	TPacketDGNewIkarusShop pack{};
	pack.bSubHeader = ikashop::SUBHEADER_DG_SHOP_RESTORE_DURATION;

#ifdef ENABLE_IKASHOP_LOGS
	IkarusShopLog(owner, 0, "SHOP_RESTORE_DURATION", owner, "");
#endif

	for(auto& peer : m_peerList)
	{
		if(peer->GetChannel() != 0)
		{
			peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(DWORD));
			peer->Encode(pack);
			peer->Encode(owner);
		}
	}
	return true;
}
#endif

#ifdef ENABLE_IKASHOP_ENTITIES
#ifdef EXTEND_IKASHOP_PRO
bool CClientManager::RecvIkarusShopMoveShopEntity(const char* data)
{
	const auto& info = Decode<ikashop::TSubPacketGDMoveShopEntity>(data);
	if(m_offlineshopShopCache.MoveShopEntity(info.owner, info.spawn))
	{
		TPacketDGNewIkarusShop pack{};
		pack.bSubHeader = ikashop::SUBHEADER_DG_MOVE_SHOP_ENTITY;

		ikashop::TSubPacketDGMoveShopEntity subpack{};
		subpack.owner = info.owner;
		subpack.spawn = info.spawn;

#ifdef ENABLE_IKASHOP_LOGS
		IkarusShopLog(info.owner, 0, "MOVE_SHOP_ENTITY", info.owner, fmt::format("NEW_POS: map({})  x({})  y({})", subpack.spawn.map, subpack.spawn.x, subpack.spawn.y));
#endif

		for(auto& peer : m_peerList)
		{
			if(peer->GetChannel() != 0)
			{
				peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack));
				peer->Encode(pack);
				peer->Encode(subpack);
			}
		}
	}
	return true;
}
#endif
#endif

#ifdef EXTEND_IKASHOP_ULTIMATE
bool CClientManager::RecvIkarusShopMoveItem(const char* data)
{
	const auto& info = Decode<ikashop::TSubPacketGDShopMoveItem>(data);
	if (!m_offlineshopShopCache.MoveItem(info.owner, info.itemid, info.destCell))
		return false;

	TPacketDGNewIkarusShop pack{};
	pack.bSubHeader = ikashop::SUBHEADER_DG_SHOP_MOVE_ITEM;

	ikashop::TSubPacketDGShopMoveItem subpack{};
	subpack.owner = info.owner;
	subpack.itemid = info.itemid;
	subpack.destCell = info.destCell;

#ifdef ENABLE_IKASHOP_LOGS
	IkarusShopLog(info.owner, info.itemid, "MOVE_SHOP_ITEM", info.owner, fmt::format("NEW_ITEM_POS: {}", info.destCell));
#endif

	for (auto& peer : m_peerList)
	{
		peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack));
		peer->Encode(&pack, sizeof(pack));
		peer->Encode(&subpack, sizeof(subpack));
	}

	return true;
}
bool CClientManager::RecvIkarusShopUnlockCell(const char* data)
{
	const auto& owner = Decode<DWORD>(data);

	// getting shop
	auto shop = m_offlineshopShopCache.Get(owner);
	if(!shop)
		return false;

	// updating cache
	shop->lock_index++;

	// updating table
	const auto query = fmt::format("UPDATE `ikashop_offlineshop` SET `lock_index` = {} WHERE `owner` = {}", shop->lock_index, owner);
	CDBManager::instance().AsyncQuery(query.c_str());

#ifdef ENABLE_IKASHOP_LOGS
	IkarusShopLog(owner, 0, "SHOP_EXPAND_SLOT", owner, fmt::format("NEW_LOCK_INDEX: {}", shop->lock_index));
#endif

	// forwarding to cores
	for(auto& peer: m_peerList)
	{
		if(peer->GetChannel() != 0)
		{
			TPacketDGNewIkarusShop pack{};
			pack.bSubHeader = ikashop::SUBHEADER_DG_SHOP_UNLOCK_CELL;

			ikashop::TSubPacketDGShopUnlockCell subpack{};
			subpack.owner = owner;
			subpack.lockIndex = shop->lock_index;

			peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack));
			peer->Encode(pack);
			peer->Encode(subpack);
		}
	}
	return true;
}

void CClientManager::SendIkarusShopDecorationSet(DWORD owner, int index, int remainTime)
{
	TPacketDGNewIkarusShop pack{};
	pack.bSubHeader = ikashop::SUBHEADER_DG_SHOP_DECORATION_SET;

	ikashop::TSubPacketDGShopDecorationSet subpack{};
	subpack.owner = owner;
	subpack.index = index;
	subpack.time = remainTime;

	for (auto& peer : m_peerList)
	{
		peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack));
		peer->Encode(&pack, sizeof(pack));
		peer->Encode(&subpack, sizeof(subpack));
	}
}
#endif

bool CClientManager::SendIkarusShopBuyLockedItemPacket(CPeer* peer, DWORD dwOwner, DWORD dwGuest, DWORD dwItem)
{
	TPacketDGNewIkarusShop pack;
	pack.bSubHeader	= ikashop::SUBHEADER_DG_LOCKED_BUY_ITEM;

	ikashop::TSubPacketDGLockedBuyItem subpack;
	subpack.ownerid	= dwOwner;
	subpack.buyerid	= dwGuest;
	subpack.itemid	= dwItem;

	peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack)+sizeof(subpack));
	peer->Encode(&pack, sizeof(pack));
	peer->Encode(&subpack, sizeof(subpack));

	
	return true;
}

bool CClientManager::SendIkarusShopBuyItemPacket(DWORD dwOwner, DWORD dwGuest, DWORD dwItem)
{
	TPacketDGNewIkarusShop pack;
	pack.bSubHeader	= ikashop::SUBHEADER_DG_BUY_ITEM;

	ikashop::TSubPacketDGBuyItem subpack;
	subpack.ownerid	= dwOwner;
	subpack.buyerid	= dwGuest;
	subpack.itemid	= dwItem;

	for (auto& peer : m_peerList)
	{
		if (peer->GetChannel() != 0) {
			peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack));
			peer->Encode(&pack, sizeof(pack));
			peer->Encode(&subpack, sizeof(subpack));
		}
	}

	
	return true;
}

bool CClientManager::SendIkarusShopEditItemPacket(DWORD dwOwner, DWORD dwItem, const ikashop::TPriceInfo& price)
{
	TPacketDGNewIkarusShop pack;
	pack.bSubHeader	= ikashop::SUBHEADER_DG_EDIT_ITEM;

	ikashop::TSubPacketDGEditItem subpack;
	subpack.ownerid = dwOwner;
	subpack.itemid = dwItem;
	subpack.price = price;

	

	for (auto& peer : m_peerList)
	{
		if (peer->GetChannel() != 0) {
			peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack));
			peer->Encode(&pack, sizeof(pack));
			peer->Encode(&subpack, sizeof(subpack));
		}
	}

	return true;
}

bool CClientManager::SendIkarusShopRemoveItemPacket(DWORD dwOwner, DWORD dwItem, CPeer* requester)
{
	TPacketDGNewIkarusShop pack;
	pack.bSubHeader	= ikashop::SUBHEADER_DG_REMOVE_ITEM;

	ikashop::TSubPacketDGRemoveItem subpack;
	subpack.ownerid	= dwOwner;
	subpack.itemid	= dwItem;

	for (auto& peer : m_peerList)
	{
		if (peer->GetChannel() != 0) {
			// updating requester flag
			subpack.requester = requester == peer;

			peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack));
			peer->Encode(&pack, sizeof(pack));
			peer->Encode(&subpack, sizeof(subpack));
		}
	}

	return true;
}

bool CClientManager::SendIkarusShopAddItemPacket(DWORD dwOwner, DWORD dwItemID, const ikashop::TShopItem& rInfo)
{
	TPacketDGNewIkarusShop pack;
	pack.bSubHeader	= ikashop::SUBHEADER_DG_ADD_ITEM;

	ikashop::TSubPacketDGAddItem subpack;
	subpack.ownerid = dwOwner;
	subpack.itemid = dwItemID;
	subpack.item = rInfo;

	for (auto& peer : m_peerList)
	{
		if (peer->GetChannel() != 0) {
			peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack));
			peer->Encode(&pack, sizeof(pack));
			peer->Encode(&subpack, sizeof(subpack));
		}
	}

	return true;
}

bool CClientManager::SendIkarusShopForceClose(DWORD dwOwnerID)
{
	TPacketDGNewIkarusShop pack{};
	pack.bSubHeader	= ikashop::SUBHEADER_DG_SHOP_FORCE_CLOSE;

	ikashop::TSubPacketDGShopForceClose subpack{};
	subpack.ownerid	= dwOwnerID;

	for (auto& peer : m_peerList)
	{
		if (peer->GetChannel() != 0) {
			peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack));
			peer->Encode(&pack, sizeof(pack));
			peer->Encode(&subpack, sizeof(subpack));
		}
	}

	return true;
}

bool CClientManager::SendIkarusShopCreateNew(const ikashop::TShopInfo& shop)
{
	TPacketDGNewIkarusShop pack{};
	pack.bSubHeader	= ikashop::SUBHEADER_DG_SHOP_CREATE_NEW;

	ikashop::TSubPacketDGShopCreateNew subpack{};
	subpack.shop = shop;

	for (auto& peer : m_peerList)
	{
		if(peer->GetChannel() != 0){
			peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack));
			peer->Encode(&pack, sizeof(pack));
			peer->Encode(&subpack, sizeof(subpack));
		}
	}

	return true;
}

bool CClientManager::SendIkarusShopChangeName(DWORD dwOwnerID, const char* szName)
{
	TPacketDGNewIkarusShop pack;
	pack.bSubHeader	= ikashop::SUBHEADER_DG_SHOP_CHANGE_NAME;

	ikashop::TSubPacketDGShopChangeName subpack;
	subpack.ownerid	= dwOwnerID;
	strncpy(subpack.name, szName, sizeof(subpack.name));


	for (auto& peer : m_peerList)
	{
		if (peer->GetChannel() != 0) {
			peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack) );
			peer->Encode(&pack, sizeof(pack));
			peer->Encode(&subpack, sizeof(subpack));
		}
	}

	return true;
}

bool CClientManager::SendIkarusShopShopExpired(DWORD dwOwnerID)
{
	TPacketDGNewIkarusShop pack;
	pack.bSubHeader	= ikashop::SUBHEADER_DG_SHOP_EXPIRED;

	ikashop::TSubPacketDGShopExpired subpack;
	subpack.ownerid	= dwOwnerID;

	for (auto& peer : m_peerList)
	{
		if (peer->GetChannel() != 0) {
			peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack) );
			peer->Encode(&pack, sizeof(pack));
			peer->Encode(&subpack, sizeof(subpack));
		}
	}

	return true;
}

void CClientManager::SendIkarusShopSafeboxAddItem(DWORD dwOwnerID, DWORD dwItem, const ikashop::TShopPlayerItem& item)
{
	TPacketDGNewIkarusShop pack{};
	pack.bSubHeader	= ikashop::SUBHEADER_DG_SAFEBOX_ADD_ITEM;

	ikashop::TSubPacketDGSafeboxAddItem subpack{};
	subpack.ownerid = dwOwnerID;
	subpack.itemid = dwItem;
	subpack.item = item;

	for (auto& peer : m_peerList)
	{
		if (peer->GetChannel() != 0) {
			peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack));
			peer->Encode(&pack, sizeof(pack));
			peer->Encode(&subpack, sizeof(subpack));
		}
	}
}

void CClientManager::SendIkarusShopSafeboxAddValutes(DWORD dwOwnerID, const ikashop::TValutesInfo& valute)
{
	TPacketDGNewIkarusShop pack;
	pack.bSubHeader	= ikashop::SUBHEADER_DG_SAFEBOX_ADD_VALUTES;

	ikashop::TSubPacketDGSafeboxAddValutes subpack;
	subpack.ownerid = dwOwnerID;
	subpack.valute = valute;

	for (auto& peer : m_peerList)
	{
		if (peer->GetChannel() != 0) {
			peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack));
			peer->Encode(&pack, sizeof(pack));
			peer->Encode(&subpack, sizeof(subpack));
		}
	}
}

//patch 08-03-2020
void CClientManager::SendIkarusShopSafeboxExpiredItem(DWORD dwOwnerID, DWORD itemID) {
	TPacketDGNewIkarusShop pack;
	pack.bSubHeader = ikashop::SUBHEADER_DG_SAFEBOX_EXPIRED_ITEM;

	ikashop::TSubPacketDGSafeboxExpiredItem subpack;
	subpack.ownerid = dwOwnerID;
	subpack.itemid = itemID;

	for (auto& peer : m_peerList)
	{
		if (peer->GetChannel() != 0) {
			peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack));
			peer->Encode(&pack, sizeof(pack));
			peer->Encode(&subpack, sizeof(subpack));
		}
	}
}

void CClientManager::SendIkarusShopSafeboxGetItemConfirm(CPeer* peer, DWORD dwOwnerID, DWORD itemID)
{
	TPacketDGNewIkarusShop pack{};
	pack.bSubHeader = ikashop::SUBHEADER_DG_SAFEBOX_GET_ITEM_CONFIRM;

	ikashop::TSubPacketDGSafeboxGetItemConfirm subpack{};
	subpack.ownerid = dwOwnerID;
	subpack.itemid = itemID;

	peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack));
	peer->Encode(&pack, sizeof(pack));
	peer->Encode(&subpack, sizeof(subpack));
}

void CClientManager::IkarusShopResultAddItemQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo)
{
	auto* qi = static_cast<ikashop::SQueryInfoAddItem*>(pQueryInfo->pvData);
	m_offlineshopShopCache.PutItem(qi->owner, qi->id, *qi);
	SendIkarusShopAddItemPacket(qi->owner, qi->id, *qi);
	delete(qi);
}

void CClientManager::IkarusShopResultCreateShopQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo)
{
	// trying to insert shop
	auto* qi = static_cast<ikashop::SQueryInfoCreateShop*>(pQueryInfo->pvData);
	if(!m_offlineshopShopCache.PutShop(qi->owner, qi->duration, qi->name.c_str()
#ifdef ENABLE_IKASHOP_ENTITIES
		, qi->spawn
#endif
#ifdef EXTEND_IKASHOP_ULTIMATE
		,0/*deco*/, 0/*decotime*/, OFFSHOP_LOCK_INDEX_INIT
#endif
	))
		sys_err("cannot insert new shop , id %d ", qi->owner);

	ikashop::TShopInfo shopInfo{};
	shopInfo.ownerid = qi->owner;
	shopInfo.duration = qi->duration;
	str_to_cstring(shopInfo.name, qi->name.c_str());

#ifdef ENABLE_IKASHOP_ENTITIES
	shopInfo.spawn = qi->spawn;
#endif
#ifdef EXTEND_IKASHOP_ULTIMATE
	shopInfo.lock_index = OFFSHOP_LOCK_INDEX_INIT;
#endif

	SendIkarusShopCreateNew(shopInfo);
	
	delete(qi);
}

void CClientManager::IkarusShopResultSafeboxAddItemQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo)
{
	auto* qi = static_cast<ikashop::SQueryInfoSafeboxAddItem *>(pQueryInfo->pvData);
	m_offlineshopSafeboxCache.PutItem(qi->owner , qi->id, *qi);
	SendIkarusShopSafeboxAddItem(qi->owner, qi->id, *qi);
	delete(qi);
}

void CClientManager::IkarusShopResultQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo)
{
	

	switch (pQueryInfo->iType)
	{
	case QID_OFFLINESHOP_ADD_ITEM:
		IkarusShopResultAddItemQuery(peer, msg, pQueryInfo);
		break;

	case QID_OFFLINESHOP_CREATE_SHOP:
		IkarusShopResultCreateShopQuery(peer, msg, pQueryInfo);
		break;

	case QID_OFFLINESHOP_SAFEBOX_ADD_ITEM:
		IkarusShopResultSafeboxAddItemQuery(peer, msg, pQueryInfo);
		break;

	case QID_OFFLINESHOP_EDIT_ITEM:
	case QID_OFFLINESHOP_REMOVE_ITEM:
	case QID_OFFLINESHOP_DELETE_SHOP:
	case QID_OFFLINESHOP_DELETE_SHOP_ITEM:
	case QID_OFFLINESHOP_SHOP_CHANGE_NAME:
	case QID_OFFLINESHOP_SAFEBOX_DELETE_ITEM:
	case QID_OFFLINESHOP_SAFEBOX_UPDATE_VALUTES:
	case QID_OFFLINESHOP_SAFEBOX_INSERT_VALUTES:
	case QID_OFFLINESHOP_SAFEBOX_UPDATE_VALUTES_ADDING:
	case QID_OFFLINESHOP_UPDATE_SOLD_ITEM:
	case QID_OFFLINESHOP_SHOP_UPDATE_DURATION:
		break;

	default:
		sys_err("Unknown offshop query type %d ",pQueryInfo->iType);
		break;

	}
}

void CClientManager::IkarusShopDurationProcess()
{
	m_offlineshopShopCache.ShopDurationProcess();
	//patch 08-03-2020
	m_offlineshopSafeboxCache.ItemExpirationProcess();
}

void CClientManager::IkarusShopExpiredShop(DWORD dwID
#ifdef EXTEND_IKASHOP_ULTIMATE
	, bool requested
#endif
)
{
	auto cache = m_offlineshopShopCache.Get(dwID);
	if(!cache)
		return;

#ifdef EXTEND_IKASHOP_PRO
#ifdef EXTEND_IKASHOP_ULTIMATE
	if(!requested)
#endif
	{
		AppendIkarusShopNotification(ikashop::ENotificationType::SELLER_SHOP_EXPIRED, dwID, 0, "", "");
	}
#endif

#ifdef EXTEND_IKASHOP_PRO
	cache->duration = 0;
	m_offlineshopShopCache.UpdateDurationQuery(dwID, *cache);
#else
	// caching item map
	auto items { cache->itemsmap };
	// closing the shop
	m_offlineshopShopCache.CloseShop(dwID);
	// inserting into safebox
	for (auto& [id, item] : items)
		m_offlineshopSafeboxCache.AddItem(dwID, id, *item);
#endif

	SendIkarusShopShopExpired(dwID);
}

void CClientManager::IkarusShopLoadShopSafebox(CPeer* peer, DWORD dwID)
{
	// getting safebox
	auto safebox = m_offlineshopSafeboxCache.LoadSafebox(dwID);
	if (!safebox)
	{
		sys_err("cannot load shop safebox for pid %d ",dwID);
		return;
	}

#ifdef ENABLE_IKASHOP_LOGS
	IkarusShopLog(dwID, 0, "SHOP_SAFEBOX_LOAD", 0, fmt::format("ITEM_COUNT: {}", safebox->itemsmap.size()));
#endif

	TPacketDGNewIkarusShop pack{};
	pack.bSubHeader = ikashop::SUBHEADER_DG_SAFEBOX_LOAD;

	ikashop::TSubPacketDGSafeboxLoad subpack;
	subpack.ownerid = dwID;
	subpack.itemcount = safebox->itemsmap.size();
	subpack.valute = safebox->valutes;

	const auto packetSize = sizeof(pack) + sizeof(subpack) + sizeof(ikashop::TShopPlayerItem) * safebox->itemsmap.size();
	peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, packetSize);
	peer->Encode(&pack, sizeof(pack));
	peer->Encode(&subpack, sizeof(subpack));

	for (auto& [id, item] : safebox->itemsmap)
		peer->Encode(&item, sizeof(item));
}

#ifdef EXTEND_IKASHOP_PRO
void CClientManager::IkarusShopLoadNotifications(CPeer* peer, DWORD owner)
{
	auto query = fmt::format("SELECT `id`, `type`, `who`, `what`, `format`, `seen`, UNIX_TIMESTAMP(`datetime`) FROM `ikashop_notification` WHERE `owner` = {} ORDER BY `datetime` ASC LIMIT 200", owner);
	auto msg = CDBManager::instance().DirectQuery(query.c_str());

	if(!msg || !msg->Get() || msg->uiSQLErrno != 0)
	{
		sys_err("CANNOT LOAD NOTIFICATION FOR OWNER %u, errorno %d", owner, (int) msg->uiSQLErrno);
		return;
	}

	std::vector<ikashop::TNotificationInfo> notifications;
	for(uint32_t i=0; i < msg->Get()->uiNumRows; i++)
	{
		if(auto row = mysql_fetch_row(msg->Get()->pSQLResult))
		{
			int col = 0;

			auto& notification = notifications.emplace_back();
			str_to_number(notification.id, row[col++]);
			str_to_enum(notification.type, row[col++]);
			str_to_cstring(notification.who, row[col++]);
			str_to_number(notification.what, row[col++]);
			str_to_cstring(notification.format, row[col++]);
			str_to_number(notification.seenflag, row[col++]);
			str_to_number(notification.datetime, row[col++]);
		}
	}

	if(!notifications.empty())
	{
		TPacketDGNewIkarusShop pack{};
		pack.bSubHeader = ikashop::SUBHEADER_DG_NOTIFICATION_LOAD;

		ikashop::TSubPacketDGNotificationLoad subpack{};
		subpack.ownerid = owner;
		subpack.count = notifications.size();

		peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack) + sizeof(ikashop::TNotificationInfo) * notifications.size());
		peer->Encode(&pack, sizeof(pack));
		peer->Encode(&subpack, sizeof(subpack));
		peer->Encode(notifications.data(), sizeof(ikashop::TNotificationInfo) * notifications.size());
	}
}

void CClientManager::AppendIkarusShopNotification(ikashop::ENotificationType type, DWORD owner, DWORD what, const std::string& who, const std::string& format)
{
	// escaping who
	static char escapeWho[500];
	CDBManager::instance().EscapeString(escapeWho, who.c_str(), who.size());
	// escaping format
	static char escapeFormat[500];
	CDBManager::instance().EscapeString(escapeFormat, format.c_str(), format.size());
	// making query
	auto query = fmt::format("INSERT INTO `ikashop_notification` (`owner`, `type`, `who`, `what`, `format`, `seen`) VALUES({}, {}, '{}', {}, '{}', {})",
								owner, (int)type, escapeWho, what, escapeFormat, 0);
	// executing query
	auto msg = CDBManager::instance().DirectQuery(query.c_str());
	if(!msg || !msg->Get() || msg->uiSQLErrno != 0)
	{
		sys_err("CANNOT INSERT NOTIFICATION INTO TABLE, SKIPPING: Owner (%u)  Type (%d)", owner, (int)type);
		return;
	}
	
	// forwarding notification
	TPacketDGNewIkarusShop pack{ ikashop::SUBHEADER_DG_NOTIFICATION_FORWARD };

	ikashop::TSubPacketDGNotificationForward subpack{};
	subpack.datetime = static_cast<int64_t>(std::time(nullptr));
	subpack.id = msg->Get()->uiInsertID;
	subpack.seenflag = false;
	subpack.type = type;
	subpack.what = what;
	subpack.ownerid = owner;
	
	str_to_cstring(subpack.format, format.c_str());
	str_to_cstring(subpack.who, who.c_str());

	for(auto& peer : m_peerList)
	{
		if(peer->GetChannel() != 0)
		{
			peer->EncodeHeader(HEADER_DG_NEW_OFFLINESHOP, 0, sizeof(pack) + sizeof(subpack));
			peer->Encode(pack);
			peer->Encode(subpack);
		}
	}
}
#endif

//updated 15-01-2020
bool CClientManager::IsUsingIkarusShopSystem(DWORD dwID)
{
	// searching shop
	if(auto shop = m_offlineshopShopCache.Get(dwID))
		if(shop->duration > 0)
			return true;
	return false;
}

#ifdef ENABLE_IKASHOP_LOGS
void CClientManager::IkarusShopLog(DWORD who, DWORD itemid, std::string_view what, DWORD shopOwner, const std::string& extra, DWORD vnum, DWORD count, int64_t yang, int cheque)
{
	//* making query
	std::string query = "INSERT INTO `ikarusshop_log` (";
	query += "`who`, `itemid`, `what`, `shop_owner`, `extra`, `vnum`, `count`, `yang`";
#ifdef ENABLE_CHEQUE_SYSTEM
	query += ", `cheque`";
#endif
	query += ") ";

	query += fmt::format("VALUES({}, {}, '{}', {}, '{}', {}, {}, {}",
		who, itemid, what, shopOwner, extra, vnum, count, yang);
#ifdef ENABLE_CHEQUE_SYSTEM
	query += fmt::format(", {}", cheque);
#endif
	query += ");";

	CDBManager::instance().AsyncQuery(query.c_str(), SQL_LOG);
}
#endif

std::string CreateIkarusShopSelectShopItemsQuery()
{
	std::string query = "SELECT `id`, `owner_id`, `pos`, `vnum`, `count`, ";
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
		query += fmt::format("`socket{}`, ", i);

	for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
		query += fmt::format("`attrtype{}`, `attrvalue{}`, ", i, i);

#ifdef ENABLE_CHANGELOOK_SYSTEM
	query += "`transmutation`, ";
#endif
	//patch 08-03-2020
	query += "`ikashop_data` FROM `item` WHERE `window` = 'IKASHOP_OFFLINESHOP'";
	return query;
}
#endif