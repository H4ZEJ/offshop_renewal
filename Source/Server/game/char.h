//search
class CBuffOnAttributes;
class CPetSystem;

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
namespace ikashop
{
	class CShop;
	class CShopSafebox;
}
#endif

//search
};

ESex GET_SEX(LPCHARACTER ch);

//add before @@@@ };
#ifdef ENABLE_IKASHOP_RENEWAL
	public:
		using SHOP_HANDLE = std::shared_ptr<ikashop::CShop>;
		using SAFEBOX_HANDLE = std::shared_ptr<ikashop::CShopSafebox>;
		SHOP_HANDLE GetIkarusShop() {return m_pkIkarusShop;}
		void SetIkarusShop(SHOP_HANDLE pkShop) {m_pkIkarusShop = pkShop;}
		SHOP_HANDLE	GetIkarusShopGuest() const {return m_pkIkarusShopGuest;}
		void SetIkarusShopGuest(SHOP_HANDLE pkShop);
		SAFEBOX_HANDLE GetIkarusShopSafebox() {return m_pkIkarusShopSafebox;}
		void SetIkarusShopSafebox(SAFEBOX_HANDLE pk);
		int GetIkarusShopUseTime() const { return m_iIkarusShopUseTime; }
		void SetIkarusShopUseTime() { m_iIkarusShopUseTime = thecore_pulse(); }
		void SetLookingShopOwner(bool state) { m_bIsLookingShopOwner  = state;}
		bool IsLookingShopOwner() const { return m_bIsLookingShopOwner; }
#ifdef EXTEND_IKASHOP_PRO
		void SetLookingNotificationList(bool state) { m_bIsLookingNotificationList = state; }
		bool IsLookingNotificationList() const { return m_bIsLookingNotificationList; }
#endif
		bool CanTakeInventoryItem(LPITEM item, TItemPos* pos);
		bool IkarusShopFloodCheck(ShopActionWeight weight);

	private:
		SHOP_HANDLE	m_pkIkarusShop;
		SHOP_HANDLE	m_pkIkarusShopGuest;
		SAFEBOX_HANDLE m_pkIkarusShopSafebox;

		int m_iIkarusShopUseTime = 0;
		bool m_bIsLookingShopOwner = false;
#ifdef EXTEND_IKASHOP_PRO
		bool m_bIsLookingNotificationList = false;
#endif
#endif