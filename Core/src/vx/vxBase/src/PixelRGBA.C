// $Id: PixelRGBA.C,v 1.3 2004/03/05 19:34:32 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Ingmar ingmar@viatronix.com


// includes
#include "stdafx.h"
#include "PixelRGBA.h"


// defines
#define FILE_REVISION "$Revision: 1.3 $"


// namespaces
using namespace std;


// explicit instantiations of most frequently used types
template class VX_BASE_DLL PixelRGBA<uint1>;    
template class VX_BASE_DLL PixelRGBA<uint2>;    
template class VX_BASE_DLL PixelRGBA<float4>;


/**
 * Get a pixel color from a color palette that has easy to differentiate colors as neighbors
 * 
 * @param iIndex    desired palette index
 */
template < class PixelType > 
PixelRGBA<PixelType> PixelRGBA<PixelType>::GetColorPalettePixel( uint1 iIndex )
{
  try
  {
    static std::vector< PixelRGBA<PixelType> > pixelArray(256);
    static bool bFirstTimeCalled(true);

    if ( bFirstTimeCalled )
    {
      bFirstTimeCalled = false;

      /// now fill in the color palette
      static std::vector< PixelRGBA<PixelType> > pixelArraySrc(9);
      //                                                         // color    // order used
      pixelArraySrc[ 0] = PixelRGBA<PixelType>(255,  0,  0,255); // Red         0
      pixelArraySrc[ 1] = PixelRGBA<PixelType>(255,  0,255,255); // Magenta         4
      pixelArraySrc[ 2] = PixelRGBA<PixelType>(  0,  0,255,255); // Blue          2
      pixelArraySrc[ 3] = PixelRGBA<PixelType>(  0,255,255,255); // Cyan              6
      pixelArraySrc[ 4] = PixelRGBA<PixelType>(  0,255,  0,255); // Green        1
      pixelArraySrc[ 5] = PixelRGBA<PixelType>(255,255,  0,255); // Yellow           5
      pixelArraySrc[ 6] = PixelRGBA<PixelType>(255,165,  0,255); // Orange         3
      pixelArraySrc[ 7] = PixelRGBA<PixelType>(  0,165,255,255); // Sky Blue           7
      pixelArraySrc[ 8] = PixelRGBA<PixelType>(255,255,255,255); // White       delta color

      /// reverse bitpattern of k to use as linear interpolation location within the first 9 entries above
      /// the reversing causes colors differing by 3 bits only to be used by indices 128 entries appart
      /// the reversing causes colors differing by 4 bits only to be used by indices  64 entries appart
      /// the reversing causes colors differing by 5 bits only to be used by indices  32 entries appart
      /// the reversing causes colors differing by 6 bits only to be used by indices  16 entries appart
      /// the reversing causes colors differing by 7 bits only to be used by indices   8 entries appart
      for ( uint4 k(0); k < 256; ++k )
      {
        uint1 uPos = // all bits in reverse order
                     1  * (0 != (k &128)) +
                     2  * (0 != (k & 64)) +
                     4  * (0 != (k & 32)) +
                     8  * (0 != (k & 16)) +
                     16 * (0 != (k &  8)) +
                     32 * (0 != (k &  4)) +
                     64 * (0 != (k &  2)) +
                     128* (0 != (k &  1));
        float4 fPos          = (uPos/256.0) * (pixelArraySrc.size()-1);
        uint1  uIndexL       = fPos;
        uint1  uIndexR       = uIndexL + 1;
        float4 fFraction     = fPos - uIndexL;
        float4 fOneMinusFrac = (1.0 - fFraction);

        PixelRGBA<PixelType> pixel(fOneMinusFrac * pixelArraySrc[uIndexL].R() + fFraction * pixelArraySrc[uIndexR].R(),
                                   fOneMinusFrac * pixelArraySrc[uIndexL].G() + fFraction * pixelArraySrc[uIndexR].G(),
                                   fOneMinusFrac * pixelArraySrc[uIndexL].B() + fFraction * pixelArraySrc[uIndexR].B(),
                                   fOneMinusFrac * pixelArraySrc[uIndexL].A() + fFraction * pixelArraySrc[uIndexR].A());
        pixelArray[k] = pixel;
      } // end for ( k < 256 )
    } // end bFirstTimeCalled

    return pixelArray[iIndex];
  }
  catch ( ... )
  {
    throw ex::VException( LogRec( "GetColorPalettePixel failed.","PixelRGBA","GetColorPalettePixel" ) );
  }
} // GetColorPalettePixel()


/**
 * convert class state to string
 */
template <class T> 
std::string PixelRGBA<T>::ToString() const
{
  std::strstream ss; 
  ss << "PixelRGBA("<<R()<<", "<<G()<<", "<<B()<<", "<<A()<<")"<<std::ends; 
  return ss.str();
} // ToString()


#undef FILE_REVISION


// $Log: PixelRGBA.C,v $
// Revision 1.3  2004/03/05 19:34:32  geconomos
// Exported PixelRGBA<uint2>.
//
// Revision 1.2  2004/03/05 16:37:30  geconomos
// Updated to compile with 7.1 compiler.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/06/10 12:06:41  frank
// Code review of pixel classes
//
// Revision 1.6  2003/05/16 11:44:21  michael
// code review
//
// Revision 1.5  2003/05/15 15:54:39  frank
// spelling
//
// Revision 1.4  2002/12/20 19:00:27  ingmar
// finished code review
//
// Revision 1.3  2002/12/20 17:04:50  ingmar
// towards coding standards compliance
//
// Revision 1.2  2002/12/20 15:57:43  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 1.1  2002/06/11 21:22:23  ingmar
// added GetColorPalletPixel()
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/PixelRGBA.C,v 1.3 2004/03/05 19:34:32 geconomos Exp $
// $Id: PixelRGBA.C,v 1.3 2004/03/05 19:34:32 geconomos Exp $
