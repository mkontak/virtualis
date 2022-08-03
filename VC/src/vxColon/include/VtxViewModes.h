// $Id: VtxViewModes.h,v 1.2.2.1 2010/03/20 12:36:04 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File: VtxViewModes.h
// Owner: Jeff Meade   (Moved from ViatronStudy.h in April 10, 2001 by Bin Li)
//
// Complete history on bottom of file

#if !defined (VtxViewModes_h)
#define VtxViewModes_h

/**
 * Various view modes for v3D Application (i.e. Colon)
 */
namespace ViewModes
{
  enum VIEWMODES
  {
    NUMBITS = 31, LOWBITS = 26, HIGHBITS = NUMBITS - LOWBITS,

    LOWHIGHBREAK = (1 << LOWBITS),    /// First high-bit value

    LOWMODES = ((1 << LOWBITS) - 1),                  // 00...011...1
    HIGHMODES = (((1 << HIGHBITS) - 1) << LOWBITS),   // 11...100...0

    /// Independent modes: (long) up to LOWBITS bits (#modes = LOWBITS)
    MISSED_REGIONS     = 1,
    CENTERLINE         = 1 << 1,
    CROSS_SECTION      = 1 << 2,
    AXIAL              = 1 << 3,
    SAGITTAL           = 1 << 4,
    CORONAL            = 1 << 5,
    TRANSLUCENT        = 1 << 6,
    DRAW_DISTANCE      = 1 << 7,
    BOOK_MARK          = 1 << 8,
    DISSECTIONAL       = 1 << 9,

    SEGMENTPOLYP_VALID = 1 << 17,
    ANNOTATION_VALID   = 1 << 18,
    AUTOFLY_VALID      = 1 << 19,
    AUTOFLY2D_VALID    = 1 << 20,
    BIOPSY_VALID       = 1 << 21,
    LINEMEASURE_VALID  = 1 << 22,
    LIGHTING_VALID     = 1 << 23,
    PSEUDOCOLOR_VALID  = 1 << 24,
    ROIMEASURE_VALID   = 1 << 25,

    /// Mutually-exclusive modes: (long) up to HIGHBITS bits (#modes = 2^HIGHBITS)
    ANNOTATION      = 1 << LOWBITS,
    AUTOFLY         = 2 << LOWBITS,
    AUTOFLY2D       = 3 << LOWBITS,
    BIOPSY          = 4 << LOWBITS,
    LIGHTING        = 5 << LOWBITS,
    LINEMEASURE     = 6 << LOWBITS,
    ROIMEASURE      = 7 << LOWBITS,
    SEGMENTPOLYP    = 8 << LOWBITS,
    WINDOWLEVEL     = 9 << LOWBITS,

    /// Abbreviations
    ORTHOGONAL = AXIAL + SAGITTAL + CORONAL,
    SLICE = CROSS_SECTION + ORTHOGONAL + DISSECTIONAL,
  };
};

typedef uint8 ViewModesValue;


/**
 * Class that handles storing/setting various independent and mutually-exclusive VIEWMODES
 *
 * Note: Although there is a valid modes member variable, there is no
 *       validation of current modes (e.g. when calling Toggle()) in the
 *       class. This valid modes member is provided merely for convenience.
 */
class VtxViewModes 
{
  public:
    /// Constructor using ViewModesValue (ulong)
    inline VtxViewModes(const ViewModesValue &uModes, const ViewModesValue &uValid = 0);
    /// Constructor using VIEWMODES enum
    inline VtxViewModes(const ViewModes::VIEWMODES &uModes = ViewModes::VIEWMODES(0),
                  const ViewModes::VIEWMODES &uValid = ViewModes::VIEWMODES(0));
    /// Copy constructor
    inline VtxViewModes(const VtxViewModes &vtx);
    /// Copy operator
    inline VtxViewModes &operator=(const VtxViewModes &vtx);

    /// Set modes using VIEWMODES enum
    inline void SetModes(const ViewModes::VIEWMODES &uModes = ViewModes::VIEWMODES(0),
                  const ViewModes::VIEWMODES &uValid = ViewModes::VIEWMODES(0));

    /// Type-conversion operator
    inline operator ViewModesValue() const;

    /// Single-mode toggle
    inline bool Toggle(const ViewModes::VIEWMODES &uValue);
    /// Toggles any set mutex mode off
    inline void ClearMutexModes();

    /// Mode query operator
    inline bool operator !=(const ViewModes::VIEWMODES &uValue) const;
    /// Mode query operator
    inline bool operator & (const ViewModes::VIEWMODES &uValue) const;
    /// Mode query operator
    inline bool operator ==(const ViewModes::VIEWMODES &uValue) const;
    /// Modes query operator
    inline bool operator !=(const VtxViewModes &modes) const;
    /// Modes query operator
    inline bool operator ==(const VtxViewModes &modes) const;
    /// Mode set operator
    inline uint8 operator &=(const ViewModes::VIEWMODES &uValue);
    /// Mode set operator
    virtual inline uint8 operator +=(const ViewModes::VIEWMODES &uValue);
    /// Mode UNset operator
    virtual inline uint8 operator -=(const ViewModes::VIEWMODES &uValue);

    /// Returns only the independent modes
    inline ViewModesValue GetIndependentModes() const;
    /// Returns only the mutually-exclusive mode
    inline ViewModesValue GetMutexModes() const;

    /// Set valid view modes
    inline void SetValid(const ViewModesValue &uValue);
    /// Get valid view modes
    inline ViewModesValue GetValid() const;

  private:
    /// Is value in the low-order (independent) or high-order (mutex) family
    inline bool IsIndependent(const ViewModes::VIEWMODES &uValue) const;

  private:
    /// Current modes
    ViewModesValue m_uMode;
    /// Valid modes
    ViewModesValue m_uValidModes;
};


/**
 * Prototype for callback function that notifies of mode change in VtxViewModesWithNotify
 */
typedef void (*OnViewModesChangedProc)(const ViewModesValue &uOld, const ViewModesValue &uNew);


/**
 * Class that will notify via a function call of any change to the modes
 */
template<OnViewModesChangedProc OnModeChangeProc>
class VtxViewModesWithNotify : public VtxViewModes
{
  public:
    /// Mode set operator
    virtual inline uint8 operator+=(const ViewModes::VIEWMODES &uValue)
    {
      ViewModesValue uOldValue = ViewModesValue(*this);
      ViewModesValue uReturn = VtxViewModes::operator+=(uValue);
      OnModeChangeProc(uOldValue, ViewModesValue(*this));
      return uReturn;
    }

    /// Mode UNset operator
    virtual inline uint8 operator-=(const ViewModes::VIEWMODES &uValue)
    {
      ViewModesValue uOldValue = ViewModesValue(*this);
      ViewModesValue uReturn = VtxViewModes::operator-=(uValue);
      OnModeChangeProc(uOldValue, ViewModesValue(*this));
      return uReturn;
    }
};

#include "VtxViewModes.inl"

#endif // VtxViewModes_h

// $Log: VtxViewModes.h,v $
// Revision 1.2.2.1  2010/03/20 12:36:04  dongqing
// added menu item for dissection plane display in overview
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.2  2006/12/22 19:49:15  jmeade
// Issue 5160: remove unused modes, ensure we stay within uint8 bounds.
//
// Revision 1.1  2005/08/05 12:54:58  geconomos
// moved from vx repository
//
// Revision 3.7.2.2  2004/09/29 19:50:19  jmeade
// Interaction for polyp segmentation.
//
// Revision 3.7.2.1  2003/02/12 00:44:12  jmeade
// Issue 2763: Global mode/valid option for pseudo-color view; Clarifying 2D flight mode values.
//
// Revision 3.7  2002/12/23 19:49:17  jmeade
// Cleanup, extending use of view modes class.
//
// Revision 3.6  2002/06/04 18:37:01  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.5.2.1  2002/05/23 16:49:53  jmeade
// Issue 2270:  2D annotations mode;  I finally figured out what in the class Visual Assist doesn't recognize!!!!  Yeah!!!
//
// Revision 3.5  2002/01/03 02:22:26  kevin
// Updated modes as part of cleanup related to new TTP work
//
// Revision 3.4  2001/12/20 14:59:48  ingmar
// added class Sec_03_utils to structure the doxygen documentation
//
// Revision 3.3  2001/11/12 21:13:12  binli
// added: SLICEFLY_VALID (for 2D auto-fly control)
//
// Revision 3.2  2001/11/08 20:43:59  binli
// 2D flight
//
// Revision 3.1  2001/10/17 21:33:07  jmeade
// Coding standards.
//
// Revision 3.0  2001/10/14 23:02:16  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:30:04   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:36   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:46   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:31  soeren
// renamed utils directory
//
// Revision 1.8  2001/06/01 15:34:12  jmeade
// typedef.h
//
// Revision 1.7  2001/05/15 17:49:39  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.6  2001/05/07 21:44:09  binli
// Added control to toggle display bookmarks in Overview
// (TrackRecord ID: 000375 & 000342 )
//
// Revision 1.5  2001/04/16 13:29:01  frank
// Something wasn't fully checked in...
//
// Revision 1.4  2001/04/11 18:40:32  dmitry
// Sec_1_utils inheritance added
//
// Revision 1.3  2001/04/11 11:23:33  frank
// Removed ulong, uint, ushort, uchar datatypes.
//
// Revision 1.2  2001/04/10 18:16:00  binli
// moved inline functions of class VtxViewModes from ViatronStudy to VtxViewModes.inl.
//
// Revision 1.1  2001/04/10 15:32:14  binlimoved namespace 'ViewModes' and class 'VtxViewModes' to 1-utils
// 
//
// 
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/VtxViewModes.h,v 1.2.2.1 2010/03/20 12:36:04 dongqing Exp $
// $Id: VtxViewModes.h,v 1.2.2.1 2010/03/20 12:36:04 dongqing Exp $
