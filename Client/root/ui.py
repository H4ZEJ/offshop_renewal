#Inside "class TextLine(Window):"
#Add this, at the end:

	if app.ENABLE_IKASHOP_RENEWAL:
		def FitText(self):
			self.SetSize(*self.GetTextSize())
			
#Search for "class EditLine(TextLine):"
#Inside the "def __init__(self):"
#Search for "self.bCodePage = False"
#Add this, under:

		if app.ENABLE_IKASHOP_RENEWAL:
			self.updateEvent = None
			
			
#A bit below, search for:

	def SetTabEvent(self, event):
		self.eventTab = event

#Under it, add:

	if app.ENABLE_IKASHOP_RENEWAL:
		def SetUpdateEvent(self, event):
			self.updateEvent = event


#Search for:

	def OnSetFocus(self):
		Text = self.GetText()
		ime.SetText(Text)
		ime.SetMax(self.max)
		ime.SetUserMax(self.userMax)
		ime.SetCursorPosition(-1)
		if self.numberMode:
			ime.SetNumberMode()
		else:
			ime.SetStringMode()
		ime.EnableCaptureInput()
		if self.useIME:
			ime.EnableIME()
		else:
			ime.DisableIME()
		wndMgr.ShowCursor(self.hWnd, True)
		
# Under it, add:

		if app.ENABLE_IKASHOP_RENEWAL:
			if self.updateEvent:
				self.updateEvent(True)
				
# Search for:

	def OnKillFocus(self):
		self.SetText(ime.GetText(self.bCodePage))
		self.OnIMECloseCandidateList()
		self.OnIMECloseReadingWnd()
		ime.DisableIME()
		ime.DisableCaptureInput()
		wndMgr.HideCursor(self.hWnd)
		
# Under it, add:

		if app.ENABLE_IKASHOP_RENEWAL:
			if self.updateEvent:
				self.updateEvent(False)
				
# Seach for: 

	def OnIMEUpdate(self):
		snd.PlaySound("sound/ui/type.wav")
		TextLine.SetText(self, ime.GetText(self.bCodePage))
		
# Under it, add:

		if app.ENABLE_IKASHOP_RENEWAL:
			if self.updateEvent:
				self.updateEvent(self.IsFocus())
				
				
# Inside the "class ScrollBar(Window):"
# Search for:

	def UpdateBarSlot(self):
		self.barSlot.SetPosition(0, self.SCROLLBAR_BUTTON_HEIGHT)
		self.barSlot.SetSize(self.GetWidth() - 2, self.GetHeight() - self.SCROLLBAR_BUTTON_HEIGHT*2 - 2)
#Under it, add:

	if app.ENABLE_IKASHOP_RENEWAL:
		def UpdateScrollbarLenght(self, scrollableSpace):
			minheight = self.GetHeight() - self.downButton.GetHeight()*2
			scrollable = max(minheight, scrollableSpace)
			scale = float(scrollable) / max(1, minheight)
			scale = 1.0 / max(1.0, scale)
			scale = max(0.08, scale)
			self.SetScrollStep(scale/3.0)
			self.SetMiddleBarSize(scale)

#compare this class or replace
class ImageBox(Window):
	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

		self.eventDict={}
		self.eventFunc = {"mouse_click" : None, "mouse_over_in" : None, "mouse_over_out" : None}
		self.eventArgs = {"mouse_click" : None, "mouse_over_in" : None, "mouse_over_out" : None}

	def __del__(self):
		Window.__del__(self)
		self.eventFunc = None
		self.eventArgs = None

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterImageBox(self, layer)

	def LoadImage(self, imageName):
		self.name=imageName
		wndMgr.LoadImage(self.hWnd, imageName)

		if len(self.eventDict)!=0:
			print("LOAD IMAGE", self, self.eventDict)

	def SetAlpha(self, alpha):
		wndMgr.SetDiffuseColor(self.hWnd, 1.0, 1.0, 1.0, alpha)

	def GetWidth(self):
		return wndMgr.GetWidth(self.hWnd)

	def GetHeight(self):
		return wndMgr.GetHeight(self.hWnd)

	def OnMouseOverIn(self):
		try:
			self.eventDict["MOUSE_OVER_IN"]()
		except KeyError:
			pass

	def OnMouseOverOut(self):
		try:
			self.eventDict["MOUSE_OVER_OUT"]()
		except KeyError:
			pass

	def SAFE_SetStringEvent(self, event, func, *args):
		self.eventDict[event]=__mem_func__(func)

	def SetEvent(self, func, *args) :
		result = self.eventFunc.has_key(args[0])
		if result :
			self.eventFunc[args[0]] = func
			self.eventArgs[args[0]] = args
		else :
			print("[ERROR] ui.py SetEvent, Can`t Find has_key : %s" % args[0])

	def OnMouseLeftButtonUp(self) :
		if app.ENABLE_IKASHOP_RENEWAL:
			super(ImageBox, self).OnMouseLeftButtonUp()
		if self.eventFunc["mouse_click"] :
			apply(self.eventFunc["mouse_click"], self.eventArgs["mouse_click"])

	def OnMouseOverIn(self) :
		if self.eventFunc["mouse_over_in"] :
			apply(self.eventFunc["mouse_over_in"], self.eventArgs["mouse_over_in"])
		else:
			try:
				self.eventDict["MOUSE_OVER_IN"]()
			except KeyError:
				pass

	def OnMouseOverOut(self) :
		if self.eventFunc["mouse_over_out"] :
			apply(self.eventFunc["mouse_over_out"], self.eventArgs["mouse_over_out"])
		else :
			try:
				self.eventDict["MOUSE_OVER_OUT"]()
			except KeyError:
				pass