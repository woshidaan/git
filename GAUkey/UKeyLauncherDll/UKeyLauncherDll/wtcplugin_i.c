

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_Iplugin,0xE01EFFDA,0xD7D4,0x4504,0xB8,0xD4,0x96,0x7D,0x1B,0xE5,0x9D,0x9F);


MIDL_DEFINE_GUID(IID, LIBID_wtcpluginLib,0x7809930F,0x860E,0x4B29,0x91,0xED,0x0A,0x15,0x79,0x4C,0xFB,0xD7);


MIDL_DEFINE_GUID(IID, DIID__IpluginEvents,0xA1400B70,0xEB89,0x4DA6,0xA6,0x37,0x1D,0x0F,0xF1,0x4F,0x96,0xD4);


MIDL_DEFINE_GUID(CLSID, CLSID_plugin,0x0156A788,0x6C98,0x4ACF,0xAC,0x44,0x3B,0x40,0x22,0x8E,0x84,0x6C);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



