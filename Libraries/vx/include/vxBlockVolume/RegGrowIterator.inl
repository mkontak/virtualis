// $Id: RegGrowIterator.inl,v 1.4 2007/04/12 22:05:18 dongqing Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dong-Qing Chen

#define FILE_REVISION "$Revision: 1.4 $"

/**
 * Constructor.
 * 
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 *
 * @param dataIter iterator for the volume in which a region is supposed to be traversed.
 * @param decider  decides whether to include a voxel into the region
 * @param bInitFlags
 *                 whether to init the flag volume
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter>
RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter>::RegGrowIterator(const DataIter & dataIter, Decider decider, const bool bInitFlags)
	:m_dataIter(dataIter), 
	m_decider(decider),
  m_flagVol(new FlagVol(m_dataIter.GetDim(), 1)), // assume voxel type of flagvolset is bool
  m_bLocalFlagVol(true),
  m_flagIter(m_flagVol->GetVolIterator()), m_bGrowEnded(false),
  m_peekFlagIter(m_flagVol->GetVolIterator()),
  m_peekDataIter(dataIter),
  m_bInitFlagVol(bInitFlags)
{
  Initialize();
} // Constructor


/**
 * Constructor.
 * 
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 *
 * @param dataIter iterator for the volume in which a region is supposed to be traversed.
 * @param decider  decides whether to include a voxel into the region
 * @param bInitFlags
 *                 whether to init the flag volume
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter>
RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter>::RegGrowIterator(const DataIter & dataIter, Decider decider, FlagVol * pOutsideFlagVolume, const bool bInitFlags)
  :m_dataIter(dataIter), m_decider(decider),
  m_flagVol(pOutsideFlagVolume), // set m_background to false
  m_bLocalFlagVol(false),
	m_flagIter(m_flagVol->GetVolIterator(dataIter.GetPos())), 
  m_bGrowEnded(false),
  m_peekFlagIter(m_flagVol->GetVolIterator()),
  m_peekDataIter(dataIter),
  m_bInitFlagVol(bInitFlags)
{
  Initialize();
} // Constructor


/**
 * Destructor.
 *
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter>
RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter>::~RegGrowIterator()
{
  // only delete flag volume if it was created in the constructor
  if (m_bLocalFlagVol) 
  {
    delete m_flagVol;
  }
} // ~RegGrowIterator()


/**
 * Initialize members and check consistency.
 *
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter>
void RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter>::Initialize()
{
  // check if dimensions match
  if ( (m_dataIter.GetDim().m_x != m_flagIter.GetDim().m_x) || (m_dataIter.GetDim().m_y != m_flagIter.GetDim().m_y)
    || (m_dataIter.GetDim().m_z != m_flagIter.GetDim().m_z) )
  {
    LogWrn("dimensions of data iterator and flag iterator does not match","RegGrowIterator", "Initialize");
    m_bGrowEnded = true;
    return;
  }
  // check if m_volIterator is inside
  if ( !m_dataIter.InsideVolume() )
  {
    LogWrn("seed iterator not in volume at "+m_dataIter.GetPos(),"RegGrowIterator", "Initialize");
    m_bGrowEnded = true;
    return;
  }
  if ( !m_decider.IsPartOfRegion( m_dataIter ) )
  {
    std::stringstream ss; 
    ss << m_dataIter.GetVoxel() << "=voxel value of seed iterator not in region at " << m_dataIter.GetPos();
    LogWrn(ss.str(),"RegGrowIterator", "Initialize");
    m_bGrowEnded = true;
    return;
  }
  
  if (m_bInitFlagVol)
  {
    // Clear flags
    SetFlags(false);
  }
  else if (m_flagIter.GetVoxel() == true)
  {
    std::stringstream ss; 
    ss << m_dataIter.GetVoxel() << "=processed flag of seed iterator already set " << m_dataIter.GetPos();
    LogWrn(ss.str(),"RegGrowIterator", "Initialize");
    m_bGrowEnded = true;
    return;
  }
  
  
  // set first flag
  m_flagIter.SetVoxel(true);
  m_posQ.push(m_dataIter.GetPos());
  
  // increment once to get the start voxel off the queue,
  //   but also add the neighbors on the queue
  ++(*this);
} // Initialize()
  

/**
 * set all voxels in the flag volume to the value specified
 *
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter>
void RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter>::SetFlags(bool bValue)
{
	FlagIter flagIter = m_flagVol->GetVolIterator();
	for(; !flagIter.End(); ++flagIter)
		//*flagIter = value;
    flagIter.SetVoxel(bValue);
} // SetFlags()
  
////////////////////////////////////////////////////////////////////////
/// VOLUME TRAVERSAL

/**
 * Does the book keeping to find out it the current voxel could be included in the region.
 * If the mechanical analysis comes out positive, then the decider is asked to make the final decision.
 *
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter>
void RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter>::PossiblyAddToQueue()
{
  if ( m_peekDataIter.InsideVolume()    // still inside valid volume area
       && !m_peekFlagIter.GetVoxelRaw() // not yet flagged as processed/discovered
       && m_decider.IsPartOfRegion( m_peekDataIter.GetDataVolIter() ) )
  {
    m_posQ.push(m_peekDataIter.GetPos());
    m_peekFlagIter.SetVoxel(true);
  }
} // PossiblyAddToQueue


/**
 * pops the next position of the queue, moved the data+flag iterators to there and schedules the neighbors for adition in to the queue
 *
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter>
void RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter>::operator++()
{
  // set m_bGrowEnded if there are no more voxels to process
  if ( m_posQ.empty() )
  {
    m_bGrowEnded = true;
    return;
  }

  // get next voxel of region from queue
  Point<uint2> nextPos = m_posQ.front();
  m_dataIter.MoveAbs(nextPos);
  m_flagIter.MoveAbs(nextPos);
  m_posQ.pop();
  m_peekDataIter.CenterAt(nextPos);
  m_peekFlagIter.CenterAt(nextPos);

  // peek towards next voxels that are potentially part of the region and
  // test if these voxels indeed belong to the region

  for ( ; !m_peekFlagIter.End(); ++m_peekFlagIter,++m_peekDataIter )
  {
    PossiblyAddToQueue();
  }
} // operator ++


/**
 * Indicates when the region growing is finished.
 *
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 * @return true when the traversal is finished.
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter>
bool RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter>::End() const
{
  return(m_bGrowEnded);
} // End


/**
 * Use the position of dataIter as a new seed for region grow
 * Avoid calling constructors of various iterators, simply clear the flag volume
 *
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 * @param dataIter  seed new position for starting region grow
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter>
void RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter>::SetSeed(const DataIter & dataIter) 
{
	while(!m_posQ.empty())
		m_posQ.pop();

	m_bGrowEnded = false;
	m_dataIter = dataIter;
	m_flagIter.MoveAbs(dataIter.GetPos());
  m_peekDataIter.CenterAt(dataIter.GetPos());
  m_peekFlagIter.CenterAt(dataIter.GetPos());
		
	Initialize();
} // End


/**
 * Use the position as a new seed for region grow
 * Avoid calling constructors of various iterators, simply clear the flag volume
 *
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 * @param pos   seed new position for starting region grow
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter>
void RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter>::SetSeed(const Triple<uint2>& pos) 
{
	while(!m_posQ.empty())
		m_posQ.pop();

	m_bGrowEnded = false;
	m_dataIter.MoveAbs(pos);
	m_flagIter.MoveAbs(pos);	
  m_peekDataIter.CenterAt(pos);
  m_peekFlagIter.CenterAt(pos);
	Initialize();
} // End


////////////////////////////////////////////////////////////////////////
/// GET & SET VOXELS

/**
 * Returns the voxel value at the current position (safe version).
 *
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 * @return the voxel value at the position.
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter>
const Vox & RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter>::GetVoxel() const
{
  return(m_dataIter.GetVoxel());
} // GetVoxel


/**
 * Returns the voxel value at the current position (un-safe/fast version).
 *
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 * @return the voxel value at the position.
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter>
const Vox & RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter>::GetVoxelRaw() const
{
  return(m_dataIter.GetVoxelRaw());
} // GetVoxelRaw


/**
 * Sets the voxel value at the current position (safe version).
 *
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 * @param value         the new voxel value.
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter>
void RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter>::SetVoxel(const Vox & value)
{
  m_dataIter.SetVoxel(value);
} // SetVoxel


/**
 * Sets the voxel value at the current position (un-safe/fast version).
 *
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 * @param value         the new voxel value.
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter>
void RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter>::SetVoxelRaw(const Vox & value)
{
  m_dataIter.SetVoxelRaw(value);
} // SetVoxelRaw


////////////////////////////////////////////////////////////////////////
/// STATUS REPORTS

/**
 * Returns the current position of the iterator within the volume.
 *
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 *
 * @return the x,y,z position in the volume.
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter>
const Point<uint2> & RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter>::GetPos() const
{
  return(m_dataIter.GetPos());
} // GetPos


/**
 * indicated the iterator position relative to the volume extend.
 *
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 *
 * @return true if the iterator is inside the volume.
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter>
bool RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter>::InsideVolume() const
{
  return(m_dataIter.InsideVolume());
} // InsideVolume


/**
 * Overloaded << Operator.
 *
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 *
 * @param os        stream to output to
 * @param iter      iterator
 * @return          output stream
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagVol, class FlagIter, class FlagNeighborIter, class Decider >
std::ostream & operator<<(std::ostream & os, const RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter> & iter)
{
  if ( iter.InsideVolume() )
  {
    os << "RegGrowIterator{Pos:"<<iter.GetPos() << "(inside)"  << "Val:"<<iter.GetVoxel() << "}" << flush;
  } 
  else
  {
    os << "RegGrowIterator{Pos:"<<iter.GetPos() << "(outside)" << "Val:"<<iter.GetVoxel() << "}" << flush;
  }
  return( os );
} // operator<<


////////////////////////////////////////////////////////////////////////
/// VERIFICATION

/**
 * Test function to verify and demonstrate the class functionality.
 *
 * @param DataIter Type of the volume data iterator.
 * @param Vox      Type of the voxels in the data volume.
 * @param DataNeighborIter
 *                 Type of the neighbor iterator that is used to traverse neighbors of an in-region voxel to test if they are also part of the region.
 * @param FlagNeighborIter
 *                 Type of the neighbor iterator that is used for book keeping where we have been already.  
 *                 This iteration MUST have the same traversal pattern as the DataNeighborIterator.
 * @param Decider  Type of the decider for whether to include a voxel into the region.
 * @param FlagVol   Type of the flag volume
 * @param FlagIter  Type of the iterator of flag volume
 *
 * @param os       output stream to print progress info to
 * @return indicate whether the test was successful
 */
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter>
bool RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter>::TestByType(std::ostream & os)
{
  bool bOk(true);

  Vox label(1);
  Volume<BlockVolume<Vox>,Vox> vol(Triple<uint4>(3,3,3), 8*sizeof(Vox));

  // turn on some voxels to the pattern below 
  // ...  .+.  ..+
  // ...  .++  .++
  // +..  ...  +..
  vol.SetVoxelRaw(Point<uint2>(0,0,0),label);
  vol.SetVoxelRaw(Point<uint2>(1,1,1),label);
  vol.SetVoxelRaw(Point<uint2>(2,1,1),label);
  vol.SetVoxelRaw(Point<uint2>(1,2,1),label);
  vol.SetVoxelRaw(Point<uint2>(0,0,2),label);
  vol.SetVoxelRaw(Point<uint2>(1,1,2),label);
  vol.SetVoxelRaw(Point<uint2>(2,1,2),label);
  vol.SetVoxelRaw(Point<uint2>(2,2,2),label);

  /// SETUP the RegionGrowIterator

  // make an iterator attached to region grow seed point
  typedef BlockVolume<Vox>::VolIterator VolIter;
  VolIter iter(vol.GetVolIterator(Point<uint2>(1,1,1)));

  // make a decider that declares a voxel as part of a region if it has the same label
  typedef DeciderEqual<VolIter, Vox> Decider;
  Decider decider(label);

  // make a RegGrowIterator
  typedef RegGrowIterator<VolIter, Vox, DataNeighborIter, FlagNeighborIter, Decider> GrowIter;
  GrowIter growIter(iter, decider);

  os << "RegGrowIterator<\n  "
    << typeid(iter).name()<<",\n  "
    << typeid(DataNeighborIter).name()<<",\n  "
    << typeid(label).name()<<",\n  "
    << typeid(decider).name()<<" >::TestByType()" << endl;
  vol.PrintAllRaw();

  /// USE the RegionGrowIterator

  // grow through the voxels in the region and label them in the order traversed
  Vox newLabel(0);
  for ( ; !growIter.End(); ++growIter )
  {
    growIter.SetVoxelRaw(newLabel);
  }
  os << "after resetting via RegGrowIterator" << endl;
  vol.PrintAllRaw();

  bOk &= (newLabel == vol.GetVoxelRaw(Point<uint2>(1,1,1)));
  bOk &= (newLabel == vol.GetVoxelRaw(Point<uint2>(2,1,1)));
  bOk &= (newLabel == vol.GetVoxelRaw(Point<uint2>(1,2,1)));
  bOk &= (newLabel == vol.GetVoxelRaw(Point<uint2>(1,1,2)));
  if ( bOk )
  {
    os << ":-) TEST SUCCEEDED"<<endl;
  } else
  {
    os << ":-( TEST FAILED" << endl;
  }
  return(bOk);
} // TestByType

#undef FILE_REVISION

// $Log: RegGrowIterator.inl,v $
// Revision 1.4  2007/04/12 22:05:18  dongqing
// code review comments
//
// Revision 1.3  2007/04/10 01:23:32  jmeade
// code review.
//
// Revision 1.2  2007/03/06 18:58:23  dongqing
// code review
//
// Revision 1.1  2005/08/05 13:01:31  geconomos
// moved from vx repository
//
// Revision 1.1  2005/04/14 12:49:46  geconomos
// initial revision
//
// Revision 3.8  2003/01/22 21:29:17  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.7  2001/12/17 16:35:53  jaddonisio
// i. Initialize flag iter with data iter pos.
// ii. Log a warning when dimension between data and flag volumes don't match.
// iii. Check if inside volume using data iter instead of flag iter.
//
// Revision 3.6  2001/12/06 22:25:03  jenny
// exit on invalid seed point, otherwise always increment the pointer
//
// Revision 3.5  2001/11/16 01:05:34  jaddonisio
// Set empty flag when not in region.
//
// Revision 3.4  2001/11/07 18:25:02  soeren
// make sure we are not starting at a position masked out by the handed-in flag volume
//
// Revision 3.3  2001/11/02 15:12:15  liwei
// Added flag to indicate whether the voxels of the  flag volume need to be initialized to false.
//
// Revision 3.2  2001/11/01 15:42:14  jaddonisio
// Removed SetFlags(false) to give that flexibity to the calling functions.
//
// Revision 3.1  2001/10/26 02:21:07  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:24  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:48:24   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:00   ingmar
// Initial revision.
// Revision 1.3  2001/09/13 17:42:49  soeren
// added quotes to function name in logger macro
//
// Revision 1.2  2001/09/10 09:04:59  ingmar
// fixed use of logger defines
//
// Revision 1.1  2001/09/10 01:16:43  ingmar
// new dir structure
//
// Revision 1.3  2001/09/07 21:21:16  soeren
// added #undef FILE_VERSION in the end of each file
//
// Revision 1.2  2001/09/07 17:59:09  uday
// Logger::xyz  -> LogXyz
//
// Revision 1.1  2001/09/01 00:12:21  soeren
// renamed volume  directory
//
// Revision 1.9  2001/08/24 17:41:50  liwei
// RegGrowIteratorNew merged with the previous RegGrowIterator.
//
// Revision 1.3  2001/08/16 23:38:46  liwei
// Bugs in SetSeed() fixed.
//
// Revision 1.2  2001/08/16 22:48:49  liwei
// Added SetSeed() to RegGrowIterator to accelerate series of region grow on
// volumes or slices of the same size.
//
// Revision 1.1  2001/08/16 17:39:16  liwei
// The duplicated RegGrowIterator is put at 4_image with the name RegGrowIterator.
// It handles both 3D and 2D region grow. It is expected to replace the current RegGrowIterator,
// but all the existing references should use two extra templates.
// Slice and ImageIterator modified to have compatible functions with Volume
// and VolIterator required by the RegGrowIterator.
//
// Revision 1.7  2001/08/14 21:01:50  liwei
// Comitted for huamin to compile.
//
// Revision 1.6  2001/08/14 05:22:16  ingmar
// improved logging
//
// Revision 1.5  2001/08/09 22:18:44  frank
// added outside flagVolume to be handed in
//
// Revision 1.4  2001/08/07 17:03:40  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.3  2001/05/13 18:49:09  ingmar
// code beautify
//
// Revision 1.2  2001/05/12 04:23:02  ingmar
// cleaner use of template classes
//
// Revision 1.1  2001/04/28 01:25:48  ingmar
// RegionGrowIterator for any neighborhood
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/RegGrowIterator.inl,v 1.4 2007/04/12 22:05:18 dongqing Exp $
// $Id: RegGrowIterator.inl,v 1.4 2007/04/12 22:05:18 dongqing Exp $

