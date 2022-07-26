

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for ReportLib.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



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
        EXTERN_C __declspec(selectany) const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif // !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, LIBID_ReportLib,0x4EC43105,0x6FAB,0x4AD8,0xBC,0xC7,0x10,0xD8,0x2F,0xC1,0x7B,0x0C);


MIDL_DEFINE_GUID(IID, IID_IImage,0xDE827E70,0x00D9,0x413C,0xB2,0x39,0xB4,0xB4,0x5B,0x2F,0x98,0x03);


MIDL_DEFINE_GUID(IID, IID_IReport,0x7EC556B8,0x05D6,0x46BC,0xAC,0x2E,0x72,0xDD,0x8C,0x28,0x26,0x6D);


MIDL_DEFINE_GUID(IID, IID_IReportRevisions,0x95B613D6,0x4616,0x47D4,0x8F,0x40,0x58,0x8E,0xAD,0xF3,0xE8,0xDD);


MIDL_DEFINE_GUID(IID, IID_IReportRevision,0x3131037F,0xA183,0x41D5,0xBD,0x2B,0x6B,0x8E,0xA1,0x65,0x9C,0x49);


MIDL_DEFINE_GUID(IID, IID_IReportEntries,0xD78AF9DD,0xF87F,0x4EE2,0xB1,0x2E,0xD4,0xFA,0x5F,0x00,0x99,0xCA);


MIDL_DEFINE_GUID(IID, IID_IReportEntry,0x0CF9B742,0xECAE,0x423A,0xB3,0x47,0xD7,0xCB,0xFD,0xA9,0x90,0x1C);


MIDL_DEFINE_GUID(IID, IID_IReportImages,0xCCA2B720,0xA0DF,0x4AE9,0x9D,0x21,0x3A,0x16,0x8A,0x92,0x29,0x38);


MIDL_DEFINE_GUID(IID, IID_IReportInfo,0x7D7F3959,0xECF1,0x42B7,0x8D,0x82,0x89,0x34,0x7D,0x0A,0xB3,0x81);


MIDL_DEFINE_GUID(IID, IID_IReportOtherFindings,0xFF2105C1,0xAE1C,0x40F7,0x8A,0x6F,0x6E,0x41,0x50,0x6E,0x58,0x5E);


MIDL_DEFINE_GUID(IID, IID_IReportOtherFinding,0xC23BEF18,0x3DB4,0x43D4,0xB3,0x58,0x65,0xDA,0xD3,0x3B,0x22,0xA8);


MIDL_DEFINE_GUID(IID, IID_IReportNonDistendedFindings,0x263CE509,0x680C,0x4C74,0xAA,0x77,0xFE,0x06,0x87,0xF6,0x5E,0xDD);


MIDL_DEFINE_GUID(IID, IID_IReportNonDistendedFinding,0x0BC74E4C,0xE84F,0x4702,0x8D,0x48,0x26,0xC0,0xC7,0xE5,0xC9,0x1D);


MIDL_DEFINE_GUID(CLSID, CLSID_Image,0x3886F2BB,0xAF89,0x4464,0x86,0xFF,0x90,0x58,0x60,0x1F,0x5F,0x30);


MIDL_DEFINE_GUID(CLSID, CLSID_ReportRevision,0x3AF6831F,0x5732,0x4E82,0xA3,0x33,0x04,0x65,0x4A,0x79,0x0B,0x85);


MIDL_DEFINE_GUID(CLSID, CLSID_ReportEntries,0xE579A075,0x8CE2,0x42BE,0x83,0x20,0x1C,0x83,0x13,0x34,0x70,0x6B);


MIDL_DEFINE_GUID(CLSID, CLSID_ReportEntry,0xD6F2B1B0,0x6371,0x4E64,0xB1,0xCD,0x4A,0xD6,0xEB,0x96,0xFA,0xE4);


MIDL_DEFINE_GUID(CLSID, CLSID_ReportInfo,0x845A2899,0xF269,0x456B,0x87,0xE0,0x28,0xA2,0xF4,0x49,0x14,0xF6);


MIDL_DEFINE_GUID(CLSID, CLSID_ReportImages,0x7C64E04A,0xF396,0x4BA8,0x89,0xE8,0x06,0x59,0x3E,0xF4,0xA5,0xD9);


MIDL_DEFINE_GUID(CLSID, CLSID_ReportOtherFindings,0xADAD89DF,0xE9EF,0x47B5,0xAF,0x27,0x81,0xCB,0x6C,0x67,0xE8,0xBB);


MIDL_DEFINE_GUID(CLSID, CLSID_ReportOtherFinding,0x2EF75240,0x6113,0x46F8,0xAD,0x06,0xC4,0xA4,0x1B,0x1F,0xA0,0xE1);


MIDL_DEFINE_GUID(CLSID, CLSID_ReportNonDistendedFindings,0x80DE0E55,0x31F5,0x431F,0x85,0xA5,0xE4,0x49,0x37,0x01,0xC3,0x5E);


MIDL_DEFINE_GUID(CLSID, CLSID_ReportNonDistendedFinding,0x4A9562C8,0x9614,0x4422,0xA8,0x15,0x30,0x1E,0x48,0x0D,0x1E,0xA6);


MIDL_DEFINE_GUID(CLSID, CLSID_Report,0x5A98EEB3,0x0CCF,0x4FBF,0x82,0x6C,0x4E,0x35,0x00,0xA3,0x5A,0x9B);


MIDL_DEFINE_GUID(CLSID, CLSID_ReportRevisions,0x09A2BA0C,0x1336,0x4981,0xB9,0xAD,0xAC,0xA0,0xF8,0xA5,0x92,0xB3);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



