#include "stdafx.h"

#ifdef ENABLE_IKASHOP_RENEWAL
#include "Main.h"
#include "Config.h"
#include "DBManager.h"
#include "QID.h"
#include "Peer.h"
#include "ClientManager.h"
#include "IkarusShopCache.h"


namespace ikashop
{
	template<class T>
	bool IsItemExpired(const auto now, const T& item){
		switch (static_cast<ikashop::ExpirationType>(item.expiration)) {
		case ikashop::ExpirationType::EXPIRE_REAL_TIME_FIRST_USE:
			// checking activation only for real time first use
			if (item.alSockets[1] == 0)
				return false;
		case ikashop::ExpirationType::EXPIRE_REAL_TIME:
			// checking timestamp on socket 0 for both timers
			if (now < item.alSockets[0])
				return false;
			// inserting into vector
			return true;
		default:
			break;
		}
		return false;
	}

	//SHOPS
	std::string CreateShopCacheUpdateItemQuery(const TShopItem& rItemPrice);
	std::string CreateShopCacheDeleteShopQuery(DWORD dwOwner);
	std::string CreateShopCacheDeleteShopItemQuery(DWORD dwOwner);
#ifdef ENABLE_IKASHOP_ENTITIES
	std::string CreateShopCacheInsertShopQuery(DWORD dwOwnerID, DWORD dwDuration, const char* name, const TShopSpawn& spawn);
#else
	std::string CreateShopCacheInsertShopQuery(DWORD dwOwnerID, DWORD dwDuration, const char* name);
#endif
	std::string CreateShopCacheUpdateDurationQuery(DWORD dwOwnerID, DWORD dwDuration);
	std::string CreateShopCacheDeleteItemQuery(DWORD dwOwnerID, DWORD dwItemID);

	//SAFEBOX
	std::string CreateSafeboxCacheDeleteItemQuery(DWORD dwItem);
	std::string CreateSafeboxCacheInsertItemQuery(DWORD dwOwner, DWORD dwItemID, const TShopPlayerItem& item);
	std::string CreateSafeboxCacheUpdateValutes(DWORD dwOwner, const TValutesInfo& val);
	std::string CreateSafeboxCacheInsertSafeboxValutesQuery(DWORD dwOwnerID);
	std::string CreateSafeboxCacheUpdateValutesByAdding(DWORD dwOwner, const TValutesInfo& val);
	std::string CreateSafeboxCacheLoadItemsQuery(DWORD dwOwnerID);
	std::string CreateSafeboxCacheLoadValutesQuery(DWORD dwOwnerID);

	/*
			CSHOPCACHE
	*/

	CShopCache::CShopCache()
	{
	}

	CShopCache::~CShopCache()
	{
	}

	CShopCache::SHOP_HANDLE CShopCache::Get(DWORD dwOwnerID) const
	{
		// searching shop cache
		auto it = m_shopsMap.find(dwOwnerID);
		if (it == m_shopsMap.end())
			return nullptr; // not found
		return it->second;
	}

	bool CShopCache::AddItem(const TShopCacheItemInfo& item)
	{
		// searching shop cache
		auto cache = Get(item.owner);
		if(!cache)
			return false;

		// refreshing table datas
		std::string query = CreateShopCacheUpdateItemQuery(item);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_ADD_ITEM, 0, new SQueryInfoAddItem(item));
		return true;
	}

	bool CShopCache::RemoveItem(DWORD dwOwnerID, DWORD dwItemID)
	{
		// searching for shop cache
		auto cache = Get(dwOwnerID);
		if(!cache)
			return false;

		// searching for item cache
		auto& itemsmap = cache->itemsmap;
		auto it = itemsmap.find(dwItemID);
		if (it == itemsmap.end())
			return false;

		// skipping locked items
		if(it->second->lock)
			return false;

		// removing item from cache
		itemsmap.erase(it);
		// removing item from db
		std::string query = CreateShopCacheDeleteItemQuery(dwOwnerID, dwItemID);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_REMOVE_ITEM, 0, NULL);

		return true;
	}

	void CShopCache::DeleteItem(DWORD owner, DWORD itemid)
	{
		// removing from table
		auto query = fmt::format("DELETE FROM `item` WHERE `id`={}", itemid);
		CDBManager::instance().AsyncQuery(query.c_str());

		// removing from cache
		if (auto shop = Get(owner))
			shop->itemsmap.erase(itemid);
	}

	bool CShopCache::SellItem(DWORD dwOwnerID, DWORD dwItemID, bool updateQuery)
	{
		// searching for shop cache
		auto cache = Get(dwOwnerID);
		if(!cache)
			return false;

		// searching for item cache
		auto& itemsmap = cache->itemsmap;
		auto it = itemsmap.find(dwItemID);
		if (it == itemsmap.end())
			return false; // not found

		// skipping unlocked items
		if(!it->second->lock)
			return false;

		// removing item from cache
		itemsmap.erase(it);
		// updating ikashop_data
		if (updateQuery)
		{
			std::string query = CreateShopCacheDeleteItemQuery(dwOwnerID, dwItemID);
			CDBManager::instance().DirectQuery(query.c_str());
		}

		return true;
	}

	bool CShopCache::LockSellItem(DWORD dwOwnerID, DWORD dwItemID, long long TotalPriceSeen) //patch seen price check
	{
		// searching shop cache
		auto cache = Get(dwOwnerID);
		if(!cache)
			return false;

		// searching item cache
		auto& itemsmap = cache->itemsmap;
		auto it = itemsmap.find(dwItemID);
		if (it == itemsmap.end())
			return false; // not found

		// skipping locked items
		if (it->second->lock)
			return false;

		// skipping items with a changed price
		if (it->second->price.GetTotalYangAmount() != TotalPriceSeen)
			return false;

		// locking item
		it->second->lock = true;
		return true;
	}

	bool CShopCache::UnlockSellItem(DWORD dwOwnerID, DWORD dwItemID)//topatch
	{
		// searching shop cache
		auto cache = Get(dwOwnerID);
		if(!cache)
			return false;

		// searching item cache
		auto& itemsmap = cache->itemsmap;
		auto it = itemsmap.find(dwItemID);
		if (it == itemsmap.end())
			return false; // not found

		// skipping unlocked items
		if (!it->second->lock)
			return false;

		// unlocking item
		it->second->lock = false;
		return true;
	}

	bool CShopCache::EditItem(DWORD dwOwnerID, DWORD dwItemID, const TPriceInfo& price)
	{
		// searching shop cache
		auto cache = Get(dwOwnerID);
		if(!cache)
			return false;

		// searching item cache
		auto& itemsmap = cache->itemsmap;
		auto it = itemsmap.find(dwItemID);
		if (it == itemsmap.end())
			return false;	

		// skipping locked items
		if(it->second->lock)
			return false;

		// updating price on cache
		it->second->price = price;
		// updating price on database
		std::string query = CreateShopCacheUpdateItemQuery(*it->second);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_EDIT_ITEM, 0, NULL);
		return true;
	}

	bool CShopCache::CloseShop(DWORD dwOwnerID)
	{
		// searching shop cache
		auto it = m_shopsMap.find(dwOwnerID);
		if (it == m_shopsMap.end())
			return false;

		// removing shop cache
		m_shopsMap.erase(it);
		// removing shop from db
		const auto query = CreateShopCacheDeleteShopQuery(dwOwnerID);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_DELETE_SHOP, 0, NULL);
		return true;
	}

#ifdef ENABLE_IKASHOP_ENTITIES
	bool CShopCache::CreateShop(DWORD dwOwnerID, DWORD dwDuration, const char* szName, const TShopSpawn& spawn)
#else
	bool CShopCache::CreateShop(DWORD dwOwnerID, DWORD dwDuration, const char* szName)
#endif
	{
		// searching if shop already exists
		auto cache = Get(dwOwnerID);
		if(cache)
			return false; // it already exists
		
		// making query create shop info
		auto* qi = new SQueryInfoCreateShop;
		qi->owner = dwOwnerID;
		qi->duration = dwDuration;
		qi->name = szName;
#ifdef ENABLE_IKASHOP_ENTITIES
		qi->spawn = spawn;
#endif
		// sending query
		std::string query = CreateShopCacheInsertShopQuery(dwOwnerID, dwDuration, szName
#ifdef ENABLE_IKASHOP_ENTITIES
			, spawn
#endif
		);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_CREATE_SHOP, 0, qi);

		
		return true;
	}

	bool CShopCache::ChangeShopName(DWORD dwOwnerID, const char* szName)
	{
		// searching shop cache
		auto cache = Get(dwOwnerID);
		if(!cache)
			return false;

		// updating cached name
		cache->name = szName;
		return true;
	}

	bool CShopCache::PutItem(DWORD dwOwnerID, DWORD dwItemID, const TShopCacheItemInfo& item)
	{
		// searching shop cache
		auto cache = Get(dwOwnerID);
		if(!cache)
			return false;

		// checking item already exists
		auto& itemsmap = cache->itemsmap;
		if(itemsmap.contains(dwItemID))
			return false; // already exists

		// inserting the item
		itemsmap[dwItemID] = std::make_shared<TShopCacheItemInfo>(item);
		return true;
	}

	bool CShopCache::PutShop(DWORD dwOwnerID, DWORD dwDuration, const char* szName
#ifdef ENABLE_IKASHOP_ENTITIES
		, const TShopSpawn& spawn
#endif
#ifdef EXTEND_IKASHOP_ULTIMATE
		, int decoration, int decoration_time, int lock_index
#endif
	)
	{
		// searching shop cache already exists
		if(m_shopsMap.contains(dwOwnerID))
			return false;

		auto& shop = m_shopsMap[dwOwnerID] = std::make_shared<TShopCacheInfo>();
		shop->duration = dwDuration;
		shop->name = szName;
#ifdef ENABLE_IKASHOP_ENTITIES
		shop->spawn = spawn;
#endif
#ifdef EXTEND_IKASHOP_ULTIMATE
		shop->decoration = decoration;
		shop->decoration_time = decoration_time;
		shop->lock_index = lock_index;
#endif
		return true;
	}

	void CShopCache::EncodeCache(CPeer* peer) const
	{		
		for(auto& [owner, shop] : m_shopsMap)
		{
			static TShopInfo pack{};
			pack.duration = shop->duration;
			pack.ownerid = owner;
			pack.count = shop->itemsmap.size();
			str_to_cstring(pack.name, shop->name.c_str());
#ifdef ENABLE_IKASHOP_ENTITIES
			pack.spawn = shop->spawn;
#endif
#ifdef EXTEND_IKASHOP_ULTIMATE
			pack.decoration = shop->decoration;
			pack.decoration_time = shop->decoration_time;
			pack.lock_index = shop->lock_index;
#endif
			peer->Encode(&pack, sizeof(pack));
			for (auto& [id, item] : shop->itemsmap)
				peer->Encode(static_cast<TShopItem*>(item.get()), sizeof(TShopItem));
		}
	}


	DWORD CShopCache::GetItemCount() const
	{
		return std::accumulate(m_shopsMap.begin(), m_shopsMap.end(), 0UL, 
			[](const auto& val, const auto& iter){ return val + iter.second->itemsmap.size(); });
	}

	void CShopCache::ShopDurationProcess()
	{
		//expired check
		std::set<DWORD> expiredShops;

		for (auto& [owner, shop] : m_shopsMap)
		{
			// checking for expiring shop
			if(shop->duration == 1)
				expiredShops.emplace(owner);

			if (shop->duration != 0 && --shop->duration != 0 && shop->duration % 5 == 0)
				UpdateDurationQuery(owner, *shop);
#ifdef EXTEND_IKASHOP_ULTIMATE
			if (shop->decoration_time != 0 && --shop->decoration_time != 0 && shop->decoration_time % 5 == 0)
				UpdateDecorationTime(owner, shop->decoration_time);

			if (shop->decoration != 0 && shop->decoration_time == 0)
			{
				// updating cache
				shop->decoration = 0;
				// updating table
				const auto query = fmt::format("UPDATE `ikashop_offlineshop` SET `decoration` = 0, `decoration_time` = 0 WHERE `owner` = {}", owner);
				CDBManager::instance().DirectQuery(query.c_str());
				// updating cores
				CClientManager::instance().SendIkarusShopDecorationSet(owner, 0, 0);
				CClientManager::instance().AppendIkarusShopNotification(ikashop::ENotificationType::SELLER_DECORATION_EXPIRED, owner, 0, "", "");
#ifdef ENABLE_IKASHOP_LOGS
				CClientManager::instance().IkarusShopLog(owner, 0, "SHOP_DECORATION_EXPIRED", owner);
#endif
			}
#endif
		}

		//item expired check
		std::vector<std::pair<DWORD, DWORD>> item_vec;
		const time_t now = std::time(nullptr);

		for (auto& [owner, shop] : m_shopsMap)
		{
			// skipping expired shops
			if(expiredShops.contains(owner))
				continue;

			// searching for expired items
			for(auto& [id, item] : shop->itemsmap)
				if(IsItemExpired(now, *item))
					item_vec.emplace_back(std::make_pair(owner, id));
		}

		// removing expired shops
		for (auto owner : expiredShops){
#ifdef ENABLE_IKASHOP_LOGS
			CClientManager::instance().IkarusShopLog(owner, 0, "SHOP_DURATION_EXPIRED", owner);
#endif
			CClientManager::instance().IkarusShopExpiredShop(owner);
		}

		// removing expired items
		for(auto& [owner,id] : item_vec){
#ifdef ENABLE_IKASHOP_LOGS
			auto item = GetItem(owner, id);
			CClientManager::instance().IkarusShopLog(owner, id, "SHOP_ITEM_EXPIRED", owner, "", item->vnum, item->count);
#endif
			CClientManager::Instance().SendIkarusShopRemoveItemPacket(owner, id);
			DeleteItem(owner, id);
		}
	}

	void CShopCache::UpdateDurationQuery(DWORD dwOwnerID, const TShopCacheInfo& rShop)
	{
		std::string query = CreateShopCacheUpdateDurationQuery(dwOwnerID, rShop.duration);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_SHOP_UPDATE_DURATION, 0 , NULL);
	}

	CShopCache::ITEM_CACHE_HANDLE CShopCache::GetItem(DWORD owner, DWORD item)
	{
		if(auto shop = Get(owner))
			if(auto iter = shop->itemsmap.find(item); iter != shop->itemsmap.end())
				return iter->second;
		return nullptr;
	}

#ifdef ENABLE_IKASHOP_ENTITIES
#ifdef EXTEND_IKASHOP_PRO
	bool CShopCache::MoveShopEntity(DWORD owner, const TShopSpawn& spawn)
	{
		auto shop = Get(owner);
		if(!shop)
			return false;

		// update table
		auto query = fmt::format("UPDATE `ikashop_offlineshop` SET `map` = {}, `x` = {}, `y` = {} WHERE `owner` = {}", spawn.map, spawn.x, spawn.y, owner);
		CDBManager::instance().AsyncQuery(query.c_str());

		// update cache
		shop->spawn = spawn;

		return true;
	}
#endif
#endif

#ifdef EXTEND_IKASHOP_ULTIMATE
	bool CShopCache::ChangeDecoration(DWORD owner, int index)
	{
		auto shop = Get(owner);
		if (!shop)
			return false;

		shop->decoration = index;
		shop->decoration_time = OFFSHOP_DECORATION_TIME;
		return true;
	}

	void CShopCache::UpdateDecorationTime(DWORD owner, int time)
	{
		const auto& query = fmt::format("UPDATE `ikashop_offlineshop` SET `decoration_time` = {} WHERE `owner` = {}", time, owner);
		CDBManager::instance().AsyncQuery(query.c_str());
	}

	bool CShopCache::MoveItem(DWORD owner, DWORD itemid, int destCell)
	{
		auto item = GetItem(owner, itemid);
		if (!item || item->lock)
			return false;

		// updating cache
		item->pos = destCell;

		// updating table
		const auto query = fmt::format("UPDATE `item` SET `pos` = {} WHERE `id` = {} AND `owner_id` = {} AND `window` = 'IKASHOP_OFFLINESHOP'", destCell, itemid, owner);
		CDBManager::instance().AsyncQuery(query.c_str());

		return true;
	}
#endif

	//SAFEBOX CHACHE
	CSafeboxCache::CSafeboxCache()
	{
	}

	CSafeboxCache::~CSafeboxCache()
	{
	}

	CSafeboxCache::SAFEBOX_HANDLE CSafeboxCache::Get(DWORD dwOwnerID) const
	{
		auto it = m_safeboxMap.find(dwOwnerID);
		if(it == m_safeboxMap.end())
			return nullptr;
		return it->second;
	}

	bool CSafeboxCache::PutSafebox(DWORD dwOwnerID, const TSafeboxCacheInfo& safebox)
	{
		// searching for duplicates
		if(m_safeboxMap.contains(dwOwnerID))
			return false;
		// inserting into map
		m_safeboxMap[dwOwnerID] = std::make_shared<TSafeboxCacheInfo>(safebox);
		return true;
	}

	bool CSafeboxCache::PutItem(DWORD dwOwnerID, DWORD dwItem, const TShopPlayerItem& item)
	{
		// searching safebox cache
		auto cache = Get(dwOwnerID);
		if(!cache)
			return false;

		// searching safebox item cache
		auto& itemsmap = cache->itemsmap;
		if(itemsmap.contains(dwItem))
			return false;

		// inserting into map
		itemsmap[dwItem] = item;
		return true;
	}

	bool CSafeboxCache::DeleteItem(DWORD dwOwnerID, DWORD dwItemID)
	{
		// searching safebox cache
		auto cache = Get(dwOwnerID);
		if(!cache)
			return false;

		// searching safebox item cache
		auto& itemsmap = cache->itemsmap;
		auto it = itemsmap.find(dwItemID);
		if (it == itemsmap.end())
			return false;

		// removing item from cache
		itemsmap.erase(it);

		// removing item from table
		std::string query = CreateSafeboxCacheDeleteItemQuery(dwItemID);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_SAFEBOX_DELETE_ITEM, 0, NULL);
		return true;
	}

	bool CSafeboxCache::LockItem(DWORD dwOwner, DWORD dwItemID)
	{
		// searching safebox cache
		auto cache = Get(dwOwner);
		if (!cache)
			return false;

		// searching safebox item cache
		auto& itemsmap = cache->itemsmap;
		auto it = itemsmap.find(dwItemID);
		if (it == itemsmap.end())
			return false;

		// adding item to locked map
		auto& lockmap = cache->lockedmap;
		lockmap[dwItemID] = it->second;

		// removing item from cache
		itemsmap.erase(it);
		return true;
	}

	bool CSafeboxCache::RemoveLockedItem(DWORD dwOwner, DWORD dwItemID)
	{
		// searching safebox cache
		auto cache = Get(dwOwner);
		if (!cache)
			return false;

		// searching into lockmap
		auto& lockmap = cache->lockedmap;
		auto lockiter = lockmap.find(dwItemID);
		if(lockiter == lockmap.end())
			return false;

		// removing from map
		lockmap.erase(lockiter);

		// blanking ikashop_data on db
		auto query = fmt::format("UPDATE `item` SET `ikashop_data` = '' WHERE `id` = {}", dwItemID);
		CDBManager::instance().DirectQuery(query.c_str());

		return true;
	}

	bool CSafeboxCache::UnlockItem(DWORD dwOwner, DWORD dwItemID)
	{
		// searching safebox cache
		auto cache = Get(dwOwner);
		if (!cache)
			return false;

		// searching locked item
		auto& lockmap = cache->lockedmap;
		auto lockiter = lockmap.find(dwItemID);
		if(lockiter == lockmap.end())
			return false;

		// adding item to itemsmap
		auto& itemsmap = cache->itemsmap;
		itemsmap[dwItemID] = lockiter->second;

		// removing from locked items
		lockmap.erase(lockiter);
		return true;
	}

	bool CSafeboxCache::AddItem(DWORD dwOwnerID, DWORD dwItemID, const TShopPlayerItem& item)
	{
		// saving item into query info
		auto queryItem = new SQueryInfoSafeboxAddItem(item);
		queryItem->owner = dwOwnerID; // updating owner

		// pushing query
		std::string query = CreateSafeboxCacheInsertItemQuery(dwOwnerID, dwItemID, item);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_SAFEBOX_ADD_ITEM, 0, queryItem);
		query = fmt::format("UPDATE `item` SET `window` = 'IKASHOP_SAFEBOX', `owner_id` = {}  WHERE `id` = {} ", dwOwnerID, dwItemID);
		CDBManager::Instance().DirectQuery(query.c_str());
		return true;
	}

	bool CSafeboxCache::AddValutes(DWORD dwOwnerID, const TValutesInfo& val)
	{
		// searching safebox cache
		auto cache = Get(dwOwnerID);
		if(!cache)
		{
			// adding as query -> it wasn't loaded yet on cache
			AddValutesAsQuery(dwOwnerID, val);
			return true;
		}

		// updating values on cache
		cache->valutes += val;
		// updating values into db
		std::string query = CreateSafeboxCacheUpdateValutes(dwOwnerID, cache->valutes);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_SAFEBOX_UPDATE_VALUTES, 0, NULL);
		return true;
	}

	void CSafeboxCache::AddValutesAsQuery(DWORD dwOwnerID, const TValutesInfo& val)
	{
		std::string query=CreateSafeboxCacheUpdateValutesByAdding(dwOwnerID,val);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_SAFEBOX_UPDATE_VALUTES_ADDING, 0, NULL);
	}

	bool CSafeboxCache::RemoveValutes(DWORD dwOwnerID, const TValutesInfo& val)
	{
		// searching cache
		auto cache = Get(dwOwnerID);
		if(!cache)
			return false;

		// updating values into cache
		cache->valutes -= val;
		// updating values into db
		std::string query = CreateSafeboxCacheUpdateValutes(dwOwnerID, cache->valutes);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_SAFEBOX_UPDATE_VALUTES, 0, NULL);
		return true;
	}

	CSafeboxCache::SAFEBOX_HANDLE CSafeboxCache::CreateSafebox(DWORD dwOwnerID)
	{
		// inserting into db an empty safebox
		std::string query = CreateSafeboxCacheInsertSafeboxValutesQuery(dwOwnerID);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_SAFEBOX_INSERT_VALUTES, 0, NULL);

		// inserting into cache
		return (m_safeboxMap[dwOwnerID] = std::make_shared<TSafeboxCacheInfo>());
	}

	CSafeboxCache::SAFEBOX_HANDLE CSafeboxCache::LoadSafebox(DWORD dwPID)
	{
		// searching inside cache
		auto cache = Get(dwPID);
		if(cache)
			return cache;

		// making query
		const auto query = CreateSafeboxCacheLoadValutesQuery(dwPID);
		auto msg = CDBManager::instance().DirectQuery(query.c_str());

		// checking for errors
		if(msg->uiSQLErrno != 0)
			return nullptr;

		// no row so creating it from 0
		if (msg->Get()->uiAffectedRows == 0)
			return CreateSafebox(dwPID);

		// inserting new empty cache
		auto& safebox = m_safeboxMap[dwPID] = std::make_shared<TSafeboxCacheInfo>();

		// extracting data
		auto row = mysql_fetch_row(msg->Get()->pSQLResult);
		str_to_number(safebox->valutes.yang, row[0]);

		// making query selecting items
		const auto itemquery = CreateSafeboxCacheLoadItemsQuery(dwPID);
		msg = CDBManager::instance().DirectQuery(itemquery.c_str());

		// extracting items
		auto& itemsmap = safebox->itemsmap;
		auto item = TShopPlayerItem{};
		while ((row = mysql_fetch_row(msg->Get()->pSQLResult)))
		{
			int col = 0;
			str_to_number(item.id, row[col++]);
			str_to_number(item.vnum, row[col++]);
			str_to_number(item.count, row[col++]);

			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
				str_to_number(item.alSockets[i], row[col++]);

			for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
			{
				str_to_number(item.aAttr[i].bType, row[col++]);
				str_to_number(item.aAttr[i].sValue, row[col++]);
			}

#ifdef ENABLE_CHANGELOOK_SYSTEM
			str_to_number(item.dwTransmutation, row[col++]);
#endif
			auto result = str_to_json(row[col++]);
			if(!result){
				sys_err("CANNOT EXTRACT JSON FROM SHOP SAFEBOX ITEM : pid(%u) item_id(%u)", dwPID, item.id);
				continue;
			}

			auto expirationResult = jsonHelper::getValue<int>(result.value(), "expiration");
			if(!expirationResult){
				sys_err("CANNOT EXTRACT JSON EXPIRATION FROM SHOP SAFEBOX ITEM : pid(%u) item_id(%u)", dwPID, item.id);
				continue;
			}

			item.owner = dwPID;
			item.expiration = static_cast<ikashop::ExpirationType>(expirationResult.value());
			itemsmap[item.id] = item;
		}

		return safebox;
	}

	DWORD CSafeboxCache::GetItemCount() const
	{
		return std::accumulate(m_safeboxMap.begin(), m_safeboxMap.end(), 0UL,
			[](const auto& value, const auto& iter){ return value + iter.second->itemsmap.size(); });
	}

	//patch 08-03-2020
	void CSafeboxCache::ItemExpirationProcess() {
		// making temp vector
		std::vector<std::pair<DWORD, DWORD>> vec;
		const auto now = std::time(nullptr);

		// iterating checking item expiration
		for (auto& [owner, safebox] : m_safeboxMap){
			for (auto& [id,item] : safebox->itemsmap) {
				if(IsItemExpired(now, item)){
#ifdef ENABLE_IKASHOP_LOGS
					CClientManager::instance().IkarusShopLog(owner, id, "SAFEBOX_ITEM_EXPIRED", owner, "", item.vnum, item.count);
#endif
					vec.emplace_back(std::make_pair(owner, id));
				}
			}
		}

		// removing expired items
		for (const auto& [owner, id] : vec) 
		{
			DeleteItem(owner, id);
			CClientManager::instance().SendIkarusShopSafeboxExpiredItem(owner, id);
		}
	}

#ifdef EXTEND_IKASHOP_ULTIMATE
	bool CAveragePriceCache::InitializeTable()
	{
		auto msg = CDBManager::instance().DirectQuery("SELECT `id`, `vnum`, `count`, `price`, UNIX_TIMESTAMP(`datetime`) FROM `ikashop_sale`");
		if(!msg || !msg->Get() || msg->uiSQLErrno != 0)
			return false;

		const auto currentTime = std::time(nullptr);
		m_priceAverageVector.reserve(msg->Get()->uiNumRows);

		for(uint32_t i=0; i < msg->Get()->uiNumRows; i++)
		{
			if(auto row = mysql_fetch_row(msg->Get()->pSQLResult))
			{
				int col = 0;
				TSaleHistory history{};
				str_to_number(history.account, row[col++]);
				str_to_number(history.vnum, row[col++]);
				str_to_number(history.count, row[col++]);
				str_to_number(history.price, row[col++]);
				str_to_number(history.datetime, row[col++]);

				if(history.datetime + OFFLINESHOP_EXPIRING_SALE_HISTORY < currentTime)
					continue;

				m_priceAverageVector.emplace_back(history);
			}
		}

		auto deleteQuery = fmt::format("DELETE FROM `ikashop_sale` WHERE UNIX_TIMESTAMP(`datetime`) < {}", currentTime - OFFLINESHOP_EXPIRING_SALE_HISTORY);
		CDBManager::instance().AsyncQuery(deleteQuery.c_str());
		return true;
	}

	bool CAveragePriceCache::RegisterSelling(const TSaleHistory& data)
	{
		auto& vnumMap = m_cooldownMap[data.account];
		auto& timestamp = vnumMap[data.vnum];

		const auto currentTime = std::time(nullptr);

		if(timestamp + OFFLINESHOP_SALE_HISTORY_COOLDOWN > currentTime)
			return false;
		
		timestamp = currentTime;

		// adding to cache
		m_priceAverageVector.emplace_back(data);
		
		// adding to table
		const auto query = fmt::format("INSERT INTO `ikashop_sale` (`id`, `vnum`, `count`, `price`) VALUES({}, {}, {}, {})", data.account, data.vnum, data.count, data.price);
		CDBManager::instance().AsyncQuery(query.c_str());

		return true;
	}

	void CAveragePriceCache::Encode(CPeer* peer)
	{
		peer->EncodeDWORD(m_priceAverageVector.size());
		if(!m_priceAverageVector.empty())
			peer->Encode(m_priceAverageVector.data(), sizeof(TSaleHistory)* m_priceAverageVector.size());
	}
#endif

	// QUERY MAKE
	std::string CreateShopCacheUpdateItemQuery(const TShopItem& item)
	{
		// making json object
		rapidjson::Document value;
		value.SetObject();
		// inserting values
		auto& allocator = value.GetAllocator();
		jsonHelper::insertValue(value, "yang", item.price.yang, allocator);
#ifdef ENABLE_CHEQUE_SYSTEM
		jsonHelper::insertValue(value, "cheque", item.price.cheque, allocator);
#endif
		jsonHelper::insertValue(value, "expiration", static_cast<int>(item.expiration), allocator);
		// making json string
		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> StringWriter(buffer);
		value.Accept(StringWriter);
		// escaping json string
		static char escape[1024];
		CDBManager::instance().EscapeString(escape, buffer.GetString(), buffer.GetSize());
		// making query
		return fmt::format("UPDATE `item` SET `ikashop_data`='{}' WHERE `id`={}", escape, item.id);
	}

	std::string CreateShopCacheDeleteShopQuery(DWORD dwOwner)
	{
		return fmt::format("DELETE FROM `ikashop_offlineshop` WHERE `owner` = {}", dwOwner);
	}

	std::string CreateShopCacheDeleteShopItemQuery(DWORD dwOwner)
	{
		return fmt::format("UPDATE `item` SET `ikashop_data` = '' WHERE `window` = 'IKASHOP_OFFLINESHOP' AND `owner_id` = {}", dwOwner);
	}

#ifdef ENABLE_IKASHOP_ENTITIES
	std::string CreateShopCacheInsertShopQuery(DWORD dwOwnerID, DWORD dwDuration, const char* name, const TShopSpawn& spawn)
#else
	std::string CreateShopCacheInsertShopQuery(DWORD dwOwnerID, DWORD dwDuration, const char* name)
#endif
	{
		return fmt::format("INSERT INTO `ikashop_offlineshop` (`owner`, `duration`"
#ifdef ENABLE_IKASHOP_ENTITIES
			", `map`, `x`, `y`"
#endif
			") VALUES({}, {}"
#ifdef ENABLE_IKASHOP_ENTITIES
			", {}, {}, {}"
#endif
			");",
			dwOwnerID, dwDuration
#ifdef ENABLE_IKASHOP_ENTITIES
			, spawn.map, spawn.x, spawn.y
#endif
		);
	}

	std::string CreateShopCacheUpdateDurationQuery(DWORD dwOwnerID, DWORD dwDuration)
	{
		return fmt::format("UPDATE `ikashop_offlineshop` SET `duration` = '{}' WHERE `owner` = {};", dwDuration, dwOwnerID);
	}

	std::string CreateSafeboxCacheDeleteItemQuery(DWORD dwItem)
	{
		return fmt::format("DELETE FROM `item` WHERE `id` = {}", dwItem);
	}

	std::string CreateSafeboxCacheInsertItemQuery(DWORD dwOwner, DWORD dwItemID, const TShopPlayerItem& item)
	{
		// making json value
		rapidjson::Document jsonValue;
		jsonValue.SetObject();

		// encoding data
		auto& allocator = jsonValue.GetAllocator();
		jsonHelper::insertValue(jsonValue, "expiration", static_cast<int>(item.expiration), allocator);
		
		// making json string
		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> StringWriter(buffer);
		jsonValue.Accept(StringWriter);
		
		// escaping json string
		static char escape[1024];
		CDBManager::instance().EscapeString(escape, buffer.GetString(), buffer.GetSize());
		return fmt::format("UPDATE `item` SET `ikashop_data` = '{}' WHERE `id` = {}", escape, item.id);
	}

	std::string CreateSafeboxCacheUpdateValutes(DWORD dwOwner, const TValutesInfo& val)
	{
		return fmt::format("UPDATE `ikashop_safebox` SET `gold` = {} WHERE `owner` = {};", val.yang, dwOwner);
	}

	std::string CreateSafeboxCacheUpdateValutesByAdding(DWORD dwOwner, const TValutesInfo& val)
	{
		return fmt::format("UPDATE `ikashop_safebox` SET `gold` = `gold`+{}  WHERE `owner` = {};", val.yang, dwOwner);
	}

	std::string CreateSafeboxCacheInsertSafeboxValutesQuery(DWORD dwOwnerID)
	{
		return fmt::format("INSERT INTO `ikashop_safebox` (`owner`,`gold`,`cheque`) VALUES({} , 0,0);", dwOwnerID);
	}

	std::string CreateSafeboxCacheLoadValutesQuery(DWORD dwOwnerID)
	{
		return fmt::format("SELECT `gold` from `ikashop_safebox` WHERE `owner` = {};", dwOwnerID);
	}

	std::string CreateSafeboxCacheLoadItemsQuery(DWORD dwOwnerID)
	{
		std::string query = "SELECT `id`, `vnum`, `count` ";
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
			query += fmt::format(",`socket{}` ", i);
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
			query += fmt::format(",`attrtype{}` , `attrvalue{}` ", i, i);
#ifdef ENABLE_CHANGELOOK_SYSTEM
		query += " , `trans` ";
#endif
		query += fmt::format(", `ikashop_data` FROM `item` WHERE `owner_id` = {} AND `window` = 'IKASHOP_SAFEBOX'", dwOwnerID);
		return query;
	}

#endif


