// $Id: ViaQueue.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViaQueue.C  
// Description: implementation (4/17) of the class ViaStrip 
// Created: July 31, 2000 
// Author: Freeware,
//         rewrited by Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.


#include "stdafx.h"
#include "ViaStrip.h"


/**
 *	Init the (vertices|polygons) list
 *
 *	@param	LHead	pointer (head) of a (vertices|polygons) list.
 *	@return	true on a successful initial.
 */
bool ViaStrip::InitList(ListHead * LHead)
{
	if (LHead == NULL) return false;

	LHead->LHeaders[LISTHEAD] = LHead->LHeaders[LISTTAIL] = NULL;
	LHead->NumList = 0;
	return true;
}


/**
 *	Add an element (vert, face ...) to the list: the operation will occur at the head.
 *
 *	@param	LHead	a space of the list (Vertices ...).
 *	@param	LInfo	element to be added to this space.
 *	@return	true after a successful operation.
 */
bool ViaStrip::AddHead(ListHead * LHead, ListInfo * LInfo)
{
	if (LHead == NULL || LInfo == NULL) 
		return false;

	if (EMPTYLIST(LHead))
		LHead->LHeaders[LISTTAIL] = LInfo;
	else
		LHead->LHeaders[LISTHEAD]->ListNode.Previous = (void *)LInfo;
     
	LInfo->ListNode.Next = (void *) LHead->LHeaders[LISTHEAD];
	LHead->LHeaders[LISTHEAD] = LInfo;
	LInfo->ListNode.Previous = NULL;   
	LHead->NumList ++;
	return true;
}


/**
 *	Add an element to the list : the operation will occur at the tail.
 *
 *	@param	LHead		in which list the element will be added.
 *	@param	LInfo		element to be added.
 *	@return	true on a successful operation.
 */
bool ViaStrip:: AddTail(ListHead * LHead, ListInfo * LInfo)
{
	if (LHead == NULL || LInfo == NULL)
		return false;
     
	if (EMPTYLIST(LHead))         
		LHead->LHeaders[LISTHEAD] = LInfo; 
	else 
		LHead->LHeaders[LISTTAIL]->ListNode.Next = (void *)LInfo;
	
	LInfo->ListNode.Previous = (void *)LHead->LHeaders[LISTTAIL];
	LHead->LHeaders[LISTTAIL] = LInfo;
	LInfo->ListNode.Next = NULL;     
	LHead->NumList++;

	return true;
}


/** 
 *	insert a node to the list.
 *
 *	@param	LHead		in which list the node will be insert.
 *	@param	nPos		position into which the node to be insert.
 *	@param	LInfo		node to be added.
 *	@return	true on a successful operation.
 */
bool ViaStrip::InsertNode(ListHead * LHead, int4 nPos, ListInfo * LInfo)
{
	ListInfo * LAddNode;

	if (LHead == NULL || LInfo == NULL || nPos > NumOnList(LHead))
		return false;

	if (nPos == 0)  
		AddHead(LHead, LInfo); 
	else if (nPos == NumOnList(LHead)) 
		AddTail(LHead, LInfo); 
	else 
	{ 
		if ((LAddNode = PeekList(LHead, LISTHEAD, nPos-1)) == NULL)
			return (FALSE);
		((ListInfo *)LAddNode->ListNode.Next)->ListNode.Previous = LInfo;
		LInfo->ListNode.Next = LAddNode->ListNode.Next;
		LInfo->ListNode.Previous = LAddNode;
		LAddNode->ListNode.Next = LInfo;
		LHead->NumList++;
	}
	return true;
}


/**
 *	Remove an element from the head of (vertices|polygons) list.
 *
 *	@param	LHead		the head of the list.
 *	@return	origianl head of the list if the list is not empty.
 */
ViaStrip::ListInfo * ViaStrip::RemHead(ListHead * LHead)
{
	ListInfo *t, *t1;
	
	if (LHead == NULL || EMPTYLIST(LHead))
		return(NULL);
     
	t = LHead->LHeaders[LISTHEAD];
	LHead->LHeaders[LISTHEAD] = (ListInfo *) t->ListNode.Next;
     
	if (LHead->LHeaders[LISTHEAD] != NULL) 
	{
		t1 = (ListInfo *) t->ListNode.Next;
		t1->ListNode.Previous = NULL;
	} else
		LHead->LHeaders[LISTTAIL] = NULL;
     
	LHead->NumList--;
	
	return(t);
}


/** 
 *	Remove an element from the the tail of the (vertices|polygons) list.
 *
 *	@param	LHead		the head of the list.
 *	@return	the last element in the list.
 */
ViaStrip::ListInfo * ViaStrip::RemTail(ListHead * LHead)
{
	ListInfo *t, *t1;

	if ( LHead == NULL || EMPTYLIST(LHead) )
		return(NULL);

	t = LHead->LHeaders[LISTTAIL];
	LHead->LHeaders[LISTTAIL] = (ListInfo *) t->ListNode.Previous;
	if (LHead->LHeaders[LISTTAIL] != NULL) 
	{
		t1 = (ListInfo *) t->ListNode.Previous;
		t1->ListNode.Next = NULL;
	} else
		LHead->LHeaders[LISTHEAD] = NULL;
     
	LHead->NumList--;
	return (t);
}


/**
 *	Peek the list.
 *
 *	@param	LHead	the head of the list.
 *	@param	wch		which node to peek in the list.
 *	@param	index	the index number of the node.
 *	@return	the node
 */
ViaStrip::ListInfo * ViaStrip::PeekList(ListHead * LHead, int4 wch, int4 index)
{
	ListInfo *t;

	if (LHead == NULL)
		return(NULL);
     
	if ((t = LHead->LHeaders[wch]) == NULL)
		return(NULL);
     
	for (; t != NULL && index>0; index--)
		t = (wch == LISTHEAD) ? (ListInfo *) t->ListNode.Next : (ListInfo *) t->ListNode.Previous;

	return(t);
}


/**
 *	Remove an element from the list
 *
 *	@param LHead	in which list the element will be removed.
 *	@param Linfo	which element will be removed.
 *	@return	the removed node if the list is not empty
 */
ViaStrip::ListInfo * ViaStrip::RemoveList(ListHead * LHead, ListInfo * LInfo)
{
	ListInfo *t, *t1;

  t = LInfo;
  if (LHead == NULL)
		return (NULL);
  if (LHead->LHeaders[LISTHEAD] == t)
		t = (ListInfo *) RemHead(LHead);
  else if (LHead->LHeaders[LISTTAIL] == t)
		t = (ListInfo *) RemTail(LHead);
  else 
	{
		t1 = (ListInfo *) t->ListNode.Previous;
		t1->ListNode.Next = t->ListNode.Next;
		t1 = (ListInfo *) t->ListNode.Next;
		t1->ListNode.Previous = t->ListNode.Previous;
		LHead->NumList--;
	}
	return(t);
}


/**
 * SearchList:
 *       Try to find a specific node in the queue whose key matches with
 *  searching key. Return the pointer to that node if found, return NULL
 *  otherwise.
 *
 *  @param lpHashTbl		a far pointer to the hash table.
 *	@param lpKey				a far poniter to searching key.
 *  @param CompareCallBack	comparision function.
 *  @return a far pointer to the node to be found.
 */
ViaStrip::ListInfo * ViaStrip::SearchList(ListHead * lpListHead, PVOID lpSKey,
						int4 (* CompareCallBack)(PVOID, PVOID))
{
	ListInfo *lpListInfo;
    
	lpListInfo = PeekList( lpListHead, LISTHEAD, 0);
	while (lpListInfo != NULL) 
	{
		if (CompareCallBack(lpListInfo, lpSKey))
			break;
		lpListInfo = (ListInfo *)GetNextNode(lpListInfo);
	}
	return( lpListInfo );
}


// $Log: ViaQueue.C,v $
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
//
// Revision 3.0  2001/10/14 23:01:50  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:08   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:40   ingmar
// Initial revision.
// Revision 1.9  2001/07/03 14:59:48  soeren
// removed stdafx.c and .h
//
// Revision 1.8  2001/04/09 17:31:26  dmitry
// updated datatypes
//
// Revision 1.7  2001/03/21 16:36:39  binli
// reflected the movement of classes.
//
// Revision 1.6  2001/03/21 15:24:11  binli
// replaced all structs with classes.
//
// Revision 1.5  2001/02/28 21:01:59  binli
// 1> BOOL->bool
// 2> Error Info
//
// Revision 1.4  2000/11/06 23:57:49  binli
// copied the old version (from mixkit) back to replace 'polygon generation'  in lastest version.
// so as to make the triangle generation modular works.
//
// Revision 1.3  2000/10/21 21:09:04  ingmar
// BOOL -> bool and other code cleanup
//
// Revision 1.2  2000/09/20 15:29:47  binli
// Analysized the 'Strip' module to fix the memory leaking problem.
//
// Revision 1.1.1.1  2000/08/11 22:15:54  ingmar
// unit tests complete
//
// 
// 6     9/07/00 12:11p Binli
// fitted to viatronix standard
// 
// 5     8/29/00 1:30p Antonio
// updated footer
// 
// *****************  Version 4  *****************
// User: Binli           Date:  8/23/00  Time: 10:34a
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments
// 
// *****************  Version 3  *****************
// User: Antonio         Date:  8/21/00  Time:  6:12p
// Checked in $/v2k/src/Triangles
// Comment:
//   added header and footer
// 
// *****************  Version 2  *****************
// User: Sarang          Date:  8/20/00  Time:  6:49p
// Checked in $/v2k/src/Triangles
// Comment:
//   The whole pipeline now works under linux+gcc. Removed tons of bugs
// and syntax errors found by gcc.
// 
// **********************
// Label: last_void_*_volume_version
// User: Jay             Date:  8/11/00  Time:  1:19a
// Labeled 'last_void_*_volume_version'
// Label comment:
//   last void * volume version
// about to replace with template volume
// 
// *****************  Version 1  *****************
// User: Binli           Date:  7/31/00  Time:  5:28p
// Created ViaQueue.C
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaQueue.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
// $Id: ViaQueue.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
