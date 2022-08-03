// $Id: vxAnnotations.C,v 1.17 2006/11/23 08:02:41 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Michael Meissner (mailto:meissner@viatronix.com)


// pragmas
#pragma warning (disable : 4786)


// includes
#include "stdafx.h"
#include "vxAnnotations.h"
#include "vxSerializer.h"
#include "utilities.h"


// defines
#define FILE_REVISION "$Revision: 1.17 $"


/**
 * Default Constructor
 */
vxAnnotations::vxAnnotations()
:m_iActiveAnnotationIndex(-1)
{
}


/**
 * Copy constructor
 * @param annotations object
 */
vxAnnotations::vxAnnotations(const vxAnnotations & other)
{
	m_vpAnnotations.clear();
	for ( int index=0; index < other.m_vpAnnotations.size(); index++ )
	{
		vxAnnotation * pAnnotation = other.m_vpAnnotations[index]->Clone();
		m_vpAnnotations.push_back( pAnnotation );
	}

	m_iActiveAnnotationIndex = other.m_iActiveAnnotationIndex;
} // copy constructor


/**
 * Assignment operator
 * @param annotations object
 * @return const reference to this
 */
const vxAnnotations & vxAnnotations::operator=(const vxAnnotations & other)
{
  if (this != &other)
  {
		m_vpAnnotations.clear();
		for ( int index=0; index < other.m_vpAnnotations.size(); index++ )
		{
			vxAnnotation * pAnnotation = other.m_vpAnnotations[index]->Clone();
			m_vpAnnotations.push_back( pAnnotation );
		}

    m_iActiveAnnotationIndex = other.m_iActiveAnnotationIndex;
  }

  return *this;
} // operator=()


/** 
 * Destructor
 */
vxAnnotations::~vxAnnotations()
{
  for ( int index =0; index < m_vpAnnotations.size() ; ++index )
  {
		try
		{
			vxAnnotation * pAnnotation = m_vpAnnotations[ index ];
			delete pAnnotation;
			pAnnotation = NULL;
		}
		catch ( ... )
		{
			LogErr ( "Error in cleaning up of annotations", "~vxAnnotations", "vxAnnotations" );
		}
  }
}//~vxAnnotations


/**
 * Write object to XML node
 * @param node
 */
void vxAnnotations::ToXml(vxDOMElement & element) const
{
  int4 iActiveAnnotation(GetActiveIndex());

  vxSerializer::SetAttribute(element, "version", "2.0");
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(iActiveAnnotation), "active index"), iActiveAnnotation);
  vxSerializer::WriteISerialize(vxSerializer::AddElement(element, vxSerializer::GetElementName(m_vpAnnotations), "annotations"), m_vpAnnotations);
} // ToXml()


/**
 * initialize from XML node
 * @param node
 * @return successful?
 */
bool vxAnnotations::FromXml(vxDOMElement & element)
{
  if (element.IsNull()) 
    return false;

  bool bSuccess(true);

  std::string sVersion("0.0");
  bSuccess &= vxSerializer::GetAttribute(element, "version", sVersion);
  if (atof(sVersion.c_str()) == 2.0)
  {
    int4 iActiveAnnotation(-1);
    std::vector<vxAnnotation*> vpAnnotation;

    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(iActiveAnnotation), "active index"), iActiveAnnotation);
    bSuccess &= vxSerializer::ReadISerialize(vxSerializer::FindElement(element, vxSerializer::GetElementName(vpAnnotation), "annotations"), vpAnnotation);
    m_vpAnnotations = vpAnnotation;
    iActiveAnnotation;
  }
  else
  {
    LogDbg("Unknown version number for XML", "vxAnnotations", "FromXml()");
  }

  return bSuccess;
} // FromXml()


/**
 * SetActiveAnnotationIndex
 * @param index
 */
void vxAnnotations::SetActiveIndex(int4 iIndex)
{
  if (iIndex < -1 || iIndex >= (int4)m_vpAnnotations.size())
    throw ex::IndexOutOfBoundsException(LogRec("invalid index (" + ToAscii(iIndex) + ")", "vxAnnotations", "SetActiveAnnotationIndex"));
  m_iActiveAnnotationIndex = iIndex;
}


/**
 * Get annotation from the annotation vector
 * @param index
 * @return pointer to annotation
 */
vxAnnotation * vxAnnotations::Get(int4 iIndex)
{
  if (iIndex < 0 || iIndex >= (int4)m_vpAnnotations.size())
    throw ex::IndexOutOfBoundsException(LogRec("invalid index (" + ToAscii(iIndex) + ")", "vxAnnotations", "GetAnnotation"));

  return m_vpAnnotations[iIndex];
} // Get()


/**
 * Add an annotation to vector of annotations
 * @param pointer to annotation to be added
 */
void vxAnnotations::Add(vxAnnotation * pAnnotation)
{
  m_vpAnnotations.push_back(pAnnotation);
  SetActiveIndex(GetCount() - 1);

  return;
} // Add()


/**
 * Remove the indexed annotation
 * @param index
 */
void vxAnnotations::Remove(int4 iIndex)
{
  if (iIndex < 0 || iIndex >= m_vpAnnotations.size())
    throw ex::IndexOutOfBoundsException(LogRec("invalid index (" + ToAscii(iIndex) + ")",
    "vxAnnotations",
    "GetAnnotation"));

	//vxAnnotation * pDeprecated =  m_vpAnnotations[ iIndex ];

  m_vpAnnotations.erase(m_vpAnnotations.begin()+iIndex);

	//Don't delete it. Let it erased from the Undo Stack.
	//delete pDeprecated;
	//pDeprecated = NULL;

  SetActiveIndex( -1 );

  return;
} // Remove()

/**
 * Removes all annotations
 */
void vxAnnotations::RemoveAll( )
{
	//Let the memory be managed by the Undo Stack.
	/*for ( int index =0; index < m_vpAnnotations.size() ; ++index )
  {
		try
		{
			vxAnnotation * pAnnotation = m_vpAnnotations[ index ];
			delete pAnnotation;
			pAnnotation = NULL;
		}
		catch ( ... )
		{
			LogErr ( "Error in cleaning up of annotations", "~vxAnnotations", "vxAnnotations" );
		}
  }*/
	m_vpAnnotations.clear();
  SetActiveIndex(-1);
} // RemoveAll()


/**
 * Swaps an annotation from a specific index
 */
vxAnnotation * vxAnnotations::Swap( int4 iIndex, vxAnnotation * pAnnotation )
{
	if (iIndex < -1 || iIndex >= (int4)m_vpAnnotations.size())
   throw ex::IndexOutOfBoundsException(LogRec("invalid index (" + ToAscii(iIndex) + ")", "vxAnnotations", "Swap"));

	vxAnnotation * pOriginal = m_vpAnnotations[ iIndex ];
  m_vpAnnotations[ iIndex ] = pAnnotation;

	return pOriginal;
}//Swap


/**
 * Replaces an item from its position
 */
void vxAnnotations::Replace( int4 iIndex, vxAnnotation * pAnnotation )
{
 if (iIndex < -1 || iIndex >= (int4)m_vpAnnotations.size())
   throw ex::IndexOutOfBoundsException(LogRec("invalid index (" + ToAscii(iIndex) + ")", "vxAnnotations", "Replace"));

 m_vpAnnotations[ iIndex ] = pAnnotation;

}//Replace


/**
 * Get active annotation
 * @return pointer to annotation
 */
vxAnnotation * vxAnnotations::GetActive()
{
  const int4 iIndex(GetActiveIndex());
  if (iIndex < 0)
    return NULL;
  else
    return m_vpAnnotations[GetActiveIndex()];
} // GetActive()


#undef FILE_REVISION


// Revision History:
// $Log: vxAnnotations.C,v $
// Revision 1.17  2006/11/23 08:02:41  romy
// fixed linker error
//
// Revision 1.13  2006/09/27 17:51:04  romy
// commented the deletion of Annotation objects. They are now managing from the UndoAction stack point of view
//
// Revision 1.12  2006/09/21 15:13:20  romy
// added annotation removeAll
//
// Revision 1.11  2006/08/08 04:39:08  romy
// bug fix
//
// Revision 1.10  2006/08/08 04:32:32  romy
// minor log message correction
//
// Revision 1.9  2006/08/08 04:03:52  romy
// safe delete
//
// Revision 1.8  2006/08/07 21:01:12  romy
// destructor modified
//
// Revision 1.7  2006/08/07 20:57:31  romy
// added vxUndoActionAnnotationAdjust
//
// Revision 1.6  2006/08/07 18:48:00  romy
// Added replace method
//
// Revision 1.5  2006/02/21 19:45:07  frank
// formatting
//
// Revision 1.4  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.3  2005/11/22 21:37:57  frank
// fixed problem removing annotations
//
// Revision 1.2  2005/04/12 15:23:32  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.17  2004/01/05 18:59:49  michael
// added "mailto:"
//
// Revision 1.16  2003/09/22 21:11:50  michael
// added "mailto:"
//
// Revision 1.15  2003/05/16 13:17:40  frank
// formatting
//
// Revision 1.14  2003/04/24 13:46:29  michael
// initial code reviews
//
// Revision 1.13  2003/04/01 17:53:05  michael
// cosmetics
//
// Revision 1.12  2003/03/18 17:15:31  michael
// fixing parts of issue #2898, where the creation interrupted by 'ESC' is causing
// an invalid access after deleting the annotation in the current active one (in the
// ~vxManipulatorAnnotationCreate)
//
// Revision 1.11  2003/03/04 03:26:08  michael
// took out silly debug information (my own :-)
//
// Revision 1.10  2003/03/04 01:43:24  michael
// fixed 'ESC' on creating annotations
//
// Revision 1.9  2003/02/27 00:59:06  michael
// fixed error in FromXML() in case there is no annotation (negative index)
//
// Revision 1.8  2003/02/22 00:22:56  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.7  2003/02/19 18:22:59  michael
// changes to account for serialization of annotations
//
// Revision 1.6  2003/01/17 17:39:40  michael
// text of annotations can now be selected and moved
//
// Revision 1.5  2003/01/15 23:53:49  michael
// added annotation manipulation and handle highlighting
//
// Revision 1.4  2003/01/15 16:30:55  michael
// added defines for FILE_VERSION
//
// Revision 1.3  2003/01/14 23:36:27  michael
// furter adjustments to support point, arrow, distance and angle
//
// Revision 1.2  2003/01/13 14:43:56  geconomos
// Set the active annotation in AddAnnotation().
//
// Revision 1.1  2003/01/09 14:54:36  michael
// added new annotations class that represents all available annotations
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxAnnotations.C,v 1.17 2006/11/23 08:02:41 romy Exp $
// $Id: vxAnnotations.C,v 1.17 2006/11/23 08:02:41 romy Exp $
