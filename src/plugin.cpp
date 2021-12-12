#include "plugin.h"
#include <fstream>

using namespace Microsoft::WRL;
wchar_t userFolder[MAX_PATH];
wchar_t pathToTypeLib[MAX_PATH];

wil::com_ptr<ITypeLib> g_typeLib;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		// HostObject implementation requires COM TypeLib file to load interface type
		// Since LoadTypeLib can't load embedded resource, we need to write type data 
		// into a file in temp directory.
		// TypeLib file binary data is embedded into DLL and can be retrieved using LoadResource
		GetTempPath(MAX_PATH, userFolder);
		wcscat(userFolder, L"RmWebViewUserFolder");

		GetTempPath(MAX_PATH, pathToTypeLib);
		wcscat(pathToTypeLib, L"PluginWebView.tlb");

		// Read embedded resource: ID = 1, Type = TYPELIB
		HRSRC hResInfo = FindResource(hinstDLL, MAKEINTRESOURCE(1), L"TYPELIB");
		HGLOBAL hRes = LoadResource(hinstDLL, hResInfo);
		LPVOID memRes = LockResource(hRes);
		DWORD sizeRes = SizeofResource(hinstDLL, hResInfo);

		HANDLE hFile = CreateFile(pathToTypeLib, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD dwWritten = 0;
		WriteFile(hFile, memRes, sizeRes, &dwWritten, NULL);
		CloseHandle(hFile);
	
		// TODO: Error handling
		LoadTypeLib(pathToTypeLib, &g_typeLib);
		break;
	}
	}

	return TRUE;
}

void MeasureWebView::Reload()
{
	typeLib = g_typeLib;
	skin = RmGetSkin(rm);
	hwnd = RmGetSkinWindow(rm);

	url = std::wstring(RmReadString(rm, L"URL", L"", 1));
	script = std::wstring(RmReadPath(rm, L"ScriptFile", L""));
	if (url.length() > 0)
	{
		type = WEBPAGE;
	}
	else if (script.length() > 0)
	{
		type = SCRIPT;
	}
	else
	{
		type = INVALID;
	}

	x = RmReadInt(rm, L"X", 0);
	y = RmReadInt(rm, L"Y", 0);
	w = RmReadInt(rm, L"W", 0);
	h = RmReadInt(rm, L"H", 0);
}

bool MeasureWebView::Update()
{
	if (view != nullptr && type == SCRIPT) {
		view->ExecuteScript(
			L"Update ? Update() : ''",
			Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
				[this](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
					scriptOut = std::wstring(resultObjectAsJson);
					// Trim double quotes
					if (scriptOut.front() == '"') {	
						scriptOut.erase(0, 1);
						scriptOut.erase(scriptOut.size() - 1);
					}
					return S_OK;
				}
			).Get()
		);
		return true;
	}

	return false;
}

HRESULT MeasureWebView::CreateEnvironmentHandler(HRESULT result, ICoreWebView2Environment* env)
{
	env->CreateCoreWebView2Controller(
		hwnd,
		Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
			this,
			&MeasureWebView::CreateControllerHandler
		).Get()
	);

	return S_OK;
}

HRESULT MeasureWebView::CreateControllerHandler(HRESULT result, ICoreWebView2Controller* ctl)
{
	if (ctl != nullptr)
	{
		controller = ctl;
		controller->get_CoreWebView2(&view);
	}
	else
	{
		return S_FALSE;
	}

	// Settings for the webview
	ICoreWebView2Settings* settings;
	view->get_Settings(&settings);
	settings->put_IsScriptEnabled(TRUE);
	settings->put_AreDefaultScriptDialogsEnabled(TRUE);
	settings->put_IsWebMessageEnabled(TRUE);
	settings->put_AreHostObjectsAllowed(TRUE);

	if (type == WEBPAGE)
	{
		// Resize WebView to fit the bounds of the parent window
		RECT bounds;
		GetClientRect(hwnd, &bounds);
		bounds.left = x;
		bounds.top = y;
		if (w > 0)
		{
			bounds.right = x + w;
		}
		if (h > 0)
		{
			bounds.bottom = y + h;
		}
		controller->put_Bounds(bounds);

		// Transparent background
		auto controller2 = controller.query<ICoreWebView2Controller2>();
		COREWEBVIEW2_COLOR m_webViewColor = { 0, 0, 0, 0 };
		controller2->put_DefaultBackgroundColor(m_webViewColor);

		RmLog(LOG_DEBUG, L"Navigate to:");
		RmLog(LOG_DEBUG, url.c_str());
		view->Navigate(url.c_str());
		//view->OpenDevToolsWindow();

		hostObject = Microsoft::WRL::Make<HostObjectRmAPI>(this);

		// Add Host Object exposing Rainmeter API to environment
		VARIANT variant = {};
		hostObject.query_to<IDispatch>(&variant.pdispVal);
		variant.vt = VT_DISPATCH;
		view->AddHostObjectToScript(L"RainmeterAPI", &variant);
		variant.pdispVal->Release();

		// Script to add global methods
		view->AddScriptToExecuteOnDocumentCreated(
			L"RainmeterAPI = chrome.webview.hostObjects.sync.RainmeterAPI",
			nullptr
		);

		return S_OK;
	}

	if (type == SCRIPT)
	{
		controller->put_Bounds(RECT{ 0, 0, 0, 0 });

		std::wifstream input1(script);
		std::wstring content((std::istreambuf_iterator<wchar_t>(input1)), {});
		RmLog(LOG_WARNING, content.c_str());
		// TODO: Syntax Error handling
		view->ExecuteScript(content.c_str(), nullptr);
		return S_OK;
	}
	return S_OK;
}

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	MeasureWebView* measure = new MeasureWebView(rm);
	*data = measure;

	LPWSTR versionInfo = nullptr;
	GetAvailableCoreWebView2BrowserVersionString(nullptr, &versionInfo);
	if (versionInfo == nullptr)
	{
		auto command = MessageBox(
			nullptr,
			L"Click OK to get one from the darkweb",
			L"WebView2 runtime not found",
			MB_OKCANCEL | MB_ICONWARNING
		);

		if (command == IDOK)
		{
			ShellExecute(0, 0, L"https://go.microsoft.com/fwlink/p/?LinkId=2124703", 0, 0, SW_SHOW);
		}
	}
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	MeasureWebView* measure = (MeasureWebView*)data;
	measure->Reload();

	// Initiate WebView2
	CreateCoreWebView2EnvironmentWithOptions(nullptr, userFolder, nullptr,
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			measure,
			&MeasureWebView::CreateEnvironmentHandler
		).Get()
	);
}

PLUGIN_EXPORT double Update(void* data)
{
	MeasureWebView* measure = (MeasureWebView*)data;
	if (measure->Update()) {
		return wcstod(measure->scriptOut.c_str(), nullptr);
	}
	return 0.0;
}

PLUGIN_EXPORT LPCWSTR GetString(void* data)
{
	MeasureWebView* measure = (MeasureWebView*)data;
	return measure->scriptOut.c_str();
}

PLUGIN_EXPORT void Finalize(void* data)
{
	MeasureWebView* measure = (MeasureWebView*)data;
	delete measure;
}
