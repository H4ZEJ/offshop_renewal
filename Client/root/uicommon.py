# Search for:

class PopupDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		

#Under it, add:

		if app.ENABLE_IKASHOP_RENEWAL:
			self.autocloseTime = None


#Search for:

	def OnIMEReturn(self):
		self.Close()
		return True
		
		
#Under it, add:

	if app.ENABLE_IKASHOP_RENEWAL:
		def SetAutoClose(self, seconds):
			self.autocloseTime = app.GetTime() + seconds

		def OnUpdate(self):
			if self.autocloseTime is not None:
				remainTime = max(0, self.autocloseTime - app.GetTime())
				if remainTime == 0:
					self.autocloseTime = None
					self.accceptButton.SetText(uiScriptLocale.OK)
					self.Close()
					return
				self.accceptButton.SetText(uiScriptLocale.OK + " ({:0.1f} s)".format(remainTime))
				
				
				
#Inside "class MoneyInputDialog(ui.ScriptWindow):"
#Search for: 

		self.inputValue.SetNumberMode()
		self.inputValue.OnIMEUpdate = ui.__mem_func__(self.__OnValueUpdate)
		self.moneyText = getObject("MoneyValue")
		
#Under them, add:

		if app.ENABLE_IKASHOP_RENEWAL:
			self.inputValue.numberMode = False
			if app.EXTEND_IKASHOP_ULTIMATE:
				self.priceAverage = None
				
				
#Search for:

	def Open(self):
		self.inputValue.SetText("")
		self.inputValue.SetFocus()
		self.__OnValueUpdate()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()
		
		
#Under it, add:

	if app.ENABLE_IKASHOP_RENEWAL:
		if app.EXTEND_IKASHOP_ULTIMATE:
			def SetPriceAverage(self, price):
				if not self.priceAverage:
					self.priceAverage = ui.TextLine()
					self.priceAverage.SetParent(self)
					self.priceAverage.SetPosition(self.GetWidth()/2, 30)
					self.priceAverage.SetHorizontalAlignCenter()
					self.priceAverage.Show()

					for child in self.board.Children:
						cx, cy = child.GetLocalPosition()
						child.SetPosition(cx, cy+15)
					self.SetSize(self.GetWidth(), self.GetHeight() + 15)
					self.board.SetSize(self.board.GetWidth(), self.board.GetHeight() + 15)

				if price == -1:
					self.priceAverage.SetText(localeInfo.IKASHOP_ULTIMATE_PRICE_AVERAGE_REQUESTING)
				elif price == 0:
					self.priceAverage.SetText(localeInfo.IKASHOP_ULTIMATE_PRICE_AVERAGE_NOT_AVAILABLE)
				else:
					self.priceAverage.SetText(localeInfo.IKASHOP_ULTIMATE_PRICE_AVERAGE_VALUE.format(localeInfo.NumberToMoneyString(price)))
					
					
#Search for:

	def Close(self):
		self.ClearDictionary()
		if app.ENABLE_CHEQUE_SYSTEM:
			self.inputChequeValue = None
		self.board = None
		self.acceptButton = None
		self.cancelButton = None
		self.inputValue = None
		
		
#Under it, add:

		if app.ENABLE_IKASHOP_RENEWAL:
			if app.EXTEND_IKASHOP_ULTIMATE:
				if self.priceAverage:
					self.priceAverage.Destroy()
				self.priceAverage = None


#Search for:

	def SetValue(self, value):
		value=str(value)
		
#Above "value", add:

		if app.ENABLE_IKASHOP_RENEWAL:
			if value == "":
				self.inputValue.SetText("")
				self.__OnValueUpdate()
				ime.SetCursorPosition(0)
				return
				
				
#Under the "def SetValue"
#Add:

	if app.ENABLE_IKASHOP_RENEWAL:
		def SoftClose(self):
			self.Hide()
			for child in vars(self).values():
				if isinstance(child, ui.EditLine):
					if child.IsFocus():
						child.KillFocus()
						
						
						
### SKIP IF YOU DON'T HAVE THIS ###

#Search for:

	if app.ENABLE_CHEQUE_SYSTEM:
		def HideCheque(self):
			slotCheque = self.GetChild("InputSlot_Cheque")
			slotYang = self.GetChild("InputSlot")
			if slotCheque and slotCheque.IsShow():
				slotCheque.Hide()
				slotYang.SetPosition(0, slotYang.GetLocalPosition()[1])
				slotYang.SetWindowHorizontalAlignCenter()
				self.chequeText.Hide()
				self.GetChild("SellInfoText").Hide()
				self.moneyText.SetPosition(0, 59)
				self.acceptButton.SetPosition(self.acceptButton.GetLocalPosition()[0], 78)
				self.cancelButton.SetPosition(self.cancelButton.GetLocalPosition()[0], 78)
				
				
# Under it, add:

				if app.ENABLE_IKASHOP_RENEWAL:
					if app.EXTEND_IKASHOP_ULTIMATE:
						self.SetSize(200, 110 if not self.priceAverage else 125)
						self.board.SetSize(200, 110 if not self.priceAverage else 125)

						if self.priceAverage:
							self.moneyText.SetPosition(0, 59 + 15)
							self.acceptButton.SetPosition(self.acceptButton.GetLocalPosition()[0], 78 + 15)
							self.cancelButton.SetPosition(self.cancelButton.GetLocalPosition()[0], 78 + 15)
					else:
						self.SetSize(200, 110)
						self.board.SetSize(200, 110)
				else:
	
					self.SetSize(200, 110)
					self.board.SetSize(200, 110)
					
### SKIP IF YOU DON'T HAVE THIS ###

# Search for:

		def ShowCheque(self):
			slotCheque = self.GetChild("InputSlot_Cheque")
			slotYang = self.GetChild("InputSlot")
			if not slotCheque.IsShow():
				slotCheque.Show()
				slotYang.SetPosition(90, slotYang.GetLocalPosition()[1])
				slotYang.SetWindowHorizontalAlignLeft()
				self.chequeText.Show()
				self.GetChild("SellInfoText").Show()
				self.moneyText.SetPosition(0, 100)
				self.acceptButton.SetPosition(self.acceptButton.GetLocalPosition()[0], 118)
				self.cancelButton.SetPosition(self.cancelButton.GetLocalPosition()[0], 118)
				
# Under it, add:

				if app.ENABLE_IKASHOP_RENEWAL:
					if app.EXTEND_IKASHOP_ULTIMATE:
						self.SetSize(200, 150 if not self.priceAverage else 165)
						self.board.SetSize(200, 150 if not self.priceAverage else 165)

						if self.priceAverage:
							self.moneyText.SetPosition(0, 100 + 15)
							self.acceptButton.SetPosition(self.acceptButton.GetLocalPosition()[0], 118 + 15)
							self.cancelButton.SetPosition(self.cancelButton.GetLocalPosition()[0], 118 + 15)
					else:
						self.SetSize(200, 150)
						self.board.SetSize(200, 150)
				else:
	
					self.SetSize(200, 150)
					self.board.SetSize(200, 150)
					


#Search for:

		def SetCheque(self, cheque):

			cheque=str(cheque)
			
# Above "cheque", add:

			if app.ENABLE_IKASHOP_RENEWAL:
				if cheque == "":
					self.inputChequeValue.SetText(cheque)
					self.__OnValueUpdate()
					ime.SetCursorPosition(0)
					return
					
					
# Search for:

		def __OnValueUpdate(self):
			if self.inputValue.IsFocus():
				ui.EditLine.OnIMEUpdate(self.inputValue)
			elif self.inputChequeValue.IsFocus():
				ui.EditLine.OnIMEUpdate(self.inputChequeValue)
			else:
				pass

			text = self.inputValue.GetText()
			cheque_text = self.inputChequeValue.GetText()


#Add:

			if app.ENABLE_IKASHOP_RENEWAL:
				text = text.lower().replace("k", "000")
				
#