//search in function @@@ LPITEM CItem::RemoveFromCharacter()
		if (GetWindow() != SAFEBOX && GetWindow() != MALL)
		{

//replace with
		if (GetWindow() != SAFEBOX && GetWindow() != MALL
#ifdef ENABLE_IKASHOP_RENEWAL
			&& GetWindow() != IKASHOP_OFFLINESHOP 
			&& GetWindow() != IKASHOP_SAFEBOX
#endif
		)
		{

