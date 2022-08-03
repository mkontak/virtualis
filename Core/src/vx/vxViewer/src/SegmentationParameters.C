// $Id: SegmentationParameters.C,v 1.2 2007/06/28 19:33:55 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)

/**
 * Class that encapsulates the Segmentation Parameters. It provides set/get
 * for access to all parameters.
 *
 */


// includes
#include "stdafx.h"
#include "SegmentationParameters.h"


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Default constructor
 */
SegParams::SegParams()
//*******************************************************************
{
  m_eRegionGrowMethodSel     = DEFAULT_REGION_GROW;  // Region grow method selection.
  m_eRegionThresSegMethodSel = DEFAULT_REGION_THRESHOLD_SEGMENT; // Region threshold segmentation method selection.
  m_eRegionGrowAddSel        = DEFAULT_REGION_ADDS;  // Region grow add selection.
  m_uRegionGrowIterations    = 0;                    // Region grow iterations.
  m_fRegionSampleRadius      = 0.0F;                 // Region sample radius.
  m_iRegionIntensityLow      = 0;                    // Region intensity range, low.
  m_iRegionIntensityHigh     = 0;                    // Region intensity range, high.
  m_uRegionClassHigh         = 0;                    // Region class range, high.
  m_uRegionClassLow          = 0;                    // Region class range, low.
  m_eMorphOpSelection        = DEFAULT_MORPH_OP;     // Morphological operation selection.
  m_uMorphOpIterations       = 0;                    // Morphological operation iterations.
  m_eConnectivitySel         = DEFAULT_CONNECTIVITY; // Conectivity selection.
  m_uNumberOfClasses         = 0;                    // Number of classes.
  m_uClassVoxelType          = 0;                    // Classification voxel type.
  m_uClassIntensityRangeHigh = 0;                    // Class intensity range, high.
  m_uClassIntensityRangeLow  = 0;                    // Class intensity range, low.
  m_eClassWinSel             = DEFAULT_CLASS_WINDOW; // Classification window selection.
  m_uContourAdjTension       = 0;                    // Contour adjustment tension.
}


/**
 * Public method that gets properties of the SegParams class
 * @param ePropID   enumeration of property id
 * @param ...       variable argument list for get
 * @return Status of get
 */
int4 SegParams::Get(const PropIDsEnum ePropID, ...)
//*******************************************************************
{
  int4 iStatus(0);
  va_list ap;
  va_start(ap, ePropID);

  switch (ePropID)
  {
    case REGION_GROW_METHOD:
    {
      RegionGrowMethodEnum * pVar = va_arg(ap,RegionGrowMethodEnum*);
      if (pVar != NULL)
      {
        *pVar = m_eRegionGrowMethodSel;
      }
    }
    break; // case REGION_GROW_METHOD
    case REGION_ADD_OPT:
    {
      RegionAddOptionEnum * pVar = va_arg(ap,RegionAddOptionEnum*);
      if (pVar != NULL)
      {
        *pVar = m_eRegionGrowAddSel;
      }
    }
    break; // case REGION_ADD_OPT
    case REGION_THRESHOLD_SEGMENT_METHOD:
    {
      RegionThresholdSegmentMethodEnum * pVar = va_arg(ap,RegionThresholdSegmentMethodEnum*);
      if (pVar != NULL)
      {
        *pVar = m_eRegionThresSegMethodSel;
      }
    }
    break; // case REGION_THRESHOLD_SEGMENT_METHOD
    case REGION_SAMPLE_RADIUS:
    {
      float4 * pVar = va_arg(ap, float4*);
      if (pVar != NULL)
      {
        *pVar = m_fRegionSampleRadius;
      }
    }
    break; // case REGION_SAMPLE_RADIUS
    case REGION_THRESHOLD_LOW:
    {
      int4 * pVar = va_arg(ap,int4*);
      if (pVar != NULL)
      {
        *pVar = m_iRegionIntensityLow;
      }
    }
    break; // case REGION_THRESHOLD_LOW
    case REGION_THRESHOLD_HIGH:
    {
      int4 * pVar = va_arg(ap,int4*);
      if (pVar != NULL)
      {
        *pVar = m_iRegionIntensityHigh;
      }
    }
    break; // case REGION_THRESHOLD_HIGH
    case MORPH_OP_ITER:
    {
      int4 * pVar = va_arg(ap,int4*);
      if (pVar != NULL)
      {
        *pVar = m_uMorphOpIterations;
      }
    }
    break; // case MORPH_OP_ITER
    default:
    break;
  }

  va_end(ap);

  return iStatus;
} // Get()


/**
 * Public method that sets properties of the SegParams class
 * @param ePropID   enumeration of property id
 * @param ...       variable argument list for set
 * @return Status of set
 */
int4 SegParams::Set(const PropIDsEnum ePropID, ...)
//*******************************************************************
{
  int4 iStatus(0);
  va_list ap;
  va_start(ap, ePropID);

  switch (ePropID)
  {
    case REGION_GROW_METHOD:
      m_eRegionGrowMethodSel = va_arg(ap, RegionGrowMethodEnum);
    break;
    case REGION_ADD_OPT:
      m_eRegionGrowAddSel = va_arg(ap, RegionAddOptionEnum);
    break;
    case REGION_THRESHOLD_SEGMENT_METHOD:
      m_eRegionThresSegMethodSel = va_arg(ap, RegionThresholdSegmentMethodEnum);
    break;
    case REGION_SAMPLE_RADIUS:
      m_fRegionSampleRadius = va_arg(ap, float8); // <== I DO NOT KNOW WHY WE NEED FLOAT8 AND NOT FLOAT4!!!
    break;
    case REGION_THRESHOLD_LOW:
      m_iRegionIntensityLow = va_arg(ap, int4);
    break;
    case REGION_THRESHOLD_HIGH:
      m_iRegionIntensityHigh = va_arg(ap, int4);
    break;
    case MORPH_OP_ITER:
      m_uMorphOpIterations = va_arg(ap, uint4);
    break;
    default:
    break;
  }

  va_end(ap);

  return iStatus;
} // Set()


#undef FILE_REVISION


// Revision History:
// $Log: SegmentationParameters.C,v $
// Revision 1.2  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.1.1.1.28.1  2007/04/25 19:46:20  romy
// converted threshold values to int4. Issue#5611
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.17  2003/12/22 16:20:58  michael
// worked on the DeciderSphere to take a radius that is float since it needs to
// be scaled anisotropically with the spacing of the volume which cannot be
// handled properlin on an integer basis.
//
// Revision 1.16  2003/06/07 15:36:37  michael
// reformatting
//
// Revision 1.15  2003/05/16 13:20:38  frank
// formatting
//
// Revision 1.14  2003/04/28 17:01:32  michael
// more code reviews
//
// Revision 1.13  2003/04/25 19:16:42  dongqing
// code review
//
// Revision 1.12  2003/04/14 21:06:03  michael
// performed initial code review
//
// Revision 1.11  2002/06/13 22:19:51  jaddonisio
// i. New RegionThresholdSegment function.
// ii. Renamed Get/Set function for threshold range.
//
// Revision 1.10  2002/04/04 16:19:03  geconomos
// Changed al 16-bit integers to 32-bit integers.
//
// Revision 1.9  2001/12/17 16:36:43  jaddonisio
// Changed thresholds to int2 instead of uint2.
//
// Revision 1.8  2001/12/07 23:51:27  jaddonisio
// Corrected function comments for doxygen.
//
// Revision 1.7  2001/12/05 23:26:05  jaddonisio
// Mod. enum names in SegParams to comply with coding standard.
//
// Revision 1.6  2001/11/30 23:45:52  jaddonisio
// Changed 'REGION_GROW_ADDS' to REGION_ADD_OPTIONS'
//
// Revision 1.5  2001/11/09 21:53:59  jaddonisio
// New property: MorphOpIter
//
// Revision 1.4  2001/11/06 21:39:09  jaddonisio
// New region grow method selection: SampleIntensityRange
//
// Revision 1.3  2001/11/02 23:03:57  jaddonisio
// inserted missing 'break's'
//
// Revision 1.2  2001/11/02 16:10:29  jaddonisio
// Added implementation of 'get's' for region grow threshold.
//
// Revision 1.1  2001/10/31 22:32:58  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/SegmentationParameters.C,v 1.2 2007/06/28 19:33:55 jmeade Exp $
// $Id: SegmentationParameters.C,v 1.2 2007/06/28 19:33:55 jmeade Exp $
