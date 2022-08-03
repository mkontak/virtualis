

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for vxComCAD.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __vxComCad_h__
#define __vxComCad_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IDictionary_FWD_DEFINED__
#define __IDictionary_FWD_DEFINED__
typedef interface IDictionary IDictionary;

#endif 	/* __IDictionary_FWD_DEFINED__ */


#ifndef __IFinding_FWD_DEFINED__
#define __IFinding_FWD_DEFINED__
typedef interface IFinding IFinding;

#endif 	/* __IFinding_FWD_DEFINED__ */


#ifndef __IFindingCollection_FWD_DEFINED__
#define __IFindingCollection_FWD_DEFINED__
typedef interface IFindingCollection IFindingCollection;

#endif 	/* __IFindingCollection_FWD_DEFINED__ */


#ifndef __IFindingDimension_FWD_DEFINED__
#define __IFindingDimension_FWD_DEFINED__
typedef interface IFindingDimension IFindingDimension;

#endif 	/* __IFindingDimension_FWD_DEFINED__ */


#ifndef __IFindingDimensions_FWD_DEFINED__
#define __IFindingDimensions_FWD_DEFINED__
typedef interface IFindingDimensions IFindingDimensions;

#endif 	/* __IFindingDimensions_FWD_DEFINED__ */


#ifndef __IFindingROI_FWD_DEFINED__
#define __IFindingROI_FWD_DEFINED__
typedef interface IFindingROI IFindingROI;

#endif 	/* __IFindingROI_FWD_DEFINED__ */


#ifndef __ILogger_FWD_DEFINED__
#define __ILogger_FWD_DEFINED__
typedef interface ILogger ILogger;

#endif 	/* __ILogger_FWD_DEFINED__ */


#ifndef __IProcessor_FWD_DEFINED__
#define __IProcessor_FWD_DEFINED__
typedef interface IProcessor IProcessor;

#endif 	/* __IProcessor_FWD_DEFINED__ */


#ifndef __IProcessorEnvironment_FWD_DEFINED__
#define __IProcessorEnvironment_FWD_DEFINED__
typedef interface IProcessorEnvironment IProcessorEnvironment;

#endif 	/* __IProcessorEnvironment_FWD_DEFINED__ */


#ifndef __IProcessorPlugin_FWD_DEFINED__
#define __IProcessorPlugin_FWD_DEFINED__
typedef interface IProcessorPlugin IProcessorPlugin;

#endif 	/* __IProcessorPlugin_FWD_DEFINED__ */


#ifndef __IVector_FWD_DEFINED__
#define __IVector_FWD_DEFINED__
typedef interface IVector IVector;

#endif 	/* __IVector_FWD_DEFINED__ */


#ifndef __IVolume_FWD_DEFINED__
#define __IVolume_FWD_DEFINED__
typedef interface IVolume IVolume;

#endif 	/* __IVolume_FWD_DEFINED__ */


#ifndef __IVolumeDimensions_FWD_DEFINED__
#define __IVolumeDimensions_FWD_DEFINED__
typedef interface IVolumeDimensions IVolumeDimensions;

#endif 	/* __IVolumeDimensions_FWD_DEFINED__ */


#ifndef __IVolumeUnits_FWD_DEFINED__
#define __IVolumeUnits_FWD_DEFINED__
typedef interface IVolumeUnits IVolumeUnits;

#endif 	/* __IVolumeUnits_FWD_DEFINED__ */


#ifndef __IVoxelPosition_FWD_DEFINED__
#define __IVoxelPosition_FWD_DEFINED__
typedef interface IVoxelPosition IVoxelPosition;

#endif 	/* __IVoxelPosition_FWD_DEFINED__ */


#ifndef __IVoxelPositionCollection_FWD_DEFINED__
#define __IVoxelPositionCollection_FWD_DEFINED__
typedef interface IVoxelPositionCollection IVoxelPositionCollection;

#endif 	/* __IVoxelPositionCollection_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __vxComCAD_LIBRARY_DEFINED__
#define __vxComCAD_LIBRARY_DEFINED__

/* library vxComCAD */
/* [helpstring][uuid] */ 

















typedef 
enum VectorUnits
    {
        Millimeter	= 0,
        Voxels	= ( Millimeter + 1 ) 
    } 	VectorUnits;


EXTERN_C const IID LIBID_vxComCAD;

#ifndef __IDictionary_INTERFACE_DEFINED__
#define __IDictionary_INTERFACE_DEFINED__

/* interface IDictionary */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_IDictionary;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8E877FAC-DCC4-49d5-8BB1-AA100247D8C6")
    IDictionary : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR sKey,
            /* [in] */ BSTR sValue) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ BSTR sKey) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetValue( 
            /* [in] */ BSTR sKey,
            /* [retval][out] */ BSTR *psValue) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Contains( 
            /* [in] */ BSTR sKey,
            /* [retval][out] */ VARIANT_BOOL *pbContains) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IDictionaryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDictionary * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDictionary * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDictionary * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDictionary * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDictionary * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDictionary * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDictionary * This,
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
            IDictionary * This,
            /* [in] */ BSTR sKey,
            /* [in] */ BSTR sValue);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IDictionary * This,
            /* [in] */ BSTR sKey);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetValue )( 
            IDictionary * This,
            /* [in] */ BSTR sKey,
            /* [retval][out] */ BSTR *psValue);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Contains )( 
            IDictionary * This,
            /* [in] */ BSTR sKey,
            /* [retval][out] */ VARIANT_BOOL *pbContains);
        
        END_INTERFACE
    } IDictionaryVtbl;

    interface IDictionary
    {
        CONST_VTBL struct IDictionaryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDictionary_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDictionary_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDictionary_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDictionary_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IDictionary_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IDictionary_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IDictionary_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IDictionary_Add(This,sKey,sValue)	\
    ( (This)->lpVtbl -> Add(This,sKey,sValue) ) 

#define IDictionary_Remove(This,sKey)	\
    ( (This)->lpVtbl -> Remove(This,sKey) ) 

#define IDictionary_GetValue(This,sKey,psValue)	\
    ( (This)->lpVtbl -> GetValue(This,sKey,psValue) ) 

#define IDictionary_Contains(This,sKey,pbContains)	\
    ( (This)->lpVtbl -> Contains(This,sKey,pbContains) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDictionary_INTERFACE_DEFINED__ */


#ifndef __IFinding_INTERFACE_DEFINED__
#define __IFinding_INTERFACE_DEFINED__

/* interface IFinding */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_IFinding;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C6380D2D-09FE-4163-8FAD-D0472257F31C")
    IFinding : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Id( 
            /* [retval][out] */ BSTR *psId) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Segment( 
            /* [retval][out] */ unsigned char *piSegment) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Segment( 
            /* [in] */ unsigned char iSegment) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Dimensions( 
            /* [retval][out] */ IFindingDimensions **ppiDimensions) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Centroid( 
            /* [retval][out] */ IVector **ppiCentroid) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Normal( 
            /* [retval][out] */ IVector **ppiNormal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_SurfaceROI( 
            /* [retval][out] */ IFindingROI **ppiSurfaceROI) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_VolumeROI( 
            /* [retval][out] */ IFindingROI **ppiVolumeROI) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_VendorSpecificData( 
            /* [retval][out] */ IDictionary **ppiData) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IFindingVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFinding * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFinding * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFinding * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFinding * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFinding * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFinding * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFinding * This,
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
            IFinding * This,
            /* [retval][out] */ BSTR *psId);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Segment )( 
            IFinding * This,
            /* [retval][out] */ unsigned char *piSegment);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Segment )( 
            IFinding * This,
            /* [in] */ unsigned char iSegment);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Dimensions )( 
            IFinding * This,
            /* [retval][out] */ IFindingDimensions **ppiDimensions);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Centroid )( 
            IFinding * This,
            /* [retval][out] */ IVector **ppiCentroid);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Normal )( 
            IFinding * This,
            /* [retval][out] */ IVector **ppiNormal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SurfaceROI )( 
            IFinding * This,
            /* [retval][out] */ IFindingROI **ppiSurfaceROI);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VolumeROI )( 
            IFinding * This,
            /* [retval][out] */ IFindingROI **ppiVolumeROI);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VendorSpecificData )( 
            IFinding * This,
            /* [retval][out] */ IDictionary **ppiData);
        
        END_INTERFACE
    } IFindingVtbl;

    interface IFinding
    {
        CONST_VTBL struct IFindingVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFinding_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFinding_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFinding_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFinding_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IFinding_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IFinding_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IFinding_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IFinding_get_Id(This,psId)	\
    ( (This)->lpVtbl -> get_Id(This,psId) ) 

#define IFinding_get_Segment(This,piSegment)	\
    ( (This)->lpVtbl -> get_Segment(This,piSegment) ) 

#define IFinding_put_Segment(This,iSegment)	\
    ( (This)->lpVtbl -> put_Segment(This,iSegment) ) 

#define IFinding_get_Dimensions(This,ppiDimensions)	\
    ( (This)->lpVtbl -> get_Dimensions(This,ppiDimensions) ) 

#define IFinding_get_Centroid(This,ppiCentroid)	\
    ( (This)->lpVtbl -> get_Centroid(This,ppiCentroid) ) 

#define IFinding_get_Normal(This,ppiNormal)	\
    ( (This)->lpVtbl -> get_Normal(This,ppiNormal) ) 

#define IFinding_get_SurfaceROI(This,ppiSurfaceROI)	\
    ( (This)->lpVtbl -> get_SurfaceROI(This,ppiSurfaceROI) ) 

#define IFinding_get_VolumeROI(This,ppiVolumeROI)	\
    ( (This)->lpVtbl -> get_VolumeROI(This,ppiVolumeROI) ) 

#define IFinding_get_VendorSpecificData(This,ppiData)	\
    ( (This)->lpVtbl -> get_VendorSpecificData(This,ppiData) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFinding_INTERFACE_DEFINED__ */


#ifndef __IFindingCollection_INTERFACE_DEFINED__
#define __IFindingCollection_INTERFACE_DEFINED__

/* interface IFindingCollection */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_IFindingCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("547B1BFD-96C5-4fd7-92B8-EC65F0E70F3F")
    IFindingCollection : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ int *piCount) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Vendor( 
            /* [retval][out] */ BSTR *psVendor) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Vendor( 
            /* [in] */ BSTR vendor) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Version( 
            /* [retval][out] */ BSTR *psVersion) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Version( 
            /* [in] */ BSTR sVersion) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_VendorSpecificData( 
            /* [retval][out] */ IDictionary **ppiDictionary) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ IFinding *piFinding) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Get( 
            /* [in] */ int iIndex,
            /* [retval][out] */ IFinding **ppiFinding) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ int index) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CreateFinding( 
            /* [retval][out] */ IFinding **ppiFinding) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ToFile( 
            /* [in] */ BSTR sFilename) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IFindingCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFindingCollection * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFindingCollection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFindingCollection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFindingCollection * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFindingCollection * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFindingCollection * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFindingCollection * This,
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
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IFindingCollection * This,
            /* [retval][out] */ int *piCount);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Vendor )( 
            IFindingCollection * This,
            /* [retval][out] */ BSTR *psVendor);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Vendor )( 
            IFindingCollection * This,
            /* [in] */ BSTR vendor);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Version )( 
            IFindingCollection * This,
            /* [retval][out] */ BSTR *psVersion);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Version )( 
            IFindingCollection * This,
            /* [in] */ BSTR sVersion);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VendorSpecificData )( 
            IFindingCollection * This,
            /* [retval][out] */ IDictionary **ppiDictionary);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IFindingCollection * This,
            /* [in] */ IFinding *piFinding);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Get )( 
            IFindingCollection * This,
            /* [in] */ int iIndex,
            /* [retval][out] */ IFinding **ppiFinding);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IFindingCollection * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IFindingCollection * This,
            /* [in] */ int index);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CreateFinding )( 
            IFindingCollection * This,
            /* [retval][out] */ IFinding **ppiFinding);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ToFile )( 
            IFindingCollection * This,
            /* [in] */ BSTR sFilename);
        
        END_INTERFACE
    } IFindingCollectionVtbl;

    interface IFindingCollection
    {
        CONST_VTBL struct IFindingCollectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFindingCollection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFindingCollection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFindingCollection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFindingCollection_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IFindingCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IFindingCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IFindingCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IFindingCollection_get_Count(This,piCount)	\
    ( (This)->lpVtbl -> get_Count(This,piCount) ) 

#define IFindingCollection_get_Vendor(This,psVendor)	\
    ( (This)->lpVtbl -> get_Vendor(This,psVendor) ) 

#define IFindingCollection_put_Vendor(This,vendor)	\
    ( (This)->lpVtbl -> put_Vendor(This,vendor) ) 

#define IFindingCollection_get_Version(This,psVersion)	\
    ( (This)->lpVtbl -> get_Version(This,psVersion) ) 

#define IFindingCollection_put_Version(This,sVersion)	\
    ( (This)->lpVtbl -> put_Version(This,sVersion) ) 

#define IFindingCollection_get_VendorSpecificData(This,ppiDictionary)	\
    ( (This)->lpVtbl -> get_VendorSpecificData(This,ppiDictionary) ) 

#define IFindingCollection_Add(This,piFinding)	\
    ( (This)->lpVtbl -> Add(This,piFinding) ) 

#define IFindingCollection_Get(This,iIndex,ppiFinding)	\
    ( (This)->lpVtbl -> Get(This,iIndex,ppiFinding) ) 

#define IFindingCollection_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IFindingCollection_Remove(This,index)	\
    ( (This)->lpVtbl -> Remove(This,index) ) 

#define IFindingCollection_CreateFinding(This,ppiFinding)	\
    ( (This)->lpVtbl -> CreateFinding(This,ppiFinding) ) 

#define IFindingCollection_ToFile(This,sFilename)	\
    ( (This)->lpVtbl -> ToFile(This,sFilename) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFindingCollection_INTERFACE_DEFINED__ */


#ifndef __IFindingDimension_INTERFACE_DEFINED__
#define __IFindingDimension_INTERFACE_DEFINED__

/* interface IFindingDimension */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_IFindingDimension;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("73D35558-5AA7-4255-9BA0-FF206392E600")
    IFindingDimension : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_StartPoint( 
            /* [retval][out] */ IVector **ppiVector) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_EndPoint( 
            /* [retval][out] */ IVector **ppiVector) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IFindingDimensionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFindingDimension * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFindingDimension * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFindingDimension * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFindingDimension * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFindingDimension * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFindingDimension * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFindingDimension * This,
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
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StartPoint )( 
            IFindingDimension * This,
            /* [retval][out] */ IVector **ppiVector);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EndPoint )( 
            IFindingDimension * This,
            /* [retval][out] */ IVector **ppiVector);
        
        END_INTERFACE
    } IFindingDimensionVtbl;

    interface IFindingDimension
    {
        CONST_VTBL struct IFindingDimensionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFindingDimension_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFindingDimension_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFindingDimension_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFindingDimension_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IFindingDimension_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IFindingDimension_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IFindingDimension_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IFindingDimension_get_StartPoint(This,ppiVector)	\
    ( (This)->lpVtbl -> get_StartPoint(This,ppiVector) ) 

#define IFindingDimension_get_EndPoint(This,ppiVector)	\
    ( (This)->lpVtbl -> get_EndPoint(This,ppiVector) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFindingDimension_INTERFACE_DEFINED__ */


#ifndef __IFindingDimensions_INTERFACE_DEFINED__
#define __IFindingDimensions_INTERFACE_DEFINED__

/* interface IFindingDimensions */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_IFindingDimensions;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D8A86671-74CF-4164-BF57-CAAC083BFDA7")
    IFindingDimensions : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Largest( 
            /* [retval][out] */ IFindingDimension **ppiLargest) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Perpendicular( 
            /* [retval][out] */ IFindingDimension **ppiPerpendicular) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Depth( 
            /* [retval][out] */ IFindingDimension **ppiDepth) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IFindingDimensionsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFindingDimensions * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFindingDimensions * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFindingDimensions * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFindingDimensions * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFindingDimensions * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFindingDimensions * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFindingDimensions * This,
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
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Largest )( 
            IFindingDimensions * This,
            /* [retval][out] */ IFindingDimension **ppiLargest);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Perpendicular )( 
            IFindingDimensions * This,
            /* [retval][out] */ IFindingDimension **ppiPerpendicular);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Depth )( 
            IFindingDimensions * This,
            /* [retval][out] */ IFindingDimension **ppiDepth);
        
        END_INTERFACE
    } IFindingDimensionsVtbl;

    interface IFindingDimensions
    {
        CONST_VTBL struct IFindingDimensionsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFindingDimensions_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFindingDimensions_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFindingDimensions_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFindingDimensions_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IFindingDimensions_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IFindingDimensions_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IFindingDimensions_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IFindingDimensions_get_Largest(This,ppiLargest)	\
    ( (This)->lpVtbl -> get_Largest(This,ppiLargest) ) 

#define IFindingDimensions_get_Perpendicular(This,ppiPerpendicular)	\
    ( (This)->lpVtbl -> get_Perpendicular(This,ppiPerpendicular) ) 

#define IFindingDimensions_get_Depth(This,ppiDepth)	\
    ( (This)->lpVtbl -> get_Depth(This,ppiDepth) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFindingDimensions_INTERFACE_DEFINED__ */


#ifndef __IFindingROI_INTERFACE_DEFINED__
#define __IFindingROI_INTERFACE_DEFINED__

/* interface IFindingROI */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_IFindingROI;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2797390E-A9B0-4ca2-AE20-8D277908B4B4")
    IFindingROI : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Voxels( 
            /* [retval][out] */ IVoxelPositionCollection **ppiVoxels) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_AverageHU( 
            /* [retval][out] */ float *pfAverage) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_AverageHU( 
            /* [in] */ float fAverage) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_StandardDeviationHU( 
            /* [retval][out] */ float *fStddev) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_StandardDeviationHU( 
            /* [in] */ float fStddev) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IFindingROIVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFindingROI * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFindingROI * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFindingROI * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFindingROI * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFindingROI * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFindingROI * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFindingROI * This,
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
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Voxels )( 
            IFindingROI * This,
            /* [retval][out] */ IVoxelPositionCollection **ppiVoxels);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AverageHU )( 
            IFindingROI * This,
            /* [retval][out] */ float *pfAverage);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AverageHU )( 
            IFindingROI * This,
            /* [in] */ float fAverage);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StandardDeviationHU )( 
            IFindingROI * This,
            /* [retval][out] */ float *fStddev);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_StandardDeviationHU )( 
            IFindingROI * This,
            /* [in] */ float fStddev);
        
        END_INTERFACE
    } IFindingROIVtbl;

    interface IFindingROI
    {
        CONST_VTBL struct IFindingROIVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFindingROI_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFindingROI_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFindingROI_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFindingROI_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IFindingROI_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IFindingROI_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IFindingROI_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IFindingROI_get_Voxels(This,ppiVoxels)	\
    ( (This)->lpVtbl -> get_Voxels(This,ppiVoxels) ) 

#define IFindingROI_get_AverageHU(This,pfAverage)	\
    ( (This)->lpVtbl -> get_AverageHU(This,pfAverage) ) 

#define IFindingROI_put_AverageHU(This,fAverage)	\
    ( (This)->lpVtbl -> put_AverageHU(This,fAverage) ) 

#define IFindingROI_get_StandardDeviationHU(This,fStddev)	\
    ( (This)->lpVtbl -> get_StandardDeviationHU(This,fStddev) ) 

#define IFindingROI_put_StandardDeviationHU(This,fStddev)	\
    ( (This)->lpVtbl -> put_StandardDeviationHU(This,fStddev) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFindingROI_INTERFACE_DEFINED__ */


#ifndef __ILogger_INTERFACE_DEFINED__
#define __ILogger_INTERFACE_DEFINED__

/* interface ILogger */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_ILogger;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3E56BFB7-2CAE-4c78-A8A1-DDD1F6CB229D")
    ILogger : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE LogError( 
            /* [in] */ BSTR sMsg,
            /* [in] */ BSTR sFunction,
            /* [in] */ BSTR sClassname) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE LogWarning( 
            /* [in] */ BSTR sMsg,
            /* [in] */ BSTR sFunction,
            /* [in] */ BSTR sClassname) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE LogInformation( 
            /* [in] */ BSTR sMsg,
            /* [in] */ BSTR sFunction,
            /* [in] */ BSTR sClassname) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE LogDebug( 
            /* [in] */ BSTR sMsg,
            /* [in] */ BSTR sFunction,
            /* [in] */ BSTR sClassname) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE LogTemporary( 
            /* [in] */ BSTR sMsg,
            /* [in] */ BSTR sFunction,
            /* [in] */ BSTR sClassname) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ILoggerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILogger * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILogger * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILogger * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ILogger * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ILogger * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ILogger * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ILogger * This,
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
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *LogError )( 
            ILogger * This,
            /* [in] */ BSTR sMsg,
            /* [in] */ BSTR sFunction,
            /* [in] */ BSTR sClassname);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *LogWarning )( 
            ILogger * This,
            /* [in] */ BSTR sMsg,
            /* [in] */ BSTR sFunction,
            /* [in] */ BSTR sClassname);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *LogInformation )( 
            ILogger * This,
            /* [in] */ BSTR sMsg,
            /* [in] */ BSTR sFunction,
            /* [in] */ BSTR sClassname);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *LogDebug )( 
            ILogger * This,
            /* [in] */ BSTR sMsg,
            /* [in] */ BSTR sFunction,
            /* [in] */ BSTR sClassname);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *LogTemporary )( 
            ILogger * This,
            /* [in] */ BSTR sMsg,
            /* [in] */ BSTR sFunction,
            /* [in] */ BSTR sClassname);
        
        END_INTERFACE
    } ILoggerVtbl;

    interface ILogger
    {
        CONST_VTBL struct ILoggerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILogger_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ILogger_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ILogger_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ILogger_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ILogger_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ILogger_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ILogger_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ILogger_LogError(This,sMsg,sFunction,sClassname)	\
    ( (This)->lpVtbl -> LogError(This,sMsg,sFunction,sClassname) ) 

#define ILogger_LogWarning(This,sMsg,sFunction,sClassname)	\
    ( (This)->lpVtbl -> LogWarning(This,sMsg,sFunction,sClassname) ) 

#define ILogger_LogInformation(This,sMsg,sFunction,sClassname)	\
    ( (This)->lpVtbl -> LogInformation(This,sMsg,sFunction,sClassname) ) 

#define ILogger_LogDebug(This,sMsg,sFunction,sClassname)	\
    ( (This)->lpVtbl -> LogDebug(This,sMsg,sFunction,sClassname) ) 

#define ILogger_LogTemporary(This,sMsg,sFunction,sClassname)	\
    ( (This)->lpVtbl -> LogTemporary(This,sMsg,sFunction,sClassname) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ILogger_INTERFACE_DEFINED__ */


#ifndef __IProcessor_INTERFACE_DEFINED__
#define __IProcessor_INTERFACE_DEFINED__

/* interface IProcessor */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_IProcessor;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("804F0825-4486-480d-BF79-AC09ABF1949E")
    IProcessor : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Environment( 
            /* [retval][out] */ IProcessorEnvironment **ppiEnvironment) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Findings( 
            /* [retval][out] */ IFindingCollection **ppiFindings) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Logger( 
            /* [retval][out] */ ILogger **ppiLogger) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IProcessorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProcessor * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProcessor * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProcessor * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IProcessor * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IProcessor * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IProcessor * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IProcessor * This,
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
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Environment )( 
            IProcessor * This,
            /* [retval][out] */ IProcessorEnvironment **ppiEnvironment);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Findings )( 
            IProcessor * This,
            /* [retval][out] */ IFindingCollection **ppiFindings);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Logger )( 
            IProcessor * This,
            /* [retval][out] */ ILogger **ppiLogger);
        
        END_INTERFACE
    } IProcessorVtbl;

    interface IProcessor
    {
        CONST_VTBL struct IProcessorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProcessor_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProcessor_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProcessor_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProcessor_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IProcessor_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IProcessor_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IProcessor_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IProcessor_get_Environment(This,ppiEnvironment)	\
    ( (This)->lpVtbl -> get_Environment(This,ppiEnvironment) ) 

#define IProcessor_get_Findings(This,ppiFindings)	\
    ( (This)->lpVtbl -> get_Findings(This,ppiFindings) ) 

#define IProcessor_get_Logger(This,ppiLogger)	\
    ( (This)->lpVtbl -> get_Logger(This,ppiLogger) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProcessor_INTERFACE_DEFINED__ */


#ifndef __IProcessorEnvironment_INTERFACE_DEFINED__
#define __IProcessorEnvironment_INTERFACE_DEFINED__

/* interface IProcessorEnvironment */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_IProcessorEnvironment;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("770D1C6E-44E0-4ac0-89F7-2BAB3EC71B15")
    IProcessorEnvironment : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_IntensityVolume( 
            /* [retval][out] */ IVolume **ppiIntensity) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_SegmentVolume( 
            /* [retval][out] */ IVolume **ppiSegment) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_DicomDirectory( 
            /* [retval][out] */ BSTR *psDirectory) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_DestDirectory( 
            /* [retval][out] */ BSTR *psDirectory) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IProcessorEnvironmentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProcessorEnvironment * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProcessorEnvironment * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProcessorEnvironment * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IProcessorEnvironment * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IProcessorEnvironment * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IProcessorEnvironment * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IProcessorEnvironment * This,
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
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IntensityVolume )( 
            IProcessorEnvironment * This,
            /* [retval][out] */ IVolume **ppiIntensity);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SegmentVolume )( 
            IProcessorEnvironment * This,
            /* [retval][out] */ IVolume **ppiSegment);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DicomDirectory )( 
            IProcessorEnvironment * This,
            /* [retval][out] */ BSTR *psDirectory);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DestDirectory )( 
            IProcessorEnvironment * This,
            /* [retval][out] */ BSTR *psDirectory);
        
        END_INTERFACE
    } IProcessorEnvironmentVtbl;

    interface IProcessorEnvironment
    {
        CONST_VTBL struct IProcessorEnvironmentVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProcessorEnvironment_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProcessorEnvironment_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProcessorEnvironment_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProcessorEnvironment_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IProcessorEnvironment_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IProcessorEnvironment_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IProcessorEnvironment_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IProcessorEnvironment_get_IntensityVolume(This,ppiIntensity)	\
    ( (This)->lpVtbl -> get_IntensityVolume(This,ppiIntensity) ) 

#define IProcessorEnvironment_get_SegmentVolume(This,ppiSegment)	\
    ( (This)->lpVtbl -> get_SegmentVolume(This,ppiSegment) ) 

#define IProcessorEnvironment_get_DicomDirectory(This,psDirectory)	\
    ( (This)->lpVtbl -> get_DicomDirectory(This,psDirectory) ) 

#define IProcessorEnvironment_get_DestDirectory(This,psDirectory)	\
    ( (This)->lpVtbl -> get_DestDirectory(This,psDirectory) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProcessorEnvironment_INTERFACE_DEFINED__ */


#ifndef __IProcessorPlugin_INTERFACE_DEFINED__
#define __IProcessorPlugin_INTERFACE_DEFINED__

/* interface IProcessorPlugin */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_IProcessorPlugin;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9724D4BD-9A24-4f3f-98A9-8F119AE6F454")
    IProcessorPlugin : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Run( 
            /* [in] */ IProcessor *piProcessor) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IProcessorPluginVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProcessorPlugin * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProcessorPlugin * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProcessorPlugin * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IProcessorPlugin * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IProcessorPlugin * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IProcessorPlugin * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IProcessorPlugin * This,
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
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Run )( 
            IProcessorPlugin * This,
            /* [in] */ IProcessor *piProcessor);
        
        END_INTERFACE
    } IProcessorPluginVtbl;

    interface IProcessorPlugin
    {
        CONST_VTBL struct IProcessorPluginVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProcessorPlugin_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProcessorPlugin_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProcessorPlugin_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProcessorPlugin_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IProcessorPlugin_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IProcessorPlugin_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IProcessorPlugin_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IProcessorPlugin_Run(This,piProcessor)	\
    ( (This)->lpVtbl -> Run(This,piProcessor) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProcessorPlugin_INTERFACE_DEFINED__ */


#ifndef __IVector_INTERFACE_DEFINED__
#define __IVector_INTERFACE_DEFINED__

/* interface IVector */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_IVector;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F7DF4AEE-2073-4be0-B3D2-DD694A78368B")
    IVector : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_X( 
            /* [retval][out] */ float *pfX) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_X( 
            /* [in] */ float fX) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Y( 
            /* [retval][out] */ float *pfY) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Y( 
            /* [in] */ float fY) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Z( 
            /* [retval][out] */ float *pfZ) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Z( 
            /* [in] */ float fZ) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Units( 
            /* [retval][out] */ VectorUnits *pUnits) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Units( 
            /* [in] */ VectorUnits units) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Set( 
            /* [in] */ float fX,
            /* [in] */ float fY,
            /* [in] */ float fZ) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVectorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVector * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVector * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVector * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVector * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVector * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVector * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVector * This,
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
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_X )( 
            IVector * This,
            /* [retval][out] */ float *pfX);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_X )( 
            IVector * This,
            /* [in] */ float fX);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Y )( 
            IVector * This,
            /* [retval][out] */ float *pfY);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Y )( 
            IVector * This,
            /* [in] */ float fY);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Z )( 
            IVector * This,
            /* [retval][out] */ float *pfZ);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Z )( 
            IVector * This,
            /* [in] */ float fZ);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Units )( 
            IVector * This,
            /* [retval][out] */ VectorUnits *pUnits);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Units )( 
            IVector * This,
            /* [in] */ VectorUnits units);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Set )( 
            IVector * This,
            /* [in] */ float fX,
            /* [in] */ float fY,
            /* [in] */ float fZ);
        
        END_INTERFACE
    } IVectorVtbl;

    interface IVector
    {
        CONST_VTBL struct IVectorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVector_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVector_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVector_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVector_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVector_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVector_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVector_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVector_get_X(This,pfX)	\
    ( (This)->lpVtbl -> get_X(This,pfX) ) 

#define IVector_put_X(This,fX)	\
    ( (This)->lpVtbl -> put_X(This,fX) ) 

#define IVector_get_Y(This,pfY)	\
    ( (This)->lpVtbl -> get_Y(This,pfY) ) 

#define IVector_put_Y(This,fY)	\
    ( (This)->lpVtbl -> put_Y(This,fY) ) 

#define IVector_get_Z(This,pfZ)	\
    ( (This)->lpVtbl -> get_Z(This,pfZ) ) 

#define IVector_put_Z(This,fZ)	\
    ( (This)->lpVtbl -> put_Z(This,fZ) ) 

#define IVector_get_Units(This,pUnits)	\
    ( (This)->lpVtbl -> get_Units(This,pUnits) ) 

#define IVector_put_Units(This,units)	\
    ( (This)->lpVtbl -> put_Units(This,units) ) 

#define IVector_Set(This,fX,fY,fZ)	\
    ( (This)->lpVtbl -> Set(This,fX,fY,fZ) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVector_INTERFACE_DEFINED__ */


#ifndef __IVolume_INTERFACE_DEFINED__
#define __IVolume_INTERFACE_DEFINED__

/* interface IVolume */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_IVolume;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("22820AB8-B477-4e2a-9A68-13A9D2659827")
    IVolume : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Dimensions( 
            /* [retval][out] */ IVolumeDimensions **ppiDimensions) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Units( 
            /* [retval][out] */ IVolumeUnits **ppiUnits) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Voxels( 
            /* [retval][out] */ VARIANT *pVoxels) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetHdrVar( 
            /* [in] */ BSTR sGroup,
            /* [in] */ BSTR sField,
            /* [retval][out] */ BSTR *psValue) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVolumeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVolume * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVolume * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVolume * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVolume * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVolume * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVolume * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVolume * This,
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
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Dimensions )( 
            IVolume * This,
            /* [retval][out] */ IVolumeDimensions **ppiDimensions);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Units )( 
            IVolume * This,
            /* [retval][out] */ IVolumeUnits **ppiUnits);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Voxels )( 
            IVolume * This,
            /* [retval][out] */ VARIANT *pVoxels);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetHdrVar )( 
            IVolume * This,
            /* [in] */ BSTR sGroup,
            /* [in] */ BSTR sField,
            /* [retval][out] */ BSTR *psValue);
        
        END_INTERFACE
    } IVolumeVtbl;

    interface IVolume
    {
        CONST_VTBL struct IVolumeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVolume_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVolume_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVolume_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVolume_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVolume_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVolume_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVolume_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVolume_get_Dimensions(This,ppiDimensions)	\
    ( (This)->lpVtbl -> get_Dimensions(This,ppiDimensions) ) 

#define IVolume_get_Units(This,ppiUnits)	\
    ( (This)->lpVtbl -> get_Units(This,ppiUnits) ) 

#define IVolume_get_Voxels(This,pVoxels)	\
    ( (This)->lpVtbl -> get_Voxels(This,pVoxels) ) 

#define IVolume_GetHdrVar(This,sGroup,sField,psValue)	\
    ( (This)->lpVtbl -> GetHdrVar(This,sGroup,sField,psValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVolume_INTERFACE_DEFINED__ */


#ifndef __IVolumeDimensions_INTERFACE_DEFINED__
#define __IVolumeDimensions_INTERFACE_DEFINED__

/* interface IVolumeDimensions */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_IVolumeDimensions;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6AA6323F-4264-4ce9-BDDD-577D57985C7A")
    IVolumeDimensions : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_X( 
            /* [retval][out] */ long *piX) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Y( 
            /* [retval][out] */ long *piY) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Z( 
            /* [retval][out] */ long *piZ) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVolumeDimensionsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVolumeDimensions * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVolumeDimensions * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVolumeDimensions * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVolumeDimensions * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVolumeDimensions * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVolumeDimensions * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVolumeDimensions * This,
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
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_X )( 
            IVolumeDimensions * This,
            /* [retval][out] */ long *piX);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Y )( 
            IVolumeDimensions * This,
            /* [retval][out] */ long *piY);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Z )( 
            IVolumeDimensions * This,
            /* [retval][out] */ long *piZ);
        
        END_INTERFACE
    } IVolumeDimensionsVtbl;

    interface IVolumeDimensions
    {
        CONST_VTBL struct IVolumeDimensionsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVolumeDimensions_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVolumeDimensions_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVolumeDimensions_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVolumeDimensions_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVolumeDimensions_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVolumeDimensions_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVolumeDimensions_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVolumeDimensions_get_X(This,piX)	\
    ( (This)->lpVtbl -> get_X(This,piX) ) 

#define IVolumeDimensions_get_Y(This,piY)	\
    ( (This)->lpVtbl -> get_Y(This,piY) ) 

#define IVolumeDimensions_get_Z(This,piZ)	\
    ( (This)->lpVtbl -> get_Z(This,piZ) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVolumeDimensions_INTERFACE_DEFINED__ */


#ifndef __IVolumeUnits_INTERFACE_DEFINED__
#define __IVolumeUnits_INTERFACE_DEFINED__

/* interface IVolumeUnits */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_IVolumeUnits;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4648ACA6-C500-497c-B78A-46E0351A4BC9")
    IVolumeUnits : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_X( 
            /* [retval][out] */ float *pfX) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Y( 
            /* [retval][out] */ float *pfY) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Z( 
            /* [retval][out] */ float *pfZ) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVolumeUnitsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVolumeUnits * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVolumeUnits * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVolumeUnits * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVolumeUnits * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVolumeUnits * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVolumeUnits * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVolumeUnits * This,
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
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_X )( 
            IVolumeUnits * This,
            /* [retval][out] */ float *pfX);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Y )( 
            IVolumeUnits * This,
            /* [retval][out] */ float *pfY);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Z )( 
            IVolumeUnits * This,
            /* [retval][out] */ float *pfZ);
        
        END_INTERFACE
    } IVolumeUnitsVtbl;

    interface IVolumeUnits
    {
        CONST_VTBL struct IVolumeUnitsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVolumeUnits_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVolumeUnits_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVolumeUnits_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVolumeUnits_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVolumeUnits_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVolumeUnits_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVolumeUnits_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVolumeUnits_get_X(This,pfX)	\
    ( (This)->lpVtbl -> get_X(This,pfX) ) 

#define IVolumeUnits_get_Y(This,pfY)	\
    ( (This)->lpVtbl -> get_Y(This,pfY) ) 

#define IVolumeUnits_get_Z(This,pfZ)	\
    ( (This)->lpVtbl -> get_Z(This,pfZ) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVolumeUnits_INTERFACE_DEFINED__ */


#ifndef __IVoxelPosition_INTERFACE_DEFINED__
#define __IVoxelPosition_INTERFACE_DEFINED__

/* interface IVoxelPosition */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_IVoxelPosition;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9ED04CB4-BC3B-4c82-BD02-8D0382C48ECA")
    IVoxelPosition : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_X( 
            /* [retval][out] */ int *piX) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_X( 
            /* [in] */ int iX) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Y( 
            /* [retval][out] */ int *piY) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Y( 
            /* [in] */ int iY) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Z( 
            /* [retval][out] */ int *piZ) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Z( 
            /* [in] */ int iZ) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Set( 
            /* [in] */ int iX,
            /* [in] */ int iY,
            /* [in] */ int iZ) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVoxelPositionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVoxelPosition * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVoxelPosition * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVoxelPosition * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVoxelPosition * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVoxelPosition * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVoxelPosition * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVoxelPosition * This,
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
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_X )( 
            IVoxelPosition * This,
            /* [retval][out] */ int *piX);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_X )( 
            IVoxelPosition * This,
            /* [in] */ int iX);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Y )( 
            IVoxelPosition * This,
            /* [retval][out] */ int *piY);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Y )( 
            IVoxelPosition * This,
            /* [in] */ int iY);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Z )( 
            IVoxelPosition * This,
            /* [retval][out] */ int *piZ);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Z )( 
            IVoxelPosition * This,
            /* [in] */ int iZ);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Set )( 
            IVoxelPosition * This,
            /* [in] */ int iX,
            /* [in] */ int iY,
            /* [in] */ int iZ);
        
        END_INTERFACE
    } IVoxelPositionVtbl;

    interface IVoxelPosition
    {
        CONST_VTBL struct IVoxelPositionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVoxelPosition_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVoxelPosition_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVoxelPosition_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVoxelPosition_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVoxelPosition_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVoxelPosition_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVoxelPosition_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVoxelPosition_get_X(This,piX)	\
    ( (This)->lpVtbl -> get_X(This,piX) ) 

#define IVoxelPosition_put_X(This,iX)	\
    ( (This)->lpVtbl -> put_X(This,iX) ) 

#define IVoxelPosition_get_Y(This,piY)	\
    ( (This)->lpVtbl -> get_Y(This,piY) ) 

#define IVoxelPosition_put_Y(This,iY)	\
    ( (This)->lpVtbl -> put_Y(This,iY) ) 

#define IVoxelPosition_get_Z(This,piZ)	\
    ( (This)->lpVtbl -> get_Z(This,piZ) ) 

#define IVoxelPosition_put_Z(This,iZ)	\
    ( (This)->lpVtbl -> put_Z(This,iZ) ) 

#define IVoxelPosition_Set(This,iX,iY,iZ)	\
    ( (This)->lpVtbl -> Set(This,iX,iY,iZ) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVoxelPosition_INTERFACE_DEFINED__ */


#ifndef __IVoxelPositionCollection_INTERFACE_DEFINED__
#define __IVoxelPositionCollection_INTERFACE_DEFINED__

/* interface IVoxelPositionCollection */
/* [uuid][unique][dual][nonextensible][object] */ 


EXTERN_C const IID IID_IVoxelPositionCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C9CE589D-F039-4ec1-BA0D-C1A994876E45")
    IVoxelPositionCollection : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ int *piCount) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ int iX,
            /* [in] */ int iY,
            /* [in] */ int iZ) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Get( 
            /* [in] */ int iIndex,
            /* [retval][out] */ IVoxelPosition **ppiPosition) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ int iIndex) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVoxelPositionCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVoxelPositionCollection * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVoxelPositionCollection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVoxelPositionCollection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVoxelPositionCollection * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVoxelPositionCollection * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVoxelPositionCollection * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVoxelPositionCollection * This,
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
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IVoxelPositionCollection * This,
            /* [retval][out] */ int *piCount);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IVoxelPositionCollection * This,
            /* [in] */ int iX,
            /* [in] */ int iY,
            /* [in] */ int iZ);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Get )( 
            IVoxelPositionCollection * This,
            /* [in] */ int iIndex,
            /* [retval][out] */ IVoxelPosition **ppiPosition);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IVoxelPositionCollection * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IVoxelPositionCollection * This,
            /* [in] */ int iIndex);
        
        END_INTERFACE
    } IVoxelPositionCollectionVtbl;

    interface IVoxelPositionCollection
    {
        CONST_VTBL struct IVoxelPositionCollectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVoxelPositionCollection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVoxelPositionCollection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVoxelPositionCollection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVoxelPositionCollection_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVoxelPositionCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVoxelPositionCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVoxelPositionCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVoxelPositionCollection_get_Count(This,piCount)	\
    ( (This)->lpVtbl -> get_Count(This,piCount) ) 

#define IVoxelPositionCollection_Add(This,iX,iY,iZ)	\
    ( (This)->lpVtbl -> Add(This,iX,iY,iZ) ) 

#define IVoxelPositionCollection_Get(This,iIndex,ppiPosition)	\
    ( (This)->lpVtbl -> Get(This,iIndex,ppiPosition) ) 

#define IVoxelPositionCollection_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IVoxelPositionCollection_Remove(This,iIndex)	\
    ( (This)->lpVtbl -> Remove(This,iIndex) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVoxelPositionCollection_INTERFACE_DEFINED__ */

#endif /* __vxComCAD_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


