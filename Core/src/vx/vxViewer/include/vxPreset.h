// $Id: vxPreset.h,v 1.6.8.1 2009/03/27 16:31:59 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)

#ifndef vxPreset_h
#define vxPreset_h


// includes
#include <vector>
#include "vxCameraPreset.h"


// forward declarations
class vxColormap;
class vxVisualizationPreset;


/**
 * This class contains all the data for a preset including
 * colormap presets, camera presets, body part, etc.
 */
class VX_VIEWER_DLL vxPreset
{
// member functions
public:

  /// constructor
  vxPreset();

  /// copy constructor
  vxPreset(const vxPreset & other);

  /// destructor
  ~vxPreset();

  /// assignment operator
  const vxPreset & operator =(const vxPreset & other);

  /// get name
  const std::string & GetName() const { return m_sName; }

  /// set name
  void SetName(const std::string & name) { m_sName = name; }

  /// get modality
  const std::string & GetModality() const { return m_sModality; }

  /// set modality
  void SetModality(const std::string & sModality) { m_sModality = sModality; }

  /// get body part
  const std::string & GetBodyPart() const { return m_sBodyPart; }

  /// set body part
  void SetBodyPart(const std::string & sBodyPart) { m_sBodyPart = sBodyPart; }

  /// get user id
  const int4 & GetUserId() const { return m_iUserId; }

  /// set user id
  void SetUserId(const int4 iUserId) { m_iUserId = iUserId; }

  /// get DICOM group
  const std::string & GetDicomGroup() const { return m_sDicomGroup; }

  /// set DICOM group
  void SetDicomGroup(const std::string & sDicomGroup) { m_sDicomGroup = sDicomGroup; }

  /// get DICOM field
  const std::string & GetDicomField() const { return m_sDicomField; }

  /// set DICOM field
  void SetDicomField(const std::string & sDicomField) { m_sDicomField = sDicomField; }

  /// get DICOM value
  const std::string & GetDicomValue() const { return m_sDicomValue; }

  /// set DICOM value
  void SetDicomValue(const std::string & sDicomValue) { m_sDicomValue = sDicomValue; }

  /// get initial camera preset
  const int4 & GetInitialCameraPreset() const { return m_iInitialCameraPreset; }

  /// set initial camera
  void SetInitialCameraPreset(const int4 iInitialCameraPreset) { m_iInitialCameraPreset = iInitialCameraPreset; }

  /// get initial visualization preset 2D
  const int4 & GetInitialVisualizationPreset2D() const { return m_iInitialVisualizationPreset2D; }

  /// set initial visualization preset 2D
  void SetInitialVisualizationPreset2D(const int4 iInitialVisualizationPreset2D) { m_iInitialVisualizationPreset2D = iInitialVisualizationPreset2D; }

  /// get initial visualization preset 3D
  const int4 & GetInitialVisualizationPreset3D() const { return m_iInitialVisualizationPreset3D; }

  /// set initial visualization preset 3D
  void SetInitialVisualizationPreset3D(const int4 iInitialVisualizationPreset3D) { m_iInitialVisualizationPreset3D = iInitialVisualizationPreset3D; }

  /// get selected visualization preset 2D
  const int4 & GetSelectedVisualizationPreset2D() const { return m_iSelectedVisualizationPreset2D; }

  /// set selected visualization preset 2D
  void SetSelectedVisualizationPreset2D(const int4 iSelectedVisualizationPreset2D) { m_iSelectedVisualizationPreset2D = iSelectedVisualizationPreset2D; }

  /// get selected visualization preset 3D
  const int4 & GetSelectedVisualizationPreset3D() const { return m_iSelectedVisualizationPreset3D; }

  /// set selected visualization preset 3D
  void SetSelectedVisualizationPreset3D(const int4 iSelectedVisualizationPreset3D) { m_iSelectedVisualizationPreset3D = iSelectedVisualizationPreset3D; }

  /// get camera presets
  std::vector<vxCameraPreset> GetCameraPresets() const { return m_vpCameraPresets; }

  /// set camera presets
  void SetCameraPresets(const std::vector<vxCameraPreset> & vCameraPresets) { m_vpCameraPresets = vCameraPresets; }

  /// get visualization preset presets array for 2D
  const std::vector<const vxVisualizationPreset *> & GetVisualizationPresets2D() const { return m_vpVisualizationPresets2D; }

  /// set visualization preset presets array for 2D
  void SetVisualizationPresets2D(const std::vector<const vxVisualizationPreset *> & vVisualizationPresets2D) { m_vpVisualizationPresets2D = vVisualizationPresets2D; }

  /// add visualization preset to 2D array
  void AddVisualizationPresets2D( vxVisualizationPreset & vVisualizationPresets2D ) { m_vpVisualizationPresets2D.push_back( &vVisualizationPresets2D ); }

  /// get visualization preset presets array for 3D
  const std::vector<const vxVisualizationPreset *> & GetVisualizationPresets3D() const { return m_vpVisualizationPresets3D; }

  /// set visualization preset presets array for 3D
  void SetVisualizationPresets3D(const std::vector<const vxVisualizationPreset *> & vVisualizationPresets3D) { m_vpVisualizationPresets3D = vVisualizationPresets3D; }

  /// add visualization preset to 3D array
  void AddVisualizationPresets3D( vxVisualizationPreset & vVisualizationPresets2D ) { m_vpVisualizationPresets3D.push_back( &vVisualizationPresets2D ); }



  // temporary HACK function
  Box<float4> GetBoundingBox()
  {
    if (m_vpCameraPresets.size() > 0)
      return m_vpCameraPresets[0].GetBoundingBox();
    else
    {
      return Box<float4>(0,0,0,512,512,512);
    }
  }

// data members
private:

  /// name
  std::string m_sName;

  /// modality
  std::string m_sModality;

  /// body part
  std::string m_sBodyPart;

  /// user/owner of this preset
  int4 m_iUserId;

  /// DICOM group to be sensitive to
  std::string m_sDicomGroup;

  /// DICOM field to be sensitive to
  std::string m_sDicomField;

  /// DICOM value to be sensitive to
  std::string m_sDicomValue;

  /// initial camera to start with
  int4 m_iInitialCameraPreset;

  /// initial visualization preset to start with for 2D
  int4 m_iInitialVisualizationPreset2D;

  /// initial visualization preset to start with for 3D
  int4 m_iInitialVisualizationPreset3D;

  /// selected visualization preset for 2D
  int4 m_iSelectedVisualizationPreset2D;

  /// selected visualization preset for 3D
  int4 m_iSelectedVisualizationPreset3D;

  /// camera presets
  std::vector<vxCameraPreset> m_vpCameraPresets;

  /// visualization preset presets array for 2D
  std::vector<const vxVisualizationPreset *> m_vpVisualizationPresets2D;

  /// visualization preset presets array for 3D
  std::vector<const vxVisualizationPreset *> m_vpVisualizationPresets3D;

}; // class vxPreset


#endif // vxPreset_h


// Revision History:
// $Log: vxPreset.h,v $
// Revision 1.6.8.1  2009/03/27 16:31:59  gdavidson
// Added AddVisualizationPreset2D and AddVisualizationPreset3D methods
//
// Revision 1.6  2006/12/21 15:06:14  gdavidson
// Issue #5189 & 5106: Added SelectedVisualizationPreset2D and SelectedVisualizationPreset3D
//
// Revision 1.5  2005/03/08 19:09:24  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.4.6.1  2005/02/10 20:57:24  michael
// Introduced vxCameraPreset with BoundingBox, name (to be displayed in GUI),
// and new Preset xml version (2.1). Converted presets, except for PET and XA
// ones. Added V3D Utils function and added a call to it to adjust the camera
// relative to the bounding box of the (camera) preset once it is assigned from
// the preset to the environment camera object (CopyFrom()).
//
// Revision 1.4  2004/04/26 19:02:50  michael
// made vxPreset/V3D Preset a first class citizen of the environment
//
// Revision 1.3  2004/04/06 13:31:38  frank
// added some const gets
//
// Revision 1.2  2004/04/05 15:59:07  frank
// working on expanding the presets to include rendering modes, etc.
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/08/11 11:30:22  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxPreset.h,v 1.6.8.1 2009/03/27 16:31:59 gdavidson Exp $
// $Id: vxPreset.h,v 1.6.8.1 2009/03/27 16:31:59 gdavidson Exp $
