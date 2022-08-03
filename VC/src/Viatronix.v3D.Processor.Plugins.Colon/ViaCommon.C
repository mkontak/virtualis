// $Id: ViaCommon.C,v 1.3 2006/06/02 19:01:16 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Created: July 31, 2000 
// Author: Freeware
//		     rewriten by Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.


#include "stdafx.h"
#include "ViaStrip.h"

#define FILE_REVISION "$Revision: 1.3 $"

using namespace std; 
/**
 * Find the bucket that the face_id is currently in.
 *
 * @param face_id	face id
 * @return	bucket number that this face is in.
 */
int4 ViaStrip::Old_Adj(int4 face_id)
{
	// Find the bucket that the face_id is currently in,
	// because maybe we will be deleting it. 
	FFaces * temp = NULL;
	ListHead * pListHead;
	int4 y;
  
	pListHead = m_ppPolFaces[face_id];
	temp = (FFaces *) PeekList( pListHead, LISTHEAD, 0);
  if (temp == NULL) 
  {
    throw ex::AbortDataProcessingException(LogRec("ViaStrip", "Old_Adj", "The face was already deleted, there is an error." ));
  }
  
	if (Done(face_id,&y) == NULL) 
	{
		throw ex::AbortDataProcessingException(LogRec("ViaStrip", "Old_Adj", "There is an error in finding the face." ));
	}
	return y;
}

// determine how many polygons share this edge.
int4 ViaStrip::Number_Adj(int4 id1, int4 id2, int4 curr_id)
{
	// Given edge whose endpoints are specified by id1 and id2,
	// determine how many polygons share this edge and return that
	// number minus one (since we do not want to include the polygon
	// that the caller has already).
  
	int4 size,y,count=0;
	FEdges * temp = NULL;
	FFaces * temp2 = NULL;
	ListHead * pListHead;
	bool there= false;
  
	// Always want smaller id first
	SwitchLower(&id1,&id2);
  
	pListHead = m_ppPolEdges[id1];
	temp = (FEdges *) PeekList(pListHead,LISTHEAD,count);
	if (temp == NULL)
		// new edge that was created might not be here 
		return 0;
	while (temp->edge[0] != id2) 
	{
		count++;
		temp = (FEdges *)GetNextNode(temp); 
		if (temp == NULL)
			// This edge was not there in the original, which
			// mean that we created it in the partial triangulation.
			// So it is adjacent to nothing.
			return 0;
	}
	// Was not adjacent to anything else except itself 
	if (temp->edge[2] == -1)
		return 0;
	else 
	{
		// It was adjacent to another polygon, but maybe we did this
		// polygon already, and it was done partially so that this edge
		// could have been done
		if (curr_id != temp->edge[1]) {
			// Did we use this polygon already?and it was deleted completely from the structure
			pListHead = m_ppPolFaces[temp->edge[1]];
			temp2 = (FFaces *) PeekList( pListHead, LISTHEAD, 0);
			if (Done(temp->edge[1],&size) == NULL)
			return 0;
		} 
		else 
		{
			pListHead = m_ppPolFaces[temp->edge[2]];
			temp2 = (FFaces *) PeekList( pListHead, LISTHEAD, 0);
			if (Done(temp->edge[2],&size)== NULL)
				return 0;
		}
      
		// Now we have to check whether it was partially done, before
		// we can say definitely if it is adjacent.
		// Check each edge of the face and tally the number of adjacent
		// polygons to this face. 
	      		
		if ( temp2 != NULL ) 
		{
			// Size of the polygon
			size = temp2->nPolSize;
			for (y = 0; y< size; y++) 
			{
				// If we are doing partial triangulation, we must check
				// to see whether the edge is still there in the polygon,
				// since we might have done a portion of the polygon
				// and saved the rest for later.
	      
				if (y != (size-1))
				{
					if(((id1 == *(temp2->pPolygon+y)) && (id2 ==*(temp2->pPolygon+y+1))) ||
						((id2 == *(temp2->pPolygon+y)) && (id1 ==*(temp2->pPolygon+y+1))))
						// edge is still there we are ok 
						there = true;
				} 
				else 
				{
					if(((id1 == *(temp2->pPolygon)) && (id2 == *(temp2->pPolygon+size-1))) ||
						((id2 == *(temp2->pPolygon)) && (id1 ==*(temp2->pPolygon+size-1))))
						// edge is still there we are ok 
						there = true;
				}
			}
		}
		if (there ) return 1;
		return 0;
	}
}

/**
 *	Used for the lookahead to break ties.
 */
int4 ViaStrip::Min_Adj(int4 id)
{
	// Used for the lookahead to break ties. It will
	// return the minimum adjacency found at this face.
	int4 y,numverts,t,x=60;
	FFaces * temp=NULL;
	ListHead * pListHead;
  
	// If polygon was used then we can't use this face
	if (Done(id,&y) == NULL)
		return 60;
  
	// It was not used already 
	pListHead = m_ppPolFaces[id];
	temp = (FFaces *) PeekList( pListHead, LISTHEAD, 0 );
	if ( temp != NULL ) 
	{
		numverts = temp->nPolSize;
		for (y = 0; y< numverts; y++) 
		{
			if (y != (numverts-1))
				t = Number_Adj(*(temp->pPolygon+y),*(temp->pPolygon+y+1),id);
			else
				t = Number_Adj(*(temp->pPolygon),*(temp->pPolygon+(numverts-1)),id);
			if (t < x)
				x = t;
		}
	}
	if (x == -1) 
	{
		throw ex::AbortDataProcessingException(LogRec("ViaStrip", "Min_Adj", "Error in the look." ));
	}
	return x;
}

/** 
 *	pick one of the edges with the least number of adjacencies to be the input edge.
 */
void ViaStrip::EdgeLeast(int4 *index,int4 *new1,int4 *new2,int4 face_id,int4 size) 
{
	// We had a polygon without an input edge and now we re going to pick one
    // of the edges with the least number of adjacencies to be the input edge
	register int4 x,value,smallest=60;
  
	for (x = 0; x<size; x++) 
	{
		if (x != (size -1) )
			value = Number_Adj(*(index+x),*(index+x+1),face_id);
		else 
			value = Number_Adj(*(index),*(index+size-1),face_id);
		if (value < smallest) 
		{
			smallest = value;
			if (x != (size -1)) 
			{
				*new1 = *(index+x);
				*new2 = *(index+x+1);
			} 
			else 
			{
				*new1 = *(index);
				*new2 = *(index+size-1);
			}
		}
	}
	if ((smallest == 60) || (smallest < 0)) {
		throw ex::AbortDataProcessingException(LogRec("ViaStrip", "EdgeLeast", "There is an error in getting the least edge." ));
	}
}

/** 
 *	Check to see the adjacencies by going into a polygon that has greater than 4 sides.
 */
void ViaStrip::CheckInPolygon(int4 face_id, int4 *min, int4 size)
{
	 // Check to see the adjacencies by going into a polygon that has greater than 4 sides.
	ListHead * pListHead;
	FFaces * temp;
	int4 y,id1,id2,id3,x=0,z=0;
	int4 saved[2];
	int4 big_saved[60];
  
	pListHead = m_ppPolFaces[face_id];
	temp = (FFaces *) PeekList( pListHead, LISTHEAD, 0 );
  
	// Get the input edge that we came in on
	LastEdge(&id1,&id2,&id3,0);
  
	// Find the number of adjacencies to the edges that are adjacent to the input edge. 
	for (y=0; y< size; y++)
	{
		if (y != (size-1))
		{
			if (((*(temp->pPolygon+y) == id2) && (*(temp->pPolygon+y+1) != id3))
				|| ((*(temp->pPolygon+y) == id3) && (*(temp->pPolygon+y+1) != id2)))
			{
				saved[x++] = Number_Adj(*(temp->pPolygon+y), *(temp->pPolygon+y+1),face_id);
				big_saved[z++] = saved[x-1];
			}
			else
				big_saved[z++] = Number_Adj(*(temp->pPolygon+y),*(temp->pPolygon+y+1),face_id);
		} 
		else 
		{
			if (((*(temp->pPolygon) == id2) && (*(temp->pPolygon+size-1) != id3)) ||
				((*(temp->pPolygon) == id3) && (*(temp->pPolygon+size-1) != id2))) 
			{
				saved[x++] = Number_Adj(*(temp->pPolygon),*(temp->pPolygon+size-1),face_id);
				big_saved[z++] = saved[x-1];
			}
			else
				big_saved[z++] = Number_Adj(*(temp->pPolygon),*(temp->pPolygon+size-1),face_id);
		}
	}
	// There was an input edge
	if (x == 2) 
	{
		if (saved[0] < saved[1])
			// Count the polygon that we will be cutting as another adjacency
			*min = saved[0] + 1;
		else
			*min = saved[1] + 1;
	} 
	else
	{ // There was not an input edge
		if (z != size) 
    {
      std::string ErrorInfo("ViaStrip::There is an error with the z ");
      ErrorInfo += size;
      ErrorInfo += z;
      throw ex::AbortDataProcessingException(LogRec("ViaStrip", "CheckInPolygon" , "There is an error with the z."));
    }
		*min = 60;
		for (x = 0; x < size; x++) 
		{
			if (*min > big_saved[x])
				*min = big_saved[x];
		}
	}
}

/**
 * change the face, that was face_id, to a triangle
 */
void ViaStrip::New_Face(int4 face_id, int4 v1, int4 v2, int4 v3)
{
	// We want to change the face that was face_id, we will change it
	// to a triangle, since the rest of the polygon was already outputtted
	ListHead * pListHead;
	FFaces * temp = NULL;
  
	pListHead = m_ppPolFaces[face_id];
	temp = (FFaces *) PeekList( pListHead, LISTHEAD, 0);
	// Check each edge of the face and tally the number of adjacent polygons to this face. 
  	      		
	if ( temp != NULL ) 
	{
		// Size of the polygon
		if (temp->nPolSize != 4) 
		{
			throw ex::AbortDataProcessingException(LogRec("ViaStrip", "New_Face", "There is a miscalculation in the partial." ));
		}
		temp->nPolSize = 3;
		*(temp->pPolygon) = v1;
		*(temp->pPolygon+1) = v2;
		*(temp->pPolygon+2) = v3;
	}
}

/** 
 *	want to change the face that was face_id, change it to a triangle, 
 *	since the rest of the polygon was already outputtted
 *
 *	@param face_id	face id
 */
void ViaStrip::NewSizeFace(int4 face_id)
{   
  stringstream tt;
	// We want to change the face that was face_id, we will change it
	// to a triangle, since the rest of the polygon was already outputtted
	ListHead * pListHead;
	FFaces * temp = NULL;
  
	pListHead = m_ppPolFaces[face_id];
	temp = (FFaces *) PeekList( pListHead, LISTHEAD, 0 );
	// Check each edge of the face and tally the number of adjacent polygons to this face. 

	if ( temp != NULL )
		(temp->nPolSize)--;
	else
    tt << "ViaStrip::There is an error in updating the size." << std::endl;
  LogWrn(tt.str(),"ViaStrip", "NewSizeFace");
  tt.str("");

}

/**
 *	Check to see what the adjacencies are for the polygons that are inside the quad
 */
void  ViaStrip::CheckInQuad(int4 face_id,int4 *min)
{
	// Check to see what the adjacencies are for the polygons that
    // are inside the quad, ie the 2 triangles that we can form.
	ListHead * pListHead;
	int4 y,id1,id2,id3,x=0;
	int4 saved[4];
	FFaces * temp;
	register int4 size = 4;

	pListHead = m_ppPolFaces[face_id];
	temp = (FFaces *) PeekList( pListHead, LISTHEAD, 0 );

	// Get the input edge that we came in on
	LastEdge(&id1,&id2,&id3,0);

	// Now find the adjacencies for the inside triangles
	for (y = 0; y< size; y++) 
	{
		// Will not do this if the edge is the input edge
		if (y != (size-1)) 
		{
			if ((((*(temp->pPolygon+y) == id2) && (*(temp->pPolygon+y+1) == id3))) ||
				(((*(temp->pPolygon+y) == id3) && (*(temp->pPolygon+y+1) == id2))))
				saved[x++] = -1;
			else 
			{
				if (x == 4) 
        {
          throw ex::AbortDataProcessingException(LogRec("ViaStrip", "CheckInQuad", "There is an error in the check in quad."  ));
        }
				// Save the number of Adjacent Polygons to this edge
				saved[x++] = Number_Adj(*(temp->pPolygon+y),*(temp->pPolygon+y+1),face_id);
			}
		} 
		else if ((((*(temp->pPolygon) == id2) && (*(temp->pPolygon+size-1) == id3))) ||
				(((*(temp->pPolygon) == id3) && (*(temp->pPolygon+size-1) == id2))) )
				saved[x++] = -1;
		else 
		{
			if (x == 4) 
      {
        throw ex::AbortDataProcessingException(LogRec( "ViaStrip", "CheckInQuad", "There is an error in the check in quad." ));
      }
			// Save the number of Adjacent Polygons to this edge
			saved[x++] = Number_Adj(*(temp->pPolygon), *(temp->pPolygon+size-1),face_id);
		}
	}
	if (x != 4) 
  {
    std::string ErrorInfo("ViaStrip::Did not enter all the values ");
    ErrorInfo += x;
    throw ex::AbortDataProcessingException(LogRec( "ViaStrip", "CheckInQuad" , "Did not enter all the values."));
  }
  
	*min = 10;
	for (x=0; x<4; x++) 
	{
		if (x!= 3) 
		{
			if ((saved[x] != -1) && (saved[x+1] != -1) && ((saved[x] + saved[x+1]) < *min))
				*min = saved[x] + saved[x+1];
		} 
		else 
		{
			if ((saved[0] != -1) && (saved[x] != -1) &&	((saved[x] + saved[0]) < *min))
				*min = saved[0] + saved[x];
		}
	}
}

/**
 *	Return the vertex adjacent to either input1 or input2 that
 *	s adjacent to the least number of polygons on the edge that
 *	is shared with either input1 or input2.
 */
int4 ViaStrip::GetOutputEdge(int4 face_id, int4 size, int4 *index,int4 id2,int4 id3)
{
	register int4 x=0,y;
	int4 saved[2];
	int4 edges[2][1];
  
	for (y = 0; y < size; y++) 
	{
		if (y != (size-1)) 
		{
			if (((*(index+y) == id2) && (*(index+y+1) != id3))
				|| ((*(index+y) == id3) && (*(index+y+1) != id2))) 
			{
				saved[x++] = Number_Adj(*(index+y),*(index+y+1),face_id);
				edges[x-1][0] = *(index+y+1);
			} 
			else if (y != 0) 
			{
				if (( (*(index+y) == id2) && (*(index+y-1) != id3) ) ||
					( (*(index+y) == id3) && (*(index+y-1) != id2)) ) 
				{
					saved[x++] = Number_Adj(*(index+y),*(index+y-1),face_id);
					edges[x-1][0] = *(index+y-1);
				}
			} 
			else if (y == 0) 
			{
				if (((*(index) == id2) && (*(index+size-1) != id3) ) ||
					((*(index) == id3) && (*(index+size-1) != id2)) ) 
				{
					saved[x++] = Number_Adj(*(index),*(index+size-1),face_id);
					edges[x-1][0] = *(index+size-1);
				}
			}
		} 
		else 
		{
			if (((*(index+size-1) == id2) && (*(index) != id3))
				|| ((*(index+size-1) == id3) && (*(index) != id2))) 
			{
				saved[x++] = Number_Adj(*(index),*(index+size-1),face_id);
				edges[x-1][0] = *(index);
			}
			if (((*(index+size-1) == id2) && (*(index+y-1) != id3) ) ||
				((*(index+size-1) == id3) && (*(index+y-1) != id2))) 
			{
				saved[x++] = Number_Adj(*(index+size-1),*(index+y-1),face_id);
				edges[x-1][0] = *(index+y-1);
			}
		}
	}
	if ((x != 2)) 
	{
		std::string ErrorInfo("ViaStrip::There is an error in getting the input edge ");
		ErrorInfo += x;
		throw ex::AbortDataProcessingException(LogRec( "ViaStrip", "GetOutputEdge", "There is an error in getting the input edge." ));		
	}
	if (saved[0] < saved[1])
		return edges[0][0];
	else
		return edges[1][0];
}

/** 
 *	pick one edge as the input one from the polygon without an input edge 
 */
void ViaStrip::GetInputEdge(int4 *index,int4 id1,int4 id2,int4 id3,
		    int4 *new1,int4 *new2,int4 size,int4 face_id)
{
	// We had a polygon without an input edge and now we are going to pick one
	// as the input edge. The last triangle was id1,id2,id3, we will try to
	// get an edge to have something in common with one of those vertices, 
	// otherwise we will pick the edge with the least number of adjacencies.

	int4 saved[3] = {-1, -1, -1};
  
	// Go through the edges to see if there is one in common with one
	// of the vertices of the last triangle that we had, preferably id2 or
	// id3 since those are the last 2 things in the stack of size 2.
	for (int4 x=0; x< size; x++) 
	{
		if (*(index+x) == id1) 
		{
			if (x != (size-1))
				saved[0] = *(index+x+1);
			else
				saved[0] = *(index);
		}
		if (*(index+x) == id2) 
		{
			if (x != (size-1))
				saved[1] = *(index+x+1);
			else
				saved[1] = *(index);
		}
		if (*(index+x) == id3) 
		{
			if (x != (size -1))
				saved[2] = *(index+x+1);
			else
				saved[2] = *(index);
		}
	}
	// Now see what we saved
	if (saved[2] != -1) 
	{
		*new1 = id3;
		*new2 = saved[2];
		return;
	} 
	else if (saved[1] != -1) 
	{
		*new1 = id2;
		*new2 = saved[1];
		return;
	} else if (saved[0] != -1) 
	{
		*new1 = id1;
		*new2 = saved[0];
		return;
	}
	// We did not find anything so get the edge with the least number of adjacencies
	EdgeLeast(index,new1,new2,face_id,size);
}

/**	
 *	Find the face that is adjacent to the edge and is not the current face.
 */
int4 ViaStrip::FindFace(int4 current_face, int4 id1, int4 id2, int4 *bucket)
{
	register int4 size,y,count=0;    
	FEdges * temp = NULL;
	FFaces * temp2 = NULL;
	ListHead * pListHead;
	int4 next_face;
	bool there = false;

	// Always want smaller id first 
	SwitchLower(&id1,&id2);
  
	pListHead = m_ppPolEdges[id1];
	temp = (FEdges *) PeekList(pListHead,LISTHEAD,count);
	// The input edge was a new edge 
	if (temp == NULL)
		return -1;
  
	while (temp->edge[0] != id2) 
	{
		count++;
		temp = (FEdges *)GetNextNode(temp);                                   
      
		// The input edge was a new edge
		if (temp == NULL)
			return -1;
	}
	//	Was not adjacent to anything else except itself 
	if (temp->edge[2] == -1)
		return -1;
	else 
	{
		if (temp->edge[2] == current_face)
			next_face =  temp->edge[1];
		else 
			next_face = temp->edge[2];
	}
	// We have the other face adjacent to this edge, it is next_face. 

	pListHead = m_ppPolFaces[next_face];
	temp2 = (FFaces *) PeekList( pListHead, LISTHEAD, 0 );
  
	// See if the face was already deleted, and where it is if it was not
	if (Done(next_face,bucket) == NULL)
		return -1;
  
	// Make sure the edge is still in this polygon, and that it is not done
	// Size of the polygon
	size = temp2->nPolSize;
	for (y = 0; y< size; y++) 
	{
		// Make sure that the edge is still in the
		// polygon and was not deleted, because if the edge was
		// deleted, then we used it already.
		if (y != (size-1)) 
		{
			if( ((id1 == *(temp2->pPolygon+y)) && (id2 ==*(temp2->pPolygon+y+1)))
				|| ((id2 == *(temp2->pPolygon+y)) && (id1 ==*(temp2->pPolygon+y+1))))
				//	edge is still there we are ok
				there = true;
		} 
		else 
		{		
			if(((id1 == *(temp2->pPolygon)) && (id2 ==*(temp2->pPolygon+size-1))) ||
				((id2 == *(temp2->pPolygon)) && (id1 ==*(temp2->pPolygon+size-1))))
				// edge is still there we are ok
				there = true;
		}
	}
  
	if (!there)
		// Edge already used and deleted from the polygon
		return -1;
	else
		return next_face;
}

/**	
 *	See if the endpoints of the edge specified by id1 and id2
 *	are adjacent to the face with face_id 
 */
bool ViaStrip::LookUp(int4 id1,int4 id2,int4 face_id)
{
	register int4 count = 0;
	FEdges * temp  = NULL;
	ListHead * pListHead;
  
	// Always want smaller id first
	SwitchLower(&id1,&id2);
  
	pListHead = m_ppPolEdges[id1];
	temp = (FEdges *) PeekList(pListHead,LISTHEAD,count);
	if (temp == NULL)	// Was a new edge that we created
		return 0;
  
	while (temp->edge[0] != id2) 
	{
		count++;
		temp = (FEdges *)GetNextNode(temp);                                  
		if (temp == NULL)	// Was a new edge that we created
			return 0;
	}
	// Was not adjacent to anything else except itself 
	if ((temp->edge[2] == face_id) || (temp->edge[1] == face_id)) 
	{
		// Edge was adjacent to face, make sure that edge is still there
		if (Exist(face_id,id1,id2))
			return 1;
		else
			return 0;
	} else
		return 0;
}

/** 
 *	Just save the triangle for later
 */
void ViaStrip::AddIdStrips(int4 id, bool tail) 
{
	Strips * pfNode = new Strips;

	if (!pfNode) 
    throw ex::OutOfMemoryException(LogRec("ViaStrip", "AddIdStrips", "Out of memory." ));

  pfNode->face_id = id;
  // if (where == 1)
  if ( tail )
    AddTail(m_pStrips,(ListInfo *) pfNode);
		// We are backtracking in the strip
	else
		AddHead(m_pStrips,(ListInfo *) pfNode);
}

/**
 *	Given edge whose endpoints are specified by id1 and id2,
 *	determine how many polygons share this edge and return that
 *	number minus one (since we do not want to include the polygon
 *	that the caller has already).
 */
int4	ViaStrip::Num_Adj(int4 id1, int4 id2)
{
	FEdges * temp = NULL;
	ListHead * pListHead;

  // register is specific to VC++
  #ifdef _WIN32
	register int4 count=-1;
  #else
  int4 count=-1;
  #endif // _WIN32
  
	// Always want smaller id first
	SwitchLower(&id1,&id2);
  
	pListHead = m_ppPolEdges[id1];
	temp = (FEdges *) PeekList(pListHead,LISTHEAD,count);
	if (temp == NULL) 
	{
		throw ex::AbortDataProcessingException(LogRec("ViaStrip", "Num_Adj", "There is an error in the creation of the table." ));
	}
	while (temp->edge[0] != id2) 
	{
		count++;
		temp = (FEdges *)GetNextNode(temp);                                    
      
    if (temp == NULL) 
    {
      throw ex::AbortDataProcessingException(LogRec("ViaStrip", "Num_Adj", "There is an error in the creation of the table."  ));
    }
	}
	// Was not adjacent to anything else except itself
	if (temp->edge[2] == -1) return 0;
		
	return 1;
}

/**	
 *	This routine will add the face to the proper bucket,
 *	depending on how many faces are adjacent to it (what the value bucket should be).
 */
void ViaStrip::AddSgiAdj(int4 bucket,int4 face_id) 
{
	ADJACENCIES * pfNode = new ADJACENCIES;

	if (! pfNode) 
		throw ex::OutOfMemoryException(LogRec("ViaStrip", "AddSgiAdj", "Out of memory." ));

  pfNode->face_id = face_id;
	AddHead(m_pArray[bucket],(ListInfo *) pfNode);
	m_pFaceArray[face_id].bucket = bucket;
	m_pFaceArray[face_id].pfNode = pfNode;
	m_pFaceArray[face_id].head = m_pArray[bucket];
}

/** 
 *	find adjacent faces
 */
void ViaStrip::FindAdjacencies(int4 num_faces)
{
	register int4 x,y;
	register int4 numverts;
	FFaces * temp = NULL;
	ListHead * pListHead;
  
	// Fill in the adjacencies data structure for all the faces
	for (x=0; x<num_faces; x++) 
	{
		pListHead = m_ppPolFaces[x];
		temp = (FFaces *)PeekList(pListHead, LISTHEAD, 0);
		if (temp != NULL) 
		{
			numverts = temp->nPolSize;
			if (numverts != 1) 
			{
				for (y = 0; y< numverts; y++) 
				{
					if (y != (numverts-1))
						AddAdjEdge(*(temp->pPolygon+y), *(temp->pPolygon+y+1),x,y);
					else 
						AddAdjEdge(*(temp->pPolygon), *(temp->pPolygon+(numverts-1)),x,numverts-1);
				}
			}
			temp = NULL;
		}
	}
}

// $Log: ViaCommon.C,v $
// Revision 1.3  2006/06/02 19:01:16  geconomos
// updated to vs 2005
//
// Revision 1.2  2005/07/01 12:46:05  vxconfig
// Added new Logger.
//
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
//
// Revision 3.2  2002/08/05 20:42:32  dongqing
// rename the Logger to vxLogger due to the re-name of Logger Class
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:01:50  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:04   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:36   ingmar
// Initial revision.
// Revision 1.27  2001/08/13 20:29:19  sachin
// changed exceptions ,warnings,debug and information
// according to the new logger class
//
// Revision 1.26  2001/08/07 17:04:40  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.25  2001/07/25 16:08:39  lihong
// Defect Id: 599
// Added throw ex:: after the function name
//
// Revision 1.24  2001/07/24 23:06:51  lihong
// Defect ID: 599
// new format of exceptions
//
// Revision 1.23  2001/07/23 20:03:11  lihong
// Defect ID: 599
// Changed previous exception with detailed LogRecord
//
// Revision 1.22  2001/07/23 16:56:27  lihong
// Defect ID: 599
// replaced file version with "$Revision: 1.3 $"
//
// Revision 1.21  2001/07/20 23:07:10  lihong
// Defect ID: 599
// changed abort() to throw ex
//
// Revision 1.20  2001/05/21 00:22:53  dave
// uses & implement. of Error::DebugMsg, Warn, Abort, etc, gone
//
// Revision 1.19  2001/04/13 14:33:42  dave
// changed namespaces for exceptions
//
// Revision 1.18  2001/04/12 15:50:49  dave
// added namespace vx for exceptions
//
// Revision 1.17  2001/04/09 22:27:33  dave
// changing from Abort to exceptions
//
// Revision 1.16  2001/04/09 17:31:26  dmitry
// updated datatypes
//
// Revision 1.15  2001/03/27 19:49:51  binli
// code walkthrough: name convension
//
// Revision 1.14  2001/03/27 16:55:53  binli
// reflected new code standard.
//
// Revision 1.13  2001/03/23 20:41:19  binli
// code walkthrough
//
// Revision 1.12  2001/03/22 21:15:21  binli
// renamed some variables according to new code standard.
//
// Revision 1.11  2001/03/21 17:15:55  binli
// reflected changes made below:
//   1> replaced triple variable with standard Triple.
//   2> changed struct Strips to class Strips.
//
// Revision 1.10  2001/03/21 15:24:11  binli
// replaced all structs with classes.
//
// Revision 1.9  2001/03/07 19:57:21  binli
// malloc/free replaced by new/delete
//
// Revision 1.8  2001/03/01 17:49:52  binli
// changed 'int' to 'bool' in Output_Tri() and Output_TriEx().
//
// Revision 1.7  2001/02/28 21:01:59  binli
// 1> BOOL->bool
// 2> Error Info
//
// Revision 1.6  2001/02/28 18:19:38  binli
// tried to solve 'BOOL' problem in ViaStrip: changed parameter from BOOL to int in Output_Tri().
//
// Revision 1.5  2000/11/06 23:57:41  binli
// copied the old version (from mixkit) back to replace 'polygon generation'  in lastest version.
// so as to make the triangle generation modular works.
//
// Revision 1.4  2000/11/02 04:54:47  antonio
// changed int where -> bool end for functions in ViaStrip.h
//
// Revision 1.3  2000/10/21 21:05:01  ingmar
// BOOL -> bool and other code cleanup
//
// Revision 1.2  2000/09/20 15:29:48  binli
// Analysized the 'Strip' module to fix the memory leaking problem.
//
// Revision 1.1.1.1  2000/08/11 22:15:54  ingmar
// unit tests complete
//
// 
// 6     9/07/00 12:11p Binli
// fitted to viatronix standard
// 
// 5     8/29/00 1:47p Antonio
// updated footer and header
// 
// *****************  Version 4  *****************
// User: Binli           Date:  8/23/00  Time: 12:57p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments
// 
// *****************  Version 3  *****************
// User: Antonio         Date:  8/21/00  Time:  5:58p
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
// Created ViaCommon.C
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaCommon.C,v 1.3 2006/06/02 19:01:16 geconomos Exp $
// $Id: ViaCommon.C,v 1.3 2006/06/02 19:01:16 geconomos Exp $
