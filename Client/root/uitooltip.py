# Inside "def SetInventoryItem(self, slotIndex, window_type = player.INVENTORY"
# Search for:

		metinSlot = [player.GetItemMetinSocket(window_type, slotIndex, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
		attrSlot = [player.GetItemAttribute(window_type, slotIndex, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]

		self.AddItemData(itemVnum, metinSlot, attrSlot, 0, 0, window_type, slotIndex)
		
#Under it, add:

		if app.ENABLE_IKASHOP_RENEWAL:
			if app.EXTEND_IKASHOP_ULTIMATE:
				if itemVnum == 51998:
					self.AppendTextLine(localeInfo.IKASHOP_ULTIMATE_SHOP_PRIVATE_KEY_MULTIUSE_TOOLTIP)
					

#Search for:

	def __AppendMagicDefenceInfo(self, itemAbsChance = 0):
		magicDefencePower = item.GetValue(0)
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if itemAbsChance != 0:
				magicDefencePower = self.CalcAcceValue(magicDefencePower, itemAbsChance)

		if magicDefencePower > 0:
			self.AppendTextLine(localeInfo.TOOLTIP_ITEM_MAGIC_DEF_POWER % magicDefencePower, self.GetChangeTextLineColor(magicDefencePower))


#Under it, add:

	if app.ENABLE_IKASHOP_RENEWAL:
		def AppendAttributes(self, attrs):
			attrs = list(attrs)
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM - len(attrs)):
				attrs.append((0,0))
			self.__AppendAttributeInformation(attrs)