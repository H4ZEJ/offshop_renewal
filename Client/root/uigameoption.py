# Search for:

			if app.ENABLE_IKASHOP_RENEWAL:
				text = text.lower().replace("k", "000")
				
				
# Under them, add:

		if app.ENABLE_IKASHOP_RENEWAL:
			if app.EXTEND_IKASHOP_PRO:
				self.shopRangeViewHeader = ui.TextLine()
				self.shopRangeViewHeader.SetParent(self)
				self.shopRangeViewHeader.SetPosition(30, self.GetHeight() - 10 - 10)
				self.shopRangeViewHeader.SetText(localeInfo.IKASHOP_PRO_SHOP_RANGE_VIEW_HEADER)
				self.shopRangeViewHeader.Show()

				distance = systemSetting.GetShopRangeView()
				pct = (distance - 600.0) / 4400.0
				self.shopFloatBar = ui.SliderBar()
				self.shopFloatBar.SetParent(self)
				self.shopFloatBar.SetPosition(130-39, self.GetHeight()-6 - 10)
				self.shopFloatBar.SetEvent(self._OnSlideShopRangeView)
				self.shopFloatBar.SetSliderPos(pct)
				self.shopFloatBar.Show()

				self.SetSize(self.GetWidth(), self.GetHeight() + 10)
				board = self.GetChild("board")
				board.SetSize(board.GetWidth(), board.GetHeight() + 10)
				
	if app.ENABLE_IKASHOP_RENEWAL:
		if app.EXTEND_IKASHOP_PRO:
			def _OnSlideShopRangeView(self):
				pos = self.shopFloatBar.GetSliderPos()
				distance = pos * 4400.0 + 600
				systemSetting.SetShopRangeView(distance)