// $Id: vxEnvironment.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


/**
 * Gets an element from the environment
 * @param id ID of element to retrieve
 * @return element
 */
template<class T>
vxShareableObject<T> * vxEnvironment::GetElement(const vxID & id) const
//*******************************************************************
{
  // verify that element exists in the environmnt
  vxElements::const_iterator elementIter(m_elements.find(id));

  if (elementIter == m_elements.end())
  {
    throw ex::IllegalArgumentException(LogRec("Enviornment element not found.", "vxEnvironment", "AddElement"));
  }
  
  // try to cast the element to desired type
  T * pElement = dynamic_cast< T* >(elementIter->second);
  if (pElement == NULL)
  {
    std::string sMsg("Unable to cast element to type: ");
    throw ex::InvalidCastException(sMsg, "vxEnvironment", "AddElement"));
  }

  return pElement;
} // GetElement()


// Revision History
// $Log: vxEnvironment.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/08/20 14:09:46  michael
// added "mailto:"
//
// Revision 1.3  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.2  2003/05/06 18:25:50  michael
// initial code review
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.2  2002/06/07 19:59:55  geconomos
// Changed extended elements fron shareable object to vxElement.
//
// Revision 1.1  2002/06/05 21:38:11  geconomos
// Initial extended element implementation.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxEnvironment.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxEnvironment.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
