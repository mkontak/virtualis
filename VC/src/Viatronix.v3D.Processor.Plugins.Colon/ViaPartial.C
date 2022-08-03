// $Id: ViaPartial.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// File: ViaPartial.C
// Description:  implementation (11/17) of Class ViaStrip
// Created: July 31, 2000
// Author:  Freeware
//                          Rewriten by Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.


#include "stdafx.h"
#include "ViaStrip.h"

#define FILE_REVISION "$Revision: 1.2 $"

/**
 *	nonblindly triangulate a quad
 */
void ViaStrip::P_Triangulate_Quad(int4 out_edge1, int4 out_edge2, int4 in_edge1, int4 in_edge2, 
                                  int4 size, int4 *index, int4 reversed, int4 face_id, 
                                  ListHead *pListHead, ADJACENCIES * temp, 
                                  int4 where, std::ofstream & band)
{
	int4 vertex4,vertex5,dummy=60;

	// This routine will nonblindly triangulate a quad, meaning that 
	// there is a definite input and a definite output edge that we 
	// must adhere to. Reversed will tell the orientation of the input 
	// edge. (Reversed is -1 is we do not have an input edge, in other 
	// words we are at the beginning of a strip.) 
	// Out_edge* is the output edge, and in_edge* is the input edge. 
	// Index are the edges of the polygon
	// and size is the size of the polygon. Begin is whether we are
	// at the start of a new strip.
	// Note that we will not necessarily triangulate the whole quad;
	// maybe we will do half and leave the other half (a triangle) for later.

	// If we do not have an input edge, then we can make our input
	// edge whatever we like, therefore it will be easier to come
	// out on the output edge.	In this case the whole quad is done.
  if (reversed == -1) 
  {
    vertex4 = AdjacentEx(out_edge1,out_edge2,index,size);
    vertex5 = GetOtherVertex(vertex4,out_edge1,out_edge2,index);

    OutputTriEx(vertex5,vertex4,out_edge1, -1, (where)?true:false, band);
    OutputTriEx(vertex4,out_edge1,out_edge2, -1, (where)?true:false, band);
      
    dummy = UpdateAdjacenciesEx(face_id, &dummy, &dummy,&dummy,&dummy);

    if (m_pFaceArray[temp->face_id].head == pListHead)
      m_pFaceArray[temp->face_id].pfNode = NULL;
    RemoveList(pListHead,(ListInfo *) temp);
    return;
  }
  
	// These are the 5 cases that we can have for the output edge */
	// Are they consecutive so that we form a triangle to peel off, but cannot use the whole quad?
  if (in_edge2 == out_edge1)  
	{
	  // Output the triangle that comes out the correct edge. Save the other half for later.
		vertex4 = GetOtherVertex(in_edge1,in_edge2,out_edge2,index);
		OutputTriEx(in_edge1,in_edge2,out_edge2, -1,(where)?true:false, band);

	  // Now we have a triangle used, and a triangle that is left for later.
		// Now delete the adjacencies by one for all the faces that are adjacent to 
	  // the triangle that we just outputted.
		DeleteAdjEx(in_edge1,in_edge2,&dummy,&dummy, face_id,&dummy,&dummy,&dummy);
		DeleteAdjEx(out_edge2,in_edge2,&dummy,&dummy, face_id,&dummy,&dummy,&dummy);

	  // Put the new face in the proper bucket of adjacencies 
	  // There are 2 edges that need to be checked for the triangle
	  // that was just outputted. For the output edge we definitely
	  // will be decreasing the adjacency, but we must check for the input edge.
		dummy = ChangeFaceEx(face_id,in_edge1,in_edge2,pListHead, false);
		dummy = ChangeFaceEx(face_id,in_edge2,out_edge2,pListHead, true);

	  // Update the face data structure, by deleting the old face and putting in the triangle as the new face 
	  New_Face(face_id,in_edge1,out_edge2,vertex4);
		return;									  
	} 
	else if (in_edge1 == out_edge1) 
	{
	  // We want to output the first triangle (whose output edge is not
	  // the one that we want. We have to find the vertex that we need, 
	  // which is the other vertex which we do not have.
		vertex4 = GetOtherVertex(in_edge1,in_edge2,out_edge2,index);
		OutputTriEx(in_edge2,in_edge1,out_edge2, -1,(where)?true:false, band);
      
	  // Now we have a triangle used, and a triangle that is left for later.
		// Now delete the adjacencies by one for all the faces
	  // that are adjacent to the triangle that we just outputted.
		DeleteAdjEx(in_edge1,in_edge2,&dummy,&dummy,face_id, &dummy,&dummy,&dummy);
		DeleteAdjEx(out_edge2,out_edge1,&dummy,&dummy, face_id,&dummy,&dummy,&dummy);
      
	  // Put the new face in the proper bucket of adjacencies
		dummy = ChangeFaceEx(face_id,in_edge1,in_edge2,pListHead,false);
		dummy = ChangeFaceEx(face_id,in_edge1,out_edge2,pListHead,true);

	  // Update the face data structure, by deleting the old
	  // face and putting in the triangle as the new face 
		New_Face(face_id,in_edge2,out_edge2,vertex4);
		return;
	}
  
	// Consecutive cases again, but with the output edge reversed
	else if (in_edge1 == out_edge2) 
	{
		vertex4 = GetOtherVertex(in_edge1,in_edge2,out_edge1,index);
		OutputTriEx(in_edge2,in_edge1,out_edge1, -1,(where)?true:false, band);

	  // Now we have a triangle used, and a triangle that is left for later.
		// Now delete the adjacencies by one for all the faces
	  // that are adjacent to the triangle that we just outputted.
		DeleteAdjEx(in_edge1,in_edge2,&dummy,&dummy,face_id, &dummy,&dummy,&dummy);
		DeleteAdjEx(out_edge2,out_edge1,&dummy,&dummy, face_id,&dummy,&dummy,&dummy);

	  // Put the new face in the proper bucket of adjacencies
		dummy = ChangeFaceEx(face_id,in_edge1,in_edge2,pListHead,false);
		dummy = ChangeFaceEx(face_id,out_edge1,out_edge2,pListHead,true);
      
	  // Update the face data structure, by deleting the old
	  // face and putting in the triangle as the new face 
		New_Face(face_id,in_edge2,out_edge1,vertex4);
		return;
	} 
	else if (in_edge2 == out_edge2) 
	{
		vertex4 = GetOtherVertex(in_edge1,in_edge2,out_edge1,index);
		OutputTriEx(in_edge1,in_edge2,out_edge1, -1,(where)?true:false, band);

	  // Now we have a triangle used, and a triangle that is left for later.
		// Now delete the adjacencies by one for all the faces
	  // that are adjacent to the triangle that we just outputted.
		DeleteAdjEx(in_edge1,in_edge2,&dummy,&dummy,face_id, &dummy,&dummy,&dummy);
		DeleteAdjEx(out_edge2,out_edge1,&dummy,&dummy, face_id,&dummy,&dummy,&dummy);
      
	  // Put the new face in the proper bucket of adjacencies
		dummy = ChangeFaceEx(face_id,in_edge1,in_edge2,pListHead,false);
		dummy = ChangeFaceEx(face_id,out_edge1,out_edge2,pListHead,true);
      
	  // Update the face data structure, by deleting the old
	  // face and putting in the triangle as the new face 
		New_Face(face_id,in_edge1,out_edge1,vertex4);
		return;
	}
	// The final case is where we want to come out the opposite edge.
	else 
	{
		if( ((!reversed) && 
			(out_edge1 == (AdjacentEx(in_edge1,in_edge2,index,size)))) ||
			((reversed) && 
			(out_edge2 == (AdjacentEx(in_edge2,in_edge1,index,size))))) 
		{
		// We need to know the orientation of the input edge, 
		// so we know which way to put the diagonal.
		// And also the output edge, so that we triangulate correctly. 
		// Does not need partial.
	  OutputTriEx(in_edge1,in_edge2,out_edge2, -1,(where)?true:false, band);
		OutputTriEx(in_edge2,out_edge2,out_edge1, -1,(where)?true:false, band);
		dummy = UpdateAdjacenciesEx(face_id, &dummy, &dummy,&dummy,&dummy);
	  
		if (m_pFaceArray[temp->face_id].head == pListHead)
			m_pFaceArray[temp->face_id].pfNode = NULL;
			RemoveList(pListHead,(ListInfo *) temp);
		} 
		else 
		{
			// Input and output orientation was reversed, so diagonal will be reversed from above.
			OutputTriEx(in_edge1,in_edge2,out_edge1, -1,(where)?true:false, band);
			OutputTriEx(in_edge2,out_edge1,out_edge2, -1,(where)?true:false, band);
			dummy = UpdateAdjacenciesEx(face_id, &dummy, &dummy,&dummy,&dummy);
	  
			if (m_pFaceArray[temp->face_id].head == pListHead)
				m_pFaceArray[temp->face_id].pfNode = NULL;
			RemoveList(pListHead,(ListInfo *) temp);
		}
		return;
	}
}

/**
 *	When there is a polygon greater than 4 sides, partially triangulate it
 */
void ViaStrip::P_TriangulatePolygon(int4 out_edge1, int4 out_edge2, int4 in_edge1, int4 in_edge2,
                                    int4 size, int4 *index, int4 reversed,int4 face_id,int4 *next_id,
				                            ListHead *pListHead, ADJACENCIES * temp2, int4 where,
                                    std::ofstream & band)
{
  int4 next_bucket,vertex4,dummy = 60;
  int4 *temp;

  // Since we are calling this recursively, we have to check whether
  // we are down to the case of the quad.
  if (size == 4) {
		P_Triangulate_Quad(out_edge1,out_edge2,in_edge1,in_edge2,size,
				index,reversed,face_id, pListHead,temp2,where, band);
		return;
  }
  
  // We do not have a specified input edge, and therefore we
  // can make it anything we like, as long as we still come out 
  // the output edge that we want.
  if (reversed  == -1) 
	{
	  // Get the vertex for the last triangle, which is the one coming 
	  // out the output edge, before we do any deletions to the list. 
	  // We will be doing this bottom up.
		vertex4 = AdjacentEx(out_edge1,out_edge2,index,size);
		temp = new int4 [size];
		memcpy(temp,index,sizeof(int4)*size);
		Delete_From_ListEx(out_edge2,index,size);
      
	  // We do not have to partially triangulate,
	  // since we will do the whole thing, so use the whole routine
		Triangulate_PolygonEx(vertex4,out_edge1,in_edge2,
			    vertex4,size-1,index, 
			    reversed,face_id, where, band);
		memcpy(index,temp,sizeof(int4)*size);
      
	  // Lastly do the triangle that comes out the output edge.
		OutputTriEx(vertex4,out_edge1,out_edge2, -1,(where)?true:false, band);

	  // We were able to do the whole polygon, now we
	  // can delete the whole thing from our data structure.
		dummy = UpdateAdjacenciesEx(face_id, &dummy, &dummy,&dummy,&dummy);
      
		if (m_pFaceArray[temp2->face_id].head == pListHead)
			m_pFaceArray[temp2->face_id].pfNode = NULL;
		RemoveList(pListHead,(ListInfo *) temp2);
		return;
	}

  // These are the 5 cases that we can have for the output edge
  // Are they consecutive so that we form a triangle to peel off 
  // that comes out the correct output edge, but we cannot use the whole polygon?
  if (in_edge2 == out_edge1) 
	{
		OutputTriEx(in_edge1,out_edge1,out_edge2, -1,(where)?true:false, band);

	  // Now delete the adjacencies by one for all the faces
	  // that are adjacent to the triangle that we just outputted.
		DeleteAdjEx(in_edge1,in_edge2,&dummy,&dummy,face_id, &dummy,&dummy,&dummy);
		DeleteAdjEx(out_edge2,out_edge1,&dummy,&dummy, face_id,&dummy,&dummy,&dummy);
	  
	  // Put the new face in the proper bucket of adjacencies */ 
		next_bucket = ChangeFaceEx(face_id,in_edge1,in_edge2,pListHead,false);
		next_bucket = ChangeFaceEx(face_id,out_edge1,out_edge2,pListHead,true);
      
	  // Create a new edgelist without the triangle that was just outputted.
		Delete_From_ListEx(in_edge2,index,size);
	  // Update the face data structure, by deleting the old face and 
	  // putting in the polygon minus the triangle as the new face, 
	  // here we will be decrementing the size by one.
		NewSizeFace(face_id);
		return;
	}
  
  // Next case is where it is again consecutive, but the triangle
  // formed by the consecutive edges do not come out of the
  // correct output edge. (the input edge will be reversed in
  // the next triangle)
  else if (in_edge1 == out_edge1) 
	{
	  // Get vertex adjacent to in_edge2, but is not in_edge1
		OutputTriEx(in_edge2,in_edge1,out_edge2, -1,(where)?true:false, band);

	  // Now delete the adjacencies by one for all the faces
	  // that are adjacent to the triangle that we just outputted.
		DeleteAdjEx(in_edge1,in_edge2,&dummy,&dummy,face_id, &dummy,&dummy,&dummy);
		DeleteAdjEx(out_edge2,out_edge1,&dummy,&dummy, face_id,&dummy,&dummy,&dummy);

	  // Put the new face in the proper bucket of adjacencies
		next_bucket = ChangeFaceEx(face_id,in_edge1,in_edge2,pListHead,false);
		next_bucket = ChangeFaceEx(face_id,out_edge1,out_edge2,pListHead,true);

	  // Create a new edgelist without the triangle that was just outputted.
		Delete_From_ListEx(in_edge1,index,size);
	  // Update the face data structure, by deleting the old face and 
	  // putting in the polygon minus the triangle as the new face, 
	  // here we will be decrementing the size by one.
	  NewSizeFace(face_id);
		return;
	} 
	else if (in_edge1 == out_edge2) 
	{
		OutputTriEx(in_edge2,in_edge1,out_edge1, -1,(where)?true:false, band);
      
		// Now delete the adjacencies by one for all the faces
	  // that are adjacent to the triangle that we just outputted.
		DeleteAdjEx(in_edge1,in_edge2,&dummy,&dummy,face_id,&dummy,&dummy,&dummy);
		DeleteAdjEx(out_edge1,out_edge2,&dummy,&dummy,face_id,&dummy,&dummy,&dummy);
      
	  // Put the new face in the proper bucket of adjacencies
		next_bucket = ChangeFaceEx(face_id,in_edge1,in_edge2,pListHead,false);
		next_bucket = ChangeFaceEx(face_id,out_edge1,out_edge2,pListHead,true);

	  // Create a new edgelist without the triangle that was just outputted.
		Delete_From_ListEx(in_edge1,index,size);

	  // Update the face data structure, by deleting the old
	  // face and putting in the polygon minus the triangle 
	  // as the new face, here we will be decrementing the size by one.
		NewSizeFace(face_id);
		return;
	} 
	else if (in_edge2 == out_edge2) 
	{
		OutputTriEx(in_edge1,in_edge2,out_edge1, -1,(where)?true:false, band);

	  // Now delete the adjacencies by one for all the faces
	  // that are adjacent to the triangle that we just outputted.
		DeleteAdjEx(in_edge1,in_edge2,&dummy,&dummy,face_id, &dummy,&dummy,&dummy);
		DeleteAdjEx(out_edge2,out_edge1,&dummy,&dummy, face_id,&dummy,&dummy,&dummy);
      
	  // Put the new face in the proper bucket of adjacencies
		next_bucket = ChangeFaceEx(face_id,in_edge1,in_edge2,pListHead,false);
		next_bucket = ChangeFaceEx(face_id,out_edge1,out_edge2,pListHead,true);

	  // Create a new edgelist without the triangle that was just outputted.
		Delete_From_ListEx(in_edge2,index,size);

	  // Update the face data structure, by deleting the old
	  // face and putting in the polygon minus the triangle 
	  // as the new face, here we will be decrementing the size	by one.
		NewSizeFace(face_id);
		return;
	}

  // Else the edge is not consecutive, and it is sufficiently
  // far away, for us not to make a conclusion at this time.
  // So we can take off a triangle and recursively call this function.
  else 
	{
		if (!reversed) 
		{
			vertex4 = AdjacentEx(in_edge2,in_edge1,index,size);
			OutputTriEx(in_edge1,in_edge2,vertex4, -1,(where)?true:false, band);

			// Now delete the adjacencies by one for all the faces
			// that are adjacent to the triangle that we just outputted.
			DeleteAdjEx(in_edge1,in_edge2,&dummy,&dummy,face_id,&dummy,&dummy,&dummy);
			DeleteAdjEx(in_edge1,vertex4,&dummy,&dummy, face_id,&dummy,&dummy,&dummy);

			// Put the new face in the proper bucket of adjacencies
			next_bucket = ChangeFaceEx(face_id,in_edge1,in_edge2,pListHead,false);
			next_bucket = ChangeFaceEx(face_id,in_edge1,vertex4,pListHead,false);
	  
			// Create a new edgelist without the triangle that was just outputted.
			Delete_From_ListEx(in_edge1,index,size);

			// Update the face data structure, by deleting the old
			// face and putting in the polygon minus the triangle 
			// as the new face, here we will be decrementing the size by one.
			NewSizeFace(face_id);

			// Save the info for the new bucket, we will need it on
			// the next pass for the variables, pListHead and temp 
			pListHead = m_pArray[next_bucket];
			temp2 = m_pFaceArray[face_id].pfNode;
			if (temp2 == NULL) {
				int1 ErrorInfo[100];
				sprintf(ErrorInfo, "ViaStrip::Create strips: there is an error finding the next polygon10\n", next_bucket,face_id);
				throw ex::AbortDataProcessingException(LogRec("ViaStrip", "P_TriangulatePolygon", "Create strips: There is an error finding the next polygon10."));
			}
			P_TriangulatePolygon(out_edge1,out_edge2,in_edge2,
					vertex4,size-1,index, !reversed,
					face_id,next_id,pListHead,temp2,(where)?true:false, band);
		} 
		else 
		{
			vertex4 = AdjacentEx(in_edge1,in_edge2,index,size);
			OutputTriEx(in_edge2,in_edge1,vertex4, -1,(where)?true:false, band);

			// Now delete the adjacencies by one for all the faces
			// that are adjacent to the triangle that we just outputted.	  
			DeleteAdjEx(in_edge1,in_edge2,&dummy,&dummy,face_id,&dummy,&dummy,&dummy);
			DeleteAdjEx(in_edge2,vertex4,&dummy,&dummy, face_id,&dummy,&dummy,&dummy);

			// Put the new face in the proper bucket of adjacencies
			next_bucket = ChangeFaceEx(face_id,in_edge1,in_edge2,pListHead,false);
			next_bucket = ChangeFaceEx(face_id,in_edge2,vertex4, pListHead,false);
	  
			// Create a new edgelist without the triangle that was just outputted.
			Delete_From_ListEx(in_edge2,index,size);
	  
			// Update the face data structure, by deleting the old
			// face and putting in the polygon minus the triangle 
			// as the new face, here we will be decrementing the size by one.
			NewSizeFace(face_id);

			// Save the info for the new bucket, we will need it on
			// the next pass for the variables, pListHead and temp 
			pListHead = m_pArray[next_bucket];
			temp2 = m_pFaceArray[face_id].pfNode;
			if (temp2 == NULL) 
			{
				int1 ErrorInfo[100];
				sprintf(ErrorInfo, "ViaStrip::Create strips: there is an error finding the next polygon11 %d %d\n", face_id,next_bucket);
				throw ex::AbortDataProcessingException(LogRec("ViaStrip", "P_TriangulatePolygon", "Create strips: There is an error finding the next polygon11."));
			}
			P_TriangulatePolygon(out_edge1,out_edge2,vertex4,
					in_edge1,size-1,index, !reversed,
					face_id,next_id,pListHead,temp2,where, band);
		}
		return;
	}
}

/**
 * do triangulate
 */
void ViaStrip::P_Triangulate(int4 out_edge1, int4 out_edge2, int4 in_edge1, int4 in_edge2, 
                             int4 size, int4 *index, int4 reversed, int4 face_id, 
                             int4 *next_id,ListHead *pListHead, ADJACENCIES * temp, int4 where,
                             std::ofstream & band)
{
  
  if (size == 4) 
	{
    P_Triangulate_Quad(out_edge1,out_edge2,in_edge1,in_edge2,size,
		       index,reversed,face_id,
		       pListHead,temp,where, band);
	}
  else
	{
    P_TriangulatePolygon(out_edge1,out_edge2,in_edge1,in_edge2,size,
			  index, reversed,face_id,next_id,
			  pListHead,temp,where, band);
	}
}

/**
 *	A polygon has to be triangulated but cannot do it blindly, 
 *	try to come out on the edge that	has the least number of adjacencies, 
 */
void ViaStrip::Partial_Triangulate(int4 size, int4 *index, int4 next_face_id, int4 face_id,
                                   int4 *next_id,ListHead *pListHead, ADJACENCIES * temp, 
                                   int4 where, std::ofstream & band)
{
	int4 id1,id2,id3;
	int4 nedge1,nedge2;
	int4 reversed;
  
	/*	We have a polygon that has to be triangulated and we cannot
	do it blindly, ie we will try to come out on the edge that
	has the least number of adjacencies, But also we do not
	want to triangulate the whole polygon now, so that means 
	we will output the least number of triangles that we can
	and then update the data structures, with the polygon
	that is left after we are done. */
	LastEdge(&id1,&id2,&id3,0);

	///Find the edge that is adjacent to the new face ,
	// also return whether the orientation is reversed in the
	// face of the input edge, which is id2 and id3.
	reversed = GetEdgeEx(&nedge1,&nedge2,index,next_face_id,size,id2,id3);

	// Input edge and output edge can be the same if there are more than
  // one polygon on an edge
	if (((nedge1 == id2) && (nedge2 == id3)) || ((nedge1 == id3) && (nedge2 == id2)) )
		nedge2 = ReturnOther(index,id2,id3);
  
	// Do the triangulation
	P_Triangulate(nedge1,nedge2,id2,id3,size,index, reversed,
			face_id,next_id,pListHead,temp,where, band);
}

/**
 *	The polygon had an input edge, specified by input1 and input2
 */
void ViaStrip::Input_Edge(int4 face_id, int4 *index, int4 size, int4 in_edge1, int4 in_edge2,  
                          ListHead *pListHead, int4 where, std::ofstream & band)
{
  int4 output1,next_bucket;
  int4 vertex4, vertex5,dummy=60;
  
  output1 = GetOutputEdge(face_id,size,index,in_edge1,in_edge2);
  vertex5 = AdjacentEx(in_edge2,in_edge1,index,size); 
  vertex4 = AdjacentEx(in_edge1,in_edge2,index,size);
  
  if (vertex4 == output1) 
	{
		OutputTriEx(in_edge2,in_edge1,output1,-1,(where)?true:false, band);

	  // Now delete the adjacencies by one for all the faces
	  // that are adjacent to the triangle that we just outputted.
		DeleteAdjEx(in_edge1,in_edge2,&dummy,&dummy,face_id, &dummy,&dummy,&dummy);
		DeleteAdjEx(in_edge2,output1,&dummy,&dummy, face_id,&dummy,&dummy,&dummy);

	  // Put the new face in the proper bucket of adjacencies
		next_bucket = ChangeFaceEx(face_id,in_edge1,in_edge2,pListHead,false);
		next_bucket = ChangeFaceEx(face_id,in_edge2,output1,pListHead,false);
      
	  // Create a new edgelist without the triangle that was just outputted.
		Delete_From_ListEx(in_edge2,index,size);
	}	
	else if (vertex5 == output1) 
	{
		OutputTriEx(in_edge1,in_edge2,vertex5, -1,(where)?true:false, band);

	  // Now delete the adjacencies by one for all the faces
	  // that are adjacent to the triangle that we just outputted.
		DeleteAdjEx(in_edge1,in_edge2,&dummy,&dummy,face_id, &dummy,&dummy,&dummy);
		DeleteAdjEx(in_edge1,vertex5,&dummy,&dummy, face_id,&dummy,&dummy,&dummy);
		next_bucket = ChangeFaceEx(face_id,in_edge1,in_edge2,pListHead,false);
		next_bucket = ChangeFaceEx(face_id,in_edge1,vertex5,pListHead,false);
	  // Create a new edgelist without the triangle that was just outputted.
		Delete_From_ListEx(in_edge1,index,size);
	}
  /* Update the face data structure, by deleting the old
	face and putting in the polygon minus the triangle 
	as the new face, here we will be decrementing the size
	by one.
  */
  NewSizeFace(face_id);
  return;
}

/**
 * If there is a a polygon that is greater than 4 sides, and
 * that it is better to go inside the polygon for the next one, 
 * since inside will have less adjacencies than going outside.
 * So, we are not doing partial for a part of the polygon.
 */
void ViaStrip::Inside_Polygon(int4 size, int4 *index, int4 face_id,
                              ListHead *pListHead, int4 where, std::ofstream & band)
{
	/* We know that we have a polygon that is greater than 4 sides, and
     that it is better for us to go inside the polygon for the next
     one, since inside will have less adjacencies than going outside.
     So, we are not doing partial for a part of the polygon. */
	int4 id1,id2,id3;
	int4 new1,new2;
  
	LastEdge(&id1,&id2,&id3,0);
  
	// See if the input edge existed in the polygon, that will help us
	if (Exist(face_id,id2,id3))
		Input_Edge(face_id,index,size,id2,id3,pListHead,where, band);
	else 
	{
		// Make one of the input edges 
		// We will choose it by trying to get an edge that has something
		// in common with the last triangle, or by getting the edge that
		// is adjacent to the least number of thigs, with preference given
		// to the first option.
		GetInputEdge(index,id1,id2,id3,&new1,&new2,size,face_id);
		Input_Edge(face_id,index,size,new1,new2,pListHead,where, band);
	}
}


// $Log: ViaPartial.C,v $
// Revision 1.2  2005/07/01 12:46:05  vxconfig
// Added new Logger.
//
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
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
//    Rev 2.0   Sep 16 2001 23:39:08   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:40   ingmar
// Initial revision.
// Revision 1.26  2001/08/13 20:29:19  sachin
// changed exceptions ,warnings,debug and information
// according to the new logger class
//
// Revision 1.25  2001/07/25 16:08:38  lihong
// Defect Id: 599
// Added throw ex:: after the function name
//
// Revision 1.24  2001/07/24 23:06:51  lihong
// Defect ID: 599
// new format of exceptions
//
// Revision 1.23  2001/07/23 16:56:27  lihong
// Defect ID: 599
// replaced file version with "$Revision: 1.2 $"
//
// Revision 1.22  2001/07/20 23:07:10  lihong
// Defect ID: 599
// changed abort() to throw ex
//
// Revision 1.21  2001/05/21 00:22:54  dave
// uses & implement. of Error::DebugMsg, Warn, Abort, etc, gone
//
// Revision 1.20  2001/04/09 17:31:26  dmitry
// updated datatypes
//
// Revision 1.19  2001/04/02 19:18:20  binli
// code simplified!
//
// Revision 1.18  2001/03/27 19:49:51  binli
// code walkthrough: name convension
//
// Revision 1.17  2001/03/27 16:55:54  binli
// reflected new code standard.
//
// Revision 1.16  2001/03/23 20:41:20  binli
// code walkthrough
//
// Revision 1.15  2001/03/22 21:15:21  binli
// renamed some variables according to new code standard.
//
// Revision 1.14  2001/03/21 15:24:11  binli
// replaced all structs with classes.
//
// Revision 1.13  2001/03/07 19:57:21  binli
// malloc/free replaced by new/delete
//
// Revision 1.12  2001/03/01 17:49:52  binli
// changed 'int' to 'bool' in Output_Tri() and Output_TriEx().
//
// Revision 1.11  2001/02/28 21:01:59  binli
// 1> BOOL->bool
// 2> Error Info
//
// Revision 1.10  2000/12/08 21:16:41  binli
// removed code to write out 'text' triangle files.
//
// Revision 1.9  2000/12/08 16:56:18  binli
// changed the output of triangle files from text to binary.
// added header to triangle files.
//
// Revision 1.8  2000/11/06 23:57:28  binli
// copied the old version (from mixkit) back to replace 'polygon generation'  in lastest version.
// so as to make the triangle generation modular works.
//
// Revision 1.7  2000/11/02 04:54:47  antonio
// changed int where -> bool end for functions in ViaStrip.h
//
// Revision 1.6  2000/10/21 22:24:25  ingmar
// code cleanup
//
// Revision 1.5  2000/10/21 22:21:55  ingmar
// BOOL -> bool and other code cleanup
//
// Revision 1.4  2000/10/21 06:06:04  ingmar
// merged mixkit branch back into main trunk
//
// Revision 1.1.1.1  2000/08/11 22:15:54  ingmar
// unit tests complete
//
//
// 6     9/07/00 12:11p Binli
// fitted to viatronix standard
//
// 5     8/29/00 1:33p Antonio
// updated footer
//
// *****************  Version 4  *****************
// User: Binli           Date:  8/24/00  Time: 12:30p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments to Class ViaStrip
//
// *****************  Version 3  *****************
// User: Antonio         Date:  8/21/00  Time:  6:10p
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
// Created ViaPartial.C
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaPartial.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
// $Id: ViaPartial.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
