// $Id: Jitter.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright© 2000,2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.com)
//
// Complete History at bottom of file.




/**
 * Retrive a jitter value for a pixel.
 *
 * @param uPositionX the x coordinate of the pixel.
 * @param uPositionY the y coordinate of the pixel.
 * @return the floating point jitter value for the specified pixel.
 */
float4 Jitter::GetValue(const uint4 uPositionX, const uint4 uPositionY) const
{
  uint4 uX((uPositionX & m_eBitMask));
  uint4 uY((uPositionY & m_eBitMask) << m_eBitShift);
  return m_pfCurrentBuffer[ uX | uY ];
} // GetValue()


/**
 * Update the randomness; call once per frame.
 */
void Jitter::SwapBuffers()
{
  uint4 uMaxIndex((m_eNumBuffers-1)*m_eSize*m_eSize-1);

  // choose a _uniform_ random number between 0 and uMaxIndex
  // +1 to have an equal chance of rounding down to uMaxIndex compared to any other
  uint4 uNewBufferStartIndex(int(Drand48(0.0F, uMaxIndex+1.0F)));

  // eliminate the remote possability that it selected uMaxIndex exactly
  uNewBufferStartIndex = Bound(uint4(0), uNewBufferStartIndex, uMaxIndex);
  m_pfCurrentBuffer = m_vfAllBuffers + uNewBufferStartIndex;

} // SwapBuffers()


// $Log: Jitter.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/05/16 11:45:25  michael
// code review
//
// Revision 1.1  2002/07/17 15:30:52  ingmar
// moved Jitter class from v3DColon/Readlib to 75_renderUtils
//
// Revision 3.0  2001/10/14 23:01:53  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:12   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:00   ingmar
// Initial revision.
// Revision 1.4  2001/06/26 16:21:28  ana
// Results of code review for Fields and Jitter classes
//
// Revision 1.3  2001/04/10 13:13:39  frank
// Code walkthrough results.
//
// Revision 1.2  2001/03/19 12:18:43  frank
// Updated to reflect new coding standards.
//
// Revision 1.1.1.1  2000/09/11 00:33:24  ingmar
// unit tests complete
//
// 
// 4     8/28/00 12:23p Antonio
// updated header and footer
//
//*****************  Version 3  *****************
//User: Jeff            Date:  7/18/00  Time:  1:57p
//Branched at version 3
//Comment:
//  
//
//*****************  Version 2  *****************
//User: Frank           Date:  7/18/00  Time: 11:09a
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Added history and header
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created Jitter.i
//Comment:
//  
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/Jitter.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: Jitter.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
