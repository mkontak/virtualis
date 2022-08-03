// $Id: vxUndoActionSegmentation.C,v 1.6 2006/09/27 17:55:02 romy Exp $
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
#include "vxUndoActionSegmentation.h"
#include "vxUtils.h"


// defines
#define FILE_REVISION "$Revision: 1.6 $"


/**
 * constructor
 *
 * @param pEnvironment the environment
 */
vxUndoActionSegmentation::vxUndoActionSegmentation( vxShareableObject< vxBlockVolume<uint2> > * pLabelVolume,
                                                    vxShareableObject< vxComponentArray > * pComponents )
: vxUndoAction(),
m_labelVolumePrevious( * pLabelVolume ),
m_pLabelVolumeCurrent( pLabelVolume ),
m_componentsPrevious( * pComponents ),
m_pComponentsCurrent( pComponents )
{
} // vxUndoActionSegmentation()


/**
 * destructor
 */
vxUndoActionSegmentation::~vxUndoActionSegmentation()
{
}//~vxUndoActionSegmentation

/**
 * Undo the action
 */
void vxUndoActionSegmentation::ExecuteAction()
{

  // update the label volume and set modified
  * m_pLabelVolumeCurrent = m_labelVolumePrevious;
  m_pLabelVolumeCurrent->Modified( vxModInfo( vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_MODIFIED ) );

  // update the components and set modified
  * m_pComponentsCurrent = m_componentsPrevious;
  m_pComponentsCurrent->Modified( vxModIndInfo( vxComponentArray, 0, vxUtils::COMPONENTARRAY_MODIFIED ) );
  m_pComponentsCurrent->Modified( vxModIndInfo( vxComponentArray, m_componentsPrevious.GetSelectedComponentIndex(), vxUtils::COMPONENTARRAY_SELECTED ) );

} // ExecuteAction()


/**
 * constructor
 *
 * @param pComponents the environment
 */
vxUndoActionComponents::vxUndoActionComponents( vxShareableObject< vxComponentArray > * pComponents )
: vxUndoAction(),
m_componentsPrevious( * pComponents ),
m_pComponentsCurrent( pComponents )
{
} // vxUndoActionComponents()


/**
 * Undo the action
 */
void vxUndoActionComponents::ExecuteAction()
{

  * m_pComponentsCurrent = m_componentsPrevious;
  m_pComponentsCurrent->Modified( vxModIndInfo( vxComponentArray, 0, vxUtils::COMPONENTARRAY_MODIFIED ) );
  m_pComponentsCurrent->Modified( vxModIndInfo( vxComponentArray, m_componentsPrevious.GetSelectedComponentIndex(), vxUtils::COMPONENTARRAY_SELECTED ) );

} // ExecuteAction()


// undefines
#undef FILE_REVISION


// $Log: vxUndoActionSegmentation.C,v $
// Revision 1.6  2006/09/27 17:55:02  romy
// destructor added
//
// Revision 1.5  2006/03/29 20:03:08  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.4  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxUndoActionSegmentation.C,v 1.6 2006/09/27 17:55:02 romy Exp $
// $Id: vxUndoActionSegmentation.C,v 1.6 2006/09/27 17:55:02 romy Exp $
