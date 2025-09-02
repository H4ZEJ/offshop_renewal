# Search for:

if app.ENABLE_LOCALE_COMMON:
	def TryLoadLocaleFile(filename):
		if pack.Exist(filename) or os.path.exists(filename):
			LoadLocaleFile(filename, globals())
	TryLoadLocaleFile("locale/common/locale_game_ex.txt")
	TryLoadLocaleFile("%s/locale_game_ex.txt" % app.GetLocalePath())
	
#Under it, add:

	if app.ENABLE_IKASHOP_RENEWAL:
		TryLoadLocaleFile("locale/common/locale_game_ikashop.txt")