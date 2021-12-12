// Copyright (C) 2022 khanhas. All rights reserved.

#include "HostObjectRmAPI.h"
#include "plugin.h"

HostObjectRmAPI::HostObjectRmAPI(MeasureWebView* mwv)
    : plugin(mwv), rm(plugin->rm), skin((Skin*)RmGetSkin(rm))
{
}

STDMETHODIMP HostObjectRmAPI::GetMeasure(BSTR name, HostObjectMeasure** measureObj)
{
    Measure* target = nullptr;
    for (auto m : skin->GetMeasures())
    {
        if (_wcsicmp(name, m->GetName()) == 0)
        {
            target = m;
            break;
        }
    }
    if (target == nullptr) {
        RmLog(LOG_ERROR, L"Cant find measure");
        measureObj = nullptr;
        return S_FALSE;
    }

    wil::com_ptr<HostObjectMeasure> host = Microsoft::WRL::Make<HostObjectMeasure>(plugin, target);
    host.copy_to(measureObj);
    return S_OK;
}

STDMETHODIMP HostObjectRmAPI::GetMeter(BSTR name, HostObjectMeter** meterObj)
{
    Meter* target = nullptr;
    for (auto m : skin->GetMeters())
    {
        if (_wcsicmp(name, m->GetName()) == 0)
        {
            target = m;
            break;
        }
    }
    if (target == nullptr) {
        RmLog(LOG_ERROR, L"Cant find measure");
        meterObj = nullptr;
        return S_FALSE;
    }

    wil::com_ptr<HostObjectMeter> host = Microsoft::WRL::Make<HostObjectMeter>(plugin, target);
    host.copy_to(meterObj);
    return S_OK;
}

STDMETHODIMP HostObjectRmAPI::GetVariable(BSTR stringParameter, BSTR* stringResult)
{
    std::wstring name(stringParameter);
    name = L"#" + name + L"#";
    std::wstring value(RmReplaceVariables(rm, name.c_str()));
    *stringResult = SysAllocString(value.c_str());

    return S_OK;
}

STDMETHODIMP HostObjectRmAPI::GetOption(BSTR optionName, BOOL replaceVariables, BSTR* stringResult)
{
    std::wstring name(optionName);
    std::wstring value(RmReadString(rm, name.c_str(), nullptr, replaceVariables));
    *stringResult = SysAllocString(value.c_str());

    return S_OK;
}

STDMETHODIMP HostObjectRmAPI::Bang(BSTR stringParameter)
{
    std::wstring bang(stringParameter);
    RmExecute(RmGetSkin(rm), bang.c_str());

    return S_OK;
}


STDMETHODIMP HostObjectRmAPI::Log(BSTR stringParameter)
{
    std::wstring message(stringParameter);
    RmLog(LOG_NOTICE, message.c_str());

    return S_OK;
}

STDMETHODIMP HostObjectRmAPI::GetTypeInfoCount(UINT* pctinfo)
{
    *pctinfo = 1;
    return S_OK;
}

STDMETHODIMP HostObjectRmAPI::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
    if (0 != iTInfo || !plugin->typeLib)
    {
        return TYPE_E_ELEMENTNOTFOUND;
    }
    return plugin->typeLib->GetTypeInfoOfGuid(__uuidof(IHostObjectRmAPI), ppTInfo);
}

STDMETHODIMP HostObjectRmAPI::GetIDsOfNames(
    REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
    wil::com_ptr<ITypeInfo> typeInfo;
    RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
    return typeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
}

STDMETHODIMP HostObjectRmAPI::Invoke(
    DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
    VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    wil::com_ptr<ITypeInfo> typeInfo;
    RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
    return typeInfo->Invoke(
        this, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}
