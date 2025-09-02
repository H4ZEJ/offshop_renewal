//search
		case HEADER_GC_QUICKSLOT_ADD:
			if (RecvQuickSlotAddPacket())
				return;
			break;

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
		case HEADER_GC_NEW_OFFLINESHOP:
			RecvOfflineshopPacket();
			return;
#endif