

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __ReportLib_h__
#define __ReportLib_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IImage_FWD_DEFINED__
#define __IImage_FWD_DEFINED__
typedef interface IImage IImage;

#endif 	/* __IImage_FWD_DEFINED__ */


#ifndef __IReport_FWD_DEFINED__
#define __IReport_FWD_DEFINED__
typedef interface IReport IReport;

#endif 	/* __IReport_FWD_DEFINED__ */


#ifndef __IReportRevisions_FWD_DEFINED__
#define __IReportRevisions_FWD_DEFINED__
typedef interface IReportRevisions IReportRevisions;

#endif 	/* __IReportRevisions_FWD_DEFINED__ */


#ifndef __IReportRevision_FWD_DEFINED__
#define __IReportRevision_FWD_DEFINED__
typedef interface IReportRevision IReportRevision;

#endif 	/* __IReportRevision_FWD_DEFINED__ */


#ifndef __IReportEntries_FWD_DEFINED__
#define __IReportEntries_FWD_DEFINED__
typedef interface IReportEntries IReportEntries;

#endif 	/* __IReportEntries_FWD_DEFINED__ */


#ifndef __IReportEntry_FWD_DEFINED__
#define __IReportEntry_FWD_DEFINED__
typedef interface IReportEntry IReportEntry;

#endif 	/* __IReportEntry_FWD_DEFINED__ */


#ifndef __IReportImages_FWD_DEFINED__
#define __IReportImages_FWD_DEFINED__
typedef interface IReportImages IReportImages;

#endif 	/* __IReportImages_FWD_DEFINED__ */


#ifndef __IReportInfo_FWD_DEFINED__
#define __IReportInfo_FWD_DEFINED__
typedef interface IReportInfo IReportInfo;

#endif 	/* __IReportInfo_FWD_DEFINED__ */


#ifndef __IReportOtherFindings_FWD_DEFINED__
#define __IReportOtherFindings_FWD_DEFINED__
typedef interface IReportOtherFindings IReportOtherFindings;

#endif 	/* __IReportOtherFindings_FWD_DEFINED__ */


#ifndef __IReportOtherFinding_FWD_DEFINED__
#define __IReportOtherFinding_FWD_DEFINED__
typedef interface IReportOtherFinding IReportOtherFinding;

#endif 	/* __IReportOtherFinding_FWD_DEFINED__ */


#ifndef __IReportNonDistendedFindings_FWD_DEFINED__
#define __IReportNonDistendedFindings_FWD_DEFINED__
typedef interface IReportNonDistendedFindings IReportNonDistendedFindings;

#endif 	/* __IReportNonDistendedFindings_FWD_DEFINED__ */


#ifndef __IReportNonDistendedFinding_FWD_DEFINED__
#define __IReportNonDistendedFinding_FWD_DEFINED__
typedef interface IReportNonDistendedFinding IReportNonDistendedFinding;

#endif 	/* __IReportNonDistendedFinding_FWD_DEFINED__ */


#ifndef __Image_FWD_DEFINED__
#define __Image_FWD_DEFINED__

#ifdef __cplusplus
typedef class Image Image;
#else
typedef struct Image Image;
#endif /* __cplusplus */

#endif 	/* __Image_FWD_DEFINED__ */


#ifndef __ReportRevision_FWD_DEFINED__
#define __ReportRevision_FWD_DEFINED__

#ifdef __cplusplus
typedef class ReportRevision ReportRevision;
#else
typedef struct ReportRevision ReportRevision;
#endif /* __cplusplus */

#endif 	/* __ReportRevision_FWD_DEFINED__ */


#ifndef __ReportEntries_FWD_DEFINED__
#define __ReportEntries_FWD_DEFINED__

#ifdef __cplusplus
typedef class ReportEntries ReportEntries;
#else
typedef struct ReportEntries ReportEntries;
#endif /* __cplusplus */

#endif 	/* __ReportEntries_FWD_DEFINED__ */


#ifndef __ReportEntry_FWD_DEFINED__
#define __ReportEntry_FWD_DEFINED__

#ifdef __cplusplus
typedef class ReportEntry ReportEntry;
#else
typedef struct ReportEntry ReportEntry;
#endif /* __cplusplus */

#endif 	/* __ReportEntry_FWD_DEFINED__ */


#ifndef __ReportInfo_FWD_DEFINED__
#define __ReportInfo_FWD_DEFINED__

#ifdef __cplusplus
typedef class ReportInfo ReportInfo;
#else
typedef struct ReportInfo ReportInfo;
#endif /* __cplusplus */

#endif 	/* __ReportInfo_FWD_DEFINED__ */


#ifndef __ReportImages_FWD_DEFINED__
#define __ReportImages_FWD_DEFINED__

#ifdef __cplusplus
typedef class ReportImages ReportImages;
#else
typedef struct ReportImages ReportImages;
#endif /* __cplusplus */

#endif 	/* __ReportImages_FWD_DEFINED__ */


#ifndef __ReportOtherFindings_FWD_DEFINED__
#define __ReportOtherFindings_FWD_DEFINED__

#ifdef __cplusplus
typedef class ReportOtherFindings ReportOtherFindings;
#else
typedef struct ReportOtherFindings ReportOtherFindings;
#endif /* __cplusplus */

#endif 	/* __ReportOtherFindings_FWD_DEFINED__ */


#ifndef __ReportOtherFinding_FWD_DEFINED__
#define __ReportOtherFinding_FWD_DEFINED__

#ifdef __cplusplus
typedef class ReportOtherFinding ReportOtherFinding;
#else
typedef struct ReportOtherFinding ReportOtherFinding;
#endif /* __cplusplus */

#endif 	/* __ReportOtherFinding_FWD_DEFINED__ */


#ifndef __ReportNonDistendedFindings_FWD_DEFINED__
#define __ReportNonDistendedFindings_FWD_DEFINED__

#ifdef __cplusplus
typedef class ReportNonDistendedFindings ReportNonDistendedFindings;
#else
typedef struct ReportNonDistendedFindings ReportNonDistendedFindings;
#endif /* __cplusplus */

#endif 	/* __ReportNonDistendedFindings_FWD_DEFINED__ */


#ifndef __ReportNonDistendedFinding_FWD_DEFINED__
#define __ReportNonDistendedFinding_FWD_DEFINED__

#ifdef __cplusplus
typedef class ReportNonDistendedFinding ReportNonDistendedFinding;
#else
typedef struct ReportNonDistendedFinding ReportNonDistendedFinding;
#endif /* __cplusplus */

#endif 	/* __ReportNonDistendedFinding_FWD_DEFINED__ */


#ifndef __Report_FWD_DEFINED__
#define __Report_FWD_DEFINED__

#ifdef __cplusplus
typedef class Report Report;
#else
typedef struct Report Report;
#endif /* __cplusplus */

#endif 	/* __Report_FWD_DEFINED__ */


#ifndef __ReportRevisions_FWD_DEFINED__
#define __ReportRevisions_FWD_DEFINED__

#ifdef __cplusplus
typedef class ReportRevisions ReportRevisions;
#else
typedef struct ReportRevisions ReportRevisions;
#endif /* __cplusplus */

#endif 	/* __ReportRevisions_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __ReportLib_LIBRARY_DEFINED__
#define __ReportLib_LIBRARY_DEFINED__

/* library ReportLib */
/* [helpstring][version][uuid] */ 













typedef 
enum PatientSexEnum
    {
        Male	= 0,
        Female	= 1
    } 	PatientSexEnum;

typedef 
enum ColonFindingEnum
    {
        Polyp	= 0,
        Diverticulum	= 1,
        Stenosis	= 2,
        Other_Normal	= 99,
        Other_Colonic	= 100,
        Other_Extracolonic	= 101
    } 	ColonFindingEnum;

typedef 
enum OtherFindingsEnum
    {
        Lung_Nodule	= 0,
        Lung_Granuloma	= 1,
        Lung_Pneomuthorax	= 2,
        Liver_Mass	= 3,
        Liver_Cyst	= 4,
        Liver_Gallstones	= 5,
        Liver_FattyLiver	= 6,
        Adrenal_Mass	= 7,
        Renal_Mass	= 8,
        Renal_Cyst	= 9,
        Renal_Stones	= 10,
        Renal_Scar	= 11,
        Renal_Hydronephrosis	= 12,
        Vascular_AAA	= 13,
        Vascular_ArteralCalcification	= 14,
        Spleen_Mass	= 15,
        Spleen_Granuloma	= 16,
        GI_InguinalHernia	= 17,
        GI_HiatalHernia	= 18,
        Gynecology_UterineFibroids	= 19,
        Misc_Other	= 20
    } 	OtherFindingsEnum;

typedef 
enum LocationEnum
    {
        Cecum	= 0,
        Ascending	= 1,
        HepaticFlexure	= 2,
        Transverse	= 3,
        SplenicFlexure	= 4,
        Descending	= 5,
        Sigmoid	= 6,
        Rectum	= 7,
        NearFlexure	= 16
    } 	LocationEnum;

typedef 
enum ShapeEnum
    {
        Round	= 0,
        Oval	= 1,
        Sessile	= 2,
        PedunculatedStalkSeen	= 3,
        PedunculatedStalkUnseen	= 4,
        Flat	= 5
    } 	ShapeEnum;

typedef 
enum SizeComparisonEnum
    {
        NoPriorStudy	= 0,
        Increase	= 1,
        Descrease	= 2,
        Same	= 3
    } 	SizeComparisonEnum;

typedef 
enum TranslucencyEnum
    {
        Indeterminate	= 0,
        SimilarToColonWall	= 1,
        GradualDensityChange	= 2,
        AbruptDensityChange	= 3,
        IntermediateDensityChange	= 4
    } 	TranslucencyEnum;

typedef 
enum LungLocationEnum
    {
        RightLung_UpperLobe	= 0,
        RightLung_MiddleLobe	= 1,
        RightLung_LowerLobe	= 2,
        LeftLung_UpperLobe	= 3,
        LeftLung_MiddleLobe	= 4,
        LeftLung_LowerLobe	= 5
    } 	LungLocationEnum;

typedef 
enum LiverLocationEnum
    {
        RightLobe_AnteriorSegment	= 0,
        RightLobe_PosteriorSegment	= 1,
        LeftLobe_MedialSegment	= 2,
        LeftLobe_LateralSegment	= 3
    } 	LiverLocationEnum;

typedef 
enum KidneyLocationEnum
    {
        RightKidney	= 0,
        LeftKidney	= 1
    } 	KidneyLocationEnum;

typedef 
enum VascularLocationEnum
    {
        Suprarenal	= 0,
        Juxtarenal	= 1,
        Infrarenal	= 2
    } 	VascularLocationEnum;

typedef 
enum SpleenLocationEnum
    {
        Superior	= 0,
        Middle	= 1,
        Inferior	= 2
    } 	SpleenLocationEnum;

typedef 
enum AdrenalLocationEnum
    {
        Right	= 0,
        Left	= 1
    } 	AdrenalLocationEnum;

typedef 
enum RenalLocationEnum
    {
        Right_UpperPole	= 0,
        Right_Middle	= 1,
        Right_LowerPole	= 2,
        Left_UpperPole	= 3,
        Left_Middle	= 4,
        Left_LowerPole	= 5
    } 	RenalLocationEnum;

typedef 
enum DegreeEnum
    {
        Mild	= 0,
        Moderate	= 1,
        Severe	= 2
    } 	DegreeEnum;

typedef 
enum PneumothoraxLocationEnum
    {
        RightLung	= 0,
        LeftLung	= 1
    } 	PneumothoraxLocation;

typedef 
enum PneumothoraxSizeEnum
    {
        Small	= 0,
        Medium	= 1,
        Large	= 2
    } 	PneumothoraxSizeEnum;

typedef 
enum IndicationsEnum
    {
        Screening	= 0x1,
        PersonalHistoryOfPolyps	= 0x2,
        Constipation	= 0x4,
        PersonalHistoryOfCancer	= 0x8,
        FamilyHistoryOfCancer	= 0x10,
        Anemia	= 0x20,
        Hematochezia	= 0x40,
        AbnormalImaging	= 0x80,
        AbdominalPain	= 0x100,
        IBD	= 0x200,
        Diarrhea	= 0x400,
        PolypOnFlexSigmoid	= 0x800,
        FamilyHistoryOfPolyps	= 0x1000,
        FailedColonoscopy	= 0x2000,
        HighSedationRisk	= 0x4000,
        IndicationsEnumLast	= 0x4000
    } 	Indications;

typedef 
enum ExamQualityEnum
    {
        FecalTagging	= 0x1,
        CO2	= 0x2,
        RoomAir	= 0x4,
        Barium	= 0x8,
        Iodine	= 0x10,
        Glucagon	= 0x20,
        Buscopan	= 0x40,
        ExamQualityEnumLast	= 0x40
    } 	ExamQuality;

typedef 
enum RecommendationsEnum
    {
        FollowUp	= 0x1,
        Removal	= 0x2
    } 	RecommendationsEnum;

typedef 
enum RecommendationTypeEnum
    {
        Virtual	= 0,
        Optical	= 1,
        Standard	= 2
    } 	RecommendationTypeEnum;

typedef 
enum RecommendationUnitEnum
    {
        Years	= 0,
        Months	= 1
    } 	RecommendationUnitEnum;

typedef 
enum DatasetPositionEnumProp
    {
        Supine	= 0,
        Prone	= 1,
        Decubitus	= 2,
        DecubitusLeft	= 3,
        DecubitusRight	= 4
    } 	DatasetPositionEnumProp;


EXTERN_C const IID LIBID_ReportLib;

#ifndef __IImage_INTERFACE_DEFINED__
#define __IImage_INTERFACE_DEFINED__

/* interface IImage */
/* [unique][helpstring][dual][uuid][nonextensible][object] */ 


EXTERN_C const IID IID_IImage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DE827E70-00D9-413C-B239-B4B45B2F9803")
    IImage : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Width( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Height( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BitsPerPixel( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Bitmap( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ID( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Pixels( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ImageSize( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Create( 
            /* [in] */ long Width,
            /* [in] */ long Height,
            /* [in] */ short BitsPerPixel) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPixels( 
            /* [in] */ VARIANT Bytes) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Resize( 
            /* [in] */ long nWidth,
            /* [in] */ long nHeight) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IImageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IImage * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IImage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IImage * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IImage * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IImage * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IImage * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IImage * This,
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
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Width )( 
            IImage * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Height )( 
            IImage * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BitsPerPixel )( 
            IImage * This,
            /* [retval][out] */ short *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Bitmap )( 
            IImage * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ID )( 
            IImage * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Pixels )( 
            IImage * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ImageSize )( 
            IImage * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Create )( 
            IImage * This,
            /* [in] */ long Width,
            /* [in] */ long Height,
            /* [in] */ short BitsPerPixel);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPixels )( 
            IImage * This,
            /* [in] */ VARIANT Bytes);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Resize )( 
            IImage * This,
            /* [in] */ long nWidth,
            /* [in] */ long nHeight);
        
        END_INTERFACE
    } IImageVtbl;

    interface IImage
    {
        CONST_VTBL struct IImageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IImage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IImage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IImage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IImage_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IImage_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IImage_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IImage_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IImage_get_Width(This,pVal)	\
    ( (This)->lpVtbl -> get_Width(This,pVal) ) 

#define IImage_get_Height(This,pVal)	\
    ( (This)->lpVtbl -> get_Height(This,pVal) ) 

#define IImage_get_BitsPerPixel(This,pVal)	\
    ( (This)->lpVtbl -> get_BitsPerPixel(This,pVal) ) 

#define IImage_get_Bitmap(This,pVal)	\
    ( (This)->lpVtbl -> get_Bitmap(This,pVal) ) 

#define IImage_get_ID(This,pVal)	\
    ( (This)->lpVtbl -> get_ID(This,pVal) ) 

#define IImage_get_Pixels(This,pVal)	\
    ( (This)->lpVtbl -> get_Pixels(This,pVal) ) 

#define IImage_get_ImageSize(This,pVal)	\
    ( (This)->lpVtbl -> get_ImageSize(This,pVal) ) 

#define IImage_Create(This,Width,Height,BitsPerPixel)	\
    ( (This)->lpVtbl -> Create(This,Width,Height,BitsPerPixel) ) 

#define IImage_SetPixels(This,Bytes)	\
    ( (This)->lpVtbl -> SetPixels(This,Bytes) ) 

#define IImage_Resize(This,nWidth,nHeight)	\
    ( (This)->lpVtbl -> Resize(This,nWidth,nHeight) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IImage_INTERFACE_DEFINED__ */


#ifndef __IReport_INTERFACE_DEFINED__
#define __IReport_INTERFACE_DEFINED__

/* interface IReport */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IReport;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7EC556B8-05D6-46BC-AC2E-72DD8C28266D")
    IReport : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Revisions( 
            /* [retval][out] */ IReportRevisions **Revisions) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Create( 
            /* [in] */ BSTR FileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ BSTR FileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IReportVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IReport * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IReport * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IReport * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IReport * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IReport * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IReport * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IReport * This,
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
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Revisions )( 
            IReport * This,
            /* [retval][out] */ IReportRevisions **Revisions);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Create )( 
            IReport * This,
            /* [in] */ BSTR FileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Open )( 
            IReport * This,
            /* [in] */ BSTR FileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IReport * This);
        
        END_INTERFACE
    } IReportVtbl;

    interface IReport
    {
        CONST_VTBL struct IReportVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReport_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IReport_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IReport_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IReport_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IReport_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IReport_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IReport_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IReport_get_Revisions(This,Revisions)	\
    ( (This)->lpVtbl -> get_Revisions(This,Revisions) ) 

#define IReport_Create(This,FileName)	\
    ( (This)->lpVtbl -> Create(This,FileName) ) 

#define IReport_Open(This,FileName)	\
    ( (This)->lpVtbl -> Open(This,FileName) ) 

#define IReport_Close(This)	\
    ( (This)->lpVtbl -> Close(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IReport_INTERFACE_DEFINED__ */


#ifndef __IReportRevisions_INTERFACE_DEFINED__
#define __IReportRevisions_INTERFACE_DEFINED__

/* interface IReportRevisions */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IReportRevisions;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("95B613D6-4616-47D4-8F40-588EADF3E8DD")
    IReportRevisions : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IReportRevision **Info) = 0;
        
        virtual /* [restricted][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **Unknown) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *Count) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [retval][out] */ IReportRevision **Entry) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ VARIANT Index) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IReportRevisionsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IReportRevisions * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IReportRevisions * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IReportRevisions * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IReportRevisions * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IReportRevisions * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IReportRevisions * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IReportRevisions * This,
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
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            IReportRevisions * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IReportRevision **Info);
        
        /* [restricted][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IReportRevisions * This,
            /* [retval][out] */ IUnknown **Unknown);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IReportRevisions * This,
            /* [retval][out] */ long *Count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IReportRevisions * This,
            /* [retval][out] */ IReportRevision **Entry);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IReportRevisions * This,
            /* [in] */ VARIANT Index);
        
        END_INTERFACE
    } IReportRevisionsVtbl;

    interface IReportRevisions
    {
        CONST_VTBL struct IReportRevisionsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReportRevisions_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IReportRevisions_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IReportRevisions_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IReportRevisions_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IReportRevisions_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IReportRevisions_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IReportRevisions_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IReportRevisions_get_Item(This,Index,Info)	\
    ( (This)->lpVtbl -> get_Item(This,Index,Info) ) 

#define IReportRevisions_get__NewEnum(This,Unknown)	\
    ( (This)->lpVtbl -> get__NewEnum(This,Unknown) ) 

#define IReportRevisions_get_Count(This,Count)	\
    ( (This)->lpVtbl -> get_Count(This,Count) ) 

#define IReportRevisions_Add(This,Entry)	\
    ( (This)->lpVtbl -> Add(This,Entry) ) 

#define IReportRevisions_Remove(This,Index)	\
    ( (This)->lpVtbl -> Remove(This,Index) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IReportRevisions_INTERFACE_DEFINED__ */


#ifndef __IReportRevision_INTERFACE_DEFINED__
#define __IReportRevision_INTERFACE_DEFINED__

/* interface IReportRevision */
/* [unique][helpstring][dual][uuid][nonextensible][object] */ 


EXTERN_C const IID IID_IReportRevision;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3131037F-A183-41D5-BD2B-6B8EA1659C49")
    IReportRevision : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Info( 
            /* [retval][out] */ IReportInfo **Info) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Entries( 
            /* [retval][out] */ IReportEntries **Entries) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsOpen( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OtherFindings( 
            /* [retval][out] */ IReportOtherFindings **OtherFindings) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_NonDistendedFindings( 
            /* [retval][out] */ IReportNonDistendedFindings **NonDistendedFindings) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MajorVersion( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MinorVersion( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsReadOnly( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MarkedCompleted( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MarkedCompleted( 
            /* [in] */ VARIANT_BOOL val) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IReportRevisionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IReportRevision * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IReportRevision * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IReportRevision * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IReportRevision * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IReportRevision * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IReportRevision * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IReportRevision * This,
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
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Info )( 
            IReportRevision * This,
            /* [retval][out] */ IReportInfo **Info);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Entries )( 
            IReportRevision * This,
            /* [retval][out] */ IReportEntries **Entries);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsOpen )( 
            IReportRevision * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OtherFindings )( 
            IReportRevision * This,
            /* [retval][out] */ IReportOtherFindings **OtherFindings);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NonDistendedFindings )( 
            IReportRevision * This,
            /* [retval][out] */ IReportNonDistendedFindings **NonDistendedFindings);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MajorVersion )( 
            IReportRevision * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MinorVersion )( 
            IReportRevision * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsReadOnly )( 
            IReportRevision * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MarkedCompleted )( 
            IReportRevision * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_MarkedCompleted )( 
            IReportRevision * This,
            /* [in] */ VARIANT_BOOL val);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Save )( 
            IReportRevision * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IReportRevision * This);
        
        END_INTERFACE
    } IReportRevisionVtbl;

    interface IReportRevision
    {
        CONST_VTBL struct IReportRevisionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReportRevision_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IReportRevision_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IReportRevision_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IReportRevision_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IReportRevision_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IReportRevision_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IReportRevision_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IReportRevision_get_Info(This,Info)	\
    ( (This)->lpVtbl -> get_Info(This,Info) ) 

#define IReportRevision_get_Entries(This,Entries)	\
    ( (This)->lpVtbl -> get_Entries(This,Entries) ) 

#define IReportRevision_get_IsOpen(This,pVal)	\
    ( (This)->lpVtbl -> get_IsOpen(This,pVal) ) 

#define IReportRevision_get_OtherFindings(This,OtherFindings)	\
    ( (This)->lpVtbl -> get_OtherFindings(This,OtherFindings) ) 

#define IReportRevision_get_NonDistendedFindings(This,NonDistendedFindings)	\
    ( (This)->lpVtbl -> get_NonDistendedFindings(This,NonDistendedFindings) ) 

#define IReportRevision_get_MajorVersion(This,pVal)	\
    ( (This)->lpVtbl -> get_MajorVersion(This,pVal) ) 

#define IReportRevision_get_MinorVersion(This,pVal)	\
    ( (This)->lpVtbl -> get_MinorVersion(This,pVal) ) 

#define IReportRevision_get_IsReadOnly(This,pVal)	\
    ( (This)->lpVtbl -> get_IsReadOnly(This,pVal) ) 

#define IReportRevision_get_MarkedCompleted(This,pVal)	\
    ( (This)->lpVtbl -> get_MarkedCompleted(This,pVal) ) 

#define IReportRevision_put_MarkedCompleted(This,val)	\
    ( (This)->lpVtbl -> put_MarkedCompleted(This,val) ) 

#define IReportRevision_Save(This)	\
    ( (This)->lpVtbl -> Save(This) ) 

#define IReportRevision_Close(This)	\
    ( (This)->lpVtbl -> Close(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IReportRevision_INTERFACE_DEFINED__ */


#ifndef __IReportEntries_INTERFACE_DEFINED__
#define __IReportEntries_INTERFACE_DEFINED__

/* interface IReportEntries */
/* [unique][helpstring][dual][uuid][nonextensible][object] */ 


EXTERN_C const IID IID_IReportEntries;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D78AF9DD-F87F-4EE2-B12E-D4FA5F0099CA")
    IReportEntries : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IReportEntry **Info) = 0;
        
        virtual /* [restricted][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **Unknown) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *Count) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [retval][out] */ IReportEntry **Entry) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ VARIANT Index) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IReportEntriesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IReportEntries * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IReportEntries * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IReportEntries * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IReportEntries * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IReportEntries * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IReportEntries * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IReportEntries * This,
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
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            IReportEntries * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IReportEntry **Info);
        
        /* [restricted][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IReportEntries * This,
            /* [retval][out] */ IUnknown **Unknown);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IReportEntries * This,
            /* [retval][out] */ long *Count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IReportEntries * This,
            /* [retval][out] */ IReportEntry **Entry);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IReportEntries * This,
            /* [in] */ VARIANT Index);
        
        END_INTERFACE
    } IReportEntriesVtbl;

    interface IReportEntries
    {
        CONST_VTBL struct IReportEntriesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReportEntries_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IReportEntries_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IReportEntries_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IReportEntries_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IReportEntries_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IReportEntries_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IReportEntries_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IReportEntries_get_Item(This,Index,Info)	\
    ( (This)->lpVtbl -> get_Item(This,Index,Info) ) 

#define IReportEntries_get__NewEnum(This,Unknown)	\
    ( (This)->lpVtbl -> get__NewEnum(This,Unknown) ) 

#define IReportEntries_get_Count(This,Count)	\
    ( (This)->lpVtbl -> get_Count(This,Count) ) 

#define IReportEntries_Add(This,Entry)	\
    ( (This)->lpVtbl -> Add(This,Entry) ) 

#define IReportEntries_Remove(This,Index)	\
    ( (This)->lpVtbl -> Remove(This,Index) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IReportEntries_INTERFACE_DEFINED__ */


#ifndef __IReportEntry_INTERFACE_DEFINED__
#define __IReportEntry_INTERFACE_DEFINED__

/* interface IReportEntry */
/* [unique][helpstring][dual][uuid][nonextensible][object] */ 


EXTERN_C const IID IID_IReportEntry;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0CF9B742-ECAE-423A-B347-D7CBFDA9901C")
    IReportEntry : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ID( 
            /* [retval][out] */ BSTR *EntryID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Images( 
            /* [retval][out] */ IReportImages **Images) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Comments( 
            /* [retval][out] */ BSTR *Comments) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Comments( 
            /* [in] */ BSTR Comments) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Location( 
            /* [retval][out] */ LocationEnum *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Location( 
            /* [in] */ LocationEnum newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Shape( 
            /* [retval][out] */ ShapeEnum *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Shape( 
            /* [in] */ ShapeEnum newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SizeComparison( 
            /* [retval][out] */ SizeComparisonEnum *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SizeComparison( 
            /* [in] */ SizeComparisonEnum newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Translucency( 
            /* [retval][out] */ TranslucencyEnum *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Translucency( 
            /* [in] */ TranslucencyEnum newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Maxiumum( 
            /* [retval][out] */ float *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Maxiumum( 
            /* [in] */ float newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Perpendicular( 
            /* [retval][out] */ float *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Perpendicular( 
            /* [in] */ float newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ColonFinding( 
            /* [retval][out] */ ColonFindingEnum *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ColonFinding( 
            /* [in] */ ColonFindingEnum newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CameraCoordinates( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CameraCoordinates( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OrigDistanceToRectum( 
            /* [retval][out] */ float *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OrigDistanceToRectum( 
            /* [in] */ float newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OtherFindingID( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OtherFindingID( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Version( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Version( 
            /* [in] */ int newVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IReportEntryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IReportEntry * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IReportEntry * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IReportEntry * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IReportEntry * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IReportEntry * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IReportEntry * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IReportEntry * This,
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
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ID )( 
            IReportEntry * This,
            /* [retval][out] */ BSTR *EntryID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IReportEntry * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            IReportEntry * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Images )( 
            IReportEntry * This,
            /* [retval][out] */ IReportImages **Images);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Comments )( 
            IReportEntry * This,
            /* [retval][out] */ BSTR *Comments);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Comments )( 
            IReportEntry * This,
            /* [in] */ BSTR Comments);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Location )( 
            IReportEntry * This,
            /* [retval][out] */ LocationEnum *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Location )( 
            IReportEntry * This,
            /* [in] */ LocationEnum newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Shape )( 
            IReportEntry * This,
            /* [retval][out] */ ShapeEnum *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Shape )( 
            IReportEntry * This,
            /* [in] */ ShapeEnum newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SizeComparison )( 
            IReportEntry * This,
            /* [retval][out] */ SizeComparisonEnum *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SizeComparison )( 
            IReportEntry * This,
            /* [in] */ SizeComparisonEnum newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Translucency )( 
            IReportEntry * This,
            /* [retval][out] */ TranslucencyEnum *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Translucency )( 
            IReportEntry * This,
            /* [in] */ TranslucencyEnum newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Maxiumum )( 
            IReportEntry * This,
            /* [retval][out] */ float *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Maxiumum )( 
            IReportEntry * This,
            /* [in] */ float newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Perpendicular )( 
            IReportEntry * This,
            /* [retval][out] */ float *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Perpendicular )( 
            IReportEntry * This,
            /* [in] */ float newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ColonFinding )( 
            IReportEntry * This,
            /* [retval][out] */ ColonFindingEnum *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ColonFinding )( 
            IReportEntry * This,
            /* [in] */ ColonFindingEnum newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CameraCoordinates )( 
            IReportEntry * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CameraCoordinates )( 
            IReportEntry * This,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OrigDistanceToRectum )( 
            IReportEntry * This,
            /* [retval][out] */ float *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OrigDistanceToRectum )( 
            IReportEntry * This,
            /* [in] */ float newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OtherFindingID )( 
            IReportEntry * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OtherFindingID )( 
            IReportEntry * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Version )( 
            IReportEntry * This,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Version )( 
            IReportEntry * This,
            /* [in] */ int newVal);
        
        END_INTERFACE
    } IReportEntryVtbl;

    interface IReportEntry
    {
        CONST_VTBL struct IReportEntryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReportEntry_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IReportEntry_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IReportEntry_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IReportEntry_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IReportEntry_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IReportEntry_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IReportEntry_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IReportEntry_get_ID(This,EntryID)	\
    ( (This)->lpVtbl -> get_ID(This,EntryID) ) 

#define IReportEntry_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define IReportEntry_put_Name(This,newVal)	\
    ( (This)->lpVtbl -> put_Name(This,newVal) ) 

#define IReportEntry_get_Images(This,Images)	\
    ( (This)->lpVtbl -> get_Images(This,Images) ) 

#define IReportEntry_get_Comments(This,Comments)	\
    ( (This)->lpVtbl -> get_Comments(This,Comments) ) 

#define IReportEntry_put_Comments(This,Comments)	\
    ( (This)->lpVtbl -> put_Comments(This,Comments) ) 

#define IReportEntry_get_Location(This,pVal)	\
    ( (This)->lpVtbl -> get_Location(This,pVal) ) 

#define IReportEntry_put_Location(This,newVal)	\
    ( (This)->lpVtbl -> put_Location(This,newVal) ) 

#define IReportEntry_get_Shape(This,pVal)	\
    ( (This)->lpVtbl -> get_Shape(This,pVal) ) 

#define IReportEntry_put_Shape(This,newVal)	\
    ( (This)->lpVtbl -> put_Shape(This,newVal) ) 

#define IReportEntry_get_SizeComparison(This,pVal)	\
    ( (This)->lpVtbl -> get_SizeComparison(This,pVal) ) 

#define IReportEntry_put_SizeComparison(This,newVal)	\
    ( (This)->lpVtbl -> put_SizeComparison(This,newVal) ) 

#define IReportEntry_get_Translucency(This,pVal)	\
    ( (This)->lpVtbl -> get_Translucency(This,pVal) ) 

#define IReportEntry_put_Translucency(This,newVal)	\
    ( (This)->lpVtbl -> put_Translucency(This,newVal) ) 

#define IReportEntry_get_Maxiumum(This,pVal)	\
    ( (This)->lpVtbl -> get_Maxiumum(This,pVal) ) 

#define IReportEntry_put_Maxiumum(This,newVal)	\
    ( (This)->lpVtbl -> put_Maxiumum(This,newVal) ) 

#define IReportEntry_get_Perpendicular(This,pVal)	\
    ( (This)->lpVtbl -> get_Perpendicular(This,pVal) ) 

#define IReportEntry_put_Perpendicular(This,newVal)	\
    ( (This)->lpVtbl -> put_Perpendicular(This,newVal) ) 

#define IReportEntry_get_ColonFinding(This,pVal)	\
    ( (This)->lpVtbl -> get_ColonFinding(This,pVal) ) 

#define IReportEntry_put_ColonFinding(This,newVal)	\
    ( (This)->lpVtbl -> put_ColonFinding(This,newVal) ) 

#define IReportEntry_get_CameraCoordinates(This,pVal)	\
    ( (This)->lpVtbl -> get_CameraCoordinates(This,pVal) ) 

#define IReportEntry_put_CameraCoordinates(This,newVal)	\
    ( (This)->lpVtbl -> put_CameraCoordinates(This,newVal) ) 

#define IReportEntry_get_OrigDistanceToRectum(This,pVal)	\
    ( (This)->lpVtbl -> get_OrigDistanceToRectum(This,pVal) ) 

#define IReportEntry_put_OrigDistanceToRectum(This,newVal)	\
    ( (This)->lpVtbl -> put_OrigDistanceToRectum(This,newVal) ) 

#define IReportEntry_get_OtherFindingID(This,pVal)	\
    ( (This)->lpVtbl -> get_OtherFindingID(This,pVal) ) 

#define IReportEntry_put_OtherFindingID(This,newVal)	\
    ( (This)->lpVtbl -> put_OtherFindingID(This,newVal) ) 

#define IReportEntry_get_Version(This,pVal)	\
    ( (This)->lpVtbl -> get_Version(This,pVal) ) 

#define IReportEntry_put_Version(This,newVal)	\
    ( (This)->lpVtbl -> put_Version(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IReportEntry_INTERFACE_DEFINED__ */


#ifndef __IReportImages_INTERFACE_DEFINED__
#define __IReportImages_INTERFACE_DEFINED__

/* interface IReportImages */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IReportImages;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CCA2B720-A0DF-4AE9-9D21-3A168A922938")
    IReportImages : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IImage **Image) = 0;
        
        virtual /* [restricted][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **Unknown) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *Count) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ IImage *Image) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ VARIANT Index) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IReportImagesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IReportImages * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IReportImages * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IReportImages * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IReportImages * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IReportImages * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IReportImages * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IReportImages * This,
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
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            IReportImages * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IImage **Image);
        
        /* [restricted][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IReportImages * This,
            /* [retval][out] */ IUnknown **Unknown);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IReportImages * This,
            /* [retval][out] */ long *Count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IReportImages * This,
            /* [in] */ IImage *Image);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IReportImages * This,
            /* [in] */ VARIANT Index);
        
        END_INTERFACE
    } IReportImagesVtbl;

    interface IReportImages
    {
        CONST_VTBL struct IReportImagesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReportImages_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IReportImages_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IReportImages_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IReportImages_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IReportImages_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IReportImages_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IReportImages_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IReportImages_get_Item(This,Index,Image)	\
    ( (This)->lpVtbl -> get_Item(This,Index,Image) ) 

#define IReportImages_get__NewEnum(This,Unknown)	\
    ( (This)->lpVtbl -> get__NewEnum(This,Unknown) ) 

#define IReportImages_get_Count(This,Count)	\
    ( (This)->lpVtbl -> get_Count(This,Count) ) 

#define IReportImages_Add(This,Image)	\
    ( (This)->lpVtbl -> Add(This,Image) ) 

#define IReportImages_Remove(This,Index)	\
    ( (This)->lpVtbl -> Remove(This,Index) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IReportImages_INTERFACE_DEFINED__ */


#ifndef __IReportInfo_INTERFACE_DEFINED__
#define __IReportInfo_INTERFACE_DEFINED__

/* interface IReportInfo */
/* [unique][helpstring][dual][uuid][nonextensible][object] */ 


EXTERN_C const IID IID_IReportInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7D7F3959-ECF1-42B7-8D82-89347D0AB381")
    IReportInfo : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PatientName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PatientName( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PatientID( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PatientID( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StudyDate( 
            /* [retval][out] */ DATE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StudyDate( 
            /* [in] */ DATE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Endoscopist( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Endoscopist( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ReferringDoctor( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ReferringDoctor( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PatientBirthday( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PatientBirthday( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PatientAge( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PatientAge( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PatientWeight( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PatientWeight( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PatientSex( 
            /* [retval][out] */ PatientSexEnum *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PatientSex( 
            /* [in] */ PatientSexEnum newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_InstitutionName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_InstitutionName( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Indication( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Indication( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OtherIndication( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OtherIndication( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Recommendation( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Recommendation( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RecommendationType( 
            /* [retval][out] */ RecommendationTypeEnum *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RecommendationType( 
            /* [in] */ RecommendationTypeEnum newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RecommendationUnit( 
            /* [retval][out] */ RecommendationUnitEnum *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RecommendationUnit( 
            /* [in] */ RecommendationUnitEnum newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RecommendationTime( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RecommendationTime( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OtherRecommendation( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OtherRecommendation( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Author( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Author( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Impression( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Impression( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CRADSCategory( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CRADSCategory( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ExtraColonicCRADSCategory( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ExamQuality( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ExamQuality( 
            /* [in] */ long newVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IReportInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IReportInfo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IReportInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IReportInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IReportInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IReportInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IReportInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IReportInfo * This,
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
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PatientName )( 
            IReportInfo * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PatientName )( 
            IReportInfo * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PatientID )( 
            IReportInfo * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PatientID )( 
            IReportInfo * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StudyDate )( 
            IReportInfo * This,
            /* [retval][out] */ DATE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_StudyDate )( 
            IReportInfo * This,
            /* [in] */ DATE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Endoscopist )( 
            IReportInfo * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Endoscopist )( 
            IReportInfo * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ReferringDoctor )( 
            IReportInfo * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ReferringDoctor )( 
            IReportInfo * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PatientBirthday )( 
            IReportInfo * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PatientBirthday )( 
            IReportInfo * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PatientAge )( 
            IReportInfo * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PatientAge )( 
            IReportInfo * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PatientWeight )( 
            IReportInfo * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PatientWeight )( 
            IReportInfo * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PatientSex )( 
            IReportInfo * This,
            /* [retval][out] */ PatientSexEnum *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PatientSex )( 
            IReportInfo * This,
            /* [in] */ PatientSexEnum newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InstitutionName )( 
            IReportInfo * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InstitutionName )( 
            IReportInfo * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Indication )( 
            IReportInfo * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Indication )( 
            IReportInfo * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OtherIndication )( 
            IReportInfo * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OtherIndication )( 
            IReportInfo * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Recommendation )( 
            IReportInfo * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Recommendation )( 
            IReportInfo * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RecommendationType )( 
            IReportInfo * This,
            /* [retval][out] */ RecommendationTypeEnum *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_RecommendationType )( 
            IReportInfo * This,
            /* [in] */ RecommendationTypeEnum newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RecommendationUnit )( 
            IReportInfo * This,
            /* [retval][out] */ RecommendationUnitEnum *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_RecommendationUnit )( 
            IReportInfo * This,
            /* [in] */ RecommendationUnitEnum newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RecommendationTime )( 
            IReportInfo * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_RecommendationTime )( 
            IReportInfo * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OtherRecommendation )( 
            IReportInfo * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OtherRecommendation )( 
            IReportInfo * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Author )( 
            IReportInfo * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Author )( 
            IReportInfo * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Impression )( 
            IReportInfo * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Impression )( 
            IReportInfo * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CRADSCategory )( 
            IReportInfo * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CRADSCategory )( 
            IReportInfo * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ExtraColonicCRADSCategory )( 
            IReportInfo * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ExamQuality )( 
            IReportInfo * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ExamQuality )( 
            IReportInfo * This,
            /* [in] */ long newVal);
        
        END_INTERFACE
    } IReportInfoVtbl;

    interface IReportInfo
    {
        CONST_VTBL struct IReportInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReportInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IReportInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IReportInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IReportInfo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IReportInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IReportInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IReportInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IReportInfo_get_PatientName(This,pVal)	\
    ( (This)->lpVtbl -> get_PatientName(This,pVal) ) 

#define IReportInfo_put_PatientName(This,newVal)	\
    ( (This)->lpVtbl -> put_PatientName(This,newVal) ) 

#define IReportInfo_get_PatientID(This,pVal)	\
    ( (This)->lpVtbl -> get_PatientID(This,pVal) ) 

#define IReportInfo_put_PatientID(This,newVal)	\
    ( (This)->lpVtbl -> put_PatientID(This,newVal) ) 

#define IReportInfo_get_StudyDate(This,pVal)	\
    ( (This)->lpVtbl -> get_StudyDate(This,pVal) ) 

#define IReportInfo_put_StudyDate(This,newVal)	\
    ( (This)->lpVtbl -> put_StudyDate(This,newVal) ) 

#define IReportInfo_get_Endoscopist(This,pVal)	\
    ( (This)->lpVtbl -> get_Endoscopist(This,pVal) ) 

#define IReportInfo_put_Endoscopist(This,newVal)	\
    ( (This)->lpVtbl -> put_Endoscopist(This,newVal) ) 

#define IReportInfo_get_ReferringDoctor(This,pVal)	\
    ( (This)->lpVtbl -> get_ReferringDoctor(This,pVal) ) 

#define IReportInfo_put_ReferringDoctor(This,newVal)	\
    ( (This)->lpVtbl -> put_ReferringDoctor(This,newVal) ) 

#define IReportInfo_get_PatientBirthday(This,pVal)	\
    ( (This)->lpVtbl -> get_PatientBirthday(This,pVal) ) 

#define IReportInfo_put_PatientBirthday(This,newVal)	\
    ( (This)->lpVtbl -> put_PatientBirthday(This,newVal) ) 

#define IReportInfo_get_PatientAge(This,pVal)	\
    ( (This)->lpVtbl -> get_PatientAge(This,pVal) ) 

#define IReportInfo_put_PatientAge(This,newVal)	\
    ( (This)->lpVtbl -> put_PatientAge(This,newVal) ) 

#define IReportInfo_get_PatientWeight(This,pVal)	\
    ( (This)->lpVtbl -> get_PatientWeight(This,pVal) ) 

#define IReportInfo_put_PatientWeight(This,newVal)	\
    ( (This)->lpVtbl -> put_PatientWeight(This,newVal) ) 

#define IReportInfo_get_PatientSex(This,pVal)	\
    ( (This)->lpVtbl -> get_PatientSex(This,pVal) ) 

#define IReportInfo_put_PatientSex(This,newVal)	\
    ( (This)->lpVtbl -> put_PatientSex(This,newVal) ) 

#define IReportInfo_get_InstitutionName(This,pVal)	\
    ( (This)->lpVtbl -> get_InstitutionName(This,pVal) ) 

#define IReportInfo_put_InstitutionName(This,newVal)	\
    ( (This)->lpVtbl -> put_InstitutionName(This,newVal) ) 

#define IReportInfo_get_Indication(This,pVal)	\
    ( (This)->lpVtbl -> get_Indication(This,pVal) ) 

#define IReportInfo_put_Indication(This,newVal)	\
    ( (This)->lpVtbl -> put_Indication(This,newVal) ) 

#define IReportInfo_get_OtherIndication(This,pVal)	\
    ( (This)->lpVtbl -> get_OtherIndication(This,pVal) ) 

#define IReportInfo_put_OtherIndication(This,newVal)	\
    ( (This)->lpVtbl -> put_OtherIndication(This,newVal) ) 

#define IReportInfo_get_Recommendation(This,pVal)	\
    ( (This)->lpVtbl -> get_Recommendation(This,pVal) ) 

#define IReportInfo_put_Recommendation(This,newVal)	\
    ( (This)->lpVtbl -> put_Recommendation(This,newVal) ) 

#define IReportInfo_get_RecommendationType(This,pVal)	\
    ( (This)->lpVtbl -> get_RecommendationType(This,pVal) ) 

#define IReportInfo_put_RecommendationType(This,newVal)	\
    ( (This)->lpVtbl -> put_RecommendationType(This,newVal) ) 

#define IReportInfo_get_RecommendationUnit(This,pVal)	\
    ( (This)->lpVtbl -> get_RecommendationUnit(This,pVal) ) 

#define IReportInfo_put_RecommendationUnit(This,newVal)	\
    ( (This)->lpVtbl -> put_RecommendationUnit(This,newVal) ) 

#define IReportInfo_get_RecommendationTime(This,pVal)	\
    ( (This)->lpVtbl -> get_RecommendationTime(This,pVal) ) 

#define IReportInfo_put_RecommendationTime(This,newVal)	\
    ( (This)->lpVtbl -> put_RecommendationTime(This,newVal) ) 

#define IReportInfo_get_OtherRecommendation(This,pVal)	\
    ( (This)->lpVtbl -> get_OtherRecommendation(This,pVal) ) 

#define IReportInfo_put_OtherRecommendation(This,newVal)	\
    ( (This)->lpVtbl -> put_OtherRecommendation(This,newVal) ) 

#define IReportInfo_get_Author(This,pVal)	\
    ( (This)->lpVtbl -> get_Author(This,pVal) ) 

#define IReportInfo_put_Author(This,newVal)	\
    ( (This)->lpVtbl -> put_Author(This,newVal) ) 

#define IReportInfo_get_Impression(This,pVal)	\
    ( (This)->lpVtbl -> get_Impression(This,pVal) ) 

#define IReportInfo_put_Impression(This,newVal)	\
    ( (This)->lpVtbl -> put_Impression(This,newVal) ) 

#define IReportInfo_get_CRADSCategory(This,pVal)	\
    ( (This)->lpVtbl -> get_CRADSCategory(This,pVal) ) 

#define IReportInfo_put_CRADSCategory(This,newVal)	\
    ( (This)->lpVtbl -> put_CRADSCategory(This,newVal) ) 

#define IReportInfo_get_ExtraColonicCRADSCategory(This,pVal)	\
    ( (This)->lpVtbl -> get_ExtraColonicCRADSCategory(This,pVal) ) 

#define IReportInfo_get_ExamQuality(This,pVal)	\
    ( (This)->lpVtbl -> get_ExamQuality(This,pVal) ) 

#define IReportInfo_put_ExamQuality(This,newVal)	\
    ( (This)->lpVtbl -> put_ExamQuality(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IReportInfo_INTERFACE_DEFINED__ */


#ifndef __IReportOtherFindings_INTERFACE_DEFINED__
#define __IReportOtherFindings_INTERFACE_DEFINED__

/* interface IReportOtherFindings */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IReportOtherFindings;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FF2105C1-AE1C-40F7-8A6F-6E41506E585E")
    IReportOtherFindings : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IReportOtherFinding **Info) = 0;
        
        virtual /* [restricted][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **Unknown) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *Count) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [retval][out] */ IReportOtherFinding **Entry) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ VARIANT Index) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IReportOtherFindingsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IReportOtherFindings * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IReportOtherFindings * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IReportOtherFindings * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IReportOtherFindings * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IReportOtherFindings * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IReportOtherFindings * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IReportOtherFindings * This,
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
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            IReportOtherFindings * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IReportOtherFinding **Info);
        
        /* [restricted][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IReportOtherFindings * This,
            /* [retval][out] */ IUnknown **Unknown);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IReportOtherFindings * This,
            /* [retval][out] */ long *Count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IReportOtherFindings * This,
            /* [retval][out] */ IReportOtherFinding **Entry);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IReportOtherFindings * This,
            /* [in] */ VARIANT Index);
        
        END_INTERFACE
    } IReportOtherFindingsVtbl;

    interface IReportOtherFindings
    {
        CONST_VTBL struct IReportOtherFindingsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReportOtherFindings_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IReportOtherFindings_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IReportOtherFindings_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IReportOtherFindings_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IReportOtherFindings_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IReportOtherFindings_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IReportOtherFindings_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IReportOtherFindings_get_Item(This,Index,Info)	\
    ( (This)->lpVtbl -> get_Item(This,Index,Info) ) 

#define IReportOtherFindings_get__NewEnum(This,Unknown)	\
    ( (This)->lpVtbl -> get__NewEnum(This,Unknown) ) 

#define IReportOtherFindings_get_Count(This,Count)	\
    ( (This)->lpVtbl -> get_Count(This,Count) ) 

#define IReportOtherFindings_Add(This,Entry)	\
    ( (This)->lpVtbl -> Add(This,Entry) ) 

#define IReportOtherFindings_Remove(This,Index)	\
    ( (This)->lpVtbl -> Remove(This,Index) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IReportOtherFindings_INTERFACE_DEFINED__ */


#ifndef __IReportOtherFinding_INTERFACE_DEFINED__
#define __IReportOtherFinding_INTERFACE_DEFINED__

/* interface IReportOtherFinding */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IReportOtherFinding;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C23BEF18-3DB4-43D4-B358-65DAD33B22A8")
    IReportOtherFinding : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Finding( 
            /* [retval][out] */ OtherFindingsEnum *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Finding( 
            /* [in] */ OtherFindingsEnum newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Location( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Location( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MaximumSize( 
            /* [retval][out] */ float *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MaximumSize( 
            /* [in] */ float newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PerpendicularSize( 
            /* [retval][out] */ float *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PerpendicularSize( 
            /* [in] */ float newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Length( 
            /* [retval][out] */ float *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Length( 
            /* [in] */ float newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Degree( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Degree( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Comment( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Comment( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ID( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Size( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CRADSCategory( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CRADSCategory( 
            /* [in] */ long newVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IReportOtherFindingVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IReportOtherFinding * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IReportOtherFinding * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IReportOtherFinding * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IReportOtherFinding * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IReportOtherFinding * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IReportOtherFinding * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IReportOtherFinding * This,
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
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Finding )( 
            IReportOtherFinding * This,
            /* [retval][out] */ OtherFindingsEnum *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Finding )( 
            IReportOtherFinding * This,
            /* [in] */ OtherFindingsEnum newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Location )( 
            IReportOtherFinding * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Location )( 
            IReportOtherFinding * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MaximumSize )( 
            IReportOtherFinding * This,
            /* [retval][out] */ float *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_MaximumSize )( 
            IReportOtherFinding * This,
            /* [in] */ float newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PerpendicularSize )( 
            IReportOtherFinding * This,
            /* [retval][out] */ float *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PerpendicularSize )( 
            IReportOtherFinding * This,
            /* [in] */ float newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Length )( 
            IReportOtherFinding * This,
            /* [retval][out] */ float *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Length )( 
            IReportOtherFinding * This,
            /* [in] */ float newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Degree )( 
            IReportOtherFinding * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Degree )( 
            IReportOtherFinding * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Comment )( 
            IReportOtherFinding * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Comment )( 
            IReportOtherFinding * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ID )( 
            IReportOtherFinding * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            IReportOtherFinding * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Size )( 
            IReportOtherFinding * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CRADSCategory )( 
            IReportOtherFinding * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CRADSCategory )( 
            IReportOtherFinding * This,
            /* [in] */ long newVal);
        
        END_INTERFACE
    } IReportOtherFindingVtbl;

    interface IReportOtherFinding
    {
        CONST_VTBL struct IReportOtherFindingVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReportOtherFinding_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IReportOtherFinding_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IReportOtherFinding_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IReportOtherFinding_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IReportOtherFinding_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IReportOtherFinding_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IReportOtherFinding_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IReportOtherFinding_get_Finding(This,pVal)	\
    ( (This)->lpVtbl -> get_Finding(This,pVal) ) 

#define IReportOtherFinding_put_Finding(This,newVal)	\
    ( (This)->lpVtbl -> put_Finding(This,newVal) ) 

#define IReportOtherFinding_get_Location(This,pVal)	\
    ( (This)->lpVtbl -> get_Location(This,pVal) ) 

#define IReportOtherFinding_put_Location(This,newVal)	\
    ( (This)->lpVtbl -> put_Location(This,newVal) ) 

#define IReportOtherFinding_get_MaximumSize(This,pVal)	\
    ( (This)->lpVtbl -> get_MaximumSize(This,pVal) ) 

#define IReportOtherFinding_put_MaximumSize(This,newVal)	\
    ( (This)->lpVtbl -> put_MaximumSize(This,newVal) ) 

#define IReportOtherFinding_get_PerpendicularSize(This,pVal)	\
    ( (This)->lpVtbl -> get_PerpendicularSize(This,pVal) ) 

#define IReportOtherFinding_put_PerpendicularSize(This,newVal)	\
    ( (This)->lpVtbl -> put_PerpendicularSize(This,newVal) ) 

#define IReportOtherFinding_get_Length(This,pVal)	\
    ( (This)->lpVtbl -> get_Length(This,pVal) ) 

#define IReportOtherFinding_put_Length(This,newVal)	\
    ( (This)->lpVtbl -> put_Length(This,newVal) ) 

#define IReportOtherFinding_get_Degree(This,pVal)	\
    ( (This)->lpVtbl -> get_Degree(This,pVal) ) 

#define IReportOtherFinding_put_Degree(This,newVal)	\
    ( (This)->lpVtbl -> put_Degree(This,newVal) ) 

#define IReportOtherFinding_get_Comment(This,pVal)	\
    ( (This)->lpVtbl -> get_Comment(This,pVal) ) 

#define IReportOtherFinding_put_Comment(This,newVal)	\
    ( (This)->lpVtbl -> put_Comment(This,newVal) ) 

#define IReportOtherFinding_get_ID(This,pVal)	\
    ( (This)->lpVtbl -> get_ID(This,pVal) ) 

#define IReportOtherFinding_get_Size(This,pVal)	\
    ( (This)->lpVtbl -> get_Size(This,pVal) ) 

#define IReportOtherFinding_put_Size(This,newVal)	\
    ( (This)->lpVtbl -> put_Size(This,newVal) ) 

#define IReportOtherFinding_get_CRADSCategory(This,pVal)	\
    ( (This)->lpVtbl -> get_CRADSCategory(This,pVal) ) 

#define IReportOtherFinding_put_CRADSCategory(This,newVal)	\
    ( (This)->lpVtbl -> put_CRADSCategory(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IReportOtherFinding_INTERFACE_DEFINED__ */


#ifndef __IReportNonDistendedFindings_INTERFACE_DEFINED__
#define __IReportNonDistendedFindings_INTERFACE_DEFINED__

/* interface IReportNonDistendedFindings */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IReportNonDistendedFindings;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("263CE509-680C-4C74-AA77-FE0687F65EDD")
    IReportNonDistendedFindings : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IReportNonDistendedFinding **Finding) = 0;
        
        virtual /* [restricted][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **Unknown) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *Count) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [retval][out] */ IReportNonDistendedFinding **Finding) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ VARIANT Index) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IReportNonDistendedFindingsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IReportNonDistendedFindings * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IReportNonDistendedFindings * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IReportNonDistendedFindings * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IReportNonDistendedFindings * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IReportNonDistendedFindings * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IReportNonDistendedFindings * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IReportNonDistendedFindings * This,
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
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            IReportNonDistendedFindings * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IReportNonDistendedFinding **Finding);
        
        /* [restricted][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IReportNonDistendedFindings * This,
            /* [retval][out] */ IUnknown **Unknown);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IReportNonDistendedFindings * This,
            /* [retval][out] */ long *Count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IReportNonDistendedFindings * This,
            /* [retval][out] */ IReportNonDistendedFinding **Finding);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IReportNonDistendedFindings * This,
            /* [in] */ VARIANT Index);
        
        END_INTERFACE
    } IReportNonDistendedFindingsVtbl;

    interface IReportNonDistendedFindings
    {
        CONST_VTBL struct IReportNonDistendedFindingsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReportNonDistendedFindings_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IReportNonDistendedFindings_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IReportNonDistendedFindings_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IReportNonDistendedFindings_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IReportNonDistendedFindings_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IReportNonDistendedFindings_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IReportNonDistendedFindings_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IReportNonDistendedFindings_get_Item(This,Index,Finding)	\
    ( (This)->lpVtbl -> get_Item(This,Index,Finding) ) 

#define IReportNonDistendedFindings_get__NewEnum(This,Unknown)	\
    ( (This)->lpVtbl -> get__NewEnum(This,Unknown) ) 

#define IReportNonDistendedFindings_get_Count(This,Count)	\
    ( (This)->lpVtbl -> get_Count(This,Count) ) 

#define IReportNonDistendedFindings_Add(This,Finding)	\
    ( (This)->lpVtbl -> Add(This,Finding) ) 

#define IReportNonDistendedFindings_Remove(This,Index)	\
    ( (This)->lpVtbl -> Remove(This,Index) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IReportNonDistendedFindings_INTERFACE_DEFINED__ */


#ifndef __IReportNonDistendedFinding_INTERFACE_DEFINED__
#define __IReportNonDistendedFinding_INTERFACE_DEFINED__

/* interface IReportNonDistendedFinding */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IReportNonDistendedFinding;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0BC74E4C-E84F-4702-8D48-26C0C7E5C91D")
    IReportNonDistendedFinding : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ID( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Quaternion( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Quaternion( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ZoomFactor( 
            /* [retval][out] */ float *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ZoomFactor( 
            /* [in] */ float newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Text( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Text( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Location( 
            /* [retval][out] */ LocationEnum *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Location( 
            /* [in] */ LocationEnum newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RotationMatrix( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RotationMatrix( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TransferX( 
            /* [retval][out] */ float *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TransferX( 
            /* [in] */ float newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TransferY( 
            /* [retval][out] */ float *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TransferY( 
            /* [in] */ float newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewMode( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ViewMode( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Image( 
            /* [retval][out] */ IImage **pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Image( 
            /* [in] */ IImage *newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DatasetPositionEnumProp( 
            /* [retval][out] */ DatasetPositionEnumProp *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DatasetPositionEnumProp( 
            /* [in] */ DatasetPositionEnumProp newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Offset( 
            /* [retval][out] */ float *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Offset( 
            /* [in] */ float newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Position( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Position( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Direction( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Direction( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StartPoint( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StartPoint( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EndPoint( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EndPoint( 
            /* [in] */ VARIANT newVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IReportNonDistendedFindingVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IReportNonDistendedFinding * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IReportNonDistendedFinding * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IReportNonDistendedFinding * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IReportNonDistendedFinding * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IReportNonDistendedFinding * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IReportNonDistendedFinding * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IReportNonDistendedFinding * This,
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
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ID )( 
            IReportNonDistendedFinding * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Quaternion )( 
            IReportNonDistendedFinding * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Quaternion )( 
            IReportNonDistendedFinding * This,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ZoomFactor )( 
            IReportNonDistendedFinding * This,
            /* [retval][out] */ float *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ZoomFactor )( 
            IReportNonDistendedFinding * This,
            /* [in] */ float newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Text )( 
            IReportNonDistendedFinding * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Text )( 
            IReportNonDistendedFinding * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Location )( 
            IReportNonDistendedFinding * This,
            /* [retval][out] */ LocationEnum *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Location )( 
            IReportNonDistendedFinding * This,
            /* [in] */ LocationEnum newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RotationMatrix )( 
            IReportNonDistendedFinding * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_RotationMatrix )( 
            IReportNonDistendedFinding * This,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TransferX )( 
            IReportNonDistendedFinding * This,
            /* [retval][out] */ float *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TransferX )( 
            IReportNonDistendedFinding * This,
            /* [in] */ float newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TransferY )( 
            IReportNonDistendedFinding * This,
            /* [retval][out] */ float *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TransferY )( 
            IReportNonDistendedFinding * This,
            /* [in] */ float newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewMode )( 
            IReportNonDistendedFinding * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ViewMode )( 
            IReportNonDistendedFinding * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Image )( 
            IReportNonDistendedFinding * This,
            /* [retval][out] */ IImage **pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Image )( 
            IReportNonDistendedFinding * This,
            /* [in] */ IImage *newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DatasetPositionEnumProp )( 
            IReportNonDistendedFinding * This,
            /* [retval][out] */ DatasetPositionEnumProp *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DatasetPositionEnumProp )( 
            IReportNonDistendedFinding * This,
            /* [in] */ DatasetPositionEnumProp newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Offset )( 
            IReportNonDistendedFinding * This,
            /* [retval][out] */ float *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Offset )( 
            IReportNonDistendedFinding * This,
            /* [in] */ float newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Position )( 
            IReportNonDistendedFinding * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Position )( 
            IReportNonDistendedFinding * This,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Direction )( 
            IReportNonDistendedFinding * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Direction )( 
            IReportNonDistendedFinding * This,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StartPoint )( 
            IReportNonDistendedFinding * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_StartPoint )( 
            IReportNonDistendedFinding * This,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EndPoint )( 
            IReportNonDistendedFinding * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EndPoint )( 
            IReportNonDistendedFinding * This,
            /* [in] */ VARIANT newVal);
        
        END_INTERFACE
    } IReportNonDistendedFindingVtbl;

    interface IReportNonDistendedFinding
    {
        CONST_VTBL struct IReportNonDistendedFindingVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReportNonDistendedFinding_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IReportNonDistendedFinding_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IReportNonDistendedFinding_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IReportNonDistendedFinding_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IReportNonDistendedFinding_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IReportNonDistendedFinding_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IReportNonDistendedFinding_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IReportNonDistendedFinding_get_ID(This,pVal)	\
    ( (This)->lpVtbl -> get_ID(This,pVal) ) 

#define IReportNonDistendedFinding_get_Quaternion(This,pVal)	\
    ( (This)->lpVtbl -> get_Quaternion(This,pVal) ) 

#define IReportNonDistendedFinding_put_Quaternion(This,newVal)	\
    ( (This)->lpVtbl -> put_Quaternion(This,newVal) ) 

#define IReportNonDistendedFinding_get_ZoomFactor(This,pVal)	\
    ( (This)->lpVtbl -> get_ZoomFactor(This,pVal) ) 

#define IReportNonDistendedFinding_put_ZoomFactor(This,newVal)	\
    ( (This)->lpVtbl -> put_ZoomFactor(This,newVal) ) 

#define IReportNonDistendedFinding_get_Text(This,pVal)	\
    ( (This)->lpVtbl -> get_Text(This,pVal) ) 

#define IReportNonDistendedFinding_put_Text(This,newVal)	\
    ( (This)->lpVtbl -> put_Text(This,newVal) ) 

#define IReportNonDistendedFinding_get_Location(This,pVal)	\
    ( (This)->lpVtbl -> get_Location(This,pVal) ) 

#define IReportNonDistendedFinding_put_Location(This,newVal)	\
    ( (This)->lpVtbl -> put_Location(This,newVal) ) 

#define IReportNonDistendedFinding_get_RotationMatrix(This,pVal)	\
    ( (This)->lpVtbl -> get_RotationMatrix(This,pVal) ) 

#define IReportNonDistendedFinding_put_RotationMatrix(This,newVal)	\
    ( (This)->lpVtbl -> put_RotationMatrix(This,newVal) ) 

#define IReportNonDistendedFinding_get_TransferX(This,pVal)	\
    ( (This)->lpVtbl -> get_TransferX(This,pVal) ) 

#define IReportNonDistendedFinding_put_TransferX(This,newVal)	\
    ( (This)->lpVtbl -> put_TransferX(This,newVal) ) 

#define IReportNonDistendedFinding_get_TransferY(This,pVal)	\
    ( (This)->lpVtbl -> get_TransferY(This,pVal) ) 

#define IReportNonDistendedFinding_put_TransferY(This,newVal)	\
    ( (This)->lpVtbl -> put_TransferY(This,newVal) ) 

#define IReportNonDistendedFinding_get_ViewMode(This,pVal)	\
    ( (This)->lpVtbl -> get_ViewMode(This,pVal) ) 

#define IReportNonDistendedFinding_put_ViewMode(This,newVal)	\
    ( (This)->lpVtbl -> put_ViewMode(This,newVal) ) 

#define IReportNonDistendedFinding_get_Image(This,pVal)	\
    ( (This)->lpVtbl -> get_Image(This,pVal) ) 

#define IReportNonDistendedFinding_put_Image(This,newVal)	\
    ( (This)->lpVtbl -> put_Image(This,newVal) ) 

#define IReportNonDistendedFinding_get_DatasetPositionEnumProp(This,pVal)	\
    ( (This)->lpVtbl -> get_DatasetPositionEnumProp(This,pVal) ) 

#define IReportNonDistendedFinding_put_DatasetPositionEnumProp(This,newVal)	\
    ( (This)->lpVtbl -> put_DatasetPositionEnumProp(This,newVal) ) 

#define IReportNonDistendedFinding_get_Offset(This,pVal)	\
    ( (This)->lpVtbl -> get_Offset(This,pVal) ) 

#define IReportNonDistendedFinding_put_Offset(This,newVal)	\
    ( (This)->lpVtbl -> put_Offset(This,newVal) ) 

#define IReportNonDistendedFinding_get_Position(This,pVal)	\
    ( (This)->lpVtbl -> get_Position(This,pVal) ) 

#define IReportNonDistendedFinding_put_Position(This,newVal)	\
    ( (This)->lpVtbl -> put_Position(This,newVal) ) 

#define IReportNonDistendedFinding_get_Direction(This,pVal)	\
    ( (This)->lpVtbl -> get_Direction(This,pVal) ) 

#define IReportNonDistendedFinding_put_Direction(This,newVal)	\
    ( (This)->lpVtbl -> put_Direction(This,newVal) ) 

#define IReportNonDistendedFinding_get_StartPoint(This,pVal)	\
    ( (This)->lpVtbl -> get_StartPoint(This,pVal) ) 

#define IReportNonDistendedFinding_put_StartPoint(This,newVal)	\
    ( (This)->lpVtbl -> put_StartPoint(This,newVal) ) 

#define IReportNonDistendedFinding_get_EndPoint(This,pVal)	\
    ( (This)->lpVtbl -> get_EndPoint(This,pVal) ) 

#define IReportNonDistendedFinding_put_EndPoint(This,newVal)	\
    ( (This)->lpVtbl -> put_EndPoint(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IReportNonDistendedFinding_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Image;

#ifdef __cplusplus

class DECLSPEC_UUID("3886F2BB-AF89-4464-86FF-9058601F5F30")
Image;
#endif

EXTERN_C const CLSID CLSID_ReportRevision;

#ifdef __cplusplus

class DECLSPEC_UUID("3AF6831F-5732-4E82-A333-04654A790B85")
ReportRevision;
#endif

EXTERN_C const CLSID CLSID_ReportEntries;

#ifdef __cplusplus

class DECLSPEC_UUID("E579A075-8CE2-42BE-8320-1C831334706B")
ReportEntries;
#endif

EXTERN_C const CLSID CLSID_ReportEntry;

#ifdef __cplusplus

class DECLSPEC_UUID("D6F2B1B0-6371-4E64-B1CD-4AD6EB96FAE4")
ReportEntry;
#endif

EXTERN_C const CLSID CLSID_ReportInfo;

#ifdef __cplusplus

class DECLSPEC_UUID("845A2899-F269-456B-87E0-28A2F44914F6")
ReportInfo;
#endif

EXTERN_C const CLSID CLSID_ReportImages;

#ifdef __cplusplus

class DECLSPEC_UUID("7C64E04A-F396-4BA8-89E8-06593EF4A5D9")
ReportImages;
#endif

EXTERN_C const CLSID CLSID_ReportOtherFindings;

#ifdef __cplusplus

class DECLSPEC_UUID("ADAD89DF-E9EF-47B5-AF27-81CB6C67E8BB")
ReportOtherFindings;
#endif

EXTERN_C const CLSID CLSID_ReportOtherFinding;

#ifdef __cplusplus

class DECLSPEC_UUID("2EF75240-6113-46F8-AD06-C4A41B1FA0E1")
ReportOtherFinding;
#endif

EXTERN_C const CLSID CLSID_ReportNonDistendedFindings;

#ifdef __cplusplus

class DECLSPEC_UUID("80DE0E55-31F5-431F-85A5-E4493701C35E")
ReportNonDistendedFindings;
#endif

EXTERN_C const CLSID CLSID_ReportNonDistendedFinding;

#ifdef __cplusplus

class DECLSPEC_UUID("4A9562C8-9614-4422-A815-301E480D1EA6")
ReportNonDistendedFinding;
#endif

EXTERN_C const CLSID CLSID_Report;

#ifdef __cplusplus

class DECLSPEC_UUID("5A98EEB3-0CCF-4FBF-826C-4E3500A35A9B")
Report;
#endif

EXTERN_C const CLSID CLSID_ReportRevisions;

#ifdef __cplusplus

class DECLSPEC_UUID("09A2BA0C-1336-4981-B9AD-ACA0F8A592B3")
ReportRevisions;
#endif
#endif /* __ReportLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


