// $Id: Histogram.C,v 1.3 2006/01/18 20:37:46 geconomos Exp $
//
// Copyright 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created:
// Author: Jay
//
// Complete history at bottom of file.



// pragma declarations
#pragma warning (disable : 4786)



// include declarations
#include "stdafx.h"
#include "Histogram.h"
#include "Volume.h"



// namespace declarations
using namespace std;



bool Histogram<int4>::Test(ostream & os)
{
/*
  os << "simple example on how to use a Histogram" << endl;

  // make a volume to work with
  BlockVolI vol(Triple<uint4>(3,3,3), 8*sizeof(int4));
  vol.SetVoxel(Point<uint2>(1,1,0), 1);
  vol.SetVoxel(Point<uint2>(1,1,1), 2);
  vol.SetVoxel(Point<uint2>(1,1,2), 2);
  vol.SetVoxel(Point<uint2>(1,2,2), 2);
  vol.PrintAllRaw();

  // make an iterator attached to voxel (0,0,0)
  BlockVolIterI iter(vol.GetVolIterator(Point<uint2>(0,0,0)));

  // make the volume histogram
  Histogram<int4> hist(iter);
  os << hist << endl;

  //initialize values
  bool success = true, stepsuccess=false;
 */
 
 bool success = true;
 

  /*

  NOTE: The following is what I commented out:
  
  typedef unsigned long ulong;
  typedef unsigned char uchar;
  float epsilon = float(.001);
  static const char * const m_sBoolStr[] = {"Oh No ! :-( ",":-) "};
  int divisor = 23;
  int IntArray[100];
  std::vector<int> Vec(100);
  std::vector<int> iVec(100);
  std::vector<unsigned char> uVec(100);
  std::vector<float>FVec(100);
  ulong i;
  for ( i = 0; i<Vec.size (); i++ ) {
    Vec[i] = (i%divisor);
    uVec[i] = (i%divisor);
    iVec[iVec.size()-i] = i;
    IntArray[i] = (i%divisor);
    FVec[i] = (i%divisor) + 123.453;
  } // endfor (i<Vec.size())
  os << "Beginning test of class Histogram" << std::endl;


  { //INT portion of test function
    int k = 0;

    os << std::endl << "epsilon = " << epsilon;
    os << std::endl << "int portion of histogram class test"
    << std::endl;
    os << "Array of integers passed onto h1 histogram (IntArray): " << std::endl;
    for ( unsigned long j=0; j<99; j++ ) {
      if ( j%12 == 0 ) {
        os << std::endl;
      } else {
        os << IntArray[j] << " " ;
      }// endif (j%12 == 0)
    } //endfor (; j<99;)

    // test constructor with 2 arguments (for int/char/long)
    Histogram<int> h1(&IntArray[0],&IntArray[0]+100);
    os << std::endl;
    os << "histogram instantiated with array of size 100" << std::endl;
    os << "h1 Histogram= " << std::endl;
    os << h1 << std::endl << std::endl;
    std::vector<unsigned long> vTmp = h1.GetHistogramVector();
    os << "Test of constructor with 2 parameters: h1";
    stepsuccess = (vTmp[0] ==5 && vTmp[1] ==5 && vTmp[2] ==5 && vTmp[3] ==5 &&
                   vTmp[4] ==5 && vTmp[5] ==5 && vTmp[6] ==5 && vTmp[7] ==5 &&
                   vTmp[8] ==4 && vTmp[9] ==4 && vTmp[10]==4 && vTmp[11]==4 &&
                   vTmp[12]==4 && vTmp[13]==4 && vTmp[14]==4 && vTmp[15]==4 &&
                   vTmp[16]==4 && vTmp[17]==4 && vTmp[18]==4 && vTmp[19]==4 &&
                   vTmp[20]==4 && vTmp[21]==4 && vTmp[22]==4);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    //
    // test GetHistogramVector()
    // test done as part of test of constructor
    //
    os << "Test of GetHistogramVector: ";
    stepsuccess = 1;
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test copy constructor
    Histogram<int> h3(h1);
    os << "Test of copy constructor: Histogram<int> h3(h1)";
    vTmp = h3.GetHistogramVector();
    stepsuccess = (vTmp[0] ==5 && vTmp[1] ==5 && vTmp[2] ==5 && vTmp[3] ==5 &&
                   vTmp[4] ==5 && vTmp[5] ==5 && vTmp[6] ==5 && vTmp[7] ==5 &&
                   vTmp[8] ==4 && vTmp[9] ==4 && vTmp[10]==4 && vTmp[11]==4 &&
                   vTmp[12]==4 && vTmp[13]==4 && vTmp[14]==4 && vTmp[15]==4 &&
                   vTmp[16]==4 && vTmp[17]==4 && vTmp[18]==4 && vTmp[19]==4 &&
                   vTmp[20]==4 && vTmp[21]==4 && vTmp[22]==4);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test overloaded assignment operator
    Histogram<int> h4 = h1;
    os << "Test of assignment operator: Histogram<int> h4 = h1";
    vTmp = h4.GetHistogramVector();
    stepsuccess = (vTmp[0] ==5 && vTmp[1] ==5 && vTmp[2] ==5 && vTmp[3] ==5 &&
                   vTmp[4] ==5 && vTmp[5] ==5 && vTmp[6] ==5 && vTmp[7] ==5 &&
                   vTmp[8] ==4 && vTmp[9] ==4 && vTmp[10]==4 && vTmp[11]==4 &&
                   vTmp[12]==4 && vTmp[13]==4 && vTmp[14]==4 && vTmp[15]==4 &&
                   vTmp[16]==4 && vTmp[17]==4 && vTmp[18]==4 && vTmp[19]==4 &&
                   vTmp[20]==4 && vTmp[21]==4 && vTmp[22]==4);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test GetDataValueAt()
    os << "Test h1.GetDataValueAt(k) for k<=GetMax():" ;
    stepsuccess = 1;
    for ( k=1; k<=h1.GetMax(); k++ ) {
      i = ulong(k);
      if ( i == static_cast<unsigned long>(h1.GetDataValueAt(i)) )
        stepsuccess = stepsuccess && 1;
      else
        stepsuccess = 0;
    }
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test GetBin(value)
    Histogram<int> h5(&iVec[0],&iVec[0]+100);
    os << "Test of GetBin(value) function; h5.GetBin(IntArray[k]) for (k<100):";
    stepsuccess = 1;
    for ( k=0; k<100; k++ ) {
      if ( h5.GetBin(IntArray[k]) == static_cast<ulong>(k%divisor) ) {
        stepsuccess = stepsuccess && 1;
      } else {
        stepsuccess = stepsuccess && 0;
      }
    }
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test GetMax()
    os << "Test GetMax() function: (h5.GetMax() == 99)" ;
    ulong utemp1 = h5.GetMax();
    stepsuccess = (utemp1 == 99);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test GetMin()
    os << "Test GetMin() function: (h5.GetMin()==0)" ;
    utemp1 = h5.GetMin();
    stepsuccess = (utemp1 == 0);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test GetHistogramInterval()
    os << "Test GetHistogramInterval(): interval == 1";
    utemp1 = h5.GetHistogramInterval();
    stepsuccess = (utemp1 == 1);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test GetHistogramWidth()
    os << "Test GetHistogramWidth() function: width == 100";
    utemp1 = h5.GetHistogramWidth();
    stepsuccess = (utemp1 == 100);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

  } // end of INT test


  { //UCHAR portion of test function

    os << std::endl << std::endl;
    os << "uchar portion of Histogram class test. " << std::endl;
    os << "Array of integers passed onto h1 histogram: " << std::endl;

    // test constructor with 2 arguments (for int/char/long)
    Histogram<uchar> h1(&uVec[0],&uVec[0]+100);
    os << "h1 Histogram= " << std::endl;
    os << h1 << std::endl << std::endl;
    std::vector<unsigned long> vTmp = h1.GetHistogramVector();
    os << "Test of constructor with 2 parameters: h1";
    stepsuccess = (vTmp[0] ==5 && vTmp[1] ==5 && vTmp[2] ==5 && vTmp[3] ==5 &&
                   vTmp[4] ==5 && vTmp[5] ==5 && vTmp[6] ==5 && vTmp[7] ==5 &&
                   vTmp[8] ==4 && vTmp[9] ==4 && vTmp[10]==4 && vTmp[11]==4 &&
                   vTmp[12]==4 && vTmp[13]==4 && vTmp[14]==4 && vTmp[15]==4 &&
                   vTmp[16]==4 && vTmp[17]==4 && vTmp[18]==4 && vTmp[19]==4 &&
                   vTmp[20]==4 && vTmp[21]==4 && vTmp[22]==4);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    //
    // test GetHistogramVector()
    // test done as part of test of constructor
    //
    os << "Test of GetHistogramVector: ";
    stepsuccess = 1;
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test copy constructor
    Histogram<uchar> h3(h1);
    os << "Test of copy constructor: Histogram<uchar> h3(h1)";
    vTmp = h3.GetHistogramVector();
    stepsuccess = (vTmp[0] ==5 && vTmp[1] ==5 && vTmp[2] ==5 && vTmp[3] ==5 &&
                   vTmp[4] ==5 && vTmp[5] ==5 && vTmp[6] ==5 && vTmp[7] ==5 &&
                   vTmp[8] ==4 && vTmp[9] ==4 && vTmp[10]==4 && vTmp[11]==4 &&
                   vTmp[12]==4 && vTmp[13]==4 && vTmp[14]==4 && vTmp[15]==4 &&
                   vTmp[16]==4 && vTmp[17]==4 && vTmp[18]==4 && vTmp[19]==4 &&
                   vTmp[20]==4 && vTmp[21]==4 && vTmp[22]==4);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test overloaded assignment operator
    Histogram<uchar> h4 = h1;
    os << "Test of assignment operator: Histogram<uchar> h4 = h1";
    vTmp = h4.GetHistogramVector();
    stepsuccess = (vTmp[0] ==5 && vTmp[1] ==5 && vTmp[2] ==5 && vTmp[3] ==5 &&
                   vTmp[4] ==5 && vTmp[5] ==5 && vTmp[6] ==5 && vTmp[7] ==5 &&
                   vTmp[8] ==4 && vTmp[9] ==4 && vTmp[10]==4 && vTmp[11]==4 &&
                   vTmp[12]==4 && vTmp[13]==4 && vTmp[14]==4 && vTmp[15]==4 &&
                   vTmp[16]==4 && vTmp[17]==4 && vTmp[18]==4 && vTmp[19]==4 &&
                   vTmp[20]==4 && vTmp[21]==4 && vTmp[22]==4);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test GetDataValueAt()
    os << "Test h1.GetDataValueAt(k) for k<=h1.GetMax(): i == h1.GetDataValueAt(i)";
    stepsuccess = 1;
    for ( i=1; i<=h1.GetMax(); i++ ) {
      if ( i == h1.GetDataValueAt(i) ) {
        stepsuccess = stepsuccess && 1;
      } else {
        stepsuccess = 0;
      } //endif (k == h1.GetDataValueAt(k))
    } //endfor (k<=h1.GetMax())
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;


    // test GetBin(value)
    Histogram<uchar> h5(&iVec[0],&iVec[0]+100);
    os << "Test of GetBin(value) function; h5.GetBin(uVec[k]) for (k<100):";
    stepsuccess = 1;
    for ( i=0; i<100; i++ ) {
      if ( h5.GetBin(uVec[i]) == i%divisor ) {
        stepsuccess = stepsuccess && 1;
      } else {
        stepsuccess = stepsuccess && 0;
      }
    }
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test GetMax()
    os << "Test GetMax() function: h5.GetMax() == 99" ;
    ulong utemp1 = h5.GetMax();
    stepsuccess = (utemp1 == 99);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test GetMin()
    utemp1 = h5.GetMin();
    os << "Test GetMin() function: h5.GetMin() == 0" ;
    stepsuccess = (utemp1 == 0);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test GetHistogramInterval()
    os << "Test GetHistogramInterval() function: interval == 1";
    utemp1 = h5.GetHistogramInterval();
    stepsuccess = (utemp1 == 1);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test GetHistogramWidth()
    os << "Test GetHistogramWidth() function: width == 100";
    utemp1 = h5.GetHistogramWidth();
    stepsuccess = (utemp1 == 100);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;
  } // end of UCHAR test


  { //FLOAT portion of test function

    os << std::endl << std::endl;
    os << "float portion of Histogram class test. " << std::endl;

    // test constructor with 5 arguments (for floats/double)
    float low = 0,high = 0;
    Histogram<float> h1(&FVec[0],&FVec[0]+100,22,low, high);
    os << "h1 Histogram= " << std::endl;
    os << h1 << std::endl << std::endl;
    std::vector<unsigned long> vTmp = h1.GetHistogramVector();
    os << "Test of constructor with 5 parameters: h1";
    stepsuccess = (vTmp[0] ==5 && vTmp[1] ==5 && vTmp[2] ==5 && vTmp[3] ==5 &&
                   vTmp[4] ==5 && vTmp[5] ==5 && vTmp[6] ==5 && vTmp[7] ==5 &&
                   vTmp[8] ==4 && vTmp[9] ==4 && vTmp[10]==4 && vTmp[11]==4 &&
                   vTmp[12]==4 && vTmp[13]==4 && vTmp[14]==4 && vTmp[15]==4 &&
                   vTmp[16]==4 && vTmp[17]==4 && vTmp[18]==4 && vTmp[19]==4 &&
                   vTmp[20]==4 && vTmp[21]==8);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    //
    // test GetHistogramVector()
    // test done as part of test of constructor
    //
    os << "Test of GetHistogramVector: ";
    stepsuccess = 1;
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test copy constructor
    Histogram<float> h3(h1);
    os << "Test of copy constructor: Histogram<int> h3(h1)";
    vTmp = h3.GetHistogramVector();
    stepsuccess = (vTmp[0] ==5 && vTmp[1] ==5 && vTmp[2] ==5 && vTmp[3] ==5 &&
                   vTmp[4] ==5 && vTmp[5] ==5 && vTmp[6] ==5 && vTmp[7] ==5 &&
                   vTmp[8] ==4 && vTmp[9] ==4 && vTmp[10]==4 && vTmp[11]==4 &&
                   vTmp[12]==4 && vTmp[13]==4 && vTmp[14]==4 && vTmp[15]==4 &&
                   vTmp[16]==4 && vTmp[17]==4 && vTmp[18]==4 && vTmp[19]==4 &&
                   vTmp[20]==4 && vTmp[21]==8);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test overloaded assignment operator
    Histogram<float> h4 = h1;
    os << "Test of assignment operator: Histogram<int> h4 = h1";
    vTmp = h4.GetHistogramVector();
    stepsuccess = (vTmp[0] ==5 && vTmp[1] ==5 && vTmp[2] ==5 && vTmp[3] ==5 &&
                   vTmp[4] ==5 && vTmp[5] ==5 && vTmp[6] ==5 && vTmp[7] ==5 &&
                   vTmp[8] ==4 && vTmp[9] ==4 && vTmp[10]==4 && vTmp[11]==4 &&
                   vTmp[12]==4 && vTmp[13]==4 && vTmp[14]==4 && vTmp[15]==4 &&
                   vTmp[16]==4 && vTmp[17]==4 && vTmp[18]==4 && vTmp[19]==4 &&
                   vTmp[20]==4 && vTmp[21]==8 );
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test GetDataValueAt()
    os << "Test h1.GetDataValueAt(k): (FVec[k] - h1.GetDataValueAt(k))<epsilon) for (k<width) ";
    stepsuccess = 1;
    for ( i=0; i<h1.GetHistogramWidth(); i++ ) {
      if ( (FVec[i] - h1.GetDataValueAt(i))<epsilon ) {
        stepsuccess = stepsuccess && 1;
      } else {
        stepsuccess = 0;
      } //endif ((FVec[k] - h1.GetDataValueAt(k))<epsilon)
    } //endfor (k<h1.GetHistogramWidth())
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess];

    // test GetBin(value)
    Histogram<float> h5(&FVec[0],&FVec[0]+100,22, low, high);
    os << "Test of GetBin(value) function; h5.GetBin(FVec[k]) for (k<100):";
    stepsuccess = (h5.GetBin(FVec[0]) ==0  && h5.GetBin(FVec[1]) ==1  && h5.GetBin(FVec[2]) ==2  && h5.GetBin(FVec[3]) ==3  &&
                   h5.GetBin(FVec[4]) ==4  && h5.GetBin(FVec[5]) ==5  && h5.GetBin(FVec[6]) ==6  && h5.GetBin(FVec[7]) ==7  &&
                   h5.GetBin(FVec[8]) ==8  && h5.GetBin(FVec[9]) ==9  && h5.GetBin(FVec[10])==10 && h5.GetBin(FVec[11])==11 &&
                   h5.GetBin(FVec[12])==12 && h5.GetBin(FVec[13])==13 && h5.GetBin(FVec[14])==14 && h5.GetBin(FVec[15])==14 &&
                   h5.GetBin(FVec[16])==16 && h5.GetBin(FVec[17])==17 && h5.GetBin(FVec[18])==18 && h5.GetBin(FVec[19])==19 &&
                   h5.GetBin(FVec[20])==20 && h5.GetBin(FVec[21])==21 );
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;

    // test GetMax()
    os << "Test GetMax() function: max == 145" ;
    float ftemp = h5.GetMax();
    stepsuccess = (ftemp == 145);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess];

    // test GetMin()
    ftemp = h5.GetMin();
    os << "Test GetMin() function:  min == 123";
    stepsuccess = (ftemp == 123);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess];

    // test GetHistogramInterval()
    os << "Test GetHistogramInterval() function: interval == 1 ";
    ftemp = h5.GetHistogramInterval();
    stepsuccess = (ftemp == 1);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess];

    // test GetHistogramWidth()
    os << "Test GetHistogramWidth() function: width == 22";
    ftemp = h5.GetHistogramWidth();
    stepsuccess = (ftemp == 22);
    success = success && stepsuccess;
    os << std::endl << m_sBoolStr[stepsuccess] ;
  } //end of FLOAT test


  // end of tests, return whether successful or not.
  os << std::endl <<"End of test of class Histogram\n\n";
  if ( success ) {
    os << "Test of Histogram class is a success!" << std::endl;
  } else {
    os << "NOT a success" << std::endl;
  } //endif (success)
  */

  return(success);

} //end of Test function


// $Log: Histogram.C,v $
// Revision 1.3  2006/01/18 20:37:46  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.2  2004/03/04 21:00:55  geconomos
// Modifications for 7.1 compiler including: use of typename for dependant types and resolving ambiguities.
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2002/03/08 22:34:22  michael
// added Histogram to 20_image to avoid forward dependencies
//
// Revision 1.2  2002/03/08 17:15:18  michael
// further removed include dependencies
//
// Revision 1.1  2002/02/20 01:39:28  michael
// moved histogram from 03_utils to 20_image since PNGimage is needed
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:29:58   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:18   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:34   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:29  soeren
// renamed utils directory
//
// Revision 1.13  2001/04/09 16:25:34  dave
// removed some uses of uint, uchar, etc
//
// Revision 1.12  2001/04/08 22:38:44  dave
// added set_terminate() handler
//
// Revision 1.11  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.10  2001/03/30 18:47:22  dmitry
// Updated datatypes.
//
// Revision 1.9  2001/03/27 19:10:51  dmitry
// Updated more types as def. in Typedef.h
//
// Revision 1.8  2000/11/16 17:19:48  dave
// added native BlockVolume VolIterator
//
// Revision 1.7  2000/11/13 18:07:43  ingmar
// much improved rewrite of the Histogram class
//
// Revision 1.6  2000/10/26 02:49:33  ingmar
// moved /*  */ to //
//
// Revision 1.5  2000/09/11 04:12:01  antonio
// got rid of a few more warnings
//
// Revision 1.4  2000/09/11 01:10:06  dave
// added author
//
// Revision 1.3  2000/09/11 01:06:53  dave
// back to old
//
// Revision 1.2  2000/09/11 00:29:39  antonio
// removed some warnings
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
//
// 9     9/05/00 9:02p Ingmar
// towards compile in linux
//
// 8     9/03/00 9:43p Antonio
// fixed a few warnings from gcc.
//
// 7     8/21/00 3:22p Antonio
// added Histogram.C to LibUtils
//
// 6     8/21/00 2:29p Antonio
// put test function back in Histogram.C
//
// *****************  Version 5  *****************
// User: Jay          Date: 8/18/00    Time: 3:56p
// Updated in $/vx/src/Utils
// Histogram::Test now in Histogram.inl, Histogram.C removed from Project
//
// *****************  Version 4  *****************
// User: Antonio      Date: 8/11/00    Time: 10:10a
// Updated in $/vx/src/Utils
// updated header of file
//
// *****************  Version 3  *****************
// User: Antonio      Date: 8/02/00    Time: 10:01p
// Updated in $/vx/src/Utils
// added complete history to end of file
//
// *****************  Version 2  *****************
// User: Antonio         Date:  8/01/00  Time:  3:17p
// Checked in $/vx/src/Utils
// Comment:
//   added header, history and copyright
//
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:02a
// Created Histogram.C
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/Histogram.C,v 1.3 2006/01/18 20:37:46 geconomos Exp $
// $Id: Histogram.C,v 1.3 2006/01/18 20:37:46 geconomos Exp $