

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Thu Jan 16 14:46:41 2014
 */
/* Compiler settings for ConsoleInterop.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

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

MIDL_DEFINE_GUID(IID, LIBID_ConsoleInterop,0xAA5F208B,0x8B2A,0x4038,0x93,0x5C,0x7A,0xD1,0x14,0x15,0xD5,0xC8);


MIDL_DEFINE_GUID(IID, IID_IConsole,0x0FEA47ED,0x26AB,0x4e1e,0xB7,0x0F,0xF5,0x96,0x66,0x92,0x47,0xF3);


MIDL_DEFINE_GUID(IID, IID_ISeries,0x58FDE160,0x5F10,0x4da4,0x99,0x4F,0x1D,0x20,0xFC,0x7E,0x98,0x88);


MIDL_DEFINE_GUID(IID, IID_ISeriesCollection,0x55D73772,0x412F,0x4d43,0x86,0xEE,0x30,0xF4,0x23,0x8A,0xC1,0xE2);


MIDL_DEFINE_GUID(IID, IID_IViewer,0xEBC43D1E,0x72EE,0x43D2,0xAD,0xC3,0xC6,0x1B,0x23,0xD2,0xDC,0x69);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



