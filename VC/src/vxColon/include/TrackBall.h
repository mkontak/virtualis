// $Id: TrackBall.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File : OverviewTrackBall.h
// Description: 
// A class using 2D interactive device (mouse) to simulate 3D controls 
// just like a trackball. These controls include rotate, translate, scale
// Created: June 18, 2000 (First day under control: July 18, 2000)
// Author: Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.


#ifndef TrackBall_h
#define TrackBall_h

// includes
#include "ReaderLib.h" // defines Sec_ReaderLib
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "Point2D.h"

namespace ReaderLib
{
  class TrackBall : Sec_ReaderLib
  {
  public:
    /// these status match the orientation in Study.
    enum StudyOrientationEnum { DEFAULT = 0, SUPINE, PRONE, PRIMARY, SECONDARY, SWAP, NON_ROTATE = 10 };
    enum OrientationOfPlaneEnum { XYFace=0, XYBack, YZFace, YZBack, ZXFace, ZXBack };

    enum EventsEnum
    {
      EVENT_MODIFIED = 0x00000001,
    };

  // member functions
  public:
    
    // Constructor/Destructor/Reset/Initial functions
    // ----------------------------------------------

    /// Constructor
    TrackBall();

    /// Destructor
    virtual ~TrackBall();

    /// Copy Constructor
    TrackBall(const TrackBall & otherTB);

    /// Assigment operator
    const TrackBall& operator=(const TrackBall & otherTB);
    
    /// Reset.
    void Reset();

    /// Init
    void Init();

    /// Initialize (S&P).
    void Init(const StudyOrientationEnum eOrientation);

    /// Initialize (based on orientation)
    void Init(const OrientationOfPlaneEnum eOrientation, const int4 iRotateDegree=0);

    /// Partial Reset (Supine/Prone application).
    void PartialReset();

    /// virtual trackball calculation.
    void InitCalc(const int4 iWidth, const int4 iHeight);

    /// Copy a matrix to m_mCurrMatrix[][];
    void CopyMatrix(const Matrix<float4> & mMatrix);

  public:
    // Get/Set Access functions
    // -------------------------
    /// set up start point for rotate calculation.
    inline void ResetZoomFactor();

    /// set up start point for rotate calculation.
    inline void SetRotateStartPoint(const Point2D<int4> & vPt2D);
    
    /// set up current point for rotate calculation.
    inline void SetRotateCurrentPoint(const Point2D<int4> & vPt2D);
    
    /// set up start point for translate calculation.
    inline void SetMoveStartPoint(const Point2D<int4> & vPt2D);

    /// set up offset of a translate operation and
    /// set up new start point for the next translate.
    inline void SetMoveCurrentPoint(const Point2D<int4> & vPt2D);

    /// set start point (y) for a scale calculation.
    inline void StartZoom(const int4 iZoom);
    
    /// set scale factor.
    inline void SetZoom(const int4 iZoom);

    /// set default zoom factor.
    inline void SetDefaultZoomFactor(const float4 fZoomFactor);

    /// set zoom limits
    inline void SetZoomLimits( const float4 fMin, const float4 fMax);

    /// set scale factor.
    inline void MouseWheelZoom(const float4 iDelta);

    /// set zoom factor.
    inline void SetZoomFactor(const float4 fZoomFactor);
    
    /// retrive scale factor.
    inline float4 GetZoomFactor() const;

    /// get X transfer.
    inline int4 GetTransferX() const;

    /// get Y transfer.
    inline int4 GetTransferY() const;

    // set transfer offset.
    inline void SetTransfer(const Point2D<int4> & vPt2D);

    /// Set m_qCurrQuat.
    inline void SetCurrentQuat(const Quaternion<float4> & vQuat);

    /// Get m_qCurrQuat.
    inline const Quaternion<float4> & GetCurrentQuat() const;

    /// Get m_mCurrMatrix.
    inline const Matrix<float4> & GetCurrentMatrix() const;

    /// Get m_mCurrMatrix.
    inline Matrix<float4> * GetCurrentMatrixPtr();

    /// Set m_mCurrMatrix.
    inline void SetCurrentMatrix(Matrix<float4> & mOther);
    
    // Primary functions of the Trackball Class
    // -----------------------------------------
    /// build rotate matrix: to a matrix.
    void BuildRotMatrix(float4 fMatrix[][4]);
    
    /// build rotate matrix: to a Matrix.
    void BuildRotMatrix(Matrix<float4> & mMatrix);

    /// build rotate matrix: to m_mCurrMatrix.
    void BuildRotMatrix();

    /// accumulate the quatrad.
    void AddQuats();

    virtual void Modified(uint4 uFlags){};

  private:
    /// mapping ...
    float4 ProjectToSphere(const float4 fRadius, const float4 x, const float4 y);

    // Data members
    // --------------
  public:

  protected:

  private:  

    /// a matrix generated from m_qCurrQuat.
    Matrix<float4> m_mCurrMatrix;

    /// reserved quaternion.
    Quaternion<float4> m_vLastQuat;

    /// current quaternion from it form the rotate matrix.
    Quaternion<float4> m_vCurrQuat;

    /// start point for rotate calculateion.
    Point2D<int4> m_vRotStartPt;

    /// current point for rotate calculation.
    Point2D<int4> m_vCurrPt;
    
    /// start point for move calculation.
    Point2D<int4> m_vMvStartPt;

    /// offset in move calculation.
    Point2D<int4> m_vTransferPt;
    
    /// scale point for scale calculation, and zoom factor.
    int4 m_iZoomStart;
    float m_fZoomFactor;
    float4 m_fDefaultZoomFactor;

    /// limits of zoom factors
    float4 m_fMinFactor, m_fMaxFactor;

  }; // end class TrackBall

  extern TrackBall defaultTrackball;

  #include "TrackBall.inl"

} // namespace ReaderLib

# endif // TrackBall_h


// $Log: TrackBall.h,v $
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.2  2002/07/03 03:23:21  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 1.1  2002/05/02 17:00:31  suzi
// Added TrackBall (only colon needs it).
//
// Revision 3.15  2002/03/11 19:01:38  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.14  2002/03/07 22:31:08  dmitry
// Modified to fit new architecture.
//
// Revision 3.13  2002/02/05 22:15:51  binli
// added new member: default zoom factor
// (which can be changed based on volume size.)
//
// Revision 3.12.2.1  2002/02/11 15:13:35  binli
// issue 1699 in branch: default zoom factor is variable according to volume size.
//
// Revision 3.12  2002/01/29 20:09:59  michael
// comments, restructuring, and misc
//
// Revision 3.11  2002/01/17 16:25:12  binli
// removed unused variable.
//
// Revision 3.10  2001/12/28 21:57:52  ingmar
// added Sec_EnvElements to structure doxygen documentation
//
// Revision 3.9  2001/12/18 17:18:50  ingmar
// added class Sec_75_rendererUtils to structure the doxygen documentation
//
// Revision 3.8  2001/12/03 18:45:19  jmeade
// Init function overloaded (instead of renamed) for easier access and backward compatibility
//
// Revision 3.7  2001/12/03 18:42:59  jmeade
// Init function overloaded (instead of renamed) for easier access and backward compatibility
//
// Revision 3.6  2001/12/03 16:31:32  binli
// adjustment of initial function: based on study orientation/face orientation
//
// Revision 3.5  2001/11/21 23:54:27  binli
// initialized trackball based on direction of each face.
//
// Revision 3.4  2001/11/16 04:10:05  soeren
// Fixed typo limmiiits to limits
//
// Revision 3.3  2001/11/16 01:45:57  geconomos
// Updated events
//
// Revision 3.2  2001/11/13 18:00:34  dmitry
// Events added and implemented.
//
// Revision 3.1  2001/11/08 22:23:49  geconomos
// Started new event processing (Have fun Dmitry!)
//
// Revision 3.0  2001/10/14 23:02:32  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:50:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:44   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 23:32:19  ingmar
// new directory structure
//
// Revision 1.30  2001/08/08 19:57:22  liwei
// Arguments to MouseWheelZoom() changed from int4 to float4.
//
// Revision 1.29  2001/08/01 16:21:45  liwei
// Added FireOnChanged() and OnChanged().
//
// Revision 1.28  2001/05/24 20:39:04  binli
// moved to vxBase/9_render
//
// Revision 1.27  2001/05/23 19:16:38  liwei
// zoomFactor changed from int4 to float4
//
// Revision 1.26  2001/05/23 16:24:22  binli
// code conventions
//
// Revision 1.25  2001/05/22 17:23:10  liwei
// Allow user configurable default zoom factors
//
// Revision 1.24  2001/05/21 20:21:43  liwei
// TrackBall::Init(void) added
//
// Revision 1.23  2001/04/05 20:43:27  binli
// private m_mCurrMatrix
//
// Revision 1.22  2001/04/05 16:27:07  binli
// variable name converntion/made m_qCurrQuat private
//
// Revision 1.21  2001/04/03 22:09:00  binli
// changed (x,y) to Point2D.
//
// Revision 1.20  2001/04/03 21:13:14  binli
// 1st code walkthrough
//
// Revision 1.19  2001/03/30 22:13:06  binli
// int-> int4
// float->float4
//
// Revision 1.18  2001/03/30 21:21:11  binli
// using template Quaternion
//
// Revision 1.17  2001/03/27 21:33:21  binli
// continued code walkthrough.
//
// Revision 1.16  2001/03/27 15:12:29  binli
// reflected Typedef.h
//
// Revision 1.15  2001/03/26 15:01:23  binli
// changed some variable name to make them more meanless.
//
// Revision 1.14  2001/03/16 21:39:16  binli
// renamed some variables according to new code standard
//
// Revision 1.13  2001/01/26 19:58:29  binli
// defect 000188: default Overview position: added Init()
//
// Revision 1.12  2001/01/24 08:15:14  geconomos
// Added SetTransfer() And SetZoomFactor()
//
// Revision 1.11  2001/01/15 18:03:10  geconomos
// Added assignment operator
//
// Revision 1.10  2000/12/28 19:59:04  binli
// used Matrix Class defined in 3_math.
//
// Revision 1.9  2000/11/22 20:23:21  binli
// fixed reset of trackball
//
// Revision 1.8  2000/11/17 21:27:13  binli
// adjusted the rendering order of colon and other stuffs in translucent mode.
//
// Revision 1.7  2000/11/14 18:30:57  binli
// followed changes in COverview.
//
// Revision 1.6  2000/11/10 22:35:52  binli
// worked on both Supine/Prone overview rendering. In global variable 'rdrGlobal',
// two local trackballs are added. Some changes are made in TrackBall and OverView.
// Followed the change in OverView, VerifyView, EndoVisView, StandardView are
// changed a little.
//
// Revision 1.5  2000/09/26 02:58:14  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.4.2.1  2000/09/25 18:56:37  kevin
// Made Static Libraries
//
// Revision 1.4  2000/09/22 14:25:02  binli
// Added some comments
//
// Revision 1.3  2000/09/21 21:19:03  binli
// removed vector operation originally defined in Trackball class.
// The vector operations are replaced by standard Vector defined in libMath.lib
//
// Revision 1.2  2000/09/21 18:39:55  binli
// Quaternion was import
//
// Revision 1.1  2000/09/18 21:11:55  binli
// The source file has been renamed
//
// Revision 1.3  2000/09/18 18:19:23  binli
// Start Quaternion stuff
//
// Revision 1.2  2000/09/13 14:58:55  binli
// fitted to the Viatronix C++ coding conventions and standards
//
// Revision 1.1.1.1  2000/09/11 00:33:24  ingmar
// unit tests complete
//
// 
// 19    8/30/00 6:34p Jeff
// Frame box and translucent modes need not be stored in TrackBall class
// (which stores position and orientation of the overview, i.e. where to
// draw it, not how (-:)
// 
// 18    8/28/00 12:29p Antonio
// updated header and footer
//
//*****************  Version 17  *****************
//User: Binli           Date:  8/22/00  Time:  9:50a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  add comments
//
//*****************  Version 16  *****************
//User: Binli           Date:  8/18/00  Time:  4:33p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 15  *****************
//User: Binli           Date:  8/18/00  Time:  4:13p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 14  *****************
//User: Binli           Date:  8/18/00  Time:  4:12p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  add comments
//
//*****************  Version 13  *****************
//User: Binli           Date:  8/18/00  Time:  4:09p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Add comments
//
//*****************  Version 12  *****************
//User: Binli           Date:  8/18/00  Time:  3:38p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 11  *****************
//User: Binli           Date:  8/18/00  Time:  3:07p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 10  *****************
//User: Binli           Date:  8/17/00  Time:  4:55p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 9  *****************
//User: Jeff            Date:  8/16/00  Time: 10:13a
//Checked in $/ViatronMDI/src/ReaderLib
//Comment:
//  
//
//*****************  Version 8  *****************
//User: Binli           Date:  8/14/00  Time:  3:40p
//Checked in $/Viatron1000/src/Viatron
//
//*****************  Version 7  *****************
//User: Frank           Date:  8/10/00  Time:  4:37p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Removed compile warnings
//
//*****************  Version 6  *****************
//User: Binli           Date:  8/07/00  Time:  2:28p
//Checked in $/Viatron1000/src/Viatron
//
//*****************  Version 5  *****************
//User: Kevin           Date:  7/26/00  Time:  5:27p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Fixed compile errors
//
//*****************  Version 4  *****************
//User: Binli           Date:  7/26/00  Time:  2:00p
//Checked in $/Viatron1000/src/Viatron
//
//*****************  Version 3  *****************
//User: Binli           Date:  7/26/00  Time: 10:48a
//Checked in $/Viatron1000/src/Viatron
//
//*****************  Version 2  *****************
//User: Binli           Date:  7/26/00  Time:  9:29a
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  
//
//*****************  Version 1  *****************
//User: Binli           Date:  7/25/00  Time:  3:45p
//Created OverviewTrackBall.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/TrackBall.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $
// $Id: TrackBall.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $
