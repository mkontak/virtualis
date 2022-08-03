// $Id: MatrixNxM.C,v 1.1 2005/09/13 12:59:56 geconomos Exp $
//
// Copyright © 2002 Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: S. Yoakum-Stover   suzi@viatronix.net


// includes
#include "stdafx.h"
#include "MatrixNxM.h"


/**
 * Default Constructor.
 */
MatrixNxM::MatrixNxM() 
{
  m_uNumRows = 0;
  m_uNumCols = 0;
  m_hV = NULL;
} // MatrixNxM()


/**
 * Builds a zero matrix with uNRows and uMCols.
 *
 * @param uNRows the y-size of the matrix.
 * @param uMCols the x-size of the matrix
 */
MatrixNxM::MatrixNxM(uint4 uNRows, uint4 uMCols) 
{
  m_uNumRows = uNRows;
  m_uNumCols = uMCols;
  m_hV = new float4 * [m_uNumRows];
  for(uint4 r=0; r<m_uNumRows; r++)
  {
    m_hV[r] = new float4[m_uNumCols];
  } // endfor r
} // MatrixNxM(uNRows, uMCols)


/**
 * Builds a matrix with uNRows and uMCols and all elements with value v.
 *
 * @param the initial value of the elements.
 * @param uNRows the y-size of the matrix.
 * @param uMCols the x-size of the matrix
 */
MatrixNxM::MatrixNxM(float4 fV, uint4 uNRows, uint4 uMCols) 
{
  m_uNumRows = uNRows;
  m_uNumCols = uMCols;
  m_hV = new float4 * [m_uNumRows];
  for(uint4 r=0; r<m_uNumRows; r++)
  {
    m_hV[r] = new float4[m_uNumCols];
    for(uint4 c=0;c<m_uNumCols;c++) m_hV[r][c] = fV;
  } // endfor r
} // MatrixNxM(fV, uNRows, uMCols)


/**
 * Default Destrcutor.
 */
MatrixNxM::~MatrixNxM() 
{
  if(m_hV)
  {
    for(uint4 r=0; r<m_uNumRows; r++)
    {
      if (m_hV[r]) delete [] m_hV[r];
    } // endfor r
    delete [] m_hV;
  } // endif m_hV
} // ~MatrixNxM()


/**
 * Change the size o the matrix.
 *
 * @param uNRows the y-size of the matrix.
 * @param uMCols the x-size of the matrix
 */
void MatrixNxM::Resize(uint4 uNRows, uint4 uMCols) 
{
  if(uNRows==m_uNumRows && uMCols==m_uNumCols) return;
  
  if(m_hV)
  {
    for(uint4 r=0; r<m_uNumRows; r++)
    {
      if(m_hV[r]) delete [] m_hV[r];
    } //endfor r
    delete [] m_hV;
  }  // endif m_hV
  
  m_uNumRows = uNRows;
  m_uNumCols = uMCols;
  m_hV = new float4 * [m_uNumRows];
  for(uint4 r=0; r<m_uNumRows; r++)
  {
    m_hV[r] = new float4[m_uNumCols];
  } //endfor r
} // Resize()


/**
 * [r][c] access function. NOTE: no boundary checking
 *
 * @param r the y-location to access.
 * @param c the x-location to access.
 * @return  tha value at the (row,col) position.
 */
float4 & MatrixNxM::operator()(uint4 uRow, uint4 uCol)
{
  return m_hV[uRow][uCol];
} // operator(uRow, uCol)


/**
 * [r][c] access function. NOTE: no boundary checking
 *
 * @param r the y-location to access.
 * @param c the x-location to access.
 * @return  tha value at the (row,col) position.
 */
float4 MatrixNxM::operator()(uint4 uRow, uint4 uCol) const
{
  return m_hV[uRow][uCol];
} // operator(uRow, uCol) const


/**
 * Debug output.
 *
 * @param   message      the header to print the file before the matrix data.
 */
void MatrixNxM::PrintToFile(std::string message)
{
  std::ofstream outStream("C:/V-System/Log/Registration.txt", std::ios::app); outStream.precision(2); outStream.setf(std::ios::showpoint | std::ios::fixed | std::ios::showpos);
  outStream << message << std::endl;
  for (int r=0; r<m_uNumRows; r++)
  {
    for (int c=0; c<m_uNumCols; c++)
    {
      outStream << m_hV[r][c] << "  ";
    } // endfor c
    outStream << std::endl;
  } // endfor r
  outStream << std::endl;
} // PrintToFile()


// $Log: MatrixNxM.C,v $
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.10.2.3  2003/05/27 12:16:12  frank
// code review
//
// Revision 1.10.2.2  2003/05/23 19:34:12  kevin
// Code Walkthrough cosmetic changes
//
// Revision 1.10.2.1  2003/05/01 16:56:06  kevin
// Code Walkthrough cosmetic changes
//
// Revision 1.10  2002/10/31 20:04:40  kevin
// Debug bug.   De-bug-bug    de-bug-bug-bug-bug    de-bug-bug
//
// Revision 1.9  2002/10/23 00:25:08  kevin
// Code cleanup after last round of S/P registration algorithm development
//
// Revision 1.7  2002/09/11 19:13:58  kevin
// Proper deleting of the pointer to the valarray stl object
//
// Revision 1.6  2002/08/30 19:01:27  kevin
// Move S/P Registration debug output to C:\V-system
//
// Revision 1.5  2002/06/20 22:36:38  suzi
// Removed duplicate method bodies.
//
// Revision 1.4  2002/06/20 21:52:31  suzi
// Fixed - operator overload.
//
// Revision 1.3  2002/06/19 19:50:01  ana
// Bugs fixes in the new registration.
//
// Revision 1.2  2002/06/12 20:10:16  suzi
// Huge refactoring and implementation of new registration with multiple seeds.
//
// Revision 1.1  2002/06/07 19:46:03  suzi
// First implementation of MatchingMetrics (registration code).
//
// Revision 1.5  2002/06/03 19:38:37  suzi
// Added normal form.
//
// Revision 1.4  2002/05/02 18:31:28  suzi
// Added default constructor.
//
// Revision 1.3  2002/05/01 22:50:29  suzi
// no message
//
// Revision 1.2  2002/04/30 21:08:04  suzi
// Added more operator overloads and a GetMaxValue function.
//
// Revision 1.1  2002/04/30 19:23:08  suzi
// no message
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/MatrixNxM.C,v 1.1 2005/09/13 12:59:56 geconomos Exp $
// $Id: MatrixNxM.C,v 1.1 2005/09/13 12:59:56 geconomos Exp $
