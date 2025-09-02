//search
			bool			bShowSalesText;

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
#ifdef EXTEND_IKASHOP_PRO
			float			shopRangeView = 5000.f;
#endif
#endif

//search
		void							SetAlwaysShowNameFlag(int iFlag);
		void							SetShowDamageFlag(int iFlag);
		void							SetShowSalesTextFlag(int iFlag);

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
#ifdef EXTEND_IKASHOP_PRO
		void							SetShopRangeView(float distance) { m_Config.shopRangeView = MINMAX(600.0f, distance, 5000.f); }
		float							GetShopRangeView() const {return m_Config.shopRangeView;}
#endif
#endif