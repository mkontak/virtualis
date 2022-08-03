// $Id: ReaderGlobalRegistration.C,v 1.5.2.1 2009/01/10 07:25:47 jmeade Exp $
//
// Copyright (c) 2000,2001 Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: S. Yoakum-Stover, Ana Paula Centeno   ana@viatronix.net
// Owner: Kevin Kreeger (kevin@viatronix.com)


// includes
#include "StdAfx.h"
#include <limits>
#include "ReaderGlobal.h"
#include "PrimitiveUtilities.h"
#include "Line.h"


// defines
#define FILE_REVISION "$Revision: 1.5.2.1 $"
#ifdef max
#undef max
#endif


// namespaces
using namespace ReaderLib;


/**
 * Fill in the matrix signifying how well each possible match compares to all others
 */
void ReaderGlobal::BuildMatchQuality()
{
  // Kevins NORMALIZED (sum to 1) without Rank's. (originated from 9/24 row-normal)
  float4 fEuclidDistWeight = 0.339527027F;
  float4 fInVecSimWeight   = 0.205236486F;
  float4 fOutVecSimWeight  = 0.190878378F;
  float4 fInOutSimWeight   = 0.264358108F;

  //////////////////////////////////////////////////
  // Keep these weights around for future reference.
  // These were developed with a lot of effort and
  // it would be a shame to lose it.
  //////////////////////////////////////////////////

  // Kevins NORMALIZED (sum to 1) 9/24 row-normal
  //float4 fEuclidDistWeight = 0.324979790F;
  //float4 fInVecSimWeight   = 0.196443007F;
  //float4 fOutVecSimWeight  = 0.182700081F;
  //float4 fInOutSimWeight   = 0.253031528F;
  //float4 fZRankSimWeight   = 0.004042037F;
  //float4 fXRankSimWeight   = 0.033144705F;
  //float4 fYRankSimWeight   = 0.005658852F;
  //float4 fOffset           = 0.0F;
  
  // 9/24 row-normal
  //float4 fEuclidDistWeight = 0.0402F;
  //float4 fInVecSimWeight   = 0.0243F;
  //float4 fOutVecSimWeight  = 0.0226F;
  //float4 fInOutSimWeight   = 0.0313F;
  //float4 fZRankSimWeight   = 0.0005F;
  //float4 fXRankSimWeight   = 0.0041F;
  //float4 fYRankSimWeight   = 0.0007F;
  //float4 fOffset           = 0.0282F;
  
  // Original weights
  //float4 fEuclidDistWeight = 0.3416F;
  //float4 fInVecSimWeight   = 0.0817F;
  //float4 fOutVecSimWeight  = 0.0742F;
  //float4 fInOutSimWeight   = 0.1028F;
  //float4 fZRankSimWeight   = 0.0186F;
  //float4 fXRankSimWeight   = 0.0525F;
  //float4 fYRankSimWeight   = 0.0035F;
  //float4 fOffset           =-1.1802F;

  /// Compute the cross-out matrix
  ComputeInCrossOutMatrix();

#ifndef FINAL_RELEASE
  m_mCrossProducts.PrintToFile("InCrossOutMetric");
#endif
  ComputeOutVectorMatrix();
#ifndef FINAL_RELEASE
  m_mOutVectors.PrintToFile("OutVectorMetric");
#endif
  ComputeInVectorMatrix();
#ifndef FINAL_RELEASE
  m_mInVectors.PrintToFile("InVectorMetric");
#endif
  ComputeEuclideanDistanceMatrix();
#ifndef FINAL_RELEASE
  m_mEuclideanDistances.PrintToFile("EuclideanDistanceMetric");
#endif

  m_mMatchQuality.Resize(m_supine.m_ivFeatures.size(),m_prone.m_ivFeatures.size());
  for (uint4 r=0; r<m_supine.m_ivFeatures.size(); r++)
  {
    for (uint4 c=0; c<m_prone.m_ivFeatures.size(); c++)
    {
      m_mMatchQuality(r,c) = (m_mEuclideanDistances(r,c) * fEuclidDistWeight) +
                             (m_mInVectors(r,c)          * fInVecSimWeight  ) +  
                             (m_mOutVectors(r,c)         * fOutVecSimWeight ) + 
                             (m_mCrossProducts(r,c)      * fInOutSimWeight  );
    } // for each prone feature
  } // for each supine feature
#ifndef FINAL_RELEASE
  m_mMatchQuality.PrintToFile("MatchQuality");
#endif
  
  //compute the mean and standard deviation
  float4 sum(0);
  float4 sumSquares(0);
  float4 fCount = m_supine.m_ivFeatures.size()*m_prone.m_ivFeatures.size();
  for (uint4 r=0; r<m_supine.m_ivFeatures.size(); r++)
  {
    for (uint4 c=0; c<m_prone.m_ivFeatures.size(); c++)
    {
      sum += m_mMatchQuality(r,c);
      sumSquares += m_mMatchQuality(r,c) * m_mMatchQuality(r,c);
    } // for each prone feature
  } // for each supine feature
  m_fRegistrationMatchMean   = (fCount == 0) ? 0 : sum / fCount;
  m_fRegistrationMatchStdDev = (fCount == 1) ? 1 : sqrt((sumSquares - sum*sum/fCount)/(fCount -1));
#ifndef FINAL_RELEASE
  std::ofstream outStream("C:/V-System/Log/Registration.txt", std::ios::app);  outStream.precision(4);
  outStream.setf(std::ios::showpoint | std::ios::fixed | std::ios::showpos);
  outStream << " mean " << m_fRegistrationMatchMean << " StdDev " << m_fRegistrationMatchStdDev << std::endl;
  outStream.close();
#endif

  ComputeConfidence();
#ifndef FINAL_RELEASE
  m_mConfidence.PrintToFile("ConfidenceMatrix");  
#endif
} // BuildMatchQuality()


/**
 * Fill in the matrix signifying how well each possible match compares regarding to 3D position (normalized to center of gravity)
 */
void ReaderGlobal::ComputeEuclideanDistanceMatrix()
{
  
  m_mEuclideanDistances.Resize(m_supine.m_ivFeatures.size(),m_prone.m_ivFeatures.size());
  
  Vector<float4> coGravityShift(m_supine.m_joinedSkeleton.GetCenterOfGravity() - m_prone.m_joinedFlippedSkeleton.GetCenterOfGravity());
  
#ifndef FINAL_RELEASE
  //std::ofstream outStream("C:/V-System/Log/Registration.txt", std::ios::app); outStream.precision(4); outStream.setf(std::ios::showpoint | std::ios::fixed | std::ios::showpos);
  //outStream << "Center of gravity shift." << std::endl << coGravityShift << std::endl;
#endif
  
  for (uint4 r=0; r<m_supine.m_ivFeatures.size(); r++)
  {
    const Point<float4> vSupinePos = m_supine.m_joinedSkeleton.GetWorldPos(m_supine.m_ivFeatures[r]);
    for (uint4 c=0; c<m_prone.m_ivFeatures.size(); c++)
    {
      Point<float4> vProneAdjustedPos = m_prone.m_joinedFlippedSkeleton.GetWorldPos(m_prone.m_ivFeatures[c]) + coGravityShift;
      m_mEuclideanDistances(r,c) = vSupinePos.GetEuclideanDistance(vProneAdjustedPos);
    } // for each prone feature
  } // for each supine feature
  
  float4 fMaxValue = 0;
  for (uint4 r=0; r<m_supine.m_ivFeatures.size(); r++)
  {
    for (uint4 c=0; c<m_prone.m_ivFeatures.size(); c++)
    {
      if(m_mEuclideanDistances(r,c)>fMaxValue) fMaxValue = m_mEuclideanDistances(r,c);
    } // for each prone feature
  } // for each supine feature
#ifndef FINAL_RELEASE
  //outStream << "ED max value." << std::endl << fMaxValue << std::endl;
#endif


  for (uint4 r=0; r<m_supine.m_ivFeatures.size(); r++)
  {

    for (uint4 c=0; c<m_prone.m_ivFeatures.size(); c++)
    {
      m_mEuclideanDistances(r,c) = 1.0F - m_mEuclideanDistances(r,c)/fMaxValue;
    } // END ... for each prone feature

  } // END ... for each supine feature

} // ComputeEuclideanDistanceMatrix()


/**
 * Fill in the matrix signifying how well each possible match compares regarding to inXout-vectors
 * Note: this signifies the orientation of each bend.
 */
void ReaderGlobal::ComputeInCrossOutMatrix()
{
  const uint4 uFeatureResolution = 20;
  m_mCrossProducts.Resize(m_supine.m_ivFeatures.size(),m_prone.m_ivFeatures.size());
  
  // for each supine feature
  for (uint4 r=0; r<m_supine.m_ivFeatures.size(); r++)
  {
    Vector<float4> vSupineInNormalized(m_supine.m_joinedSkeleton.GetWorldPos(m_supine.m_ivFeatures[r]) -
                                       m_supine.m_joinedSkeleton.GetWorldPos(m_supine.m_ivFeatures[r]-uFeatureResolution));
    vSupineInNormalized.Normalize();
    Vector<float4> vSupineOutNormalized(m_supine.m_joinedSkeleton.GetWorldPos(m_supine.m_ivFeatures[r]+uFeatureResolution) -
                                        m_supine.m_joinedSkeleton.GetWorldPos(m_supine.m_ivFeatures[r]));
    vSupineOutNormalized.Normalize();
    Vector<float4> vSupineInCrossOut = vSupineInNormalized.Cross(vSupineOutNormalized);
    vSupineInCrossOut.Normalize();

    // for each prone feature
    for (uint4 c=0; c<m_prone.m_ivFeatures.size(); c++)
    {
      Vector<float4> vProneInNormalized(m_prone.m_joinedFlippedSkeleton.GetWorldPos(m_prone.m_ivFeatures[c]) - 
                                        m_prone.m_joinedFlippedSkeleton.GetWorldPos(m_prone.m_ivFeatures[c]-uFeatureResolution));
      vProneInNormalized.Normalize();
      Vector<float4> vProneOutNormalized(m_prone.m_joinedFlippedSkeleton.GetWorldPos(m_prone.m_ivFeatures[c]+uFeatureResolution) - 
                                         m_prone.m_joinedFlippedSkeleton.GetWorldPos(m_prone.m_ivFeatures[c]));
      vProneOutNormalized.Normalize();
      Vector<float4> vProneInCrossOut = vProneInNormalized.Cross(vProneOutNormalized);
      vProneInCrossOut.Normalize();

      m_mCrossProducts(r,c) = vSupineInCrossOut.Dot(vProneInCrossOut);
    } // END ... for each prone feature

  } // END ... for each supine feature

} // ComputeInCrossOutMatrix()


/**
 * Fill in the matrix signifying how well each possible match compares regarding to in-vectors
 */
void ReaderGlobal::ComputeInVectorMatrix()
{
  const uint4 uFeatureResolution = 20;
  m_mInVectors.Resize(m_supine.m_ivFeatures.size(),m_prone.m_ivFeatures.size());
  
  //////////////////////////////
  // for each supine feature
  //////////////////////////////
  for (uint4 r=0; r<m_supine.m_ivFeatures.size(); r++)
  {
    Vector<float4> vSupineInNormalized(m_supine.m_joinedSkeleton.GetWorldPos(m_supine.m_ivFeatures[r]) -
                                       m_supine.m_joinedSkeleton.GetWorldPos(m_supine.m_ivFeatures[r]-uFeatureResolution));
    vSupineInNormalized.Normalize();

    ///////////////////////////
    // for each prone feature
    ///////////////////////////
    for (uint4 c=0; c<m_prone.m_ivFeatures.size(); c++)
    {
      Vector<float4> vProneInNormalized(m_prone.m_joinedFlippedSkeleton.GetWorldPos(m_prone.m_ivFeatures[c]) - 
                                        m_prone.m_joinedFlippedSkeleton.GetWorldPos(m_prone.m_ivFeatures[c]-uFeatureResolution));
      vProneInNormalized.Normalize();
      m_mInVectors(r,c) = vSupineInNormalized.Dot(vProneInNormalized);
    } // END ... for each prone feature

  } // END ... for each supine feature

} // ComputeInVectorMatrix()


/**
 * Fill in the matrix signifying how well each possible match compares regarding to out-vectors
 */
void ReaderGlobal::ComputeOutVectorMatrix()
{
  const uint4 uFeatureResolution = 20;
  m_mOutVectors.Resize(m_supine.m_ivFeatures.size(),m_prone.m_ivFeatures.size());
  
  ////////////////////////////
  // for each supine feature
  /////////////////////////////
  for (uint4 r=0; r<m_supine.m_ivFeatures.size(); r++)
  {
    Vector<float4> vSupineOutNormalized(m_supine.m_joinedSkeleton.GetWorldPos(m_supine.m_ivFeatures[r]+uFeatureResolution) -
                                        m_supine.m_joinedSkeleton.GetWorldPos(m_supine.m_ivFeatures[r]));
    vSupineOutNormalized.Normalize();

    ////////////////////////////
    // for each prone feature
    //////////////////////////
    for (uint4 c=0; c<m_prone.m_ivFeatures.size(); c++)
    {
      Vector<float4> vProneOutNormalized(m_prone.m_joinedFlippedSkeleton.GetWorldPos(m_prone.m_ivFeatures[c]+uFeatureResolution) - 
                                         m_prone.m_joinedFlippedSkeleton.GetWorldPos(m_prone.m_ivFeatures[c]));
      vProneOutNormalized.Normalize();
      m_mOutVectors(r,c) = vSupineOutNormalized.Dot(vProneOutNormalized);
    } // END ... for each prone feature

  } // END ... for each supine feature
} // ComputeOutVectorMatrix()


/**
 * Fill in the matrix signifying the confidence of each possible match which is how many standard 
 * deviations each match quality is above (or below if negative) the 2nd highest quality.
 * Thus a high numver emans a feature can only possibly match a single other feature otherwise
 * it will be negative (signifiying it is not one of the top two matches).
 * Horizontal compares each prone feature to all supine features.
 */
void ReaderGlobal::ComputeHorizConfidence()
{
#ifndef FINAL_RELEASE
  std::ofstream outStream("C:/V-System/Log/Registration.txt", std::ios::app); outStream.precision(4);
  outStream.setf(std::ios::showpoint | std::ios::fixed | std::ios::showpos);
#endif
  
  m_mHorizConfidence.Resize(m_supine.m_ivFeatures.size(),m_prone.m_ivFeatures.size());
  
  // Deal with small segments. If only 1 feature in prone, then set conf to 1.0 so
  // that Vert conf will be used. If both datasets have only 1 feature set confidence to 0.0F
  if(m_prone.m_ivFeatures.size() == 0) return;
  if(m_prone.m_ivFeatures.size() == 1)
  {
    if(m_supine.m_ivFeatures.size() == 1)
    {
      m_mHorizConfidence(0,0) = 0.0F;
    }
    else
    {
      for (uint4 r=0; r<m_supine.m_ivFeatures.size(); r++) m_mHorizConfidence(r,0) = 1.0F;
    }
    return;
  } // endif only 1 feature

  ///////////////////////////
  // for each supine feature
  ///////////////////////////
  for (uint4 r=0; r<m_supine.m_ivFeatures.size(); r++)
  {
    float4 fRowMax       = (m_mMatchQuality(r,0) > m_mMatchQuality(r,1)) ? m_mMatchQuality(r,0) : m_mMatchQuality(r,1);
    float4 fRowSecondMax = (m_mMatchQuality(r,0) > m_mMatchQuality(r,1)) ? m_mMatchQuality(r,1) : m_mMatchQuality(r,0);
    
    //////////////////////////
    // for each prone feature
    ///////////////////////////
    for (uint4 c=2; c<m_prone.m_ivFeatures.size(); c++)
    {
      if(m_mMatchQuality(r,c) > fRowSecondMax && m_mMatchQuality(r,c) > fRowMax)
      {
        fRowSecondMax = fRowMax;
        fRowMax = m_mMatchQuality(r,c);
      }
      else if(m_mMatchQuality(r,c) > fRowSecondMax)
      {
        fRowSecondMax = m_mMatchQuality(r,c);
      }
    } // END ... for each prone feature

#ifndef FINAL_RELEASE
    outStream << "Row " << r << " Max = " << fRowMax << " Second Max = " << fRowSecondMax << std::endl;
#endif
    for (uint4 c=0; c<m_prone.m_ivFeatures.size(); c++)
    {
      m_mHorizConfidence(r,c) = (m_mMatchQuality(r,c) - fRowSecondMax)/m_fRegistrationMatchStdDev;
    } // END ... for each prone feature

  } // END ... for each supine feature

} // ComputeHorizConfidence()


/**
 * Fill in the matrix signifying the confidence of each possible match which is how many standard 
 * deviations each match quality is above (or below if negative) the 2nd highest quality.
 * Thus a high numver emans a feature can only possibly match a single other feature otherwise
 * it will be negative (signifiying it is not one of the top two matches).
 * Vertical compares each prone feature to all supine features.
 */
void ReaderGlobal::ComputeVertConfidence()
{
#ifndef FINAL_RELEASE
  std::ofstream outStream("C:/V-System/Log/Registration.txt", std::ios::app); outStream.precision(4);
  outStream.setf(std::ios::showpoint | std::ios::fixed | std::ios::showpos);
#endif
  
  m_mVertConfidence.Resize(m_supine.m_ivFeatures.size(),m_prone.m_ivFeatures.size());
  
  // Deal with small segments. If only 1 feature in supine, then set conf to 1.0 so
  // that Horiz conf will be used. If both datasets have only 1 feature set confidence to 0.0F
  if(m_supine.m_ivFeatures.size() == 0) return;

  /////////////////////////////////
  // If only 1 supine feature
  ///////////////////////////////
  if( m_supine.m_ivFeatures.size() == 1 )
  {

    ///////////////////////////////
    // If only one prone feature
    //////////////////////////////
    if(m_prone.m_ivFeatures.size() == 1)
    {
      m_mHorizConfidence(0,0) = 0.0F;
    }
    else
    {
      for (uint4 c=0; c<m_prone.m_ivFeatures.size(); c++) m_mHorizConfidence(0,c) = 1.0F;
    }

    return;

  } // END ... if only 1 feature

  ///////////////////////////
  // for each prone feature
  ///////////////////////////
  for (uint4 c=0; c<m_prone.m_ivFeatures.size(); c++)
  {
    float4 fColMax       = (m_mMatchQuality(0,c) > m_mMatchQuality(1,c)) ? m_mMatchQuality(0,c) : m_mMatchQuality(1,c);
    float4 fColSecondMax = (m_mMatchQuality(0,c) > m_mMatchQuality(1,c)) ? m_mMatchQuality(1,c) : m_mMatchQuality(0,c);
    
    /////////////////////////////
    // for each supine feature
    //////////////////////////////
    for (uint4 r=2; r<m_supine.m_ivFeatures.size(); r++)
    {
      if(m_mMatchQuality(r,c) > fColSecondMax && m_mMatchQuality(r,c) > fColMax)
      {
        fColSecondMax = fColMax;
        fColMax = m_mMatchQuality(r,c);
      }
      else if(m_mMatchQuality(r,c) > fColSecondMax)
      {
        fColSecondMax = m_mMatchQuality(r,c);
      }
    } // END ... for each prone feature

#ifndef FINAL_RELEASE
    outStream << "Column " << c << " Max = " << fColMax << " Second Max = " << fColSecondMax << std::endl;
#endif

    ////////////////////////////
    // for each supine feature
    ///////////////////////////
    for (uint4 r=0; r<m_supine.m_ivFeatures.size(); r++)
    {
      m_mVertConfidence(r,c) = (m_mMatchQuality(r,c) - fColSecondMax)/m_fRegistrationMatchStdDev;
    } // END ... for each supine feature

  } // END ... for each prone feature

} // ComputeVertConfidence()

/**
 * Fill in the matrix signifying the confidence of each possible match
 */
void ReaderGlobal::ComputeConfidence()
{  
  ComputeHorizConfidence();
#ifndef FINAL_RELEASE
  m_mHorizConfidence.PrintToFile("HorizConfidenceMatrix");
#endif
  ComputeVertConfidence();
#ifndef FINAL_RELEASE
  m_mVertConfidence.PrintToFile("VertConfidenceMatrix");
#endif
  
  m_mConfidence.Resize(m_supine.m_ivFeatures.size(),m_prone.m_ivFeatures.size());
  
  /////////////////////////////
  // for each supine feature
  /////////////////////////////
  for (uint4 r=0; r<m_supine.m_ivFeatures.size(); r++)
  {

    /////////////////////////////
    // for each prone feature
    /////////////////////////////
    for (uint4 c=0; c<m_prone.m_ivFeatures.size(); c++)
    {
      m_mConfidence(r,c) = min(m_mHorizConfidence(r,c),m_mVertConfidence(r,c));
    } // END ... for each prone feature

  } // END ... for each supine feature

} // ComputeConfidence()


/**
 * Given 2 endpoints compute the stretch of the colon at some potential match point between them
 *
 * @param startFeature          Endpoints tied down.
 * @param stopFeature           Endpoints tied down.
 * @param potentialMatchFeature Test points.
 * @return                      Amount of stretch.
 */
float4 ReaderGlobal::Stretch(const MatchingFeature startFeature, const MatchingFeature stopFeature,const MatchingFeature potentialMatchFeature)
{
  // Distance between the endpoints
  float4 fSupineDistanceRange =  m_supine.m_joinedSkeleton.GetWorldDfs(          stopFeature.m_uSupineFeatureIndex) -
                                 m_supine.m_joinedSkeleton.GetWorldDfs(         startFeature.m_uSupineFeatureIndex);
  float4 fProneDistanceRange  =   m_prone.m_joinedSkeleton.GetWorldDfs(          stopFeature.m_uProneFeatureIndex ) -
                                  m_prone.m_joinedSkeleton.GetWorldDfs(         startFeature.m_uProneFeatureIndex );
  // Distance to the test point
  float4 fSupinePotentialRange = m_supine.m_joinedSkeleton.GetWorldDfs(potentialMatchFeature.m_uSupineFeatureIndex) -
                                 m_supine.m_joinedSkeleton.GetWorldDfs(         startFeature.m_uSupineFeatureIndex);
  float4 fPronePotentialRange  =  m_prone.m_joinedSkeleton.GetWorldDfs(potentialMatchFeature.m_uProneFeatureIndex ) -
                                  m_prone.m_joinedSkeleton.GetWorldDfs(         startFeature.m_uProneFeatureIndex );
  
  // Represent it as a percent along the total range (makes it a stretch both before AND after the point)
  float4 fSupinePercent = (fSupineDistanceRange != 0.0F) ? fSupinePotentialRange/fSupineDistanceRange : 0.5F;
  float4 fPronePercent  = ( fProneDistanceRange != 0.0F) ?  fPronePotentialRange/fProneDistanceRange : 0.5F;
  
  // Stretch from Supine to Prone
  float4 fStretch = (fPronePercent != 0.0F) ? fSupinePercent/fPronePercent : 1.0F;

  // In case it is a supine->prone compression make it a prone->supine stretch. (like absolute value)
  if(fStretch != 0.0F && fStretch<1.0F) fStretch = 1.0/fStretch;

  return fStretch;

} // Stretch(const MatchingFeature startFeature, const MatchingFeature stopFeature,const MatchingFeature potentialMatchFeature)


/**
 * Given 2 features compute decide if a feature is not too close
 * Note: the logic or the name should be reversed so as to avoid double negatives
 *
 * @param feature1          TestPoint 1.
 * @param feature2          TestPoint 2.
 * @return                  Outside the threshold distance.
 */
bool ReaderGlobal::FeaturesNotTooClose(const MatchingFeature feature1, const MatchingFeature feature2)
{
  const uint4 uSupineDistance( abs(static_cast<long>(feature1.m_uSupineFeatureIndex - feature2.m_uSupineFeatureIndex)));
  const uint4 uProneDistance ( abs(static_cast<long>(feature1.m_uProneFeatureIndex  - feature2.m_uProneFeatureIndex)));
  const uint4 uThreshold(30);
  return (uSupineDistance > uThreshold) && (uProneDistance > uThreshold);
} // FeaturesNotTooClose(const MatchingFeature feature1, const MatchingFeature feature2)


/**
 * This is the recursive registration function to register 2 sub-sections of the colon
 *
 * @param startFeature                   Endpoints tied down.
 * @param stopFeature                    Endpoints tied down.
 * @param iStartRow                      Endpoints tied down in the matrices.
 * @param iStopRow                       Endpoints tied down in the matrices.
 * @param iStartCol                      Endpoints tied down in the matrices.
 * @param iStopCol                       Endpoints tied down in the matrices.
 * @param uLevel                         For debugging the recursion.
 * @return                               Amount of stretch.
 */
std::vector<MatchingFeature> ReaderGlobal::FindSeeds(const MatchingFeature startFeature, const MatchingFeature stopFeature,
                                                     const int4 iStartRow, const int4 iStopRow, const int4 iStartCol, const int4 iStopCol,
                                                     const uint4 uLevel)
{
#ifndef FINAL_RELEASE
  std::ofstream outStream("C:/V-System/Log/Registration.txt", std::ios::app); outStream.precision(4); outStream.setf(std::ios::showpoint | std::ios::fixed | std::ios::showpos);
  for(uint4 i=0;i<uLevel;i++) outStream << "|  ";
  outStream << "Processing (" << iStartRow <<".."<< iStopRow <<":"<< iStartCol <<".."<< iStopCol << ")" << std::endl;
#endif

  const float4 fStretchThreshold( 1.5F );
  std::vector<MatchingFeature> featureList;
  
  if(iStartRow > iStopRow || iStartCol > iStopCol)
  {
    // In either supine or prone there are no features in the current range.
    // return an empty list.
    return featureList;
  }
  else if(iStartRow == iStopRow && iStartCol == iStopCol) 
  {
    // In both supine or prone there is only one feature in the range.
    // Check that it's match quality is of sufficent strength and the stretch is OK
    MatchingFeature potentialMatchFeature(m_supine.m_ivFeatures[iStartRow],m_prone.m_ivFeatures[iStartCol],m_mConfidence(iStartRow,iStartCol),m_mMatchQuality(iStartRow,iStartCol));
    if(m_mMatchQuality(iStartRow,iStartCol) > m_fRegistrationMatchMean + m_fRegistrationMatchStdDev &&
       Stretch(startFeature, stopFeature, potentialMatchFeature) < fStretchThreshold &&
       FeaturesNotTooClose(startFeature, potentialMatchFeature) && FeaturesNotTooClose(stopFeature, potentialMatchFeature))
    {
#ifndef FINAL_RELEASE
      for(uint4 i=0;i<uLevel;i++) outStream << "|  ";
      outStream << "OnlyFeatureWithinRange (" << potentialMatchFeature.m_uSupineFeatureIndex <<":"<< potentialMatchFeature.m_uProneFeatureIndex << ") " <<
                   "from (" << iStartRow <<":"<< iStartCol << ") at " << 
                   m_mMatchQuality(iStartRow,iStartCol) << " quality and " << 
                   potentialMatchFeature.m_fConfidence << " confidence, producing " << 
                   Stretch(startFeature, stopFeature, potentialMatchFeature) << " stretch"  << std::endl;
#endif
      featureList.push_back(potentialMatchFeature);      
    }
  }
  else if(iStartRow == iStopRow) 
  {
    // In supine there is only one feature in the range.
    // Find the prone feature that it matches and verify that the
    // match quality is of sufficient strength and the stretch is OK
    float4 fBestMatchedConf = m_mConfidence(iStartRow,iStartCol);
    uint4 uBestMatchedCol = iStartCol;
    for (uint4 c=iStartCol; c<=iStopCol; c++)
    {
      if(m_mConfidence(iStartRow,c) > fBestMatchedConf)
      {
        fBestMatchedConf = m_mConfidence(iStartRow,c);
        uBestMatchedCol = c;
      }
    } // for each prone feature
    MatchingFeature potentialMatchFeature(m_supine.m_ivFeatures[iStartRow],m_prone.m_ivFeatures[uBestMatchedCol],m_mConfidence(iStartRow,uBestMatchedCol),m_mMatchQuality(iStartRow,uBestMatchedCol));
    if(m_mMatchQuality(iStartRow,uBestMatchedCol) > m_fRegistrationMatchMean + m_fRegistrationMatchStdDev &&
       Stretch(startFeature, stopFeature, potentialMatchFeature) < fStretchThreshold &&
       FeaturesNotTooClose(startFeature, potentialMatchFeature) && FeaturesNotTooClose(stopFeature, potentialMatchFeature))
    {
#ifndef FINAL_RELEASE
      for(uint4 i=0;i<uLevel;i++) outStream << "|  ";
      outStream << "OnlySupineFeatureWithinRange (" << potentialMatchFeature.m_uSupineFeatureIndex <<":"<< potentialMatchFeature.m_uProneFeatureIndex << ") " <<
                   "from (" << iStartRow <<":"<< uBestMatchedCol << ") at " << 
                   m_mMatchQuality(iStartRow,uBestMatchedCol) << " quality and " << 
                   potentialMatchFeature.m_fConfidence << " confidence, producing " << 
                   Stretch(startFeature, stopFeature, potentialMatchFeature) << " stretch" << std::endl;
#endif
      featureList.push_back(potentialMatchFeature);      
    }
  }
  else if(iStartCol == iStopCol)
  {
    // In prone there is only one feature in the range.
    // Find the supine feature that it matches and verify that the
    // match quality is of sufficient strength
    float4 fBestMatchedConf = m_mConfidence(iStartRow,iStartCol);
    uint4 uBestMatchedRow = iStartRow;
    for (uint4 r=iStartRow; r<=iStopRow; r++)
    {
      if(m_mConfidence(r,iStartCol) > fBestMatchedConf)
      {
        fBestMatchedConf = m_mConfidence(r,iStartCol);
        uBestMatchedRow = r;
      }
    } // for each prone feature
    MatchingFeature potentialMatchFeature(m_supine.m_ivFeatures[uBestMatchedRow],m_prone.m_ivFeatures[iStartCol],m_mConfidence(uBestMatchedRow,iStartCol),m_mMatchQuality(uBestMatchedRow,iStartCol));
    if(m_mMatchQuality(uBestMatchedRow,iStartCol) > m_fRegistrationMatchMean + m_fRegistrationMatchStdDev &&
       Stretch(startFeature, stopFeature, potentialMatchFeature) < fStretchThreshold &&
       FeaturesNotTooClose(startFeature, potentialMatchFeature) && FeaturesNotTooClose(stopFeature, potentialMatchFeature))
    {
#ifndef FINAL_RELEASE
      for(uint4 i=0;i<uLevel;i++) outStream << "|  ";
      outStream << "OnlyProneFeatureWithinRange (" << potentialMatchFeature.m_uSupineFeatureIndex <<":"<< potentialMatchFeature.m_uProneFeatureIndex << ") " <<
                     "from (" << uBestMatchedRow <<":"<< iStartCol << ") at " << 
                      m_mMatchQuality(uBestMatchedRow,iStartCol) << " quality and " << 
                      potentialMatchFeature.m_fConfidence << " confidence, producing " << 
                      Stretch(startFeature, stopFeature, potentialMatchFeature) << " stretch"  << std::endl;
#endif
      featureList.push_back(potentialMatchFeature);      
    }
  }
  else
  {
    // In both supine and prone there are at least 2 features in the current range.
    // Analyze the confidence matrix to pick the most confident feature. First check for
    // the highest confidence with acceptable quality and stretch, then if no features
    // are found which meet all these requirements, just the highest stretch-adjusted-quality.
    float4 fBestMatchedConf = -1.0F;
    uint4 uBestMatchedRow = iStartRow;
    uint4 uBestMatchedCol = iStartCol;
    for (uint4 r=iStartRow; r<=iStopRow; r++)
    {
      for (uint4 c=iStartCol; c<=iStopCol; c++)
      {
        MatchingFeature potentialMatchFeature(m_supine.m_ivFeatures[r],m_prone.m_ivFeatures[c],0.0F,0.0F);
        if(m_mConfidence(r,c) > fBestMatchedConf &&
           m_mMatchQuality(r,c) > m_fRegistrationMatchMean + m_fRegistrationMatchStdDev &&
           Stretch(startFeature, stopFeature, potentialMatchFeature) < fStretchThreshold &&
           FeaturesNotTooClose(startFeature, potentialMatchFeature) && FeaturesNotTooClose(stopFeature, potentialMatchFeature))
        {
          fBestMatchedConf = m_mConfidence(r,c);
          uBestMatchedRow = r;
          uBestMatchedCol = c;
        }
      } // for each prone feature
    } // for each supine feature

    if(fBestMatchedConf <= 0.0F)
    {
      float4 fBestMatchedQuality = m_mMatchQuality(iStartRow,iStartCol) /
                                   Stretch(startFeature, stopFeature, MatchingFeature(m_supine.m_ivFeatures[iStartRow],m_prone.m_ivFeatures[iStartCol],0.0F,0.0F));
      uBestMatchedRow = iStartRow;
      uBestMatchedCol = iStartCol;
      for (uint4 r=iStartRow; r<=iStopRow; r++)
      {
        for (uint4 c=iStartCol; c<=iStopCol; c++)
        {
          float4 fMatchQuality = m_mMatchQuality(r,c) / Stretch(startFeature, stopFeature, MatchingFeature(m_supine.m_ivFeatures[r],m_prone.m_ivFeatures[c],0.0F,0.0F));
          if(fMatchQuality > fBestMatchedQuality)
          {
            fBestMatchedQuality = fMatchQuality;
            uBestMatchedRow = r;
            uBestMatchedCol = c;
          }
        } // for each prone feature
      } // for each supine feature
#ifndef FINAL_RELEASE
      
      for(uint4 i=0;i<uLevel;i++) outStream << "|  ";
      outStream << "BestMatchedFeature (" << uBestMatchedRow <<":"<< uBestMatchedCol << ") at " << fBestMatchedQuality << " quality" << std::endl;
#endif
    }
    else
    {
#ifndef FINAL_RELEASE
      for(uint4 i=0;i<uLevel;i++) outStream << "|  ";
      outStream << "BestMatchedFeature (" << uBestMatchedRow <<":"<< uBestMatchedCol << ") at " << fBestMatchedConf << " confidence" << std::endl;
#endif
    }
    
    MatchingFeature bestMatchedFeature(m_supine.m_ivFeatures[uBestMatchedRow],m_prone.m_ivFeatures[uBestMatchedCol],m_mConfidence(uBestMatchedRow,uBestMatchedCol),m_mMatchQuality(uBestMatchedRow,uBestMatchedCol));
    
    featureList = FindSeeds(startFeature, bestMatchedFeature, iStartRow,uBestMatchedRow-1,iStartCol,uBestMatchedCol-1,uLevel+1); 
#ifndef FINAL_RELEASE
    for(uint4 i=0;i<uLevel;i++) outStream << "|  ";
    outStream << "Adding bestMatchFeature (" << bestMatchedFeature.m_uSupineFeatureIndex <<":"<< bestMatchedFeature.m_uProneFeatureIndex << ") " <<
                   "from (" << uBestMatchedRow <<":"<< uBestMatchedCol << ") at " << 
                   m_mMatchQuality(uBestMatchedRow,uBestMatchedCol) << " quality and " << 
                   bestMatchedFeature.m_fConfidence << " confidence, producing " << 
                   Stretch(startFeature, stopFeature, bestMatchedFeature) << " stretch"  << std::endl;
#endif
    featureList.push_back(bestMatchedFeature);
    std::vector<MatchingFeature> rightSideFeatureList = FindSeeds(bestMatchedFeature, stopFeature, uBestMatchedRow+1,iStopRow,uBestMatchedCol+1,iStopCol,uLevel+1); 
    featureList.insert(featureList.end(),rightSideFeatureList.begin(),rightSideFeatureList.end());
  } // endif configuration of features within the curretn range
  
  return featureList;

} // FindSeeds()


/**
 * Register the colon
 */
void ReaderGlobal::Register()
{
#ifndef FINAL_RELEASE
  std::ofstream outStream("C:/V-System/Log/Registration.txt", std::ios::app); outStream.precision(4); outStream.setf(std::ios::showpoint | std::ios::fixed | std::ios::showpos);  
  outStream << std::endl << std::endl;
#endif

  // The user features may have changed since the last time we called registration
  std::vector<MatchingFeature> vUserFeatures;

  // Rectum and Cecum are always "user" features
  vUserFeatures.resize(Max(m_supine.m_session.m_vUserFeatures.size(),m_prone.m_session.m_vUserFeatures.size()) + 2);
  vUserFeatures[0] = MatchingFeature(0,0,1.0F,1.0F);
	uint4 uFeature(0);
  for ( ; uFeature < Min(m_supine.m_session.m_vUserFeatures.size(),m_prone.m_session.m_vUserFeatures.size()); uFeature++)
  {
    vUserFeatures[uFeature+1] = MatchingFeature(m_supine.m_session.m_vUserFeatures[uFeature], m_prone.m_session.m_vUserFeatures[uFeature], 1.0F, 1.0F);
  } // endfor all user points
  vUserFeatures[++uFeature] = MatchingFeature(m_supine.m_joinedSkeleton.GetNumOfElements()-1,m_prone.m_joinedSkeleton.GetNumOfElements()-1,1.0F,1.0F);

#ifndef FINAL_RELEASE
  //outStream << "User Features" << std::endl;
  //for(i=0;i<vUserFeatures.size();i++)
  //{
  //  outStream << "(" << vUserFeatures[i].m_uSupineFeatureIndex <<":"<< vUserFeatures[i].m_uProneFeatureIndex << ") " << 
  //    "at " << vUserFeatures[i].m_fConfidence << " confidence, " << vUserFeatures[i].m_fQuality << " quality " <<
  //    "and " << vUserFeatures[i].m_fWindowedQuality << " windowed quality" << std::endl;
  //}
#endif

  // Loop through adding user features and calling recursive registraion on segments between user features
  // Note: rectum and cecum points are considered user features.
  m_vRegMatchingFeatures.clear();
  int4 iSupineStartFeature = 0;
  int4 iProneStartFeature = 0;
  for (uint4 i=0; i<vUserFeatures.size()-1;i++)
  {
    int4 iSupineStopFeature = iSupineStartFeature;
    int4 iProneStopFeature  = iProneStartFeature;
    while (iSupineStopFeature < m_supine.m_ivFeatures.size() && m_supine.m_ivFeatures[iSupineStopFeature] < vUserFeatures[i+1].m_uSupineFeatureIndex) iSupineStopFeature++;
    while (iProneStopFeature  < m_prone.m_ivFeatures.size()  && m_prone.m_ivFeatures[iProneStopFeature ]  < vUserFeatures[i+1].m_uProneFeatureIndex ) iProneStopFeature++;
    m_vRegMatchingFeatures.push_back(vUserFeatures[i]);
    std::vector<MatchingFeature> vFeatures = FindSeeds(vUserFeatures[i], vUserFeatures[i+1],
                                                         iSupineStartFeature,iSupineStopFeature-1,
                                                         iProneStartFeature ,iProneStopFeature-1,
                                                         0);
    m_vRegMatchingFeatures.insert(m_vRegMatchingFeatures.end(), vFeatures.begin(), vFeatures.end());
    iSupineStartFeature = iSupineStopFeature;
    iProneStartFeature  =  iProneStopFeature;
  } //endfor all user features
  m_vRegMatchingFeatures.push_back(vUserFeatures[vUserFeatures.size()-1]);
  
  // Update these fields whenever registration changes
  ComputeRegistrationTranslations();
  ComputeCenterlineConfidence();

#ifndef FINAL_RELEASE
  m_supine.m_joinedSkeleton.PrintJoinedData("Final Supine");
  m_prone.m_joinedSkeleton.PrintJoinedData("Final Prone");
  outStream << "Matched Features" << std::endl;
  for(uint4 i=0;i<m_vRegMatchingFeatures.size();i++)
  {
    outStream << "(" << m_vRegMatchingFeatures[i].m_uSupineFeatureIndex <<":"<< m_vRegMatchingFeatures[i].m_uProneFeatureIndex << ") " << 
                 "at " << m_vRegMatchingFeatures[i].m_fConfidence << " confidence, " << m_vRegMatchingFeatures[i].m_fQuality << " quality " <<
                 "and " << m_vRegMatchingFeatures[i].m_fWindowedQuality << " windowed quality" << std::endl;
  } // END ... for all matching features
#endif
} // Register()



/**
 * Computes the indexes into the other centerline for the current registration
 */
void ReaderGlobal::ComputeRegistrationTranslations()
{
  
  // for all match features
  for (uint4 uCurrFeature=0; uCurrFeature<m_vRegMatchingFeatures.size()-1; uCurrFeature++ )
  {
    const uint4 uSupineStartFeatureIndex = m_vRegMatchingFeatures[uCurrFeature  ].m_uSupineFeatureIndex;
    const uint4 uSupineStopFeatureIndex  = m_vRegMatchingFeatures[uCurrFeature+1].m_uSupineFeatureIndex;     
    const uint4 uProneStartFeatureIndex  = m_vRegMatchingFeatures[uCurrFeature  ].m_uProneFeatureIndex;
    const uint4 uProneStopFeatureIndex   = m_vRegMatchingFeatures[uCurrFeature+1].m_uProneFeatureIndex;    
    
    // for the current pair of two subsequent features set the supine translation values
    float4 fMorphSupineToProneScale = (float4)(uProneStopFeatureIndex - uProneStartFeatureIndex)/(float4)(uSupineStopFeatureIndex - uSupineStartFeatureIndex);
    for (uint4 uCurrIndex=uSupineStartFeatureIndex; uCurrIndex<uSupineStopFeatureIndex; uCurrIndex++ )
    {
      m_supine.m_joinedSkeleton.SetRegistrationOtherIndex(uCurrIndex, uProneStartFeatureIndex + (float4)(uCurrIndex - uSupineStartFeatureIndex)*fMorphSupineToProneScale);
    } // endfor all supine points between the two current features
    
    // for the current pair of two subsequent features set the prone translation values
    float4 fMorphProneToProneScale = 1/fMorphSupineToProneScale;
    for (uint4 uCurrIndex=uProneStartFeatureIndex; uCurrIndex<uProneStopFeatureIndex; uCurrIndex++ )
    {
      m_prone.m_joinedSkeleton.SetRegistrationOtherIndex(uCurrIndex,uSupineStartFeatureIndex + (float4)(uCurrIndex - uProneStartFeatureIndex)*fMorphProneToProneScale);
    } // endfor all prone points between the two current features
  } // endfor all match features

  // The last index along the centerline also needs to be set
  m_supine.m_joinedSkeleton.SetRegistrationOtherIndex(m_supine.m_joinedSkeleton.GetNumOfElements()-1, m_prone .m_joinedSkeleton.GetNumOfElements()-1);
  m_prone .m_joinedSkeleton.SetRegistrationOtherIndex( m_prone.m_joinedSkeleton.GetNumOfElements()-1, m_supine.m_joinedSkeleton.GetNumOfElements()-1);

} // ComputeRegistrationTranslations()


/**
 * Computes the centerline confidence accordingly with the seeds confidence
 */
void ReaderGlobal::ComputeCenterlineConfidence()
{
  // compute the windowed quality
  for(uint4 i=0;i<m_vRegMatchingFeatures.size();i++)
  {
    uint4 uCount = 0;
    float4 fSum = 0.0F;
    // use a 5x1 window of features along the centerline
    for(int4 j=-2;j<3;j++)
    {
      if(i+j>0 && i+j<m_vRegMatchingFeatures.size()-1) // don't allow the pseudo 1's at the endpoints
      {
        // weight the center higher than the ends
        float4 fWeight = 3-fabs(static_cast<float4>(j));
        uCount+=fWeight;
        fSum += fWeight*m_vRegMatchingFeatures[i+j].m_fQuality;
      }
    }
    float4 fAve = fSum/uCount;
    float4 conf = m_vRegMatchingFeatures[i].m_fConfidence;
    if(conf < 0.0F) conf = 0.0F;
    if(conf > 1.0F) conf = 1.0F;
    // if very confident use my own quality otherwise use the windowed one
    m_vRegMatchingFeatures[i].m_fWindowedQuality = conf*m_vRegMatchingFeatures[i].m_fQuality + (1.0F-conf)*fAve;
  } //endfor all matching features
  
  // Set the confidence along the centerline
  uint4 uSupineSkelIndex = 0;
  uint4 uProneSkelIndex = 0;
  
  // for all features
  for(uint4 i=0;i<m_vRegMatchingFeatures.size()-1;i++)
  {
    // for the current pair of two subsequent features set the supine centerline values
    uint4 uStartIndex  = m_vRegMatchingFeatures[i  ].m_uSupineFeatureIndex;
    uint4 uStopIndex   = m_vRegMatchingFeatures[i+1].m_uSupineFeatureIndex;
    float4 fStartValue = m_vRegMatchingFeatures[i  ].m_fWindowedQuality;
    float4 fStopValue  = m_vRegMatchingFeatures[i+1].m_fWindowedQuality;
    float4 fSlope = (fStopValue-fStartValue)/(uStopIndex-uStartIndex);
    
    for (uint4 j=uStartIndex; j<uStopIndex; j++)
    {
      float4 v = fSlope*(j-uStartIndex) + fStartValue;
      v = (v < 0.0F) ? 0.0F : ((v > 1.0F) ? 1.0F : v);
      m_supine.m_joinedSkeleton.SetRegistrationConfidence(uSupineSkelIndex++,v);
    } //enfor all skeleton nodes between the two current features

    // for the current pair of two subsequent features set the prone centerline values
    uStartIndex = m_vRegMatchingFeatures[i  ].m_uProneFeatureIndex;
    uStopIndex  = m_vRegMatchingFeatures[i+1].m_uProneFeatureIndex;
    fStartValue = m_vRegMatchingFeatures[i  ].m_fWindowedQuality;
    fStopValue  = m_vRegMatchingFeatures[i+1].m_fWindowedQuality;
    fSlope = (fStopValue-fStartValue)/(uStopIndex-uStartIndex);
    
    // for all skeleton nodes between the two current features
    for (uint4 j=uStartIndex; j<uStopIndex; j++)
    {
      float4 v = fSlope*(j-uStartIndex) + fStartValue;
      v = (v < 0.0F) ? 0.0F : ((v > 1.0F) ? 1.0F : v);
      m_prone.m_joinedSkeleton.SetRegistrationConfidence(uProneSkelIndex++,v);
    } // for all skeleton nodes between the two current features
  } // for all features

} // ComputeCenterlineConfidence()


/**
 * Maps the registration quality to a weight.
 * Note: the function should be re-named to represent what it computes
 *
 * @param fCorrelation        level of correlation to map.
 * @return                    weight
 */
float4 ReaderGlobal::ColorFunction(float4 fCorrelation)
{
  return 1.0F / (1.0F + exp(15.0F * (0.75F - fCorrelation)));
} // ColorFunction(float4 fCorrelation)


/**
 * Finds the closest CSI
 *
 * @param dset      Dataset
 * @param point     Point
 */
uint2 FindClosestCSI(const Dataset& dset, const Point<float4>& point)
{
  const SkeletonVCExt& skel = dset.m_joinedSkeleton;
  uint2 uCsi = skel.ConvertToJoinedIndex(dset.m_segmentList.GetCurrentSegment(), dset.m_fields.GetClosestSkeletonIndex(point));
  uint2 uRealCsi = uCsi;

  const uint4 uMaxSrcCSI(Min(skel.GetNumOfElements()-1, uint4(uCsi+10)));
  float4 fDist = point.GetEuclideanDistance(skel.GetWorldPos(uCsi));

  for (uint4 i(uCsi+1); i <= uMaxSrcCSI; i++)
  {
    float fCheckDist(0.0F);
    if ( (fCheckDist = point.GetEuclideanDistance(skel.GetWorldPos(i))) < fDist )
    {
      uRealCsi = i;
      fDist = fCheckDist;
    }
    else if (fCheckDist > fDist)
    {
      break;
    }
  } // END ... 

  for (uint4 i(uCsi-1); i >= Max(0,uCsi-10); i--)
  {
    float fCheckDist(0.0F);
    if ( (fCheckDist = point.GetEuclideanDistance(skel.GetWorldPos(i))) < fDist )
    {
      uRealCsi = i;
      fDist = fCheckDist;
    }
    else if (fCheckDist > fDist)
    {
      break;
    }
  } // END ... 

  return uRealCsi;
} // FindClosestCSI(const Dataset& dset, const Point<float4>& point)


/**
 * Class used only within this file to estimate as accurately as possible
 * the (fractional) current skeleton index along the joined skeleton.
 * You give it a series of points and distance of the skeleton point
 * from the desired point and it will estimate the position.
 */
class CsiEstimator
{
public:

  /**
   * Reset all the data
   */
  void Reset()
  {
    uSkeletonIndices.clear();
    fDistancesFromSkeletonPoint.clear();
  } // Reset()

  /**
   * Add a point to the estimator
   *
   * @param uSkeletonIndex              the current skeleton index
   * @param fDistanceFromSkeletonPoint  the distance of the point from the skeleton point
   */
  void AddPoint( const uint2 uSkeletonIndex, const float4 fDistanceFromSkeletonPoint )
  {
    uSkeletonIndices.push_back( uSkeletonIndex );
    fDistancesFromSkeletonPoint.push_back( fDistanceFromSkeletonPoint );
  } // AddPoint()

  /**
   * Calculate the fractional skeleton index
   *
   * @return the fractional skeleton index
   */
  float4 EstimateIndex()
  {
    if ( uSkeletonIndices.size() == 0 || fDistancesFromSkeletonPoint.size() == 0 )
      return 0.0F;

    // get the average distance
    float4 fSumDistances( 0 );
    float4 fMinimumDistance( std::numeric_limits<float4>::max() );
    for ( int i=0; i<fDistancesFromSkeletonPoint.size(); i++ )
    {
      fSumDistances += fDistancesFromSkeletonPoint[ i ];
      if ( fDistancesFromSkeletonPoint[ i ] < fMinimumDistance )
        fMinimumDistance = fDistancesFromSkeletonPoint[ i ];
    }
    const float4 fAverageDistance = fSumDistances / fDistancesFromSkeletonPoint.size();

    // estimate using the weight = 1 / ( distance - smallest distance + average distance )
    float4 fSumWeights( 0 );
    float4 fSumWeightedIndices( 0 );
    for ( int i=0; i<uSkeletonIndices.size(); i++ )
    {
      float4 fWeight = 1.0F / ( fDistancesFromSkeletonPoint[ i ] - fMinimumDistance + fAverageDistance );
      fSumWeights += fWeight;
      fSumWeightedIndices += fWeight * uSkeletonIndices[ i ];
    }

    // estimate!
    return fSumWeightedIndices / fSumWeights;

  } // EstimateIndex()

// data members
private:

  /// the index along the joined skeleton
  std::vector< uint2 > uSkeletonIndices;

  /// the distance of the point from the given skeleton point on the joined skeleton
  std::vector< float4 > fDistancesFromSkeletonPoint;

}; // class CsiEstimator


/**
 * search for the nearest skeleton point along a joined skeleton
 */
uint2 SelectNearestSkeletonPoint( const SkeletonVCExt & joinedSkeleton, const Point<float4> & worldPosition )
{
  
  // initialize
  uint2 uNearestJoinedIndex( 0 );
  float4 fMinimumDistance( std::numeric_limits<float4>::max() );
  
  // loop over the entire skeleton
  for ( int j( 0 ); j < joinedSkeleton.GetNumOfElements(); j ++ )
  {
    const float4 fDistance = joinedSkeleton.GetWorldPos( j ).GetEuclideanDistance( worldPosition );
    if ( fDistance < fMinimumDistance )
    {
      fMinimumDistance = fDistance;
      uNearestJoinedIndex = j;
    }
  } // loop over the entire skeleton

  return uNearestJoinedIndex;

} // SelectNearestSkeletonPoint()


float4 EstimateJoinedIndex( const Dataset & dataset, const Point<float4> & worldPosition )
{

  // determine the starting point
  const SkeletonVCExt & joinedSkeleton = dataset.m_joinedSkeleton;
  uint2 uCurrentSkeletonIndex = dataset.m_submarine.GetClosestSkeletonIndex();
  //uint2 uJoinedIndex = joinedSkeleton.ConvertToJoinedIndex( dataset.m_segmentList.GetCurrentSegment(), dataset.m_fields.GetClosestSkeletonIndex( dataset.m_submarine.GetPositionVolume() ) );
  uint2 uJoinedIndex = SelectNearestSkeletonPoint( joinedSkeleton, worldPosition );

  // add the nearest point to the estimator
  float4 fDistance = worldPosition.GetEuclideanDistance( joinedSkeleton.GetWorldPos( uJoinedIndex ) );
  static CsiEstimator estimator;
  estimator.Reset();
  estimator.AddPoint( uJoinedIndex, fDistance );

  // walk a few skeleton points in both directions
  const int4 iSteps = 3;
  for ( int4 i( 0 ); i < iSteps; i++ )
  {

    // walk forward
    int4 iTestIndex = uJoinedIndex + i + 1;
    if ( iTestIndex < joinedSkeleton.GetNumOfElements() )
    {
      const float4 fTestDistance = worldPosition.GetEuclideanDistance( joinedSkeleton.GetWorldPos( iTestIndex ) );
      estimator.AddPoint( iTestIndex, fTestDistance );
    }

    // walk backward
    iTestIndex = uJoinedIndex - i - 1;
    if ( iTestIndex >= 0 )
    {
      const float4 fTestDistance = worldPosition.GetEuclideanDistance( joinedSkeleton.GetWorldPos( iTestIndex ) );
      estimator.AddPoint( iTestIndex, fTestDistance );
    }

  } // walk a few skeleton points in both directions

  const float4 fEstimate = estimator.EstimateIndex();
  return fEstimate;

} // EstimateJoinedIndex( dataset, worldPosition )


/**
 * Updates the world location of the destination dataset given that of the (registered) source dataset
 *
 * @param srcDset   The dataset referenced for the change in world position
 * @param dstDset   The dataset, registered to srcDset, whose world coordinates will be changed.
 */
void ReaderGlobal::UpdateWorldPosition( const Dataset & srcDataset, Dataset & destDataset ) const
{
  if ( ! ( IsDualDataLoaded() && m_bSPLocked && srcDataset.IsJoined() && destDataset.IsJoined() ) )
    return;

  const SkeletonVCExt & srcSkeleton = srcDataset.m_joinedSkeleton;
  const SkeletonVCExt & destSkeleton = destDataset.m_joinedSkeleton;

  const Point<float4> srcWorldPos( srcDataset.m_submarine.GetPositionWorld() );
  const float4 fSrcCSI( EstimateJoinedIndex( srcDataset, srcWorldPos ) );

  // interpolate the centerline
  uint4 uSrcJoinedCenterlineIndex = static_cast< uint4 >( fSrcCSI );
  uint4 uDestJoinedCenterlineIndex1 = srcSkeleton.GetRegistrationOtherIndex( uSrcJoinedCenterlineIndex );
  uint4 uDestJoinedCenterlineIndex2 = srcSkeleton.GetRegistrationOtherIndex( uSrcJoinedCenterlineIndex + 1 );
  if ( uDestJoinedCenterlineIndex1 >= destSkeleton.GetNumOfElements() )
  {
    LogWrn( "Destination centerline index: " + ToAscii( uDestJoinedCenterlineIndex1 ) + " greater than max: " +
            ToAscii( destSkeleton.GetNumOfElements() ), "ReaderGlobal", "UpdateWorldPosition");
    uDestJoinedCenterlineIndex1 = destSkeleton.GetNumOfElements() - 1;
  }
  if ( uDestJoinedCenterlineIndex2 >= destSkeleton.GetNumOfElements() )
  {
    LogWrn( "Destination centerline index: " + ToAscii( uDestJoinedCenterlineIndex2 ) + " greater than max: " +
            ToAscii( destSkeleton.GetNumOfElements() ), "ReaderGlobal", "UpdateWorldPosition");
    uDestJoinedCenterlineIndex2 = destSkeleton.GetNumOfElements() - 1;
  }
  Point<float4> vDestPosWorldCoord1 = destSkeleton.GetWorldPos( uDestJoinedCenterlineIndex1 );
  Point<float4> vDestPosWorldCoord2 = destSkeleton.GetWorldPos( uDestJoinedCenterlineIndex2 );
  const float4 fWeight = fSrcCSI - static_cast< int4 >( fSrcCSI );
  Point<float4> vDestPosWorldCoordInterpolated = Point3Df(
    Vector3Df( vDestPosWorldCoord1 ) * ( 1 - fWeight ) +
    Vector3Df( vDestPosWorldCoord2 ) * fWeight );

  //{
  //  // find out if CSI is closer to uSrcJoinedCenterlineIndex-1 or +1
  //  uint4 uMaxSrcCSI(srcSkel.GetNumOfElements()-1);
  //  float8 fDistPrev = (uSrcCSI <= 0) ? MAXFLOAT  : srcWorldPos.EuclideanDistance(srcSkel.GetWorldPos(uSrcCSI-1));
  //  float8 fDistNext = (uSrcCSI >= uMaxSrcCSI) ? MAXFLOAT : srcWorldPos.EuclideanDistance(srcSkel.GetWorldPos(uSrcCSI+1));
  //  uint4 uCloserIndex = fDistPrev < fDistNext ? uSrcCSI-1 : uSrcCSI+1;

  //  Point<float4> ptSrcCSI = srcSkel.GetWorldPos(uSrcCSI);
  //  Point<float4> ptNextClosestCSI = srcSkel.GetWorldPos(uCloserIndex);

  //  // project srcWorldPos down to line between uSrcCSI and uCloserIndex
  //  Point<float4> ptSrcPosOnCSILine =
  //    PrimitiveUtilities::ProjectPointOntoLine(Line(ptSrcCSI, ptNextClosestCSI), srcWorldPos);

  //  // find the percentage distance of worldlocation between the CSI and the next-closer index
  //  float8 fDistLinePtAndCSI = ptSrcPosOnCSILine.EuclideanDistance(ptSrcCSI);
  //  float8 fDistCSIs = ptSrcCSI.EuclideanDistance(ptNextClosestCSI);
  //  float8 fPerc = Bound(float8(0.0F), fDistLinePtAndCSI / fDistCSIs, float8(1.0F));

  //  // interpolate vDestPosWorld by percentage between uDestJoinedCenterlineIndex and uDestJoinedCenterlineIndex-/+1
  //  Point<float4> B(dstSkel.GetWorldPos(fDistPrev < fDistNext ? uDestJoinedCenterlineIndex-1 : uDestJoinedCenterlineIndex+1));
  //  Vector<float4> destAB = Vector<float4>(vDestPosWorldCoord) + Vector<float4>(B);
  //  destAB.Normalize();
  //  vDestPosWorldCoord = Point<float4>(vDestPosWorldCoord + (destAB * fPerc));
  //}

  Normal<float4> vSrcOrientation = srcDataset.m_submarine.GetOrientation();
  // negate X and Y coordinates to flip supine to prone
  Normal<float4> vDestOrientation( vSrcOrientation.X(), vSrcOrientation.Y(), vSrcOrientation.Z() );
  
  // if Autoflying orient along the centerline, not the view direction
  if ( srcDataset.m_submarine.GetPotential() ) 
  {
    vDestOrientation = destSkeleton.GetWorldTan( uDestJoinedCenterlineIndex1 );
    if ( srcDataset.m_submarine.GetFlightDirection() == Submarine::CECUM_TO_RECTUM ) vDestOrientation = -vDestOrientation;
  }
  Normal<float4> srcUpDir = srcDataset.m_submarine.GetUpDirection();

  destDataset.SelectSegment( vDestPosWorldCoord1 );
  destDataset.m_submarine.SetPositionWorld( vDestPosWorldCoord1 );
  destDataset.m_submarine.SetOrientation( vDestOrientation );
  // negate X and Y coordinates to flip supine to prone
  destDataset.m_submarine.SetUpDirection( Normal<float4>( srcUpDir.X(), srcUpDir.Y(), srcUpDir.Z() ) );
} // UpdateWorldPosition


// $Log: ReaderGlobalRegistration.C,v $
// Revision 1.5.2.1  2009/01/10 07:25:47  jmeade
// comments
//
// Revision 1.5  2006/09/13 15:22:42  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.4  2006/02/01 14:07:07  mkontak
// Coding standards
//
// Revision 1.3  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.2  2005/11/19 12:04:57  vxconfig
// coding standards
//
// Revision 1.1.4.2  2006/01/11 21:23:39  frank
// Issue #4507: Improved the quality of synchronized 3D flythroughs
//
// Revision 1.1.4.1  2006/01/10 15:04:57  jmeade
// update world position.
//
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.9.2.10.4.2.4.1  2005/07/13 17:28:30  geconomos
// Added a check in UpdateWorldPosition to ensure that the datsets have been registered
//
// Revision 3.9.2.10.4.2  2005/04/22 16:48:40  frank
// cleaned up field class interface
//
// Revision 3.9.2.10.4.1  2005/04/22 03:21:56  jmeade
// extra checks on vector bounds in registration computations.
//
// Revision 3.9.2.10  2004/03/22 21:46:20  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.9.2.9  2003/05/23 19:34:11  kevin
// Code Walkthrough cosmetic changes
//
// Revision 3.9.2.8  2003/05/22 19:29:35  kevin
// Tracker issue 3114: Fixed the very last skeleton index in the cross-index
// registration field.
//
// Revision 3.9.2.7  2003/05/01 19:06:09  frank
// code review
//
// Revision 3.9.2.6  2003/05/01 16:58:26  kevin
// Code Walkthrough cosmetic changes
//
// Revision 3.9.2.5  2003/04/28 17:50:29  jmeade
// Debug: close debug reg'n file after writing (other print methods access it).
//
// Revision 3.9.2.4  2003/04/14 22:27:43  kevin
// Handle short segments with no registration features
//
// Revision 3.9.2.3  2003/03/26 23:18:53  jmeade
// Issue 2849:  Combined duplicate register-update methods, added check to ensure jump within destination skeleton.
//
// Revision 3.9.2.2  2003/03/12 23:07:10  jmeade
// Removed debug output in final release builds.
//
// Revision 3.9.2.1  2003/02/12 23:22:47  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 3.11  2003/02/12 17:40:41  jmeade
// Code standards.
//
// Revision 3.10  2003/02/10 18:55:42  kevin
// Registration Debug Info
//
// Revision 3.9  2003/01/18 16:27:14  kevin
// Made the "link" button on the Supine/Prone screen effect the linking of the registration too
//
// Revision 3.8  2003/01/07 22:47:16  kevin
// If not auto flying use the registered view direction so that the user can look at the wall
// and the view in the registered data changes view direction also.
//
// Revision 3.7  2002/11/24 17:50:21  kevin
// Upadtes now consider the flight direction
//
// Revision 3.6  2002/11/14 19:34:37  kevin
// Don't set detectecd features too close to user features
//
// Revision 3.5  2002/11/11 14:32:07  kevin
// First attempt at making the thermometer match the centerline color
//
// Revision 3.4  2002/11/08 01:33:01  kevin
// removed debug output for now
//
// Revision 3.3  2002/10/31 20:02:32  kevin
// Changed user feature points to make them more robust and sorted
// so that they cannot become invalid (e.g. crossing user features)
//
// Revision 3.2  2002/10/23 00:24:09  kevin
// Code cleanup after last round of S/P registration algorithm development
//
// Revision 3.1  2002/10/21 18:51:53  kevin
// Kevin's refactored S/P Registration with new heirarchical algorithm
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/ReaderGlobalRegistration.C,v 1.5.2.1 2009/01/10 07:25:47 jmeade Exp $
// $Id: ReaderGlobalRegistration.C,v 1.5.2.1 2009/01/10 07:25:47 jmeade Exp $>>>>>>> 1.13
