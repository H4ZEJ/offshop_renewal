# Search for "def AttachObject(self, Owner, Type, SlotNumber, ItemIndex, count = 0):
# Inside the function, search for:
		try:

			width = 1
			height = 1
# Under it, add:

			if app.ENABLE_IKASHOP_RENEWAL:
				if app.EXTEND_IKASHOP_ULTIMATE:
					if Type == player.SLOT_TYPE_OFFLINESHOP:
						item.SelectItem(self.AttachedItemIndex)
						self.AttachedIconHandle = item.GetIconInstance()

						if not self.AttachedIconHandle:
							self.AttachedIconHandle = 0
							self.DeattachObject()
							return

						(width, height) = item.GetItemSize()