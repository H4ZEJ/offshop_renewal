# Search for:

	if app.ENABLE_DRAGON_SOUL_SYSTEM:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_DRAGON_SOUL_DECK1] = (localeInfo.TOOLTIP_DRAGON_SOUL_DECK1, "d:/ymir work/ui/dragonsoul/buff_ds_sky1.tga")
		AFFECT_DATA_DICT[chr.NEW_AFFECT_DRAGON_SOUL_DECK2] = (localeInfo.TOOLTIP_DRAGON_SOUL_DECK2, "d:/ymir work/ui/dragonsoul/buff_ds_land1.tga")


#Under it, add:

	if app.ENABLE_IKASHOP_RENEWAL:
		if app.EXTEND_IKASHOP_ULTIMATE:
			AFFECT_DATA_DICT[chr.NEW_AFFECT_SEARCH_SHOP_PREMIUM] = (localeInfo.IKASHOP_ULTIMATE_PREMIUM_AFFECT_DESCRIPTION, "ikashop/ultimate/search_shop_preium_affect.png")


# Search for:

	def ClearAffects(self):
		self.living_affectImageDict={}
		for key, image in self.affectImageDict.items():
			if not image.IsSkillAffect():
				self.living_affectImageDict[key] = image
		self.affectImageDict = self.living_affectImageDict
		self.__ArrangeImageList()
		
		
# Under it, add:

	if app.ENABLE_IKASHOP_RENEWAL:
		if app.EXTEND_IKASHOP_ULTIMATE:
			def SetSearchShopBoard(self, board):
				from _weakref import proxy
				self.searchShopBoard = proxy(board)
				
# Search for:

	def BINARY_NEW_AddAffect(self, type, pointIdx, value, duration):

		print "BINARY_NEW_AddAffect", type, pointIdx, value, duration

		if type < 500:
			return
			
# Under it, add:

		if app.ENABLE_IKASHOP_RENEWAL:
			if app.EXTEND_IKASHOP_ULTIMATE:
				if type == chr.NEW_AFFECT_SEARCH_SHOP_PREMIUM:
					self.searchShopBoard.ActiveSortByPriceFeature()
					
# Search for:

		if affect != chr.NEW_AFFECT_AUTO_SP_RECOVERY and affect != chr.NEW_AFFECT_AUTO_HP_RECOVERY:
			description = description(float(value))

#Replace with:

		if affect != chr.NEW_AFFECT_AUTO_SP_RECOVERY and affect != chr.NEW_AFFECT_AUTO_HP_RECOVERY:
			if app.ENABLE_IKASHOP_RENEWAL:
				if app.EXTEND_IKASHOP_ULTIMATE:
					if affect != chr.NEW_AFFECT_SEARCH_SHOP_PREMIUM:
						description = description(float(value))
				else:
					description = description(float(value))
			else:
				description = description(float(value))


#Search for:

	def BINARY_NEW_RemoveAffect(self, type, pointIdx):
		if type == chr.NEW_AFFECT_MALL:
			affect = self.MALL_DESC_IDX_START + pointIdx
		else:
			affect = type


# Under it, add:


		if app.ENABLE_IKASHOP_RENEWAL:
			if app.EXTEND_IKASHOP_ULTIMATE:
				if type == chr.NEW_AFFECT_SEARCH_SHOP_PREMIUM:
					self.searchShopBoard.DeactiveSortByPriceFeature()