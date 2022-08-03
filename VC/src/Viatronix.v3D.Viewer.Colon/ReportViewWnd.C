// $Id: ReportViewWnd.C,v 1.37.2.15 2010/09/20 18:41:34 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Jeff Meade (Author: George Economos)


/**
 * Work area for colon findings reporting
 */


// includes
#include "stdafx.h"
#include "PersonName.h"
#include "ReportViewWnd.h"
#include "ReaderGlobal.h"
#include "MainAppFrame.h"
#include "ReportVariant.h"
#include "VxReportCreator.h"
#include "File.h"
#include "Viatron.h"
#include "Overview.h"
#include "VxVCReport.h"
#include "tlhelp32.h"
#include "DrawOverView.h"
#include "vxBlockVolumeIterator.h"
#include "Theme.h"
#include "user.h"
#include "Segment3DChildView.h"
#include "NavigationView.h"
#include "ExeResourceSwapper.h"

// defines
#define FILE_REVISION   "$Revision: 1.37.2.15 $"


// namespaces
using namespace ColonViews::Layouts;
using namespace ReaderLib;
using namespace WinUtils;
using namespace ConsoleLib;
using namespace ColonReport;
using namespace vxCAD;


#define ENTRY_CURRENT_VERSION  2
// Versions:
//  2: Added version info, CameraCoordinates of previous entry versions are Dataset-orientation dependent for old prone studies

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ReportLayout, ColonViatronixWnd)
  //{{AFX_MSG_MAP(ReportLayout)
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_WM_ERASEBKGND()
  ON_COMMAND(IDM_RPT_COLON_FINDINGS, OnColonFindings)
  ON_COMMAND(IDM_RPT_NON_DISTENDED, OnNonDistendedAreas)
  ON_COMMAND(IDM_RPT_OTHER_FINDINGS, OnOtherFindings)
  ON_COMMAND(IDM_RPT_RECOMMENDATIONS, OnRecommendations)
  ON_COMMAND(IDM_RPT_MISCELLANEOUS, OnMiscellaneous)
  ON_COMMAND(IDM_RPT_SAVE, OnReportSave)
  ON_COMMAND(IDM_OPEN_REPORT, OnOpenReport)
  ON_COMMAND(IDM_RPT_PREVIEW, OnPreviewReport)
  ON_COMMAND(ID_REPORT_MARKCOMPLETE, OnToggleReportCompleteMark)
  ON_WM_DESTROY()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

VxVCReport ReportLayout::m_report;


/**
 * Constructor 
 */
ReportLayout::ReportLayout() : ColonViatronixWnd( false ),
  m_pCurrentView(NULL),
  m_pDrawBuffer( NULL )
{
} // Constructor

  
/**
 * WM_DESTROY handler
 */
void ReportLayout::OnDestroy()
{
  if (m_pCurrentView)
  {
    m_pCurrentView->OnReportViewUnactivate();
  }

	Colon::FullScreenController::GetInstance().Disconnect( this );
} // OnDestroy()


/**
 * Destructor
 */
ReportLayout::~ReportLayout()
{
} // Destructor


/**
 * Sets the 'Marked Complete' flag for the study.
 *
 * @param bCompleted   Indicates whether or not the report is complete.
 * @return             Indicates success/failure
 */
bool ReportLayout::SetMarkCompleteFlag(const bool bCompleted)
{
  try
  {
    m_report.SetMarkedCompleteFlag(bCompleted);
  }
  catch(_com_error& e)
  {
    ReportWndBase::DisplayReportError(IDS_OPEN, e.Error());
    return false;
  }

  return true;
} // SetMarkCompleteFlag()


/**
 * Whether or not a report file for the current study exists.
 *
 * @return   TRUE if a report for the current study exists, FALSE otherwise.
 */
BOOL ReportLayout::ReportFileExists()
{
  MainAppFrame* pFrame = dynamic_cast<MainAppFrame*>(AfxGetApp()->m_pMainWnd);
  ASSERT(pFrame);

	std::string sReportFile = VxVCReport::GenerateReportFilename(pFrame->GetReportPrefix());
	return (vx::File(VxVCReport::m_sXMLFilename.c_str()).Exists() || vx::File(sReportFile.c_str()).Exists()) ? TRUE : FALSE;
} // ReportFileExists()


/**
 * Replaces patient information in the report.
 *
 * @param pszReport          Pathname of the report.
 * @param pszEnPatientName   Encrypted/anonymized patient name.
 * @param pszEnPatientID     Encrypted/anonymized patient ID.
 * @return                   Success of the anonymization.
 */
BOOL ReportLayout::ReplacePatientInfo(LPCTSTR pszReport, LPCTSTR pszEnPatientName, LPCTSTR pszEnPatientID)
{
  // Fail if file is inaccessible
  if (_access(pszReport, 0) == -1)
  {
    return FALSE;
  }

  // Fail if cannot clear read-only flag
  const DWORD dwOrigAttribs(GetFileAttributes(pszReport));
  if (!SetFileAttributes(pszReport, dwOrigAttribs & ~FILE_ATTRIBUTE_READONLY))
  {
    return FALSE;
  }

  // TODO: Fix these conflicting definitions
  ReportLib::IReportPtr spReport;
  if (FAILED(spReport.CreateInstance( __uuidof(ReportLib::Report))))
  {
    return FALSE;
  }

  spReport->Open(_bstr_t(pszReport));
  m_report.m_spRevision = spReport->Revisions->GetItem(0L);

  ReportLib::IReportInfoPtr spInfo = m_report.m_spRevision->Info;
  spInfo->PatientID = static_cast<LPCTSTR>(pszEnPatientID);
  spInfo->PatientName = static_cast<LPCTSTR>(pszEnPatientName);
  spInfo->PatientWeight = static_cast<LPCTSTR>("0");
  spInfo->PatientAge = static_cast<LPCTSTR>("0");
  spInfo->PatientSex = ReportLib::Male;
	
  m_report.m_spRevision->Save();
  m_report.m_spRevision->Close();
  m_report.m_spRevision = NULL;

  if ( spReport != NULL )
  {
    spReport->Close();
    spReport = NULL;
  }

  // Reset file attributes
  SetFileAttributes(pszReport, dwOrigAttribs);

  return TRUE;
} // ReplacePatientInfo()


/**
 * Adds an image entry to the report.
 *
 * @param childViewList   View windows for snapshot.
 * @return                -1 If report is not open,
 *                        -2 if report is read-only,
 *                         1 Otherwise
 */
BOOL ReportLayout::AddReportEntry(const std::list<GLChildViewPtr>& childViewList,
                                    const MeasureLineList& currMeasureList /* = MeasureLineList()*/)
{
  // BAND-AID HERE!!!! :-)
  if (m_report.m_spRevision==NULL || m_report.m_spRevision->IsOpen == VARIANT_FALSE)
  {
    return -1;
  }

  if (m_report.m_spRevision->IsReadOnly == VARIANT_TRUE)
  {
    return -2;
  }
  // (END) BAND-AID

  try
  {
    Dataset& currDataset = *rdrGlobal.m_pCurrDataset;

    ReportLib::IReportEntryPtr spEntry = m_report.m_spRevision->Entries->Add();
    spEntry->Comments = "";

    spEntry->Version = ENTRY_CURRENT_VERSION;

    std::vector<float4> vMeasurement;
    if ( ( (ViatronStudy::m_modeEndoView & ViewModes::SEGMENTPOLYP) || (ViatronStudy::m_modeOrthogView & ViewModes::SEGMENTPOLYP) )
      && ( currDataset.m_suspiciousRegions.GetCount() > 0 ) )
    {
      EndoluminalView::m_bForceDrawCurrentROI = true; // kludge, to draw current ROI during snapshot

      // default to polyp and add measurements if snapshot is a suspicious region
      const vxCadFinding& cadfind = currDataset.m_suspiciousRegions.GetFinding(currDataset.m_suspiciousRegions.GetCount()-1);
      vxCadDimension dim;
      dim = cadfind.GetDimensions().GetLargestDimension();
      vMeasurement.push_back((dim.GetEndPoint().GetVector() - dim.GetStartPoint().GetVector()).GetLength());
      dim = cadfind.GetDimensions().GetPerpendicularDimension();
      vMeasurement.push_back((dim.GetEndPoint().GetVector() - dim.GetStartPoint().GetVector()).GetLength());

			std::string sVolumeUnits(" mm^3");
      spEntry->Comments = (std::string(LPCTSTR(LoadResourceString(IDS_ROI_VOLUME_TXT)))
        + ToAscii(cadfind.GetVolumeROI().GetVolumeValue()) + sVolumeUnits).c_str();
    }
    else if (currMeasureList.size() > 0)
    {
      // When some measurements are taken & at least one is visible, default type is Polyp; otherwise, Normal ('Other').
      MeasureLineList::const_iterator listCurr = currMeasureList.begin();
      MeasureLineList::const_iterator listEnd  = currMeasureList.end();

      for (; listCurr!=listEnd; ++listCurr)
      {
        if (!(listCurr->m_ePosn == MeasureLine::NODISPLAY))
          vMeasurement.push_back(listCurr->m_f3Ddistance);
      }
    }
    // if some lines are visible, set up the value.
    if (vMeasurement.size() < 1)
    {
      spEntry->Maxiumum = 0.0f;
      spEntry->Perpendicular = 0.0f;
      spEntry->ColonFinding = ReportLib::Other_Normal;
    }
    else
    {
      std::sort(vMeasurement.begin(), vMeasurement.end(), std::greater<float>());
      spEntry->Maxiumum = vMeasurement[0];
      spEntry->Perpendicular = (vMeasurement.size() > 1) ? vMeasurement[1] : 0.0f;
      spEntry->ColonFinding = ReportLib::Polyp;
    }

    // add images to the report entry
    GLChildView::SetReportRendering(true);
    for (std::list<GLChildViewPtr>::const_iterator it = childViewList.begin(); it != childViewList.end(); it++)
    {
      AddImageToEntry(**it, spEntry);
    }
    GLChildView::SetReportRendering(false);

    EndoluminalView::m_bForceDrawCurrentROI = false; // kludge, to draw current ROI during snapshot

    CString sDistFromRectum;
    // added Supine&Prone marks.
    sDistFromRectum.Format( LoadResourceString( IDS_DISTANCE_FROM_RECTUM_FORMAT ), currDataset.GetCurrentDistanceFromRectumMM()/10);

    std::string sEntryName;
    sEntryName += "(";
    sEntryName += PatientOrientationToString(currDataset);
    sEntryName += std::string(") ") + LPCTSTR(sDistFromRectum);

    spEntry->Name = sEntryName.c_str();
    spEntry->Location = VxVCReport::MapColonSegment(currDataset.GetCurrentColonSegment());
    spEntry->OrigDistanceToRectum = currDataset.GetCurrentDistanceFromRectumMM();
    spEntry->CameraCoordinates = ReportVariant(currDataset.m_submarine.GetPositionVolume(),
      currDataset.m_submarine.GetOrientation(), currDataset.m_submarine.GetUpDirection());
  }
  catch(_com_error& e)
  {
    ReportWndBase::DisplayReportError(IDS_ADD_ENTRY, e.Error());
    return FALSE;
  }
  
  return TRUE;
} // AddReportEntry()


/**
 * Converts a reportlib dataset position value into the corresponding serialized dicom patient orientation value
 *
 * @param eOrient   reportlib dataset position value
 * @return          the corresponding serialized dicom patient orientation value
 */
SerializedDicom::PatientOrientation DatasetPositionToDicomPatientOrientation(const ReportLib::DatasetPositionEnumProp eOrient)
{
  switch ( eOrient )
  {
    case ReportLib::Supine:
      return SerializedDicom::SUPINE;
    case ReportLib::Prone:
      return SerializedDicom::PRONE;
    case ReportLib::Decubitus:
      return SerializedDicom::DECUBITUS;
    case ReportLib::DecubitusLeft:
      return SerializedDicom::DECUBITUS_LEFT;
    case ReportLib::DecubitusRight:
      return SerializedDicom::DECUBITUS_RIGHT;
    default:
      break;
  }

  return SerializedDicom::UNKNOWN;
} // DatasetPositionToDicomPatientOrientation()


/**
 * Converts a reportlib dataset position value into the corresponding serialized dicom patient orientation value
 *
 * @param eOrient   serialized dicom patient orientation value
 * @return          the corresponding reportlib dataset position value
 */
 ReportLib::DatasetPositionEnumProp DicomPatientOrientationToDatasetPosition(const SerializedDicom::PatientOrientation eOrient)
{
  switch ( eOrient )
  {
    case SerializedDicom::SUPINE:
      return ReportLib::Supine;
    case SerializedDicom::PRONE:
      return ReportLib::Prone;
    case SerializedDicom::DECUBITUS:
      return ReportLib::Decubitus;
    case SerializedDicom::DECUBITUS_LEFT:
      return ReportLib::DecubitusLeft;
    case SerializedDicom::DECUBITUS_RIGHT:
      return ReportLib::DecubitusRight;
    default:
      break;
  }

  return ReportLib::DatasetPositionEnumProp(-1);
} // DicomPatientOrientationToDatasetPosition()


/**
 * Updates the *displayed* distance to rectum in the list of report entries
 *
 * @param bSegmentOrderChanged   Whether the list of segments has been changed by the user
 */
void ReportLayout::UpdateEntryDistances(const bool bSegmentOrderChanged)
{ 
  return;
  /** By design, after the finding entry is created in the report, the information that is
   *  associated to that entry should not be changed or edited. It should only allow to be 
   *  deleted. So, this function does not make any sense for practical use. Dongqing 17 June 2009.
   *///


  //const int4 iNumEntries = m_report.m_spRevision->Entries->Count;
  //Point<float4> pt; Normal<float4> norm; Normal<float4> norm2;

  //// for each entry in the revision
  //for (int4 iEntry(0); iEntry < iNumEntries; iEntry++)
  //{
  //  // grab the entry from revision
  //  ReportLib::IReportEntryPtr spEntry = m_report.m_spRevision->Entries->GetItem((long)iEntry);

  //  if (spEntry->Name.length() <= 0)
  //    continue;

  //  const std::string sName = spEntry->Name;
  //  ReportLib::DatasetPositionEnumProp eStudyType = ReportColonFindings::MapFindingNameToDatasetPosition(sName);
  //  float4 fOrigDistFromRectum = spEntry->OrigDistanceToRectum;

  //  std::string::size_type iLoc(0);
  //  if ((iLoc = sName.find(LoadResourceString(IDS_RECTUM), 0)) != std::string::npos)
  //  {
  //    spEntry->Name = (sName.substr(0, iLoc) + std::string(LoadResourceString(IDS_ANAL_VERGE))).c_str();
  //  }

  //  // If no distance to rectum (and by relation, a camera coordinate) or study orientation is stored,
  //  //   then new distance to rectum cannot be calculated
  //  if ((spEntry->OrigDistanceToRectum == 0.0f) || (eStudyType == -1))
  //  {
  //    // TODO: Maybe better to add an entry field identifying the following case
  //    std::string sEntry = spEntry->Name;
  //    if (bSegmentOrderChanged && sEntry[sEntry.length()-1] != '*')
  //    {
  //      spEntry->Name += "*";
  //    }
  //    continue;
  //  }

  //  ReportVariant dist;
  //  dist.Attach(spEntry->CameraCoordinates.Detach());
  //  dist.asCameraCoordinates(pt, norm, norm2);

  //  const SerializedDicom::PatientOrientation ePatOrt = DatasetPositionToDicomPatientOrientation(eStudyType);
  //  Dataset& dataset = (ePatOrt == rdrGlobal.m_supine.GetPatientOrientation()) ? rdrGlobal.m_supine
  //    : (ePatOrt == rdrGlobal.m_prone.GetPatientOrientation()) ? rdrGlobal.m_prone : rdrGlobal.m_nullDataset;

  //  if ( dataset.IsDatasetRotatedProne() && spEntry->Version < 2 )
  //  {
  //    pt.m_x = (dataset.m_volumeHeader.GetWorldDim().m_x - pt.m_x);
  //    pt.m_y = (dataset.m_volumeHeader.GetWorldDim().m_y - pt.m_y);
  //  }

  //  vxBlockVolumeIterator<uint1> insIter( dataset.m_fields.m_ins );
  //  const int4 iSegment = dataset.m_segmentList.GetSortedSegmentIndexFromIdentifier( insIter.Peek( pt.m_x, pt.m_y, pt.m_z ) );

  //  if ((iSegment < 0) || !dataset.IsLoadingComplete())
  //  {
  //    std::string sUncertain;
  //    sUncertain  = LoadResourceString((eStudyType == ReportLib::Supine) ? IDS_SUPINE_EXTRA : IDS_PRONE_EXTRA);
  //    sUncertain += " " + LoadResourceString(IDS_COLONFINDING_DISTANCE_UNCERTAIN);
  //    spEntry->Name = sUncertain.c_str();
  //    continue;
  //  }

  //  const Segment & segment = dataset.m_segmentList.GetSegment(iSegment);
  //  const Skeleton & skel = dataset.m_segmentList.GetSkeleton(iSegment);

  //  vxBlockVolumeIterator<uint2> csiIter( dataset.m_fields.m_csi );
  //  uint2 uCsi = csiIter.Peek( pt.m_x, pt.m_y, pt.m_z );
  //  uCsi = Bound(uint2(0), uCsi, static_cast<uint2>(skel.GetNumOfElements()-1));

  //  float4 fDistFromRectumMM = segment.GetOffsetDistanceFromRectumMM();
  //  fDistFromRectumMM += segment.IsRegularDirection()? skel.GetWorldDfs(uCsi) : skel.GetWorldDfe(uCsi);

  //  CString sDistFromRectum;  sDistFromRectum.Format(IDS_DISTANCE_FROM_RECTUM_FORMAT, fDistFromRectumMM/10);

  //  std::string sEntryName;
  //  sEntryName += "(";
  //  sEntryName += PatientOrientationToString(dataset.GetPatientOrientation(), dataset.GetModality(), true);
  //  sEntryName += std::string(") ") + LPCTSTR(sDistFromRectum);

  //  spEntry->Name = sEntryName.c_str();
  //}
} // UpdateEntryDistances()


/**
 * split a single image into two images - the left and right halves
 * all images should be word aligned
 * new image data is allocated with new and you now own the memory
 *
 * @param pLeft                              the output left image buffer
 * @param pRight                             the output right image buffer
 * @param iOutputWidthPixels                 the output image width in pixels
 * @param iOutputWidthBytesDoublewordAligned the width of the output image buffers in bytes, doubleword aligned
 * @param pSourceImage                       the source image buffer
 * @param piWidthPixels                      the source image width in pixels
 * @param piHeightPixels                     the source image height in pixels
 * @param iWidthBytesDoublewordAligned       the width of the input image buffer in bytes, doubleword aligned
 */
void SplitImage( uint1 ** pLeft, uint1 ** pRight, int4 & iOutputWidthPixels, int4 & iOutputWidthBytesDoublewordAligned,
                 uint1 * pSourceImage, const int4 iWidthPixels, const int4 iHeightPixels, const int4 iWidthBytesDoublewordAligned )
{

  // determine the size of the new images
  static const uint1 uBitsPerPixel( 24 );
  iOutputWidthPixels = iWidthPixels / 2;
  iOutputWidthBytesDoublewordAligned = ( (iOutputWidthPixels * uBitsPerPixel + 31) & (~31) ) / 8;

  // allocate the memory
  const int4 iBufferSize = iHeightPixels * iOutputWidthBytesDoublewordAligned;
  * pLeft  = new uint1[ iBufferSize ];
  * pRight = new uint1[ iBufferSize ];
  memset( * pLeft, 0, iBufferSize );
  memset( * pRight, 0, iBufferSize );

  // copy the pixels
  for ( int4 y=0; y<iHeightPixels; ++y )
  {
    for ( int4 x=0; x<iWidthPixels; ++x )
    {
      uint1 * pSourcePixel = pSourceImage + y * iWidthBytesDoublewordAligned + 3 * x;
      uint1 * pDestinationPixel;
      if ( x < iOutputWidthPixels )
        pDestinationPixel = * pLeft + y * iOutputWidthBytesDoublewordAligned + 3 * x;
      else
        pDestinationPixel = * pRight + y * iOutputWidthBytesDoublewordAligned + 3 * ( x - iOutputWidthPixels );
      pDestinationPixel[0] = pSourcePixel[0];
      pDestinationPixel[1] = pSourcePixel[1];
      pDestinationPixel[2] = pSourcePixel[2];
    }
  }

} // SplitImage()


/**
 * Adds an image to the report entry.
 *
 * @param view      View whose snapshot will be added.
 * @param spEntry   Report entry.
 * @return          Success/failure.
 */
BOOL ReportLayout::AddImageToEntry(GLChildViewRef view, ReportLib::IReportEntryPtr& spEntry)
{
  if(!CanModifyReport())
  {
    return FALSE;
  }

  // CWaitCursor cursor; /* duplicate of this setting */
  int4 iWidth(0);
  int4 iHeight(0);
  int4 iWidthAlignedDWORD( 0 );

  LPBYTE pBuffer = LPBYTE(view.GetUpdateBuffer(iWidth, iHeight, iWidthAlignedDWORD, GL_BGR_EXT, GL_UNSIGNED_BYTE));
  if (pBuffer == NULL)
  {
    return FALSE;
  }

  try
  {

    bool bDissectionView( dynamic_cast< Segment3DChildView * >( & view ) != NULL ? true : false);

    if ( bDissectionView )
    {
      // split into two images, left and right
      uint1 * pLeft = NULL;
      uint1 * pRight = NULL;
      int4 iNewWidth = 1;
      int4 iNewWidthBytesDoublewordAligned = 4;
      SplitImage( & pLeft, & pRight, iNewWidth, iNewWidthBytesDoublewordAligned, pBuffer, iWidth, iHeight, iWidthAlignedDWORD );

      COleSafeArray saLeft;
      DWORD dwElementsLeft( iNewWidthBytesDoublewordAligned * iHeight );
      saLeft.CreateOneDim( VT_UI1, dwElementsLeft, pLeft );
      ReportLib::IImagePtr spImageLeft( __uuidof( ReportLib::Image ) );
      spImageLeft->Create( iWidth / 2, iHeight, 24 );
      spImageLeft->SetPixels( saLeft );
      spImageLeft->Resize( 512, int4( float4(512*iHeight)/float4(iWidth/2) ) );
      spEntry->Images->Add( spImageLeft );

      COleSafeArray saRight;
      DWORD dwElementsRight( iNewWidthBytesDoublewordAligned * iHeight );
      saRight.CreateOneDim( VT_UI1, dwElementsRight, pRight );
      ReportLib::IImagePtr spImageRight( __uuidof( ReportLib::Image ) );
      spImageRight->Create( iWidth / 2, iHeight, 24 );
      spImageRight->SetPixels( saRight );
      spImageRight->Resize( 512, int4(float4(512*iHeight)/float4(iWidth/2)) );
      spEntry->Images->Add( spImageRight );

    }
    else
    {
      bool bDrawOverview( dynamic_cast< DrawOverView * >( & view ) != NULL ? true : false);
     
      COleSafeArray sa;
      DWORD dwElements( iWidthAlignedDWORD * iHeight );
      sa.CreateOneDim(VT_UI1, dwElements, pBuffer);

      ReportLib::IImagePtr spImage(__uuidof(ReportLib::Image));
      spImage->Create(iWidth, iHeight, 24);    
      spImage->SetPixels(sa);

			if (rdrGlobal.m_bNormalizeSnapshotDim)
			{
        if (typeid(view) == typeid(OverView) || bDrawOverview )
          spImage->Resize( 512, 512 );
        else
          spImage->Resize( 512, 512 );
			} // if(rdrGlobal.m_bNormalizeSnapshotDim)

      spEntry->Images->Add(spImage);
    }
  }
  catch(_com_error& e)
  {
    wglMakeCurrent( NULL, NULL );
    throw e;
  }

  wglMakeCurrent( NULL, NULL );
  return TRUE;
} // AddImageToEntry()


/**
 * Creates the views for the window.
 *
 * @param hWndThis   Handle to this window.
 */
void ReportLayout::CreateChildViews(HWND hWndThis)
{
  // create the dialogs using language specific resources
  ExeResourceSwapper swapper( true );

  m_wndColonFindings.Create(ReportColonFindings::IDD, CWnd::FromHandle(hWndThis));
  m_wndOtherFindings.Create(ReportOtherFindings::IDD, CWnd::FromHandle(hWndThis));
  m_wndNonDistendedAreas.Create(ReportNonDistendedAreas::IDD, CWnd::FromHandle(hWndThis));
  m_wndMiscellaneous.Create(ReportMiscellaneous::IDD, CWnd::FromHandle(hWndThis));
  
  if (m_report.m_spRevision != NULL && m_report.m_spRevision->IsOpen)
  {
    m_wndColonFindings.SetReport(m_report.m_spRevision);
    m_wndOtherFindings.SetReport(m_report.m_spRevision);
    m_wndNonDistendedAreas.SetReport(m_report.m_spRevision);
    m_wndMiscellaneous.SetReport(m_report.m_spRevision);
  }
} // CreateChildViews()


/**
 * Calculates size and position of child views and controls.
 *
 * @param iWidth    Width of window.
 * @param iHeight   Height of window.
 */
void ReportLayout::CalculateViewRects(const int4 iWidth, const int4 iHeight)
{
  if ( (iWidth == 0) || (iHeight == 0) )
    return;

  CWnd *pWnd = GetDlgItem(IDC_TEMPLATE_PATIENTINFO);
  const int4 iInfoHeight = (pWnd!=NULL)?WinUtils::GetWindowRect(pWnd).Height():0;

  CRect rcColon;
  m_wndColonFindings.GetWindowRect(rcColon);
  int4 x = (iWidth - rcColon.Width()) / 2;
  int4 y = iInfoHeight + 125; int4 iFindingsYPos = y;
  m_wndColonFindings.SetWindowPos(NULL, x, y, rcColon.Width(), rcColon.Height(), SWP_NOZORDER);
  
  CRect rcNonDistended;
  m_wndNonDistendedAreas.GetWindowRect(rcNonDistended);
  x = (iWidth - rcNonDistended.Width()) / 2;
  y = iInfoHeight + 125;  
  m_wndNonDistendedAreas.SetWindowPos(NULL, x, y, rcNonDistended.Width(), rcNonDistended.Height(), SWP_NOZORDER);
 
  CRect rcOther;
  m_wndOtherFindings.GetWindowRect(rcOther);
  x = (iWidth - rcOther.Width()) / 2;
  y = iInfoHeight + 125;  
  m_wndOtherFindings.SetWindowPos(NULL, x, y, rcOther.Width(), rcOther.Height(), SWP_NOZORDER);

  CRect rcMisc;
  m_wndMiscellaneous.GetWindowRect(rcMisc);
  x = (iWidth - rcMisc.Width()) / 2;
  y = ((iHeight - iInfoHeight) - rcMisc.Height()) / 2;
  m_wndMiscellaneous.SetWindowPos(NULL, x, iFindingsYPos, rcMisc.Width(), rcMisc.Height(), SWP_NOZORDER);
} // CalculateViewRects()


/**
 * Activates colon findings selection view.
 */
void ReportLayout::OnColonFindings()
{
  ActivateView(&m_wndColonFindings);
} // OnColonFindings()


/**
 * Activates non-distended areas view.
 */
void ReportLayout::OnNonDistendedAreas()
{
  ActivateView(&m_wndNonDistendedAreas);
} // OnNonDistendedAreas()


/**
 * Activates other findings view.
 */
void ReportLayout::OnOtherFindings()
{
  ActivateView(&m_wndOtherFindings);
} // OnOtherFindings()


/**
 * Message: IDM_RPT_MISCELLANEOUS.  Activates miscellaneous view.
 */
void ReportLayout::OnMiscellaneous()
{
  ActivateView(&m_wndMiscellaneous);
} // OnMiscellaneous()


/**
 * Activates recommendations view.
 */
void ReportLayout::OnRecommendations()
{
  ActivateView(NULL);
} // OnRecommendations()


/**
 * Message: IDM_RPT_SAVE.  Called to save report.
 */
void ReportLayout::OnReportSave()
{
  if (m_pCurrentView)
  {
    m_pCurrentView->OnReportViewUnactivate();
  }
} // OnReportSave()


/**
 * Activates the specified view.
 *
 * @param pNewView   View to activate.
 * @return           TRUE.
 */
BOOL ReportLayout::ActivateView(ReportWndBase* pNewView)
{
  if (m_pCurrentView == pNewView)
  {
    return TRUE;
  }
  
  CWaitCursor cursor;
  
  if (m_pCurrentView)
  {
    m_pCurrentView->OnReportViewUnactivate();
    m_pCurrentView->ShowWindow(SW_HIDE);
  }

  m_pCurrentView = pNewView;
  
  if (m_pCurrentView)
  {
    m_pCurrentView->OnReportViewActivate();
    m_pCurrentView->ShowWindow(SW_SHOW);
  }
  RedrawWindow();
  
  return TRUE;
} // ActivateView()


/**
 * Called to open report.
 */
void ReportLayout::OnOpenReport()
{
  static CString szFilter = LoadResourceString(IDS_REPORT_OPEN_FILESYSTEM_FILTER);
  
  if (m_report.m_spRevision != NULL && m_report.m_spRevision->IsOpen)
  {
    m_wndColonFindings.Reset();
  }

  CFileDialog dlg(true,NULL,NULL,NULL,szFilter);
  CString title; title.LoadString(IDS_REPORT_OPEN_FILESYSTEM_TITLE);
  dlg.m_ofn.lpstrTitle = title;

  if (dlg.DoModal() == IDOK)
  {
    if (VxVCReport::OpenReportFile(rdrGlobal.m_pCurrDataset->m_volumeHeader, dlg.GetPathName()))
    {
      m_wndColonFindings.SetReport(m_report.m_spRevision);
      m_wndOtherFindings.SetReport(m_report.m_spRevision);
      m_wndNonDistendedAreas.SetReport(m_report.m_spRevision);
      m_wndMiscellaneous.SetReport(m_report.m_spRevision);
    }
  }
} // OnOpenReport()=


/**
 * Called to determine if report is modifiable.
 *
 * @return    TRUE if report is modifiable.
 */
BOOL ReportLayout::CanModifyReport()
{
  if ( (m_report.m_spRevision==NULL) || (m_report.m_spRevision->IsOpen == VARIANT_FALSE) ||
		m_report.IsReportMarkedCompleted() || (m_report.m_spRevision->IsReadOnly == VARIANT_TRUE) ||
    ( rdrGlobal.m_supine.m_session.GetVersion() < 3.0F ) || ( rdrGlobal.m_prone.m_session.GetVersion() < 3.0F ) ||
		 !User::GetCurrentUser().IsAllowed("CompleteReport") )
  {
    return FALSE;
  }

  int4 TODO_FIGURE_THIS_READONLY_STUFF_OUT;
  // if read from CD, can't modify...
  //if (m_report.m_eDBLocation == DBLOC_CD)
  //{
  //  return FALSE;
  //}

  if (!vx::File(GetReportFileName()).CanWrite())
  {
    return FALSE;
  }

  return TRUE;
} // CanModifyReport()



/**
 * Checks for OpenReport right
 *
 * @return    TRUE if user is allowed to open report.
 */
BOOL ReportLayout::CanOpenReport()
{
  const User::UserTypeEnum eUserType( User::GetCurrentUser().GetType() );
  if ( eUserType == User::UserType_Service || User::GetCurrentUser().IsAllowed("CompleteReport") )
  {
    return TRUE;
  }
  return FALSE;
}//CanOpenReport()


/**
 * Called to determine if current user has permission to modify the report.
 *
 * @return    TRUE if user has permission to modify report.
 */
BOOL ReportLayout::HasPermissions()
{
  const User::UserTypeEnum eUserType( User::GetCurrentUser().GetType() );
  if ( eUserType == User::UserType_Service || User::GetCurrentUser().IsAllowed("CompleteReport")
    || ( rdrGlobal.m_supine.m_session.GetVersion() >= 3.0F ) || ( rdrGlobal.m_prone.m_session.GetVersion() >= 3.0F ) )
  {
    return TRUE;
  }

  return FALSE;
} // HasPermissions()


/**
 * saves the report and all study session data
 */
void ReportLayout::SaveSessionData()
{
  CWaitCursor wait;

  try
  {
    Timer timer;
    if( !rdrGlobal.SaveData(false) )
		{
	    AfxMessageBox("An error occurred while saving the session.");
		} // if
    LogFyi("Saving sessions took " + timer.AsString() + " seconds to complete.","ReportLayout","OnPreviewReport"); timer.Reset();
    m_report.m_spRevision->Save();
		/// save this version of report back to the v3D Enterprise system
		m_report.WritePDFandSnapshots(false);
		LogFyi("Saving Report took " + timer.AsString() + " seconds to complete.", "ReportLayout", "OnPreviewReport"); timer.Reset();
  }
  catch(...)
  {
    LogErr("Exception occurred when saving session/report data.", "ReportLayout", "OnPreviewReport");
    const CString sErr = LoadResourceString(IDS_ERROR_CREATE_REPORT_PREVIEW) + "\n\n" + LoadResourceString(IDS_CONTACT_SERVICE);
    AfxMessageBox(sErr, MB_ICONSTOP|MB_OK);
    return;
  }
} // SaveSessionData()


/**
 * EnumWindows procedure, returns FALSE if the window is Acrobat reader, TRUE otherwise
 *
 * @param hWnd     Handle to the window in question.
 * @param lParam   Pointer to HWND, receives window handle if the enum window is identified as Adobe Acrobat.
 * @return         FALSE (to stop enumeration) if the window is identified as Adobe Acrobat, TRUE otherwise.
 */
BOOL __stdcall WinEnumChildCallback(HWND hWnd, LPARAM lParam)
{
  HWND* phWnd = reinterpret_cast<HWND*>(lParam);

  if ( phWnd != NULL )
  {
  
    char szWindow[1024];
    ::GetWindowText(hWnd, szWindow, 1024);

    //====================================================================================
    // 11/06/2003 Mark Kontak
    //
    // The window title was not what was expected and we were not finding the ADOBE 
    // reader due to that fact this it was running under the shell32.dll (explorer).
    //
  
    CString sTitle(szWindow);  
    LogTmp(std::string("  Child Window Title : ") + szWindow, "ReportLayout", "WinEnumChildCallback");
    if (sTitle.Find("Acro", 0) != -1 )
    {
      *phWnd = hWnd;
      return FALSE;
    }
    
    return TRUE;
  }

  return FALSE;
} // WinEnumChildCallback()


/**
 * EnumWindows procedure, returns FALSE if the window is Acrobat reader, TRUE otherwise
 *
 * @param hWnd     Handle to the window in question.
 * @param lParam   Pointer to HWND, receives window handle if the enum window is identified as Adobe Acrobat.
 * @return         FALSE (to stop enumeration) if the window is identified as Adobe Acrobat, TRUE otherwise.
 */
BOOL __stdcall FindAcroRd32WinEnumCallback(HWND hWnd, LPARAM lParam)
{
  HWND* phWnd = reinterpret_cast<HWND*>(lParam);

  DWORD pid;
  ::GetWindowThreadProcessId(hWnd, &pid);
  
  HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, pid);


  PROCESSENTRY32 pe;
  pe.dwSize =  sizeof (PROCESSENTRY32);

  if ( Process32First(hSnapshot, &pe) == TRUE )
  {
    do
    {
      CString sExe(pe.szExeFile);
      if (sExe.Find("AcroRd32.exe", 0) != -1 )
      {
        *phWnd = hWnd;
        return FALSE;
      }
    }
    while ( Process32Next(hSnapshot, &pe) == TRUE );
  }

  return TRUE;
  
} // FindAcroRd32WinEnumCallback()


/**
 * Called to preview report.
 */
void ReportLayout::OnPreviewReport()
{
  // create temporary file name for PDF document
  CString sTemp;

  {
    SaveSessionData();

    try
    {
      sTemp = vx::File::CreateTempFile(
        std::string(vx::ReportExt).substr(1,3), std::string(vx::AdobePDFExt).substr(1,3)).ToString().c_str();
    }
    catch(ex::IOException &)
    {
      LogErr("IOException occurred while creating temporary file for report preview.", "ReportLayout", "OnPreviewReport");
      const CString sErr = LoadResourceString(IDS_ERROR_CREATE_REPORT_PREVIEW) + "\n\n" + LoadResourceString(IDS_CONTACT_SERVICE);
      AfxMessageBox(sErr, MB_ICONSTOP|MB_OK);
      return;
    }
    catch(...)
    {
      LogErr("Exception occurred while creating temporary file for report preview.", "ReportLayout", "OnPreviewReport");
      const CString sErr = LoadResourceString(IDS_ERROR_CREATE_REPORT_PREVIEW) + "\n\n" + LoadResourceString(IDS_CONTACT_SERVICE);
      AfxMessageBox(sErr, MB_ICONSTOP|MB_OK);
      return;
    }
  }
  
  try
  {
    // create the PDF document
    VxReportCreator rptCreator(m_report.m_spRevision);
    if (rptCreator.Create(sTemp))
    {
      SHELLEXECUTEINFO sei;
      memset(&sei, 0, sizeof(SHELLEXECUTEINFO));

      // Open the document (which will invoke Acrobat Reader)
      sei.cbSize = sizeof(SHELLEXECUTEINFO);
      sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_DDEWAIT | SEE_MASK_HMONITOR;
      sei.hwnd = AfxGetMainWnd()->GetSafeHwnd();
      sei.lpFile = static_cast<LPCTSTR>(sTemp);
      sei.lpVerb = "open";
      sei.lpDirectory = ".";
      sei.nShow = SW_MAXIMIZE;
      sei.hMonitor = MonitorFromWindow(AfxGetMainWnd()->GetSafeHwnd(), MONITOR_DEFAULTTONEAREST);

      // Save the show state of the main window
      const int4 iShowCmd(AfxGetMainWnd()->IsZoomed() ? SW_MAXIMIZE : SW_RESTORE);
      AfxGetMainWnd()->SetWindowPos(&wndNoTopMost, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);

      if (ShellExecuteEx(&sei))
      {
        HWND hWndReader = NULL;
        // Find the Acrobat Reader window
        EnumWindows(&FindAcroRd32WinEnumCallback, reinterpret_cast<LPARAM>(&hWndReader));

        //================================================================================================================
        //
        // 11/06/2003 Mark Kontak
        //
        // These calls were causing the system to hang on a single CPU. If appears that the handle is not actually the 
        // handle for Adobe Reader, but for a Shell32.dll. 
        //
        // Set Reader window to topmost
        if (hWndReader != NULL)
        {
          ::SetWindowPos(hWndReader, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
          // Note: Don't ask why, but only this combination works (setting both found wnd and its top level owner)
          CWnd * pWndReader = CWnd::FromHandle(hWndReader);
          CWnd * pWndTop = pWndReader ? pWndReader->GetTopLevelParent() : NULL;
          if (pWndTop)
          {
            pWndTop->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
          }
        }
        //=================================================================================================================

        // Hide this window
        AfxGetMainWnd()->ShowWindow(SW_HIDE);

        while (WaitForSingleObject(sei.hProcess, 0) != WAIT_OBJECT_0)
        {
          MSG msg;
          if (GetMessage(&msg, NULL, 0, 0))
          {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
          }

          //////////////////////////////////////////////////////////////////////
          // 11/06/2003 Mark Kontak
          //
          // Maximize the window if the user tries to hide it
          //if (hWndReader != NULL)
          //  ::ShowWindow(hWndReader, SW_MAXIMIZE);
          ///////////////////////////////////////////////////////////////////////
        }
      }

      // Maximize then restore this window, helps to erase strange remnants Acrobat Reader leaves behind
      //AfxGetMainWnd()->ShowWindow(SW_MAXIMIZE);
      AfxGetMainWnd()->ShowWindow(SW_SHOW);   /// If this fix cause trouble, we need to roll back to the line of code above this line.
			                                        /// It should work since this code only affect the Report tab, which has no image window.

      // Restore the window to its original size
      AfxGetMainWnd()->ShowWindow(iShowCmd);

#ifdef FINAL_RELEASE    
      // Bring the window to the top; Don't know why, but only all of this works properly
      AfxGetMainWnd()->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
      AfxGetMainWnd()->SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
      AfxGetMainWnd()->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
#else
      ::SetWindowPos(AfxGetMainWnd()->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
      AfxGetMainWnd()->BringWindowToTop();
#endif

      DeleteFile(sTemp);
    }

    // If a dataset is open, return to the report screen
    if (rdrGlobal.IsLoadingComplete())
    {
      this->RedrawWindow();
      if (ReportLayout::HasPermissions() && ReportLayout::CanModifyReport())
      {
        AfxGetMainWnd()->PostMessage(WM_APP_VIEW_SELECTION_CHANGED, WM_APP_VIEWSEL_REPORT, FramesManager::VFrameManager::ReportFrameManager::eREPORTPAGE_FINDINGS);
      }
      else
      {
        AfxGetMainWnd()->PostMessage(WM_APP_VIEW_SELECTION_CHANGED, WM_APP_VIEWSEL_NAVIGATION, 0);
      }
      AfxGetMainWnd()->RedrawWindow();
    }
    else // else return to the study browser
    {
      AfxGetMainWnd()->PostMessage(WM_COMMAND, MAKEWPARAM(IDM_STUDY_CLOSE,0), LPARAM(0));
    }  

		/// save this version of report back to the v3D Enterprise system
		m_report.WritePDFandSnapshots(false);
  }
  catch (...)
  {
    LogErr("Exception caught during report preview", "ReportLayout", "OnPreviewReport");
    const CString sErr = LoadResourceString(IDS_ERROR_CREATE_REPORT_PREVIEW) + "\n\n" + LoadResourceString(IDS_CONTACT_SERVICE);
    AfxMessageBox(sErr, MB_ICONSTOP|MB_OK);
  }
} // OnPreviewReport()


/**
 * WM_MOUSEMOVE handler
 *
 * @param uFlags   Current key flags
 * @param point    Current point of mouse move
 */
void ReportLayout::OnMouseMove(UINT uFlags, CPoint point)
{
  // special: no call of SetFocus()
  CWnd::OnMouseMove(uFlags, point);
} // OnMouseMove()


/**
 * Toggle the report complete state
 */
void ReportLayout::OnToggleReportCompleteMark()
{
  m_wndMiscellaneous.OnReportViewUnactivate();
  ColonViatronixWnd::OnToggleReportCompleteMark();
  AfxGetMainWnd()->PostMessage(WM_APP_VIEW_SELECTION_CHANGED, WM_APP_VIEWSEL_NAVIGATION, 0);
} // OnToggleReportCompleteMark()


/**
 * OnPaint message handler
 */
void ReportLayout::OnPaint()
{
  CPaintDC dc( this );

  // draw the buffer to the screen
  if( m_pDrawBuffer != NULL )
  {
    Gdiplus::Graphics gfx( dc.GetSafeHdc() );
    gfx.DrawImage( m_pDrawBuffer, 0, 0, m_pDrawBuffer->GetWidth(), m_pDrawBuffer->GetHeight() );
  } 
} // OnPaint()


/**
 * WM_ERASEBKGND handler, called when background needs erasing
 *
 * @param   pdc     Device context for window
 * @return  Non-zero to indicate NOT to erase, zero to let framework erase window
 */
BOOL ReportLayout::OnEraseBkgnd( CDC * pdc )
{
  return GLWnd::OnEraseBkgnd( pdc );
} // OnEraseBkgnd( CDC * pdc )


/**
 * WM_SIZE handler, Called when window is resized
 *
 * @param uType   Type of resizing (e.g. maximize, restore)
 * @param iCX     Width
 * @param iCY     Height
 */
void ReportLayout::OnSize( UINT uType, int cx, int cy )
{
  ColonViatronixWnd::OnSize( uType, cx, cy );

  CalculateViewRects( cx, cy );

  // free the previous background image
  if( m_pDrawBuffer != NULL )
  {
    delete m_pDrawBuffer;
    m_pDrawBuffer = NULL;
  }

  if( cx >0  && cy > 0 )
  {
    // create new drawing buffer based on new window size
    m_pDrawBuffer = new Gdiplus::Bitmap( cx, Theme::BkgndHeaderMiddle->GetHeight(), PixelFormat24bppRGB );
  
    Gdiplus::Graphics gfx( m_pDrawBuffer );
    
    Gdiplus::TextureBrush brush( Theme::BkgndHeaderMiddle );
    gfx.FillRectangle( &brush, 0, 0, cx, Theme::BkgndHeaderMiddle->GetHeight() );
    gfx.DrawImage( Theme::BkgndHeaderLeft, 0, 0 );
    gfx.DrawImage( Theme::BkgndHeaderRight, cx - Theme::BkgndHeaderRight->GetWidth(), 0, Theme::BkgndHeaderRight->GetWidth(), Theme::BkgndHeaderRight->GetHeight() );
  }
  Invalidate();
} // OnSize( UINT uType, int cx, int cy )


/**
 * Derived classes can override to remove areas from the parent region
 *
 * @param   parentRgn     Updates the parent region removing areas to update as necessary
 */
void ReportLayout::OnUpdateBackgroundRegion( CRgn & parentRgn )
{
  // remove the logo from the update 
  if( m_pDrawBuffer )
  {
    CRgn logoRgn;
    logoRgn.CreateRectRgn( 0, 0, m_pDrawBuffer->GetWidth(), m_pDrawBuffer->GetHeight() );
    parentRgn.CombineRgn( &parentRgn, &logoRgn, RGN_DIFF );
  }

} // OnUpdateBackgroundRegion( CRgn & parentRgn )


// undefines
#undef FILE_REVISION


// $Log: ReportViewWnd.C,v $
// Revision 1.37.2.15  2010/09/20 18:41:34  dongqing
// Jeff's bug fix for his various monitor aspect ratio work
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.37.2.14  2010/04/09 12:42:50  dongqing
// attempt at getting acrobat to display on the selected monitor (doesn't yet work, though the code appears correct)
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.37.2.13  2010/03/09 20:17:43  dongqing
// print study info on every snapshot image
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.37.2.12  2010/01/31 04:58:41  dongqing
// prevent crash when attempting to access an empty suspicious regions list
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.37.2.11  2009/12/09 20:20:41  dongqing
// space
//
// Revision 1.37.2.10  2009/10/07 16:13:14  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.37.2.9  2009/08/24 22:02:00  dongqing
// Since Gerg finds out the correct fix to the report libray for Microsoft ATL
// update, we need to roll back Dongqing's temporary fix.
//
// Revision 1.37.2.8  2009/08/19 15:31:22  vxconfig
// add initialization of extra-colonic for each new colonic findings
// Otherwise, it causes crash when flip colonic to extra-colonic
//
// Revision 1.37.2.7  2009/06/17 21:08:25  dongqing
// The report entry distance does not need to be updated after
// the segments being re-selected by design.
//
// Revision 1.37.2.6  2008/11/18 01:01:04  jmeade
// fixed error in HasPermissions() fn
//
// Revision 1.37.2.5  2008/11/14 20:16:18  jmeade
// give version info to each report entry;
// flip camera coordinates for report entries of old prone datasets.
//
// Revision 1.37.2.4  2008/03/21 00:45:26  jmeade
// Issue 6047: reposition dialogs.
//
// Revision 1.37.2.3  2008/02/14 06:57:48  jmeade
// Issue 5924: identify xc findings properly; identify report layout pages properly.
//
// Revision 1.37.2.2  2007/06/14 18:11:14  jmeade
// read-only report and session for session files previous to version 3.0.
//
// Revision 1.37.2.1  2007/03/23 02:00:18  jmeade
// Issue 5502: save changes for current view when destroying reportlayout win.
//
// Revision 1.37  2007/03/12 19:43:46  jmeade
// code standards.
//
// Revision 1.36  2007/03/12 16:16:23  jmeade
// Issue 5278: drawing srt measurement during snapshot.
// code standards.
//
// Revision 1.35  2007/03/09 22:02:28  jmeade
// Issue 5278: drawing srt measurement during snapshot.
//
// Revision 1.34  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.33  2007/02/09 00:11:49  jmeade
// Issue 5219: Implementing C-RADS: morphology, other findings E0-E4, failed colonoscopy indication.
//
// Revision 1.32  2007/02/06 16:44:57  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.31  2007/02/01 21:35:08  romy
// added CanOpenReport method
//
// Revision 1.30  2007/01/22 21:00:11  romy
// modified HasPermission method
//
// Revision 1.29  2006/11/07 20:09:02  jmeade
// roi volume info into comments block of segmented region snapshot.
//
// Revision 1.28  2006/10/03 02:09:00  jmeade
// cad measurements during segment region snapshot.
//
// Revision 1.27  2006/09/29 16:13:40  frank
// fixed the colors when taking a snapshot on the dissection view
//
// Revision 1.26  2006/09/28 21:21:41  frank
// split the dissection snapshot into two (more square) images
//
// Revision 1.25  2006/09/22 21:50:29  jmeade
// Issue 4946: display of decubitus patient positions in report.
//
// Revision 1.24  2006/09/18 19:43:15  jmeade
// protecting against fatal error on possible empty string.
//
// Revision 1.23  2006/08/25 21:14:18  jmeade
// Issue 4946: strings for decubitus studies.
//
// Revision 1.22  2006/07/28 20:16:10  jmeade
// Issue 4658: grant permissions to physician and service.
//
// Revision 1.21  2006/07/18 15:11:13  jmeade
// Issue 4860: report permissions shouldn't be determined by report file accessibility, that will be a different test.
//
// Revision 1.20  2006/07/11 19:17:11  jmeade
// Issue 4658: check 'take snapshot' permissions.
//
// Revision 1.19  2006/01/31 14:02:08  frank
// code review
//
// Revision 1.18  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.17  2005/11/21 23:15:54  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.16  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.15  2005/11/15 18:52:17  geconomos
// reworked user to be compatible with the new console
//
// Revision 1.14  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.13.2.5  2006/01/10 18:54:19  geconomos
// first crack at reducing flicker while resizing
//
// Revision 1.13.2.4  2006/01/05 18:07:45  geconomos
// Issue# 4589: Overview not being sized correctly
//
// Revision 1.13.2.3  2005/12/23 16:49:16  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.13.2.2  2005/11/17 16:30:43  geconomos
// issue# 4540: report preview causing crash (reworked GetUpdateBuffer)
//
// Revision 1.13.2.1  2005/11/16 02:18:44  jmeade
// Issue 4528: Align image dimensions along word boundaries.
//
// Revision 1.13  2005/10/25 16:46:49  jmeade
// ParentViews ==> Layouts
//
// Revision 1.12  2005/10/04 02:07:20  geconomos
// added drawing code for header images
//
// Revision 1.11  2005/09/29 14:12:12  vxconfig
// switched csi volume to new vx block volume
//
// Revision 1.10  2005/09/28 20:08:38  vxconfig
// Pulling weeds - replaced old block volume with new for inside volume
//
// Revision 1.9  2005/09/23 20:10:22  vxconfig
// Merged from 1-6-1 branch to the trunk after release
//
// Revision 1.8  2005/09/21 17:14:25  jmeade
// comments.
//
// Revision 1.7  2005/09/13 17:27:19  jmeade
// ColonReport namespace.
//
// Revision 1.6.2.1  2005/09/21 13:28:47  geconomos
// modified DrawImage to implicitly use an image size of 512
//
// Revision 1.6  2005/08/24 20:19:34  jmeade
// namespaces for ConsoleLib.
//
// Revision 1.5  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.4  2005/08/19 13:00:09  vxconfig
// merged incremental changes from vx VC_1-6 to v3d
//
// Revision 1.3  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/10 23:48:54  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.38.2.13.2.2.2.4  2005/07/21 15:59:41  jmeade
// Issue 4208: Anonymized patient age set to '0'.
//
// Revision 3.38.2.13.2.2.2.3  2005/07/20 15:53:47  geconomos
// Issue# 4286: Snapshot crash with DrawOverImage.
//
// Revision 3.38.2.13.2.2.2.2  2005/07/01 16:52:09  jmeade
// Issue 4169: Store value of marked-complete flag to avoid checking file on each query.
//
// Revision 3.38.2.13.2.2.2.1  2005/06/27 17:39:10  geconomos
// Issue #4173: (Frank) Fixed distance from anal verge when the viewpoint is adjusted for better path visibility which changes the distance from rectum
//
// Revision 3.38.2.13.2.2  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.38.2.13.2.1.2.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 3.38.2.13.2.1  2005/03/12 14:36:03  jmeade
// Snapshots wit 2d measurements interpreted as polyps (as with 3d measurements).
//
// Revision 3.38.2.12.2.3  2005/02/23 03:16:28  jmeade
// Snapshots wit 2d measurements interpreted as polyps (as with 3d measurements).
//
// Revision 3.38.2.12.2.2  2004/10/23 00:47:06  jmeade
// Return to navigation screen after marking study complete.
//
// Revision 3.38.2.12.2.1  2004/10/07 21:50:10  jmeade
// Replace "distance from rectum" text with "distance from anal verge" in entry names.
//
// Revision 3.38.2.12  2004/09/21 18:16:05  jmeade
// Just-in-case preventing a null comments block from choking other code.
//
// Revision 3.38.2.11  2004/08/11 16:46:31  geconomos
// Report images are always at least 512x512
//
// Revision 3.38.2.10  2004/07/22 17:16:47  jmeade
// Issue 3630: Ensure you won't use an unloaded dataset when matching finding location to segments.
//
// Revision 3.38.2.9  2004/07/21 20:25:40  jmeade
// Issue 3630: Note where appropriate when the rectum-distance is unable to be calculated.
//
// Revision 3.38.2.8  2004/07/17 03:12:41  jmeade
// Issue 3630: Update colon finding distances when colon segments are changed.
//
// Revision 3.38.2.7  2004/03/30 23:45:46  jmeade
// Issue 3590: Fix bug where more than one finding type could be highlighted.
//
// Revision 3.38.2.6  2004/03/03 22:02:58  jmeade
// Redraw background to remove remnants of removed controls (for newer video drivers);
// use local variable instead of referencing global where appropriate.
//
// Revision 3.38.2.5  2003/11/05 16:44:12  mkontak
// Fixed issue with single CPU and the report preview
//
// Revision 3.38.2.4  2003/07/07 21:27:17  jmeade
// Code standards, comments.
//
// Revision 3.38.2.3  2003/04/28 18:13:54  jmeade
// Issue 3001: Ensure the correct image params are set on snapshot capture.
//
// Revision 3.38.2.2  2003/02/22 11:59:03  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.38.2.1  2003/02/19 22:24:32  jmeade
// Issue 2842:  Implemented variable number of snapshots per report entry.
//
// Revision 3.38  2002/12/24 03:48:38  jmeade
// Bug: Measure list not saved in session b/c there were two copies of it!
//
// Revision 3.37  2002/12/12 22:07:52  jmeade
// Unused or unneeded view modes code cleanup.
//
// Revision 3.36  2002/11/22 00:28:21  jmeade
// Coding standard on m_trackball* variables.
//
// Revision 3.35  2002/11/15 00:34:20  jmeade
// No hard-coding of # of child views.
//
// Revision 3.34  2002/09/18 20:06:16  jmeade
// Moved UserAdmin to ConsoleLib.
//
// Revision 3.33  2002/09/12 18:06:07  jmeade
// No need for typedef'd iterator.
//
// Revision 3.32  2002/09/06 16:10:26  jmeade
// Merged with VC_1-2-1_CS-1 branch, from 07-02-2002 to 09-05-2002.
//
// Revision 3.31  2002/08/22 13:20:01  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.29  2002/07/18 20:25:39  jmeade
// Moved report functionality to ConsoleLib.
//
// Revision 3.28  2002/07/03 03:23:42  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.27  2002/06/26 17:35:44  jmeade
// Moving more hard-coded strings to the string table.
//
// Revision 3.26  2002/04/26 15:57:31  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.25  2002/04/12 02:14:56  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.24  2002/03/13 22:01:41  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.23  2002/02/04 20:18:14  binli
// Improved image quality in report file
//
// Revision 3.22.2.13  2002/08/07 22:26:48  jmeade
// Issue 2590:  Use a dizzying combination of SetWindowPos() calls to get Viewer and Acrobat Reader to behave properly.
//
// Revision 3.22.2.12  2002/06/12 02:22:33  jmeade
// Issue 2423:  Use one GetUpdateBuffer method that returns private, temporary memory.
//
// Revision 3.22.2.11  2002/04/18 01:15:09  jmeade
// Issue 2043:  Throw exceptions from IsReportMarkedCompleted() on error;
//  return success of SetMarkCompletedFlag().
//
// Revision 3.22.2.10  2002/03/05 20:55:31  jmeade
// Proper reset of file attributes.
//
// Revision 3.22.2.9  2002/03/05 20:17:33  binli
// adjustment of 3D measurement line drawing.
//
// Revision 3.22.2.8  2002/03/01 17:01:25  binli
// issue 1966: when measurement line is visible, default to 'polyp'
//
// Revision 3.22.2.7  2002/02/27 22:11:27  jmeade
// Jump back to navigation if only preview enabled; Resize measure vector once
//  instead of using multiple push_back() calls; Comments.
//
// Revision 3.22.2.6  2002/02/27 18:03:41  jmeade
// Close report (.vrx file) after generating PDF.
//
// Revision 3.22.2.5  2002/02/26 21:42:43  binli
// issue 1900: override OnMouseMove(): keep the focus & no need to call fucntions defined in ViatronViewWnd & GLViewWnd
//
// Revision 3.22.2.4  2002/02/22 20:03:36  binli
// change what?
//
// Revision 3.22.2.3  2002/02/19 18:32:31  jmeade
// Code standards.
//
// Revision 3.22.2.2  2002/02/11 16:37:19  binli
// Improved image quality in report file
//
// Revision 3.22.2.1  2002/02/05 23:47:48  jmeade
// Issue 1629:  Check IsMarkedComplete status of report when switching to report view.
//
// Revision 3.22  2002/01/30 23:11:31  jmeade
// Issue 1721:  Can only change information (done in OpenReport) on a writable file,
//  so copy read-only report to temporary before generating pdf.
//
// Revision 3.21  2002/01/30 23:07:04  jmeade
// Issue 1640:  Can only call ReplacePatientInfo() on a non-read-only file.
//
// Revision 3.20  2002/01/30 18:53:53  binli
// issue 1721: compensation to #1557, just open report, assign string to spInfo->Author, got error.
// (This statement should not be reached)
//
// Revision 3.19  2002/01/25 23:03:50  binli
// issue 1652: appendix to fix job in 3.18
//
// Revision 3.18  2002/01/25 21:05:04  binli
// issue 1652: underlying function: File::CreateTempFile() creates a temp file now.
//     (as compared with old version, which just creates a temp file NAME)
//
// Revision 3.17  2002/01/23 21:46:00  binli
// issue 1589: Endo-SliceView: take snapshot from all 3 views now.
//
// Revision 3.16  2002/01/23 20:36:33  binli
// issue 1557: compability
//
// Revision 3.15  2002/01/18 22:00:06  binli
// code cleaned
//
// Revision 3.14  2002/01/17 02:23:50  jmeade
// Generate PDF Function; comments.
//
// Revision 3.13  2002/01/12 01:56:58  jmeade
// Ensure non-empty string in GenerateRptFilename()
//
// Revision 3.12  2002/01/08 22:49:23  jmeade
// Users to database.
//
// Revision 3.11  2002/01/08 15:13:55  binli
// Report: mistake to set the width of image.
//
// Revision 3.10  2002/01/07 21:43:51  binli
// Report adjustment: no display of overview imge in ExtraColonicFinding
//
// Revision 3.9  2001/12/29 00:11:31  jmeade
// Merge from 1.1 branch.
//
// Revision 3.8  2001/12/22 00:10:30  jmeade
// Code standards.
//
// Revision 3.7  2001/11/21 15:26:09  jmeade
// Removed obsolete.
//
// Revision 3.6  2001/11/17 03:35:18  jmeade
// Patient view stored in base class.
//
// Revision 3.5  2001/11/09 04:50:21  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.4  2001/10/30 00:44:27  geconomos
// Added methods for getting/setting MarkedComplete flag
//
// Revision 3.3.2.13  2001/12/03 21:31:37  jmeade
// StudyType changed to DatasetPositionEnumProp to eliminate current and future conflicts
//
// Revision 3.3.2.12  2001/11/07 20:05:57  jmeade
// Bug fix:  Buggy code causing report to be incorrectly identified as read-only (and thus creating temporary file)
//
// Revision 3.3.2.11  2001/11/06 20:09:53  jmeade
// Cleaned up V1KMessages.h enum, comments.
//
// Revision 3.3.2.10  2001/11/06 01:42:44  jmeade
// Coding standards and comments.
//
// Revision 3.3.2.9  2001/11/05 22:21:36  jmeade
// Coding standards.
//
// Revision 3.3.2.8  2001/11/05 18:01:10  jmeade
// Formatting of entry name with text from resources
//
// Revision 3.3.2.7  2001/11/04 05:14:29  jmeade
// Code standards.
//
// Revision 3.3.2.6  2001/10/31 03:33:00  jmeade
// Oops, compile error.
//
// Revision 3.3.2.5  2001/10/31 00:46:17  jmeade
// Code conventions
//
// Revision 3.3.2.4  2001/10/31 00:33:38  jmeade
// Code conventions
//
// Revision 3.3.2.3  2001/10/31 00:30:06  jmeade
// From Revision 3.4  2001/10/30 00:44:27  geconomos:  Added methods for getting/setting MarkedComplete flag
//
// Revision 3.3.2.2  2001/10/30 23:11:45  jmeade
// Comments and conventions
//
// Revision 3.3.2.1  2001/10/28 11:11:27  jmeade
// Coding standards; Safer references instead of pointers
//
// Revision 3.3  2001/10/20 03:09:44  jmeade
// After report preview, show window in same state as before preview
//
// Revision 3.2  2001/10/18 18:12:39  binli
// issue 1208: read Max/Perpendicular from spEntry even though no polyp
//
// Revision 3.1  2001/10/17 21:08:36  binli
// issue 1184: 2 solutions: a> hide 3DViewer when the report is opened from CD (Jeff M did)
//                                       b> set up the flag DBLocation, and ensure only a ReadOnly reportview can created. (BinLi did)
//
// Revision 3.0  2001/10/14 23:02:00  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.15   Oct 05 2001 11:50:54   binli
// report patient info: use normal full name (without '^')
// 
//    Rev 2.14   Oct 04 2001 10:09:24   binli
// small adjust of error fix job 2.11.
// 
//    Rev 2.13   Oct 03 2001 19:38:16   jmeade
// Coding conventions in UserAdmin.C classes
// 
//    Rev 2.12   Oct 03 2001 18:26:16   binli
// check in error: info is the same as 2.11
// 
//    Rev 2.11   Oct 03 2001 18:12:24   binli
// bug fix: error when open study from CD
// fix: readonly for all level custom.
// (.vrx file exists, open it, else, create new temporary file, but no save it)
// 
//    Rev 2.10   Oct 03 2001 11:10:04   binli
// report bug: ensure to create all report images.
// fixed: temporay lock of NavigationBar.
// 
//    Rev 2.9   Oct 02 2001 13:11:38   binli
// report bug: no display of centerline in ColonFindings in report file.
// fixed: no unsetting of display centerline.
// 
//    Rev 2.8   Oct 01 2001 17:10:18   binli
// functions to ensure no bad/black images in report file: synchronous control.
// 
//    Rev 2.7   Sep 27 2001 20:43:08   jmeade
// Merge in changes from branch 2.5.1.0
//
//    Rev 2.6   Sep 27 2001 14:56:26   binli
// ID 1009: changed the order to get NonIndistendedArea images:  this MAY prevent from creating bad image (err code: 0x80030002/0x8003005) --> more work is needed.
// 
//    Rev 2.5.1.0   Sep 27 2001 11:00:36   jmeade
// No more encrypted patient name/ID
// 
//    Rev 2.5   Sep 25 2001 17:12:36   binli
// ID 1009 (part2): 1> correct position of arrow; 2> always show up of arrow.
// 
//    Rev 2.4   Sep 25 2001 13:46:28   binli
// ID 1009: (part 1): correct study images in PDF file.
// 
//    Rev 2.3   Sep 22 2001 03:21:18   jmeade
// Removed an English string
// 
//    Rev 2.2   Sep 19 2001 15:13:42   binli
// no change
// 
//    Rev 2.1   Sep 19 2001 15:07:26   binli
// (no track ID): bad patient name('0' ) and ID ('0')
// 
//    Rev 2.0   Sep 16 2001 23:41:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:48   ingmar
// Initial revision.
// Revision 1.74  2001/09/14 23:06:04  jmeade
// Localized calculation of report filename (to reduce future errors),
// Code conventions, topmost for debug only, save only with proper permission
//
// Revision 1.73  2001/09/10 22:39:47  binli
// track ID: 000672: added (Supine)/(Prone) to caption in Colon-Findings and NonDistendedAreas.
//
// Revision 1.72  2001/09/10 19:59:33  jmeade
// Addendum to revision 1.67: default to Polyp when a measurement was taken
//
// Revision 1.71  2001/09/02 23:42:02  ingmar
// IOException -> ex::IOException
//
// Revision 1.70  2001/08/31 22:20:54  jmeade
// Try/catch only for release builds
//
// Revision 1.69  2001/08/29 22:26:51  binli
// close report file error: check spReport first (no null) before close it.
//
// Revision 1.68  2001/08/24 15:55:57  jmeade
// Handle removed FileExtensions const char * operator
//
// Revision 1.67  2001/08/22 21:34:21  binli
// bug pt1001: colonfinds, default is "Normal", not "Polyp"
//
// Revision 1.66  2001/08/21 18:24:10  binli
// no message
//
// Revision 1.65  2001/08/21 02:25:57  jmeade
// Enums for view window positions (no hardcoded values)
//
// Revision 1.64  2001/08/09 18:38:04  binli
// return value
//
// Revision 1.63  2001/08/09 15:19:58  binli
// bug 509: close .vrx file after the study is closed.
//
// Revision 1.62  2001/07/09 18:21:13  jmeade
// Bug fix 522: preview report message (not a direct call), like other handlers; delete temp pdf
// after preview; redraw windows and switch views (or close study) as appropriate
// after preview
//
// Revision 1.61  2001/06/08 01:13:20  jmeade
// Using a function instead of referencing an extern
//
// Revision 1.60  2001/05/30 14:23:21  jmeade
// coding standards to fix name clashes
//
// Revision 1.59  2001/05/25 08:43:44  jmeade
// user account types to UserAccountData class
//
// Revision 1.58  2001/05/17 20:42:05  binli
// VerifyView vs. StandardVIew/S&PView: controled under different trackball.
//
// Revision 1.57  2001/05/15 17:49:38  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.56  2001/05/11 23:35:43  jmeade
// Coding conventions
//
// Revision 1.55  2001/05/10 21:04:40  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.54  2001/05/07 00:51:33  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.53  2001/04/30 12:33:11  frank
// Reflected changes in Study class.
//
// Revision 1.51  2001/04/16 13:49:15  geconomos
// Removed some uneeded image properties
//
// Revision 1.50  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.49  2001/04/13 00:38:57  jmeade
// file extensions
//
// Revision 1.48  2001/04/05 20:43:00  binli
// reflected change in TrackBall: private matrix.
//
// Revision 1.47  2001/03/30 18:13:20  geconomos
// Partially fixed bug with reportspinedit control
//
// Revision 1.46  2001/03/26 16:03:39  frank
// Reflected changes in Segment class.
//
// Revision 1.45  2001/03/21 16:31:02  geconomos
// Detected and fixed memory leak
//
// Revision 1.44  2001/03/21 15:47:27  geconomos
// Fixed bug with read only report based on user level
//
// Revision 1.43  2001/03/19 18:07:21  frank
// Reflected variable name changes in Segment class.
//
// Revision 1.42  2001/03/09 22:06:02  geconomos
// Fixed some STUPID TYPO bugs!!!
//
// Revision 1.41  2001/03/09 01:25:56  jmeade
// protect access to some more Study members
//
// Revision 1.40  2001/03/08 23:08:50  jmeade
// HARD-CODED STRINGS!!!  OH THE HORROR!!!
// (don't need the read-only report tab either)
//
// Revision 1.39  2001/03/08 16:34:55  geconomos
// Added read only only report functionailty.
//
// Revision 1.38  2001/03/08 15:52:11  geconomos
// Worked on grabbing Non-Distended regions when creating report.
//
// Revision 1.37  2001/03/02 21:56:58  geconomos
// Added GetUserFullName()
//
// Revision 1.36  2001/02/28 16:37:01  geconomos
// no message
//
// Revision 1.35  2001/02/27 20:58:37  geconomos
// Added report header stuff
//
// Revision 1.34  2001/02/26 22:00:30  geconomos
// Changed reading of patientName from volume header.
//
// Revision 1.33  2001/02/22 01:36:09  jeff
// variable naming to follow coding conventions
//
// Revision 1.32  2001/02/12 16:44:38  geconomos
// Fixe supine and prone dilema in non distended areas
//
// Revision 1.31  2001/02/07 16:25:03  geconomos
// Added revisions to report file
//
// Revision 1.30  2001/02/05 18:23:27  geconomos
// Added start  and end points in order to draw centerline
//
// Revision 1.29  2001/02/05 16:17:40  jeff
// strings to resource table
//
// Revision 1.28  2001/02/05 14:32:12  geconomos
// Implementation of ReportVariant
//
// Revision 1.27  2001/02/01 17:07:14  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportViewWnd.C,v 1.37.2.15 2010/09/20 18:41:34 dongqing Exp $
// $Id: ReportViewWnd.C,v 1.37.2.15 2010/09/20 18:41:34 dongqing Exp $
