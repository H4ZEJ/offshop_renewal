//search
	if (!InitializeObjectTable())
	{
		sys_err("InitializeObjectTable FAILED");
		return false;
	}

	if (!InitializeMonarch())
	{
		sys_err("InitializeMonarch FAILED");
		return false;
	}

//add after

#ifdef ENABLE_IKASHOP_RENEWAL
	if (!InitializeIkarusShopTable())
	{
		sys_err("InitializeIkarusShopTable FAILED");
		return false;
	}
#endif