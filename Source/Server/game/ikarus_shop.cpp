#include "stdafx.h"
#include "../../common/tables.h"
#include "packet.h"
#include "item.h"
#include "char.h"
#include "item_manager.h"
#include "desc.h"
#include "char_manager.h"

#include "ikarus_shop.h"
#include "ikarus_shop_manager.h"

#ifdef ENABLE_IKASHOP_RENEWAL


namespace ikashop
{
	TItemTable* CShopItem::GetTable() const
	{
		return ITEM_MANAGER::instance().GetTable(m_info.vnum);
	}

	const TPriceInfo& CShopItem::GetPrice() const
	{
		return m_info.price;
	}

	LPITEM CShopItem::CreateItem() const
	{
		if(auto item = ITEM_MANAGER::instance().CreateItem(m_info.vnum, m_info.count, GetID()))
		{
			item->SetAttributes(m_info.aAttr);
			item->SetSockets(m_info.alSockets);
#ifdef ENABLE_CHANGELOOK_SYSTEM
			item->SetTransmutation(m_info.dwTransmutation);
#endif
			return item;
		}

		return nullptr;
	}

	const TShopItem& CShopItem::GetInfo() const
	{
		return m_info;
	}

	void CShopItem::SetInfo(const TShopItem& info)
	{
		m_info = info;
	}

	void CShopItem::SetPrice(const TPriceInfo& price)
	{
		m_info.price = price;
	}
	
	DWORD CShopItem::GetID() const
	{
		return m_info.id;
	}

	bool CShopItem::CanBuy(LPCHARACTER ch)
	{
		if(!ch)
			return false;

		if(m_info.price.yang > static_cast<long long>(ch->GetGold()))
			return false;
#ifdef ENABLE_CHEQUE_SYSTEM
		if(m_info.price.cheque > ch->GetCheque())
			return false;
#endif
		return true;
	}

	void CShopItem::SetCell(int pos)
	{
		m_info.pos = pos;
	}

	const CShop::SHOP_ITEM_MAP& CShop::GetItems() const
	{
		return m_items;
	}

	const CShop::VECGUEST& CShop::GetGuests() const
	{
		return m_guests;
	}

	void CShop::SetDuration(DWORD duration)
	{
		m_duration=duration;
	}

	void CShop::DecreaseDuration()
	{
#ifdef EXTEND_IKASHOP_ULTIMATE
		if(m_decorationTime > 0)
			m_decorationTime--;
#endif

		m_duration--;
		_RefreshViews();
	}

	DWORD CShop::GetDuration() const
	{
		return m_duration;
	}

	void CShop::SetOwnerPID(DWORD ownerid)
	{
		m_pid = ownerid;
	}

	DWORD CShop::GetOwnerPID() const
	{
		return m_pid;
	}

	bool CShop::AddGuest(LPCHARACTER ch)
	{
		// registering pid
		m_guests.emplace(ch->GetPlayerID());
		// setting up guesting
		GetManager().SendShopOpenClientPacket(ch, this);
		return true;
	}

	bool CShop::RemoveGuest(LPCHARACTER ch)
	{
		return m_guests.erase(ch->GetPlayerID()) != 0;
	}

	void CShop::KickAllGuests()
	{
		auto guests{ GetGuests() }; //copying guests intentionally
		for (auto& guest : guests) 
		{
			if (auto chGuest = CHARACTER_MANAGER::instance().FindByPID(guest)) 
			{
				if (chGuest->GetIkarusShopGuest().get() == this) 
				{
					GetManager().SendShopExpiredGuesting(GetOwnerPID(), chGuest);
					chGuest->SetIkarusShopGuest(nullptr);
				}
			}
		}
	}

	bool CShop::AddItem(const TShopItem& item)
	{
		// making item
		auto sitem = std::make_shared<CShopItem>();
		sitem->SetInfo(item);
		
		// registering item
		m_items[item.id] = sitem;
		
		// registering item into grid
		if(auto table = sitem->GetTable()){
#ifdef EXTEND_IKASHOP_ULTIMATE
			const auto page = item.pos / (GRID_SIZE);
			const auto localPos = item.pos % GRID_SIZE;
			if(page >= 0 && page < PAGE_COUNT)
				m_grid[page].RegisterItem(localPos, table->bSize);
#else
			m_grid.RegisterItem(item.pos, table->bSize);
#endif
		}
		
		// forwarding update to guests
		_RefreshViews();
		
		return true;
	}

	bool CShop::RemoveItem(DWORD itemid)
	{
		// getting item handle
		auto item = GetItem(itemid);
		if(!item)
			return false;

		// removing from shop
		m_items.erase(itemid);
		
		// removing from grid
		if(auto table = item->GetTable()){
#ifdef EXTEND_IKASHOP_ULTIMATE
			const auto page = item->GetInfo().pos / (GRID_SIZE);
			const auto localPos = item->GetInfo().pos % GRID_SIZE;
			if (page >= 0 && page < PAGE_COUNT)
				m_grid[page].UnregisterItem(localPos, table->bSize);
#else
			m_grid.UnregisterItem(item->GetInfo().pos, table->bSize);
#endif
		}
		
		// refreshing to guests
		TPacketGCNewIkarusShop pack{};
		pack.header = HEADER_GC_NEW_OFFLINESHOP;
		pack.size = sizeof(pack) + sizeof(TSubPacketGCShopRemoveItem);
		pack.subheader = SUBHEADER_GC_SHOP_REMOVE_ITEM_GUEST;

		TSubPacketGCShopRemoveItem subpack{};
		subpack.itemid = itemid;

		for (auto& guest : m_guests)
		{
			if (auto ch = CHARACTER_MANAGER::Instance().FindByPID(guest))
			{
				if (ch->GetDesc())
				{
					ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
					ch->GetDesc()->Packet(&subpack, sizeof(subpack));
				}
			}
		}

		// refreshing to owner
		pack.subheader = SUBHEADER_GC_SHOP_REMOVE_ITEM_OWNER;
		if(auto ch = FindOwnerCharacter())
		{
			if(ch->GetDesc())
			{
				ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
				ch->GetDesc()->Packet(&subpack, sizeof(subpack));
			}
		}
		
		return true;
	}

	bool CShop::ModifyItemPrice(DWORD itemid, const TPriceInfo& price)
	{
		// getting item
		auto shopItem = GetItem(itemid);
		if(!shopItem)
			return false;

		// editing price and forwarding to guests
		shopItem->SetPrice(price);
		
		// refreshing guests
		TPacketGCNewIkarusShop pack{};
		pack.header = HEADER_GC_NEW_OFFLINESHOP;
		pack.subheader = SUBHEADER_GC_SHOP_EDIT_ITEM_GUEST;
		pack.size = sizeof(pack) + sizeof(TSubPacketGCShopEditItem);

		TSubPacketGCShopEditItem subpack{};
		subpack.itemid = itemid;
		subpack.price = price;

		for (auto& guest : m_guests)
		{
			if (auto ch = CHARACTER_MANAGER::Instance().FindByPID(guest))
			{
				if (ch->GetDesc())
				{
					ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
					ch->GetDesc()->Packet(&subpack, sizeof(subpack));
				}
			}
		}

		// refreshing owner
		pack.subheader = SUBHEADER_GC_SHOP_EDIT_ITEM_OWNER;
		if (auto ch = FindOwnerCharacter())
		{
			if (ch->GetDesc())
			{
				ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
				ch->GetDesc()->Packet(&subpack, sizeof(subpack));
			}
		}

		return true;
	}

	bool CShop::BuyItem(DWORD itemid)
	{
		return RemoveItem(itemid);
	}

	CShop::ITEM_HANDLE CShop::GetItem(DWORD itemid)
	{
		auto it = m_items.find(itemid);
		return it != m_items.end() ? it->second : nullptr;
	}

	void CShop::_RefreshViews(LPCHARACTER ch)
	{
		// applying to each guest
		for (auto& guest : m_guests)
			if (auto ch = CHARACTER_MANAGER::Instance().FindByPID(guest))
				GetManager().SendShopOpenClientPacket(ch, this);

		// applying to owner
		RefreshToOwner();
	}

	void CShop::Clear()
	{
		m_items.clear();
		m_guests.clear();
#ifdef EXTEND_IKASHOP_ULTIMATE
		for(auto& grid : m_grid)
			grid.Clear();
#else
		m_grid.Clear();
#endif
	}

	LPCHARACTER CShop::FindOwnerCharacter()
	{
		return CHARACTER_MANAGER::instance().FindByPID(GetOwnerPID());
	}

#ifdef EXTEND_IKASHOP_ULTIMATE
	void CShop::_RefreshGrid()
	{
		// resetting grids
		for (auto& grid : m_grid)
			grid.Clear();

		// filling grid with items
		for (auto& [id, item] : m_items)
			if (auto table = item->GetTable())
				ReserveSpace(item->GetInfo().pos, table->bSize);

		// locking by lock index
		for (int cell = GetLockIndex(); cell < GRID_SIZE * PAGE_COUNT; cell++)
			ReserveSpace(cell, 1);
	}
#endif

	const char* CShop::GetName() const
	{
		return m_name.c_str();
	}

	bool CShop::ReserveSpace(int cell, int size)
	{
#ifdef EXTEND_IKASHOP_ULTIMATE
		const auto page = cell / GRID_SIZE;
		const auto localPos = cell % GRID_SIZE;
		if(page < 0 || page >= PAGE_COUNT)
			return false;

		if (!m_grid[page].CheckSpace(localPos, size))
			return false;

		m_grid[page].RegisterItem(localPos, size);
#else
		if(!m_grid.CheckSpace(cell, size))
			return false;
		m_grid.RegisterItem(cell, size);
#endif
		return true;
	}

#ifdef EXTEND_IKASHOP_ULTIMATE
	void CShop::MoveItem(DWORD itemid, int destCell)
	{
		if(auto item = GetItem(itemid))
		{
			if(auto table = item->GetTable())
			{
				// freeing old space
				const auto page = item->GetInfo().pos / (GRID_SIZE);
				const auto localPos = item->GetInfo().pos % GRID_SIZE;
				if (page >= 0 && page < PAGE_COUNT)
					m_grid[page].UnregisterItem(localPos, table->bSize);

				// locking new space
				ReserveSpace(destCell, table->bSize);
			}

			// updating item pos
			item->SetCell(destCell);

			// refreshing views
			_RefreshViews();
		}
	}

	void CShop::ChangeDuration(DWORD duration)
	{
		SetDuration(duration);
		_RefreshViews();
	}


	void CShop::SetDecoration(int decoration)
	{
		m_decoration = decoration;
	}

	void CShop::SetDecorationDuration(int decoration_time)
	{
		m_decorationTime = decoration_time;
	}

	int CShop::GetDecoration() const
	{
		return m_decoration;
	}

	int CShop::GetDecorationTime() const
	{
		return m_decorationTime;
	}

	void CShop::SetLockIndex(int lockIndex)
	{
		m_lockIndex = lockIndex;
		_RefreshGrid();
	}

	int CShop::GetLockIndex() const
	{
		return m_lockIndex;
	}

	void CShop::ChangeLockIndex(int lockIndex)
	{
		if (GetLockIndex() < lockIndex)
			SetLockIndex(lockIndex);
		_RefreshViews();
	}
#endif

#ifdef ENABLE_IKASHOP_ENTITIES
	void CShop::SetEntity(ENTITY_HANDLE handle)
	{
		m_entity = handle;
	}

	CShop::ENTITY_HANDLE CShop::GetEntity() const
	{
		return m_entity;
	}

	void CShop::SetSpawn(const TShopSpawn& spawn)
	{
		m_spawn = spawn;
	}

	const TShopSpawn& CShop::GetSpawn() const
	{
		return m_spawn;
	}
#endif

	void CShop::SetName(const SHOPNAME& name)
	{
		m_name = name;
	}

	void CShop::RefreshToOwner()
	{
		if(auto ch = FindOwnerCharacter())
			if(ch->IsLookingShopOwner())
				GetManager().SendShopOpenMyShopClientPacket(ch);
	}

#ifdef ENABLE_IKASHOP_ENTITIES
	ShopEntity::ShopEntity()
	{
		CEntity::Initialize(ENTITY_NEWSHOPS);
	}

	//NEW ENTITY
	void ShopEntity::EncodeInsertPacket(LPENTITY entity)
	{
		// checking enitity is a character
		if(entity->GetType() != ENTITY_CHARACTER)
			return;

		// checking character is a player
		auto ch = static_cast<LPCHARACTER>(entity);
		if(!ch->IsPC())
			return;

		// checking shop duration
		if(GetShop() && GetShop()->GetDuration() == 0)
			return;

		GetManager().EncodeInsertShopEntity(*this, ch);
	}

	void ShopEntity::EncodeRemovePacket(LPENTITY entity)
	{
		// checking enitity is a character
		if (entity->GetType() != ENTITY_CHARACTER)
			return;

		// checking character is a player
		auto ch = static_cast<LPCHARACTER>(entity);
		if (!ch->IsPC())
			return;

		GetManager().EncodeRemoveShopEntity(*this, ch);
	}

	const char* ShopEntity::GetShopName() const
	{
		return m_name.c_str();
	}

	void ShopEntity::SetShopName(const char* name)
	{
		m_name = name;
	}

	int ShopEntity::GetShopType() const
	{
		return m_type;
	}

	void ShopEntity::SetShopType(int iType)
	{
		m_type = iType;
	}

	void ShopEntity::SetShop(SHOP_HANDLE shop)
	{
		m_shop = shop;
	}
#endif
}



#endif






