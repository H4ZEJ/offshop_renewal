//search
	m_Config.bShowDamage		= true;
	m_Config.bShowSalesText		= true;

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
#ifdef EXTEND_IKASHOP_PRO
	m_Config.shopRangeView = 5000.0f;
#endif
#endif

//search
		else if (!stricmp(command, "SHOW_SALESTEXT"))
			m_Config.bShowSalesText = atoi(value) == 1 ? true : false;


//add after
#ifdef ENABLE_IKASHOP_RENEWAL
#ifdef EXTEND_IKASHOP_PRO
		else if (!stricmp(command, "SHOP_RANGE_VIEW"))
			m_Config.shopRangeView = std::stof(value);
#endif
#endif

//search
	if (m_Config.bShowSalesText == 0)
		fprintf(fp, "SHOW_SALESTEXT		%d\n", m_Config.bShowSalesText);

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
#ifdef EXTEND_IKASHOP_PRO
	fprintf(fp, "SHOP_RANGE_VIEW		%f\n", m_Config.shopRangeView);
#endif
#endif