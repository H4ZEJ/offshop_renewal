//search
		void __BettingGuildWar_Initialize();
		void __BettingGuildWar_SetObserverCount(UINT uObserverCount);
		void __BettingGuildWar_SetBettingMoney(UINT uBettingMoney);


//add after
#ifdef ENABLE_IKASHOP_RENEWAL
	public:
		bool RecvOfflineshopPacket();
		bool RecvOfflineshopShopOpen();
		bool RecvOfflineshopShopOpenOwner();
		bool RecvOfflineshopShopOpenOwnerNoShop();
		bool RecvOfflineshopShopExpiredGuesting();
		bool RecvOfflineshopShopRemoveItem(bool owner);
		bool RecvOfflineshopShopGuestEditItem();
		bool RecvOfflineshopShopOwnerEditItem();
		bool RecvOfflineshopShopFilterResult();
		bool RecvOfflineshopShopSafeboxRefresh();
		bool RecvOfflineshopShopSafeboxRemoveItem();
		bool RecvOfflineshopShopSafeboxAddItem();
		bool RecvOfflineshopPopupMessage();
		bool RecvOfflineshopBoardCounters();
#ifdef EXTEND_IKASHOP_PRO
		bool RecvOfflineshopNotificationList();
#endif
#ifdef EXTEND_IKASHOP_ULTIMATE
		bool RecvOfflineshopPriceAverageResponse();
#endif
#ifdef ENABLE_IKASHOP_ENTITIES
		bool RecvOfflineshopInsertEntity();
		bool RecvOfflineshopRemoveEntity();
		void SendIkarusShopOnClickShopEntity(DWORD dwPickedShopVID);
#ifdef EXTEND_IKASHOP_PRO
		void SendIkarusShopMoveShopEntity();
#endif
		bool RecvOfflineshopSearchResultDelete();
#endif
		void SendIkarusShopCreateShop();
		void SendIkarusShopForceCloseShop();
		void SendIkarusShopRequestShopList();
		void SendIkarusShopOpenShop(DWORD dwOwnerID);
		void SendIkarusShopOpenShopOwner();
		void SendIkarusShopBuyItem(DWORD dwOwnerID, DWORD dwItemID, bool isSearch, long long TotalPriceSeen);
		void SendIkarusShopAddItem(ikashop::TShopItemInfo& itemInfo);
		void SendIkarusShopRemoveItem(DWORD dwItemID);
		void SendOfflineShopEditItem(DWORD dwItemID, const ikashop::TPriceInfo& price);
		void SendIkarusShopFilterRequest(const ikashop::TFilterInfo& filter);
		void SendIkarusShopSearchFillRequest();
		void SendIkarusShopSafeboxOpen();
		void SendIkarusShopSafeboxGetItem(DWORD dwItemID);
		void SendIkarusShopSafeboxGetValutes();
		void SendIkarusShopSafeboxClose();
		void SendIkarusMyShopCloseBoard();
		void SendIkarusCloseShopGuestBoard();
#ifdef EXTEND_IKASHOP_PRO
		void SendIkarusNotificationListRequest();
		void SendIkarusNotificationListClose();
#endif
#ifdef EXTEND_IKASHOP_ULTIMATE
		void SendPriceAverageRequest(DWORD vnum, DWORD count);
		void SendShopDecorationUsage(int index);
		void SendShopMoveItem(int srcPos, int destPos);
#endif
#endif