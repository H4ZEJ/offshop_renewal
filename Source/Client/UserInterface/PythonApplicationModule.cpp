#ifdef ENABLE_IKASHOP_RENEWAL
#ifdef EXTEND_IKASHOP_PRO
	PyModule_AddIntConstant(poModule, "EXTEND_IKASHOP_PRO", 1);
#else
	PyModule_AddIntConstant(poModule, "EXTEND_IKASHOP_PRO", 0);
#endif
#ifdef EXTEND_IKASHOP_ULTIMATE
	PyModule_AddIntConstant(poModule, "EXTEND_IKASHOP_ULTIMATE", 1);
#else
	PyModule_AddIntConstant(poModule, "EXTEND_IKASHOP_ULTIMATE", 0);
#endif
	PyModule_AddIntConstant(poModule, "ENABLE_IKASHOP_RENEWAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_IKASHOP_RENEWAL", 0);
#endif