// $Id: vxBlockVolumeIteratorNeighbors.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc, All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Rekha (ingmar|rekha)@viatronix.com


// includes
#include "stdafx.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxComplex.h"


// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


// partial template instantiations
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIteratorNeighbors<uint1>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIteratorNeighbors<uint2>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIteratorNeighbors<uint4>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIteratorNeighbors< int1>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIteratorNeighbors< int2>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIteratorNeighbors< int4>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIteratorNeighbors< bool>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIteratorNeighbors< float4>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIteratorNeighbors<vxComplex>;


// namespaces
using namespace std;


/**
 * Sets the current x,y,z position to the next 4 connected 2D neighbor in the xy plane.
 * It also increments the m_iNeighborIndex.
 */
template <class VoxelType>
void vxBlockVolumeIteratorNeighbors<VoxelType>::Next2Dneighbor04xy()
{
  //  bottom     middle     top
  //  z = -1     z = 0      z = +1
  //
  //  .  .  .    .  d  .    .  .  .     Y
  //                                        Z
  //  .  .  .    a  @  c    .  .  .     | /
  //                                    |/
  //  .  .  .    .  b  .    .  .  .     +--- X

  switch (m_iNeighborIndex)
  {
  case 0: // from center to first neighbor i.e 'a'
    m_volIter.DecX();                   
    break; 
  case 1: // to second neighbor i.e 'b'
    m_volIter.IncX(); m_volIter.DecY(); 
    break; 
  case 2: // to third neighbor i.e 'c'
    m_volIter.IncX(); m_volIter.IncY(); 
    break;
  case 3: // to forth neighbor i.e 'd'
    m_volIter.DecX(); m_volIter.IncY(); 
    break;
  case 4: // back to center i.e '@'
    m_volIter.DecY(); 
    break; 
  }
  ++m_iNeighborIndex;    
} // Next2Dneighbor04xy()
     


/**
 * Sets the current x,y,z position to the next 4 connected 2D neighbor in the yz plane.
 * It also increments the m_iNeighborIndex.
 */
template <class VoxelType>
void vxBlockVolumeIteratorNeighbors<VoxelType>::Next2Dneighbor04xz()
{
  //  bottom     middle     top
  //  z = -1     z = 0      z = +1
  //
  //  .  .  .    .  .  .    .  .  .     Y
  //                                        Z
  //  .  b  .    a  @  c    .  d  .     | /
  //                                    |/
  //  .  .  .    .  .  .    .  .  .     +--- X

  switch (m_iNeighborIndex)
  {
  case 0: // from center to first neighbor i.e 'a'
    m_volIter.DecX();                   
    break;
  case 1: // to second neighbor i.e 'b'
    m_volIter.IncX(); m_volIter.DecZ(); 
    break; 
  case 2: // to third neighbor i.e 'c'
    m_volIter.IncX(); m_volIter.IncZ(); 
    break; 
  case 3: // to forth neighbor i.e 'd'
    m_volIter.DecX(); m_volIter.IncZ(); 
    break; 
  case 4: // back to center i.e '@'                 
    m_volIter.DecZ(); 
    break; 
  }
  ++m_iNeighborIndex;    
} // Next2Dneighbor04xz()


/**
 * Sets the current x,y,z position to the next 4 connected 2D neighbor in the yz plane.
 * It also increments the m_iNeighborIndex.
 */
template <class VoxelType>
void vxBlockVolumeIteratorNeighbors<VoxelType>::Next2Dneighbor04yz()
{
  //  bottom     middle     top
  //  z = -1     z = 0      z = +1
  //
  //  .  .  .    .  d  .    .  .  .     Y
  //                                        Z
  //  .  a  .    .  @  .    .  c  .     | /
  //                                    |/
  //  .  .  .    .  b  .    .  .  .     +--- X

  switch (m_iNeighborIndex)
  {
  case 0: // from center to first neighbor i.e 'a'
    m_volIter.DecZ();                   
    break; 
  case 1: // to second neighbor i.e 'b'
    m_volIter.IncZ(); 
    m_volIter.DecY(); 
    break; 
  case 2: // to third neighbor i.e 'c'
    m_volIter.IncZ(); 
    m_volIter.IncY(); 
    break; 
  case 3: // to forth neighbor i.e 'd'
    m_volIter.DecZ(); 
    m_volIter.IncY(); 
    break; 
  case 4: // back to center i.e '@'                 
    m_volIter.DecY(); 
    break; 
  }
  ++m_iNeighborIndex;    
} // Next2Dneighbor04yz()


/**
 * Sets the current x,y,z position to the next 8 connected 2D neighbor in the zy plane.
 * It also increments the m_iNeighborIndex.
 */
template <class VoxelType>
void vxBlockVolumeIteratorNeighbors<VoxelType>::Next2Dneighbor08xy()
{
  //  bottom     middle     top
  //  z = -1     z = 0      z = +1
  //
  //  .  .  .    d--c--b    .  .  .     Y
  //             |     |                    Z
  //  .  .  .    e  @--a    .  .  .     | /
  //             |                      |/
  //  .  .  .    f--g--h    .  .  .     +--- X

  switch (m_iNeighborIndex)
  {
  case 0: // from center to 1st neighbor i.e 'a'
    m_volIter.IncX();                    
    break; 
  case 1: // to 2nd neighbor i.e 'b'
    m_volIter.IncY();                    
    break; 
  case 2: // to 3rd neighbor i.e 'c'
    m_volIter.DecX();                    
    break; 
  case 3: // to 4th neighbor i.e 'd'
    m_volIter.DecX();                    
    break; 
  case 4: // to 5th neighbor i.e 'e' 
    m_volIter.DecY();                    
    break;
  case 5: // to 6th neighbor i.e 'f'
    m_volIter.DecY();                    
    break; 
  case 6: // to 7th neighbor i.e 'g'
    m_volIter.IncX();                    
    break; 
  case 7: // to 8th neighbor i.e 'h'
    m_volIter.IncX();                    
    break; 
  case 8: // back to the center i.e '@'
    m_volIter.DecX(); m_volIter.IncY();  
    break; 
  }
  ++m_iNeighborIndex;    

} // Next2Dneighbor08xy()


/**
 * Sets the current x,y,z position to the next 8 connected 2D neighbor in the xz plane.
 * It also increments the m_iNeighborIndex.
 */
template <class VoxelType>
void vxBlockVolumeIteratorNeighbors<VoxelType>::Next2Dneighbor08xz()
{
  //  bottom     middle     top
  //  z = -1     z = 0      z = +1
  //
  //  .  .  .    .  .  .    .  .  .     Y
  //                                        Z
  //  d--c--b    e  @--a    f--g--h     | /
  //                                    |/
  //  .  .  .    .  .  .    .  .  .     +--- X

  switch (m_iNeighborIndex)
  {
  case 0: // from center to 1st neighbor i.e 'a'
    m_volIter.IncX();                    
    break; 
  case 1: // to 2nd neighbor i.e 'b'
    m_volIter.IncZ();                    
    break; 
  case 2: // to 3rd neighbor i.e 'c' 
    m_volIter.DecX();                    
    break; 
  case 3: // to 4th neighbor i.e 'd'
    m_volIter.DecX();                    
    break; 
  case 4: // to 5th neighbor i.e 'e'
    m_volIter.DecZ();                    
    break;
  case 5: // to 6th neighbor i.e 'f'
    m_volIter.DecZ();                    
    break; 
  case 6: // to 7th neighbor i.e 'g'
    m_volIter.IncX();                    
    break; 
  case 7: // to 8th neighbor i.e 'h'
    m_volIter.IncX();                    
    break; 
  case 8: // back to the center i.e '@'
    m_volIter.DecX(); m_volIter.IncZ();  
    break; 
  }
  ++m_iNeighborIndex;    
} // Next2Dneighbor08xz()


/**
 * Sets the current x,y,z position to the next 8 connected 2D neighbor in the yz plane.
 * It also increments the m_iNeighborIndex.
 */
template <class VoxelType>
void vxBlockVolumeIteratorNeighbors<VoxelType>::Next2Dneighbor08yz()
{
  //  bottom     middle     top
  //  z = -1     z = 0      z = +1
  //
  //  .  d  .    .  c  .    .  b  .     Y
  //     |                     |            Z
  //  .  e  .    .  @  .    .  a  .     | /
  //     |                              |/
  //  .  f  .    .  g  .    .  h  .     +--- X

  switch (m_iNeighborIndex)
  {
  case 0: // from center to 1st neighbor i.e 'a' 
    m_volIter.IncZ();                    
    break; 
  case 1: // to 2nd neighbor i.e 'b' 
    m_volIter.IncY();                    
    break; 
  case 2: // to 3rd neighbor i.e 'c' 
    m_volIter.DecZ();                    
    break; 
  case 3: // to 4th neighbor i.e 'd' 
    m_volIter.DecZ();                    
    break; 
  case 4: // to 5th neighbor i.e 'e' 
    m_volIter.DecY();                    
    break; 
  case 5: // to 6th neighbor i.e 'f' 
    m_volIter.DecY();                    
    break; 
  case 6: // to 7th neighbor i.e 'g' 
    m_volIter.IncZ();                    
    break; 
  case 7: // to 8th neighbor i.e 'h'
    m_volIter.IncZ();                    
    break; 
  case 8: // back to the center i.e '@'
    m_volIter.DecZ(); m_volIter.IncY();  
    break; 
  }
  ++m_iNeighborIndex;    
} // Next2Dneighbor08yz()


/**
 * Sets the current x,y,z position to the next 3D, 7 neighbor.
 * It also increments the m_iNeighborIndex.
 **/
template <class VoxelType>
void vxBlockVolumeIteratorNeighbors<VoxelType>::Next3Dneighbor07()
{
  //  bottom     middle     top
  //  z = -1     z = 0      z = +1
  //
  //  .  .  .    .  c--b    .  d--e     Y
  //                   |          |         Z
  //  .  .  .    .  @--a    .  g--f     | /
  //                                    |/
  //  .  .  .    .  .  .    .  .  .     +--- X

  switch (m_iNeighborIndex)
  {
  case 0: // from center to first neighbor i.e 'a'
    m_volIter.IncX();  
    break; 
  case 1: // to second neighbor i.e 'b'
    m_volIter.IncY();  
    break; 
  case 2: // to third neighbor i.e 'c'
    m_volIter.DecX();  
    break; 
  case 3: // to forth neighbor i.e 'd'
    m_volIter.IncZ();  
    break; 
  case 4: // to fifth neighbor i.e 'e'
    m_volIter.IncX();  
    break; 
  case 5: // to sixth neighbor i.e 'f'
    m_volIter.DecY();  
    break;
  case 6: // to sixth neighbor i.e 'g'
    m_volIter.DecX();  
    break; 
  case 7: // back to center i.e '@'
    m_volIter.DecZ();  
    break; 
  }
  ++m_iNeighborIndex;    
} // Next3Dneighbor07()


/**
 * Sets the current x,y,z position to the next 3D, 6 neighbor.
 * It also increments the m_iNeighborIndex.
 */
template <class VoxelType>
void vxBlockVolumeIteratorNeighbors<VoxelType>::Next3Dneighbor06()
{
  //  bottom     middle     top
  //  z = -1     z = 0      z = +1
  //
  //  .  .  .    .  e  .    .  .  .     Y
  //                                        Z
  //  .  a  .    d  @  c    .  b  .     | /
  //                                    |/
  //  .  .  .    .  f  .    .  .  .     +--- X

  switch (m_iNeighborIndex)
  {
  case 0: // from center to first neighbor i.e 'a'
    m_volIter.DecZ();                   
    break; 
  case 1: // to second neighbor i.e 'b'
    m_volIter.IncZ(); m_volIter.IncZ(); 
    break; 
  case 2: // to third neighbor i.e 'c'
    m_volIter.DecZ(); m_volIter.IncX(); 
    break; 
  case 3: // to forth neighbor i.e 'd'
    m_volIter.DecX(); m_volIter.DecX(); 
    break; 
  case 4: // to fifth neighbor i.e 'e'
    m_volIter.IncX(); m_volIter.IncY(); 
    break; 
  case 5: // to sixth neighbor i.e 'f'
    m_volIter.DecY(); m_volIter.DecY(); 
    break; 
  case 6: // back to center i.e '@'
    m_volIter.IncY();                   
    break; 
  }
  ++m_iNeighborIndex;    
} // Next3Dneighbor06()


/**
 * Sets the current x,y,z position to the next 3D, 18 neighbor.
 * It also increments the m_iNeighborIndex.
 */
template <class VoxelType>
void vxBlockVolumeIteratorNeighbors<VoxelType>::Next3Dneighbor18()
{
  //  bottom     middle     top
  //  z = -1     z = 0      z = +1
  //
  //  .  b  .    i--h--g    .  o  .     Y
  //   / |       |     |       | \          Z
  //  c  a  e    j  @  f    r  p  n     | /
  //   \   /     |           \ |        |/
  //  .  d  .    k--l--m    .  q  .     +--- X

  switch (m_iNeighborIndex)
  {
  case 0: // from center to 1st neighbor i.e 'a'
    m_volIter.DecZ();                    
    break; 
  case 1: // to 2nd neighbor i.e 'b'
    m_volIter.IncY();                    
    break; 
  case 2: // to 3rd neighbor i.e 'c'
    m_volIter.DecX(); m_volIter.DecY();  
    break; 
  case 3: // to 4th neighbor i.e 'd'
    m_volIter.IncX(); m_volIter.DecY();  
    break; 
  case 4: // to 5th neighbor i.e 'e'
    m_volIter.IncX(); m_volIter.IncY();  
    break; 
  case 5: // to 6th neighbor i.e 'f'
    m_volIter.IncZ();    
    break; 
  case 6: // to 7th neighbor i.e 'g'
    m_volIter.IncY();                    
    break; 
  case 7: // to 8th neighbor i.e 'h'
    m_volIter.DecX();                    
    break; 
  case 8: // to 9th neighbor i.e 'i'
    m_volIter.DecX();                    
    break; 
  case 9: // to 10th neighbor i.e 'j'
    m_volIter.DecY();                    
    break; 
  case 10: // to 11th neighbor i.e 'k'
    m_volIter.DecY();                    
    break; 
  case 11: // to 12th neighbor i.e 'l'
    m_volIter.IncX();                    
    break; 
  case 12: // to 13th neighbor i.e 'm'
    m_volIter.IncX();                    
    break; 
  case 13: // to 14th neighbor i.e 'n'
    m_volIter.IncY(); m_volIter.IncZ();  
    break; 
  case 14: // to 15th neighbor i.e 'o'
    m_volIter.DecX(); m_volIter.IncY();  
    break; 
  case 15: // to 16th neighbor i.e 'p'
    m_volIter.DecY();                    
    break; 
  case 16: // to 17th neighbor i.e 'q'
    m_volIter.DecY();                    
    break; 
  case 17: // to 18th neighbor i.e 'r'
    m_volIter.DecX(); m_volIter.IncY();  
    break; 
  case 18: // back to center position i.e '@'
    m_volIter.IncX(); m_volIter.DecZ();  
    break; 
  }
  ++m_iNeighborIndex;    
} // Next3Dneighbor18


/**
 * Sets the current x,y,z position to the next 3D, 26 neighbor.
 * It also increments the m_iNeighborIndex.
 */
template <class VoxelType>
void vxBlockVolumeIteratorNeighbors<VoxelType>::Next3Dneighbor26()
{
  //  bottom     middle     top
  //  z = -1     z = 0      z = +1
  //
  //  i--h--g    j  q--p    s--r  y     Y
  //        |    |     |    |    /|         Z
  //  b--a  f    k  @  o    t  z  x     | /
  //  |     |    |     |    |     |     |/
  //  c--d--e    l--m--n    u--v--w     +--- X

  switch (m_iNeighborIndex)
  {
  case 0:  m_volIter.DecZ(); break; // from center to 1st neighbor i.e 'a'
  case 1:  m_volIter.DecX(); break; // to 2nd neighbor i.e 'b'
  case 2:  m_volIter.DecY(); break; // to 3rd neighbor i.e 'c'
  case 3:  m_volIter.IncX(); break; // to 4th neighbor i.e 'd'
  case 4:  m_volIter.IncX(); break; // to 5th neighbor i.e 'e'
  case 5:  m_volIter.IncY(); break; // to 6th neighbor i.e 'f'
  case 6:  m_volIter.IncY(); break; // to 7th neighbor i.e 'g'
  case 7:  m_volIter.DecX(); break; // to 8th neighbor i.e 'h'
  case 8:  m_volIter.DecX(); break; // to 9th neighbor i.e 'i'
  case 9:  m_volIter.IncZ(); break; // to 10th neighbor i.e 'j'
  case 10: m_volIter.DecY(); break; // to 11th neighbor i.e 'k'
  case 11: m_volIter.DecY(); break; // to 12th neighbor i.e 'l'
  case 12: m_volIter.IncX(); break; // to 13th neighbor i.e 'm'
  case 13: m_volIter.IncX(); break; // to 14th neighbor i.e 'n'
  case 14: m_volIter.IncY(); break; // to 15th neighbor i.e 'o'
  case 15: m_volIter.IncY(); break; // to 16th neighbor i.e 'p'
  case 16: m_volIter.DecX(); break; // to 17th neighbor i.e 'q'
  case 17: m_volIter.IncZ(); break; // to 18th neighbor i.e 'r'
  case 18: m_volIter.DecX(); break; // to 19th neighbor i.e 's'
  case 19: m_volIter.DecY(); break; // to 20th neighbor i.e 't'
  case 20: m_volIter.DecY(); break; // to 21st neighbor i.e 'u'
  case 21: m_volIter.IncX(); break; // to 22nd neighbor i.e 'v'
  case 22: m_volIter.IncX(); break; // to 23rd neighbor i.e 'w'
  case 23: m_volIter.IncY(); break; // to 24th neighbor i.e 'x'
  case 24: m_volIter.IncY(); break; // to 25th neighbor i.e 'y'
  case 25: m_volIter.DecX(); 
           m_volIter.DecY(); break; // to 26th neighbor i.e 'z'
  case 26: m_volIter.DecZ(); break; // back to center position '@'
  }
  ++m_iNeighborIndex;    
} // Next3Dneighbor26()


/**
 * Sets the current x,y,z position to the next 3D, 64 neighbor.
 * It also increments the m_iNeighborIndex.
 **/
template <class VoxelType>
void vxBlockVolumeIteratorNeighbors<VoxelType>::Next3Dneighbor63()
{
  // bottom           middle           middle+1          top
  // z = -1           z = 0            z = +1            z = +2
  //
  // 16--15--14--13   17  28--27--26   30--29  40--39    55--56  45--46
  //              |   |           |    |       |   |     |   |   |   |
  // 05--04--03  12   18  61--62  25   31  60  41  38    54  57  44  47
  // |        |   |   |       |   |    |   |   |   |     |   |   |   |
  // 06  01--02  11   19   @--63  24   32  59  42  37    53  58  43  48
  // |            |   |           |    |           |     |           | 
  // 07--08--09--10   20--21--22--23   33--34--35--36    52--51--50--49

  switch (m_iNeighborIndex)
  {
  case 0:  m_volIter.DecZ(); break; // XX -> 01
  case 1:  m_volIter.IncX(); break; // 01 -> 02
  case 2:  m_volIter.IncY(); break; // 02 -> 03
  case 3:  m_volIter.DecX(); break; // 03 -> 04
  case 4:  m_volIter.DecX(); break; // 04 -> 05
  case 5:  m_volIter.DecY(); break; // 05 -> 06
  case 6:  m_volIter.DecY(); break; // 06 -> 07
  case 7:  m_volIter.IncX(); break; // 07 -> 08
  case 8:  m_volIter.IncX(); break; // 08 -> 09
  case 9:  m_volIter.IncX(); break; // 09 -> 10
  case 10: m_volIter.IncY(); break; // 10 -> 11
  case 11: m_volIter.IncY(); break; // 11 -> 12
  case 12: m_volIter.IncY(); break; // 12 -> 13
  case 13: m_volIter.DecX(); break; // 13 -> 14
  case 14: m_volIter.DecX(); break; // 14 -> 15
  case 15: m_volIter.DecX(); break; // 15 -> 16
  case 16: m_volIter.IncZ(); break; // 16 -> 17
  case 17: m_volIter.DecY(); break; // 17 -> 18
  case 18: m_volIter.DecY(); break; // 18 -> 19
  case 19: m_volIter.DecY(); break; // 19 -> 20
  case 20: m_volIter.IncX(); break; // 20 -> 21
  case 21: m_volIter.IncX(); break; // 21 -> 22
  case 22: m_volIter.IncX(); break; // 22 -> 23
  case 23: m_volIter.IncY(); break; // 23 -> 24
  case 24: m_volIter.IncY(); break; // 24 -> 25
  case 25: m_volIter.IncY(); break; // 25 -> 26
  case 26: m_volIter.DecX(); break; // 26 -> 27
  case 27: m_volIter.DecX(); break; // 27 -> 28
  case 28: m_volIter.IncZ(); break; // 28 -> 29
  case 29: m_volIter.DecX(); break; // 29 -> XX
  case 30: m_volIter.DecY(); break; // 30 -> 31
  case 31: m_volIter.DecY(); break; // 31 -> 32
  case 32: m_volIter.DecY(); break; // 32 -> 33
  case 33: m_volIter.IncX(); break; // 33 -> 34
  case 34: m_volIter.IncX(); break; // 34 -> 35
  case 35: m_volIter.IncX(); break; // 35 -> 36
  case 36: m_volIter.IncY(); break; // 36 -> 37
  case 37: m_volIter.IncY(); break; // 37 -> 38
  case 38: m_volIter.IncY(); break; // 38 -> 39
  case 39: m_volIter.DecX(); break; // 39 -> 40
  case 40: m_volIter.DecY(); break; // 40 -> 41
  case 41: m_volIter.DecY(); break; // 41 -> 42
  case 42: m_volIter.IncZ(); break; // 42 -> 43
  case 43: m_volIter.IncY(); break; // 43 -> 44
  case 44: m_volIter.IncY(); break; // 44 -> 45
  case 45: m_volIter.IncX(); break; // 45 -> 46
  case 46: m_volIter.DecY(); break; // 46 -> 47
  case 47: m_volIter.DecY(); break; // 47 -> 48
  case 48: m_volIter.DecY(); break; // 48 -> 49
  case 49: m_volIter.DecX(); break; // 49 -> 50
  case 50: m_volIter.DecX(); break; // 50 -> 51
  case 51: m_volIter.DecX(); break; // 51 -> 52
  case 52: m_volIter.IncY(); break; // 52 -> 53
  case 53: m_volIter.IncY(); break; // 53 -> 54
  case 54: m_volIter.IncY(); break; // 54 -> 55
  case 55: m_volIter.IncX(); break; // 55 -> 56
  case 56: m_volIter.DecY(); break; // 56 -> 57
  case 57: m_volIter.DecY(); break; // 57 -> 58
  case 58: m_volIter.DecZ(); break; // 58 -> 59
  case 59: m_volIter.IncY(); break; // 59 -> 60
  case 60: m_volIter.DecZ(); break; // 60 -> 61
  case 61: m_volIter.IncX(); break; // 61 -> 62
  case 62: m_volIter.DecY(); break; // 62 -> 63
  case 63: m_volIter.DecX(); break; // 63 ->  @
  }
  ++m_iNeighborIndex;
} // Next3Dneighbor63 


/**
 * To string conversion
 * @return string
 */
template <class VoxelType>
std::string vxBlockVolumeIteratorNeighbors<VoxelType>::ToString() const
{
  stringstream ss;

  ss << "vxBlockVolumeIteratorNeighbors{Pos: "<<GetPos().ToString();

  if (IsInsideVolume() == true)
  {
    ss << ", inside, Vox: ";
  }
  else
  {
    ss << ", outside,Vox: ";
  }
  ss << GetVoxel() << "}";

  return(string(ss.str()));
} // ToString()


#undef FILE_REVISION


// $Log: vxBlockVolumeIteratorNeighbors.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.15  2003/05/03 16:18:19  michael
// completed code review
//
// Revision 1.14  2003/05/02 14:36:56  dongqing
// code walk through
//
// Revision 1.13  2003/01/22 21:30:11  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.12  2002/08/28 15:08:33  ingmar
// added support for vxComplex
//
// Revision 1.11  2002/08/14 16:07:41  wenli
// initial version - Vascular
//
// Revision 1.10  2002/07/02 20:44:13  geconomos
// Exporting classes .
//
// Revision 1.9  2002/05/14 21:52:09  ingmar
// added e3Dneighbors07 for traversal of 2x2x2 neighborhood
//
// Revision 1.8  2002/04/25 16:35:34  ingmar
// added e2Dneighbors08 xy, xz and zy neighbor traversal
//
// Revision 1.7  2002/04/25 16:13:18  ingmar
// added e2Dneighbors04xz and zy neighbor traversal
//
// Revision 1.6  2002/04/25 16:02:28  ingmar
// added e2Dneighbors04xy neighbor traversal
//
// Revision 1.5  2002/04/22 20:44:36  soeren
// added new constructor to regiongrowIterator
//
// Revision 1.4  2002/04/18 19:54:32  ingmar
// removed obsolete GetVoxelRaw() from vxBlockVolumeIterator class
//
// Revision 1.3  2002/04/12 16:16:33  rekha
// extended to 6 & 18 & 26 & 63 neighborhoods
//
// Revision 1.2  2002/04/09 21:20:41  ingmar
// first working test
//
// Revision 1.1  2002/04/09 18:46:48  ingmar
// initial version
//
// $Id: vxBlockVolumeIteratorNeighbors.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
