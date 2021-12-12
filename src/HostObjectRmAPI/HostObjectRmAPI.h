// Copyright (C) 2022 khanhas. All rights reserved.

#pragma once
#include "common_h.h"

#include "HostObject_h.h"
#include "HostObjectMeasure/HostObjectMeasure.h"
#include "HostObjectMeter/HostObjectMeter.h"

class MeasureWebView;

class HostObjectRmAPI : public Microsoft::WRL::RuntimeClass<
                               Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
                               IHostObjectRmAPI, IDispatch>
{
public:
    HostObjectRmAPI(MeasureWebView* mwv);

    // Methods
    STDMETHODIMP GetMeasure(BSTR name, HostObjectMeasure** measureObj) override;
    STDMETHODIMP GetMeter(BSTR name, HostObjectMeter** meterObj) override;
    STDMETHODIMP GetVariable(BSTR stringParameter, BSTR* stringResult) override;
    STDMETHODIMP GetOption(BSTR stringParameter, BOOL replaceVariables, BSTR* stringResult) override;
    STDMETHODIMP Bang(BSTR stringParameter) override;
    STDMETHODIMP Log(BSTR stringParameter) override;

    // Properties

    // Host Object essentials
    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo) override;
    STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;
    STDMETHODIMP GetIDsOfNames(
        REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override;
    STDMETHODIMP Invoke(
        DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
        VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;


private:
    MeasureWebView* plugin;
    void* rm;
    Skin* skin;
};
