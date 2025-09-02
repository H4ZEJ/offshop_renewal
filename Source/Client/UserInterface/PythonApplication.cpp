//search in function @@@ void CPythonApplication::RenderGame()
		m_kEftMgr.Render();

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
		m_pyOfflineshop.RenderEntities();
#endif

//search in function @@@ void CPythonApplication::UpdateGame()
	m_GameEventManager.SetCenterPosition(kPPosMainActor.x, kPPosMainActor.y, kPPosMainActor.z);
	m_GameEventManager.Update();

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
	m_pyOfflineshop.UpdateEntities();
#endif

//search
void CPythonApplication::Destroy()
{
	WebBrowser_Destroy();

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
	CPythonIkarusShop::instance().DeleteEntities();
#endif