

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Thu Jan 16 13:04:04 2014
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


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __ConsoleInterop_h_h__
#define __ConsoleInterop_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IConsole_FWD_DEFINED__
#define __IConsole_FWD_DEFINED__
typedef interface IConsole IConsole;

#endif 	/* __IConsole_FWD_DEFINED__ */


#ifndef __ISeries_FWD_DEFINED__
#define __ISeries_FWD_DEFINED__
typedef interface ISeries ISeries;

#endif 	/* __ISeries_FWD_DEFINED__ */


#ifndef __ISeriesCollection_FWD_DEFINED__
#define __ISeriesCollection_FWD_DEFINED__
typedef interface ISeriesCollection ISeriesCollection;

#endif 	/* __ISeriesCollection_FWD_DEFINED__ */


#ifndef __IViewer_FWD_DEFINED__
#define __IViewer_FWD_DEFINED__
typedef interface IViewer IViewer;

#endif 	/* __IViewer_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __ConsoleInterop_LIBRARY_DEFINED__
#define __ConsoleInterop_LIBRARY_DEFINED__

/* library ConsoleInterop */
/* [uuid] */ 





typedef 
enum WindowStateEnum
    {
        NormalState	= 0,
        MaximizedState	= ( NormalState + 1 ) ,
        MinimizedState	= ( MaximizedState + 1 ) 
    } 	WindowStateEnum;


EXTERN_C const IID LIBID_ConsoleInterop;

#ifndef __IConsole_INTERFACE_DEFINED__
#define __IConsole_INTERFACE_DEFINED__

/* interface IConsole */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_IConsole;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0FEA47ED-26AB-4e1e-B70F-F596669247F3")
    IConsole : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ProcessId( 
            /* [retval][out] */ int *id) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_UserXml( 
            /* [retval][out] */ BSTR *userXml) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Show( 
            /* [in] */ RECT bounds,
            /* [in] */ WindowStateEnum state) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Login( 
            /* [in] */ BSTR username,
            /* [in] */ BSTR password,
            /* [retval][out] */ VARIANT_BOOL *result) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetViewerClosing( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE LockApplication( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IConsoleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IConsole * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IConsole * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IConsole * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IConsole * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IConsole * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IConsole * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IConsole * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ProcessId )( 
            IConsole * This,
            /* [retval][out] */ int *id);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserXml )( 
            IConsole * This,
            /* [retval][out] */ BSTR *userXml);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Show )( 
            IConsole * This,
            /* [in] */ RECT bounds,
            /* [in] */ WindowStateEnum state);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Login )( 
            IConsole * This,
            /* [in] */ BSTR username,
            /* [in] */ BSTR password,
            /* [retval][out] */ VARIANT_BOOL *result);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetViewerClosing )( 
            IConsole * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *LockApplication )( 
            IConsole * This);
        
        END_INTERFACE
    } IConsoleVtbl;

    interface IConsole
    {
        CONST_VTBL struct IConsoleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IConsole_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IConsole_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IConsole_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IConsole_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IConsole_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IConsole_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IConsole_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IConsole_get_ProcessId(This,id)	\
    ( (This)->lpVtbl -> get_ProcessId(This,id) ) 

#define IConsole_get_UserXml(This,userXml)	\
    ( (This)->lpVtbl -> get_UserXml(This,userXml) ) 

#define IConsole_Show(This,bounds,state)	\
    ( (This)->lpVtbl -> Show(This,bounds,state) ) 

#define IConsole_Login(This,username,password,result)	\
    ( (This)->lpVtbl -> Login(This,username,password,result) ) 

#define IConsole_SetViewerClosing(This)	\
    ( (This)->lpVtbl -> SetViewerClosing(This) ) 

#define IConsole_LockApplication(This)	\
    ( (This)->lpVtbl -> LockApplication(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IConsole_INTERFACE_DEFINED__ */


#ifndef __ISeries_INTERFACE_DEFINED__
#define __ISeries_INTERFACE_DEFINED__

/* interface ISeries */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_ISeries;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("58FDE160-5F10-4da4-994F-1D20FC7E9888")
    ISeries : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Id( 
            /* [retval][out] */ BSTR *type) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Id( 
            /* [in] */ BSTR type) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ BSTR *type) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Type( 
            /* [in] */ BSTR type) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Directory( 
            /* [retval][out] */ BSTR *directory) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Directory( 
            /* [in] */ BSTR directory) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Orientation( 
            /* [retval][out] */ BSTR *type) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Orientation( 
            /* [in] */ BSTR Orientation) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ISeriesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISeries * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISeries * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISeries * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISeries * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISeries * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISeries * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISeries * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Id )( 
            ISeries * This,
            /* [retval][out] */ BSTR *type);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Id )( 
            ISeries * This,
            /* [in] */ BSTR type);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            ISeries * This,
            /* [retval][out] */ BSTR *type);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Type )( 
            ISeries * This,
            /* [in] */ BSTR type);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Directory )( 
            ISeries * This,
            /* [retval][out] */ BSTR *directory);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Directory )( 
            ISeries * This,
            /* [in] */ BSTR directory);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Orientation )( 
            ISeries * This,
            /* [retval][out] */ BSTR *type);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Orientation )( 
            ISeries * This,
            /* [in] */ BSTR Orientation);
        
        END_INTERFACE
    } ISeriesVtbl;

    interface ISeries
    {
        CONST_VTBL struct ISeriesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISeries_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISeries_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISeries_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISeries_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISeries_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISeries_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISeries_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISeries_get_Id(This,type)	\
    ( (This)->lpVtbl -> get_Id(This,type) ) 

#define ISeries_put_Id(This,type)	\
    ( (This)->lpVtbl -> put_Id(This,type) ) 

#define ISeries_get_Type(This,type)	\
    ( (This)->lpVtbl -> get_Type(This,type) ) 

#define ISeries_put_Type(This,type)	\
    ( (This)->lpVtbl -> put_Type(This,type) ) 

#define ISeries_get_Directory(This,directory)	\
    ( (This)->lpVtbl -> get_Directory(This,directory) ) 

#define ISeries_put_Directory(This,directory)	\
    ( (This)->lpVtbl -> put_Directory(This,directory) ) 

#define ISeries_get_Orientation(This,type)	\
    ( (This)->lpVtbl -> get_Orientation(This,type) ) 

#define ISeries_put_Orientation(This,Orientation)	\
    ( (This)->lpVtbl -> put_Orientation(This,Orientation) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISeries_INTERFACE_DEFINED__ */


#ifndef __ISeriesCollection_INTERFACE_DEFINED__
#define __ISeriesCollection_INTERFACE_DEFINED__

/* interface ISeriesCollection */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_ISeriesCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("55D73772-412F-4d43-86EE-30F4238AC1E2")
    ISeriesCollection : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR key,
            /* [in] */ ISeries *series) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Get( 
            /* [in] */ BSTR key,
            /* [retval][out] */ ISeries **series) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ISeriesCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISeriesCollection * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISeriesCollection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISeriesCollection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISeriesCollection * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISeriesCollection * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISeriesCollection * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISeriesCollection * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            ISeriesCollection * This,
            /* [in] */ BSTR key,
            /* [in] */ ISeries *series);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Get )( 
            ISeriesCollection * This,
            /* [in] */ BSTR key,
            /* [retval][out] */ ISeries **series);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            ISeriesCollection * This);
        
        END_INTERFACE
    } ISeriesCollectionVtbl;

    interface ISeriesCollection
    {
        CONST_VTBL struct ISeriesCollectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISeriesCollection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISeriesCollection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISeriesCollection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISeriesCollection_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISeriesCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISeriesCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISeriesCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISeriesCollection_Add(This,key,series)	\
    ( (This)->lpVtbl -> Add(This,key,series) ) 

#define ISeriesCollection_Get(This,key,series)	\
    ( (This)->lpVtbl -> Get(This,key,series) ) 

#define ISeriesCollection_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISeriesCollection_INTERFACE_DEFINED__ */


#ifndef __IViewer_INTERFACE_DEFINED__
#define __IViewer_INTERFACE_DEFINED__

/* interface IViewer */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_IViewer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EBC43D1E-72EE-43D2-ADC3-C61B23D2DC69")
    IViewer : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ IConsole *console,
            /* [in] */ int iIdleInterval) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Show( 
            /* [in] */ RECT bounds,
            /* [in] */ WindowStateEnum state) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Load( 
            /* [in] */ ISeries *session,
            /* [in] */ ISeriesCollection *primary,
            /* [in] */ ISeriesCollection *secondary) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IViewerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IViewer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IViewer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IViewer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IViewer * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IViewer * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IViewer * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IViewer * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IViewer * This,
            /* [in] */ IConsole *console,
            /* [in] */ int iIdleInterval);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Show )( 
            IViewer * This,
            /* [in] */ RECT bounds,
            /* [in] */ WindowStateEnum state);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IViewer * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Load )( 
            IViewer * This,
            /* [in] */ ISeries *session,
            /* [in] */ ISeriesCollection *primary,
            /* [in] */ ISeriesCollection *secondary);
        
        END_INTERFACE
    } IViewerVtbl;

    interface IViewer
    {
        CONST_VTBL struct IViewerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IViewer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IViewer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IViewer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IViewer_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IViewer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IViewer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IViewer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IViewer_Initialize(This,console,iIdleInterval)	\
    ( (This)->lpVtbl -> Initialize(This,console,iIdleInterval) ) 

#define IViewer_Show(This,bounds,state)	\
    ( (This)->lpVtbl -> Show(This,bounds,state) ) 

#define IViewer_Close(This)	\
    ( (This)->lpVtbl -> Close(This) ) 

#define IViewer_Load(This,session,primary,secondary)	\
    ( (This)->lpVtbl -> Load(This,session,primary,secondary) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IViewer_INTERFACE_DEFINED__ */

#endif /* __ConsoleInterop_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


