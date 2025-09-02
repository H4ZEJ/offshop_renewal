//add where you want
#ifdef ENABLE_IKASHOP_RENEWAL
#include <rapidjson/rapidjson.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <fmt/fmt.h>
#include "../../common/json_helper.h"
#include "IkarusShopCache.h"
#endif

#ifdef ENABLE_IKASHOP_RENEWAL
inline std::optional<rapidjson::Document> str_to_json(const char* str) {
	if (!str || str[0] == 0)
		return std::nullopt;

	rapidjson::Document ret;
	ret.Parse(str);

	if (ret.HasParseError()) {
		sys_err("Json Document has parse errors : %d (str %s)", (int)ret.GetParseError(), str);
		return std::nullopt;
	}

	return ret;
}
#endif

//search
	TItemIDRangeTable GetItemRange() { return m_itemRange; }

//after add
#ifdef ENABLE_IKASHOP_RENEWAL
public:
	void SendIkarusShopTable(CPeer* peer);
	bool InitializeIkarusShopTable();
	bool InitializeIkarusShopShopTable();
	bool InitializeIkarusShopShopItemsTable();
	void RecvIkarusShopPacket(CPeer* peer, const char* data);
	bool RecvIkarusShopBuyItemPacket(const char* data);
	bool RecvIkarusShopLockBuyItem(CPeer* peer, const char* data);
	bool RecvIkarusShopEditItemPacket(const char* data);
	bool RecvIkarusShopRemoveItemPacket(CPeer* peer, const char* data);
	bool RecvIkarusShopAddItemPacket(const char* data);
	bool RecvIkarusShopForceClose(CPeer* peer, const char* data);
	bool RecvIkarusShopCreateNew(const char* data);
	bool RecvIkarusShopChangeName(const char* data);
	bool RecvIkarusShopSafeboxGetItem(CPeer* peer, const char* data);
	bool RecvIkarusShopSafeboxGetValutes(const char* data);
	bool RecvIkarusShopSafeboxAddItem(const char* data);
	bool RecvIkarusShopSafeboxGetItemFeedback(const char* data);

#ifdef EXTEND_IKASHOP_PRO
	bool RecvIkarusShopNotificationSeen(const char* data);
	bool RecvIkarusShopRestoreDuration(const char* data);
#ifdef ENABLE_IKASHOP_ENTITIES
	bool RecvIkarusShopMoveShopEntity(const char* data);
#endif
#endif

#ifdef EXTEND_IKASHOP_ULTIMATE
	bool RecvIkarusShopMoveItem(const char* data);
	bool RecvIkarusShopDecorationUse(const char* data);
	bool RecvIkarusShopUnlockCell(const char* data);
	void SendIkarusShopDecorationSet(DWORD owner, int index, int remainTime);
#endif

	bool SendIkarusShopBuyItemPacket(DWORD dwOwner, DWORD dwGuest, DWORD dwItem);
	bool SendIkarusShopBuyLockedItemPacket(CPeer* peer,DWORD dwOwner, DWORD dwGuest, DWORD dwItem);
	bool SendIkarusShopEditItemPacket(DWORD dwOwner, DWORD dwItem, const ikashop::TPriceInfo& price);
	bool SendIkarusShopRemoveItemPacket(DWORD dwOwner, DWORD dwItem, CPeer* requester = nullptr);
	bool SendIkarusShopAddItemPacket(DWORD dwOwner, DWORD dwItemID, const ikashop::TShopItem& rInfo);
	bool SendIkarusShopForceClose(DWORD dwOwnerID);
	bool SendIkarusShopCreateNew(const ikashop::TShopInfo& shop);
	bool SendIkarusShopChangeName(DWORD dwOwnerID, const char* szName);
	bool SendIkarusShopShopExpired(DWORD dwOwnerID);
	void SendIkarusShopSafeboxAddItem(DWORD dwOwnerID, DWORD dwItem, const ikashop::TShopPlayerItem& item);
	void SendIkarusShopSafeboxAddValutes(DWORD dwOwnerID, const ikashop::TValutesInfo& valute);
	void SendIkarusShopSafeboxExpiredItem(DWORD dwOwnerID, DWORD itemID);
	void SendIkarusShopSafeboxGetItemConfirm(CPeer* peer, DWORD dwOwnerID, DWORD itemID);
	void IkarusShopResultQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
	void IkarusShopResultAddItemQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
	void IkarusShopResultCreateShopQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
	void IkarusShopResultSafeboxAddItemQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);


	void IkarusShopDurationProcess();
	void IkarusShopExpiredShop(DWORD dwID
#ifdef EXTEND_IKASHOP_ULTIMATE
		, bool requested = false
#endif
	);
	void IkarusShopLoadShopSafebox(CPeer* peer ,DWORD dwID);
#ifdef EXTEND_IKASHOP_PRO
	void IkarusShopLoadNotifications(CPeer* peer, DWORD owner);
	void AppendIkarusShopNotification(ikashop::ENotificationType type, DWORD owner, DWORD what, const std::string& who, const std::string& format);
#endif
	bool IsUsingIkarusShopSystem(DWORD dwID);

#ifdef ENABLE_IKASHOP_LOGS
	void IkarusShopLog(DWORD who, DWORD itemid, std::string_view what, DWORD shopOwner, const std::string& extra = "", DWORD vnum = 0, DWORD count = 0, int64_t yang = 0, int cheque = 0);
#endif

private:
	ikashop::CShopCache			m_offlineshopShopCache;
	ikashop::CSafeboxCache		m_offlineshopSafeboxCache;
#endif
