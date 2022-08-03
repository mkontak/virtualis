// $Id: InteractiveSegmentation.C,v 1.22 2007/06/28 19:33:55 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (mailto:jaddonisio@viatronix.com)

/**
* Main class that encapsulates interactive segmentation. This class provides
* methods to get/set segmentation parameters and run various segmentation
* operations.
*/


// includes
#include "stdafx.h"
#include "InteractiveSegmentation.h"
#include "vxMessageBox.h"
#include "vxComponentUtils.h"
#include "Timer.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxBlockVolumeIteratorNeighborsConsts.h"
#include "vxUtils.h"

#include "vxIntensityConverter.h"

// namespaces
using namespace std;


// initialization of static members
vxBlockVolume<uint2>  InteractiveSegmentation::m_defaultIntensityVol;   // Default scan volume.
vxShareableObject< vxBlockVolume<uint2> > InteractiveSegmentation::m_defaultLabelVolume;    // Default segmentation volume.


// defines
#define FILE_REVISION "$Revision: 1.22 $"


/**
* Default constructor
*/
InteractiveSegmentation::InteractiveSegmentation()
//*******************************************************************
{
  // Segmentation status
  m_iStatus = SEGMENTATION_OK;

  // Set default components
  m_pCompHdlr = & m_defaultCompHdlr;

  // Set default volumes:
  m_defaultIntensityVol.GetHeader().SetVolDim(2,2,2);
  m_defaultIntensityVol.GetHeader().SetVoxelUnits(Triplef(1,1,1));
  m_defaultLabelVolume.GetHeader().SetVolDim(2,2,2);
  m_defaultLabelVolume.GetHeader().SetVoxelUnits(Triplef(1,1,1));
  m_pIntensityVol = & m_defaultIntensityVol;
  m_pLabelVolume  = & m_defaultLabelVolume;

  // Set SegParams default:
  SetRegionGrowMethod(SegParams::INTENSITY_THRESHOLD); // Region grow method.
  SetRegionSampleRadius(3.0F);                                               // Region sampler radius.
  SetRegionGrowAddOpt(SegParams::NEW_REGIONS);          // Region grow adds options.
  SetRegionIntensityLow (1200);                                              // Region grow intensity range, low.
  SetRegionIntensityHigh(4095);                                              // Region grow intensity range, high.
  SetMorphOpIterations(1);                                                   // Morphological operations, number of iterations.
  SetSeedPoint(Point<uint2>(0,0,0));                                         // Seed point.
} // end default constructor


/**
* Method of running interactive segmentation. This works from the mode selected 
* and the segmentation parameters that are set.
* @param the mode of interactive segmentation
* @param option for run
*/
void InteractiveSegmentation::Run(const ModeIDsEnum & eModeID, 
                                  const int4 & iOption)
                                  //*******************************************************************
{
  // initialize status
  m_iStatus = SEGMENTATION_OK;

  // set delta tag volume for segmentation
  m_segVols.InitDeltaSegTagVolume();

  // switch on interactive segmentation mode
  switch (eModeID)
  {
  case REGION_GROW_MODE:              RunRegionGrow(m_seedPoint);          break;
  case REGION_THRESHOLD_SEGMENT_MODE: RunRegionThresholdSegment();         break;
  case REGION_MERGE_MODE:                                                  break;
  case REGION_SPLIT_MODE:                                                  break;
  case CLASSIFICATION_MODE:                                                break;
  case MORPH_OP_MODE:                 RunMorphOp(static_cast<enum SegParams::MorphOperSelEnum>(iOption)); break;
  case REGION_SAMPLE_2D:              RunRegionSample(m_seedPoint, uint4(Abs(iOption)));                  break;
  case REGION_SAMPLE_3D:              RunRegionSample(m_seedPoint, RegionSampler::SPHERICAL); break;
  case AUTO_CENTERLINE_MODE:                                               break;
  case AUTO_SKELETON_MODE:                                                 break;
  case MANUAL_SKELETON_MODE:                                               break;
  case MANUAL_SKELETON_BRANCH_MODE:                                        break;
  case COMPHDLR_UPDATE_MODE:          RunCompHdlrUpdate();                 break;
  default:
    // do nothing.
    break;
  }
} // Run()


/**
* Private method that calls the RegionGrow class
* @param rSeedPt 3D point coordinates of seed used in region grow.
*/
void InteractiveSegmentation::RunRegionGrow(const Point<uint2> & seedPoint)
//*******************************************************************
{ 
  // setup segmentation
  ///////////////////////
  uint2 uIndexLabel(0);

  // region grow add option
  SegParams::RegionAddOptionEnum eRegGrowAddOpt;
  m_segParams.Get(SegParams::REGION_ADD_OPT, &eRegGrowAddOpt);
  m_pCompHdlr->OnUserInputUpdate();

  // check whether we append to NONE
  bool bAppendingToNone(m_pCompHdlr->GetSelectedComponentIndex() == 0 && eRegGrowAddOpt == SegParams::APPEND_REGIONS);

  // volumes
  vxBlockVolume<bool> * pMskTagVol(m_pCompHdlr->GetMaskTagVolume());
  vxBlockVolume<bool> * pRegTagVol(m_pCompHdlr->AddCompReg(uIndexLabel, eRegGrowAddOpt));

  // region grow method
  SegParams::RegionGrowMethodEnum eRegionGrowMethod;
  m_segParams.Get(SegParams::REGION_GROW_METHOD, &eRegionGrowMethod);

  // region threshold range
  int4 uThresholdLow(0);
  int4 uThresholdHigh(0);
  if (eRegionGrowMethod == SegParams::SAMPLE_INTENSITY_RANGE)
  {
    std::pair< uint2, uint2 > uRange( m_regionSampler.GetThresholdRange() );
    uThresholdLow  = uRange.first;
    uThresholdHigh = uRange.second;
  }
  else
  {
    m_segParams.Get(SegParams::REGION_THRESHOLD_LOW, &uThresholdLow);
    m_segParams.Get(SegParams::REGION_THRESHOLD_HIGH, &uThresholdHigh);
  } // end if RegionGrowMethod is SampleIntensityRange

  // set and run region grow
  m_regionGrow.Set(RegionGrow::REGION_THRESHOLD_RANGE, uThresholdLow, uThresholdHigh);
  m_regionGrow.Run(seedPoint, m_pIntensityVol, pMskTagVol, pRegTagVol, m_validSubset);

  // update segmentation volume
  ///////////////////////////////

  // get number of voxels in region grow
  uint4 uNumOfRegVoxels(0);
  m_regionGrow.Get(RegionGrow::NUMBER_OF_REGION_VOXELS, &uNumOfRegVoxels);

  if (uNumOfRegVoxels > 0)
  {

    // update the label volume
    m_segVols.UpdateSegVolume(pRegTagVol, uIndexLabel);

    // update the individual region tag volumes
    m_pCompHdlr->UpdateRegionTags();

    // update the mask volume(to lock component)
    GetCompHdlrPtr()->ModifyMaskTagVol(pRegTagVol, true);

  }
  else // the region we generated is actually empty
  {
    if (eRegGrowAddOpt == SegParams::NEW_REGIONS || bAppendingToNone == true)
    {
      m_pCompHdlr->RemCompReg(m_pCompHdlr->GetComponentCount()-1);
      m_iStatus = FAILED_TO_SEGMENT;
    }
  }

  return;
} // RunRegionGrow()


/**
* Private method that calls the RegionThresholdSegment class.
*/
void InteractiveSegmentation::RunRegionThresholdSegment()
//*******************************************************************
{ 
  // setup segmentation
  ///////////////////////
  uint2 uIndexLabel(0);

  // region grow add option
  SegParams::RegionAddOptionEnum eRegGrowAddOpt;
  m_segParams.Get(SegParams::REGION_ADD_OPT, &eRegGrowAddOpt);
  m_pCompHdlr->OnUserInputUpdate();

  // check whether we append to NONE
  bool bAppendingToNone(m_pCompHdlr->GetSelectedComponentIndex() == 0 && eRegGrowAddOpt == SegParams::APPEND_REGIONS);

  // volumes
  vxBlockVolume<bool> * pMskTagVol(m_pCompHdlr->GetMaskTagVolume());
  vxBlockVolume<bool> * pRegTagVol(m_pCompHdlr->AddCompReg(uIndexLabel, eRegGrowAddOpt));

  // region grow method
  SegParams::RegionThresholdSegmentMethodEnum eRegionThresholdSegmentMethod;
  m_segParams.Get(SegParams::REGION_THRESHOLD_SEGMENT_METHOD, &eRegionThresholdSegmentMethod);

  // region threshold range
  int4 uThresholdLow(0);
  int4 uThresholdHigh(0);
  m_segParams.Get(SegParams::REGION_THRESHOLD_LOW, &uThresholdLow);
  m_segParams.Get(SegParams::REGION_THRESHOLD_HIGH, &uThresholdHigh);

  // set and run region threshold segmentation
  m_regionThresSeg.Set(RegionGrow::REGION_THRESHOLD_RANGE, uThresholdLow, uThresholdHigh);
  m_regionThresSeg.Run(m_pIntensityVol, pMskTagVol, pRegTagVol, m_validSubset);

  // update segmentation volume
  ///////////////////////////////

  // get number of voxels in region grow:
  uint4 uNumOfRegVoxels(0);
  m_regionThresSeg.Get(RegionGrow::NUMBER_OF_REGION_VOXELS, &uNumOfRegVoxels);

  if (uNumOfRegVoxels > 0)
  {
    m_segVols.UpdateSegVolume(pRegTagVol, uIndexLabel);
    m_pCompHdlr->UpdateRegionTags();
  }
  else // the region we generated is actually empty
  {
    if (eRegGrowAddOpt == SegParams::NEW_REGIONS || bAppendingToNone == true)
    {
      m_pCompHdlr->RemCompReg(m_pCompHdlr->GetComponentCount()-1);
      m_iStatus = FAILED_TO_SEGMENT;
    }
  } // end if Number Of Region Voxels > zero.

  return;
} // RunRegionThresholdSegment()


/**
* Private method that calls the MorphOp class
* @param eMorphOpSel enum for morphological option selected
*/
void InteractiveSegmentation::RunMorphOp(SegParams::MorphOperSelEnum eMorphOpSel)
//*******************************************************************
{ 
  // setup segmentation
  ///////////////////////
  uint2 uIndexHistory(m_pCompHdlr->GetSelectedComponentIndex());
  uint2 uIndexLabel( ( * m_pCompHdlr->GetComponentsPtr() )[ uIndexHistory ]->GetLabel() );

  if (uIndexHistory == 0)
  {
    m_iStatus = NO_SELECTED_REGION;
    return;
  }

  // volumes
  vxBlockVolume<bool> * pMskTagVol(m_pCompHdlr->GetMaskTagVolume());
  const std::vector<vxBlockVolume<bool> *> vpRegTagVol(m_pCompHdlr->GetRegTagVols(uIndexHistory));

  // region threshold range
  int4 uThresholdLow(0);
  int4 uThresholdHigh(0);
  m_segParams.Get(SegParams::REGION_THRESHOLD_LOW, &uThresholdLow);
  m_segParams.Get(SegParams::REGION_THRESHOLD_HIGH, &uThresholdHigh);

  // get segmentation parameters
  uint2 uMorphOpIter(0);
  m_segParams.Get(SegParams::MORPH_OP_ITER, &uMorphOpIter);

  // set the segmentation volume class
  m_morphOp.SetSegVol(&m_segVols);

  // run morphological operation
  ////////////////////////////////
  m_morphOp.Run(vpRegTagVol, pMskTagVol, uIndexLabel, eMorphOpSel, 
    m_pIntensityVol, uMorphOpIter, uThresholdLow, uThresholdHigh);

  // get number of voxels in region grow:
  uint4 uNumOfRegVoxels(0);
  m_regionThresSeg.Get(RegionGrow::NUMBER_OF_REGION_VOXELS, &uNumOfRegVoxels);

  // update region tags for components
  m_pCompHdlr->UpdateRegionTags();

  return;
} // RunMorphOp()


/**
* Private method that runs the RegionSample class. This will set the threshold range.
* @param rtSeedPt     3D point coordinates of seed used in region grow.
* @param uAxisIndexIn index of main axis, (0,1,2) => (x,y,z).
*/
void InteractiveSegmentation::RunRegionSample(const Point<uint2> & rSeedPt, 
                                              const uint4 & uAxisIndexIn)
                                              //*******************************************************************
{ 
  // Sample Plane(based on axis index):
  RegionSampler::SampleRestrictionEnum eSamplePlane[] = { RegionSampler::YZ_PLANE,
    RegionSampler::XZ_PLANE,
    RegionSampler::XY_PLANE,
    RegionSampler::SPHERICAL};
  uint4 uAxisIndex(Bound((uint4)0,uAxisIndexIn,(uint4)2));

  // threshold range
  int4 uThresholdLow( vxIntensityConverter::ModalityToRaw( GetRegionIntensityLow(), m_pIntensityVol->GetHeader() )); 
  int4 uThresholdHigh(  vxIntensityConverter::ModalityToRaw( GetRegionIntensityHigh(), m_pIntensityVol->GetHeader() ));   

  // set parameters
  m_regionSampler.SetSampleRadius( GetRegionSampleRadius() );

  // sample intensity
  m_regionSampler.SampleIntensityRangeOrtho( rSeedPt, m_pIntensityVol, eSamplePlane[ uAxisIndex ] );

  // update threshold range in segmentation parameters
  SetRegionIntensityLow( m_regionSampler.GetThresholdRange().first );
  SetRegionIntensityHigh( m_regionSampler.GetThresholdRange().second );

} // RunRegionSample()



/**
* Private method that updates the component handler
*/
void InteractiveSegmentation::RunCompHdlrUpdate()
//*******************************************************************
{
  m_pCompHdlr->OnUserInputUpdate();
} // RunCompHdlrUpdate()


/**
* Function to demonstrate the class functionality.
*/
#ifdef XXX
const bool InteractiveSegmentation::UnitTest()
//*******************************************************************
{
#ifdef DEBUG

  // interactiveSegmentation testIS;
  stringstream ss;
  ss << endl;
  bool bOK(true);
  bool bRetVal(true);

  // make volumes to work with
  LinVolInt2  * pSmallVolume      = new LinVolInt2 ( Triple<int2>(10,10,10), 8*sizeof(uint2), Triple<float4>(0.5,0.5,1), hdr::MILLIMETER);
  BlkVolUInt2 * pSmallLabelVolume = new BlkVolUInt2(Triple<uint2>(10,10,10), 8*sizeof(uint2), Triple<float4>(0.5,0.5,1), hdr::MILLIMETER);

  // component<uint1> comp(false);   // Component object.

  // make a component handler
  ComponentHandler compHdlr;

  // turn on some voxels to the pattern below 
  // ....... ....... ....... ....... ....+++
  // ....... ....... ....... ....... ....+++
  // ....... ....... ....... ....... ....+++
  // ....... ....... ....... ....... ....+++
  // ....... ....... ....... ....... .......
  // ....... .+..... .+..... ....... .......
  // .+..... +++.... .++.... ....... .......
  // ....... .+..... ....... ....... .......
  pSmallVolume->SetVoxel(Point<uint2>(1,1,1),1400);
  pSmallVolume->SetVoxel(Point<uint2>(0,1,1),1170);
  pSmallVolume->SetVoxel(Point<uint2>(2,1,1),1250);
  pSmallVolume->SetVoxel(Point<uint2>(1,0,1),1340);
  pSmallVolume->SetVoxel(Point<uint2>(1,2,1),1140);
  pSmallVolume->SetVoxel(Point<uint2>(1,1,0),1450);
  pSmallVolume->SetVoxel(Point<uint2>(1,1,2),1230);
  pSmallVolume->SetVoxel(Point<uint2>(2,1,2),1280);
  pSmallVolume->SetVoxel(Point<uint2>(2,2,2),1310);
  pSmallVolume->SetVoxel(Point<uint2>(5,5,5),1800);
  pSmallVolume->SetVoxel(Point<uint2>(5,6,5),1800);
  pSmallVolume->SetVoxel(Point<uint2>(5,7,5),1900);
  pSmallVolume->SetVoxel(Point<uint2>(5,8,5),2000);
  pSmallVolume->SetVoxel(Point<uint2>(6,5,5),1900);
  pSmallVolume->SetVoxel(Point<uint2>(6,6,5),1800);
  pSmallVolume->SetVoxel(Point<uint2>(6,7,5),1900);
  pSmallVolume->SetVoxel(Point<uint2>(6,8,5),2000);
  pSmallVolume->SetVoxel(Point<uint2>(7,5,5),2000);
  pSmallVolume->SetVoxel(Point<uint2>(7,6,5),1800);
  pSmallVolume->SetVoxel(Point<uint2>(7,7,5),1900);
  pSmallVolume->SetVoxel(Point<uint2>(7,8,5),2000);


  // Test Volume - Scan
  AnyVolume testAnyScanVolume;
  testAnyScanVolume.SetVolumePtr((void*) pSmallVolume);
  testAnyScanVolume.m_eLayout   = hdr::LINEAR;
  testAnyScanVolume.m_eDataType = hdr::INT2;

  // Test Volume - Segmentation
  AnyVolume testAnySegVolume; // Test any volume, segmentation.
  testAnySegVolume.SetVolumePtr((void*) pSmallLabelVolume);
  testAnySegVolume.m_eLayout   = hdr::BLOCK;
  testAnySegVolume.m_eDataType = hdr::UINT2;
  testSegVolumeSet.SetVolume((void*) pSmallLabelVolume);

  // Seed point:
  Point<uint2> tSeedPt1(1,1,2);
  Point<uint2> tSeedPt2(5,5,5);

  //-----------------------------------------
  // Test
  //-----------------------------------------

  // Test constants:
  uint2  iTestThresLow1    = 500;
  uint2  iTestThresHigh1   = 1500;
  uint2  iTestThresLow2    = 0;
  uint2  iTestThresHigh2   = 2000;
  float4 fTestSampleRadius = 3.0F;
  uint2  uTestMorphOpIters = 1;

  try
  {
    // Set volumes of I.S.
    SetScanVolume(testAnyScanVolume);
    SetSegmentVolume(&testSegVolumeSet);

    // Set comp.of I.S.:
    SetCompHdlrPtr(&compHdlr);

    // Run I.S.: Phase I
    SetRegionGrowIntensityLow(500);
    SetRegionGrowIntensityHigh(1500);
    RunRegionGrow(tSeedPt1);
    RunMorphOp(SegParams::MorphOperSelEnum::CLOSE);

    // Test Results: Phase I
    bOK = bOK && (GetRegionGrowMethod()        == SegParams::INTENSITY_THRESHOLD);
    bOK = bOK && (GetRegionGrowAddOpt()        == SegParams::NEW_REGIONS);
    bOK = bOK && (GetRegionGrowIntensityLow()  == iTestThresLow1       );
    bOK = bOK && (GetRegionGrowIntensityHigh() == iTestThresHigh1      );
    bOK = bOK && (GetMorphOpIterations()       == uTestMorphOpIters    );
    bOK = bOK && (compHdlr.GetComponentCount()-1   == 1                    );

    // Run I.S.: Phase II
    SetRegionGrowMethod(SegParams::SAMPLE_CLASS_RANGE);
    RunRegionSample(tSeedPt2, SegParams::SAMPLE_INTENSITY_RANGE);
    RunRegionGrow(tSeedPt2);

    // Test Results: Phase II
    bOK = bOK && (GetRegionGrowMethod()        == SegParams::SAMPLE_CLASS_RANGE);
    bOK = bOK && (GetRegionGrowAddOpt()        == SegParams::NEW_REGIONS);
    bOK = bOK && (GetRegionGrowIntensityLow()  == iTestThresLow2       );
    bOK = bOK && (GetRegionGrowIntensityHigh() == iTestThresHigh2      );
    bOK = bOK && (GetRegionSampleRadius()      == fTestSampleRadius    );
    bOK = bOK && (compHdlr.GetComponentCount()-1   == 2                    );

    // Run I.S.: Phase III
    //RunRegionRemove(2);

    // Test Results: Phase III
    bOK = bOK && (compHdlr.GetComponentCount()-1 == 1);

  }
  catch (...)
  {
    ss << "Error during test" << endl;
    bRetVal = false;
  }

  if (pSmallVolume      != NULL) delete pSmallVolume;
  if (pSmallLabelVolume != NULL) delete pSmallLabelVolume;

  // Generate log entry:

  ss << "              Method: " << GetRegionGrowMethod()          << endl;
  ss << "          Add Option: " << GetRegionGrowAddOpt()          << endl;
  ss << "      Threshold, Low: " << GetRegionGrowIntensityLow()    << endl;
  ss << "     Threshold, High: " << GetRegionGrowIntensityHigh()   << endl;
  ss << "       Sample Radius: " << GetRegionSampleRadius()        << endl;
  ss << "  MorphOp Iterations: " << GetMorphOpIterations()         << endl;
  ss << "Number Of Components: " << compHdlr.GetComponentCount()-1 << endl;
  ss << "test " << (bOK ? "passed" : "failed") << endl;

  LogDbg(ss.str(),"InteractiveSegmentation","UintTest");

  //cout << ss.str() << endl;

  return(bRetVal);

#else

  return(0);

#endif

} // UnitTest()
#endif // XXX


/**
* Function to demonstrate the class functionality
*
* @return const bool 
*/
const bool InteractiveSegmentation::UnitTest()
{
  return false;
  ////*******************************************************************
  //
  //#ifdef _DEBUG
  //
  //  stringstream ss;
  //  ss << endl;
  //	bool bOK     = true;
  //  bool bRetVal = true;
  //	
  //
  //  // Test Volume - Scan
  //  vxBlockVolume<uint2> testIntensityVolume;
  //  testIntensityVolume.GetHeader().SetVolDim(10,10,10);
  //  vxBlockVolumeIterator<uint2> iterIntensityVol(testIntensityVolume);
  //
  //  // Test Volume - Segmentation
  //  vxBlockVolume<uint2> testLabelVolume;
  //  testLabelVolume.GetHeader().SetVolDim(10,10,10);
  //
  //  // Make a component handler:
  //  ComponentHandler compHdlr;
  //  
  //  // turn on some voxels to the pattern below 
  //  // ....... ....... ....... ....... ....+++
  //  // ....... ....... ....... ....... ....+++
  //  // ....... ....... ....... ....... ....+++
  //  // ....... ....... ....... ....... ....+++
  //  // ....... ....... ....... ....... .......
  //	// ....... .+..... .+..... ....... .......
  //	// .+..... +++.... .++.... ....... .......
  //	// ....... .+..... ....... ....... .......
  //	iterIntensityVol.SetPos(Vector3D<int4>(1,1,1)); iterIntensityVol.SetVoxel(1400); //pSmallVolume->SetVoxel(Point<uint2>(1,1,1),1400);
  //	iterIntensityVol.SetPos(Vector3D<int4>(0,1,1)); iterIntensityVol.SetVoxel(1700); //pSmallVolume->SetVoxel(Point<uint2>(0,1,1),1170);
  //	iterIntensityVol.SetPos(Vector3D<int4>(2,1,1)); iterIntensityVol.SetVoxel(1250); //pSmallVolume->SetVoxel(Point<uint2>(2,1,1),1250);
  //	iterIntensityVol.SetPos(Vector3D<int4>(1,0,1)); iterIntensityVol.SetVoxel(1340); //pSmallVolume->SetVoxel(Point<uint2>(1,0,1),1340);
  //	iterIntensityVol.SetPos(Vector3D<int4>(1,2,1)); iterIntensityVol.SetVoxel(1140); //pSmallVolume->SetVoxel(Point<uint2>(1,2,1),1140);
  //	iterIntensityVol.SetPos(Vector3D<int4>(1,1,0)); iterIntensityVol.SetVoxel(1450); //pSmallVolume->SetVoxel(Point<uint2>(1,1,0),1450);
  //	iterIntensityVol.SetPos(Vector3D<int4>(1,1,2)); iterIntensityVol.SetVoxel(1230); //pSmallVolume->SetVoxel(Point<uint2>(1,1,2),1230);
  //	iterIntensityVol.SetPos(Vector3D<int4>(2,1,2)); iterIntensityVol.SetVoxel(1280); //pSmallVolume->SetVoxel(Point<uint2>(2,1,2),1280);
  //	iterIntensityVol.SetPos(Vector3D<int4>(2,2,2)); iterIntensityVol.SetVoxel(1310); //pSmallVolume->SetVoxel(Point<uint2>(2,2,2),1310);
  //	iterIntensityVol.SetPos(Vector3D<int4>(5,5,5)); iterIntensityVol.SetVoxel(1800); //pSmallVolume->SetVoxel(Point<uint2>(5,5,5),1800);
  //	iterIntensityVol.SetPos(Vector3D<int4>(5,6,5)); iterIntensityVol.SetVoxel(1800); //pSmallVolume->SetVoxel(Point<uint2>(5,6,5),1800);
  //	iterIntensityVol.SetPos(Vector3D<int4>(5,7,5)); iterIntensityVol.SetVoxel(1900); //pSmallVolume->SetVoxel(Point<uint2>(5,7,5),1900);
  //	iterIntensityVol.SetPos(Vector3D<int4>(5,8,5)); iterIntensityVol.SetVoxel(2000); //pSmallVolume->SetVoxel(Point<uint2>(5,8,5),2000);
  //	iterIntensityVol.SetPos(Vector3D<int4>(6,5,5)); iterIntensityVol.SetVoxel(1900); //pSmallVolume->SetVoxel(Point<uint2>(6,5,5),1900);
  //	iterIntensityVol.SetPos(Vector3D<int4>(6,6,5)); iterIntensityVol.SetVoxel(1800); //pSmallVolume->SetVoxel(Point<uint2>(6,6,5),1800);
  //	iterIntensityVol.SetPos(Vector3D<int4>(6,7,5)); iterIntensityVol.SetVoxel(1900); //pSmallVolume->SetVoxel(Point<uint2>(6,7,5),1900);
  //	iterIntensityVol.SetPos(Vector3D<int4>(6,8,5)); iterIntensityVol.SetVoxel(2000); //pSmallVolume->SetVoxel(Point<uint2>(6,8,5),2000);
  //	iterIntensityVol.SetPos(Vector3D<int4>(7,5,5)); iterIntensityVol.SetVoxel(2000); //pSmallVolume->SetVoxel(Point<uint2>(7,5,5),2000);
  //	iterIntensityVol.SetPos(Vector3D<int4>(7,6,5)); iterIntensityVol.SetVoxel(1800); //pSmallVolume->SetVoxel(Point<uint2>(7,6,5),1800);
  //	iterIntensityVol.SetPos(Vector3D<int4>(7,7,5)); iterIntensityVol.SetVoxel(1900); //pSmallVolume->SetVoxel(Point<uint2>(7,7,5),1900);
  //	iterIntensityVol.SetPos(Vector3D<int4>(7,8,5)); iterIntensityVol.SetVoxel(2000); //pSmallVolume->SetVoxel(Point<uint2>(7,8,5),2000);
  //
  //
  //  // Seed point:
  //  Point<uint2> tSeedPt1(1,1,2);
  //  Point<uint2> tSeedPt2(5,5,5);
  //  
  //  //-----------------------------------------
  //  // Test
  //  //-----------------------------------------
  //
  //  // Test constants:
  //  uint2  iTestThresLow1   = 500;
  //  uint2  iTestThresHigh1  = 1500;
  //  uint2  iTestThresLow2   = 0;
  //  uint2  iTestThresHigh2  = 2000;
  //  uint2 fTestSampleRadius = 3.0F;
  //  uint2 uTestMorphOpIters = 1;
  //
  //  try
  //  {
  //    // Set volumes of I.S.
  //    SetScanVolume(testIntensityVolume);
  //    SetSegmentVolume(&testLabelVolume);
  //
  //    // Set comp.of I.S.:
  //    SetCompHdlrPtr(&compHdlr);
  //
  //    // Run I.S.: Phase I
  //    SetRegionIntensityLow(500);
  //    SetRegionIntensityHigh(1500);
  //    RunRegionGrow(tSeedPt1);
  //    RunMorphOp(SegParams::MorphOperSelEnum::CLOSE);
  //
  //    // Test Results: Phase I
  //    bOK = bOK && (GetRegionGrowMethod()          == SegParams::INTENSITY_THRESHOLD);
  //    bOK = bOK && (GetRegionGrowAddOpt()          == SegParams::NEW_REGIONS);
  //    bOK = bOK && (GetRegionIntensityLow()        == iTestThresLow1       );
  //    bOK = bOK && (GetRegionIntensityHigh()       == iTestThresHigh1      );
  //    bOK = bOK && (GetMorphOpIterations()         == uTestMorphOpIters    );
  //    bOK = bOK && (compHdlr.GetComponentCount()-1 == 1                    );
  //
  //    // Run I.S.: Phase II
  //    SetRegionGrowMethod(SegParams::SAMPLE_CLASS_RANGE);
  //    RunRegionSample(tSeedPt2, SegParams::SAMPLE_INTENSITY_RANGE);
  //    RunRegionGrow(tSeedPt2);
  //
  //    // Test Results: Phase II
  //    bOK = bOK && (GetRegionGrowMethod()          == SegParams::SAMPLE_CLASS_RANGE);
  //    bOK = bOK && (GetRegionGrowAddOpt()          == SegParams::NEW_REGIONS);
  //    bOK = bOK && (GetRegionIntensityLow()        == iTestThresLow2       );
  //    bOK = bOK && (GetRegionIntensityHigh()       == iTestThresHigh2      );
  //    bOK = bOK && (GetRegionSampleRadius()        == fTestSampleRadius    );
  //    bOK = bOK && (compHdlr.GetComponentCount()-1 == 2                    );
  //    
  //    // Run I.S.: Phase III
  //    //RunRegionRemove(2);
  //
  //    // Test Results: Phase III
  //    bOK = bOK && (compHdlr.GetComponentCount()-1 == 1);
  //
  //  }
  //
  //  catch (...)
  //  {
  //    ss << "Error during test" << endl;
  //    bRetVal = false;
  //  }
  //
  //  // Generate log entry:
  //
  //  ss << "              Method: " << GetRegionGrowMethod()          << endl;
  //  ss << "          Add Option: " << GetRegionGrowAddOpt()          << endl;
  //  ss << "      Threshold, Low: " << GetRegionIntensityLow()        << endl;
  //  ss << "     Threshold, High: " << GetRegionIntensityHigh()       << endl;
  //  ss << "       Sample Radius: " << GetRegionSampleRadius()        << endl;
  //  ss << "  MorphOp Iterations: " << GetMorphOpIterations()         << endl;
  //  ss << "Number Of Components: " << compHdlr.GetComponentCount()-1 << endl;
  //  ss << "test " << (bOK ? "passed" : "failed") << endl;
  //
  //  LogDbg(ss.str(),"InteractiveSegmentation","UintTest");
  //
  ////cout << ss.str() << endl;
  //  
  //  return(bRetVal);
  //
  //#else
  //
  //  return(0);
  //
  //#endif // _DEBUG
} // UnitTest()



/**
* Grows a component
*
* @param iComponentIndex the index of the component to shrink
*
* @return if the component grow was successful)
*/
bool InteractiveSegmentation::GrowComponent( const int4 iComponentIndex )
{
  Timer timer;

  //determine the label
  const uint2 uGrowLabel = (*m_pCompHdlr->GetComponentsPtr())[ iComponentIndex ]->GetLabel();
  //Create a temp label
  const uint2 uTempLabel = m_pCompHdlr->GetComponentCount() + 100; 
  vxBlockVolumeIterator<uint2> labelIter( * m_pLabelVolume );

  //pre-compute the component lock information
  bool * pbLocked = new bool[ m_pCompHdlr->GetComponentsPtr()->ArrayByLabel().size() ];
  for ( int i=0; i<m_pCompHdlr->GetComponentsPtr()->ArrayByLabel().size(); i++ )
    pbLocked[ i ] = m_pCompHdlr->GetComponentsPtr()->ArrayByLabel()[ i ]->IsLocked();

  //loop over all the 6-neighbors of the voxels that match our label
  for ( labelIter.FirstNonBackgroundBlockZYX(); labelIter.IsNotAtEnd(); labelIter.NextNonBackgroundBlockZYX() )
  {
    //loop over all non-background voxels in block
    for ( ; labelIter.IsNotAtEndOfBlock(); labelIter.NextZYXinsideBlock() )
    {
      if ( labelIter.GetVoxel() == uGrowLabel /*&& !pbLocked[ uGrowLabel ] */ )  //No need to check for self lock

      {
        //For all the next 6 points check whether it is not the same as templabel and not locked and if it is inside Mark it as 
        //Next(+ve) X
        uint2 uFellowLabel(0);
        if ( ( uFellowLabel = labelIter.Peek1px0py0pz()) != uTempLabel && !pbLocked[ uFellowLabel ] &&
                m_validSubset.IsInside( Triple<int4>( labelIter.GetPos() ))  )
        {
          labelIter.SetVoxelNextX( uTempLabel );
        }

        //Prev(-ve) X
        if ( ( uFellowLabel = labelIter.Peek1nx0py0pz()) != uTempLabel && !pbLocked[ uFellowLabel ] &&
                m_validSubset.IsInside( Triple<int4>( labelIter.GetPos() )) )
        {
          labelIter.SetVoxelPrevX( uTempLabel );
        }

        //Next(+ve) Y
        if ( ( uFellowLabel = labelIter.Peek0px1py0pz()) != uTempLabel && !pbLocked[ uFellowLabel ] && 
                 m_validSubset.IsInside( Triple<int4>( labelIter.GetPos() )) )
        {
          labelIter.SetVoxelNextY( uTempLabel );
        }

        //Prev(-ve) Y
        if ( ( uFellowLabel = labelIter.Peek0px1ny0pz()) != uTempLabel && !pbLocked[ uFellowLabel ] && 
                 m_validSubset.IsInside( Triple<int4>( labelIter.GetPos() )))
        {
          labelIter.SetVoxelPrevY( uTempLabel );
        }

        //Next (+ve)Z 
        if ( ( uFellowLabel = labelIter.Peek0px0py1pz()) != uTempLabel &&   !pbLocked[ uFellowLabel ]  && 
                m_validSubset.IsInside( Triple<int4>( labelIter.GetPos() )))
        {
          labelIter.SetVoxelNextZ( uTempLabel );
        }

        ////Prev(-ve) Z
        if ( ( uFellowLabel = labelIter.Peek0px0py1nz()) != uTempLabel && !pbLocked[ uFellowLabel ] &&
                 m_validSubset.IsInside( Triple<int4>( labelIter.GetPos() )) )
        {
          labelIter.SetVoxelPrevZ( uTempLabel );
        }
      }
    } // for all non-background voxels
  } // for all non-background blocks


  bool bAnyVoxelsChanged = false;
  //  check if any voxels were modified in the making of this grow
  //  loop over the grown voxels
  for ( labelIter.FirstNonBackgroundBlockZYX(); labelIter.IsNotAtEnd(); labelIter.NextNonBackgroundBlockZYX() )
  {
    // loop over all non-background voxels in block
    for ( ; labelIter.IsNotAtEndOfBlock(); labelIter.NextZYXinsideBlock() )
    {
      // grow to this voxel
      if ( labelIter.GetVoxel() == uTempLabel )
      {
        bAnyVoxelsChanged = true;
        labelIter.SetVoxel( uGrowLabel );
      }
    } // for all non-background voxels
  } // for all non-background blocks

  // possibly set label volume modified
  if ( bAnyVoxelsChanged )
  {
    m_pLabelVolume->Modified( vxModInfo( vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_MODIFIED ) );
  }

  // clean up memory
  delete [] pbLocked;

  // writing timing to log
  LogDbg( "Component grow took: " + timer.AsString2(), "InteractiveSegmentation", "GrowComponent" );

  return bAnyVoxelsChanged;

} // GrowComponent()




/**
* Shrink a component
*
* @param iComponentIndex the index of the component to shrink
*
* @return if the component is still available (it will be automatically removed if shrunk to nothing)
*/
bool InteractiveSegmentation::ShrinkComponent( const int4 iComponentIndex )
{
  Timer timer;

  //determine the label
  const uint2 uShrinkLabel = (*m_pCompHdlr->GetComponentsPtr())[ iComponentIndex ]->GetLabel();
  vxBlockVolumeIterator<uint2> labelIter( * m_pLabelVolume );
  
  //Create a temp label
  const uint2 uTempLabel = m_pCompHdlr->GetComponentCount() + 200; 

  //pre-compute the component lock information
  bool * pbLocked = new bool[ m_pCompHdlr->GetComponentsPtr()->ArrayByLabel().size() ];
  for ( int i=0; i<m_pCompHdlr->GetComponentsPtr()->ArrayByLabel().size(); i++ )
    pbLocked[ i ] = m_pCompHdlr->GetComponentsPtr()->ArrayByLabel()[ i ]->IsLocked();


  //loop over all the 6-neighbors of the voxels that match our label
  for ( labelIter.FirstNonBackgroundBlockZYX(); labelIter.IsNotAtEnd(); labelIter.NextNonBackgroundBlockZYX() )
  {
    //loop over all non-background voxels in block
    for ( ; labelIter.IsNotAtEndOfBlock(); labelIter.NextZYXinsideBlock() )
    {
      if ( m_validSubset.IsInside( Triple<int4>( labelIter.GetPos() )) &&
           labelIter.GetVoxel() == uShrinkLabel )
        {
        uint2 uFellowLabel(0);
        //see if any voxels are touching outside the region
        if ( ( ( uFellowLabel = labelIter.Peek1px0py0pz()) != uShrinkLabel && uFellowLabel != uTempLabel  )  ||
             ( ( uFellowLabel = labelIter.Peek1nx0py0pz()) != uShrinkLabel && uFellowLabel != uTempLabel  )  ||
             ( ( uFellowLabel = labelIter.Peek0px1py0pz()) != uShrinkLabel && uFellowLabel != uTempLabel  )  ||
             ( ( uFellowLabel = labelIter.Peek0px1ny0pz()) != uShrinkLabel && uFellowLabel != uTempLabel  )  ||
             ( ( uFellowLabel = labelIter.Peek0px0py1pz()) != uShrinkLabel && uFellowLabel != uTempLabel  )  ||
             ( ( uFellowLabel = labelIter.Peek0px0py1nz()) != uShrinkLabel && uFellowLabel != uTempLabel  ) )
          {
            labelIter.SetVoxel( uTempLabel );
          }
        }

    }//for all non-background voxels
  }//for all non-background blocks

  
  bool bAnyVoxelsChanged = false;
  //  check if any voxels were modified in the making of this grow
  //  loop over the grown voxels
  for ( labelIter.FirstNonBackgroundBlockZYX(); labelIter.IsNotAtEnd(); labelIter.NextNonBackgroundBlockZYX() )
  {
    // loop over all non-background voxels in block
    for ( ; labelIter.IsNotAtEndOfBlock(); labelIter.NextZYXinsideBlock() )
    {
      // grow to this voxel
      if ( labelIter.GetVoxel() == uTempLabel )
      {
        bAnyVoxelsChanged = true;
        labelIter.SetVoxel( 0 );
      }
    } // for all non-background voxels
  } // for all non-background blocks


  // possibly set label volume modified
  if ( bAnyVoxelsChanged )
  {
    m_pLabelVolume->Modified( vxModInfo( vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_MODIFIED ) );
  }

  // clean up memory
  delete [] pbLocked;

  // writing timing to log
  LogDbg( "Component shrink took: " + timer.AsString2(), "InteractiveSegmentation", "ShrinkComponent" );

  return bAnyVoxelsChanged;

} // ShrinkComponent()



/**
* Threshold segment the entire volume.
*
* @return true if successful
*/
bool InteractiveSegmentation::ThresholdSegment()
{

  Timer timer;

  // region threshold range
  int4 uThresholdLow(0);
  int4 uThresholdHigh(0);
  m_segParams.Get(SegParams::REGION_THRESHOLD_LOW, &uThresholdLow);
  m_segParams.Get(SegParams::REGION_THRESHOLD_HIGH, &uThresholdHigh);

  // pre-compute the component lock information
  bool * pbLocked = new bool[ m_pCompHdlr->GetComponentsPtr()->ArrayByLabel().size() ];
  for ( int i=0; i<m_pCompHdlr->GetComponentsPtr()->ArrayByLabel().size(); i++ )
    pbLocked[ i ] = m_pCompHdlr->GetComponentsPtr()->ArrayByLabel()[ i ]->IsLocked();

  // prepare the iterators
  vxBlockVolume<bool> thresholdVolume( m_pLabelVolume->GetHeader() );
  vxBlockVolumeIterator<bool> thresholdIter( thresholdVolume );
  vxBlockVolumeIterator<uint2> labelIter( * m_pLabelVolume );
  vxBlockVolumeIterator<uint2> intensityIter( * m_pIntensityVol );
  bool bAnyVoxels = false;

	int4 iLowThresholdRaw   = vxIntensityConverter::ModalityToRaw( uThresholdLow,  m_pIntensityVol->GetHeader() );
	int4 iHighThresholdRaw  = vxIntensityConverter::ModalityToRaw( uThresholdHigh, m_pIntensityVol->GetHeader() );

  // loop over entire label volume
  for ( ; intensityIter.IsNotAtEnd(); intensityIter.NextBlockZYX() )
  {
    // loop over all non-background voxels in block
    for ( ; intensityIter.IsNotAtEndOfBlock(); intensityIter.NextZYXinsideBlock() )
    {
			int4 iValue = intensityIter.GetVoxel();

      //check if within range
      if ( iValue >= iLowThresholdRaw && iValue <= iHighThresholdRaw )
      {
        // check if not locked
        labelIter.SetPos( intensityIter );
        const uint2 uLabel = labelIter.GetVoxel();
        if ( ! pbLocked[ uLabel ] )
        {
          thresholdIter.SetPos( intensityIter );
          thresholdIter.SetVoxel( true );
          bAnyVoxels = true;
        }
      }
    } // for all non-background voxels
  } // for all non-background blocks

  if ( bAnyVoxels )
  {
    uint2 uLabel = vxComponentUtils::CreateNewComponent( thresholdVolume,
      vxComponentUtils::GetUniqueComponentName( * m_pCompHdlr->GetComponentsPtr(), GetNextComponentName() ),
      m_pCompHdlr->GetComponentsPtr(), m_pLabelVolume, * this, false );

    // set the color
    uint4 uCompIndex( m_pCompHdlr->GetComponentCount() - 1 );
    vxComponent * pComponent = m_pCompHdlr->GetComponentsPtr()->operator []( uCompIndex );
    pComponent->SetColor( GetNextComponentColor() );
  }

  // clean up memory
  delete [] pbLocked;

  LogDbg( "Threshold segment took: " + timer.AsString2(), "InteractiveSegmentation", "ThresholdSegment" );

  return bAnyVoxels;

} // ThresholdSegment()


// undefines
#undef FILE_REVISION


// $Log: InteractiveSegmentation.C,v $
// Revision 1.22  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.21.2.1  2007/04/03 20:16:30  romy
// fixed issue # 5554
//
// Revision 1.21  2007/02/13 22:11:20  romy
// We need to convert all modality values to RAW before the segmentation. Issue #5382
//
// Revision 1.20  2007/02/12 19:26:14  dongqing
// fix the bug for converting threshold from modilaty to raw. It needs header
// of intensity volume rather than label volume.
//
// Revision 1.19  2006/08/15 14:34:47  romy
// fixed threshold all performance problem
//
// Revision 1.18  2006/08/08 20:05:31  romy
// bug fix for thresholdsegment. Changed the threshold values from unit2 to int2
//
// Revision 1.17  2006/07/06 12:55:46  frank
// Issue #4840: Refactored component creation and naming method
//
// Revision 1.16  2006/04/20 17:57:37  romy
// removed self lock check on Grow and shrink
//
// Revision 1.15  2006/04/20 17:30:51  romy
// removed self lock check on Grow and shrink
//
// Revision 1.14  2006/04/20 16:58:24  romy
// Shrink revisited for naming conventions
//
// Revision 1.13  2006/04/20 14:53:45  romy
// shrink component revisited
//
// Revision 1.12  2006/04/19 20:13:34  romy
// Grow Component revisited
//
// Revision 1.11  2006/04/18 18:43:58  frank
// Issue #4711: Fixed the threshold all functionality - deprecated the old code
//
// Revision 1.10  2006/03/29 20:03:08  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.9  2006/03/02 15:50:01  frank
// cleaned up grow and shrink operations
//
// Revision 1.8  2006/02/28 15:24:42  frank
// improved undo capabilities during interactive segmentation
//
// Revision 1.7  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
//
// Revision 1.6  2006/02/17 15:08:05  frank
// added functions to grow and shrink a component
//
// Revision 1.5  2006/02/03 19:27:53  frank
// removed png library
//
// Revision 1.4  2006/01/19 13:16:43  geconomos
// corrected compilation error
//
// Revision 1.3  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.2  2005/11/18 21:14:53  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.1.1.1.12.2  2005/07/19 14:15:08  frank
// bad access to component array fixed
//
// Revision 1.1.1.1.12.1  2005/07/11 17:43:42  frank
// cleaned up naming and color of components
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.89  2004/01/23 16:31:15  frank
// computed and displayed Find Region with sampling for oblique & 3D views
//
// Revision 1.88  2004/01/21 19:52:19  frank
// fixed locking the component during find region manipulation
//
// Revision 1.87  2003/12/22 16:20:56  michael
// worked on the DeciderSphere to take a radius that is float since it needs to
// be scaled anisotropically with the spacing of the volume which cannot be
// handled properlin on an integer basis.
//
// Revision 1.86  2003/12/22 12:47:29  frank
// Added a valid subset to the threshold and region grow operations
//
// Revision 1.85  2003/07/08 14:40:31  michael
// took our redundant code
//
// Revision 1.84  2003/06/26 14:24:57  dongqing
// comment
//
// Revision 1.83  2003/06/24 13:31:26  michael
// replaced exception by logger warning ...
//
// Revision 1.82  2003/06/23 19:29:27  dongqing
// change from int2 to uint2
//
// Revision 1.81  2003/06/19 20:20:05  michael
// adjusted segmentation from int2 to uint2, following the changes in the block volume.
//
// Revision 1.80  2003/06/18 15:19:54  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.79  2003/06/09 12:29:54  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.78  2003/06/07 15:36:37  michael
// reformatting
//
// Revision 1.77  2003/05/16 13:20:37  frank
// formatting
//
// Revision 1.76  2003/04/28 17:01:31  michael
// more code reviews
//
// Revision 1.75  2003/04/25 19:16:42  dongqing
// code review
//
// Revision 1.74  2003/04/14 21:06:04  michael
// performed initial code review
//
// Revision 1.73  2003/04/09 15:07:01  michael
// further step in cleaning up the semgentation/component handler code. In most
// locations there now is a IndexHistory and IndexLabel to better differentiate what
// is used. E.g. SegmentationVolumes and renderers use IndexLabel but internally
// the segmentation/component handler mostly works on HistoryIndex (component pane).
//
// Revision 1.72  2003/04/08 20:55:37  michael
// added missing update of maskt  tag volume
//
// Revision 1.71  2003/04/08 17:28:10  michael
// took out redundant updates of the binary histogram in segmentation
//
// Revision 1.70  2003/04/08 16:59:30  michael
// fixed most issues related to appending to NONE which is not possible
//
// Revision 1.69  2003/04/07 15:12:58  michael
// fixed issues #3046
//
// Revision 1.68  2003/03/26 04:06:15  michael
// fixed status to not be failed if the append is not to NONE
// (subsequent bug of fixing issue #2972)
//
// Revision 1.67  2003/03/26 03:30:42  michael
// fixed issue #2972
//
// Revision 1.66  2003/01/22 21:51:01  ingmar
// dimension and units are now Triples
//
// Revision 1.65  2001/01/09 16:58:17  frank
// Extended region sampling to 3D spherical region.
//
// Revision 1.64  2002/12/13 19:22:39  ingmar
// restricted "grow" operation (Dilate) to only include voxels in the active intensity range as long as it is beeing displayed
// also renamed Scan Vol and Seg Vol to IntensityVol and Label Vol
//
// Revision 1.63  2002/12/12 17:07:38  ingmar
// removed binary histogram 04 (3x better histogram creation time, only slightly slower raw region growing, faster overall region grow time when also counting volume updates)
//
// Revision 1.62  2002/11/26 13:42:25  michael
// fixed bug in indexing region tag volume and actual label index
//
// Revision 1.61  2002/11/25 21:07:21  michael
// update mask volume
//
// Revision 1.60  2002/11/15 23:26:09  ingmar
// added binary label histogram updates after all remaining segmentation operations
//
// Revision 1.59  2002/11/15 23:08:08  ingmar
// added binary label histogram
//
// Revision 1.58  2002/09/10 19:04:25  jaddonisio
// i. New call to region grow(passing pMsk04Vol).
// ii. Fix bug in RegionSegmentThreshold by removing bad region with index.
//
// Revision 1.57  2002/07/22 18:24:10  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.56.2.1  2002/07/02 22:24:51  michael
// added optimization using the binary histogram of the intensity volume for auto segmentation
//
// Revision 1.56  2002/06/21 18:16:02  jaddonisio
// Modified region sample to use different planes(XY,YZ,XZ).
//
// Revision 1.55  2002/06/19 14:48:05  jaddonisio
// Changed default high threshold to max value.
//
// Revision 1.54  2002/06/16 18:56:00  jaddonisio
// Offset is now 1024 for HU to raw.
//
// Revision 1.53  2002/06/13 23:03:18  jaddonisio
// Changed name on Get/Set region thresholds.
//
// Revision 1.52  2002/06/13 22:19:00  jaddonisio
// i. New RegionThresholdSegment function.
// ii. Renamed Get/Set function for threshold range.
//
// Revision 1.51  2002/06/10 18:21:08  jaddonisio
// Update regions after segmenting.
//
// Revision 1.50  2002/06/03 21:30:41  jaddonisio
// Coding standard changes.
//
// Revision 1.49  2002/05/15 21:10:22  jaddonisio
// i. New status member variable(along with access functions) for segmentation.
// ii. No longer calling Windows message box. Now setting status.
//
// Revision 1.48  2002/05/08 13:10:45  jaddonisio
// Put back message boxes(temporarily) instead of throwing exceptions.
//
// Revision 1.47  2002/05/04 17:00:50  soeren
// changed from logfyi to logdbg
//
// Revision 1.46  2002/05/03 22:25:47  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.45  2002/05/02 22:25:35  jaddonisio
// Clean up(related to old block volume).
//
// Revision 1.44  2002/05/01 22:36:17  michael
// replaced VOLUMESET and VOLUMELABELSET by VOLUME_{INTENSITY,LABEL}
//
// Revision 1.43  2002/05/01 22:10:54  michael
// Replaced DensityVolume by IntensityVolume
//
// Revision 1.42  2002/05/01 19:50:04  jaddonisio
// More clean up(VolumeSet/AnyVolume no longer used).
//
// Revision 1.41  2002/05/01 14:06:44  jaddonisio
// Clean-up.
//
// Revision 1.40  2002/04/30 20:03:47  jaddonisio
// No more VolumeSet.
//
// Revision 1.39  2002/04/29 19:40:05  jaddonisio
// Run RegionMorphOpNew.
//
// Revision 1.38  2002/04/27 23:15:07  jaddonisio
// Segmentation with new block volumes. Pardon the mess.
//
// Revision 1.37  2002/04/18 16:19:23  jaddonisio
// InteractiveSegmentation class now contains a pointer to a Component Handler.
//
// Revision 1.36  2002/04/15 15:33:31  jaddonisio
// New class: SegmentationVolumes.
//
// Revision 1.35  2002/04/11 18:05:59  geconomos
// Fixed compile time bug.
//
// Revision 1.34  2002/04/10 18:29:44  jaddonisio
// Changed name of delta seg tag vol.
//
// Revision 1.33  2002/04/06 00:05:54  jaddonisio
// Removed display option out of segmentation(its part of rendering mode).
//
// Revision 1.32  2002/04/04 00:44:21  jaddonisio
// New delta segmentation tag volume.
//
// Revision 1.31  2002/03/07 22:31:07  dmitry
// Modified to fit new architecture.
//
// Revision 1.30  2002/01/10 15:04:16  jaddonisio
// Inserted/modified comments.
//
// Revision 1.29  2001/12/20 22:17:40  jaddonisio
// New UnitTest() method.
//
// Revision 1.28  2001/12/17 16:56:25  jaddonisio
// Changed thresholds from uint2 to int2.
//
// Revision 1.27  2001/12/07 23:51:27  jaddonisio
// Corrected function comments for doxygen.
//
// Revision 1.26  2001/12/06 23:54:21  jaddonisio
// More mods for coding standard compliance.
//
// Revision 1.25  2001/12/06 17:33:06  jaddonisio
// Mods to comply with coding standard: comments on closing brackets of 'if' statements.
//
// Revision 1.24  2001/12/06 17:06:07  jaddonisio
// Mods to comply with coding standard.
//
// Revision 1.23  2001/12/05 23:26:05  jaddonisio
// Mod. enum names in SegParams to comply with coding standard.
//
// Revision 1.22  2001/12/05 21:24:43  jaddonisio
// Modifications for compliance with coding standard.
//
// Revision 1.21  2001/11/30 23:47:49  jaddonisio
// i. Changed 'REGION_GROW_ADDS' to REGION_ADD_OPTIONS'.
// ii. Removed seed point in MorphOp method.
//
// Revision 1.20  2001/11/21 20:05:52  jaddonisio
// New morphology based on vector or region tag volumes(removed seed point methods).
//
// Revision 1.19  2001/11/20 18:02:48  jaddonisio
// Fix bug that deleted components when region grow fails.
//
// Revision 1.18  2001/11/18 00:48:16  jaddonisio
// i. Checks for no region grow voxels.
// ii. For region removal: new method called 'RemoveLabelFromSegVolume' instead of 'UpdateSegVolume'.
//
// Revision 1.17  2001/11/16 19:58:41  jaddonisio
// New public method 'RunCompHdlr" to update Component Handler.
//
// Revision 1.16  2001/11/16 00:27:00  jaddonisio
// New RegionRemove.
//
// Revision 1.15  2001/11/15 01:16:25  jaddonisio
// Corrected set of thresholds: high was low and low was high!
//
// Revision 1.14  2001/11/14 16:00:21  jaddonisio
// i. New enum events.
// ii. New RunRegionRemove method.
//
// Revision 1.13  2001/11/13 21:39:11  jaddonisio
// New RunRegionSample and Set/Get seed point.
//
// Revision 1.12  2001/11/12 23:52:45  jaddonisio
// Set a default VolumeSet to avoid crash.
//
// Revision 1.11  2001/11/12 00:35:02  jaddonisio
// New bool for displaying thresholds.
//
// Revision 1.10  2001/11/11 21:18:51  jaddonisio
// FireOnChanged event for morphing.
//
// Revision 1.9  2001/11/11 19:08:14  geconomos
// Changed signature on SetSegmentVolume and added volume events.
//
// Revision 1.8  2001/11/11 05:29:39  jaddonisio
// MorphOp mods and new method to set seg. volume.
//
// Revision 1.7  2001/11/11 02:22:33  jaddonisio
// Pass seed points of components.
//
// Revision 1.6  2001/11/09 21:54:31  jaddonisio
// New MorphOP class.
//
// Revision 1.5  2001/11/07 23:21:41  jaddonisio
// Renamed 'SetRegionGrowAddOpts'
//
// Revision 1.4  2001/11/06 21:46:27  jaddonisio
// New RegionSampler.
//
// Revision 1.3  2001/11/02 23:04:43  jaddonisio
// Changed thresholds to uint2.
//
// Revision 1.2  2001/11/02 15:33:16  soeren
// removed stl warnings
//
// Revision 1.1  2001/10/31 22:32:58  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/InteractiveSegmentation.C,v 1.22 2007/06/28 19:33:55 jmeade Exp $
// $Id: InteractiveSegmentation.C,v 1.22 2007/06/28 19:33:55 jmeade Exp $
