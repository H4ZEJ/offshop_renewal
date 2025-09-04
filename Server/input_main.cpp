//search
int CInputMain::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)

//add before

#ifdef ENABLE_IKASHOP_RENEWAL
#include "ikarus_shop.h"
#include "ikarus_shop_manager.h"
struct DecodeException : public std::exception 
{
	const char* what() const noexcept override
	{
		return "Decode failed receiving ikashop packet";
	}
};

template <class T>
const T& Decode(const char*& data, int& left, int& len)
{
	if(left < sizeof(T))
		throw DecodeException();

	auto obj = reinterpret_cast<const T*>(data);
	data += sizeof(T);
	len  += sizeof(T);
	left -= sizeof(T);
	return *obj;
}

int IkarusShopPacketCreateNewShop(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	ikashop::GetManager().RecvShopCreateNewClientPacket(ch);
	return extra;
}

int IkarusShopPacketForceCloseShop(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	ikashop::GetManager().RecvShopForceCloseClientPacket(ch);
	return 0;
}

int IkarusShopPacketOpenShop(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGShopOpen>(data, left, extra);
	ikashop::GetManager().RecvShopOpenClientPacket(ch, pack.ownerid);
	return extra;
}

int IkarusShopPacketOpenShowOwner(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	ikashop::GetManager().RecvShopOpenMyShopClientPacket(ch);
	return 0;
}

int IkarusShopPacketBuyItem(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGShopBuyItem>(data, left, extra);
	ikashop::GetManager().RecvShopBuyItemClientPacket(ch, pack.ownerid, pack.itemid, pack.searching, pack.seenprice);
	return extra;
}

int IkarusShopPacketAddItem(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGAddItem>(data, left, extra);
	ikashop::GetManager().RecvShopAddItemClientPacket(ch, pack.pos, pack.price, pack.destpos);
	return extra;
}

int IkarusShopPacketRemoveItem(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGRemoveItem>(data, left, extra);
	ikashop::GetManager().RecvShopRemoveItemClientPacket(ch, pack.itemid);
	return extra;
}

int IkarusShopPacketEditItem(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGEditItem>(data, left, extra);
	ikashop::GetManager().RecvShopEditItemClientPacket(ch, pack.itemid, pack.price);
	return extra;
}

int IkarusShopPacketFilterRequest(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGFilterRequest>(data, left, extra);
	ikashop::GetManager().RecvShopFilterRequestClientPacket(ch, pack.filter);
	return extra;
}

int IkarusShopPacketSearchFillRequest(LPCHARACTER ch, const char* data, int left)
{
	ikashop::GetManager().RecvShopSearchFillRequestClientPacket(ch);
	return 0;
}

int IkarusShopPacketOpenSafebox(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	ikashop::GetManager().RecvShopSafeboxOpenClientPacket(ch);
	return 0;
}

int IkarusShopPacketCloseMyShopBoard(LPCHARACTER ch)
{
	ikashop::GetManager().RecvCloseMyShopBoardClientPacket(ch);
	return 0;
}

int IkarusShopPacketCloseShopGuest(LPCHARACTER ch)
{
	ikashop::GetManager().RecvCloseShopGuestClientPacket(ch);
	return 0;
}

int IkarusShopPacketGetItemSafebox(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGShopSafeboxGetItem>(data, left, extra);
	ikashop::GetManager().RecvShopSafeboxGetItemClientPacket(ch, pack.itemid);
	return extra;
}

int IkarusShopPacketGetValutesSafebox(LPCHARACTER ch, const char* data, int left)
{
	ikashop::GetManager().RecvShopSafeboxGetValutesClientPacket(ch);
	return 0;
}

int IkarusShopPacketCloseSafebox(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	ikashop::CShopManager& rManager = ikashop::GetManager();
	rManager.RecvShopSafeboxCloseClientPacket(ch);
	return 0;
}

int IkarusShopPacketListRequest(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	ikashop::CShopManager& rManager = ikashop::GetManager();
	return 0;
}

#ifdef ENABLE_IKASHOP_ENTITIES
int IkarusShopPacketClickEntity(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGShopClickEntity>(data, left, extra);

	ikashop::GetManager().RecvShopClickEntityClientPacket(ch, pack.vid);
	return extra;
}

#ifdef EXTEND_IKASHOP_PRO
int IkarusShopPacketMoveEntity(LPCHARACTER ch)
{
	ikashop::GetManager().RecvMoveShopEntityClientPacket(ch);
	return 0;
}
#endif
#endif

#ifdef EXTEND_IKASHOP_PRO
int IkarusShopPacketNotificationListRequest(LPCHARACTER ch)
{
	ikashop::GetManager().SendNotificationListClientPacket(ch);
	ch->SetLookingNotificationList(true);
	return 0;
}

int IkarusShopPacketNotificationListClose(LPCHARACTER ch)
{
	ch->SetLookingNotificationList(false);
	return 0;
}
#endif

#ifdef EXTEND_IKASHOP_ULTIMATE
int IkarusShopPacketPriceAverageRequest(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGPriceAverageRequest>(data, left, extra);

	ikashop::GetManager().RecvPriceAverageRequestClientPacket(ch, pack.vnum, pack.count);
	return extra;
}

int IkarusShopPacketShopDecorationUse(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& index = Decode<int>(data, left, extra);

	ikashop::GetManager().RecvShopDecorationUseClientPacket(ch, index);
	return extra;
}

int IkarusShopPacketShopMoveItem(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& subpack = Decode<ikashop::TSubPacketCGShopMoveItem>(data, left, extra);

	ikashop::GetManager().RecvShopMoveItemClientPacket(ch, subpack.srcPos, subpack.destPos);
	return extra;
}
#endif

int IkarusShopPacket(const char* data , LPCHARACTER ch, int iBufferLeft)
{
	BYTE subheader = 255;
	try
	{
		int extra = 0;
		const auto& pack = Decode<TPacketCGNewIkarusShop>(data, iBufferLeft, extra);
		subheader = pack.subheader;

		switch (pack.subheader)
		{

		case ikashop::SUBHEADER_CG_SHOP_CREATE_NEW:
			return IkarusShopPacketCreateNewShop(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_FORCE_CLOSE:
			return IkarusShopPacketForceCloseShop(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_OPEN:
			return IkarusShopPacketOpenShop(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_OPEN_OWNER:
			return IkarusShopPacketOpenShowOwner(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_BUY_ITEM:
			return IkarusShopPacketBuyItem(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_ADD_ITEM:
			return IkarusShopPacketAddItem(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_REMOVE_ITEM:
			return IkarusShopPacketRemoveItem(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_EDIT_ITEM:
			return IkarusShopPacketEditItem(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_FILTER_REQUEST:
			return IkarusShopPacketFilterRequest(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_SEARCH_FILL_REQUEST:
			return IkarusShopPacketSearchFillRequest(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_SAFEBOX_OPEN:
			return IkarusShopPacketOpenSafebox(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_SAFEBOX_GET_ITEM:
			return IkarusShopPacketGetItemSafebox(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_SAFEBOX_GET_VALUTES:
			return IkarusShopPacketGetValutesSafebox(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_SAFEBOX_CLOSE:
			return IkarusShopPacketCloseSafebox(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_CLOSE_MY_SHOP_BOARD:
			return IkarusShopPacketCloseMyShopBoard(ch);

		case ikashop::SUBHEADER_CG_CLOSE_SHOP_GUEST:
			return IkarusShopPacketCloseShopGuest(ch);

#ifdef ENABLE_IKASHOP_ENTITIES
		case ikashop::SUBHEADER_CG_CLICK_ENTITY:
			return IkarusShopPacketClickEntity(ch, data, iBufferLeft);
#ifdef EXTEND_IKASHOP_PRO
		case ikashop::SUBHEADER_CG_MOVE_SHOP_ENTITY:
			return IkarusShopPacketMoveEntity(ch);
#endif
#endif
#ifdef EXTEND_IKASHOP_PRO
		case ikashop::SUBHEADER_CG_NOTIFICATION_LIST_REQUEST:
			return IkarusShopPacketNotificationListRequest(ch);
		case ikashop::SUBHEADER_CG_NOTIFICATION_LIST_CLOSE:
			return IkarusShopPacketNotificationListClose(ch);
#endif
#ifdef EXTEND_IKASHOP_ULTIMATE
		case ikashop::SUBHEADER_CG_PRICE_AVERAGE_REQUEST:
			return IkarusShopPacketPriceAverageRequest(ch, data, iBufferLeft);
		case ikashop::SUBHEADER_CG_SHOP_DECORATION_USE:
			return IkarusShopPacketShopDecorationUse(ch, data, iBufferLeft);
		case ikashop::SUBHEADER_CG_SHOP_MOVE_ITEM:
			return IkarusShopPacketShopMoveItem(ch, data, iBufferLeft);
#endif
		default:
			sys_err("UNKNOWN SUBHEADER %d ", subheader);
			return -1;
		}
	}
	catch(const DecodeException& e)
	{
		sys_err("FAILED TO DECODE SUBHEADER %u", subheader);
		return -1;
	}
}
#endif

//search
		case HEADER_CG_REFINE:
			Refine(ch, c_pData);
			break;

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
		case HEADER_CG_NEW_OFFLINESHOP:
			if((iExtraLen = IkarusShopPacket(c_pData, ch, m_iBufferLeft))< 0)
				return -1;
			break;
#endif