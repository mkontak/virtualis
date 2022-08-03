// $Id: vxISerializeFactoryManager.C,v 1.2 2004/05/17 18:53:25 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

/**
 * This class manages a list of vxISerializeFactory objects
 * to allow for extensability of e.g. pluygins.
 */



// includes
#include "stdafx.h"
#include "vxISerializeFactoryManager.h"

// defines
#define FILE_REVISION "$Revision: 1.2 $"



// static member initialization
std::list<vxISerializeFactory*> vxISerializeFactoryManager::m_lpFactories = std::list<vxISerializeFactory*>();
vxISerializeFactoryManager * vxISerializeFactoryManager::m_pManager = NULL;


/**
 * Destructor
 */
vxISerializeFactoryManager::~vxISerializeFactoryManager()
//*******************************************************************
{
  std::list<vxISerializeFactory*>::iterator iter(m_lpFactories.begin());

  while (iter != m_lpFactories.end())
  {
    delete *iter;
    ++iter;
  }

  m_lpFactories.clear();
} // destructor


/**
 * Add a factory to the list
 * @return factory manager
 */
vxISerializeFactoryManager & vxISerializeFactoryManager::GetManager()
//*******************************************************************
{
  if (m_pManager != NULL)
    return *m_pManager;

  m_pManager = new vxISerializeFactoryManager();
  return *m_pManager;
} // GetManager


/**
 * Add a factory to the list
 * @param factory
 */
void vxISerializeFactoryManager::AddFactory(vxISerializeFactory * pFactory)
//*******************************************************************
{
  m_lpFactories.push_back(pFactory);
} // AddFactory


/**
 * Remove the given factory from the list
 * @param factory
 * @return success
 */
bool vxISerializeFactoryManager::RemoveFactory(vxISerializeFactory * pFactory)
//*******************************************************************
{
  std::list<vxISerializeFactory*>::iterator iter;
  
  iter = std::find(m_lpFactories.begin(), m_lpFactories.end(), pFactory);
  if (iter != m_lpFactories.end())
  {
    m_lpFactories.erase(iter);
    return true;
  }
  return false;
} // RemoveFactory()


/**
 * Create the object
 * @param name
 * @return serializable object
 */
vxISerialize * vxISerializeFactoryManager::Create(const std::string & sName) const
//*******************************************************************
{
  vxISerialize * pISerialize(NULL);
  std::list<vxISerializeFactory*>::const_iterator iter(m_lpFactories.begin());

  while (iter != m_lpFactories.end() && pISerialize == NULL)
  {
    pISerialize = (**iter).Create(sName);
    ++iter;
  }

  if (pISerialize == NULL)
  {
    throw ex::IllegalArgumentException(LogRec("Object with this name is unknown: " + sName, "vxISerializeFactoryManager", "Create"));
  }
  return pISerialize;
} // Creat()


// $Log: vxISerializeFactoryManager.C,v $
// Revision 1.2  2004/05/17 18:53:25  frank
// made the error more descriptive
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/07 21:00:47  dongqing
// code walkthrough
//
// Revision 1.4  2003/05/06 15:11:39  michael
// initial code review
//
// Revision 1.3  2003/02/22 00:22:56  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.2  2003/02/21 20:01:24  michael
// static member initialization and other changes
//
// Revision 1.1  2003/02/21 19:09:52  michael
// added initial version of the vxISerializeFactory classes
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxISerializeFactoryManager.C,v 1.2 2004/05/17 18:53:25 frank Exp $
// $Id: vxISerializeFactoryManager.C,v 1.2 2004/05/17 18:53:25 frank Exp $
