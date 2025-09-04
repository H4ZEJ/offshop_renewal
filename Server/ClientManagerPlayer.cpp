//search in function @@@ void CClientManager::QUERY_PLAYER_LOAD
		snprintf(queryStr, sizeof(queryStr),
				"SELECT dwPID,bType,bApplyOn,lApplyValue,dwFlag,lDuration,lSPCost FROM affect%s WHERE dwPID=%d",
				GetTablePostfix(), packet->player_id);
		CDBManager::instance().ReturnQuery(queryStr, QID_AFFECT, peer->GetHandle(), new ClientHandleInfo(dwHandle, packet->player_id));
	}

//after add
#ifdef ENABLE_IKASHOP_RENEWAL
	IkarusShopLoadShopSafebox(peer, packet->player_id);
#ifdef EXTEND_IKASHOP_PRO
	IkarusShopLoadNotifications(peer, packet->player_id);
#endif
#endif

//search in function @@@ void CClientManager::__QUERY_PLAYER_DELETE
	char szQuery[128];
	snprintf(szQuery, sizeof(szQuery), "SELECT p.id, p.level, p.name FROM player_index%s AS i, player%s AS p WHERE pid%u=%u AND pid%u=p.id",
			GetTablePostfix(), GetTablePostfix(), packet->account_index + 1, packet->player_id, packet->account_index + 1);

	ClientHandleInfo * pi = new ClientHandleInfo(dwHandle, packet->player_id);
	pi->account_index = packet->account_index;

//before add
#ifdef ENABLE_IKASHOP_RENEWAL
	if (IsUsingIkarusShopSystem(packet->player_id))
	{
		sys_log(0, "PLAYER_DELETE FAILED %u IS USING OFFLINESHOP SYSTEM", packet->player_id);
		peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, dwHandle, 1);
		peer->EncodeBYTE(packet->account_index);
		return;
	}
#endif
