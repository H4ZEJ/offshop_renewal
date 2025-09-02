#ifndef __INCLUDE_NEW_OFFLINESHOP_HEADER__
#define __INCLUDE_NEW_OFFLINESHOP_HEADER__

#ifdef ENABLE_IKASHOP_RENEWAL
#define __USE_PID_AS_GUESTLIST__
#include <ranges>
#include "item.h"

//copyarray
template <class T, size_t size>
void CopyArray(T (&objDest)[size] , const T (&objSrc)[size]){
	if(size==0)
		return;
	memcpy(&objDest[0] , &objSrc[0], sizeof(T)*size);
}

template <class T>
void DeletePointersContainer(T& obj){
	typename T::iterator it = obj.begin();
	for(; it != obj.end(); it++)
		delete(*it);
}

namespace ikashop
{
	//patch 08-03-2020
	inline ikashop::ExpirationType GetItemExpiration(LPITEM item) {
		auto proto = item->GetProto();
		for (const auto limit : proto->aLimits) {
			if (limit.bType == LIMIT_REAL_TIME)
				return ikashop::ExpirationType::EXPIRE_REAL_TIME;
			else if (limit.bType == LIMIT_REAL_TIME_START_FIRST_USE && item->GetSocket(1) != 0)
				return ikashop::ExpirationType::EXPIRE_REAL_TIME_FIRST_USE;
		} return ikashop::ExpirationType::EXPIRE_NONE;
	}


	enum eOffshopChatPacket
	{
		CHAT_PACKET_CANNOT_CREATE_SHOP,
		CHAT_PACKET_CANNOT_CHANGE_NAME,
		CHAT_PACKET_CANNOT_FORCE_CLOSE,
		CHAT_PACKET_CANNOT_OPEN_SHOP,
		CHAT_PACKET_CANNOT_OPEN_SHOP_OWNER,

		CHAT_PACKET_CANNOT_ADD_ITEM,
		CHAT_PACKET_CANNOT_BUY_ITEM, //tofix wrong chat packet
		CHAT_PACKET_CANNOT_REMOVE_ITEM,
		CHAT_PACKET_CANNOT_EDIT_ITEM,
		CHAT_PACKET_CANNOT_REMOVE_LAST_ITEM,

		CHAT_PACKET_CANNOT_FILTER,
		CHAT_PACKET_CANNOT_SEARCH_YET,

		CHAT_PACKET_CANNOT_OPEN_SAFEBOX,
		CHAT_PACKET_CANNOT_SAFEBOX_GET_ITEM,
		CHAT_PACKET_CANNOT_SAFEBOX_GET_VALUTES,
		CHAT_PACKET_CANNOT_SAFEBOX_CLOSE,


		CHAT_PACKET_RECV_ITEM_SAFEBOX,

		//GENERAL
		CHAT_PACKET_CANNOT_DO_NOW,

	};


	class CShopItem
	{
		using ItemID = DWORD;
		using OwnerID = DWORD;

	public:
		TItemTable* GetTable() const;
		LPITEM CreateItem() const;

		const TPriceInfo& GetPrice() const;
		void SetPrice(const TPriceInfo& price);
		
		const TShopItem& GetInfo() const;
		void SetInfo(const TShopItem& info);

		ItemID GetID() const;
		bool CanBuy(LPCHARACTER ch);

		void SetCell(int pos);

	protected:
		TShopItem m_info{};

	};

	template<int WIDTH, int HEIGHT>
	class CShopGrid
	{
		static constexpr auto CELL_COUNT = WIDTH*HEIGHT;

	public:
		void Clear()
		{
			m_cells = {};
		}

		void RegisterItem(int cell, int size)
		{
			const auto [col, row] = _DecomposeCell(cell);
			for(int i=0; i < size; i++)
				_SetCellState(_ComposeCell(col, row+i), true);
		}

		void UnregisterItem(int cell, int size)
		{
			const auto [col, row] = _DecomposeCell(cell);
			for (int i = 0; i < size; i++)
				_SetCellState(_ComposeCell(col, row + i), false);
		}

		bool CheckSpace(int cell, int size)
		{
			const auto [col, row] = _DecomposeCell(cell);
			for(int nrow = row; nrow < row + size; nrow++)
				if(_GetCellState(_ComposeCell(col, nrow)))
					return false;
			return true;
		}

		std::optional<int> FindSpace(int size)
		{
			for(int cell = 0; cell < CELL_COUNT; cell++)
				if(CheckSpace(cell, size))
					return cell;
			return std::nullopt;
		}

		constexpr int GetCellCount(){
			return WIDTH*HEIGHT;
		}

	private:
		std::pair<int,int> _DecomposeCell(int cell){ return { cell % WIDTH, cell / WIDTH }; }
		int _ComposeCell(int col, int row){ return row * WIDTH + col; }
		void _SetCellState(int cell, bool state){ if(cell >= 0 && cell < CELL_COUNT) m_cells[cell] = state; }
		bool _GetCellState(int cell) { return cell >= 0 && cell < CELL_COUNT ? m_cells[cell] : true; }

	private:
		std::array<bool, WIDTH*HEIGHT> m_cells{};
	};

	class ShopEntity;

	class CShop
	{
	public:
		static constexpr auto GRID_WIDTH = 15;
		static constexpr auto GRID_HEIGHT = 10;
#ifdef EXTEND_IKASHOP_ULTIMATE
		static constexpr auto PAGE_COUNT = 3;
		static constexpr auto GRID_SIZE = GRID_WIDTH*GRID_HEIGHT;
#endif

	public:
		using ITEM_HANDLE = std::shared_ptr<CShopItem>;

		using SHOP_ITEM_MAP = std::map<DWORD, ITEM_HANDLE>;

		using VECGUEST = std::set<DWORD>;
		using SHOPNAME = std::string;
#ifdef ENABLE_IKASHOP_ENTITIES
		using ENTITY_HANDLE = std::shared_ptr<ShopEntity>;
#endif
	public:
		// get const 
		const SHOP_ITEM_MAP& GetItems() const;
		const VECGUEST& GetGuests() const;

		// duration
		void SetDuration(DWORD duration);
		void DecreaseDuration();
		DWORD GetDuration() const;

		// owner pid
		void SetOwnerPID(DWORD ownerid);
		DWORD GetOwnerPID() const;

		// guests
		bool AddGuest(LPCHARACTER ch);
		bool RemoveGuest(LPCHARACTER ch);
		void KickAllGuests();

		// items
		bool AddItem(const TShopItem& rItem);
		bool RemoveItem(DWORD itemid);
		bool ModifyItemPrice(DWORD itemid, const TPriceInfo& price);
		bool BuyItem(DWORD itemid);
		ITEM_HANDLE GetItem(DWORD itemid);

		// helpers
		LPCHARACTER FindOwnerCharacter();
		void Clear();
		void RefreshToOwner();
		void SetName(const SHOPNAME& name);
		const char* GetName() const;

		// grid checks
		bool ReserveSpace(int cell, int size);

#ifdef EXTEND_IKASHOP_ULTIMATE
		void MoveItem(DWORD itemid, int destCell);
		void ChangeDuration(DWORD duration);
		
		void SetDecoration(int decoration);
		void SetDecorationDuration(int decoration_time);
		int GetDecoration() const;
		int GetDecorationTime() const;
		
		void SetLockIndex(int lockIndex);
		int GetLockIndex() const;
		void ChangeLockIndex(int lockIndex);
#endif

		void SetEntity(ENTITY_HANDLE handle);
		ENTITY_HANDLE GetEntity() const;

#ifdef ENABLE_IKASHOP_ENTITIES
		void SetSpawn(const TShopSpawn& spawn);
		const TShopSpawn& GetSpawn() const;
#endif

	private:
		void _RefreshViews(LPCHARACTER ch = nullptr);
#ifdef EXTEND_IKASHOP_ULTIMATE
		void _RefreshGrid();
#endif

	private:
		ENTITY_HANDLE m_entity;
		SHOP_ITEM_MAP m_items{};
		VECGUEST m_guests{};
		DWORD m_pid{};
		DWORD m_duration{};
		SHOPNAME m_name{};
#ifdef EXTEND_IKASHOP_ULTIMATE
		CShopGrid<GRID_WIDTH, GRID_HEIGHT> m_grid[PAGE_COUNT];
#else
		CShopGrid<GRID_WIDTH, GRID_HEIGHT> m_grid;
#endif
#ifdef ENABLE_IKASHOP_ENTITIES
		TShopSpawn m_spawn;
#endif
#ifdef EXTEND_IKASHOP_ULTIMATE
		int m_decoration = 0;
		int m_decorationTime = 0;
		int m_lockIndex = 0;
#endif
	};


	class CShopSafebox
	{
	public:
		static constexpr auto GRID_WIDTH = 9;
		static constexpr auto GRID_HEIGHT = 11;

	public:
		using ITEM_HANDLE = std::shared_ptr<CShopItem>;
		using ITEM_MAP = std::map<DWORD, ITEM_HANDLE>;

	public:
		CShopSafebox() = default;
		CShopSafebox(const CShopSafebox&) = default;
		CShopSafebox(LPCHARACTER chOwner);

		void SetOwner(LPCHARACTER ch);
		void SetValuteAmount(TValutesInfo val);
			 
		bool AddItem(const TShopPlayerItem& item);
		bool RemoveItem(DWORD itemid);
			 
		void AddValute(TValutesInfo val);
		bool RemoveValute(TValutesInfo val);

		const ITEM_MAP& GetItems() const;
		const TValutesInfo& GetValutes() const;

		ITEM_HANDLE GetItem(DWORD itemid) const;
		LPCHARACTER GetOwner() const;

		bool RefreshToOwner(LPCHARACTER ch = nullptr);

	private:
		int _RegisterGridPosition(ITEM_HANDLE item);
		void _UnregisterGridPosition(ITEM_HANDLE item);


	private:
		ITEM_MAP m_items{};
		LPCHARACTER m_owner{};
		TValutesInfo m_valutes{};
		std::vector<CShopGrid<GRID_WIDTH, GRID_HEIGHT>> m_grids;
	};

#ifdef ENABLE_IKASHOP_ENTITIES
	class ShopEntity : public CEntity
	{
	private:
		static DWORD AllocID() {
			static auto id = 0UL;
			return ++id;
		}

	public:
		using SHOP_HANDLE = std::shared_ptr<CShop>;

	public:
		ShopEntity();

		//overriding virtual CEntity methods
		void EncodeInsertPacket(LPENTITY entity);
		void EncodeRemovePacket(LPENTITY entity);
		
		const char* GetShopName() const;
		void SetShopName(const char* name);

		int GetShopType() const;
		void SetShopType(int iType);
		void SetShop(SHOP_HANDLE shop);

		SHOP_HANDLE GetShop() {return m_shop;}
		void Destroy() { CEntity::Destroy(); }

		DWORD GetVID() const { return m_vid; }

	private:
		std::string m_name{};
		int m_type{};
		SHOP_HANDLE m_shop{};
		DWORD m_vid = AllocID();
	};
#endif
}

#endif

#endif //__include
