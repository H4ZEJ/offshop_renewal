//search
			case HEADER_GC_SPECIFIC_EFFECT:
				ret = RecvSpecificEffect();
				break;

			case HEADER_GC_DRAGON_SOUL_REFINE:
				ret = RecvDragonSoulRefine();
				break;

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
			case HEADER_GC_NEW_OFFLINESHOP:
				ret = RecvOfflineshopPacket();
				break;
#endif

//search and replace
void CPythonNetworkStream::__LeaveGamePhase()
{
	CInstanceBase::ClearPVPKeySystem();

	__ClearNetworkActorManager();

	m_bComboSkillFlag = FALSE;

	IAbstractCharacterManager& rkChrMgr=IAbstractCharacterManager::GetSingleton();
	rkChrMgr.Destroy();

	CPythonItem& rkItemMgr=CPythonItem::Instance();
	rkItemMgr.Destroy();
#ifdef ENABLE_IKASHOP_RENEWAL
#ifdef ENABLE_IKASHOP_ENTITIES
	CPythonIkarusShop::instance().DeleteEntities();
#endif
#endif
}

