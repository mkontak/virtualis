// $Id: vxDecider.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com
      
   
/**
 * Obsolete, please use DeciderHullOutside.
 * Note that they are 26-connected to the inside region.
 * Thus the Main iterator has to be regionGrow6, but the peek iterator has to be neighbor26.
 *
 * @param Iter          Type of the volume data iterator.
 * @param InRegionDecider Type of Decider for what is considered inside the region.
 * @param dataIter      Iterator suggestiong a new point on th ehull of the region.
 * @return true if the data iterator in deed points to a voxel on the on voxel thick hull.
 */
template <class Iter, class Vox, class InRegionDecider>
bool DeciderOutsideSurface6<Iter, Vox, InRegionDecider>::IsPartOfRegion(const Iter & dataIter) const
{
  // only include center voxel, if it is outside the region
  if ( m_inRegionDecider.IsPartOfRegion(dataIter) ) {
    return(false);
  }

  // peek towards neighboring 26 voxels
  // if any of them is part of the region,
  //   do decide that the center one is part of the outside layer
  Neighbor26Iterator<Iter, Vox> n26Iter(dataIter);
  for ( ; !n26Iter.End(); ++n26Iter ) {
    if ( n26Iter.InsideVolume() &&
         m_inRegionDecider.IsPartOfRegion(n26Iter.GetDataVolIter()) ) {
      // we found the next voxel of the surface.
      return( true );
    } // if part of region
  } // end for all 26 neighbors

  // did not find any suitable neighbor -> center is not part of hull
  return(false);

} // DeciderOutsideSurface6::IsPartOfRegion


/**
 * Obsolete, please use DeciderHullOutside.
 * Note that they are 18-connected to the inside region.
 * Thus the Main iterator has to be regionGrow18, and the peek iterator has also to be neighbor18.
 *
 * @param Iter          Type of the volume data iterator.
 * @param InRegionDecider Type of Decider for what is considered inside the region.
 * @param dataIter      Iterator suggestiong a new point on th ehull of the region.
 * @return true if the data iterator in deed points to a voxel on the on voxel thick hull.
 */
template <class Iter, class Vox, class InRegionDecider>
bool DeciderOutsideSurface18<Iter, Vox, InRegionDecider>::IsPartOfRegion(const Iter & dataIter) const
{
  // only include center voxel, if it is outside the region
  if ( m_inRegionDecider.IsPartOfRegion(dataIter) ) {
    return(false);
  }

  // peek towards neighboring 18 voxels
  // if any of them is part of the region,
  //   do decide that the center one is part of the outside layer
  Neighbor18Iterator<Iter, Vox> n18Iter(dataIter);
  for ( ; !n18Iter.End(); ++n18Iter ) {
    if ( n18Iter.InsideVolume() &&
         m_inRegionDecider.IsPartOfRegion(n18Iter.GetDataVolIter()) ) {
      // we found the next voxel of the surface.
      return( true );
    } // if part of region
  } // end for all 18 neighbors

  // did not find any suitable neighbor -> center is not part of hull
  return(false);

} // DeciderOutsideSurface18::IsPartOfRegion


/**
 * Obsolete, please use DeciderHullOutside.
 * Note that they are 6-connected to the inside region.
 * Thus the Main iterator has to be regionGrow26, but the peek iterator has to be neighbor6.
 *
 * @param Iter          Type of the volume data iterator.
 * @param InRegionDecider Type of Decider for what is considered inside the region.
 * @param dataIter      Iterator suggestiong a new point on th ehull of the region.
 * @return true if the data iterator in deed points to a voxel on the on voxel thick hull.
 */
template <class Iter, class Vox, class InRegionDecider>
bool DeciderOutsideSurface26<Iter, Vox, InRegionDecider>::IsPartOfRegion(const Iter & dataIter) const
{
  // only include center voxel, if it is outside the region
  if ( m_inRegionDecider.IsPartOfRegion(dataIter) ) {
    return(false);
  }

  // peek towards neighboring 6 voxels
  // if any of them is part of the region,
  //   do decide that the center one is part of the outside layer
  Neighbor6Iterator<Iter, Vox> n6Iter(dataIter);
  for ( ; !n6Iter.End(); ++n6Iter ) {
    if ( n6Iter.InsideVolume() &&
         m_inRegionDecider.IsPartOfRegion(n6Iter.GetDataVolIter()) ) {
      // we found the next voxel of the surface.
      return( true );
    } // if part of region
  } // end for all 6 neighbors

  // did not find any suitable neighbor -> center is not part of hull
  return(false);

} // DeciderOutsideSurface26::IsPartOfRegion


/**
 * Obsolete, please use DeciderHullInside.
 * Note that they are 26-connected to the outside region.
 * Thus the Main iterator has to be regionGrow6, but the peek iterator has to be neighbor26.
 *
 * @param Iter          Type of the volume data iterator.
 * @param InRegionDecider Type of Decider for what is considered inside the region.
 * @param dataIter      Iterator suggestiong a new point on th ehull of the region.
 * @return true if the data iterator in deed points to a voxel on the on voxel thick hull.
 */
template <class Iter, class Vox, class InRegionDecider>
bool DeciderInsideSurface6<Iter, Vox, InRegionDecider>::IsPartOfRegion(const Iter & dataIter) const
{
  // only include center voxel, if it is inside the region
  if ( !m_inRegionDecider.IsPartOfRegion(dataIter) ) {
    return(false);
  }

  // peek towards neighboring 26 voxels
  // if any of them is NOT part of the region,
  //   do decide that the center one is part of the inside layer
  Neighbor26Iterator<Iter, Vox> n26Iter(dataIter);
  for ( ; !n26Iter.End(); ++n26Iter ) {
    if ( !n26Iter.InsideVolume() ||
         !m_inRegionDecider.IsPartOfRegion(n26Iter.GetDataVolIter()) ) {
      // we found the next voxel of the surface.
      return( true );
    } // if part of region
  } // end for all 26 neighbors

  // did not find any suitable neighbor -> center is not part of hull
  return(false);

} // DeciderInsideSurface6::IsPartOfRegion


/**
 * Obsolete, please use DeciderHullInside.
 * Note that they are 18-connected to the outside region.
 * Thus the Main iterator has to be regionGrow18, and the peek iterator also has to be neighbor18.
 *
 * @param Iter          Type of the volume data iterator.
 * @param InRegionDecider Type of Decider for what is considered inside the region.
 * @param dataIter      Iterator suggestiong a new point on th ehull of the region.
 * @return true if the data iterator in deed points to a voxel on the on voxel thick hull.
 */
template <class Iter, class Vox, class InRegionDecider>
bool DeciderInsideSurface18<Iter, Vox, InRegionDecider>::IsPartOfRegion(const Iter & dataIter) const
{
  // only include center voxel, if it is inside the region
  if ( !m_inRegionDecider.IsPartOfRegion(dataIter) ) {
    return(false);
  }

  // peek towards neighboring 18 voxels
  // if any of them is NOT part of the region,
  //   do decide that the center one is part of the inside layer
  Neighbor18Iterator<Iter, Vox> n18Iter(dataIter);
  for ( ; !n18Iter.End(); ++n18Iter ) {
    if ( !n18Iter.InsideVolume() ||
         !m_inRegionDecider.IsPartOfRegion(n18Iter.GetDataVolIter()) ) {
      // we found the next voxel of the surface.
      return( true );
    } // if part of region
  } // end for all 18 neighbors

  // did not find any suitable neighbor -> center is not part of hull
  return(false);

} // DeciderInsideSurface18::IsPartOfRegion


/**
 * Obsolete, please use DeciderHullInside.
 * Note that they are 6-connected to the inside region.
 * Thus the Main iterator has to be regionGrow26, but the peek iterator has to be neighbor6.
 *
 * @param Iter          Type of the volume data iterator.
 * @param InRegionDecider Type of Decider for what is considered inside the region.
 * @param dataIter      Iterator suggestiong a new point on th ehull of the region.
 * @return true if the data iterator in deed points to a voxel on the on voxel thick hull.
 */
template <class Iter, class Vox, class InRegionDecider>
bool DeciderInsideSurface26<Iter, Vox, InRegionDecider>::IsPartOfRegion(const Iter & dataIter) const
{
  // only include center voxel, if it is inside the region
  if ( !m_inRegionDecider.IsPartOfRegion(dataIter) ) {
    return(false);
  }

  // peek towards neighboring 6 voxels
  // if any of them is NOT part of the region,
  //   do decide that the center one is part of the outside layer
  Neighbor6Iterator<Iter, Vox> n6Iter(dataIter);
  for ( ; !n6Iter.End(); ++n6Iter ) {
    if ( !n6Iter.InsideVolume() ||
         !m_inRegionDecider.IsPartOfRegion(n6Iter.GetDataVolIter()) ) {
      // we found the next voxel of the surface.
      return( true );
    } // if part of region
  } // end for all 6 neighbors

  // did not find any suitable neighbor -> center is not part of hull
  return(false);

} // DeciderInsideSurface26::IsPartOfRegion


// $Log: vxDecider.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/05/03 16:18:21  michael
// completed code review
//
// Revision 1.1  2002/04/22 19:54:01  soeren
// intial version of Regiongrow iterator and decider
//
// Revision 3.0  2001/10/14 23:02:22  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:58   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:28   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:45  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:18  soeren
// renamed volume  directory
//
// Revision 1.17  2001/05/17 00:22:00  ingmar
// DeciderHullOutside now has its own files
//
// Revision 1.16  2001/05/15 13:44:37  ingmar
// DeciderHullInside now has its own file
//
// Revision 1.15  2001/05/13 19:53:01  ingmar
// added DeciderHullOutside
//
// Revision 1.14  2001/05/08 22:44:42  ingmar
// new hull Decider
//
// Revision 1.13  2001/04/09 18:03:51  dmitry
// Removed Revision History from Top of File
//
// Revision 1.12  2000/11/21 03:27:13  ingmar
// implemented and tested Decider Outside/Inside Surface 18
//
// Revision 1.11  2000/11/20 17:32:48  ingmar
// implemented and tested DeciderInsideSurface 6 and 26
//
// Revision 1.10  2000/11/20 01:38:05  ingmar
// implemented DeciderOutsideSurface26
//
// Revision 1.9  2000/11/20 00:53:33  ingmar
// fixed comment
//
// Revision 1.8  2000/11/19 01:34:52  ingmar
// made DeciderOutsideSurface6  use the new neighbor iterator
//
// Revision 1.7  2000/11/01 20:35:59  ingmar
// added inside surface crawlers
//
// Revision 1.6  2000/11/01 17:18:04  ingmar
// added surface crawl Decider for 26 connected voxels
//
// Revision 1.5  2000/11/01 16:30:12  lihong
// debugged surface crawling Decider + itreator
//
// Revision 1.4  2000/11/01 06:41:27  antonio
// test of region grow 26 works
//
// Revision 1.3  2000/10/31 22:33:56  ingmar
// renamed DeciderDilate to DeciderOutsideSurface
//
// Revision 1.2  2000/10/31 16:58:28  ingmar
// fixed syntax error that sneaked in before
//
// Revision 1.1  2000/10/30 18:55:04  ingmar
// added dilate region iterator
//
//
// $Id: vxDecider.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $







