//search
			Set(HEADER_GC_SPECIFIC_EFFECT,	CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCSpecificEffect), STATIC_SIZE_PACKET));
			Set(HEADER_GC_DRAGON_SOUL_REFINE,		CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCDragonSoulRefine), STATIC_SIZE_PACKET));

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
			Set(HEADER_GC_NEW_OFFLINESHOP, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCNewOfflineshop), LARGE_DYNAMIC_SIZE_PACKET));
#endif

//search
static DWORD gs_nextDownloadMarkTime = 0;

//add after
#ifdef ENABLE_LARGE_DYNAMIC_PACKETS
enum
{
	STATIC_SIZE_PACKET = 0,
	DYNAMIC_SIZE_PACKET,
	LARGE_DYNAMIC_SIZE_PACKET,
};
#else
#define LARGE_DYNAMIC_SIZE_PACKET DYNAMIC_SIZE_PACKET
#endif

//search
	public:
		enum
		{
			STATIC_SIZE_PACKET = false,
			DYNAMIC_SIZE_PACKET = true,
		};

//replace with
#ifndef ENABLE_LARGE_DYNAMIC_PACKETS
	public:
		enum
		{
			STATIC_SIZE_PACKET = false,
			DYNAMIC_SIZE_PACKET = true,
		};
#endif

//search
	if (PacketType.isDynamicSizePacket)

//replace with
#ifdef ENABLE_LARGE_DYNAMIC_PACKETS
	if (PacketType.iPacketType == DYNAMIC_SIZE_PACKET)
#else
	if (PacketType.isDynamicSizePacket)
#endif

//search
	else
	{
		if (!PeekNoFetch(PacketType.iPacketSize))
		{
			return false;
		}
	}

//add before
#ifdef ENABLE_LARGE_DYNAMIC_PACKETS
	else if (PacketType.iPacketType == LARGE_DYNAMIC_SIZE_PACKET) {
		TLargeDynamicSizePacketHeader largeDynamicSizePacketHeader;
		if (!Peek(sizeof(TLargeDynamicSizePacketHeader), &largeDynamicSizePacketHeader))
			return false;

		if (!PeekNoFetch(largeDynamicSizePacketHeader.size))
		{
			Tracef("CPythonNetworkStream::CheckPacket - Not enough LARGE dynamic packet size: header %d packet size: %d\n",
				largeDynamicSizePacketHeader.header,
				largeDynamicSizePacketHeader.size);
			return false;
		}
	}
#endif