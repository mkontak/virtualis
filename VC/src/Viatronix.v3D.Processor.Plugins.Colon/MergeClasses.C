/// $Id: MergeClasses.C,v 1.3 2005/11/15 13:12:19 mkontak Exp $
//  $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/MergeClasses.C,v 1.3 2005/11/15 13:12:19 mkontak Exp $
//
// Copyright(c) 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dongqing Chen, dongqing@viatronix.net
//
// Complete history at bottom of file.

/**  post-processing for the segmentation mask. 
*   The pure air region will be eroded 2 layers.
*   The eroded pure air region will be merged into the air partial volume layer.
*   The tagged region with big horizental plane will be merged with near by classes
*/

#include "stdafx.h"
#include "MergeClasses.h"

using namespace std;
using namespace hdr;

#define FILE_REVISION "$Revision: 1.3 $"



/** Default constructor
*/
MergeClasses::MergeClasses( LinVolS & volume, LinVolUC & segMask, std::vector<float4> & averageIntensity )
:m_Volume( volume ), m_SegMask( segMask ), m_vAverageIntensity( averageIntensity )
{
}


/** destructor
*/
MergeClasses::~MergeClasses()
{
}


/** applying class merging procedure
*
*  @return void
*/
void MergeClasses::Run( int4 airLabel )
{
  stringstream tt;
  //erode air lumen constrained
  if( airLabel > 0 ) 
  {
    int4 iCount = 1;
    uint1 uLabelROI = static_cast<uint1> (airLabel);
    // two layers erosion
    while( iCount < 3 )
    {
      ConstrainedErosion( uLabelROI );
      iCount++;
    }
  }
  else 
  {
    LogWrn("Seg: did not erode pure air region","MergeClasses", "Run");
  }
  
  // merge classes if necessary
  if( MergingMainParts() )
  {
    LogDbg("Seg: merging classes in tagged region","MergeClasses", "Run");
  }
  else 
  {
    LogDbg("Seg: no class merge happened","MergeClasses", "Run");
  }  
  
}  // end of Run()



   /**
   * Erode air region while preserving the air-fluid boundary.
   * 
   * @param labelROI        is the label index for region of interest
   * @param labelH
*/
void MergeClasses::ConstrainedErosion( uint1 uLabelROI )
{
  // Pointer for the mask
  uint1 * puSegMask;
  // get pointer to the mask
  puSegMask = m_SegMask.GetDataPtr();
  
  // dimension of volume
  Triple<uint4> dim, step;
  dim = m_SegMask.GetDim();
  step = Triple<uint4>(1,dim.m_x,dim.m_x*dim.m_y);
  // number of voxels of the entire volume
  uint4 uNumVox = m_SegMask.GetNumberOfVoxels();
  
  // offset lookup table for neighbors
  int4 vLookupTable[5] =
  {
    1, -1, -static_cast<int4>(step.m_y), static_cast<int4>(step.m_z),
      -static_cast<int4>(step.m_z)
  };
  
  // temporary variables for loop index
  int4 i, j, k;
  
  // reverted label = uLabelROI + 1
  uint1 uLabelNew = uLabelROI+1; 
  
  // temporary label - always use 0xFE - its for temporary things
  uint1 uLabelTemp = 0xFE;
  
  // the number of neigbors to look at 
  uint4 uNeighCount = 5;
  
  bool bSameLabelFound, bDiffLabelFound;
  
  // erosion loop
  for( i = 0; i < uNumVox; ++i ) 
  {
    // Is it in the ROI?
    if( puSegMask[i] == uLabelROI ) 
    {
      bDiffLabelFound = false;
      bSameLabelFound = false;
      
      // check all the neighbors.. if any one of them is not labelled labelROI, then 
      // mark the current voxel as labelNew (coz that means current is on the boundary)
      for( j = 0; j < uNeighCount; j++ ) 
      {
        k = i + vLookupTable[j];
        if( (k > 0) && (k < uNumVox) ) 
        {
          if( (puSegMask[k] != uLabelROI) && (puSegMask[k] != uLabelTemp) ) 
          {
            bDiffLabelFound = true;
          } 
          else if( (puSegMask[k] == uLabelROI) || (puSegMask[k] == uLabelTemp) ) 
          {
            bSameLabelFound = true;
          }
        }  // end of if()
        
        if( bSameLabelFound && bDiffLabelFound ) 
        {
          puSegMask[i] = uLabelTemp;
          break;
        }
      }
    }  // end of if()
  }  // end of for()
  
  // relabel labelTemp to labelNew
  for( i = 0; i < uNumVox; i++ ) 
  {
    if( puSegMask[i] == uLabelTemp ) 
    {
      puSegMask[i] = uLabelNew;
    }
  }  
}


/**  CAUTION: This function can only be called after sorting the average intensities
*   It merges classes with higher average intensities and connected each other
*
*   @return bool if it is true, the merging is down; if it is false, the merging
*                is not applied due to some reasons.
*/
bool MergeClasses::MergingMainParts()
{
  stringstream tt;
  
  // class label for temporarilly useage
  uint1 uTmpLabel1, uTmpLabel2;
  // the label for the class with the highest and the second highest 
  // average intensities
  uint1 u1stLabel, u2ndLabel;
  
  int4 i, j;
  int4 iNumOfClasses = m_vAverageIntensity.size();
  
  // get the labels of the highest label and set temporary labels.
  // the temporary labels should not be labels that have already been used.
  for( i = iNumOfClasses-1; i > 0; i-- ) 
  {
    if( m_vAverageIntensity[i] != 0 ) 
    {
      // this is the highest label
      u1stLabel = static_cast<uint1> (i);
      
      if( m_vAverageIntensity[i-1] != 0 ) 
      {
        // if yes, get the 2nd highest label
        u2ndLabel = static_cast<uint1> (i-1);
      }
      else 
      {
        LogWrn("MergeClasses: highest label was not found","MergeClasses", "MergingMainParts");
        return false;
      }
      
      // set the temporary label
      for( j = i-2; j > 1; j-- ) 
      {
        if( (m_vAverageIntensity[j] == 0)&&(m_vAverageIntensity[j-1] == 0) ) 
        {
          uTmpLabel1 = static_cast<uint1> (j);
          uTmpLabel2 = static_cast<uint1> (j-1);
          
          // exit for loops
          i = -1;  j = -1;
        }  
      }
      
      if( i == 1) 
      {
        LogWrn("MergeClasses: no available caps for temporary labels", "MergeClasses", "MergingMainParts" );
        return false;
      }      
    }  // end of if
  }  // end of for
  
  // get volume dimension
  Triple<uint4> dim = m_Volume.GetDim();
  uint4 uNumVox = m_Volume.GetNumberOfVoxels();
  int4 iSliceStep = dim.m_x*dim.m_y;
  
  // get pointer to the classification mask 
  uint1 * piClassMask = m_SegMask.GetDataPtr();
  
  // queue for region grow
  std::vector<int4> queueGrow1st;
  std::vector<int4> queueGrow2nd;
  // index for the last and first custumer in the queue
  int4 iLastInQueue, iFirstInQueue;
  
  // 26-connected neighbor offsets
  int4 vNeighbOffset[26] = 
  {
    1, static_cast<int4>(dim.m_x), -1, -static_cast<int4>(dim.m_x), 
      static_cast<int4>(dim.m_x)-1, static_cast<int4>(dim.m_x)+1, 
      -static_cast<int4>(dim.m_x)-1, -static_cast<int4>(dim.m_x)+1,
      iSliceStep, iSliceStep+1, iSliceStep+static_cast<int4>(dim.m_x), iSliceStep-1,
      iSliceStep-static_cast<int4>(dim.m_x), iSliceStep+static_cast<int4>(dim.m_x)+1,
      iSliceStep+static_cast<int4>(dim.m_x)-1, iSliceStep-static_cast<int4>(dim.m_x)+1,
      iSliceStep-static_cast<int4>(dim.m_x)-1,
      -iSliceStep, -iSliceStep+1, -iSliceStep+static_cast<int4>(dim.m_x), -iSliceStep-1,
      -iSliceStep-static_cast<int4>(dim.m_x), -iSliceStep+static_cast<int4>(dim.m_x)+1,
      -iSliceStep+static_cast<int4>(dim.m_x)-1, -iSliceStep-static_cast<int4>(dim.m_x)+1,
      -iSliceStep-static_cast<int4>(dim.m_x)-1
  };    
  
  // horizental square offset
  int4 vHorizSquareOffset[25] =
  {
    -2, -1, 0, 1, 2,
      iSliceStep-2, iSliceStep-1, iSliceStep, iSliceStep+1, iSliceStep+2,
      2*iSliceStep-2, 2*iSliceStep-1, 2*iSliceStep, 2*iSliceStep+1, 2*iSliceStep+2,
      -iSliceStep-2, -iSliceStep-1, -iSliceStep, -iSliceStep+1, -iSliceStep+2,
      -2*iSliceStep-2, -2*iSliceStep-1, -2*iSliceStep, -2*iSliceStep+1, -2*iSliceStep+2
  };
  
  int4 k, m, n, s, t;
  // indicator for detecting regions with two up-most labels 
  bool bIndicatorForUp1 = true;
  bool bIndicatorForUp2 = false;
  // merge indicator
  bool bIndicatorMerge = false;
  
  // average intensities for regions with two up-most labels
  int4 iAverageIntUp1st, iAverageIntUp2nd;
  iAverageIntUp1st = 0;
  iAverageIntUp2nd = 0;
  
  // get pointer to the volume 
  int2 * piVPtr = m_Volume.GetDataPtr();
  // check each region with the highest label and applying merge procedure
  // if the following conditions are satisfied. 
  // Merge condition:
  //  1.  The region with the highest label has number of voxels larger than 
  //      pre-set threshold.
  //  2.  There is horizontal plane with in the second highest label region
  //  3.  The two most highest label regions have similar average intensities
  //  4.  The two most highest label regions is connected to each other
  //  5.  The region with the 2nd highest label has depth larger than pre-set
  //      threshold
  for( i = 1; i < uNumVox; i++ )
  {
    // empty region growing queues
    queueGrow1st.clear();
    queueGrow2nd.clear();
    
    // Is this voxel labeled with the highest label
    if( piClassMask[i] == u1stLabel ) 
    {
      // if yes, grow out this region (26-connected)
      queueGrow1st.push_back( i );
      bIndicatorForUp1 = true;
      
      iLastInQueue = 0;
      iFirstInQueue = -1;
      
      // grow -------------------------------------
      while( iFirstInQueue < iLastInQueue ) 
      {
        iFirstInQueue++;      
        
        // the first custumer in the queue
        j = queueGrow1st[ iFirstInQueue ];
        // label the voxel
        piClassMask[ j ] = uTmpLabel1;
        
        // check its 26-connected neighbors
        for( k = 0; k < 26; k++) 
        {
          // get neighbor offset
          m = j + vNeighbOffset[ k ];
          
          // Is this neighbor with the highest label
          if( (m > 0)&&(m < uNumVox)&&(piClassMask[m] == u1stLabel) ) 
          {
            // if yes,
            queueGrow1st.push_back( m );  
            iLastInQueue++;
            
            piClassMask[m] = uTmpLabel1;
          }  // end of if
        }  // end of for(k)
      }  // end of while(queue)
      
      // if the number of voxel in this region is less than pre-set threshold
      // stop applying merge procedure, threshold 216 = 6*6*6
      if( queueGrow1st.size() < 216 ) 
      {
        /// Error::DebugMsg("segmentation: found region is too small for dominating class");
        bIndicatorForUp1 = false;
      }
      // end of grow ------------------------------
      
      if( bIndicatorForUp1 ) 
      {
        bIndicatorForUp2 = false;
        
        // grow out all regions with label u2ndLabel and connected to current region
        for( j = 0; j < queueGrow1st.size(); j++) 
        {
          // Is one of its neighbor with label u2ndLabel
          for( k = 0; k < 26; k++) 
          {
            t = queueGrow1st[j] + vNeighbOffset[k];
            
            if( (t > 0)&&(t < uNumVox)&&(piClassMask[t] == u2ndLabel) ) 
            {
              // a up2Label region is found
              bIndicatorForUp2 = true;
              
              // grow out from this neighbor =====================
              queueGrow2nd.push_back( t );
              piClassMask[ t ] = uTmpLabel2;
              
              iLastInQueue = 0;
              iFirstInQueue = -1;
              
              while( iFirstInQueue < iLastInQueue ) 
              {
                iFirstInQueue++;
                
                // the first custumer in the queue
                n = queueGrow2nd[ iFirstInQueue ];
                
                // check its 26-connected neighbors
                for( s = 0; s < 26; s++) 
                {
                  // get neighbor offset
                  m = n + vNeighbOffset[ s ];
                  
                  // Is this neighbor with the second highest label
                  if( (m > 0)&&(m < uNumVox)&&(piClassMask[m] == u2ndLabel) ) 
                  {
                    // if yes,
                    queueGrow2nd.push_back( m );
                    iLastInQueue++;
                    
                    piClassMask[m] = uTmpLabel2;
                  }  // end of if
                }  // end of for(k)
              }  // end of while(queue)
              // end of grow out =================================
              
              // exit all loop
              k = 27;  j = queueGrow1st.size() + 1;
            }      
          }
        } 
        
        if( bIndicatorForUp2 ) 
        { 
          // calculate average intensities for these two regions
          for( j = 0; j < queueGrow1st.size(); j++ )
          {
            iAverageIntUp1st = iAverageIntUp1st + static_cast<int4> ( piVPtr[queueGrow1st[j]] );
          }
          iAverageIntUp1st = iAverageIntUp1st/queueGrow1st.size();
          
          for( j = 0; j < queueGrow2nd.size(); j++)
          {
            iAverageIntUp2nd = iAverageIntUp2nd + static_cast<int4> ( piVPtr[queueGrow2nd[j]] );
          }
          
          iAverageIntUp2nd = iAverageIntUp2nd/queueGrow2nd.size();
          
          if( ((iAverageIntUp1st - iAverageIntUp2nd) > 0)&&
            ((iAverageIntUp1st - iAverageIntUp2nd) <= 400) ) 
          {
            // indicator for horizon plane
            bool bIndPlane;
            
            // detecting if there is a horizon square in Up2ndLabel region
            for( j = 0; j < queueGrow2nd.size(); j++ ) 
            { 
              // initializing indicator
              bIndPlane = true;
              
              // check square neighbor
              for( m = 0; m < 25; m++ )  
              {
                s = vHorizSquareOffset[m] + queueGrow2nd[j];
                
                if( (s > 0)&&(s < uNumVox)&&(piClassMask[s] != uTmpLabel2) ) 
                {
                  // set the indicator
                  bIndPlane = false;
                  // exit the square check
                  m = 26;
                }
              }
              
              // check the depth of the Up2ndLabel region
              if( bIndPlane ) 
              {
                for( m = 1; m < 6; m++) 
                {
                  s = m*dim.m_x + queueGrow2nd[j];
                  
                  if( (s > 0)&&(s < uNumVox)&&(piClassMask[s] != uTmpLabel2) ) 
                  {
                    // set the indicator
                    bIndPlane = false;
                    // exit the depth check
                    m = 7;
                  }
                }
              }
              
              if( bIndPlane )
              {
                // merge these two classes
                for( k = 0; k < queueGrow2nd.size(); k++) 
                {
                  piClassMask[ queueGrow2nd[k] ] = uTmpLabel1;
                  
                  // set indicator for merging occurance
                  bIndicatorMerge = true;
                  
                  // exit for loop(j)
                  j = queueGrow2nd.size() + 1;
                }
              }
            }  // end of for( j )
          } //  end of if( averageInten ..... )
        }  //  end of if( indicatorForUp2 )         
      }  // end of if( indicatorForUp1 )
      
      // reset the indicator
      bIndicatorForUp1 = true;
      bIndicatorForUp2 = false;
    }  // end of if
  }  // end of for(i), the main for loop !!! 
  
  // reset the temporary labels to real label
  for( i = 0; i < uNumVox; i++ ) 
  {
    if( piClassMask[i] == uTmpLabel1 ) 
    {
      piClassMask[i] = u1stLabel;
    }
    else if( piClassMask[i] == uTmpLabel2 )
    {
      piClassMask[i] = u2ndLabel;
    }
  }
  
  return (bIndicatorMerge); 
}  // end of MergingMainParts()






// complete revision history:
// $Log: MergeClasses.C,v $
// Revision 1.3  2005/11/15 13:12:19  mkontak
// Fix to work with the new WQ and Console 3.0
//
// Revision 1.2  2005/07/01 12:46:05  vxconfig
// Added new Logger.
//
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
//
// Revision 3.2  2002/08/05 20:42:32  dongqing
// rename the Logger to vxLogger due to the re-name of Logger Class
//
// Revision 3.1  2001/10/26 02:19:15  tushar
// removed errors due to changes made to logger class
//
// Revision 3.0  2001/10/14 23:01:45  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:38:06   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:24:22   ingmar
// Initial revision.
// Revision 1.6  2001/08/31 18:54:19  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.5  2001/08/13 20:43:16  sachin
// changed exceptions ,warnings,debug and information
// according to the new logger class
//
// Revision 1.4  2001/08/07 17:04:23  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.3  2001/06/25 22:21:20  uday
// coding conventions for the segmentation module
//
// Revision 1.2  2001/05/23 21:37:42  dongqing
// rewrite Seg module
//
// Revision 1.1.2.5  2001/05/11 21:18:13  dongqing
// new
//
// Revision 1.1.2.4  2001/05/10 21:40:58  dongqing
// re-write seg module
//
// Revision 1.1.2.3  2001/05/09 22:07:01  dongqing
// new
//
// Revision 1.1.2.2  2001/05/08 21:38:33  dongqing
// new
//
// Revision 1.1.2.1  2001/05/03 15:12:39  dongqing
// new
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/MergeClasses.C,v 1.3 2005/11/15 13:12:19 mkontak Exp $
// $Id: MergeClasses.C,v 1.3 2005/11/15 13:12:19 mkontak Exp $