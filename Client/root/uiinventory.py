# Add:

if app.ENABLE_IKASHOP_RENEWAL:
	import ikashop
	
	
# Inside "def RefreshBagSlotWindow(self):"
# Search for:

		for i in xrange(player.INVENTORY_PAGE_SIZE):
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

			itemCount = getItemCount(slotNumber)
			if 0 == itemCount:
				self.wndItem.ClearSlot(i)
				continue
			elif 1 == itemCount:
				itemCount = 0

			itemVnum = getItemVNum(slotNumber)
			setItemVNum(i, itemVnum, itemCount)
			
# Under it, add:


			if app.ENABLE_IKASHOP_RENEWAL:
				if app.EXTEND_IKASHOP_PRO:
					self.wndItem.EnableSlot(i)
					board = ikashop.GetBusinessBoard()
					if board and (board.IsShow() or board.IsCreatingAuction()):
						item.SelectItem(itemVnum)
						if item.IsAntiFlag(item.ANTIFLAG_GIVE) or item.IsAntiFlag(item.ANTIFLAG_MYSHOP):
							self.wndItem.DisableSlot(i)
							
# Search for:

	def ShowToolTip(self, slotIndex):
		if None != self.tooltipItem:
			self.tooltipItem.SetInventoryItem(slotIndex)
			
			
#Under it, add:

			if app.ENABLE_IKASHOP_RENEWAL:
				board = ikashop.GetBusinessBoard()
				if board and (board.IsShow() or board.IsCreatingAuction()):
					item.SelectItem(player.GetItemIndex(player.INVENTORY, slotIndex))
					if not item.IsAntiFlag(item.ANTIFLAG_GIVE) and not item.IsAntiFlag(item.ANTIFLAG_MYSHOP):
						self.tooltipItem.AppendTextLine(localeInfo.IKASHOP_BUSINESS_SELL_ITEM_TOOLTIP)
						
						
# Search for:


		curCursorNum = app.GetCursor()
		if app.SELL == curCursorNum:
			return

		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			return

		slotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)
		


# Under it, add:

		if app.ENABLE_IKASHOP_RENEWAL:
			board = ikashop.GetBusinessBoard()
			if board:
				item.SelectItem(player.GetItemIndex(player.INVENTORY, slotIndex))
				if not item.IsAntiFlag(item.ANTIFLAG_GIVE) and not item.IsAntiFlag(item.ANTIFLAG_MYSHOP):
					if board.IsCreatingAuction():
						ikashop.AddItemOnAuction(player.INVENTORY, slotIndex)
						return
					elif board.IsShow():
						ikashop.AddItemOnShop(player.INVENTORY, slotIndex)
						return
						
			if app.EXTEND_IKASHOP_ULTIMATE:
				itemVnum = player.GetItemIndex(player.INVENTORY, slotIndex)
				if itemVnum == 52000:
					shopSkinBoard = ikashop.GetShopSkinBoard()
					if shopSkinBoard:
						shopSkinBoard.Open()
					return
				elif itemVnum == 51998:
					if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
						for i in xrange(min(player.GetItemCount(player.INVENTORY, slotIndex), 10)):
							self.__UseItem(slotIndex)
						self.OverOutItem()
						return