import ui
import player
import net
import wndMgr
import uiScriptLocale

DECO_SHOP_MODEL_LIST = [
	["Default", 30000],
	["1001 Nights", 30002],
	["BBQ King", 30003],
	["Treasure Hunter", 30004],
	["Frugal Flame", 30005],
	["Arms Dealer", 30006],
	["Armour Dealership", 30007],
	["Alchemist", 30008]
]

DECO_SHOP_TITLE_LIST = [
	["Inferno", "d:/ymir work/ui/pattern/myshop/fire/p_fire", "0xFF976D"],
	["Ice Age", "d:/ymir work/ui/pattern/myshop/ice/p_ice", "0xF9F4FF"],
	["Parchment", "d:/ymir work/ui/pattern/myshop/paper/p_paper", "0xFFBF8B"],
	["War Banner", "d:/ymir work/ui/pattern/myshop/ribon/p_ribon", "0xFFECD1"],
	["Wings", "d:/ymir work/ui/pattern/myshop/wing/p_wing", "0xF1ECE1"],
]


class ShopDecoTitle(ui.Window):
	DEFAULT_VALUE = 16
	CORNER_WIDTH = 48
	CORNER_HEIGHT = 32
	LINE_WIDTH = 16
	LINE_HEIGHT = 32

	LT = 0
	LB = 1
	RT = 2
	RB = 3
	L = 0
	R = 1
	T = 2
	B = 3

	def __init__(self, type, layer = "UI"):
		ui.Window.__init__(self, layer)
		
		CornerFileNames, LineFileNames = self.__GetFilePath(type)
		
		if CornerFileNames == None or LineFileNames == None :
			return

		self.Corners = []
		for fileName in CornerFileNames:
			Corner = ui.ExpandedImageBox()
			Corner.AddFlag("attach")
			Corner.AddFlag("not_pick")
			Corner.LoadImage(fileName)
			Corner.SetParent(self)
			Corner.SetPosition(0, 0)
			Corner.Show()
			self.Corners.append(Corner)

		self.Lines = []
		for fileName in LineFileNames:
			Line = ui.ExpandedImageBox()
			Line.AddFlag("attach")
			Line.AddFlag("not_pick")
			Line.LoadImage(fileName)
			Line.SetParent(self)
			Line.SetPosition(0, 0)
			Line.Show()
			self.Lines.append(Line)
			
	def __del__(self):
		ui.Window.__del__(self)
		
	def __GetFilePath(self, type):		
		CornerFileNames = [ DECO_SHOP_TITLE_LIST[type][1]+"_"+dir+".tga" for dir in ["left_top","left_bottom","right_top","right_bottom"] ]
		LineFileNames = [ DECO_SHOP_TITLE_LIST[type][1]+"_"+dir+".tga" for dir in ["left","right","top","bottom"] ]
		
		return CornerFileNames, LineFileNames

	def SetSize(self, width, height):
		width = max(self.DEFAULT_VALUE*2, width)
		height = max(self.DEFAULT_VALUE*2, height)
		ui.Window.SetSize(self, width, height)
		
		self.Corners[self.LT].SetPosition(-self.CORNER_WIDTH + self.DEFAULT_VALUE, -self.CORNER_HEIGHT + self.DEFAULT_VALUE)
		self.Corners[self.LB].SetPosition(-self.CORNER_WIDTH + self.DEFAULT_VALUE, height - self.CORNER_HEIGHT + self.DEFAULT_VALUE)
		
		self.Corners[self.RT].SetPosition(width - self.DEFAULT_VALUE, -self.CORNER_HEIGHT + self.DEFAULT_VALUE)
		self.Corners[self.RB].SetPosition(width - self.DEFAULT_VALUE, height - self.CORNER_HEIGHT + self.DEFAULT_VALUE)
		
		self.Lines[self.L].SetPosition(0, self.DEFAULT_VALUE)
		self.Lines[self.R].SetPosition(width - self.DEFAULT_VALUE, self.DEFAULT_VALUE)
		self.Lines[self.B].SetPosition(self.DEFAULT_VALUE, height - self.LINE_HEIGHT + self.DEFAULT_VALUE)
		self.Lines[self.T].SetPosition(self.DEFAULT_VALUE, -self.LINE_HEIGHT + self.DEFAULT_VALUE)
		
		verticalShowingPercentage = float((height - self.DEFAULT_VALUE*2) - self.DEFAULT_VALUE) / self.DEFAULT_VALUE
		horizontalShowingPercentage = float((width - self.DEFAULT_VALUE*2) - self.DEFAULT_VALUE) / self.DEFAULT_VALUE
		
		self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)

	def ShowInternal(self):
		for wnd in self.Lines:
			wnd.Show()
		for wnd in self.Corners:
			wnd.Show()

	def HideInternal(self):
		for wnd in self.Lines:
			wnd.Hide()
		for wnd in self.Corners:
			wnd.Hide()

class DecoObj :
	def __init__(self, name, info, parent = None) :
		self.name = name
		
		if parent == None:
			self.info = info
		else:
			TITLE_VIEW_WIDTH = 190
			X = 60
			self.info = info
			self.info.SetParent(parent)
			self.info.SetPosition(X, 30)
			self.info.SetSize(TITLE_VIEW_WIDTH - 2*X, 32)
			self.info.Hide()
		
	def __del__(self):
		self.name = None
		self.info = None
	
	def GetName(self):
		return self.name
		
	def GetInfo(self):
		return self.info


class MyShopDecoration(ui.ScriptWindow):
	
	MODE_MODEL_VIEW = 1
	MODE_TITLE_VIEW = 2
	
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		
	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def Open(self):	
		self.max_pos_x = wndMgr.GetScreenWidth() - 402
		self.max_pos_y = wndMgr.GetScreenHeight() - 320
		
		self.ViewMode = self.MODE_MODEL_VIEW
		self.PolyVnum = -1
		self.TitleType = -1
		
		self.LeftButtonMax = 10
		self.ModelBtnList = []
		
		self.DecoObjList = []
		self.ShopModelList = []
		self.ShopTitleList = []
				
		self.__LoadScript("MyShopDecorationWindow.py")
		
		self.LeftBoard = self.GetChild("LeftBoard")
		self.ViewModelName = self.GetChild("ModelName")
		self.ScrollBar = self.GetChild("ScrollBar")
		self.TitleBar = self.GetChild("TitleName")
		
		self.NextBtn = self.GetChild("NextButton")
		self.CancelBtn = self.GetChild("CancelButton")
		self.PrevBtn = self.GetChild("PrevButton")
		self.CompleteBtn = self.GetChild("CompleteButton")
		self.RenderTarget = self.GetChild("RenderTarget")
				
		self.__CreateButton()
		self.__CreateEvent()
		
		self.PrevBtn.Hide()
		self.CompleteBtn.Hide()
		
		self.__LoadModelInfo()
		self.__ShopModelListSetting()
		
		self.__RefreshLeftBoard()
		player.MyShopDecoShow( True )
		
		ui.ScriptWindow.Show(self)
	
	def Close(self):
		self.ModelBtnList = []
		self.DecoObjList = []
		
		player.MyShopDecoShow( False )
		
		self.Hide()
		
	def OnPressEscapeKey(self):
		self.Close()
		
	def __LoadScript(self, FileName) :
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/" + FileName)
		except:
			import exception
			exception.Abort("UIScript/MyShopDecorationWindow.py Load Fail")
			
	def __RefreshLeftBoard(self):
		self.SelectBtnIdx = 0
		self.Diff = 0
		self.ScollPos = 0
		
		self.Diff = len(self.DecoObjList) - self.LeftButtonMax
				
		btnNum = self.LeftButtonMax
		btnGap = 6
		
		if self.Diff <= 0 : # 1 ~ 10??
			btnNum = len(self.DecoObjList)
			btnGap = 0
			self.ScrollBar.Hide()
		else :
			stepSize = 1.0 / self.Diff
			self.ScrollBar.SetScrollStep( stepSize )
			self.ScrollBar.Show()	
			
		for i  in xrange(self.LeftButtonMax) :
			if btnNum > i:
				x = 13 - btnGap
				y = 6 + i*26
				self.ModelBtnList[i].SetPosition(x, y)
				self.ModelBtnList[i].SetText(self.DecoObjList[i].GetName())
				self.ModelBtnList[i].Show()
			else:
				self.ModelBtnList[i].Hide()
				
		self.SelectButton(0)
		
	def __CreateButton(self):	
		for i in xrange(self.LeftButtonMax) :
			btn = ui.Button()
			btn.SetParent(self.LeftBoard)
			btn.SetPosition(13, 6 + i*26)
			btn.SetUpVisual("d:/ymir work/ui/game/myshop_deco/select_btn_01.sub")
			btn.SetOverVisual("d:/ymir work/ui/game/myshop_deco/select_btn_02.sub")
			btn.SetDownVisual("d:/ymir work/ui/game/myshop_deco/select_btn_03.sub")
			btn.Hide()
			
			self.ModelBtnList.append(btn)
		
	def __CreateEvent(self) :
		self.ScrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
		self.GetChild("MyShopTitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
		
		self.NextBtn.SetEvent(ui.__mem_func__(self.UseNextButton))
		self.CancelBtn.SetEvent(ui.__mem_func__(self.Close))
		self.PrevBtn.SetEvent(ui.__mem_func__(self.UsePrevButton))
		self.CompleteBtn.SetEvent(ui.__mem_func__(self.UseCompleteButton))
		
		for i in xrange(len(self.ModelBtnList)) :
			self.ModelBtnList[i].SetEvent(ui.__mem_func__(self.SelectButton), i)
	
	def SelectButton(self, idx) :
		self.ViewModelName.SetText(self.DecoObjList[idx + self.ScollPos].GetName())
		
		for btn in self.ModelBtnList :
			btn.SetUp()
			btn.Enable()
			
		self.ModelBtnList[idx].Down()
		self.ModelBtnList[idx].Disable()
		
		self.SelectBtnIdx = (idx + self.ScollPos)
		
		if self.ViewMode == self.MODE_MODEL_VIEW :	
			self.SelectShopModel(self.SelectBtnIdx)
		elif self.ViewMode == self.MODE_TITLE_VIEW :
			self.SelectTitleModel(self.SelectBtnIdx)
		else:	
			self.PolyVnum = -1
			self.TitleType = -1
		
	def OnScroll(self) : 
		self.ScollPos = int(self.ScrollBar.GetPos() * self.Diff)
			
		for i in xrange(len(self.ModelBtnList)) :
			self.ModelBtnList[i].SetText( self.DecoObjList[i + self.ScollPos].GetName() )
			
		for btn in self.ModelBtnList :
			btn.SetUp()
			btn.Enable()
			
		idx = self.SelectBtnIdx - self.ScollPos 
		
		if self.LeftButtonMax > idx >=0:
			self.ModelBtnList[idx].Down()
			self.ModelBtnList[idx].Disable()
			
	def UseNextButton(self) :
		self.NextBtn.Hide()
		self.CancelBtn.Hide()
		self.PrevBtn.Show()
		self.CompleteBtn.Show()
		self.TitleBar.SetText("Choose your name's appearance.")		
		self.ViewMode = self.MODE_TITLE_VIEW
				
		self.__ShopTitleListSetting()
		self.__RefreshLeftBoard()
		
	def UsePrevButton(self) :
		self.NextBtn.Show()
		self.CancelBtn.Show()
		self.PrevBtn.Hide()
		self.CompleteBtn.Hide()
		self.TitleBar.SetText("Choose your shop's appearance.")
		self.ViewMode = self.MODE_MODEL_VIEW
		
		self.__AllShopTitleHide()
		self.__ShopModelListSetting()
		self.__RefreshLeftBoard()
			
	def UseCompleteButton(self) :	
		if self.PolyVnum < 0 or self.TitleType < 0 :
			return 
	
		self.Close()
		
	def SelectShopModel(self, idx) :
		PolyVnum = self.DecoObjList[idx].GetInfo()
		self.PolyVnum = PolyVnum
		player.SelectShopModel( PolyVnum )	
		
	def SelectTitleModel(self, idx) :
		self.TitleType = idx
		
		for i in xrange(len(self.DecoObjList)):
			if i == idx :
				self.DecoObjList[i].GetInfo().Show()
			else:
				self.DecoObjList[i].GetInfo().Hide()
		
	def __LoadModelInfo(self) :	
		for l in DECO_SHOP_MODEL_LIST:
			self.ShopModelList.append(DecoObj(l[0], l[1]))
			
		self.ShopTitleList.append(DecoObj("Default",	ui.ThinBoard(),			self.RenderTarget))
		for i in xrange(len(DECO_SHOP_TITLE_LIST)):
			self.ShopTitleList.append(DecoObj(DECO_SHOP_TITLE_LIST[i][0], ShopDecoTitle(i), self.RenderTarget))
		
	def __AllShopTitleHide(self) :
		for i in xrange(len(self.DecoObjList)):
			self.DecoObjList[i].GetInfo().Hide()		
				
	def __ShopModelListSetting(self) :
		self.DecoObjList = self.ShopModelList
	
	def __ShopTitleListSetting(self) :
		self.DecoObjList = self.ShopTitleList
		
	def MINMAX(self, min, value, max):
		
		if value < min:
			return min
		elif value > max:
			return max
		else:
			return value
		
	def OnUpdate(self):
		x, y = self.GetGlobalPosition()
		
		pos_x = self.MINMAX(0, x, self.max_pos_x)
		pos_y = self.MINMAX(0, y, self.max_pos_y)
		
		self.SetPosition(pos_x, pos_y)

try:
	myshop = MyShopDecoration()
	myshop.Open()
except Exception as e:
	print e