#pragma once
#include "common_h.h"
#include "HostObjectRmAPI/HostObjectRmAPI.h"

using namespace Microsoft::WRL;

class MeasureWebView
{
public:
	MeasureWebView(void* rm) : rm(rm) {};

	void* rm = nullptr;
	wil::com_ptr<ITypeLib> typeLib;
	std::wstring scriptOut;

	void Reload();
	bool Update();
	HRESULT CreateEnvironmentHandler(HRESULT result, ICoreWebView2Environment* env);
	HRESULT CreateControllerHandler(HRESULT result, ICoreWebView2Controller* controller);

private:
	enum Type {
		INVALID,
		WEBPAGE,
		SCRIPT
	};

	void* skin = nullptr;
	HWND hwnd = NULL;
	std::wstring url;
	std::wstring script;
	Type type = INVALID;
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	wil::com_ptr<ICoreWebView2Controller> controller;
	wil::com_ptr<ICoreWebView2> view;
	wil::com_ptr<HostObjectRmAPI> hostObject;
};