// $Id: vxComponentUtils.C,v 1.23.2.1 2009/08/31 13:16:46 kchalupa Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxComponentUtils.h"
#include "vxUtils.h"
#include "vxBlockVolumeIterator.h"
#include "Timer.h"
#include "vxEnvironment.h"
#include "vxVolumeHeader.h"
#include "vxMessageBox.h"
#include "vxBlockVolumeIteratorNeighbors.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.23.2.1 $"


/**
 * Create a map that maps currently used label indices to new used indices that are contiguous.
 *
 * @param volumeLabel   label volume
 * @return mapping of currently used label indices to contiguous new used indices.
 */
std::map<uint2,uint2> vxComponentUtils::CreateMapToCompactVolume( vxBlockVolume<uint2> & volumeLabel )
{
  vxBlockVolumeIterator<uint2> iterLabelVolume(volumeLabel);

  // Parse label volume to determine distinct labels:
  std::list<uint2> distinctLabelsList;
  uint2 uLabel(0);

  for (iterLabelVolume.FirstNonBackgroundBlockZYX(); iterLabelVolume.IsNotAtEnd(); iterLabelVolume.NextNonBackgroundBlockZYX())
  {
    if (iterLabelVolume.GetCurrentBlock().IsHomogenous())
    {
      uLabel = iterLabelVolume.GetVoxel();

      if (std::find(distinctLabelsList.begin(), distinctLabelsList.end(), uLabel) == distinctLabelsList.end())
      {
        if ( uLabel > 0 )
        {
          distinctLabelsList.push_back(uLabel);
        }
      }
    } // if
    else
    {
      for (iterLabelVolume.FirstNonBackgroundZYXinsideBlock() ; iterLabelVolume.IsNotAtEndOfBlock() ; iterLabelVolume.NextNonBackgroundZYXinsideBlock())
      {
        uLabel = iterLabelVolume.GetVoxel();
        if (std::find(distinctLabelsList.begin(), distinctLabelsList.end(), uLabel) == distinctLabelsList.end())
        {
          if ( uLabel > 0 )
          {
            distinctLabelsList.push_back(uLabel);
          }
        }
      } // for
    } // else
  } // end for all non-zero label voxels

  // Sort list of labels:
  distinctLabelsList.sort();

  // Map labels to order(indices):
  std::map<uint2,uint2> labelMap;
  uint4 uIndex(1);

  for (std::list<uint2>::iterator iter(distinctLabelsList.begin()) ; iter != distinctLabelsList.end() ; iter++, uIndex++)
  {
    labelMap[*iter] = uIndex;
  }

  return labelMap;
} // CreateMapToCompactVolume()


/**
 *  Write the component as a separate volume
 *
 *  @param volItensity the intensity volume
 *  @param volLabel the label volume
 *  @param iNumLabels the index for the component
 */
void vxComponentUtils::WriteComponentsAsSeparateVolumes( vxBlockVolume<uint2> volIntensity, 
                                                         vxBlockVolume<uint2> volLabel, 
                                                         int4 iNumLabels )
{

  for (int4 iLabel(0);  iLabel<iNumLabels;  ++iLabel)
  {
    vxBlockVolume<uint2> vol(volIntensity.GetHeader());
    vxBlockVolumeIterator<uint2> iterLabel(volLabel);
    vxBlockVolumeIterator<uint2> iterVolSrc(volIntensity);
    vxBlockVolumeIterator<uint2> iterVolDest(vol);

    for (; iterVolDest.IsNotAtEnd(); iterVolDest.NextBlockZYX())
    {
      for (; iterVolDest.IsNotAtEndOfBlock(); iterVolDest.NextXZYinsideBlock())
      {
        if (iterLabel.GetVoxel(iterVolDest) == iLabel)
        {
          iterVolDest.SetVoxel(iterVolSrc.GetVoxel(iterVolDest));
        }
      }
    } // for 

    char sFilename[50];
    sprintf(sFilename, "D:/ds/label_%02i.vol", iLabel);
    vol.Write(sFilename);

  } // for all labels

} // WriteComponentsAsSeparateVolumes()

 
/**
 * Create a new component given a bool volume and the environment.
 * It will respect the component manager's mask and its add/create option.
 *
 * @param desiredVoxels               voxels which are desired to be part of the new component
 * @param sName                       name of the new component
 * @param pComponentArray             the components object
 * @param pLabelVolume                the label volume object
 * @param pLavelVolumeBinaryHistogram the label volume binary histogram
 * @param interactiveSegmentation     the interactive segmentation object
 * @param bTestForLockedVoxels        whether to test for locked voxels
 * @return                            new label index >0 upon success, 0 on failure
 */
uint2 vxComponentUtils::CreateNewComponent( vxBlockVolume<bool> & desiredVoxels, const std::string & sName,
                                            vxShareableObject< vxComponentArray > * pComponentArray,
                                            vxShareableObject< vxBlockVolume<uint2> > * pLabelVolume,
                                            InteractiveSegmentation & interactiveSegmentation, bool bTestForLockedVoxels )
{
  // Do we need to create a new component?
  uint2 uHistoryIndex( 0 );
  uint2 uLabel( 0 );
  const bool bNewRegions( interactiveSegmentation.GetRegionGrowAddOpt() == SegParams::NEW_REGIONS );
  const bool bAppendingToNone( pComponentArray->GetSelectedComponentIndex() == 0 && interactiveSegmentation.GetRegionGrowAddOpt()  == SegParams::APPEND_REGIONS );

  // make sure we are not appending to the background component
  if ( bNewRegions || bAppendingToNone )
  {
    // name, add, and lock it
    if ( pComponentArray->GetCount() + 1 >= MAXN_COMP )
    {
      throw ex::OutOfMemoryException( LogRec( "Maximum number of components reached.", "ComponentHandler", "AddCompReg" ) );
    }
    uHistoryIndex = pComponentArray->AddComponent( sName, ComponentColors::GetUniqueColor( pComponentArray ) );
    uLabel = ( * pComponentArray )[ uHistoryIndex ]->GetLabel();

  } // if new region
  else
  {

    uHistoryIndex = interactiveSegmentation.GetCompHdlrPtr()->GetSelectedComponentIndex();
    uLabel = ( * pComponentArray )[ uHistoryIndex ]->GetLabel();

  }

  //pre-compute the component lock information
  bool * pbLocked = new bool[ pComponentArray->ArrayByLabel().size() ]; 
  for ( int i=0; i< pComponentArray->ArrayByLabel().size(); i++ )
    pbLocked[ i ] = pComponentArray->ArrayByLabel()[ i ]->IsLocked();

  // copy over into the label volume
  vxBlockVolumeIterator<bool> desiredVolIter( desiredVoxels );
  vxBlockVolumeIterator<uint2> labelIter( * pLabelVolume );
  labelIter.SetClearBlock( uLabel );

  for ( desiredVolIter.FirstNonBackgroundBlockZYX(); desiredVolIter.IsNotAtEnd(); desiredVolIter.NextNonBackgroundBlockZYX() )
  {
    if( desiredVolIter.GetCurrentBlock().IsHomogenous() && labelIter.GetCurrentBlock().IsHomogenous()  )
    {
			//check to see if this is a non-zero voxel
			if (  desiredVolIter.GetVoxel() )
			{
				labelIter.SetPos( desiredVolIter );
				if( !bTestForLockedVoxels || !pbLocked[ labelIter.GetVoxel() ] )
				{
					labelIter.ClearCurrentBlock();
				}
			}
    } // if
    else
    {
      for ( desiredVolIter.FirstNonBackgroundZYXinsideBlock(); desiredVolIter.IsNotAtEndOfBlock(); desiredVolIter.NextNonBackgroundZYXinsideBlock() )
      {
        labelIter.SetPos( desiredVolIter );
        // check for locked voxels
        if ( !bTestForLockedVoxels || !pbLocked[ labelIter.GetVoxel() ] )
        {
          labelIter.SetVoxel( uLabel );
        }
      }
    } // else
  } // for all non background blocks

  // let everyone know that the volume has changed
  pLabelVolume->Modified( vxModInfo( vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_MODIFIED ) );
  
  if ( bNewRegions )
    pComponentArray->Modified( vxModIndInfo( vxComponentArray, uHistoryIndex, vxUtils::COMPONENTARRAY_ADDED ) );
  else
    pComponentArray->Modified( vxModIndInfo( vxComponentArray, uHistoryIndex, vxUtils::COMPONENTARRAY_MODIFIED ) );

  return uLabel;
} // CreateNewComponent()


/**
 * Create a new component given the environment with empty ROI that will be filled later.
 * It will return the new component label index, but will not update the label volume 
 * and componentArray
 *
 * @param sName                       name of the new component
 * @param environment                 the environment
 * @return                            new label index >0 upon success, 0 on failure
 */
uint2 vxComponentUtils::CreateNewEmptyComponent( const std::string & sName, vxEnvironment & environment )
{
  vxShareableObject< vxComponentArray > * pComponentArray         = environment.GetComponents();
  //vxShareableObject<InteractiveSegmentation> interactiveSegmentation = * environment.GetInteractiveSegmentation();

  // Do we need to create a new component?
  uint2 uHistoryIndex( 0 );
  uint2 uLabel( 0 );
  const bool bNewRegions( environment.GetInteractiveSegmentation()->GetRegionGrowAddOpt() == SegParams::NEW_REGIONS );
  const bool bAppendingToNone( pComponentArray->GetSelectedComponentIndex() == 0 && environment.GetInteractiveSegmentation()->GetRegionGrowAddOpt()  == SegParams::APPEND_REGIONS );

  // make sure we are not appending to the background component
  if ( bNewRegions || bAppendingToNone )
  {
    // name, add, and lock it
    if ( pComponentArray->GetCount() + 1 >= MAXN_COMP )
    {
      throw ex::OutOfMemoryException( LogRec( "Maximum number of components reached.", "ComponentHandler", "AddCompReg" ) );
    }
    uHistoryIndex = pComponentArray->AddComponent( sName, ComponentColors::GetUniqueColor( pComponentArray ) );
    uLabel = ( * pComponentArray )[ uHistoryIndex ]->GetLabel();

  } // if new region
  else
  {
    uHistoryIndex = environment.GetInteractiveSegmentation()->GetCompHdlrPtr()->GetSelectedComponentIndex();
    uLabel = ( * pComponentArray )[ uHistoryIndex ]->GetLabel();
  } // else

  return uLabel;
} // CreateNewEmptyComponent



/**
 * Create a new component given a bool volume and the environment.
 * It will respect the component manager's mask and its add/create option.
 *
 * @param desiredVoxels        voxels which are desired to be part of the new component
 * @param sName                name of the new component
 * @param environment          the environment
 * @param bTestForLockedVoxels whether to test for locked voxels
 * @return                     new label index >0 upon success, 0 on failure
 */
uint2 vxComponentUtils::CreateNewComponent( vxBlockVolume<bool> & desiredVoxels, const std::string & sName,
                                            vxEnvironment & environment, bool bTestForLockedVoxels )
{
  return CreateNewComponent( desiredVoxels, sName, environment.GetComponents(), environment.GetLabelVolume(), * environment.GetInteractiveSegmentation(), bTestForLockedVoxels );
} // CreateNewComponent()


/**
 * Create a unique name in the component list
 *
 * @param componentArray   component array
 * @param sPrefix   name prefix
 * @return    a unique component name
 */
std::string vxComponentUtils::GetUniqueComponentName( vxComponentArray & componentArray, 
                                                      const std::string & sPrefix )
{
  std::string sComponentName("");
  const uint4 ufMaxTries(256*256-1);
  uint4 uTryCount(1);

  // loop over all the possible names
  while (sComponentName.empty() && uTryCount<ufMaxTries)
  {
    // propose a name
    const string sName(sPrefix + " " + ToAscii(uTryCount));

    // loop over all components
    bool bAnyHave(false);

    for (uint4 uComponentIndex(0); uComponentIndex < componentArray.GetCount(); ++uComponentIndex)
    {
      std::string sExistingName(componentArray[ uComponentIndex ]->GetName());
      if (sExistingName == sName)
      {
        bAnyHave = true;
      }
    } // for 

    if (bAnyHave == false)
    {
      sComponentName = sName;
    }

    uTryCount++;
  }

  if (sComponentName.empty())
  {
    sComponentName = sPrefix + "...";
  }

  return sComponentName;
} // GetUniqueComponentName()


/*
 * Get the component Index by name
 *
 *  @param componentArray input component array
 *  @param sName the component name
 *  @return component index
 */
int4 vxComponentUtils::GetComponentIndex( vxComponentArray & componentArray, const std::string & sName )
{
	for (uint4 uComponentIndex(0); uComponentIndex < componentArray.GetCount(); ++uComponentIndex)
	{
		std::string sExistingName(componentArray[ uComponentIndex ]->GetName());

		if (sExistingName == sName)
			return uComponentIndex;
	}
	
	LogErr( "Unable to detect a component Index which has name " + sName , "vxComponentUtils", "GetComponentIndex" );
	return -1;
}//GetComponentIndex( vxComponentArray & componentArray, const std::string & sName )


/*
 * Get the component object by name
 *
 *  @param componentArray the input component array
 *  @param sName the name of the component
 *  @return the vxComponent
 */
vxComponent * vxComponentUtils::GetComponent( vxComponentArray & componentArray, const std::string & sName )
{
	for (uint4 uComponentIndex(0); uComponentIndex < componentArray.GetCount(); ++uComponentIndex)
	{
		std::string sExistingName(componentArray[ uComponentIndex ]->GetName());

		if (sExistingName == sName)
		{
			vxComponent * pComponent = componentArray[ uComponentIndex ];
			return pComponent;
		}
	}
	LogErr( "Unable to detect a component which has name " + sName , "vxComponentUtils", "GetComponent" );
	return NULL;
}//GetComponent( vxComponentArray & componentArray, const std::string & sName )
	

/**
 * Creates a mask volume to indicate locked voxels
 *
 * @param outMaskVolume a volume where true indicates the voxels is locked
 * @param labelVolume   the label volume (must be the same dimensions as the mask volume)
 * @param components    the component array
 */
void vxComponentUtils::CalculateLockMask( vxBlockVolume<bool> & outMaskVolume, vxBlockVolume<uint2> & labelVolume,
                                          const vxComponentArray & components )
{

  // track the time it takes
  Timer timer;

  // clear the volume
  vxBlockVolumeIterator<bool> lockIter( outMaskVolume );
  lockIter.SetClearBlock( false );
  lockIter.ClearCurrentBlock();
  lockIter.SetClearBlock( true );

  // pre-compute the component lock information
  bool * pbLocked = new bool[ components.ArrayByLabel().size() ];
  for ( int i=0; i<components.ArrayByLabel().size(); i++ )
    pbLocked[ i ] = components.ArrayByLabel()[ i ]->IsLocked();

  // traverse all the voxels
  vxBlockVolumeIterator<uint2> labelIter( labelVolume );
  for ( ; labelIter.IsNotAtEnd(); labelIter.NextBlockZYX() )
  {

    // efficiently compute homogenous blocks
    if ( labelIter.GetCurrentBlock().IsHomogenous() )
    {
      if ( pbLocked[ labelIter.GetVoxel() ] )
      {
        labelIter.ClearCurrentBlock();
      }
    }
    else
    {

      // laboriously compute non-homogenous blocks
      lockIter.SetPos( labelIter.GetPos() );
      for ( ; labelIter.IsNotAtEndOfBlock(); labelIter.NextZYXinsideBlock(), lockIter.NextZYXinsideBlock() )
      {
        const uint2 & labelValue = labelIter.GetVoxel();
        if ( pbLocked[ labelValue ] )
        {
          lockIter.SetVoxel( true );
        }
      } // end for all voxels in block
    } // non-homogenous
  } // end for all blocks

  // clean up memory
  delete [] pbLocked;

  LogDbg( "Time to calculate lock mask: " + timer.AsString2(), "vxComponentUtils", "CalculateLockMask" );

} // CalculateLockMask()


/**
 * Inverts all the voxels in the bool volume
 *
 * @param invertVolume the bool volume whose voxels will be inverted
 */
void vxComponentUtils::InvertVolume( vxBlockVolume<bool> & invertVolume )
{
  
  // traverse all the voxels
  vxBlockVolumeIterator<bool> iter( invertVolume );
  for ( ; iter.IsNotAtEnd(); iter.NextBlockZYX() )
  {

    // efficiently compute homogenous blocks
    if ( iter.GetCurrentBlock().IsHomogenous() )
    {
      iter.SetClearBlock( ! iter.GetVoxel() );
      iter.ClearCurrentBlock();
    }

    // laboriously compute non-homogenous blocks
    else
    {
      for ( ; iter.IsNotAtEndOfBlock(); iter.NextZYXinsideBlock() )
      {
        iter.SetVoxel( ! iter.GetVoxel() );
      } // end for all voxels in block
    } // non-homogenous block

  } // end for all blocks

} // InvertVolume()


/**
 * Calculates if both volumes have true for a given voxel. The result is placed in volumeA.
 * The second volume can be inverted during testing - so that A will be set to true if A is true and B is false.
 *
 * @param volumeA  the first volume (also the result volume)
 * @param volumeB  the second volume
 * @param bInvertB if the second volume values should be inverted prior during testing
 */
void vxComponentUtils::GetIntersection( vxBlockVolume<bool> & a, vxBlockVolume<bool> & b, const bool & bInvertB )
{

  vxBlockVolumeIterator<bool> iterA( a );
  vxBlockVolumeIterator<bool> iterB( b );
  for ( iterA.FirstNonBackgroundBlockZYX(); iterA.IsNotAtEnd(); iterA.NextNonBackgroundBlockZYX() )
  {
    for ( iterA.FirstNonBackgroundZYXinsideBlock(); iterA.IsNotAtEndOfBlock(); iterA.NextNonBackgroundZYXinsideBlock() )
    {

      iterB.SetPos( iterA.GetPos() );
      if ( iterB.GetVoxel() == bInvertB )
      {
        iterA.SetVoxel( false );
      }

    }
  } // for all non background blocks

} // GetIntersection()


/**
 * Cull voxels outside of the given bounding box
 *
 * @param volume  the volume to cull (result is returned here as well)
 * @param bounds  the voxels to keep
 */
void vxComponentUtils::CullOutside( vxBlockVolume<bool> & volume, const Box<uint4> & bounds )
{

  vxBlockVolumeIterator<bool> iter( volume );
  for ( iter.FirstNonBackgroundBlockZYX(); iter.IsNotAtEnd(); iter.NextNonBackgroundBlockZYX() )
  {
    for ( iter.FirstNonBackgroundZYXinsideBlock(); iter.IsNotAtEndOfBlock(); iter.NextNonBackgroundZYXinsideBlock() )
    {
      if ( ! bounds.IsInside( Triplei( iter.GetPos() ) ) )
        iter.SetVoxel( false );
    }
  }

} // CullOutside()


/**
 * Dilate a boolean mask volume by one voxel in all directions based on the given connectivity
 *
 * @param maskVolume    the input/outpu mask volume
 * @param eNeighborhood the desired connectivity
 */
void vxComponentUtils::DilateMask( vxBlockVolume<bool> & maskVolume, vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum eNeighborhood )
{

  // copy the mask to a temp volume
  vxBlockVolume<bool> tempVolume( maskVolume );
  vxBlockVolumeIterator<bool> maskIterator( maskVolume );
  vxBlockVolumeIterator<bool> tempIterator( tempVolume );

  vxBlockVolumeIteratorNeighbors<bool> neighborIter( maskVolume, Vector3D<int4>( 0, 0, 0 ), eNeighborhood );

  // loop over blocks
  for ( maskIterator.FirstNonBackgroundBlockZYX(); maskIterator.IsNotAtEnd(); maskIterator.NextNonBackgroundBlockZYX() )
  {
    // loop within block
    for ( maskIterator.FirstNonBackgroundZYXinsideBlock(); maskIterator.IsNotAtEndOfBlock(); maskIterator.NextNonBackgroundZYXinsideBlock() )
    {
      for ( neighborIter.CenterAt( maskIterator.GetPos() ); ! neighborIter.IsAtEnd(); ++neighborIter )
      {
        if ( neighborIter.GetVoxel() == false )
        {
          tempIterator.SetPos( neighborIter.GetPos() );
          tempIterator.SetVoxel( true );
        }
      } // for all neighborhood voxels
    } // for all voxels within block
  } // for all blocks

  maskVolume = tempVolume;

} // DilateMask()


/**
 * Calculates the volume of the component.
 *
 * @param   labelVolume       the label volume being checked
 * @param   component         the component being calculated
 * @return                    the volume of the component
 */
float8 vxComponentUtils::CalculateComponentVolume( vxBlockVolume< uint2 > & labelVolume, const vxComponent & component )
{
  uint8 uCount( 0 );
  vxBlockVolumeIterator<uint2> labelIter( labelVolume );
  const int4 iBlockSize = labelIter.GetCurrentBlock().GetSize();

  // count the voxels for the component
  for( labelIter.FirstNonBackgroundBlockZYX(); labelIter.IsNotAtEnd(); labelIter.NextNonBackgroundBlockZYX() )
  {
    // count entire blocks matching the compoent
    if( labelIter.GetCurrentBlock().IsHomogenous() )
    {
      if( labelIter.GetVoxel() == component.GetLabel() )
        uCount += iBlockSize;
    }
    else
    {
      // count the individual voxels matching the component
      for( labelIter.FirstNonBackgroundZYXinsideBlock(); labelIter.IsNotAtEndOfBlock(); labelIter.NextNonBackgroundZYXinsideBlock() )
      {
        if( labelIter.GetVoxel() == component.GetLabel() )
          uCount++;
      }
    }
  }

  // calculate the volume in voxel units
  const Triple<float4> & voxelUnits = labelVolume.GetHeader().GetVoxelUnits();
  return static_cast< float8 >( uCount ) * voxelUnits.m_x * voxelUnits.m_y * voxelUnits.m_z;
} // CalculateComponentVolume( labelVolume, component )


/**
 * Returns a vector of contour points lists (in voxel coordinates)
 *
 * @param   labelVolume   mask volume
 * @param   plane         plane definition
 * @param   mode          rendering mode
 * @param   components    component array
 * @param   pointsVC      vector of contour point lists ( one list for each component that is intersected by the specified plane )
 */
void vxComponentUtils::GetComponentContourLists( vxBlockVolume< uint2 > & labelVolume,  
                                                 const vxPlaneOblique< float4 > & plane, 
                                                 const vxRenderingMode & mode,
                                                 const vxComponentArray & components,
                                                 std::vector< std::list< Vector3Di > > & pointsVC )
{ 
  if( !components.AreAnyLabelsVisible() || mode.GetRenderingAlgorithm() == vxRenderingMode::XRAY )
    return;  // no component is visible

  // volume dimension and voxel spacing
  const Triple< int > viDim  ( labelVolume.GetHeader().GetVolDim()     );
  const Triple<float> vfUnits( labelVolume.GetHeader().GetVoxelUnits() );

  // oblique plane information
  const Normal3D<float> planeNormal( plane.GetNormal()   );
  const float           fDistance  ( plane.GetDistance() );

  std::vector< uint2 > vuVisibleComponentLabelList;

  register uint2 u;
  // collect all visible component labels
  for( u=0; u<components.ArrayByLabel().size(); u++ )
  {
    if( components.ArrayByLabel()[u]->IsLabelVisible() )
      vuVisibleComponentLabelList.push_back( components.ArrayByLabel()[u]->GetLabel() );
  } // for u

  pointsVC.empty();
  const int iNumOfVisibleComponents( vuVisibleComponentLabelList.size() );
  std::list< Vector3Di > vList;
  for( u=0; u<iNumOfVisibleComponents; u++ )
    pointsVC.push_back( vList );

  vxBlockVolumeIterator<uint2> it( labelVolume );

  register int x, y, z;
  register float f;
  register uint2 uv;  

  if( planeNormal.Z() == 1.0F || planeNormal.Z() == -1.0F )
  { // orthogonal XY plane
    f = fDistance/vfUnits.m_z;

    // determine the plane location
    int iz = int(f);
    if( f-int(f) > 0.5F )
      iz++;

    uint2 * puSlice = new uint2[viDim.m_x*viDim.m_y];
    if( puSlice == NULL )
      throw ex::OutOfMemoryException(LogRec("No memory for temporary slice data", "vxComponentUtils", "GetComponentContourLists"));

    it.SetPosZ( iz );
    for( y=0, it.SetPosY(0); y<viDim.m_y; y++, it.IncY() )
    {
      for( x=0, it.SetPosX(0); x<viDim.m_x; x++, it.IncX() )
      {
        if( it.GetVoxel() > 0 )
        {
          uv = it.GetVoxel();

          for( u=0; u<iNumOfVisibleComponents; u++ )
          {
            if( uv == vuVisibleComponentLabelList[u] )
            {
              if( it.PeekRel(1,0,0)   != uv ) pointsVC[u].push_back( Vector3Di(x+1, y, iz)   );
              if( it.PeekRel(1,1,0)   != uv ) pointsVC[u].push_back( Vector3Di(x+1, y+1, iz) );
              if( it.PeekRel(1,-1,0)  != uv ) pointsVC[u].push_back( Vector3Di(x+1, y-1, iz) );
              if( it.PeekRel(0,1,0)   != uv ) pointsVC[u].push_back( Vector3Di(x, y+1, iz)   );
              if( it.PeekRel(-1,0,0)  != uv ) pointsVC[u].push_back( Vector3Di(x-1, y, iz)   );
              if( it.PeekRel(-1,1,0)  != uv ) pointsVC[u].push_back( Vector3Di(x-1, y+1, iz) );
              if( it.PeekRel(-1,-1,0) != uv ) pointsVC[u].push_back( Vector3Di(x-1, y-1, iz) );
              if( it.PeekRel(0,-1,0)  != uv ) pointsVC[u].push_back( Vector3Di(x, y-1, iz)   );

              break;
            } // if
          } // u
        } // if component voxel
      } // for x
    } // for y
  } // end orthogonal XY plane

  else if( planeNormal.X() == 1.0F || planeNormal.X() == -1.0F )
  { // orthogonal YZ plane
    f = fDistance/vfUnits.m_x;

    // determine the plane location
    int ix = int(f);
    if( f-int(f) > 0.5F )
      ix++;

    uint2 * puSlice = new uint2[viDim.m_z*viDim.m_y];
    if( puSlice == NULL )
      throw ex::OutOfMemoryException(LogRec("No memory for temporary slice data", "vxComponentUtils", "GetComponentContourLists"));

    it.SetPosX( ix );
    for( z=0, it.SetPosZ(0); z<viDim.m_z; z++, it.IncZ() )
    {
      for( y=0, it.SetPosY(0); y<viDim.m_y; y++, it.IncY() )
      {
        if( it.GetVoxel() > 0 )
        {
          uv = it.GetVoxel();

          for( u=0; u<iNumOfVisibleComponents; u++ )
          {
            if( uv == vuVisibleComponentLabelList[u] )
            {
              if( it.PeekRel(0,1,0)   != uv ) pointsVC[u].push_back( Vector3Di(ix, y+1, z)   );
              if( it.PeekRel(0,1,1)   != uv ) pointsVC[u].push_back( Vector3Di(ix, y+1, z+1) );
              if( it.PeekRel(0,1,-1)  != uv ) pointsVC[u].push_back( Vector3Di(ix, y+1, z-1) );
              if( it.PeekRel(0,0,1)   != uv ) pointsVC[u].push_back( Vector3Di(ix, y, z+1)   );
              if( it.PeekRel(0,-1,0)  != uv ) pointsVC[u].push_back( Vector3Di(ix, y-1, z)   );
              if( it.PeekRel(0,-1,1)  != uv ) pointsVC[u].push_back( Vector3Di(ix, y-1, z+1) );
              if( it.PeekRel(0,-1,-1) != uv ) pointsVC[u].push_back( Vector3Di(ix, y-1, z-1) );
              if( it.PeekRel(0,0,-1)  != uv ) pointsVC[u].push_back( Vector3Di(ix, y, z-1)   );

              break;
            } // if
          } // u
        } // if component voxel
      } // for y
    } // for z
  } // end of orthogonal YZ plane

  else if( planeNormal.Y() == 1.0F || planeNormal.Y() == -1.0F )
  { // orthogonal XZ plane
    f = fDistance/vfUnits.m_y;

    // determine the plane location
    int iy = int(f);
    if( f-int(f) > 0.5F )
      iy++;

    uint2 * puSlice = new uint2[viDim.m_z*viDim.m_x];
    if( puSlice == NULL )
      throw ex::OutOfMemoryException(LogRec("No memory for temporary slice data", "vxComponentUtils", "GetComponentContourLists"));

    it.SetPosX( iy );
    for( z=0, it.SetPosZ(0); z<viDim.m_z; z++, it.IncZ() )
    {
      for( x=0, it.SetPosX(0); x<viDim.m_x; x++, it.IncX() )
      {
        if( it.GetVoxel() > 0 )
        {
          uv = it.GetVoxel();

          for( u=0; u<iNumOfVisibleComponents; u++ )
          {
            if( uv == vuVisibleComponentLabelList[u] )
            {
              if( it.PeekRel(1,0,0)   != uv ) pointsVC[u].push_back( Vector3Di(x+1, iy, z)   );
              if( it.PeekRel(1,0,1)   != uv ) pointsVC[u].push_back( Vector3Di(x+1, iy, z+1) );
              if( it.PeekRel(1,0,-1)  != uv ) pointsVC[u].push_back( Vector3Di(x+1, iy, z-1) );
              if( it.PeekRel(0,0,1)   != uv ) pointsVC[u].push_back( Vector3Di(x, iy, z+1)   );
              if( it.PeekRel(-1,0,0)  != uv ) pointsVC[u].push_back( Vector3Di(x-1, iy, z)   );
              if( it.PeekRel(-1,0,1)  != uv ) pointsVC[u].push_back( Vector3Di(x-1, iy, z+1) );
              if( it.PeekRel(-1,0,-1) != uv ) pointsVC[u].push_back( Vector3Di(x-1, iy, z-1) );
              if( it.PeekRel(0,0,-1)  != uv ) pointsVC[u].push_back( Vector3Di(x, iy, z-1)   );

              break;
            } // if
          } // u
        } // if component voxel
      } // for x
    } // for z
  } // end of orthogonal XZ plane

  else 
  { // true oblique 
    const Vector3D<float4> vfRefPtOnObliquePlane(planeNormal.X()*fDistance, planeNormal.Y()*fDistance, planeNormal.Z()*fDistance);
    
    Vector3D<float4> obliquePlaneAxis_1(0.0F,0.0F,0.0F);
    Vector3D<float4> obliquePlaneAxis_2(0.0F,0.0F,0.0F);

    /// computing the axis directions vector for oblique plane voxel grid
    if( planeNormal.X() <= planeNormal.Y() && planeNormal.X() <= planeNormal.Z() )
    { // if X direction has biggest step
      if( planeNormal.Y() != 0.0F )
      {
        obliquePlaneAxis_1.m_x = vfUnits.m_x;
        obliquePlaneAxis_1.m_y = -vfUnits.m_x*planeNormal.X()/planeNormal.Y();
        obliquePlaneAxis_1.m_z = 0.0F;
      }
      else
      {
        obliquePlaneAxis_1.m_x = vfUnits.m_x;
        obliquePlaneAxis_1.m_y = 0.0F;
        obliquePlaneAxis_1.m_z = -vfUnits.m_x*planeNormal.X()/planeNormal.Z();
      }

      /// compute the 2nd axis direction vector for the oblique plane grid (cross-product of planeNormal and obliquePlaneAxis_1)
      obliquePlaneAxis_2.m_x = planeNormal.Y()*obliquePlaneAxis_1.m_z - planeNormal.Z()*obliquePlaneAxis_1.m_y;
      obliquePlaneAxis_2.m_y = planeNormal.Z()*obliquePlaneAxis_1.m_x - planeNormal.X()*obliquePlaneAxis_1.m_z;
      obliquePlaneAxis_2.m_z = planeNormal.X()*obliquePlaneAxis_1.m_y - planeNormal.Y()*obliquePlaneAxis_1.m_x;

      f = sqrt( obliquePlaneAxis_2.m_x*obliquePlaneAxis_2.m_x + obliquePlaneAxis_2.m_y*obliquePlaneAxis_2.m_y + 
                obliquePlaneAxis_2.m_z*obliquePlaneAxis_2.m_z );

      if( planeNormal.Y() < planeNormal.Z() )
        obliquePlaneAxis_2 = obliquePlaneAxis_2*vfUnits.m_y/f;
      else
        obliquePlaneAxis_2 = obliquePlaneAxis_2*vfUnits.m_z/f;
    } // if X direction has biggest step

    else if( planeNormal.Y() <= planeNormal.X() && planeNormal.Y() <= planeNormal.Z() )
    { // if Y has the biggest step
      if( planeNormal.X() != 0.0F )
      {
        obliquePlaneAxis_1.m_x = -vfUnits.m_y*planeNormal.Y()/planeNormal.X();
        obliquePlaneAxis_1.m_y = vfUnits.m_y;
        obliquePlaneAxis_1.m_z = 0.0F;
      }
      else
      {
        obliquePlaneAxis_1.m_x = 0.0F;
        obliquePlaneAxis_1.m_y = vfUnits.m_y;
        obliquePlaneAxis_1.m_z = -vfUnits.m_y*planeNormal.Y()/planeNormal.Z();
      }

      /// compute the 2nd axis direction vector for the oblique plane grid (cross-product of planeNormal and obliquePlaneAxis_1)
      obliquePlaneAxis_2.m_x = planeNormal.Y()*obliquePlaneAxis_1.m_z - planeNormal.Z()*obliquePlaneAxis_1.m_y;
      obliquePlaneAxis_2.m_y = planeNormal.Z()*obliquePlaneAxis_1.m_x - planeNormal.X()*obliquePlaneAxis_1.m_z;
      obliquePlaneAxis_2.m_z = planeNormal.X()*obliquePlaneAxis_1.m_y - planeNormal.Y()*obliquePlaneAxis_1.m_x;

      f = sqrt( obliquePlaneAxis_2.m_x*obliquePlaneAxis_2.m_x + obliquePlaneAxis_2.m_y*obliquePlaneAxis_2.m_y + 
                obliquePlaneAxis_2.m_z*obliquePlaneAxis_2.m_z );

      if( planeNormal.X() < planeNormal.Z() )
        obliquePlaneAxis_2 = obliquePlaneAxis_2*vfUnits.m_x/f;
      else
        obliquePlaneAxis_2 = obliquePlaneAxis_2*vfUnits.m_z/f;
    } // if Y has the biggest step

    else
    { // if Z has the biggest step
      if( planeNormal.X() != 0.0F )
      {
        obliquePlaneAxis_1.m_x = -vfUnits.m_z*planeNormal.Z()/planeNormal.X();
        obliquePlaneAxis_1.m_y = 0.0F;
        obliquePlaneAxis_1.m_z = vfUnits.m_z;
      }
      else
      {
        obliquePlaneAxis_1.m_x = 0.0F;
        obliquePlaneAxis_1.m_y = -vfUnits.m_z*planeNormal.Z()/planeNormal.Y();
        obliquePlaneAxis_1.m_z = vfUnits.m_z;
      }

      /// compute the 2nd axis direction vector for the oblique plane grid (cross-product of planeNormal and obliquePlaneAxis_1)
      obliquePlaneAxis_2.m_x = planeNormal.Y()*obliquePlaneAxis_1.m_z - planeNormal.Z()*obliquePlaneAxis_1.m_y;
      obliquePlaneAxis_2.m_y = planeNormal.Z()*obliquePlaneAxis_1.m_x - planeNormal.X()*obliquePlaneAxis_1.m_z;
      obliquePlaneAxis_2.m_z = planeNormal.X()*obliquePlaneAxis_1.m_y - planeNormal.Y()*obliquePlaneAxis_1.m_x;

      f = sqrt( obliquePlaneAxis_2.m_x*obliquePlaneAxis_2.m_x + obliquePlaneAxis_2.m_y*obliquePlaneAxis_2.m_y + 
                obliquePlaneAxis_2.m_z*obliquePlaneAxis_2.m_z );

      if( planeNormal.Y() < planeNormal.X() )
        obliquePlaneAxis_2 = obliquePlaneAxis_2*vfUnits.m_y/f;
      else
        obliquePlaneAxis_2 = obliquePlaneAxis_2*vfUnits.m_x/f;
    } // if Z has the biggest step

    /// determine bounding box along the oblique grid
    int iB1( max( abs(float(viDim.m_x)*vfUnits.m_x-vfRefPtOnObliquePlane.m_x)/abs(obliquePlaneAxis_1.m_x),
              max(abs(vfRefPtOnObliquePlane.m_x)/abs(obliquePlaneAxis_1.m_x),
              max(abs(float(viDim.m_y)*vfUnits.m_y-vfRefPtOnObliquePlane.m_y)/abs(obliquePlaneAxis_1.m_y),
              max(abs(vfRefPtOnObliquePlane.m_y)/abs(obliquePlaneAxis_1.m_y),
              max(abs(float(viDim.m_z)*vfUnits.m_z-vfRefPtOnObliquePlane.m_z)/abs(obliquePlaneAxis_1.m_z),
                  abs(vfRefPtOnObliquePlane.m_z)/abs(obliquePlaneAxis_1.m_z))))) ));

    int iB2( max( abs(float(viDim.m_x)*vfUnits.m_x-vfRefPtOnObliquePlane.m_x)/abs(obliquePlaneAxis_2.m_x),
              max(abs(vfRefPtOnObliquePlane.m_x)/abs(obliquePlaneAxis_2.m_x),
              max(abs(float(viDim.m_y)*vfUnits.m_y-vfRefPtOnObliquePlane.m_y)/abs(obliquePlaneAxis_2.m_y),
              max(abs(vfRefPtOnObliquePlane.m_y)/abs(obliquePlaneAxis_2.m_y),
              max(abs(float(viDim.m_z)*vfUnits.m_z-vfRefPtOnObliquePlane.m_z)/abs(obliquePlaneAxis_2.m_z),
                  abs(vfRefPtOnObliquePlane.m_z)/abs(obliquePlaneAxis_2.m_z))))) ));

    register Vector3D<float4> vfPt1(vfRefPtOnObliquePlane-float(iB1)*obliquePlaneAxis_1);
    register Vector3D<float4> vfPt2(0.0F,0.0F,0.0F);
    register Vector3D< int2 > vPt(0,0,0);
    const int iX(viDim.m_x-1);
    const int iY(viDim.m_y-1);
    const int iZ(viDim.m_z-1);
    /// find all the contour points in the oblique plane
    for( y=-iB1; y<=iB1; y++ )
    {
      vfPt2 = vfPt1-float(iB2)*obliquePlaneAxis_2;

      for( x=-iB2; x<=iB2; x-- )
      {
        vPt.m_x = int2(vfPt2.m_x);
        vPt.m_y = int2(vfPt2.m_y);
        vPt.m_z = int2(vfPt2.m_z);

        if( vPt.m_x > 0 && vPt.m_x < iX && vPt.m_y > 0 && vPt.m_y < iY &&
            vPt.m_z > 0 && vPt.m_z < iZ )
        { // point is in the volume
          it.SetPos(vPt);

          if( it.GetVoxel() > 0 )
          {
            uv = it.GetVoxel();

            for( u=0; u<iNumOfVisibleComponents; u++ )
            {
              if( uv == vuVisibleComponentLabelList[u] )
              {
                if( it.PeekRel(1,0,0)   != uv ) pointsVC[u].push_back( vPt+Vector3Di(1,0,0) );
                if( it.PeekRel(-1,0,0)  != uv ) pointsVC[u].push_back( vPt+Vector3Di(-1,0,0) );
                if( it.PeekRel(1,1,0)   != uv ) pointsVC[u].push_back( vPt+Vector3Di(1,1,0) );
                if( it.PeekRel(1,-1,0)  != uv ) pointsVC[u].push_back( vPt+Vector3Di(1,-1,0) );
                if( it.PeekRel(0,1,0)   != uv ) pointsVC[u].push_back( vPt+Vector3Di(0,1,0) );
                if( it.PeekRel(-1,1,0)  != uv ) pointsVC[u].push_back( vPt+Vector3Di(-1,1,0) );
                if( it.PeekRel(-1,-1,0) != uv ) pointsVC[u].push_back( vPt+Vector3Di(-1,-1,0) );
                if( it.PeekRel(0,-1,0)  != uv ) pointsVC[u].push_back( vPt+Vector3Di(0,-1,0) );

                if( it.PeekRel(0,0,1)   != uv ) pointsVC[u].push_back( vPt+Vector3Di(0,0,1) );
                if( it.PeekRel(1,0,1)   != uv ) pointsVC[u].push_back( vPt+Vector3Di(1,0,1) );
                if( it.PeekRel(-1,0,1)  != uv ) pointsVC[u].push_back( vPt+Vector3Di(-1,0,1) );
                if( it.PeekRel(1,1,1)   != uv ) pointsVC[u].push_back( vPt+Vector3Di(1,1,1) );
                if( it.PeekRel(1,-1,1)  != uv ) pointsVC[u].push_back( vPt+Vector3Di(1,-1,1) );
                if( it.PeekRel(0,1,1)   != uv ) pointsVC[u].push_back( vPt+Vector3Di(0,1,1) );
                if( it.PeekRel(-1,1,1)  != uv ) pointsVC[u].push_back( vPt+Vector3Di(-1,1,1) );
                if( it.PeekRel(-1,-1,1) != uv ) pointsVC[u].push_back( vPt+Vector3Di(-1,-1,1) );
                if( it.PeekRel(0,-1,1)  != uv ) pointsVC[u].push_back( vPt+Vector3Di(0,-1,1) );

                if( it.PeekRel(0,0,-1)   != uv ) pointsVC[u].push_back( vPt+Vector3Di(0,0,-1) );
                if( it.PeekRel(1,0,-1)   != uv ) pointsVC[u].push_back( vPt+Vector3Di(1,0,-1) );
                if( it.PeekRel(-1,0,-1)  != uv ) pointsVC[u].push_back( vPt+Vector3Di(-1,0,-1) );
                if( it.PeekRel(1,1,-1)   != uv ) pointsVC[u].push_back( vPt+Vector3Di(1,1,-1) );
                if( it.PeekRel(1,-1,-1)  != uv ) pointsVC[u].push_back( vPt+Vector3Di(1,-1,-1) );
                if( it.PeekRel(0,1,-1)   != uv ) pointsVC[u].push_back( vPt+Vector3Di(0,1,-1) );
                if( it.PeekRel(-1,1,-1)  != uv ) pointsVC[u].push_back( vPt+Vector3Di(-1,1,-1) );
                if( it.PeekRel(-1,-1,-1) != uv ) pointsVC[u].push_back( vPt+Vector3Di(-1,-1,-1) );
                if( it.PeekRel(0,-1,-1)  != uv ) pointsVC[u].push_back( vPt+Vector3Di(0,-1,-1) );

                break;
              } // if
            } // u
          } // if it is component point
        } // if point is in the volume

        vfPt2 = vfPt2 + obliquePlaneAxis_2;
      } // for x

      vfPt1 = vfPt1 + obliquePlaneAxis_1;
    } // for y
  } // end of true oblique plane
  
} // GetComponentContourPoints



/**
 * Removes the component with the provided name
 *
 * @param     labelVol                      associated label volume
 * @param     componentArray                input component array
 * @param     sName                         the component name
 * @return    true if the component was removed; otherwise false
 */
bool vxComponentUtils::RemoveComponent( vxBlockVolume<uint2> & labelVol, vxComponentArray & componentArray, const std::string & sName )
{
  for( uint4 uComponentIndex(0); uComponentIndex < componentArray.GetCount(); ++uComponentIndex )
	{
		std::string sExistingName( componentArray[ uComponentIndex ]->GetName() );
		if( sExistingName == sName )
		{
			vxComponent * pComponent = componentArray[ uComponentIndex ];

      // clear the component from the label volume
      vxBlockVolumeIterator<uint2> labelIter( labelVol );
      labelIter.SetClearBlock( 0 );
      register uint2 uCurrentLabelIndex = pComponent->GetLabel();
      for( labelIter.FirstNonBackgroundBlockZYX(); labelIter.IsNotAtEnd(); labelIter.NextNonBackgroundBlockZYX() )
      {
        if( labelIter.GetCurrentBlock().IsHomogenous() && labelIter.GetVoxel() == uCurrentLabelIndex )
        {
          labelIter.ClearCurrentBlock();
          continue;
        }

        for( ; labelIter.IsNotAtEndOfBlock(); labelIter.NextZYXinsideBlock() )
        {
          if( labelIter.GetVoxel() == uCurrentLabelIndex )
            labelIter.SetVoxel( 0 );
        }
      }

      componentArray.RemoveComponent( uComponentIndex );
      return true;
		}
	}

  LogFyi( "Unable to remove the component which has name " + sName , "vxComponentUtils", "RemoveComponent" );
	return false;
} // RemoveComponent( labelVol, componentArray, sName )


/**
 * Adds voxels to current component
 *
 * @param desiredVoxels               voxels which are desired to be part of the new component
 * @param pComponentArray             the components object
 * @param pLabelVolume                the label volume object
 * @param pLavelVolumeBinaryHistogram the label volume binary histogram
 * @param interactiveSegmentation     the interactive segmentation object
 * @param bTestForLockedVoxels        whether to test for locked voxels
 * @return                            new label index >0 upon success, 0 on failure
 */
uint2 vxComponentUtils::AddVoxelsToCurrentComponent( vxBlockVolume<bool> & desiredVoxels,
                                            vxShareableObject< vxComponentArray > * pComponentArray,
                                            vxShareableObject< vxBlockVolume<uint2> > * pLabelVolume,
                                            InteractiveSegmentation & interactiveSegmentation, bool bTestForLockedVoxels )
{
  // Do we need to create a new component?
  uint2 uHistoryIndex( 0 );
  uint2 uLabel( 0 );

  /// Get current component index and label volume
  uHistoryIndex = interactiveSegmentation.GetCompHdlrPtr()->GetSelectedComponentIndex();
  uLabel = ( * pComponentArray )[ uHistoryIndex ]->GetLabel();

  //pre-compute the component lock information
  bool * pbLocked = new bool[ pComponentArray->ArrayByLabel().size() ]; 
  for ( int i=0; i< pComponentArray->ArrayByLabel().size(); i++ )
    pbLocked[ i ] = pComponentArray->ArrayByLabel()[ i ]->IsLocked();

  // copy over into the label volume
  vxBlockVolumeIterator<bool> desiredVolIter( desiredVoxels );
  vxBlockVolumeIterator<uint2> labelIter( * pLabelVolume );
  labelIter.SetClearBlock( uLabel );

  for ( desiredVolIter.FirstNonBackgroundBlockZYX(); desiredVolIter.IsNotAtEnd(); desiredVolIter.NextNonBackgroundBlockZYX() )
  {
    if( desiredVolIter.GetCurrentBlock().IsHomogenous() && labelIter.GetCurrentBlock().IsHomogenous()  )
    {
			//check to see if this is a non-zero voxel
			if (  desiredVolIter.GetVoxel() )
			{
				labelIter.SetPos( desiredVolIter );
				if( !bTestForLockedVoxels || !pbLocked[ labelIter.GetVoxel() ] )
				{
					labelIter.ClearCurrentBlock();
				}
			}
    } // if
    else
    {
      for ( desiredVolIter.FirstNonBackgroundZYXinsideBlock(); desiredVolIter.IsNotAtEndOfBlock(); desiredVolIter.NextNonBackgroundZYXinsideBlock() )
      {
        labelIter.SetPos( desiredVolIter );
        // check for locked voxels
        if ( !bTestForLockedVoxels || !pbLocked[ labelIter.GetVoxel() ] )
        {
          labelIter.SetVoxel( uLabel );
        }
      }
    } // else
  } // for all non background blocks

  // let everyone know that the volume has changed
  pLabelVolume->Modified( vxModInfo( vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_MODIFIED ) );
  
  pComponentArray->Modified( vxModIndInfo( vxComponentArray, uHistoryIndex, vxUtils::COMPONENTARRAY_MODIFIED ) );

  return uLabel;
} // AddVoxelsToCurrentComponent( ... )


/**
 * removes selected voxels from current component
 *
 * @param desiredVoxels               voxels which are desired to be part of the new component
 * @param pComponentArray             the components object
 * @param pLabelVolume                the label volume object
 * @param pLavelVolumeBinaryHistogram the label volume binary histogram
 * @param interactiveSegmentation     the interactive segmentation object
 * @param bTestForLockedVoxels        whether to test for locked voxels
 * @return                            new label index >0 upon success, 0 on failure
 */
uint2 vxComponentUtils::RemoveVoxelsFromCurrentComponent( vxBlockVolume<bool> & desiredVoxels,
                                            vxShareableObject< vxComponentArray > * pComponentArray,
                                            vxShareableObject< vxBlockVolume<uint2> > * pLabelVolume,
                                            InteractiveSegmentation & interactiveSegmentation, bool bTestForLockedVoxels )
{
  bool bVoxelRemoved = false;

  // Get currently selected component.
  uint2 uHistoryIndex( 0 );
  uint2 uLabel( 0 );

  /// Get current component index and label volume
  uHistoryIndex = interactiveSegmentation.GetCompHdlrPtr()->GetSelectedComponentIndex();
  uLabel = ( * pComponentArray )[ uHistoryIndex ]->GetLabel();

  // copy over into the label volume
  vxBlockVolumeIterator<bool> desiredVolIter( desiredVoxels );
  vxBlockVolumeIterator<uint2> labelIter( * pLabelVolume );
  labelIter.SetClearBlock( uLabel );

  for ( desiredVolIter.FirstNonBackgroundBlockZYX(); desiredVolIter.IsNotAtEnd(); desiredVolIter.NextNonBackgroundBlockZYX() )
  {
    for ( desiredVolIter.FirstNonBackgroundZYXinsideBlock(); desiredVolIter.IsNotAtEndOfBlock(); desiredVolIter.NextNonBackgroundZYXinsideBlock() )
    {
      labelIter.SetPos( desiredVolIter );

      // If in desired voxel list and has value equal to the current component, set to 0.
      if ( desiredVolIter.GetVoxel() && labelIter.GetVoxel() == uLabel )
      {
        if(labelIter.GetVoxel() != 0)
        {
          bVoxelRemoved = true;
        }

        labelIter.SetVoxel( 0 );
      }
    }
  } // for all non background blocks

  if(!bVoxelRemoved)
  {
    vxMessageBox::ShowInfo("No voxels were removed that belong to the currently selected component.");
  }

  // let everyone know that the volume has changed
  pLabelVolume->Modified( vxModInfo( vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_MODIFIED ) );
  pComponentArray->Modified( vxModIndInfo( vxComponentArray, uHistoryIndex, vxUtils::COMPONENTARRAY_MODIFIED ) );

  return uLabel;
} // RemoveVoxelsFromCurrentComponent( ... )


// $Log: vxComponentUtils.C,v $
// Revision 1.23.2.1  2009/08/31 13:16:46  kchalupa
// Add methods for Adding and Removing Voxels
//
// Revision 1.23  2008/05/05 20:37:11  dongqing
// using register to speed up component removal
//
// Revision 1.22  2008/05/02 04:56:17  cqian
// check in
//
// Revision 1.21  2007/08/21 19:34:16  gdavidson
// Added RemoveComponent method to cleanup label volume and componentArray
//
// Revision 1.20  2007/07/20 18:50:42  dongqing
// add CreateNewEmptyComponent()
//
// Revision 1.19  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.18.2.1  2007/04/03 19:41:26  romy
// added appendingtoNone check for createComponents. Issue# 5556
//
// Revision 1.18  2007/03/06 19:28:30  dongqing
// code review
//
// Revision 1.17  2006/12/13 17:50:39  dongqing
// implment the function GetComponentContourLists(...)
//
// Revision 1.16  2006/12/12 17:03:04  geconomos
// updated GetComponentContourLists
//
// Revision 1.15  2006/12/12 16:46:26  geconomos
// function prototype added for component contour lists
//
// Revision 1.14  2006/11/12 07:56:21  romy
// added GetComponent Object to get the component object by name
//
// Revision 1.13  2006/11/10 15:46:46  romy
// removed the debug messages from the GetComponentIndex method
//
// Revision 1.12  2006/11/09 23:19:32  romy
// moved  GetComponentIndex method to componentUtils
//
// Revision 1.11  2006/10/05 15:14:32  romy
// improved the performance and removed the temporary fix
//
// Revision 1.10  2006/10/05 15:10:06  romy
// temporary bug fix for CreateNewComponent. Needs to improve the performance
//
// Revision 1.9  2006/09/21 19:24:56  gdavidson
// Added CalculateComponentVolume method
//
// Revision 1.8  2006/09/01 13:23:46  frank
// moved dilate volume function here and changed class to a namespace
//
// Revision 1.7  2006/08/28 21:46:56  gdavidson
// Optimized CreateNewComponent
//
// Revision 1.6  2006/07/06 12:55:59  frank
// Issue #4840: Refactored component creation and naming method
//
// Revision 1.5  2006/04/24 20:04:05  romy
// modified Locked Voxel check in NewComponent creation
//
// Revision 1.4  2006/04/14 18:24:32  frank
// Issue #4711: Fixed the unchecked growing into locked regions
//
// Revision 1.3  2006/03/29 20:03:08  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.2  2006/02/28 15:24:42  frank
// improved undo capabilities during interactive segmentation
//
// Revision 1.1  2006/02/27 15:22:58  frank
// split out component utilities
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxComponentUtils.C,v 1.23.2.1 2009/08/31 13:16:46 kchalupa Exp $
// $Id: vxComponentUtils.C,v 1.23.2.1 2009/08/31 13:16:46 kchalupa Exp $
