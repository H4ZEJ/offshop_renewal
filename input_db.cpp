//add where you want
#ifdef ENABLE_IKASHOP_RENEWAL
#include "ikarus_shop.h"
#include "ikarus_shop_manager.h"
#endif

//search
int CInputDB::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)

//add before
#ifdef ENABLE_IKASHOP_RENEWAL
template <class T>
const T& Decode(const char*& data)
{
	auto obj = reinterpret_cast<const T*>(data);
	data += sizeof(T);
	return *obj;
}

void IkarusShopLoadTables(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGLoadTables>(data);	
	
	auto& manager = ikashop::GetManager();

	for (DWORD i = 0; i < subpack.shopcount; i++)
	{
		const auto& shopInfo = Decode<ikashop::TShopInfo>(data);
		auto shop = manager.PutsNewShop(shopInfo);

		for (DWORD j = 0; j < shopInfo.count; j++)
			shop->AddItem(Decode<ikashop::TShopItem>(data));
	}

#ifdef EXTEND_IKASHOP_ULTIMATE
	auto saleHistoryCount = Decode<DWORD>(data);
	for(DWORD i=0; i < saleHistoryCount; i++)
		manager.RecvShopRegisterSaleHistoryDBPacket(Decode<ikashop::TSaleHistory>(data));
#endif
}

void IkarusShopBuyItemPacket(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGBuyItem>(data);
	ikashop::GetManager().RecvShopBuyDBPacket(subpack.buyerid, subpack.ownerid, subpack.itemid);
}

void IkarusShopLockedBuyItemPacket(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGLockedBuyItem>(data);
	ikashop::GetManager().RecvShopLockedBuyItemDBPacket(subpack.buyerid, subpack.ownerid, subpack.itemid);
}

void IkarusShopEditItemPacket(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGEditItem>(data);
	ikashop::GetManager().RecvShopEditItemDBPacket(subpack.ownerid , subpack.itemid, subpack.price);
}

void IkarusShopRemoveItemPacket(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGRemoveItem>(data);
	ikashop::GetManager().RecvShopRemoveItemDBPacket(subpack.ownerid , subpack.itemid, subpack.requester);
}

void IkarusShopAddItemPacket(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGAddItem>(data);
	ikashop::GetManager().RecvShopAddItemDBPacket(subpack.ownerid, subpack.item);
}

void IkarusShopForceClosePacket(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGShopForceClose>(data);
	ikashop::GetManager().RecvShopForceCloseDBPacket(subpack.ownerid);
}

void IkarusShopShopCreateNewPacket(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGShopCreateNew>(data);
	
	ikashop::GetManager().RecvShopCreateNewDBPacket(subpack.shop);
}

void IkarusShopShopChangeNamePacket(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGShopChangeName>(data);
	ikashop::GetManager().RecvShopChangeNameDBPacket(subpack.ownerid , subpack.name);
}

void IkarusShopSafeboxAddItemPacket(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGSafeboxAddItem>(data);
	ikashop::GetManager().RecvShopSafeboxAddItemDBPacket(subpack.ownerid , subpack.itemid , subpack.item);
}

void IkarusShopSafeboxAddValutesPacket(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGSafeboxAddValutes>(data);
	ikashop::GetManager().RecvShopSafeboxAddValutesDBPacket(subpack.ownerid , subpack.valute);
}

void IkarusShopSafeboxLoad(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGSafeboxLoad>(data);
	std::vector<ikashop::TShopPlayerItem> items;
	items.reserve(subpack.itemcount);

	for (DWORD i = 0; i < subpack.itemcount; i++)
		items.emplace_back(Decode<ikashop::TShopPlayerItem>(data));

	ikashop::GetManager().RecvShopSafeboxLoadDBPacket(subpack.ownerid , subpack.valute, items);
}

void IkarusShopSafeboxExpiredItem(const char* data) {
	const auto& subpack = Decode<ikashop::TSubPacketDGSafeboxExpiredItem>(data);
	ikashop::GetManager().RecvShopSafeboxExpiredItemDBPacket(subpack.ownerid, subpack.itemid);
}

void IkarusShopSafeboxGetItemConfirm(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGSafeboxGetItemConfirm>(data);
	ikashop::GetManager().RecvShopSafeboxGetItemConfirm(subpack.ownerid, subpack.itemid);
}

void IkarusShopShopExpired(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGShopExpired>(data);
	ikashop::GetManager().RecvShopExpiredDBPacket(subpack.ownerid);
}

#ifdef EXTEND_IKASHOP_PRO
void IkarusShopNotificationLoad(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGNotificationLoad>(data);
	std::vector<ikashop::TNotificationInfo> notifications;
	notifications.reserve(subpack.count);
	for(DWORD i=0; i < subpack.count; i++)
		notifications.emplace_back(Decode<ikashop::TNotificationInfo>(data));
	ikashop::GetManager().RecvNotificationLoadDBPacket(subpack.ownerid, notifications);
}

void IkarusShopNotificationForward(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGNotificationForward>(data);
	ikashop::GetManager().RecvNotificationForwardDBPacket(subpack.ownerid, subpack);
}

void IkarusShopRestoreDuration(const char* data)
{
	const auto& owner = Decode<DWORD>(data);
	ikashop::GetManager().RecvShopRestoreDurationDBPacket(owner);
}
#ifdef ENABLE_IKASHOP_ENTITIES
void IkarusShopMoveShopEntity(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGMoveShopEntity>(data);
	ikashop::GetManager().RecvMoveShopEntityDBPacket(subpack.owner, subpack.spawn);
}
#endif
#endif

#ifdef EXTEND_IKASHOP_ULTIMATE
void IkarusShopRegisterSaleHistory(const char* data)
{
	const auto& sale = Decode<ikashop::TSaleHistory>(data);
	ikashop::GetManager().RecvShopRegisterSaleHistoryDBPacket(sale);
}

void IkarusShopDecorationSet(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGShopDecorationSet>(data);
	ikashop::GetManager().RecvShopDecorationSetDBPacket(subpack.owner, subpack.index, subpack.time);
}

void IkarusShopMoveItem(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGShopMoveItem>(data);
	ikashop::GetManager().RecvShopMoveItemDBPacket(subpack.owner, subpack.itemid, subpack.destCell);
}

void IkarusShopUnlockCell(const char* data)
{
	const auto& subpack = Decode<ikashop::TSubPacketDGShopUnlockCell>(data);
	ikashop::GetManager().RecvShopUnlockCellDBPacket(subpack.owner, subpack.lockIndex);
}
#endif

void IkarusShopPacket(const char* data)
{
	const auto& pack = Decode<TPacketDGNewIkarusShop>(data);
	switch (pack.bSubHeader)
	{
	case ikashop::SUBHEADER_DG_LOAD_TABLES:
		IkarusShopLoadTables(data);
		return;

	case ikashop::SUBHEADER_DG_BUY_ITEM:
		IkarusShopBuyItemPacket(data);
		return;

	case ikashop::SUBHEADER_DG_LOCKED_BUY_ITEM:
		IkarusShopLockedBuyItemPacket(data);
		return;

	case ikashop::SUBHEADER_DG_EDIT_ITEM:
		IkarusShopEditItemPacket(data);
		return;
	case ikashop::SUBHEADER_DG_REMOVE_ITEM:
		IkarusShopRemoveItemPacket(data);
		return;

	case ikashop::SUBHEADER_DG_ADD_ITEM:
		IkarusShopAddItemPacket(data);
		return;

	case ikashop::SUBHEADER_DG_SHOP_FORCE_CLOSE:
		IkarusShopForceClosePacket(data);
		return;

	case ikashop::SUBHEADER_DG_SHOP_CREATE_NEW:
		IkarusShopShopCreateNewPacket(data);
		return;

	case ikashop::SUBHEADER_DG_SHOP_CHANGE_NAME:
		IkarusShopShopChangeNamePacket(data);
		return;

	case ikashop::SUBHEADER_DG_SHOP_EXPIRED:
		IkarusShopShopExpired(data);
		break;

	case ikashop::SUBHEADER_DG_SAFEBOX_ADD_ITEM:
		IkarusShopSafeboxAddItemPacket(data);
		return;

	case ikashop::SUBHEADER_DG_SAFEBOX_ADD_VALUTES:
		IkarusShopSafeboxAddValutesPacket(data);
		return;

	case ikashop::SUBHEADER_DG_SAFEBOX_LOAD:
		IkarusShopSafeboxLoad(data);
		return;

	case ikashop::SUBHEADER_DG_SAFEBOX_EXPIRED_ITEM:
		IkarusShopSafeboxExpiredItem(data);
		return;

	case ikashop::SUBHEADER_DG_SAFEBOX_GET_ITEM_CONFIRM:
		IkarusShopSafeboxGetItemConfirm(data);
		return;

#ifdef EXTEND_IKASHOP_PRO
	case ikashop::SUBHEADER_DG_NOTIFICATION_LOAD:
		IkarusShopNotificationLoad(data);
		return;

	case ikashop::SUBHEADER_DG_NOTIFICATION_FORWARD:
		IkarusShopNotificationForward(data);
		return;

	case ikashop::SUBHEADER_DG_SHOP_RESTORE_DURATION:
		IkarusShopRestoreDuration(data);
		return;
#ifdef ENABLE_IKASHOP_ENTITIES
	case ikashop::SUBHEADER_DG_MOVE_SHOP_ENTITY:
		IkarusShopMoveShopEntity(data);
		return;
#endif
#endif

#ifdef EXTEND_IKASHOP_ULTIMATE
	case ikashop::SUBHEADER_DG_SALE_HISTORY_REGISTER:
		IkarusShopRegisterSaleHistory(data);
		return;

	case ikashop::SUBHEADER_DG_SHOP_DECORATION_SET:
		IkarusShopDecorationSet(data);
		return;

	case ikashop::SUBHEADER_DG_SHOP_MOVE_ITEM:
		IkarusShopMoveItem(data);
		return;

	case ikashop::SUBHEADER_DG_SHOP_UNLOCK_CELL:
		IkarusShopUnlockCell(data);
		return;
#endif
	default:
		sys_err("UKNOWN SUB HEADER %d ", pack.bSubHeader);
		return;
	}
}
#endif

//search
	case HEADER_DG_RESPOND_CHANNELSTATUS:
		RespondChannelStatus(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
	case HEADER_DG_NEW_OFFLINESHOP:
		IkarusShopPacket(c_pData);
		break;
#endif