// $Id: vxColorTableCPU.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Wei Li (liwei@viatronix.com)

/**
 *  color tabel of CPU
 */

#ifndef vxColorTableCPU_h
#define vxColorTableCPU_h


// includes
#include "Typedef.h"
#include "vxColorTable.h"
#include "assert.h"


// class definition
class VX_VIEWER_DLL vxColorTableCPU : public vxColorTable 
{
// member functions
public:

	/// default constructor
  vxColorTableCPU() { m_uNumEntries = 0; };

  /// constructor
  vxColorTableCPU(uint2 uMaxIndex);

  /// copy constructor
  vxColorTableCPU(const vxColorTableCPU & other);

	/// destructor
	virtual ~vxColorTableCPU();

  /// assignment operator
  const vxColorTableCPU & operator=(const vxColorTableCPU &);

  /// return values in the red table
  const uint2 GetRed(const uint2 index) const;

  /// set values to the red color
  void SetRed(const uint2 index, const uint2 value);

  /// return values in the green table
  const uint2 GetGreen(const uint2 index) const;

  /// set values to the green table
  void SetGreen(const uint2 index, const uint2 value);

  /// return values in the blue table
  const uint2 GetBlue(const uint2 index) const;

  /// set values to the blue table
  void SetBlue(const uint2 index, const uint2 value);

  /// return values in the alpha table
  const uint2 GetAlpha(const uint2 index) const;
  
  /// set values to the alpha table
  void SetAlpha(const uint2 index, const uint2 value);

  /// return the value of the specifice channel at position index
  const uint2 GetColor(const nsCOLOR::COLOR_CHANNEL color, const uint2 index);

  /// set the value of the specifice channel at position index
  void SetColor(const nsCOLOR::COLOR_CHANNEL color, const uint2 index, const uint2 value);
  
  /// return the table pointer
  float4 * GetTablePointerRGBA() { return m_pfTable; }

//Data members
private:

  /// pointer to actual array
  float4 * m_pfTable;

  /// number of entires in array
	uint4 m_uNumEntries;
};


#endif // vxColorTableCPU_h


// Revision History:
// $Log: vxColorTableCPU.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/06/09 12:29:54  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.9  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.8  2003/04/25 19:16:09  dongqing
// code review
//
// Revision 1.7  2003/04/19 01:49:08  michael
// some more code review
//
// Revision 1.6  2003/04/16 12:47:27  michael
// performed initial code review
//
// Revision 1.5  2003/03/30 20:58:31  michael
// formatting and cosmetics
//
// Revision 1.4  2002/10/02 22:49:24  frank
// Removed promotion of namespaces in header files!
//
// Revision 1.3  2002/09/25 20:36:41  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.2  2002/09/24 16:58:04  michael
// cleaned-up colortable and colormap ... hopefully we'll be able to remove
// member colortable from colormap.
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 3.4  2002/07/18 15:18:53  ingmar
// removed obsolete Initialize()
// added destructor to clean up memory leak
//
// Revision 3.3  2002/05/04 10:41:51  michael
// Removed unnecesassary cast
//
// Revision 3.2  2002/05/03 20:30:30  soeren
// scaling of alpha on get removed
//
// Revision 3.1  2002/04/25 01:39:57  michael
// no more VLI table, only ColorTableCpu!
//
// Revision 3.0  2001/10/14 23:02:30  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:49:46   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:24   ingmar
// Initial revision.
// Revision 1.5  2001/08/24 22:22:57  liwei
// Code beautify.
//
// Revision 1.4  2001/08/23 21:40:21  liwei
// {Set, Get}{Red, Green, Blue, Alpha}() are changed to inline functions while
// {Set, Get}Color() changed to virtual functions.
//
// Revision 1.3  2001/08/03 23:42:51  liwei
// TransferFunction now contains several lookup tables as its data members.
//
// Revision 1.2  2001/08/03 22:58:05  liwei
// Directly store the table as float*, not using PixelRGBA any more.
//
// Revision 1.1  2001/08/03 22:32:56  liwei
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxColorTableCPU.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxColorTableCPU.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
