//search
void initsystemSetting()

//add before
#ifdef ENABLE_IKASHOP_RENEWAL
#ifdef EXTEND_IKASHOP_PRO
PyObject* systemGetShopRangeView(PyObject* poSelf, PyObject* poArgs)
{
	return PyFloat_FromDouble(CPythonSystem::instance().GetShopRangeView());
}

PyObject* systemSetShopRangeView(PyObject* poSelf, PyObject* poArgs)
{
	float view = 0.0f;
	if(!PyTuple_GetFloat(poArgs, 0, &view))
		return Py_BadArgument();

	CPythonSystem::instance().SetShopRangeView(view);
	return Py_BuildNone();
}
#endif
#endif

//search
		{ "GetShadowLevel",				systemGetShadowLevel,			METH_VARARGS },
		{ "SetShadowLevel",				systemSetShadowLevel,			METH_VARARGS },

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
#ifdef EXTEND_IKASHOP_PRO
		{"GetShopRangeView",			systemGetShopRangeView,			METH_VARARGS },
		{"SetShopRangeView",			systemSetShopRangeView,			METH_VARARGS },
#endif
#endif