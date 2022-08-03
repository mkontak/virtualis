// $Id: Submarine.h,v 1.5.2.4 2009/01/15 04:11:26 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File:  Submarine.h
// Owner: Frank Dachille (frank@viatronix.net)

// pragmas
#pragma once


/**
 * Description: A class to support guided navigation within the colon lumen.
 * It contains a physical simulation of an object with mass, rotation
 * mass, forward/reverse thrust, and pitch/heading rotation thrust.
 */


// includes
#include <deque>
#include <string>

#include "Vector.h"
#include "Normal.h"
#include "Point.h"
#include "Timer.h"
#include "SkeletonRender.h"
#include "Fields.h"
#include "Matrix.h"
#include "Camera.h"


// namespaces
namespace ReaderLib
{

// forward declarations
class Dataset;


class Submarine
{

// Enumerated Types
public:
  /// A way to tell the current direction of navigation.
  enum NavigationDirection {CECUM_TO_RECTUM = 0, RECTUM_TO_CECUM};

  /// transition mode from manual to automatic
  enum TransitionModeEnum { TRANSITION_CLOSEST, TRANSITION_SMOOTH, TRANSITION_OTHER };

  
// Member functions
public:

  // Constructor/Destructor/Copy/Assignment Functions
  // ------------------------------------------------

  /// Constructor.  Initializes the internal data structures.
  Submarine();

  /// Destructor. Releases all data structures.
  ~Submarine();

  /// Copy constructor.
  Submarine(const Submarine & rhs);

  /// Assignment operator.
  Submarine & operator =(const Submarine & rhs);


  // Get/Set Access Functions
  // ------------------------

  /// Get current view position in world coordinates.
  inline const Point<float4> & GetPositionWorld() const;

  /// Get current view position in volume coordinates.
  inline const Point<float4> & GetPositionVolume() const;

  /// sets position, orientation and up vector
  void SetCameraWorld(const Point<float4> & vpPosition, const Normal<float4> & vnOrientation, const Normal<float4> & vnUpDirection);

  /// Set current view position.
  void SetPositionWorld(const Point<float4> & vpPosition);

  /// Get current view direction.
  inline const Normal<float4> & GetOrientation() const;

  /// Set current view direction.
  inline void SetOrientation(const Normal<float4> & vnOrientation);

  /// Get current view up direction.
  inline const Normal<float4> & GetUpDirection() const;

  /// Set current view up direction.
  inline void SetUpDirection(const Normal<float4> & vnUpDirection);

  /// Get the status of the leveling feature.
  inline const bool & GetLeveling() const;

  /// Set the leveling feature.
  inline void SetLeveling(const bool bLeveling);

  /// Get the status of the potential field.
  inline const bool & GetPotential() const;

  /// Set the influence of the potential field.
  inline void SetPotential(const bool bPotential);

  /// Get the neighbor searching state
  inline const bool & GetNeighborSearching() const;

  /// Set the neighbor searching toggle state.
  inline void SetNeighborSearching(const bool bNeighborSearch);

  /// Set the amount of pan parallel to the image plane.
  inline void SetPanAmount(const Vector<float4> & vPanAmount);

  /// Control whether we are panning or rotating.
  inline void SetPanMode(const bool bPan);

  /// Retrieves the heading turn rate.
  inline const float4 & GetHeading() const;

  /// Changes the desired heading turn rate.
  inline void SetHeading(const float4 fHeading);

  /// Increases the desired heading turn rate.
  inline void AddHeading(const float4 fHeading);

  /// Retrieves the pitch turn rate.
  inline const float4 & GetPitch() const;

  /// Changes the desired pitch turn rate.
  inline void SetPitch(const float4 fPitch);

  /// Increases the desired pitch turn rate.
  inline void AddPitch(const float4 fPitch);

  /// Apply forces from the user to the simulation.
  inline void SetForces(const float4 fThrust, const float4 fHeading, const float4 fPitch); 

  /// Apply additional forces from the user to the simulation.
  inline void AddForces(const float4 fThrust, const float4 fHeading, const float4 fPitch);

  /// Get the maximum speed.
  inline const float4 & GetThrottle() const;

  /// Set the maximum speed as a percent of the default maximum speed.
  inline void SetThrottle(const float4 fPercent);

  /// Get the strength of the potential field.
  inline const float4 & GetPotentialStrength() const;

  /// Set the strength of the potential field.
  inline void SetPotentialStrength(const float4 fPercent);

  /// Get the strength of the boundary field.
  inline const bool & GetBoundary() const;

  /// Set the strength of the boundary field.
  inline void SetBoundary(const bool bBoundary);

  /// Get the future predicted position.
  inline const Point<float4> & GetPredictedPosition() const;

  /// Access to the currently selected flight direction.
  NavigationDirection GetFlightDirection() const;

  /// Set the current flight direction.
  void SetFlightDirection(const NavigationDirection eDir);

  /// Reverse the flight direction.
  void ToggleFlightDirection();

  /// Retrieve a unique identifier for each view position and orientation.
  inline const uint4 & GetViewIdentifier() const;

  /// Immediately set the parent pointer after creation.
  inline void SetParentDataset(Dataset * pDataset);

  /// Gets if in replay mode
  inline bool GetReplayMode() { return m_bReplayMode; }

  /// Gets the transition mode
  inline const TransitionModeEnum  GetTransitionMode() const;
  
  /// Sets the transition mode
  inline void SetTransitionMode( const TransitionModeEnum eMode );

  
  // Primary Functions of the Submarine Class
  // ----------------------------------------

  /// Reset all members.
  void Reset();

  /// Begin applying forces after they have been set.
  void StartFlying();

  /// Remove all forces and allow to coast.
  void StopFlying();

  /// Update the position. Execute one iteration of the physical simulation.
  void UpdatePosition();

  /// Switch to manual mode
  void SwitchToManualMode();

  /// Switch to automatic mode
  void SwitchToAutomaticMode();

  /// Set the viewpoint to a point along the skeleton.
  void SetViewSkeleton(const uint2 & uIndex);

  /// Set the viewpoint to a point along the skeleton: interpolation applied.
  void SetViewSkeleton(const float4 & fIndex, const bool bFlip);

  /// Set the viewpoint to a particular skeleton node.
  void SetViewSkeleton(const SkeletonNode & skelNode, const bool bFlip);

  /// Set the viewpoint according to 3 input parameters
  void SetView(const Point<float4> & vpPositionWorld, const Normal<float4> & vnViewDirection, const Normal<float4> & vnUp);

  /// Compute the preferred direction for flying.
  Vector<float4> GetPotentialGradient(const Point<float4> & vpPositionVolume) const;

  /// Step back n frames and set in replay mode.
  void ReplayPosition(const int4 iFrames);

  /// Reverse the current view direction and stop moving.
  void ReverseDirection();

  /// Enforce the orthogonality constraint (between the fwd and up dirs).
  void EnforceOrthogonality();

  /// Predict where we will be in a few seconds and look there.
  void OrientPotential(const float4 fIdealFrameRate, const float4 percentagePotential);

  /// Apply friction, leveling, boundary, and potential fields.
  const Vector<float4> GetFieldForce();

  /// Query the status of flying.
  bool IsFlying();

  /// Pan the viewpoint parallel to the image plane
  void PanPosition(const Point<float4> & vpNewPanLocation);

  /// Gracefully get out of replay mode.
  void ExitReplayMode();

  /// Come to a complete stop immediately.
  void FullStop();

  /// Constrain the submarine position to inside the volume.
  void ConstrainPosition();

  /// Get the nearest skeleton index to the view point
  uint2 GetClosestSkeletonIndex() const;

  /// Get the nearest skeleton index to the given view point
  uint2 GetClosestSkeletonIndex( Point<uint2> posVolumeUSpoint ) const;

  // Input/Output Functions
  // ----------------------

  /// Routine to initialize data structures.
  bool Initialize();

  /// Translate along the gradient field one timestep.
  const Vector<float4> GetPotentialField(const Point<float4> & vpPositionWorld, const float4 fPercentage) const;

protected:

  /// Resample the SLD.
  float4 GetResampledSld(const Point<float4> & vpPositionVolume) const;

  /// Translate away from the boundary.
  const Vector<float4> GetBoundaryField(const Point<float4> & vpPositionWorld, const float4 fPercentage) const;

  /// Ramp up and down the influence of the potential field over time.
  float4 GetPercentagePotential(const float4 fElapsedPotentialTime) const;

  /// Apply the leveling function to orient the view-up vector "up".
  void ApplyLeveling();

  /// Save the current viewpoint in a history buffer.
  void SaveHistoryFrame();

  /// Test the submarine to make sure it does not bump into the walls too often.
  void TestNavigation();

  /// Refresh the coordinate conversion matrices.
  void RefreshConversionMatrices();

  /// Populates the transition vector for the manual to automatic switch.
  void PopulateTransitionVector();


// Data Members
public:

  /// Ideal frame rate
  static float4 m_fIdealFrameRate;

  /// Turn rate to tune the feel of navigation around turns
  static float4 m_fTurnRateScale;

  /// Thrust scale to tune the feel of navigation around turns
  static float4 m_fThrustScale;

  /// Whether to use perceived speed (i.e. flight at same perceived speed irregardless of cylinder diameter)
  static bool m_bPerceivedSpeed;


// private members
private:

  /// A pointer to the Dataset which contains this object.
  Dataset * m_pDataset;

  /// The predicted viewpoint position in about 4 seconds.
  Point<float4> m_vpPredictedPosition;

  /// Search the neighbors instead of just using the gradient
  bool m_bSearchNeighbors;

  /// Toggles being in replay mode (mouse wheel replay).
  bool m_bReplayMode;

  /// How far we have stepped backward in replay mode.
  int4 m_iReplayFrame;

  /// A history of the most recent positions.
  std::deque< Camera > m_replayDeque;

  /// Time since the start or end of the potential field.
  Timer m_potentialTimer;

  /// Position of submarine in world coordinates
  Point<float4> m_vpPositionWorld;
  
  /// Position of submarine in world coordinates
  Point<float4> m_vpPositionVolume;

  /// Orientation of view up direction as a unit normal in world coordinates
  Normal<float4> m_vnUp;

  /// Orientation of submarine as a unit normal in world coordinates
  Normal<float4> m_vnOrientation;

  /// The last camera position during automatic flight along the centerline
  Point3D<float4> m_vpLastCenterlinePositionWorld;

  /// The last orientation of view up direction as a unit normal in world coordinates
  Normal<float4> m_vnLastCenterlineUp;

  /// The last centerline orientation of submarine as a unit normal in world coordinates
  Normal<float4> m_vnLastCenterlineOrientation;
  
  /// The current speed (magnitude of velocity).
  float4 m_fSpeed;

  /// Derivative of submarine direction in world coord in rad/s.
  float4 m_fHeadingTurnRate;

  /// Derivative of submarine direction in world coord in rad/s.
  float4 m_fPitchTurnRate;

  /// How close to the maximum speed the user wants to fly.
  float4 m_fThrottle;

  /// How close to the maximum speed the user wants to be drawn along the potential field.
  float4 m_fPotentialStrength;

  /// How long is the transition time between interactive and guided navigation in seconds.
  static const float4 m_fMaximumPotentialTime;

  /// User input of the submarine forward (1) and backward (-1).
  float4 m_fThrustInput;

  /// User input of the submarine heading turning rate.
  float4 m_fHeadingInput;

  /// User input of the submarine pitch turning rate.
  float4 m_fPitchInput;

  /// The amount to pan parallel to the image plane
  Vector<float4> m_vPan;

  /// Whether we are panning or rotating.
  bool m_bPan;

  /// Leveling helps to orient the view up vector toward a the global up dir.
  bool m_bLeveling;

  /// This helps to draw the submarine along the potential field.
  bool m_bPotential;

  /// This helps to keep the submarine inside the colon lumen.
  bool m_bBoundary;

  /// This timer records the time since start of acceleration (button press).
  Timer m_accelerationTimer;

  /// This timer records the time since the last frame.
  Timer m_frameTimer;

  /// Conversion matrices to convert between coordinate systems. (World to Volume)
  Matrix<float4> m_mWorldToVolume;

  /// Conversion matrices to convert between coordinate systems. (Volume to Volume)
  Matrix<float4> m_mVolumeToWorld;

  /// Internal counter to uniquely identify a view position and orientation.
  uint4 m_uViewIdentifier;

  /// Test mode for navigation outputs statistics.
  bool m_bTestStats;

  /// Transition mode from manual to automatic mode.
  TransitionModeEnum m_eTransitionMode;

  /// Information related to a smooth transition
  struct TransitionInfo
  {
    Point3Df m_vpPositionWorld;
    Normal3Df m_vnOrientation;
    Normal3Df m_vnUp;
  }; //  struct TransitionInfo
  
  /// Current transition info index.
  uint2 m_uTransitionIndex;
  
  /// Vector of Transitioninfo objects.
  std::vector< TransitionInfo > m_transitions;

  /// Indicates if the submarine is transition from manual to automatic mode.
  bool m_bTransitioning;

}; // end class Submarine


#include "Submarine.inl"

} // namespace ReaderLib


// $Log: Submarine.h,v $
// Revision 1.5.2.4  2009/01/15 04:11:26  jmeade
// convenience function to set camera coordinates and orientation.
//
// Revision 1.5.2.3  2007/08/17 16:09:40  geconomos
// Still reworking the transition
//
// Revision 1.5.2.2  2007/08/16 17:46:05  geconomos
// Took another stab at the transition from manual to automatic mode
//
// Revision 1.5.2.1  2007/05/23 16:13:53  geconomos
// reworked transitioning from manual to automatic mode of flight
//
// Revision 1.5  2006/05/02 13:32:38  frank
// added the capability to remove the smooth transition between automatic and manual flight modes
//
// Revision 1.4  2006/02/07 21:32:44  gdavidson
// Code Review
//
// Revision 1.3  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.2.2.1  2006/01/11 16:44:53  frank
// set const functions constant
//
// Revision 1.2  2005/10/27 15:17:43  vxconfig
// Issue #4481: Fixed looking around with arrow keys after rewinding.
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.16.2.3  2004/08/27 22:01:05  jmeade
// Issue 3538: Adjust perceived speed according to colon inner-radius.
//
// Revision 3.16.2.2  2004/07/17 03:06:28  jmeade
// Separate method for determining closest skeleton to any given volume coordinates.
//
// Revision 3.16.2.1  2003/05/22 19:26:24  kevin
// Fixed a problem I found where the gradient of the SLD volume was being
// normalized and I t could have been out in an area where the values were
// all 0, thus you got a (0,0,0) vector being normalized.
//
// Revision 3.16  2003/01/27 21:47:15  jmeade
// Method to set current flight direction.
//
// Revision 3.15  2002/09/27 19:02:13  jmeade
// Coding standards.
//
// Revision 3.14  2002/08/20 19:23:41  kevin
// 2D and 3D segment views.
//
// Revision 3.13  2002/04/26 15:57:30  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.12  2002/04/12 02:15:19  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.11  2002/03/28 18:47:51  jmeade
// Allow run-time setting of maximum flight speeds.
//
// Revision 3.10.2.1  2002/03/28 22:17:46  jmeade
// Allow run-time setting of maximum flight speeds.
//
// Revision 3.10  2001/12/21 23:58:41  jmeade
// Moved movie recording (for use in other views).
//
// Revision 3.9  2001/12/14 21:52:48  jmeade
// Error checking and reporting to user on movie creation; Removed movie CD burn; Comments.
//
// Revision 3.8  2001/12/12 20:06:59  jmeade
// Movies now save frames via current modes (translucent and flight currently);
// Use of Camera class for movies and history.
//
// Revision 3.7  2001/12/07 18:46:19  kevin
// Added "entertainment" bitmap and CD creation to movies
//
// Revision 3.6  2001/11/21 17:41:49  binli
// more accurate fucntion GetClosestSkeletonIndex().
//
// Revision 3.5  2001/11/21 17:26:05  jmeade
// Toggle movie record, is movie recording access method.
//
// Revision 3.4  2001/11/16 22:30:40  binli
// new speed control to 2D flight
//
// Revision 3.3  2001/11/14 02:29:18  kevin
// More CLenaup of SUpine/Prone and added in orientation
// according to  submarine "lookahaed" algrotihm
//
// Revision 3.2  2001/11/11 19:55:31  kevin
// Fixes for rdrGlobal.m_pStudy when renderin 2 endoscopic views in registration mode
//
// Revision 3.1  2001/10/25 13:20:39  kevin
// Added avi/mpeg saving
//
// Revision 3.0  2001/10/14 23:01:54  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:28   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:20   ingmar
// Initial revision.
// Revision 1.29  2001/05/16 19:25:03  frank
// Needed to remove keyword inline on function.
//
// Revision 1.28  2001/05/16 17:09:02  frank
// Slowly rolling forward...
//
// Revision 1.27  2001/05/16 16:32:18  frank
// Virtual rollback to fix jittering bug.
//
// Revision 1.23  2001/04/10 17:52:21  frank
// New coding conventions in Submarine class.
//
// Revision 1.22  2001/04/10 12:49:55  frank
// Removed revision history from top of file.
//
// Revision 1.21  2001/02/15 13:46:02  frank
// Constrained the viewpoint inside the volume to avoid crashes inside volume rendering code.
//
// Revision 1.20  2001/02/01 13:40:57  frank
// Revamped study initialization procedure.
//
// Revision 1.19  2001/02/01 12:55:32  frank
// Initialized parent pointer another way.
//
// Revision 1.18  2001/01/31 14:08:11  frank
// Added a parent pointer to retain the proper (supine/prone) study.
//
// Revision 1.17  2001/01/23 18:16:21  frank
// Saved a unique view identifier for view caching purposes.
//
// Revision 1.16  2001/01/23 13:21:55  frank
// Added a function to immediately stop all movement.
//
// Revision 1.15  2001/01/11 23:46:21  kevin
// Added navigation based missed patch viewing
//
// Revision 1.14  2001/01/05 15:35:23  frank
// Used SegmentList to contain navigation direction.
//
// Revision 1.13  2000/12/22 19:30:47  frank
// Changed the style of panning back to relative mode from absolute.
//
// Revision 1.12  2000/11/24 15:29:57  frank
// Added ExitReplayMode function
//
// Revision 1.11  2000/11/20 14:36:11  frank
// Added some access functions
//
// Revision 1.10  2000/11/17 18:27:47  frank
// Added Reset functions
//
// Revision 1.9  2000/11/17 16:12:26  frank
// Filtered out upper sld values
//
// Revision 1.8  2000/11/14 18:56:20  frank
// Changed study reference to pointer
//
// Revision 1.7  2000/10/12 21:06:20  kevin
// Added direcitonal
// flag to semgnet jumping
//
// Revision 1.6  2000/10/12 02:49:18  kevin
// Changes to store the current flight direction (REctum->Cecum) here
//
// Revision 1.5  2000/09/21 19:37:21  frank
// C++ Coding Standards
//
// Revision 1.4  2000/09/19 18:16:52  kevin
// Reversed one version due to errors that could
// not be fixed due to broken pipeline/volume utlity stuff.
// (This will be revisited once the other errors
// are fixed and we can work at all once again)
//
// Revision 1.2  2000/09/12 14:54:51  frank
// Changes for coding standard compliance
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 31    9/06/00 3:09p Frank
// Added pan functionality
// 
// 30    9/01/00 10:37a Frank
// Initial work on unwrapping the colon wall
// 
// 29    9/01/00 9:18a Frank
// Cosmetics
// 
// 28    8/28/00 12:44p Antonio
// updated header and footer
//
//*****************  Version 27  *****************
//User: Kevin           Date:  8/23/00  Time:  1:41a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Really, Really fixed otrthogonalPoint to work with viewpoint
//color displaying whether point matches the viewpoint
//
//*****************  Version 26  *****************
//User: Kevin           Date:  8/22/00  Time: 11:14p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed slice positioning to utilize "orthogonalPoint"
//
//*****************  Version 25  *****************
//User: Jeff            Date:  8/22/00  Time:  7:29p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Value access functions for current throttle and potential strength
//"const TYPE &" in function headers replaces "const TYPE"
//
//*****************  Version 24  *****************
//User: Kevin           Date:  8/20/00  Time: 10:20p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added choosing of flight direction
//
//*****************  Version 23  *****************
//User: Frank           Date:  8/18/00  Time:  2:02p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Prevented updates during all navigation
//
//*****************  Version 22  *****************
//User: Frank           Date:  8/18/00  Time:  1:33p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added possible neighbor searching to navigation
//
//*****************  Version 21  *****************
//User: Frank           Date:  8/10/00  Time:  4:21p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Re-worked the internals of the physics
//to better handle wall constaints.
//
//*****************  Version 20  *****************
//User: Frank           Date:  8/08/00  Time:  9:38a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Renamed a function for clarity
//
//*****************  Version 19  *****************
//User: Kevin           Date:  8/04/00  Time:  4:46p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added inside file testing function
//Added global constant
//
//*****************  Version 18  *****************
//User: Kevin           Date:  8/02/00  Time:  9:30a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Fixed submarine jumping probelm
//
//*****************  Version 17  *****************
//User: Frank           Date:  8/01/00  Time:  4:08p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added bidirectional potential fields
//
//*****************  Version 16  *****************
//User: Frank           Date:  7/31/00  Time:  3:59p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Explicitly enforced the orthogonality constraint every update
//
//*****************  Version 15  *****************
//User: Frank           Date:  7/27/00  Time: 11:23a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added a boundary field toggle
//
//*****************  Version 14  *****************
//User: Frank           Date:  7/25/00  Time:  2:30p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Enabled look-around in replay mode
//
//*****************  Version 13  *****************
//User: Frank           Date:  7/25/00  Time:  1:49p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Extended replay mode to allow orientation
//
//*****************  Version 12  *****************
//User: Frank           Date:  7/25/00  Time:  8:53a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Re-wrote for new volume classes and file format.
//Added colon boundary constraints.
//
//*****************  Version 11  *****************
//User: Frank           Date:  7/18/00  Time:  3:56p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Removed "using namespace *" from header files to prevent ambiguities
//
//*****************  Version 10  *****************
//User: Jeff            Date:  7/18/00  Time:  1:57p
//Branched at version 10
//Comment:
//  
//
//*****************  Version 9  *****************
//User: Jeff            Date:  7/18/00  Time:  1:43p
//Checked in $/v1k/src/ReaderLib
//Comment:
//  C++ comment lines needed in front of header version information
//
//*****************  Version 8  *****************
//User: Frank           Date:  7/18/00  Time:  1:23p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Implemented instant replay
//
//*****************  Version 7  *****************
//User: Frank           Date:  7/18/00  Time: 10:45a
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  virtual rollback
//
//*****************  Version 6  *****************
//User: Frank           Date:  7/18/00  Time: 10:35a
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  broken again?
//
//*****************  Version 5  *****************
//User: Frank           Date:  7/18/00  Time: 10:33a
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  re-fixed .h expansion
//
//*****************  Version 4  *****************
//User: Frank           Date:  7/18/00  Time: 10:31a
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  new time zone test
//
//*****************  Version 3  *****************
//User: Frank           Date:  7/17/00  Time: 10:00p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  testing history
//
//*****************  Version 2  *****************
//User: Frank           Date:  7/13/00  Time: 10:20p
//Checked in $/NewViatron1000/ReaderLib
//Comment:
//  Improved navigation feel - exponential instead of linear
//acceleration
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created Submarine.h
//Comment:
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Submarine.h,v 1.5.2.4 2009/01/15 04:11:26 jmeade Exp $
// $Id: Submarine.h,v 1.5.2.4 2009/01/15 04:11:26 jmeade Exp $
