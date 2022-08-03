// $Id: vxUndoActionAnnotation.C,v 1.8 2006/09/27 17:52:48 romy Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


/**
 * Implements an undo action for segmentation
 */


// includes
#include "StdAfx.h"
#include "vxUndoActionAnnotation.h"
#include "vxEnvironment.h"
#include "vxAnnotation.h"
#include "vxUtils.h"


// defines
#define FILE_REVISION "$Revision: 1.8 $"


/**
 * constructor
 *
 * @param pAnnotations  the annotations object
 * @param iIndex        the index of the annotation that was recently added
 */
vxUndoActionAnnotationAdd::vxUndoActionAnnotationAdd( vxShareableObject< vxAnnotations > * pAnnotations, const int4 iAddedIndex )
: vxUndoAction(),
m_pAnnotations( pAnnotations ),
m_iAddedIndex( iAddedIndex )
{
} // vxUndoActionAnnotationAdd()


/**
 * destructor
 */
vxUndoActionAnnotationAdd::~vxUndoActionAnnotationAdd()
{
}//~vxUndoActionAnnotationAdd


/**
 * Undo the action
 */
void vxUndoActionAnnotationAdd::ExecuteAction()
{

  // an annotation was recently added...remove it
  LogDbg( "removing recently added annotation", "vxUndoActionAnnotationAdd", "ExecuteAction" );
  m_pAnnotations->Remove( m_iAddedIndex );
  m_pAnnotations->Modified( vxModIndInfo( vxAnnotations, m_iAddedIndex, vxUtils::ANNOTATIONS_REMOVED ) );

} // ExecuteAction()


/**
 * constructor
 *
 * @param pAnnotations  the annotations object
 * @param iIndex        the index of the annotation that was recently selected
 */
vxUndoActionAnnotationSelect::vxUndoActionAnnotationSelect( vxShareableObject< vxAnnotations > * pAnnotations, const int4 iSelectedIndex )
: vxUndoAction(),
m_pAnnotations( pAnnotations ),
m_iSelectedIndex( iSelectedIndex )
{
} // vxUndoActionAnnotationSelect()


/**
 * destructor
 */
vxUndoActionAnnotationSelect::~vxUndoActionAnnotationSelect()
{
}//~vxUndoActionAnnotationSelect


/**
 * Undo the action
 */
void vxUndoActionAnnotationSelect::ExecuteAction()
{

  // an annotation was recently selected...re-select the old one
  LogDbg( "unselecting annotation", "vxUndoActionAnnotationSelect", "ExecuteAction" );
  m_pAnnotations->SetActiveIndex( m_iSelectedIndex );
  m_pAnnotations->Modified( vxModIndInfo( vxAnnotations, m_iSelectedIndex, vxUtils::ANNOTATIONS_SELECTED ) );

} // ExecuteAction()



/**
 * constructor
 *
 * @param pAnnotations       the annotations object
 * @param eRecentAction      the type of the recent action on the annotations object
 * @param pRemovedAnnotation the annotation that was recently removed
 */
vxUndoActionAnnotationRemove::vxUndoActionAnnotationRemove( vxShareableObject< vxAnnotations > * pAnnotations,
                                                            vxAnnotation * pRemovedAnnotation )
: vxUndoAction(),
m_pAnnotations( pAnnotations ),
m_pRemovedAnnotation( pRemovedAnnotation )
{
} // vxUndoActionAnnotation()

/**
 * destructor
 */
vxUndoActionAnnotationRemove::~vxUndoActionAnnotationRemove()
{ 
	delete m_pRemovedAnnotation;
	m_pRemovedAnnotation = NULL;	
}//~vxUndoActionAnnotationRemove()


/**
 * Undo the action
 */
void vxUndoActionAnnotationRemove::ExecuteAction()
{
  // an annotation was recently removed...add it back in
  LogDbg( "adding back recently added annotation", "vxUndoActionAnnotationRemove", "ExecuteAction" );
	if ( m_pRemovedAnnotation != NULL )
	{
		m_pAnnotations->Add( m_pRemovedAnnotation );
		m_pRemovedAnnotation = NULL;
		
		m_pAnnotations->Modified( vxModIndInfo( vxAnnotations, m_pAnnotations->GetActiveIndex(), vxUtils::ANNOTATIONS_ADDED ) );
	}
} // ExecuteAction()


/**
 * constructor
 *
 * @param pAnnotations  the annotations object
 * @param iIndex        the index of the annotation that was recently added
 */
vxUndoActionAnnotationAdjust::vxUndoActionAnnotationAdjust( vxShareableObject< vxAnnotations > * pAnnotations,
																							vxAnnotation * pAdjustedAnnotation , const int4 iAdjustedIndex )
: vxUndoAction(),
 m_pAnnotations( pAnnotations ),
 m_iAdjustedIndex( iAdjustedIndex ),
 m_pAdjustedAnnotation ( pAdjustedAnnotation->Clone())
{
} // vxUndoActionAnnotationAdd()


/**
 * destructor
 */
vxUndoActionAnnotationAdjust::~vxUndoActionAnnotationAdjust() 
{
	delete m_pAdjustedAnnotation;
	m_pAdjustedAnnotation = NULL; 
}//~vxUndoActionAnnotationAdjust() 


/**
 * Undo the action
 */
void vxUndoActionAnnotationAdjust::ExecuteAction()
{
  // an annotation was recently added...remove it
  LogDbg( "reverting back to the recently added annotation", "vxUndoActionAnnotationAdd", "ExecuteAction" );
	
	if ( m_iAdjustedIndex < 0 || m_pAdjustedAnnotation == NULL )
		return;

	if ( m_pAnnotations->GetCount() > 0 )
	{
		vxAnnotation * pOldAnnotation = m_pAnnotations->Swap( m_iAdjustedIndex, m_pAdjustedAnnotation );
		delete pOldAnnotation; 
		m_pAdjustedAnnotation = NULL;

		m_pAnnotations->Modified( vxModIndInfo( vxAnnotations, m_iAdjustedIndex, vxUtils::ANNOTATIONS_MODIFIED ) );
	}

} // ExecuteAction()



// undefines
#undef FILE_REVISION


// $Log: vxUndoActionAnnotation.C,v $
// Revision 1.8  2006/09/27 17:52:48  romy
// better memory management implemented
//
// Revision 1.7  2006/08/08 04:41:57  romy
// Undo final touch
//
// Revision 1.6  2006/08/08 04:11:40  romy
// Remove Undo fixed
//
// Revision 1.5  2006/08/07 20:57:31  romy
// added vxUndoActionAnnotationAdjust
//
// Revision 1.4  2006/08/07 20:26:23  romy
// added vxUndoActionAnnotationAdjust
//
// Revision 1.3  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
//
// Revision 1.2  2006/02/21 21:21:35  frank
// added more undo for annotations
//
// Revision 1.1  2006/02/21 19:44:44  frank
// added undo for annotations
//
// Revision 1.3  2006/02/17 15:12:53  frank
// working on undoing all segmentation actions
//
// Revision 1.2  2006/02/09 14:07:54  geconomos
// fixed usage of enum
//
// Revision 1.1  2006/02/01 21:22:34  frank
// added undo actions for some segmentation operations
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxUndoActionAnnotation.C,v 1.8 2006/09/27 17:52:48 romy Exp $
// $Id: vxUndoActionAnnotation.C,v 1.8 2006/09/27 17:52:48 romy Exp $
