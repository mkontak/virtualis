// $Id: HUconverter.inl,v 1.1 2005/08/05 12:54:57 geconomos Exp $
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Description: Convert to and from Hounsfield units 8 bit and 16 bit
// Created:     September 28, 2000
// Author:      Ingmar Bitter
//
// Complete History at bottom of file.

/// convert to HU
int2 HUconverter::Uchar2HU(const uint1 uc)
{
  return(uc*2000/255-1000);
} // Uchar2HU

int2 HUconverter::Short2HU(const int2 s)
{
  return(s-1000);
} // Short2HU


/// convert to uchar
uint1 HUconverter::HU2uchar(int2 hu)
{
  hu = Bound(int2(-1000),hu,int2(1000));
  return((hu+1000)*255/2000);
} // HU2uchar

uint1 HUconverter::Short2uchar(int2 s)
{
  s = Bound(int2(0),s,int2(2000));
  return(s*255/2000);
} // Short2uchar


/// convert to short
int2 HUconverter::HU2short(const int2 hu)
{
  return(hu+1000);
} // HU2short

int2 HUconverter::Uchar2short(const uint1 uc)
{
  return(uc*2000/255);
} // Uchar2short


/// convert to HU
int2 HU::ConvertToHU(const uint1 u1)
{
  return u1*2000/255-1000;
} 

int2 HU::ConvertToHU(const int2 i2)
{
  return i2-1000;
} 


/**
 * Converts a ZeroOne value to a Hounsfield value.
 * Assumes that [0,1] covers 4095 different Hounsfield values.
 * **Rounds** the result to the nearest integer.
 */
int2 HU::ConvertToHU(const ZeroOne zo)
{
  return int2( float4(zo) * 4095.0F + 0.5F ) - 1000.0F;
} 


/// convert to uchar
void HU::ConvertFromHU(int2 hu, int1 & result)
{
  hu = Bound(int2(-1000),hu,int2(1000));
  result = ((hu+1000)*255/2000);
} 

void HU::Convert(int2 i2, int1 & result)
{
  i2 = Bound(int2(0),i2,int2(2000));
  result = (i2*255/2000);
} 


/// convert to short
void HU::ConvertFromHU(const int2 hu, int2 & result)
{
  result = (hu+1000);
} 

void HU::Convert(const uint1 u1, int2 & result)
{
  result = (u1*2000/255);
} 


/// convert to ZeroOne
void HU::ConvertFromHU(const int2 hu, ZeroOne & result)
{
  result = (hu+1000)/4095.0;
} 

/// convert to ZeroOne
ZeroOne HU::ConvertFromHU(const int2 hu)
{
  ZeroOne result;
  ConvertFromHU(hu, result);
  return result;
} 

void HU::Convert(const uint1 u1, ZeroOne & result)
{
  int2 hu(ConvertToHU(u1));
  ConvertFromHU(hu,result);
} 


void HU::Convert(const int2 i2, ZeroOne & result)
{
  int2 hu(ConvertToHU(i2));
  ConvertFromHU(hu,result);
} 


void HU::Convert(const ZeroOne & zeroOne, int2 & result)
{
  result = int2(float(zeroOne)*4095);
} 


void HU::Convert(const ZeroOne & zeroOne, int4 & result)
{
  result = int4(float(zeroOne)*4095);
} 

 



// $Log: HUconverter.inl,v $
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.7  2002/10/02 17:35:52  frank
// Properly rounded HU.
//
// Revision 3.6  2002/10/01 13:48:10  michael
// Added another helper function for conversion that returns the result rather
// than taking it as input reference.
//
// Revision 3.5  2002/04/21 22:57:40  michael
// Changed HU converter to return int2 instead of requiring it to be a second parameter
//
// Revision 3.4  2002/03/27 19:00:38  ingmar
// added conversions from ZeroOne to int2 and int4
//
// Revision 3.3  2002/03/11 17:39:07  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.2.2.1  2002/02/21 20:18:59  frank
// Coding standards compliance.
//
// Revision 3.2  2002/01/31 20:21:52  jenny
// changed result = (float(zo)*4095-1000);
// to result = (int2(float(zo)*4095)-1000);
//
// Revision 3.1  2001/12/21 17:49:16  ingmar
// added conversion to and from ZeroOne
//
// Revision 3.0  2001/10/14 23:02:27  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:49:08   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:40   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:07:25  soeren
// renamed datastruct directory
//
// Revision 1.7  2001/08/19 22:29:08  ingmar
// added class HU
//
// Revision 1.6  2001/03/29 17:22:46  jmeade
// (const type & var) instead of (type var) where appropriate;
// unsigned/signed corrections
//
// Revision 1.5  2001/03/29 16:18:22  dmitry
// Updated datatypes, implemented proper casting
//
// Revision 1.4  2000/10/31 14:15:07  frank
// Merged in new segment structure
//
// Revision 1.2.6.1  2000/10/28 19:31:06  ingmar
// merged in changes from v1k-newSegments branch
//
// Revision 1.3  2000/10/31 13:53:10  frank
// Merged in new skeleton and segment structure
//
// Revision 1.2.6.1  2000/10/28 19:31:06  ingmar
// merged in changes from v1k-newSegments branch
//
// Revision 1.2  2000/10/13 04:47:17  jeff
// PARENTHESIZE!!!!!!!
//
// Revision 1.1  2000/09/28 14:50:58  jeff
// Hundsfeld unit converter
//
//
// $Id: HUconverter.inl,v 1.1 2005/08/05 12:54:57 geconomos Exp $
