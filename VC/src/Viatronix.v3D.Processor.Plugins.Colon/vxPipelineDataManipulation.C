// $Id: vxPipelineDataManipulation.C,v 1.12 2007/03/06 16:34:01 dongqing Exp $
//
// Copyright(c) 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this
// file may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dongqing Chen dongqing@viatronix.net
//
// Complete History at bottom of file.


#include "stdafx.h"
#include "vxPipeline.h"
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include "vxVolumeHeader.h"
#include "vxIntensityConverter.h"
#include "vxBlockVolumeFileReader.h"

#include "Timer.h"
#include "File.h"
#include "Logger.h"
#include "SystemInfo.h"

#include "Mutex.h"

using namespace std;
using namespace hdr;
using namespace vx;

/** 
 *  This file includes all functions for volume manipulation of the pipeline
 */

#define FILE_REVISION "$Revision: 1.12 $"

/**
*  Read in the input volume
*  If it is vxBlockVolume in UINT2 voxel type, shift intensity based on BIAS value 
*  If it is LinVol, create a UINT2 vxBlockVolume and overwrite the vol file on disk
*
*  @param pVol is the pointer of the vxBlockVolume for the pipeline working
*  @param sourceFilePath the path for the input volume.
*  @return true if it is successfull
*/
bool vxPipeline::HandleInputVol( vxBlockVolume<uint2> * pVol, const std::string & sourceFilePath )
{  
  vxVolumeHeader *ph = new vxVolumeHeader;
  ph->Read( sourceFilePath+".vol" );

  if( m_sVoxelType == "int2" )
  { // This should be the old linear volume format
    if( ph->GetMemoryLayoutEnum() != vxVolumeHeader::MemoryLayoutEnum::LINEAR )
    {
      LogErr("Volume is not LINEAR, but it has SHORT type!", "PipelineDataManipulation.C", "HandleInputVol");
      return(false);
    }

	  // temporary linear volume for reading the old format
	  LinVolS *linVol = new LinVolS;
    try
    {
      linVol->Read( sourceFilePath+".vol" );
    }
    catch( ... )
    {
      LogErr("Fail to read the input linear volume", "PipelineDataManipulation.C", "HandleInputVol"); 
      return(false);
    }

    // Linear volume has no bias. We have to add one to the vxBlockVolume
    VarMap map(linVol->GetHdrMap());
    int4 iBias(1000);
    map.SetHdrVar("bias", iBias, "Viatronix bias value");
 
    // set mutex in case memory overload when more than one pipeline run
	  Mutex peakMemoryUsageMutex("555E2EE4-FFCD-43af-9A59-CA9B59704B05");
	  peakMemoryUsageMutex.Lock();

    try
    {
      const Triple<int2> viDim(linVol->GetDim());
	    pVol->GetHeader().SetVariableVolumeHeader(map);
	    pVol->GetHeader().SetVolDim( viDim );
	    pVol->GetHeader().SetVoxelUnits( linVol->GetUnits() );
			pVol->GetHeader().SetSignificantBitsPerVoxel( 16 );
      vxBlockVolumeIterator<uint2> iter( *pVol );
      int2 * pData( linVol->GetDataPtr() );

	    const int4 iZ(viDim.m_z);
	    const int4 iY(viDim.m_y);
	    const int4 iX(viDim.m_x);
	    register int4 x, y, z, i(0);
	    for( z=0, iter.SetPosZ(0); z<iZ; z++, iter.IncZ() )
	    { 
		    for( y=0, iter.SetPosY(0); y<iY; y++, iter.IncY() )
		    {
		      for( x=0, iter.SetPosX(0); x<iX; x++, iter.IncX() )
          {
            if( pData[i] < 0 )
              pData[i] = 0;
            iter.SetVoxel( pData[i] );
            i++;
          } // for(x)
		    } // for  y )
	    } // for( z )

	    delete linVol;
      // remove the linear volume from the disk
      std::string ss( sourceFilePath+".vol" );
      DeleteFile(ss.c_str());

			VarMap map( pVol->GetHeader().GetVariableVolumeHeader() );
		  int iVersionNum(4);
      map.SetHdrVar( "version", iVersionNum, "Header version" );
		  pVol->GetHeader().SetVariableVolumeHeader( map );

			///$$
			if( m_sBodyPosition[2] != 'S' )
			{ // for non-supine situation, include prone, DR, and DL
				ReformatVolFromOldToNewLayout( m_sBodyPosition, *pVol );

				pVol->Compact();
				// overwrite the source file on the disk
				pVol->Write( ss );

				ReformatVolFromNewToOldLayout( m_sBodyPosition, *pVol );
			} // if
			else
			{ // for supine position only
				pVol->Compact();
				// write the vxBlockVolume to the disk
				pVol->Write( ss );
			} // else
    }
    catch( ... )
    {
	    if( peakMemoryUsageMutex.IsLocked() )
	      peakMemoryUsageMutex.Unlock();

      LogErr("Fail to replace the linear volume", "PipelineDataManipulation.C", "HandleInputVol");
      return(false);
    }

	  if( peakMemoryUsageMutex.IsLocked() )
	    peakMemoryUsageMutex.Unlock();
  } // if( int2 )
  else // voxel type is uint2
  {
    if( ph->GetMemoryLayoutEnum() != vxVolumeHeader::MemoryLayoutEnum::BLOCK )
    {
      LogErr("Volume is not vxBlockVolume", "PipelineDataManipulation.C", "HandleInputVol");
      return(false);
    }

   	int4 iBias(1000);
	  int4 iMax(0);
	  // get iMax, modality
    std::string sBias;
    ph->GetHeaderEntry(vxVolumeHeader::VarVolHeaderEntryEnum::Bias, sBias);
    iBias = atoi(sBias.c_str());

    vxIntensityConverter intenConv;
    iMax = intenConv.GetMaxValue( *ph );      
	  if( iMax <= 0 )
	  {
	    LogErr("Wrong maximum value for the volume", "PipelineDataManipulation.C", "HandleInputVol");
	    return(false);
	  }

    try
    { // read in the input volume
	    vxBlockVolumeFileReader< uint2 >::Read(sourceFilePath+".vol", *pVol );
	  }
	  catch( ... )
	  {
	    LogErr("Fail to read volume file", "PipelineDataManipulation.C", "HandleInputVol");
	    return(false);
	  } 		
			
		///$$
		if( m_bIsNewVolCoordinateFormat ) 
		{ // This is the new format
			if( m_sBodyPosition[2] != 'S' )
			  ReformatVolFromNewToOldLayout( m_sBodyPosition, *pVol );
		} // if
		else
		{ // This is the old format. 	
			VarMap map( pVol->GetHeader().GetVariableVolumeHeader() );
		  int iVersionNum(4);
      map.SetHdrVar( "version", iVersionNum, "Header version" );
		  pVol->GetHeader().SetVariableVolumeHeader( map );

			if( m_sBodyPosition[2] == 'S' )
			{ //Replace the old format with new format file 
		    pVol->Write(sourceFilePath+".vol");
			}
			else
			{
				ReformatVolFromOldToNewLayout( m_sBodyPosition, *pVol );
        
				//Replace the old format with new format file 
		    pVol->Write(sourceFilePath+".vol");

			  ReformatVolFromNewToOldLayout( m_sBodyPosition, *pVol );
			} // else
		} // else

    // get volume dimension
    const Triple<int2> viDim( pVol->GetHeader().GetVolDim() );
	  const float4 fMax(static_cast<float4>(iMax));

	  vxBlockVolumeIterator<uint2> iter( *pVol );

	  if( iBias != 1000 )
	  {
      try
	    { // clamp image based on the bias
	      register int2   iv;			
	      register float4 ff;
				const int4      iii(1000-iBias);

        for( iter.SetPos(0,0,0); iter.IsNotAtEnd(); iter.NextBlockZYX() )
        {
          for( ; iter.IsNotAtEndOfBlock(); iter.NextZYXinsideBlock() )
          {
			      ff = static_cast<float4>(iter.GetVoxel())/fMax;

						iv = ff*iMax+iii;
			      if( iv < 0 )
			        iv = 0;

			      iter.SetVoxel( iv );
		      } // for inside block
        } // for all blocks
	    } // try
	    catch( ... )
	    {		
        LogErr("Fail to clamp image", "PipelineDataManipulation.C", "HandleInputVol");
	      return(false);
	    } 

	    VarMap map( ph->GetVariableVolumeHeader() );
      iBias = 1000;
      map.SetHdrVar("bias", iBias, "Viatronix bias value");
	    ph->SetVariableVolumeHeader(map);
	    pVol->SetHeader(*ph);
			pVol->Compact();
	  } // if(iBias...)
 
	  register int4 iZ(viDim.m_z);
	  register int4 iY(viDim.m_y);
	  register int4 iX(viDim.m_x);
	  register int4 iHigh(0);
	  register int4 iTissue(0);
	  iter.SetPosZ( iZ/2 );
	  register int4 x, y;
	  // check if intensity range is normal or not
	  for( y=0, iter.SetPosY(0); y<iY; y++, iter.IncY() )
	  {
	    for( x=0, iter.SetPosX(0); x<iX; x++, iter.IncX() )
	    {
	      if( iter.GetVoxel() > 4095 )
		      iHigh++;
		    else if( iter.GetVoxel() > 700 )
		      iTissue++;
	    } // for ( x )
    } // for ( y )
    
    if( (static_cast<float4>(iHigh)  /static_cast<float4>(viDim.m_x*viDim.m_y) > 0.3F) ||
		    (static_cast<float4>(iTissue)/static_cast<float4>(viDim.m_x*viDim.m_y) < 0.10F) )
	  {
      LogErr("Abnormal intensity range", "PipelineDataManipulation.C", "HandleInputVol");
      return(false);  
	  } // if
  } // if-else

  delete ph;    
  return(true);
} //HandleInputVol()


/**
*  interpolation of the volume for Imatron studies if necessary
*  The interpolation only applies to z-direction
*
*  @param vol is the input vxBlockVolume
*  @param sourceFilePath the path for the input volume.
*  @return true if interpolation successful
 */
bool vxPipeline::InterpolationForImatronCase( vxBlockVolume<uint2> *pVol, const std::string & sourceFilePath )
{
  Triple<int2>   oldDim  ( pVol->GetHeader().GetVolDim() );
  Triple<float4> oldUnits( pVol->GetHeader().GetVoxelUnits() );
  
  if( (oldUnits.m_z <= 1.5F)&&(pVol->GetHeader().GetVoxelUnitTypeEnum() == MILLIMETER) )
  {
	  LogErr("The slice interval is less than 1.5 mm. Skip interpolation", 
			     "PipelineDataManipulation.C", "InterpolationForImatronCase");
	  return(true);
  }

  Triple<int2>   newDim  ( oldDim );
  Triple<float4> newUnits( oldUnits );
  newUnits.m_z = 1.0F;
  newDim.m_z   = static_cast<int4>( static_cast<float4>(oldDim.m_z)*oldUnits.m_z );

  const int4 iX(newDim.m_x);
  const int4 iY(newDim.m_y);
  const int4 iZ(newDim.m_z);
  register int4   x, y, z, t;
  register float4 f, fl, fu;

  vxBlockVolume<uint2> *pNewVol = new vxBlockVolume<uint2>(vxVolumeHeader(newDim, newUnits, 16, 0));
	pNewVol->GetHeader().SetVariableVolumeHeader( pVol->GetHeader().GetVariableVolumeHeader() );

  vxBlockVolumeIterator<uint2> it(*pVol);
  vxBlockVolumeIterator<uint2> it0(*pVol);
  vxBlockVolumeIterator<uint2> itn(*pNewVol);

  try
  { // 1st slice
	  it.SetPosZ(0); 
	  itn.SetPosZ(0); 
	  for( y=0, it.SetPosY(0), itn.SetPosY(0); y<iY; y++, it.IncY(), itn.IncY() )
	    for( x=0, it.SetPosX(0), itn.SetPosX(0); x<iX; x++, it.IncX(), itn.IncX() )
		    itn.SetVoxel( it.GetVoxel() );

	  register int4 izb(oldDim.m_z-1);
	  register int4 izB(newDim.m_z-1);
    // last slice
	  it.SetPosZ(izb); 
	  itn.SetPosZ(izB); 
	  for( y=0, it.SetPosY(0), itn.SetPosY(0); y<iY; y++, it.IncY(), itn.IncY() )
	    for( x=0, it.SetPosX(0), itn.SetPosX(0); x<iX; x++, it.IncX(), itn.IncX() )
		    itn.SetVoxel( it.GetVoxel() );

	  // middle slices
	  for( z=1, itn.SetPosZ(0); z<iZ; z++, itn.IncZ() )
	  { 
	    f  = static_cast<float4>(z)/oldUnits.m_z;
	    t  = static_cast<int4>(f);
	    fl = f-static_cast<float4>(t);
	    fu = 1.0F-fl;
	    
	    it.SetPosZ(t);
	    it0.SetPosZ(t+1);
	    if( t >= izb )
	    {
		    for( y=0, it.SetPosY(0), it0.SetPosY(0), itn.SetPosY(0); y<iY; y++, it.IncY(), it0.IncY(), itn.IncY() )
		    {
		      for( x=0, it.SetPosX(0), it0.SetPosX(0), itn.SetPosX(0); x<iX; x++, it.IncX(), it0.IncX(), itn.IncX() )
			      itn.SetVoxel( it.GetVoxel() );
		    } // for(y)
	    } // if(t==izb)
	    else
	    {
		    for( y=0, it.SetPosY(0), it0.SetPosY(0), itn.SetPosY(0); y<iY; y++, it.IncY(), it0.IncY(), itn.IncY() )
		    {
		      for( x=0, it.SetPosX(0), it0.SetPosX(0), itn.SetPosX(0); x<iX; x++, it.IncX(), it0.IncX(), itn.IncX() )
			      itn.SetVoxel(static_cast<uint2>(fu*static_cast<float4>(it.GetVoxel()) +
						             fl*static_cast<float4>(it0.GetVoxel())));
		    } // for(y)
	    } // if-else
	  } // for(z)

	  *pVol = *pNewVol;
	  delete pNewVol;

    // remove the old volume and write out the interpolated one
    std::string ss( sourceFilePath+".vol" );
    DeleteFile(ss.c_str());

		///$$
		if( m_sBodyPosition[2] == 'S' )
		{
		  pVol->Compact();
		  // overwrite the source file on the disk
		  pVol->Write( ss );
		}
		else
		{ // for non-supine situation, include prone, DR, and DL
		  ReformatVolFromOldToNewLayout( m_sBodyPosition, *pVol );

		  pVol->Compact();
		  // overwrite the source file on the disk
		  pVol->Write( ss );

		  ReformatVolFromNewToOldLayout( m_sBodyPosition, *pVol );
		} // else
  }
  catch( ... )
  {
    LogErr("Fail to write out the interpolated volume", "PipelineDataManipulation.C", "InterpolationForImatronCase");
    return(false);
  }

  return(true);
} // InterpolationForImatronCase()


/** 
 *   convertt prone volume into supine
 *  
 *   @param proVol
 *   @param supVol
 */
void vxPipeline::FlipBodyPositionForMR( vxBlockVolume<uint2> & inVol, vxBlockVolume<uint2> & outVol )
{
  Triple<int4> dim(inVol.GetHeader().GetVolDim());
  register const int4 iX(dim.m_x);
  register const int4 iY(dim.m_y);
  register const int4 iZ(dim.m_z);

  vxBlockVolumeIterator<uint2> iti(inVol);
  vxBlockVolumeIterator<uint2> ito(outVol);

  register int4 x, y, z;
  for( z=0, ito.SetPosZ(0), iti.SetPosZ(0); z<iZ; z++, ito.IncZ(), iti.IncZ() )
  {
    for( y=0, ito.SetPosY(0), iti.SetPosY(iY-1); y<iY; y++, ito.IncY(), iti.DecY() )
    {
      for( x=0, ito.SetPosX(0), iti.SetPosX(iX-1); x<iX; x++, ito.IncX(), iti.DecX() )
        ito.SetVoxel( iti.GetVoxel() );
    } // for(y)
  } // for(z)
} // FlipBodyPositionForMR


/** 
 *   convertt prone volume into supine
 *  
 *   @param proVol
 *   @param supVol
 */
void vxPipeline::FlipBodyPositionForMR( vxBlockVolume<uint1> & inVol, vxBlockVolume<uint1> & outVol )
{
  Triple<int4> dim(inVol.GetHeader().GetVolDim());
  register const int4 iX(dim.m_x);
  register const int4 iY(dim.m_y);
  register const int4 iZ(dim.m_z);

  vxBlockVolumeIterator<uint1> iti(inVol);
  vxBlockVolumeIterator<uint1> ito(outVol);

  register int4 x, y, z;
  for( z=0, ito.SetPosZ(0), iti.SetPosZ(0); z<iZ; z++, ito.IncZ(), iti.IncZ() )
  {
    for( y=0, ito.SetPosY(0), iti.SetPosY(iY-1); y<iY; y++, ito.IncY(), iti.DecY() )
    {
      for( x=0, ito.SetPosX(0), iti.SetPosX(iX-1); x<iX; x++, ito.IncX(), iti.DecX() )
        ito.SetVoxel( iti.GetVoxel() );
    } // for(y)
  } // for(z)
} // FlipBodyPositionForMR



/**
 * Sends State Information to message handler
 * @param sStage        Stage of the process
 * @param iProgress     Progress (Overall)
 */
 void vxPipeline::SendState( const int4 iProgress , const std::string & sStage )
 {

   if ( m_pManager != __nullptr && m_pMessage != __nullptr )
   {
      LogDbg(util::Strings::Format("Sending state to message handler %s-%d",sStage.c_str(), iProgress),"vxPipeline", "SendState");
      m_pMessage->SetStage(sStage);
      m_pMessage->SetProgress((float)iProgress);

      m_pManager->Send(m_pMessage);
   }

 }  // SendState()

/**
 * Sends Information to message handler
 */
 void vxPipeline::SendInformation( const std::string & sMessage, const std::string & sStage )
 {
   if ( m_pManager != __nullptr && m_pMessage != __nullptr )
   {
      m_pMessage->SetStage(sStage);
      m_pMessage->SetStatusMessage(sMessage);

      m_pManager->Send(m_pMessage);
   }
 }  // SendInformation()


 /** 
  *  This is for executing another application within the pipeline
  *
  *  @param sCommandLine: Sample command line:
  */
 bool vxPipeline::LaunchProcessAndWait( const std::string & sCommandLine )
 {
	 STARTUPINFO si;

	 memset( &si, 0, sizeof( si ) );

	 si.cb = sizeof(si);

	 PROCESS_INFORMATION pi;

	 memset( &pi, 0, sizeof( pi ) );

	 char * s = const_cast< char * >( sCommandLine.c_str() );

	 if( !CreateProcess( NULL, s, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) ) 
		 return false;

	 // wait until child process exits

	 WaitForSingleObject( pi.hProcess, INFINITE );
	 DWORD dwExitCode( 0 );
	 GetExitCodeProcess( pi.hProcess, &dwExitCode );

	 // close process and thread handles 
	 CloseHandle( pi.hProcess );
	 CloseHandle( pi.hThread );

	 return dwExitCode == 0 ? true : false;
 } // LaunchProcessAndWait


 /**
  *  reformat the volume from old to new layout
	*
	*  @param sBodyPosition
	*  @param vol
	*/
 void vxPipeline::ReformatVolFromOldToNewLayout( const std::string & sBodyPosition,
	                                               vxBlockVolume<uint2> & vol )
 {
	 const Triple<int> dim( vol.GetHeader().GetVolDim() );
	 const int iX( dim.m_x );
	 const int iY( dim.m_y );
	 const int iZ( dim.m_z );
	 uint2 *pS = new uint2[ iX*iY ];

	 register int x, y, z, i, j;
	 const int ix(iX-1);
	 const int iy(iY-1);

	 vxBlockVolumeIterator<uint2> itv( vol );

	 if( sBodyPosition[2] == 'P' )
	 { // prone position
		 for( z=0, itv.SetPosZ(0); z<iZ; z++, itv.IncZ() )
		 {
			 i = 0;
			 /// copy slice to buffer
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 pS[i] = itv.GetVoxel();
					 i++;
				 } // for x
			 } // for y

			 j = iy*iX;
			 /// reformat slice
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 i = ix;

				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 itv.SetVoxel( pS[i+j] );
					 i--;
				 } // for x

				 j = j-iX;
			 } // for y
		 } // for z
	 } // if
	 else if( sBodyPosition[2] == 'D' && sBodyPosition[3] == 'R' )
	 { // decubitus right position
		 for( z=0, itv.SetPosZ(0); z<iZ; z++, itv.IncZ() )
		 {
			 i = 0;
			 /// copy slice to buffer
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 pS[i] = itv.GetVoxel();
					 i++;
				 } // for x
			 } // for y

			 /// reformat slice
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 i = ix*iX;

				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 itv.SetVoxel( pS[y+i] );

					 i = i-iX;
				 } // for x
			 } // for y
		 } // for z
	 } // else if
	 else if( sBodyPosition[2] == 'D' && sBodyPosition[3] == 'L' )
	 {// decubitus left position
		 for( z=0, itv.SetPosZ(0); z<iZ; z++, itv.IncZ() )
		 {
			 i = 0;
			 /// copy slice to buffer
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 pS[i] = itv.GetVoxel();
					 i++;
				 } // for x
			 } // for y

			 j = iy;
			 /// reformat slice
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 i = 0;

				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 itv.SetVoxel( pS[j+i] );

					 i = i+iX;
				 } // for x

				 j--;
			 } // for y
		 } // for z
	 } // else if

	 delete pS;
 } // ReformatVolFromOldToNewLayout uint2


 /**
  *  reformat the volume from old to new layout
	*
	*  @param sBodyPosition
	*  @param vol
	*/
 void vxPipeline::ReformatVolFromOldToNewLayout( const std::string & sBodyPosition,
	                                               vxBlockVolume<uint1> & vol )
 {
	 const Triple<int> dim( vol.GetHeader().GetVolDim() );
	 const int iX( dim.m_x );
	 const int iY( dim.m_y );
	 const int iZ( dim.m_z );
	 uint1 *pS = new uint1[ iX*iY ];

	 register int x, y, z, i, j;
	 const int ix(iX-1);
	 const int iy(iY-1);

	 vxBlockVolumeIterator<uint1> itv( vol );

	 if( sBodyPosition[2] == 'P' )
	 { // prone position
		 for( z=0, itv.SetPosZ(0); z<iZ; z++, itv.IncZ() )
		 {
			 i = 0;
			 /// copy slice to buffer
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 pS[i] = itv.GetVoxel();
					 i++;
				 } // for x
			 } // for y

			 j = iy*iX;
			 /// reformat slice
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 i = ix;

				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 itv.SetVoxel( pS[i+j] );
					 i--;
				 } // for x

				 j = j-iX;
			 } // for y
		 } // for z
	 } // if
	 else if( sBodyPosition[2] == 'D' && sBodyPosition[3] == 'R' )
	 { // decubitus right position
		 for( z=0, itv.SetPosZ(0); z<iZ; z++, itv.IncZ() )
		 {
			 i = 0;
			 /// copy slice to buffer
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 pS[i] = itv.GetVoxel();
					 i++;
				 } // for x
			 } // for y

			 /// reformat slice
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 i = ix*iX;

				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 itv.SetVoxel( pS[y+i] );

					 i = i-iX;
				 } // for x
			 } // for y
		 } // for z
	 } // else if
	 else if( sBodyPosition[2] == 'D' && sBodyPosition[3] == 'L' )
	 {// decubitus left position
		 for( z=0, itv.SetPosZ(0); z<iZ; z++, itv.IncZ() )
		 {
			 i = 0;
			 /// copy slice to buffer
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 pS[i] = itv.GetVoxel();
					 i++;
				 } // for x
			 } // for y

			 j = iy;
			 /// reformat slice
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 i = 0;

				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 itv.SetVoxel( pS[j+i] );

					 i = i+iX;
				 } // for x

				 j--;
			 } // for y
		 } // for z
	 } // else if

	 delete pS;
 } // ReformatVolFromOldToNewLayout uint1


 /**
  *  reformat the volume from new to old layout
	*
	*  @param sBodyPosition
	*  @param vol
	*/
 void vxPipeline::ReformatVolFromNewToOldLayout( const std::string & sBodyPosition,
	                                               vxBlockVolume<uint2> & vol )
 {
	 const Triple<int> dim( vol.GetHeader().GetVolDim() );
	 const int iX( dim.m_x );
	 const int iY( dim.m_y );
	 const int iZ( dim.m_z );
	 uint2 *pS = new uint2[ iX*iY ];

	 register int x, y, z, i, j;
	 const int ix(iX-1);
	 const int iy(iY-1);

	 vxBlockVolumeIterator<uint2> itv( vol );

	 if( sBodyPosition[2] == 'P' )
	 { // prone position
		 for( z=0, itv.SetPosZ(0); z<iZ; z++, itv.IncZ() )
		 {
			 i = 0;
			 /// copy slice to buffer
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 pS[i] = itv.GetVoxel();
					 i++;
				 } // for x
			 } // for y

			 j = iy*iX;
			 /// reformat slice
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 i = ix;

				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 itv.SetVoxel( pS[i+j] );
					 i--;
				 } // for x

				 j = j-iX;
			 } // for y
		 } // for z
	 } // if
	 else if( sBodyPosition[2] == 'D' && sBodyPosition[3] == 'R' )
	 { // decubitus right position
		 for( z=0, itv.SetPosZ(0); z<iZ; z++, itv.IncZ() )
		 {
			 i = 0;
			 /// copy slice to buffer
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 pS[i] = itv.GetVoxel();
					 i++;
				 } // for x
			 } // for y

			 j = iy;
			 /// reformat slice
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 i = 0;

				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 itv.SetVoxel( pS[j+i] );

					 i = i+iX;
				 } // for x

				 j--;
			 } // for y
		 } // for z
	 } // else if
	 else if( sBodyPosition[2] == 'D' && sBodyPosition[3] == 'L' )
	 {// decubitus left position
		 for( z=0, itv.SetPosZ(0); z<iZ; z++, itv.IncZ() )
		 {
			 i = 0;
			 /// copy slice to buffer
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 pS[i] = itv.GetVoxel();
					 i++;
				 } // for x
			 } // for y

			 /// reformat slice
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 i = ix*iX;

				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 itv.SetVoxel( pS[y+i] );
					 i = i-iX;
				 } // for x
			 } // for y
		 } // for z
	 } // else if

	 delete pS;
 } // ReformatVolFromNewToOldLayout uint2


 /**
  *  reformat the volume from new to old layout
	*
	*  @param sBodyPosition
	*  @param vol
	*/
 void vxPipeline::ReformatVolFromNewToOldLayout( const std::string & sBodyPosition,
	                                               vxBlockVolume<uint1> & vol )
 {
	 const Triple<int> dim( vol.GetHeader().GetVolDim() );
	 const int iX( dim.m_x );
	 const int iY( dim.m_y );
	 const int iZ( dim.m_z );
	 uint1 *pS = new uint1[ iX*iY ];

	 register int x, y, z, i, j;
	 const int ix(iX-1);
	 const int iy(iY-1);

	 vxBlockVolumeIterator<uint1> itv( vol );

	 if( sBodyPosition[2] == 'P' )
	 { // prone position
		 for( z=0, itv.SetPosZ(0); z<iZ; z++, itv.IncZ() )
		 {
			 i = 0;
			 /// copy slice to buffer
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 pS[i] = itv.GetVoxel();
					 i++;
				 } // for x
			 } // for y

			 j = iy*iX;
			 /// reformat slice
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 i = ix;

				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 itv.SetVoxel( pS[i+j] );
					 i--;
				 } // for x

				 j = j-iX;
			 } // for y
		 } // for z
	 } // if
	 else if( sBodyPosition[2] == 'D' && sBodyPosition[3] == 'R' )
	 { // decubitus right position
		 for( z=0, itv.SetPosZ(0); z<iZ; z++, itv.IncZ() )
		 {
			 i = 0;
			 /// copy slice to buffer
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 pS[i] = itv.GetVoxel();
					 i++;
				 } // for x
			 } // for y

			 j = iy;
			 /// reformat slice
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 i = 0;

				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 itv.SetVoxel( pS[j+i] );

					 i = i+iX;
				 } // for x

				 j--;
			 } // for y
		 } // for z
	 } // else if
	 else if( sBodyPosition[2] == 'D' && sBodyPosition[3] == 'L' )
	 {// decubitus left position
		 for( z=0, itv.SetPosZ(0); z<iZ; z++, itv.IncZ() )
		 {
			 i = 0;
			 /// copy slice to buffer
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 pS[i] = itv.GetVoxel();
					 i++;
				 } // for x
			 } // for y

			 /// reformat slice
			 for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
			 {
				 i = ix;

				 for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
				 {
					 itv.SetVoxel( pS[y+i] );
					 i = i-iX;
				 } // for x
			 } // for y
		 } // for z
	 } // else if

	 delete pS;
 } // ReformatVolFromNewToOldLayout uint1



#undef FILE_REVISION
  
// $Log: vxPipelineDataManipulation.C,v $
// Revision 1.12  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.11  2006/10/06 16:48:47  dongqing
// Fix the reformat bug for DL position
//
// Revision 1.10  2006/09/05 15:36:08  dongqing
// write out ULD for fast flying when "s" key on
//
// Revision 1.9  2006/08/31 20:57:57  dongqing
// made change to use new vxBlockVolume coordinate system
// That is the patient natural coordinate system
//
// Revision 1.8  2006/06/22 19:28:26  dongqing
// add vxBlockVolume::Compact to save memory space
//
// Revision 1.7  2006/05/30 13:13:45  dongqing
// switch to vxBlockVolumeReader
//
// Revision 1.6  2006/04/21 20:55:28  dongqing
// First complete beta version
//
// Revision 1.5  2006/03/21 21:09:46  dongqing
// rewrite CRES and iterator
//
// Revision 1.4  2006/03/14 21:22:45  dongqing
// 1st step tune up
//
// Revision 1.3  2006/03/13 18:39:24  dongqing
// finialize SKE codes
//
// Revision 1.2  2006/03/10 16:39:05  dongqing
// replace the iterator for better speed
//
// Revision 1.1  2006/02/06 21:44:18  dongqing
// initial check in
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxPipelineDataManipulation.C,v 1.12 2007/03/06 16:34:01 dongqing Exp $
// $Id: vxPipelineDataManipulation.C,v 1.12 2007/03/06 16:34:01 dongqing Exp $


