// Copyright (C) 2022 khanhas. All rights reserved.

#include "common_h.h"

#include "HostObjectMeter/HostObjectMeter.h"
#include "plugin.h"

HostObjectMeter::HostObjectMeter(MeasureWebView* mwv, Meter* meter)
    : plugin(mwv), rm(mwv->rm), meter(meter)
{
}

STDMETHODIMP HostObjectMeter::GetOption(BSTR optionName, BOOL replaceVariables, BSTR* stringResult)
{
    std::wstring name(optionName);
    std::wstring value(RmReadString(meter, name.c_str(), nullptr, replaceVariables));
    *stringResult = SysAllocString(value.c_str());

    return S_OK;
}

STDMETHODIMP HostObjectMeter::Update()
{
    meter->Update();
    return S_OK;
}

STDMETHODIMP HostObjectMeter::get_Hidden(BOOL* isHidden)
{
    *isHidden = meter->IsHidden();
    return S_OK;
}

STDMETHODIMP HostObjectMeter::put_Hidden(BOOL isHidden)
{
    std::wstring command(isHidden ? L"!HideMeter " : L"!ShowMeter ");
    command += L"\"" + std::wstring(meter->GetName()) + L"\"";
    RmLog(LOG_WARNING, command.c_str());
    RmExecute(RmGetSkin(rm), command.c_str());
    return S_OK;
}

STDMETHODIMP HostObjectMeter::get_DynamicVariables(BOOL* hasDV)
{
    *hasDV = meter->HasDynamicVariables();
    return S_OK;
}

STDMETHODIMP HostObjectMeter::put_DynamicVariables(BOOL hasDV)
{
    meter->SetDynamicVariables(hasDV);
    return S_OK;
}

STDMETHODIMP HostObjectMeter::get_X(int* value)
{
    *value = meter->GetX();
    return S_OK;
}

STDMETHODIMP HostObjectMeter::put_X(int value)
{
    std::wstring command(L"!MoveMeter ");
    command += std::to_wstring(value) + L" " +
        std::to_wstring(meter->GetY()) + L" " +
        L"\"" + std::wstring(meter->GetName()) + L"\"";
    RmExecute(RmGetSkin(rm), command.c_str());
    return S_OK;
}

STDMETHODIMP HostObjectMeter::get_Y(int* value)
{
    *value = meter->GetY();
    return S_OK;
}

STDMETHODIMP HostObjectMeter::put_Y(int value)
{
    std::wstring command(L"!MoveMeter ");
    command += std::to_wstring(meter->GetX()) + L" " +
        std::to_wstring(value) + L" " +
        L"\"" + std::wstring(meter->GetName()) + L"\"";
    RmExecute(RmGetSkin(rm), command.c_str());
    return S_OK;
}

STDMETHODIMP HostObjectMeter::get_Width(int* value)
{
    *value = meter->GetW();
    return S_OK;
}

STDMETHODIMP HostObjectMeter::put_Width(int value)
{
    meter->SetW(value);
    return S_OK;
}

STDMETHODIMP HostObjectMeter::get_Height(int* value)
{
    *value = meter->GetH();
    return S_OK;
}

STDMETHODIMP HostObjectMeter::put_Height(int value)
{
    meter->SetH(value);
    return S_OK;
}

STDMETHODIMP HostObjectMeter::GetTypeInfoCount(UINT* pctinfo)
{
    *pctinfo = 1;
    return S_OK;
}

STDMETHODIMP HostObjectMeter::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
    if (0 != iTInfo || !plugin->typeLib)
    {
        return TYPE_E_ELEMENTNOTFOUND;
    }

    return plugin->typeLib->GetTypeInfoOfGuid(__uuidof(IHostObjectMeter), ppTInfo);
}

STDMETHODIMP HostObjectMeter::GetIDsOfNames(
    REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
    wil::com_ptr<ITypeInfo> typeInfo;
    RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
    return typeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
}

STDMETHODIMP HostObjectMeter::Invoke(
    DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
    VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    wil::com_ptr<ITypeInfo> typeInfo;
    RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
    return typeInfo->Invoke(
        this, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}
