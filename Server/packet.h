//search
	HEADER_CG_ITEM_GIVE				= 83,

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
	HEADER_CG_NEW_OFFLINESHOP		= 84,
#endif

//search
	HEADER_GC_MAIN_CHARACTER3_BGM				= 137,
	HEADER_GC_MAIN_CHARACTER4_BGM_VOL			= 138,

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
	HEADER_GC_NEW_OFFLINESHOP					= 139,
#endif

//add at the end of file before @@@ #pragma pack()

#ifdef ENABLE_IKASHOP_RENEWAL
//ACTIONS PACKETS
struct TPacketGCNewIkarusShop
{
	BYTE header;
#ifdef ENABLE_LARGE_DYNAMIC_PACKETS
	int size;
#else
	WORD size;
#endif
	BYTE subheader;
};

struct TPacketCGNewIkarusShop {
	BYTE header;
	WORD size;
	BYTE subheader;
};

namespace ikashop
{
	struct TFilterInfo 
	{
		BYTE type;
		BYTE subtype;
		char name[ITEM_NAME_MAX_LEN];
		TPriceInfo pricestart;
		TPriceInfo pricend;
		int levelstart;
		int levelend;
		TPlayerItemAttribute attrs[ITEM_ATTRIBUTE_NORM_NUM];
		int sashGrade;
		int alchemyGrade;
	};

	struct TShopItemInfo 
	{
		TItemPos pos;
		TPriceInfo price;
	};

#ifdef EXTEND_IKASHOP_ULTIMATE
	struct TShopItemView : public ikashop::TShopItem
	{
		long long priceavg;
	};
#endif

#ifdef EXTEND_IKASHOP_ULTIMATE
	struct TSearchResultInfo : public ikashop::TShopItemView
#else
	struct TSearchResultInfo : public ikashop::TShopItem 
#endif
	{
		char seller_name[OFFLINE_SHOP_NAME_MAX_LEN];
		int duration;
	};

	enum eSubHeaderGC
	{
		SUBHEADER_GC_SHOP_OPEN,
		SUBHEADER_GC_SHOP_OPEN_OWNER,
		SUBHEADER_GC_SHOP_OPEN_OWNER_NO_SHOP,
		SUBHEADER_GC_SHOP_EXPIRED_GUESTING,
		SUBHEADER_GC_SHOP_REMOVE_ITEM_OWNER,
		SUBHEADER_GC_SHOP_REMOVE_ITEM_GUEST,
		SUBHEADER_GC_SHOP_EDIT_ITEM_OWNER,
		SUBHEADER_GC_SHOP_EDIT_ITEM_GUEST,
		SUBHEADER_GC_SHOP_FILTER_RESULT,
		SUBHEADER_GC_SHOP_SAFEBOX_REFRESH,
		SUBHEADER_GC_SHOP_SAFEBOX_REMOVE_ITEM,
		SUBHEADER_GC_SHOP_SAFEBOX_ADD_ITEM,
		SUBHEADER_GC_POPUP_MESSAGE,

#ifdef ENABLE_IKASHOP_ENTITIES
		SUBHEADER_GC_INSERT_SHOP_ENTITY,
		SUBHEADER_GC_REMOVE_SHOP_ENTITY,
#endif
		SUBHEADER_GC_BOARD_COUNTERS,
#ifdef EXTEND_IKASHOP_PRO
		SUBHEADER_GC_NOTIFICATION_LIST,
#endif
#ifdef EXTEND_IKASHOP_ULTIMATE
		SUBHEADER_GC_PRICE_AVERAGE_RESPONSE,
#endif
		SUBHEADER_GC_SEARCH_RESULT_DELETE,
	};

	struct TSubPacketGCShopOpen 
	{
		TShopInfo shop{};
	};

	struct TSubPacketGCShopOpenOwner 
	{
		TShopInfo shop{};
	};

	struct TSubPacketGCShopExpiredGuesting 
	{
		DWORD ownerid;
	};

	struct TSubPacketGCShopRemoveItem
	{
		DWORD itemid;
	};

	struct TSubPacketGCShopEditItem
	{
		DWORD itemid;
		TPriceInfo price;
	};

	struct TSubPacketGCShopFilterResult 
	{
		DWORD count;
	};

	struct TSubPacketGCShopSafeboxRefresh 
	{
		TValutesInfo valute;
		DWORD itemcount;
	};

	struct TSubPacketGCShopSafeboxRemoveItem
	{
		DWORD itemid;
	};

	struct TSubPacketGCShoppingSafeboxAddItem
	{
		TShopPlayerItem item;
	};

	struct TSubPacketGCPopupMessage
	{
		static constexpr inline auto MESSAGE_LEN = 80;
		char localeString[MESSAGE_LEN];
	};

#ifdef ENABLE_IKASHOP_ENTITIES
	struct TSubPacketGCInsertShopEntity 
	{
		DWORD vid;
		char name[OFFLINE_SHOP_NAME_MAX_LEN];
		int type;
		int x;
		int y;
		int z;
	};

	struct TSubPacketGCRemoveShopEntity 
	{
		DWORD vid;
	};
#endif

	struct TSubPacketGCBoardCounters
	{
		int safebox;
#ifdef EXTEND_IKASHOP_PRO
		int notification;
#endif
	};

#ifdef EXTEND_IKASHOP_PRO
	struct TSubPacketGCNotificationList
	{
		DWORD count;
	};
#endif

	enum eSubHeaderCG
	{
		SUBHEADER_CG_SHOP_CREATE_NEW,
		SUBHEADER_CG_SHOP_FORCE_CLOSE,
		SUBHEADER_CG_SHOP_REQUEST_SHOPLIST,
		SUBHEADER_CG_SHOP_OPEN,
		SUBHEADER_CG_SHOP_OPEN_OWNER,
		SUBHEADER_CG_SHOP_BUY_ITEM,
		SUBHEADER_CG_SHOP_ADD_ITEM,
		SUBHEADER_CG_SHOP_REMOVE_ITEM,
		SUBHEADER_CG_SHOP_EDIT_ITEM,
		SUBHEADER_CG_SHOP_FILTER_REQUEST,
		SUBHEADER_CG_SHOP_SEARCH_FILL_REQUEST,
		SUBHEADER_CG_SHOP_SAFEBOX_OPEN,
		SUBHEADER_CG_SHOP_SAFEBOX_GET_ITEM,
		SUBHEADER_CG_SHOP_SAFEBOX_GET_VALUTES,
		SUBHEADER_CG_SHOP_SAFEBOX_CLOSE,
		SUBHEADER_CG_CLOSE_MY_SHOP_BOARD,
		SUBHEADER_CG_CLOSE_SHOP_GUEST,
#ifdef ENABLE_IKASHOP_ENTITIES
		SUBHEADER_CG_CLICK_ENTITY,
#ifdef ENABLE_IKASHOP_ENTITIES
		SUBHEADER_CG_MOVE_SHOP_ENTITY,
#endif
#endif
#ifdef EXTEND_IKASHOP_PRO
		SUBHEADER_CG_NOTIFICATION_LIST_REQUEST,
		SUBHEADER_CG_NOTIFICATION_LIST_CLOSE,
#endif
#ifdef EXTEND_IKASHOP_ULTIMATE
		SUBHEADER_CG_PRICE_AVERAGE_REQUEST,
		SUBHEADER_CG_SHOP_DECORATION_USE,
		SUBHEADER_CG_SHOP_MOVE_ITEM,
#endif
	};

	struct TSubPacketCGShopOpen 
	{
		DWORD ownerid;
	};

	struct TSubPacketCGAddItem 
	{
		TItemPos pos;
		TPriceInfo price;
		int destpos;
	};

	struct TSubPacketCGRemoveItem 
	{
		DWORD itemid;
	};

	struct TSubPacketCGEditItem	
	{
		DWORD itemid;
		TPriceInfo price;
	};

	struct TSubPacketCGFilterRequest 
	{
		TFilterInfo filter;
	};

	struct TSubPacketCGShopSafeboxGetItem 
	{
		DWORD itemid;
	};

	struct TSubPacketCGShopBuyItem 
	{
		DWORD ownerid;
		DWORD itemid;
		bool searching;
		long long seenprice;
	};

#ifdef ENABLE_IKASHOP_ENTITIES
	struct TSubPacketCGShopClickEntity 
	{
		DWORD vid;
	};
#endif

#ifdef EXTEND_IKASHOP_ULTIMATE
	struct TSubPacketCGPriceAverageRequest
	{
		DWORD vnum;
		DWORD count;
	};

	struct TSubPacketCGShopMoveItem
	{
		int srcPos;
		int destPos;
	};
#endif
}
#endif
