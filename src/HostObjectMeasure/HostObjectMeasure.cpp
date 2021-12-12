// Copyright (C) 2022 khanhas. All rights reserved.

#include "common_h.h"
#include "plugin.h"

#include "HostObjectMeasure/HostObjectMeasure.h"

double Measure::GetValue()
{
    // Invert if so requested
    if (m_Invert)
    {
        return m_MaxValue - m_Value + m_MinValue;
    }

    return m_Value;
}

HostObjectMeasure::HostObjectMeasure(MeasureWebView* mwv, Measure* measure)
    : plugin(mwv), rm(mwv->rm), measure(measure)
{
}

STDMETHODIMP HostObjectMeasure::GetString(BSTR* result)
{
    const WCHAR* stringValue = measure->GetStringValue();
    if (stringValue)
    {
        *result = SysAllocString(stringValue);
    }
    return S_OK;
}

STDMETHODIMP HostObjectMeasure::GetNumber(double* result)
{
    *result = measure->GetValue();
    return S_OK;
}

STDMETHODIMP HostObjectMeasure::GetOption(BSTR optionName, BOOL replaceVariables, BSTR* stringResult)
{
    std::wstring name(optionName);
    std::wstring value(RmReadString(measure, name.c_str(), nullptr, replaceVariables));
    *stringResult = SysAllocString(value.c_str());

    return S_OK;
}

STDMETHODIMP HostObjectMeasure::Command(BSTR command)
{
    measure->Command(std::wstring(command));
    return S_OK;
}

STDMETHODIMP HostObjectMeasure::Update()
{
    std::wstring command(L"!UpdateMeasure ");
    command += L"\"" + std::wstring(measure->GetName()) + L"\"";
    RmExecute(RmGetSkin(rm), command.c_str());
    return S_OK;
}

STDMETHODIMP HostObjectMeasure::get_Disabled(BOOL* isDisabled)
{
    *isDisabled = measure->IsDisabled();
    return S_OK;
}

STDMETHODIMP HostObjectMeasure::put_Disabled(BOOL isDisabled)
{
    std::wstring command(isDisabled ? L"!DisableMeasure " : L"!EnableMeasure ");
    command += L"\"" + std::wstring(measure->GetName()) + L"\"";
    RmExecute(RmGetSkin(rm), command.c_str());
    return S_OK;
}

STDMETHODIMP HostObjectMeasure::get_Paused(BOOL* isPaused)
{
    *isPaused = measure->IsPaused();
    return S_OK;
}

STDMETHODIMP HostObjectMeasure::put_Paused(BOOL isPaused)
{
    std::wstring command(isPaused ? L"!PauseMeasure " : L"!UnpauseMeasure ");
    command += L"\"" + std::wstring(measure->GetName()) + L"\"";
    RmExecute(RmGetSkin(rm), command.c_str());
    return S_OK;
}

STDMETHODIMP HostObjectMeasure::get_DynamicVariables(BOOL* hasDV)
{
    *hasDV = measure->HasDynamicVariables();
    return S_OK;
}

STDMETHODIMP HostObjectMeasure::put_DynamicVariables(BOOL hasDV)
{
    measure->SetDynamicVariables(hasDV);
    return S_OK;
}

STDMETHODIMP HostObjectMeasure::GetTypeInfoCount(UINT* pctinfo)
{
    *pctinfo = 1;
    return S_OK;
}

STDMETHODIMP HostObjectMeasure::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
    if (0 != iTInfo || !plugin->typeLib)
    {
        return TYPE_E_ELEMENTNOTFOUND;
    }
    
    return plugin->typeLib->GetTypeInfoOfGuid(__uuidof(IHostObjectMeasure), ppTInfo);
}

STDMETHODIMP HostObjectMeasure::GetIDsOfNames(
    REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
    wil::com_ptr<ITypeInfo> typeInfo;
    RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
    return typeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
}

STDMETHODIMP HostObjectMeasure::Invoke(
    DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
    VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    wil::com_ptr<ITypeInfo> typeInfo;
    RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
    return typeInfo->Invoke(
        this, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}
