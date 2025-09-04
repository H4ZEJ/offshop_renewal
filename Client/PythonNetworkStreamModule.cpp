//search and replace

PyObject* netSetTCPRecvBufferSize(PyObject* poSelf, PyObject* poArgs)
{
	int bufSize;
	if (!PyTuple_GetInteger(poArgs, 0, &bufSize))
		return Py_BuildException();
#ifdef ENABLE_LARGE_DYNAMIC_PACKETS
	bufSize = MAX(bufSize, (600 * 1024));
#endif
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SetRecvBufferSize(bufSize);
	return Py_BuildNone();
}