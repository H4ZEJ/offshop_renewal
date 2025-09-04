//search
void initPlayer()

//add before
#ifdef ENABLE_IKASHOP_RENEWAL
#ifdef EXTEND_IKASHOP_PRO
PyObject* playerRefreshInventory(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().RefreshInventory();

	return Py_BuildNone();
}
#endif
#endif

//search
		{ "GetItemLink",				playerGetItemLink,					METH_VARARGS },
		{ "SlotTypeToInvenType",		playerSlotTypeToInvenType,			METH_VARARGS },
		{ "SendDragonSoulRefine",		playerSendDragonSoulRefine,			METH_VARARGS },

//add after

#ifdef ENABLE_IKASHOP_RENEWAL
#ifdef EXTEND_IKASHOP_PRO
		{ "RefreshInventory",			playerRefreshInventory,				METH_VARARGS },
#endif
#endif

//search
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_INVENTORY",				DRAGON_SOUL_INVENTORY);
	PyModule_AddIntConstant(poModule, "GROUND",								GROUND);

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
#ifdef EXTEND_IKASHOP_ULTIMATE
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_OFFLINESHOP",				SLOT_TYPE_OFFLINESHOP);
	PyModule_AddIntConstant(poModule, "IKASHOP_OFFLINESHOP",				IKASHOP_OFFLINESHOP);
	PyModule_AddIntConstant(poModule, "IKASHOP_SAFEBOX",					IKASHOP_SAFEBOX);
#endif
#endif