#Search for "self.currentCubeNPC = 0"
#Under it, add:

		if app.ENABLE_IKASHOP_RENEWAL:
			if app.EXTEND_IKASHOP_ULTIMATE:
				self.affectShower.SetSearchShopBoard(self.interface.wndIkashopSearchShop)
				
				
# Search for "onPressKeyDict[app.DIK_F]			= lambda : self.__PressFKey()"
# Under it, add:

		if app.ENABLE_IKASHOP_RENEWAL:
			onPressKeyDict[app.DIK_Y]		= lambda : self.__PressYKey()
			onPressKeyDict[app.DIK_F8]		= lambda : self.__PressF8Key()
			
# Add the following defs:

	if app.ENABLE_IKASHOP_RENEWAL:
		def __PressYKey(self):
			self.interface.ToggleIkashopBusinessBoard()

		def __PressF8Key(self):
			self.interface.ToggleIkashopSearchShopBoard()