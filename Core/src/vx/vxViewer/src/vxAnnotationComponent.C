// $Id: vxAnnotationComponent.C,v 1.13 2007/06/28 19:33:55 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Michael Meissner (mailto:meissner@viatronix.com)


// pragmas
#pragma warning (disable : 4786)


// includes
#include "stdafx.h"
#include "vxAnnotationComponent.h"
#include "vxViewerType.h"
#include "vxMouseCursor.h"
#include "GLFontGeorge.h"
#include "vxEnvironment.h"
#include "Point.h"
#include "vxIntensityStatistic.h"
#include "vxSerializer.h"
#include "ResourceImage.h"
#include <limits>


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.13 $"
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif


/**
 * Default constructor
 */
vxAnnotationComponent::vxAnnotationComponent()
//*******************************************************************
{
  CommonInit();

	m_bLabelEditable = false;
}


/**
 * CommonInit
 */
void vxAnnotationComponent::CommonInit()
//*******************************************************************
{
  SetMouseCursor("CURSOR_VOLUME_MEASURE");
  SetImage(ResourceImage::FromResource("ANNOTATION_COMPONENT"));
	
 // SetLabel("");
} // CommonInit()

/**
 * copy constructor
 */
vxAnnotationComponent::vxAnnotationComponent( const vxAnnotationComponent & other ) : vxAnnotation( other )
{
	CommonInit();
}//copy constructor


/**
 *Clone method
 */
vxAnnotationComponent * vxAnnotationComponent::Clone() const
{
	return new vxAnnotationComponent( *this );
}//Clone method


/**
 * Reset
 */
void vxAnnotationComponent::Reset()
//*******************************************************************
{
  // call base class
  vxAnnotation::Reset();

  // initialize all the local stuff
  CommonInit();
} // Reset()


/**
 * Update the annotation text
 * @param environment
 */
uint2 vxAnnotationComponent::GetComponentIndex()
//*******************************************************************
{
  // label volume iterator
  vxBlockVolumeIterator<uint2> iterLabel(*GetEnvironment()->GetLabelVolume());

  // get demographics
  Triple<float4> units(GetEnvironment()->GetLabelVolume()->GetHeader().GetVoxelUnits());
  Point3Df position(GetHandle(0).GetPosition());

  // get label
  iterLabel.SetPos(position[0]/units[0] + 0.5F, 
                   position[1]/units[1] + 0.5F, 
                   position[2]/units[2] + 0.5F);

  return iterLabel.GetVoxel();
} // GetComponentIndex()


/**
 * Update the annotation text
 * @param environment
 */
void vxAnnotationComponent::Update(vxEnvironment & environment)
//*******************************************************************
{
  // get label index
  uint2 uComponentIndex(GetComponentIndex());

  // update the label (name of component it is holding on to)
  SetLabel(GetEnvironment()->GetComponents()->ArrayByLabel()[uComponentIndex]->GetName());

  if (uComponentIndex == 0)
  {
    SetText("");
  }
  else
  {
    // intensity and label volume iterator
    vxBlockVolumeIterator<uint2> iterLabel(*GetEnvironment()->GetLabelVolume());
    vxBlockVolumeIterator<uint2> iterIntensity(*GetEnvironment()->GetIntensityVolume());
    iterIntensity.SetPos(0, 0, 0); 

    // get dimensions volume dimension
    const Vector3D<int4> size(GetEnvironment()->GetLabelVolume()->GetHeader().GetVolDim());

    int4 uSum(0), uCount(0);
    float8 fSum(0);
    int4 iMin(std::numeric_limits<uint2>::max());
    int4 iMax(std::numeric_limits<uint2>::min());
    // loop over the label volume
    for (iterLabel.FirstNonBackgroundBlockZYX(); iterLabel.IsNotAtEnd(); iterLabel.NextNonBackgroundBlockZYX())
    {
     for (; iterLabel.IsNotAtEndOfBlock(); iterLabel.NextZYXinsideBlock())
      {
        if (iterLabel.GetVoxel() == uComponentIndex)
        {
          iterIntensity.SetPos(iterLabel.GetPos());
          const uint2 uVoxel(iterIntensity.GetVoxel());
          if( uVoxel > iMax )
            iMax = uVoxel;
          if ( uVoxel < iMin )
            iMin = uVoxel;
          fSum += uVoxel;
          ++uCount;
        }
      }
    }

    float4 fAverage( 0 );
    float4 fStandardDeviation( 0 );
    if( uCount > 0 )
    {
      fAverage = fSum / static_cast<float4>( uCount );

      float8 fSumDifferencesSquared( 0.0F );

      // loop over the label volume
      for (iterLabel.FirstNonBackgroundBlockZYX(); iterLabel.IsNotAtEnd(); iterLabel.NextNonBackgroundBlockZYX())
      {
        for (; iterLabel.IsNotAtEndOfBlock(); iterLabel.NextZYXinsideBlock())
        {
          if (iterLabel.GetVoxel() == uComponentIndex)
          {
            iterIntensity.SetPos(iterLabel.GetPos());
            const uint2 uVoxel(iterIntensity.GetVoxel());
            const float4 fDifference = uVoxel - fAverage;
            fSumDifferencesSquared +=  fDifference * fDifference;
          }
        }
      }
      fStandardDeviation = sqrt( fSumDifferencesSquared / ( uCount - 1 ) );
    }
      
    // get demographics
    Triple<float4> units(GetEnvironment()->GetLabelVolume()->GetHeader().GetVoxelUnits());

    // create satistics
    float4 fVolume(uCount*units[0]*units[1]*units[2]);

    string sAreaOrVolumeUnits("mm^3" );
    if( fVolume > 10000.0F )
    {
      fVolume = fVolume/1000.0F;
      sAreaOrVolumeUnits = "cm^3";
    }

    // set the text
    string sAreaOrVolume("Volume");
    FormatAndSetStatisticText( iMin, iMax, fAverage, fStandardDeviation, sAreaOrVolume, fVolume, sAreaOrVolumeUnits);

  } // index not zero

  return;
} // Update()


/**
 * Write object to XML node
 * @param node
 */
void vxAnnotationComponent::ToXml(vxDOMElement & element) const
//*******************************************************************
{
  // call parent class
  vxAnnotation::ToXml(vxSerializer::AddElement(element, vxAnnotation::GetElementName(), "# parent class #"));
} // ToXml()


/**
 * Initialize from XML node
 * @param viewer type
 * @return successful?
 */
bool vxAnnotationComponent::FromXml(vxDOMElement & element)
//*******************************************************************
{
  bool bSuccess(true);

  // call parent class
  bSuccess &= vxAnnotation::FromXml(vxSerializer::FindElement(element, vxAnnotation::GetElementName(), "# parent class #"));

  return bSuccess;
} // FromXml()


#undef FILE_REVISION


// Revision History:
// $Log: vxAnnotationComponent.C,v $
// Revision 1.13  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.12  2007/05/29 18:00:26  dongqing
// make the volume measurement adaptive display to mm or cm ^ 3
//
// Revision 1.11.2.1  2007/05/29 18:22:17  dongqing
// make the volume measurement adaptive display to mm or cm ^ 3
//
// Revision 1.11  2006/11/24 19:17:49  romy
// Put back Convariant virtual Clone methods back
//
// Revision 1.10  2006/11/23 06:23:15  romy
// cloning modified
//
// Revision 1.9  2006/09/25 21:31:16  romy
// commoninit modified for proper copying
//
// Revision 1.8  2006/09/25 20:48:01  romy
// added label editable flag for Volume annotations. Also fixed the Undo bug with volume label
//
// Revision 1.7  2006/08/07 20:26:23  romy
// added vxUndoActionAnnotationAdjust
//
// Revision 1.6  2005/11/18 21:14:53  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.5.2.1  2005/08/16 18:25:32  geconomos
// Issue# 4356: (George &Frank) Reworked computation of volume annotation.
//
// Revision 1.5  2005/04/12 15:23:32  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.4  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.2.6.2  2005/02/17 18:10:43  michael
// Added access function to get the underlying component index (from label volume)
// so that there is one access function to use. Related to issue #3972
//
// Revision 1.2.6.1  2005/02/09 14:29:24  michael
// Merged from "trunc" to branch since I didn't have the trunc. Will do better next time.
//
// Revision 1.3  2005/02/09 14:21:56  michael
// Component now always shows a label (component's name that it holds on to)
//
// Revision 1.2  2004/04/06 16:24:17  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.16  2004/01/23 03:51:31  michael
// reworked annotation to have a vxHandle class and a vxHandlesObj contains
// a vector of handles and access functions. The vxAnnotation derives from this
// one and overloads needed SetHandle() calls to make needed adjustments.
//
// Revision 1.15  2003/09/22 21:11:50  michael
// added "mailto:"
//
// Revision 1.14  2003/06/18 15:19:51  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.13  2003/06/04 18:27:36  michael
// added namespace declaration
//
// Revision 1.12  2003/05/16 13:17:39  frank
// formatting
//
// Revision 1.11  2003/04/25 19:15:32  dongqing
// code review
//
// Revision 1.10  2003/04/24 13:46:29  michael
// initial code reviews
//
// Revision 1.9  2003/04/01 17:53:05  michael
// cosmetics
//
// Revision 1.8  2003/03/26 17:20:17  geconomos
// Added annotation component image.
//
// Revision 1.7  2003/03/04 01:43:05  michael
// renaming of methods removing the double occurence of "Annotation", e.g.
// vxAnnotations::GetActiveAnnotation() --> vxAnnotations::GetActive()
//
// Revision 1.6  2003/02/28 05:44:26  michael
// fixed issue #2860
//
// Revision 1.5  2003/02/22 00:22:56  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.4  2003/02/13 17:25:15  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.3.2.1  2003/02/12 22:49:48  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.3  2003/02/07 01:03:53  michael
// added annotation for components
//
// Revision 1.2  2003/02/07 00:29:59  michael
// added annotation for components
//
// Revision 1.1  2003/02/06 14:29:51  michael
// added inital version of component annotation
//
// Revision 1.11  2003/01/20 19:37:42  michael
// added ellipse annotation
//
// Revision 1.10  2003/01/19 21:07:08  michael
// added box annotation
//
// Revision 1.9  2003/01/17 23:22:58  michael
// fixed basic annotation, minor issue in angle still to be done
//
// Revision 1.8  2003/01/17 00:33:22  michael
// further iteration on new annotations. Text now shows up and there is a utility
// in the viewerType that allows to pass in the OpenGL parameters in case the
// current one should not be used, e.g. as in render text of annotations. Also extended
// the glFontGeorge class to return width and height of the given string
//
// Revision 1.7  2003/01/15 23:53:49  michael
// added annotation manipulation and handle highlighting
//
// Revision 1.6  2003/01/15 16:30:55  michael
// added defines for FILE_VERSION
//
// Revision 1.5  2003/01/14 23:36:27  michael
// furter adjustments to support point, arrow, distance and angle
//
// Revision 1.4  2003/01/13 23:20:33  michael
// made first annotation to work in new structure (vxAnnotations)
//
// Revision 1.3  2003/01/13 14:43:35  geconomos
// Did the following:
//   (1) Added mouse cursor handling.
//   (2) Set the active annotation in AddAnnotation().
//
// Revision 1.2  2003/01/09 14:55:44  michael
// some add-ons to the new annotation structure
//
// Revision 1.1  2003/01/06 17:12:18  michael
// Added initial version of annotations
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxAnnotationComponent.C,v 1.13 2007/06/28 19:33:55 jmeade Exp $
// $Id: vxAnnotationComponent.C,v 1.13 2007/06/28 19:33:55 jmeade Exp $
