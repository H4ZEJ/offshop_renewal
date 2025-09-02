#Add:

if app.ENABLE_IKASHOP_RENEWAL:
	import uiikashop
	
	
#Inside @class Interface(object):|def __init__(self):
#Search for "self.equipmentDialogDict = {}"
#Under it, add:

		if app.ENABLE_IKASHOP_RENEWAL:
			self.wndIkashopBusiness = None
			self.wndIkashopSearchShop = None
			if app.EXTEND_IKASHOP_ULTIMATE:
				self.wndIkashopShopSkin = None
				
#Inside @class Interface(object):|def __MakeWindows(self):
#Search for "self.wndChatLog = wndChatLog"
#Under it, add:

		if app.ENABLE_IKASHOP_RENEWAL:
			self.wndIkashopBusiness = uiikashop.IkarusShopBusinessBoard()
			self.wndIkashopSearchShop = uiikashop.IkarusSearchShopBoard()
			if app.EXTEND_IKASHOP_PRO:
				from _weakref import proxy
				self.wndIkashopSearchShop.SetOpenWhisper(lambda name, _s = proxy(self) : _s.OpenWhisperDialog(name))
			if app.EXTEND_IKASHOP_ULTIMATE:
				self.wndIkashopShopSkin = uiikashop.IkarusShopSkinBoard()
				
#Inside @class Interface(object):|def Close(self):
#Search for:
if self.wndCube:
	self.wndCube.Destroy()
	
# Under it, add:

		if app.ENABLE_IKASHOP_RENEWAL:
			if self.wndIkashopBusiness:
				self.wndIkashopBusiness.Destroy()
				self.wndIkashopBusiness = None
			if self.wndIkashopSearchShop:
				self.wndIkashopSearchShop.Destroy()
				self.wndIkashopSearchShop = None
			if app.EXTEND_IKASHOP_ULTIMATE:
				if self.wndIkashopShopSkin:
					self.wndIkashopShopSkin.Destroy()
					self.wndIkashopShopSkin = None
					
#Add the following defs:

	if app.ENABLE_IKASHOP_RENEWAL:
		def ToggleIkashopBusinessBoard(self):
			self.wndIkashopBusiness.Toggle()

		def ToggleIkashopSearchShopBoard(self):
			self.wndIkashopSearchShop.Toggle()
			


# Inside "def __HideWindows(self):"
# Search for "hideWindows = filter(lambda x:x.IsShow(), hideWindows)"
# Above it, add:

		if app.ENABLE_IKASHOP_RENEWAL:
			hideWindows += self.wndIkashopBusiness, self.wndIkashopSearchShop
			if app.EXTEND_IKASHOP_ULTIMATE:
				hideWindows += self.wndIkashopShopSkin,