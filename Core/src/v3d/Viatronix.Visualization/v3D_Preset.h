// $Id: v3D_Preset.h,v 1.9.8.2 2009/07/29 19:54:07 kchalupa Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:meissner@viatronix.com)

#pragma once


// includes
#include "v3D_Interfaces.h"
#include "v3D_Preset.h"
#include "vxVisualizationPreset.h"
#include "vxCamera.h"
#include <vector>


// namespaces
OPEN_VISUALIZATION_NS


// managed forward declarations
ref class Camera;
ref class VisualizationPreset;


/**
 * Represents a preset
 */
public ref class Preset : public IElement
{
// consts
public:

  /// index check for no visualization preset selection
  static const int4 VisualizationPresetNotSelected = -1;

// events
public:

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

/// construction
public:
  
  /// default constructor
  Preset();

  /// destructor
  virtual ~Preset();

  /// finalizer
  !Preset();

// member functions
public:

  /// gets a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pPreset ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer) {}

  /// disconnects an observer from the object
  virtual void Disconnect(System::IntPtr observer) {}

  /// serializes the preset object to xml
  System::String ^ ToXml();

  /// creates a new preset object from the specified xml string
  static Preset ^ FromXml(System::String ^ xml);

  /// creates a new preset object from the specified xml resource
  static Preset ^ FromResource(System::String ^ xmlResource);

  /// creates a new preset object from the specified xml file
  static Preset ^ FromFile(System::String ^ xmlFile);

  /// returns the 2D visualization preset
  Viatronix::Visualization::Camera ^ GetCamera( int iIndex );

  /// returns the 2D visualization preset
  Viatronix::Visualization::VisualizationPreset ^ GetVisualizationPreset2D( int iIndex );

  /// sets the 2D visualization preset
  void AddVisualizationPreset2D( Viatronix::Visualization::VisualizationPreset ^ mpPreset );

  /// returns the 3D visualization preset
  Viatronix::Visualization::VisualizationPreset ^ GetVisualizationPreset3D( int iIndex );

  /// sets the 2D visualization preset
  void AddVisualizationPreset3D( Viatronix::Visualization::VisualizationPreset ^ mpPreset );

  /// ...
  virtual System::String ^ ToString() override;

  /// returns name of camera (helper, should go with vxCameraPreset!!!!!!!!!!!!!!!!!
  System::String ^ GetNameOfCamera(int iIndex);

  /// returns number of cameras
  int GetNumberOfCameras() { return m_pPreset->GetCameraPresets().size(); }

  /// returns number of 2D visualization presets
  int GetNumberOfVisualizationPresets2D() { return m_pPreset->GetVisualizationPresets2D().size(); }

  /// returns number of 3D visualization presets
  int GetNumberOfVisualizationPresets3D() { return m_pPreset->GetVisualizationPresets3D().size(); }

  /// compare preset with another one. returns true if smaller, else false
  int CompareTo(Viatronix::Visualization::Preset ^ preset);

// properties
public:

  /// returns the name of the preset
  property System::String ^ Name
  {
    System::String ^ get() { return gcnew System::String( m_pPreset->GetName().c_str() ); }
   void set( System::String ^ mpsName ) { m_pPreset->SetName(ss(mpsName)); }
  } // Name
  
  /// returns the modality of the preset
  property System::String ^ Modality
  {
    System::String ^ get() { return gcnew System::String( m_pPreset->GetModality().c_str() ); }
    void set( System::String ^ mpsModality ) { m_pPreset->SetModality(ss(mpsModality)); }
  } // Modality

  /// returns the body part of the preset
  property System::String ^ BodyPart
  {
    System::String ^ get() { return gcnew System::String( m_pPreset->GetBodyPart().c_str() ); }
    void set( System::String ^ mpsBodyPart ) { m_pPreset->SetBodyPart(ss(mpsBodyPart)); }
  } // BodyPart

  /// returns the Id of the user "owning" this preset
  property int UserId
  {
    int get() { return m_pPreset->GetUserId(); }
    void set( int iUserId ) { m_pPreset->SetUserId(iUserId); }
  } // UserId

  /// returns the DICOM group the preset should be sensitive to
  property System::String ^ DicomGroup
  {
    System::String ^ get() { return gcnew System::String( m_pPreset->GetDicomGroup().c_str() ); }
    void set( System::String ^ sDicomGroup ) { m_pPreset->SetDicomGroup(ss(sDicomGroup)); }
  } // DicomGroup

  /// returns the DICOM field the preset should be sensitive to
  property System::String ^ DicomField
  {
    System::String ^ get() { return gcnew System::String( m_pPreset->GetDicomField().c_str() ); }
    void set( System::String ^ sDicomField ) { m_pPreset->SetDicomField(ss(sDicomField)); }
  } // DicomField

  /// returns the DICOM value the preset should be sensitive to
  property System::String ^ DicomValue
  {
    System::String ^ get() { return gcnew System::String( m_pPreset->GetDicomValue().c_str() ); }
    void set( System::String ^ sDicomValue ) { m_pPreset->SetDicomValue(ss(sDicomValue)); }
  } // DicomValue

  /// returns the Id of the user "owning" this preset
  property int InitialCameraPreset
  { 
    int get() { return m_pPreset->GetInitialCameraPreset(); }
    void set( int iInitialCameraPreset ) { m_pPreset->SetInitialCameraPreset(iInitialCameraPreset); }
  } // InitialCameraPreset

  /// returns the initial 2D visualization preset
  property int InitialVisualizationPreset2D
  {
    int get() { return m_pPreset->GetInitialVisualizationPreset2D(); }
    void set( int iInitialVisualizationPreset ) { m_pPreset->SetInitialVisualizationPreset2D(iInitialVisualizationPreset); }
  } // InitialVisualizationPreset2D

  /// returns the initial 3D visualization preset
  property int InitialVisualizationPreset3D
  {
    int get() { return m_pPreset->GetInitialVisualizationPreset3D(); }
    void set( int iInitialVisualizationPreset3D ) { m_pPreset->SetInitialVisualizationPreset3D(iInitialVisualizationPreset3D); }
  } // InitialVisualizationPreset3D

  /// returns the initial 2D visualization preset
  property int4 SelectedVisualizationPreset2D
  {
    int4 get() { return m_pPreset->GetSelectedVisualizationPreset2D(); }
    void set( int iSelectedVisualizationPreset ) { m_pPreset->SetSelectedVisualizationPreset2D(iSelectedVisualizationPreset); }
  } // SelectedVisualizationPreset2D

  /// returns the initial 2D visualization preset
  property int4 SelectedVisualizationPreset3D
  {
    int4 get() { return m_pPreset->GetSelectedVisualizationPreset3D(); }
    void set( int iSelectedVisualizationPreset ) { m_pPreset->SetSelectedVisualizationPreset3D(iSelectedVisualizationPreset); }
  } // SelectedVisualizationPreset3D

// member variables
private:

  /// vxPreset
  vxShareableObject<vxPreset> * m_pPreset;

}; // class Preset


CLOSE_VISUALIZATION_NS


// Revision History:
// $Log: v3D_Preset.h,v $
// Revision 1.9.8.2  2009/07/29 19:54:07  kchalupa
// Code Walkthrough
//
// Revision 1.9.8.1  2009/03/27 16:31:59  gdavidson
// Added AddVisualizationPreset2D and AddVisualizationPreset3D methods
//
// Revision 1.9  2006/12/21 15:05:22  gdavidson
// Issue #5189 & 5106: Added SelectedVisualizationPreset2D and SelectedVisualizationPreset3D
//
// Revision 1.8  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.7  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.6.2.2  2005/02/17 21:00:58  geconomos
// code cleanup
//
// Revision 1.6.2.1  2005/02/10 20:57:24  michael
// Introduced vxCameraPreset with BoundingBox, name (to be displayed in GUI),
// and new Preset xml version (2.1). Converted presets, except for PET and XA
// ones. Added V3D Utils function and added a call to it to adjust the camera
// relative to the bounding box of the (camera) preset once it is assigned from
// the preset to the environment camera object (CopyFrom()).
//
// Revision 1.6  2005/01/07 16:53:25  gdavidson
// Modified preset implementation.
//
// Revision 1.5  2004/10/04 15:10:59  frank
// improved the extended elements modified interface
//
// Revision 1.4  2004/05/17 19:07:48  frank
// removed dead code
//
// Revision 1.3  2004/04/26 19:02:50  michael
// made vxPreset/V3D Preset a first class citizen of the environment
//
// Revision 1.2  2004/04/14 18:48:53  frank
// Extended presets to set the rendering mode and other visualization settings
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.19  2003/06/03 12:25:06  michael
// added Angio (XA) presets for Shimadzu that are sensitive to a specific DICOM entry
//
// Revision 1.18  2003/05/16 13:11:05  frank
// formatting
//
// Revision 1.17  2003/05/13 13:28:46  michael
// code review
//
// Revision 1.16  2003/05/12 19:10:54  michael
// code review
//
// Revision 1.15  2003/03/31 14:44:49  michael
// took out redundant code
//
// Revision 1.14  2003/03/31 14:40:26  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.13  2003/03/30 21:01:11  michael
// extended presets to have 2D and 3D presets
//
// Revision 1.12  2003/03/07 20:35:30  geconomos
// Added InitialColormap2D to preset class.
//
// Revision 1.11  2002/11/11 19:19:07  geconomos
// Reworked serialization.
//
// Revision 1.10  2002/11/06 23:31:53  michael
// added compare method for preset: CompareTo()
//
// Revision 1.9  2002/10/24 20:16:21  michael
// added initial camera and colormap indices
//
// Revision 1.8  2002/10/24 01:32:45  michael
// added ToString method
//
// Revision 1.7  2002/10/22 15:25:20  michael
// further progress on presets: Can now set colormap & camera but it does not
// yet get reflected in vxBase ... somehow. :-(
//
// Revision 1.6  2002/10/21 21:40:00  geconomos
// added ToString() override to return preset name.
//
// Revision 1.5  2002/10/19 20:52:41  michael
// added modality and organ string
//
// Revision 1.4  2002/10/18 17:22:22  geconomos
// Removed debugging code.
//
// Revision 1.3  2002/10/17 15:28:49  geconomos
// Clean up and worked on FromString() static method.
//
// Revision 1.2  2002/10/16 23:14:05  michael
// added method to initialize from a XML document passed in as string
//
// Revision 1.1  2002/10/16 22:19:00  michael
// added v3D_Preset for the administration of the presets
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Preset.h,v 1.9.8.2 2009/07/29 19:54:07 kchalupa Exp $
// $Id: v3D_Preset.h,v 1.9.8.2 2009/07/29 19:54:07 kchalupa Exp $
