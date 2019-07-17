

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Fri Mar 03 14:15:14 2017
 */
/* Compiler settings for .\wtcplugin.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __wtcplugin_i_h__
#define __wtcplugin_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __Iplugin_FWD_DEFINED__
#define __Iplugin_FWD_DEFINED__
typedef interface Iplugin Iplugin;
#endif 	/* __Iplugin_FWD_DEFINED__ */


#ifndef ___IpluginEvents_FWD_DEFINED__
#define ___IpluginEvents_FWD_DEFINED__
typedef interface _IpluginEvents _IpluginEvents;
#endif 	/* ___IpluginEvents_FWD_DEFINED__ */


#ifndef __plugin_FWD_DEFINED__
#define __plugin_FWD_DEFINED__

#ifdef __cplusplus
typedef class plugin plugin;
#else
typedef struct plugin plugin;
#endif /* __cplusplus */

#endif 	/* __plugin_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __Iplugin_INTERFACE_DEFINED__
#define __Iplugin_INTERFACE_DEFINED__

/* interface Iplugin */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_Iplugin;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E01EFFDA-D7D4-4504-B8D4-967D1BE59D9F")
    Iplugin : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE tmVerifyCode( 
            /* [in] */ BSTR pPinCode,
            /* [retval][out] */ BSTR *pResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE tmGetArtifact( 
            /* [retval][out] */ BSTR *pBuffer) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE tmSetArtifact( 
            /* [in] */ BSTR bstrTicket,
            /* [in] */ BSTR bstrClient,
            /* [retval][out] */ BSTR *pResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE tmGetData( 
            /* [retval][out] */ BSTR *pResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE tmGetUser( 
            /* [retval][out] */ BSTR *pResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE tmGetUniqueTC( 
            /* [retval][out] */ BSTR *pResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE tmSignData( 
            /* [in] */ BSTR bstrClientData,
            /* [in] */ BSTR bstrServerData,
            /* [in] */ BSTR bstrPin,
            /* [retval][out] */ BSTR *pResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE tmGetEncode( 
            /* [in] */ BSTR bstrKey,
            /* [in] */ BSTR bstrIV,
            /* [in] */ BSTR bstrPIN,
            /* [retval][out] */ BSTR *pResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE tmSetSysParam( 
            /* [in] */ BSTR bstrParam,
            /* [in] */ BSTR bstrData,
            /* [retval][out] */ BSTR *pResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE tmGetVersion( 
            /* [in] */ BSTR bstrSection,
            /* [in] */ BSTR bstrKey,
            /* [retval][out] */ BSTR *pResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE tmGetKeyStatus( 
            /* [in] */ BSTR bstrPID,
            /* [in] */ BSTR bstrVID,
            /* [retval][out] */ BSTR *pResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE tmGetKeyInfo( 
            /* [retval][out] */ BSTR *pResult) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IpluginVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            Iplugin * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            Iplugin * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            Iplugin * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            Iplugin * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            Iplugin * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            Iplugin * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            Iplugin * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *tmVerifyCode )( 
            Iplugin * This,
            /* [in] */ BSTR pPinCode,
            /* [retval][out] */ BSTR *pResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *tmGetArtifact )( 
            Iplugin * This,
            /* [retval][out] */ BSTR *pBuffer);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *tmSetArtifact )( 
            Iplugin * This,
            /* [in] */ BSTR bstrTicket,
            /* [in] */ BSTR bstrClient,
            /* [retval][out] */ BSTR *pResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *tmGetData )( 
            Iplugin * This,
            /* [retval][out] */ BSTR *pResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *tmGetUser )( 
            Iplugin * This,
            /* [retval][out] */ BSTR *pResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *tmGetUniqueTC )( 
            Iplugin * This,
            /* [retval][out] */ BSTR *pResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *tmSignData )( 
            Iplugin * This,
            /* [in] */ BSTR bstrClientData,
            /* [in] */ BSTR bstrServerData,
            /* [in] */ BSTR bstrPin,
            /* [retval][out] */ BSTR *pResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *tmGetEncode )( 
            Iplugin * This,
            /* [in] */ BSTR bstrKey,
            /* [in] */ BSTR bstrIV,
            /* [in] */ BSTR bstrPIN,
            /* [retval][out] */ BSTR *pResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *tmSetSysParam )( 
            Iplugin * This,
            /* [in] */ BSTR bstrParam,
            /* [in] */ BSTR bstrData,
            /* [retval][out] */ BSTR *pResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *tmGetVersion )( 
            Iplugin * This,
            /* [in] */ BSTR bstrSection,
            /* [in] */ BSTR bstrKey,
            /* [retval][out] */ BSTR *pResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *tmGetKeyStatus )( 
            Iplugin * This,
            /* [in] */ BSTR bstrPID,
            /* [in] */ BSTR bstrVID,
            /* [retval][out] */ BSTR *pResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *tmGetKeyInfo )( 
            Iplugin * This,
            /* [retval][out] */ BSTR *pResult);
        
        END_INTERFACE
    } IpluginVtbl;

    interface Iplugin
    {
        CONST_VTBL struct IpluginVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Iplugin_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define Iplugin_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define Iplugin_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define Iplugin_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define Iplugin_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define Iplugin_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define Iplugin_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define Iplugin_tmVerifyCode(This,pPinCode,pResult)	\
    ( (This)->lpVtbl -> tmVerifyCode(This,pPinCode,pResult) ) 

#define Iplugin_tmGetArtifact(This,pBuffer)	\
    ( (This)->lpVtbl -> tmGetArtifact(This,pBuffer) ) 

#define Iplugin_tmSetArtifact(This,bstrTicket,bstrClient,pResult)	\
    ( (This)->lpVtbl -> tmSetArtifact(This,bstrTicket,bstrClient,pResult) ) 

#define Iplugin_tmGetData(This,pResult)	\
    ( (This)->lpVtbl -> tmGetData(This,pResult) ) 

#define Iplugin_tmGetUser(This,pResult)	\
    ( (This)->lpVtbl -> tmGetUser(This,pResult) ) 

#define Iplugin_tmGetUniqueTC(This,pResult)	\
    ( (This)->lpVtbl -> tmGetUniqueTC(This,pResult) ) 

#define Iplugin_tmSignData(This,bstrClientData,bstrServerData,bstrPin,pResult)	\
    ( (This)->lpVtbl -> tmSignData(This,bstrClientData,bstrServerData,bstrPin,pResult) ) 

#define Iplugin_tmGetEncode(This,bstrKey,bstrIV,bstrPIN,pResult)	\
    ( (This)->lpVtbl -> tmGetEncode(This,bstrKey,bstrIV,bstrPIN,pResult) ) 

#define Iplugin_tmSetSysParam(This,bstrParam,bstrData,pResult)	\
    ( (This)->lpVtbl -> tmSetSysParam(This,bstrParam,bstrData,pResult) ) 

#define Iplugin_tmGetVersion(This,bstrSection,bstrKey,pResult)	\
    ( (This)->lpVtbl -> tmGetVersion(This,bstrSection,bstrKey,pResult) ) 

#define Iplugin_tmGetKeyStatus(This,bstrPID,bstrVID,pResult)	\
    ( (This)->lpVtbl -> tmGetKeyStatus(This,bstrPID,bstrVID,pResult) ) 

#define Iplugin_tmGetKeyInfo(This,pResult)	\
    ( (This)->lpVtbl -> tmGetKeyInfo(This,pResult) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __Iplugin_INTERFACE_DEFINED__ */



#ifndef __wtcpluginLib_LIBRARY_DEFINED__
#define __wtcpluginLib_LIBRARY_DEFINED__

/* library wtcpluginLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_wtcpluginLib;

#ifndef ___IpluginEvents_DISPINTERFACE_DEFINED__
#define ___IpluginEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IpluginEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IpluginEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("A1400B70-EB89-4DA6-A637-1D0FF14F96D4")
    _IpluginEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IpluginEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IpluginEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IpluginEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IpluginEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IpluginEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IpluginEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IpluginEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IpluginEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IpluginEventsVtbl;

    interface _IpluginEvents
    {
        CONST_VTBL struct _IpluginEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IpluginEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IpluginEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IpluginEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IpluginEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IpluginEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IpluginEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IpluginEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IpluginEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_plugin;

#ifdef __cplusplus

class DECLSPEC_UUID("0156A788-6C98-4ACF-AC44-3B40228E846C")
plugin;
#endif
#endif /* __wtcpluginLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


