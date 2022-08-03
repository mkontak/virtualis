// $Id: ViaOutput.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViaOutput.C  
// Description: implementation (13/17) of Class ViaStrip
// Created: July 31, 2000
// Author:  Freeware
//			    Rewriten by Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.

#include "stdafx.h"
#include "ViaStrip.h"

static int4 norm;
static int4 text;

#define FILE_REVISION "$Revision: 1.2 $"

/**
 *	All the triangles are finished, now is time to output to the data file. 
 */
int4 ViaStrip::Finished(int4 *swap, int4 startnewstrip, std::ofstream & band)
{
	// We have finished all the triangles, now is time to output to
	// the data file. In the strips data structure, every three ids
	// is  a triangle. Now we see whether we can swap, or make a new strip
	// or continue the strip, and output the data accordingly to the data file. 

	int4 num,x,vertex1,vertex2;
	int4 id[2],other1,other2,index = 0,a,b,c;
	int4 bandBuf;
	bool cptexture;
	ListHead *pListHead;
	Strips *temp1, *temp2, *temp3, *temp4, *temp5, *temp6;
	
	*swap =0;
	// cptexture = text;
	cptexture = (text) ? true : false;
	
	pListHead = m_pStrips;
	if (pListHead == NULL)
		return 0;
  
	num = NumOnList(pListHead);
  
	// Go through the list triangle by triangle
	temp1 = (Strips *) PeekList( pListHead, LISTHEAD, 0);
	temp2 = (Strips *) PeekList( pListHead, LISTHEAD, 1);
	temp3 = (Strips *) PeekList( pListHead, LISTHEAD, 2);
  
	// Next triangle for lookahead
	temp4 = (Strips *) PeekList( pListHead, LISTHEAD, 3);
  
	// There is only one polygon in the strip
	if (temp4 == NULL) 
	{
		// Data might be mixed and we do not have textures for some of the vertices
		if ((text) &&  (m_pVT[temp3->face_id] == 0))
			cptexture = false;
		if ((norm) && (!cptexture))
		{
			if (startnewstrip && m_iOrient)	// If we want to keep orientation
				PreserveStripOrientationWithNormal(
						temp3->face_id+1,m_pVN[temp3->face_id]+1,
						temp2->face_id+1,m_pVN[temp2->face_id]+1,
						temp1->face_id+1,m_pVN[temp1->face_id]+1, band);

			// 'band' write:
			band << ' ';
			bandBuf = temp3->face_id+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "//";
			bandBuf = m_pVN[temp3->face_id]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			
			band << ' ';
			bandBuf =	temp2->face_id+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "//";
			bandBuf = m_pVN[temp2->face_id]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
					
			band << ' ';
			bandBuf = temp1->face_id+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "//";
			bandBuf = m_pVN[temp1->face_id]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
		} 
		else if ((cptexture) && (!norm)) 
		{
			if (startnewstrip && m_iOrient) // If we want to keep orientation
				PreserveStripOrientationWithTexture(//output,
						temp3->face_id+1, m_pVT[temp3->face_id]+1,
						temp2->face_id+1, m_pVT[temp2->face_id]+1,
						temp1->face_id+1, m_pVT[temp1->face_id]+1, band);

			// 'band' write:
			band << ' ';
			bandBuf = temp3->face_id+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVT[temp3->face_id]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			
			band << ' ';
			bandBuf =	temp2->face_id+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVT[temp2->face_id]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
					
			band << ' ';
			bandBuf = temp1->face_id+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVT[temp1->face_id]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
		} 
		else if ((cptexture)&& (norm)) 
		{
			if (startnewstrip && m_iOrient)	// If we want to keep orientation
				PreserveStripOrientationWithTextureAndNormal(// output,
						temp3->face_id+1, m_pVT[temp3->face_id]+1, m_pVN[temp3->face_id]+1,
						temp2->face_id+1, m_pVT[temp2->face_id]+1, m_pVN[temp2->face_id]+1,
						temp1->face_id+1, m_pVT[temp1->face_id]+1, m_pVN[temp1->face_id]+1, band);

			// 'band' write:
			band << ' ';
			bandBuf = temp3->face_id+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVT[temp3->face_id]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVN[temp3->face_id]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			
			band << ' ';
			bandBuf =	temp2->face_id+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVT[temp2->face_id]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVN[temp2->face_id]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
					
			band << ' ';
			bandBuf = temp1->face_id+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVT[temp1->face_id]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVN[temp1->face_id]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
		} 
		else 
		{
			if (startnewstrip && m_iOrient)	// If we want to keep orientation
				PreserveStripOrientation(temp3->face_id+1,temp2->face_id+1,temp1->face_id+1, band);

			// 'band' write:
			band << ' ';
			bandBuf = temp3->face_id+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << " ";
			bandBuf = temp2->face_id+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << " ";
			bandBuf = temp1->face_id+1;
			band.write((int1 *) &bandBuf, sizeof(int4));	
		}
		FreeStrips();
		return 1;
	} // endif (temp4 == NULL)
  
	// We have a real strip
	temp5 = (Strips *) PeekList( pListHead, LISTHEAD, 4);
	temp6 = (Strips *) PeekList( pListHead, LISTHEAD, 5);
  
	if ((temp1 == NULL) || (temp2 == NULL) ||
		(temp3 == NULL) || (temp5 == NULL) || (temp6 == NULL)) 
	{
		throw ex::AbortDataProcessingException(LogRec("ViaStrip", "Finished", "Create strips: there is an error in the output of the triangles."));
	}
  
	// Find the vertex in the first triangle that is not in the second
	vertex1 = Different(temp1->face_id,temp2->face_id,temp3->face_id,
					temp4->face_id,temp5->face_id,temp6->face_id, &other1,&other2);
	// Find the vertex in the second triangle that is not in the first
	vertex2 = Different(temp4->face_id,temp5->face_id,temp6->face_id,
					temp1->face_id,temp2->face_id,temp3->face_id,&other1,&other2);
  
	// Lookahead for the correct order of the 2nd and 3rd vertex of the first triangle
	temp1 = (Strips *) PeekList( pListHead, LISTHEAD, 6);
	temp2 = (Strips *) PeekList( pListHead, LISTHEAD, 7);
	temp3 = (Strips *) PeekList( pListHead, LISTHEAD, 8);
  
	if (temp1 != NULL)
		other1 = Different(temp3->face_id,temp4->face_id,temp5->face_id,
	temp1->face_id,temp2->face_id,temp3->face_id,&other1,&a);
  
	id[index] = vertex1; index = !index;
	id[index] = other1; index = !index;
	id[index] = other2; index = !index;
  
	a = temp4->face_id; 
	b = temp5->face_id; 
	c = temp6->face_id;
  
	// If we need to rearrange the first sequence because otherwise
	// there would have been a swap.
	if ((temp3 != NULL) && (text) && ( m_pVT[temp3->face_id]==0))
		cptexture = false;
	if ((norm) && (!cptexture)) 
	{
		if (m_iOrient && startnewstrip) // If we want to keep orientation
			PreserveStripOrientationWithNormal(
					     vertex1+1, m_pVN[vertex1]+1,
					     other1 +1, m_pVN[ other1]+1,
					     other2 +1, m_pVN[ other2]+1, band);

			// 'band' write:
			band << ' ';
			bandBuf = vertex1+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "//";
			bandBuf = m_pVN[vertex1]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			
			band << ' ';
			bandBuf =	other1+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "//";
			bandBuf = m_pVN[other1]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
					
			band << ' ';
			bandBuf = other2+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "//";
			bandBuf = m_pVN[other2]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
	} 
	else if ((cptexture) && (!norm)) 
	{
		if (m_iOrient && startnewstrip) // If we want to keep orientation
			PreserveStripOrientationWithTexture(//output,
					      vertex1+1, m_pVT[vertex1]+1,
					      other1 +1, m_pVT[ other1]+1,
					      other2 +1, m_pVT[ other2]+1, band);
 			
			// 'band' write:
			band << ' ';
			bandBuf = vertex1+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVT[vertex1]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			
			band << ' ';
			bandBuf =	other1+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVT[other1]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
					
			band << ' ';
			bandBuf = other2+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVT[other2]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
	} 
	else if ((cptexture) && (norm))
	{
		if (m_iOrient && startnewstrip) // If we want to keep orientation
			PreserveStripOrientationWithTextureAndNormal( //output,
							 vertex1+1,m_pVT[vertex1]+1, m_pVN[vertex1]+1,
							 other1+1, m_pVT[ other1]+1, m_pVN[ other1]+1,
							 other2+1, m_pVT[ other2]+1, m_pVN[ other2]+1, band);

			// 'band' write:
			band << ' ';
			bandBuf = vertex1+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVT[vertex1]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVN[vertex1]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			
			band << ' ';
			bandBuf =	other1+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVT[other1]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVN[other1]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
					
			band << ' ';
			bandBuf = other2+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVT[other2]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVN[other2]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
	}  
	else  
	{
		if (m_iOrient && startnewstrip) // If we want to keep orientation
			PreserveStripOrientation(vertex1+1,other1+1,other2+1, band);
    
		// 'band' write:
		band << ' ';
		bandBuf = vertex1+1;
		band.write((int1 *) &bandBuf, sizeof(int4));
		
		band << ' ';
		bandBuf =	other1+1;
		band.write((int1 *) &bandBuf, sizeof(int4));
				
		band << ' ';
		bandBuf = other2+1;
		band.write((int1 *) &bandBuf, sizeof(int4));
	}
	for (x = 6; x < num ; x = x+3) 
	{
		// Get the next triangle
		temp1 = (Strips *) PeekList( pListHead, LISTHEAD, x);       
		temp2 = (Strips *) GetNextNode(temp1);                                      
		temp3 = (Strips *) GetNextNode(temp2);                                      
      
		// Error checking
		if (!(member(id[0],a,b,c)) || !(member(id[1],a,b,c)) || !(member(vertex2,a,b,c))) 
		{
			// If we used partial we might have a break in the middle of a strip
			// 'band' write:
			band << "\nt";

			startnewstrip = 1;
			// Find the vertex in the first triangle that is not in the second
			vertex1 = Different(a,b,c,temp1->face_id,temp2->face_id,
                              temp3->face_id,&other1,&other2);
			// Find the vertex in the second triangle that is not in the first
			vertex2 = Different(temp1->face_id,temp2->face_id,temp3->face_id,
                              a,b,c,&other1,&other2);
	  
			id[index] = vertex1; index = !index;
			id[index] = other1; index = !index;
			id[index] = other2; index = !index;
		}
      
		if ((temp1 == NULL ) || (temp2 == NULL) || (temp3 == NULL)) 
		{
			throw ex::AbortDataProcessingException(LogRec("ViaStrip", "Finished", "Create strips: there is an error in the triangle list" ));
		}
		if ((id[0] == id[1]) || (id[0] == vertex2))
			continue;
      
		if ((member(id[index],temp1->face_id,temp2->face_id,temp3->face_id))) 
		{
			if ((text) && ( m_pVT[id[index]]==0))
				cptexture = false;
			if ((!norm) && (!cptexture)) 
			{
				// 'band' write:
				band << ' ';
				bandBuf = id[index]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
			} 
			else if ((norm) && (!cptexture)) 
			{
				// 'band' write:
				band << ' ';
				bandBuf = id[index]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "//";
				bandBuf = m_pVN[id[index]]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
			} 
			else if ((!norm) && (cptexture))
			{
				// 'band' write:
				band << ' ';
				bandBuf = id[index]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVT[id[index]]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
			}
			else 
			{
				// 'band' write:
				band << ' ';
				bandBuf = id[index]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVT[id[index]]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVN[id[index]]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
			}
			index = !index;
			*swap = *swap + 1;
		}
      
		if ((text) && ( m_pVT[vertex2]==0))
			cptexture = false;
		if ((!norm) && (!cptexture))
		{
			// 'band' write:
			band << "\nq ";
			bandBuf = vertex2+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
		}	
		else if ((norm) && (!cptexture)) 
		{
			// 'band' write:
			band << "\nq ";
			bandBuf = vertex2+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "//";
			bandBuf = m_pVN[vertex2]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
		} 
		else if ((!norm) && (cptexture))
		{
			// 'band' write:
			band << "\nq ";
			bandBuf = vertex2+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVT[vertex2]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
		}	
		else 
		{
			// 'band' write:
			band << "\nq ";
			bandBuf = vertex2+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVT[vertex2]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
			band << "/";
			bandBuf = m_pVN[vertex2]+1;
			band.write((int1 *) &bandBuf, sizeof(int4));
		}
		id[index] = vertex2; index = !index;
      
		// Get the next vertex not in common
		vertex2 = Different(temp1->face_id,temp2->face_id,temp3->face_id, a,b,c,&other1,&other2);
		a = temp1->face_id;
		b = temp2->face_id;
		c = temp3->face_id;
	} // endfor (x-loop)
	// Do the last vertex
	if ((text) && (m_pVT[vertex2]==0))
	{
		if (cptexture) {
			// 'band' write:
			band << "\nq";
		}
		cptexture = false;
	}
	if ((!norm) && (!cptexture))
	{
		// 'band' write:
		band << ' ';
		bandBuf = vertex2+1;
		band.write((int1 *) &bandBuf, sizeof(int4));
	} 
	else if ((norm) && (!cptexture))
	{
		// 'band' write:
		band << ' ';
		bandBuf = vertex2+1;
		band.write((int1 *) &bandBuf, sizeof(int4));
		band << "//";
		bandBuf = m_pVN[vertex2]+1;
		band.write((int1 *) &bandBuf, sizeof(int4));
	} 
	else if ((!norm) && (cptexture))
	{
		// 'band' write:
		band << ' ';
		bandBuf = vertex2+1;
		band.write((int1 *) &bandBuf, sizeof(int4));
		band << "/";
		bandBuf = m_pVT[vertex2]+1;
		band.write((int1 *) &bandBuf, sizeof(int4));
	}
	else 
	{
		// 'band' write:
		band << ' ';
		bandBuf = vertex2+1;
		band.write((int1 *) &bandBuf, sizeof(int4));
		band << "/";
		bandBuf = m_pVT[vertex2]+1;
		band.write((int1 *) &bandBuf, sizeof(int4));
		band << "/";
		bandBuf = m_pVN[vertex2]+1;
		band.write((int1 *) &bandBuf, sizeof(int4));
	}
  
	FreeStrips();
	return (num/3);       
}

/**
 *	save everything into a list, rather than output at once,
 */
void ViaStrip::OutputTri(int4 id1, int4 id2, int4 id3, bool end)
// void ViaStrip::Output_Tri(int4 id1, int4 id2, int4 id3, int4 end)
{
	// We will save everything into a list, rather than output at once,
	// as was done in the old routine. This way for future modifications
	// we can change the strips later on if we want to.
	
	int4 temp1,temp2,temp3;
  
	// Make sure we do not have an error
	// There are degeneracies in some of the files
	if ( (id1 == id2) || (id1 == id3) || (id2 == id3))
	{
		int1 ErrorInfo[100];
		sprintf(ErrorInfo, "Create strips: degenerate triangle %d %d %d\n",id1,id2,id3);
		throw ex::AbortDataProcessingException(LogRec( "ViaStrip", "OutputTri","Create strips: degenerate triangle."));
	} 
	else 
	{
		LastEdge(&temp1,&temp2,&temp3,0);
		AddIdStrips(id1,end);
		AddIdStrips(id2,end);
		AddIdStrips(id3,end);
		LastEdge(&id1,&id2,&id3,1);
	} // end-if
} // Output_Tri().

/**
 *	output a polygon
 */
int4 ViaStrip::PolygonOutput(ADJACENCIES * temp, int4 face_id,int4 bucket,
			  ListHead *pListHead, bool first, int4 *swaps,
			  int4 color1,int4 color2,int4 color3,bool global, bool end,
				std::ofstream & band)
{
	ListHead *pListFace;
	FFaces * face;
	int4 next_face_id,next_bucket,e1,e2,e3,other1,other2,other3;
	ADJACENCIES * lpListInfo; 
	int4 ties=0;
  
	// We have a polygon to output, the id is face id, and the number
    // of adjacent polygons to it is bucket. This routine extends the patches from
    // either end to make longer triangle strips.
  
	// Now get the edge
	LastEdge(&e1,&e2,&e3,0);
  
	// Get the polygon with id face_id
	pListFace  = m_ppPolFaces[face_id];
	face = (FFaces *) PeekList(pListFace,LISTHEAD,0);
  
	// We can't go any more
	if ((face->nPolSize == 1) || ((face->nPolSize == 4) && (global))) 
	{
		// Remove it from the list so we do not have to waste
		// time visiting it in the future, or winding up in an infinite loop
		// if it is the first on that we are looking at for a possible strip  
		if (face->nPolSize == 1) 
		{
			if (m_pFaceArray[temp->face_id].head == pListHead)
				m_pFaceArray[temp->face_id].pfNode = NULL;
			RemoveList(pListHead,(ListInfo *) temp);
		}
		if (first)
			return 0;
		else
			return (Finished(swaps, 0, band));
	}
	if (face->nPolSize == 3) 
	{ 
		// It is already a triangle
		if (bucket == 0) 
		{
			// It is not adjacent to anything so we do not have to
			// worry about the order of the sides or updating adjacencies
			next_face_id = Different(*(face->pPolygon),*(face->pPolygon+1),
				   *(face->pPolygon+2), e1,e2,e3,&other1,&other2);
			face->nPolSize = 1;
	  
			// If this is the first triangle in the strip
			if ((e2 == 0) && (e3 ==0)) 
			{
				e2 = other1;
				e3 = other2;
			}
			OutputTri(e2,e3,next_face_id,end);
			if (m_pFaceArray[temp->face_id].head == pListHead)
				m_pFaceArray[temp->face_id].pfNode = NULL;
			RemoveList(pListHead,(ListInfo *) temp);
			return (Finished(swaps, 0, band));
		}
		// It is a triangle with adjacencies. This means that we have to:
		// 1. Update the adjacencies in the list, because we are using this polygon and it will be deleted.
		// 2. Get the next polygon.
		else 
		{
			// Return the face_id of the next polygon we will be using,
			// while updating the adjacency list by decrementing the
			// adjacencies of everything adjacent to the current triangle.
			next_face_id = UpdateAdjacencies(face_id, &next_bucket, &e1,&e2,&ties);
			// Maybe we deleted something in a patch and could not find an adj polygon
			if (next_face_id == -1) 
			{
				OutputTri(*(face->pPolygon),*(face->pPolygon+1),*(face->pPolygon+2),end);
				face->nPolSize = 1;
				if (m_pFaceArray[temp->face_id].head == pListHead)
					m_pFaceArray[temp->face_id].pfNode = NULL;
				RemoveList(pListHead,(ListInfo *) temp);
				return (Finished(swaps, 0, band));
			}
	  
			// Find the other vertex to transmit in the triangle
			e3 = ReturnOther(face->pPolygon,e1,e2);
			LastEdge(&other1,&other2,&other3,0);
	  
			if ((other2 != 0) && (other3 != 0)) 
			{
				// See which vertex in the output edge is not in the input edge
				if ((e1 != other2) && (e1 != other3))
					e3 = e1;
				else if ((e2 != other2) && (e2 != other3))
					e3 = e2;
				else 
        {
          throw ex::AbortDataProcessingException(LogRec("ViaStrip", "PolygonOutput", "Create strips: there is an error in the tri with adj."));
        }
				// See which vertex of the input edge is not in the output edge
				if ((other2 != e1) && (other2 != e2)) 
				{
					other1 = other2;
					other2 = other3;
				} else if ((other3 != e1) && (other3 != e2))
					other1 = other3;
				else 
				{
					throw ex::AbortDataProcessingException(LogRec("ViaStrip", "PolygonOutput" , "Create strips: there is an error in getting the tri with adj."));
				}
			} 
			else 
			{
				// We are the first triangle in the strip and the starting edge has not been set yet
				// Maybe we deleted something in a patch and could not find an adj polygon
				if (next_face_id == -1) 
				{
					OutputTri(*(face->pPolygon),*(face->pPolygon+1),*(face->pPolygon+2),end);
					face->nPolSize = 1;
					m_pFaceArray[temp->face_id].pfNode = NULL;
					RemoveList(pListHead,(ListInfo *) temp);
					return (Finished(swaps, 0, band));
				}
				other1 = e3;
				e3 = e2;
				other2 = e1;
			}
			// At this point the adjacencies have been updated  and we have the next polygon id 
			OutputTri(other1,other2,e3,end);
			face->nPolSize = 1;
			if (m_pFaceArray[temp->face_id].head == pListHead)
				m_pFaceArray[temp->face_id].pfNode = NULL;
			RemoveList(pListHead,(ListInfo *) temp);
	  
			// Maybe we deleted something in a patch and could not find an adj polygon
			if (next_face_id == -1)
				return (Finished(swaps, 0, band));
	  
			if (Done(next_face_id,&next_bucket) == NULL) 
			{
				int1 ErrorInfo[100];
				sprintf(ErrorInfo, "We deleted the next face 4%d\n",next_face_id);
				throw ex::AbortDataProcessingException(LogRec("ViaStrip", "PolygonOutput", "We deleted the next face."));
			}
	  
			pListHead = m_pArray[next_bucket]; 
			lpListInfo = m_pFaceArray[next_face_id].pfNode;
			if (lpListInfo == NULL) 
			{
				int1 ErrorInfo[100]; 
				sprintf(ErrorInfo, "Create strips: there is an error finding the next polygon3 %d\n", next_face_id);
				throw ex::AbortDataProcessingException(LogRec("ViaStrip", "PolygonOutput", "Create strips: there is an error finding the next polygon3."));
			}
			return (PolygonOutput(lpListInfo,next_face_id,next_bucket,
				 pListHead, false, swaps, color1,color2,color3,global,end, band));
		} // end-else
	} // endif (face->nPolSize == 3)
	else 
	{ 
		// It is not a triangle, we have to triangulate it .
		// Since it is not adjacent to anything we can triangulate it  blindly
		if (bucket == 0) 
		{
			// It is the first polygon in the strip, therefore there is no input edge to start with.
	  	if ((e2 == 0) && (e3 ==0))
				BlindTriangulate(face->nPolSize,face->pPolygon,true,1);
			else
				BlindTriangulate(face->nPolSize,face->pPolygon,false,1);
	  
			if (m_pFaceArray[temp->face_id].head == pListHead)
				m_pFaceArray[temp->face_id].pfNode = NULL;
			RemoveList(pListHead,(ListInfo *) temp);
	  
			// We will be at the beginning of the next strip.
			face->nPolSize = 1;
			return (Finished(swaps, 0, band));
		} 
		else 
		{
			// WHOLE triangulation
			// It is not a triangle and has adjacencies.  This means that we have to:
			// 1. Triangulate this polygon, not blindly because
			//		we have an edge that we want to come out on, that
			//		is the edge that is adjacent to a polygon with the
			//		least number of adjacencies. Also we must come in on the last seen edge.
			// 2. Update the adjacencies in the list, because we are using this polygon .
			// 3. Get the next polygon.
			
			// Return the face_id of the next polygon we will be using,
			// while updating the adjacency list by decrementing the
			// adjacencies of everything adjacent to the current polygon.
	  
			next_face_id = UpdateAdjacencies(face_id, &next_bucket, &e1,&e2,&ties);
	  
			// Maybe we deleted something in a patch and could not find an adj polygon
			if (next_face_id == -1) 
			{
				// If we are at the first polygon in the strip and there is no input
				// edge, then begin is TRUE
				if ((e2 == 0) && (e3 == 0))
					BlindTriangulate(face->nPolSize,face->pPolygon,true,1);
				else
					BlindTriangulate(face->nPolSize,face->pPolygon,false,1);
				if (m_pFaceArray[temp->face_id].head == pListHead)
					m_pFaceArray[temp->face_id].pfNode = NULL;
				RemoveList(pListHead,(ListInfo *) temp);
	      
				// We will be at the beginning of the next strip.
				face->nPolSize = 1;
				return (Finished(swaps, 0, band));
			}
			if (Done(next_face_id,&next_bucket) == NULL) 
			{
				int1 ErrorInfo[100];
				sprintf(ErrorInfo, "Create strips: We deleted the next face 6 %d %d\n",next_face_id,face_id);
				throw ex::AbortDataProcessingException(LogRec("ViaStrip", "PolygonOutput", "Create strips: We deleted the next face 6." ));
			}
			Non_Blind_Triangulate(face->nPolSize,face->pPolygon, 
					next_face_id,face_id,1, color1,color2,color3, band);
			
			if (m_pFaceArray[temp->face_id].head == pListHead)
				m_pFaceArray[temp->face_id].pfNode = NULL; 
			RemoveList(pListHead,(ListInfo *) temp);
			face->nPolSize = 1;
			pListHead = m_pArray[next_bucket];
			lpListInfo = m_pFaceArray[next_face_id].pfNode;
			if (lpListInfo == NULL) 
      {
        int1 ErrorInfo[100];
        sprintf(ErrorInfo, "Create strips: there is an error finding the next polygon2 %d %d\n", next_face_id,next_bucket);
        throw ex::AbortDataProcessingException(LogRec("ViaStrip", "PolygonOutput", "Create strips: there is an error finding the next polygon2." ));
      }
			return (PolygonOutput(lpListInfo,next_face_id,next_bucket,
					pListHead, false, swaps, color1,color2,color3,global,end, band));
		} // end-else (bucket==0)
	} // end-else : (face->nPolSize == 3)
}

/**
 *	Try to extend backwards off of the local strip that we just found
 */
int4 ViaStrip::ExtendFace(int4 face_id,int4 e1,int4 e2,int4 *swaps, 
												 int4 color1,int4 color2,int4 color3,int4 *vert_norm, int4 normals,
												 int4 *vert_texture, int4 texture,	std::ofstream & band)
{
	int4 dummy=0,next_bucket;
	ADJACENCIES * lpListInfo;   
	ListHead * pListHead;
  
	// Try to extend backwards off of the local strip that we just found
	m_pVN = vert_norm;
	m_pVT = vert_texture;
	norm = normals;
	text = texture;
  
	*swaps = 0;
	// Find the face that is adjacent to the edge and is not the current face.
  
	face_id = FindFace(face_id, e1, e2,&next_bucket);
	if (face_id == -1)
		return 0;
  
	pListHead = m_pArray[next_bucket];
	lpListInfo = m_pFaceArray[face_id].pfNode;
  
	if (lpListInfo == NULL) 
	{
		int1 ErrorInfo[100];
		sprintf(ErrorInfo, "There is an error finding the next polygon3 %d\n",face_id);
		throw ex::AbortDataProcessingException(LogRec( "ViaStrip", "ExtendFace", "There is an error finding the next polygon3."));
 	}
	LastEdge(&dummy,&e1,&e2,1);
  
	// Find a strip extending from the patch and return the cost
	return (PolygonOutput(lpListInfo, face_id, next_bucket, pListHead, true, swaps,
					color1, color2, color3, true, true, band));
}


// $Log: ViaOutput.C,v $
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
//    Rev 2.0   Sep 16 2001 23:39:06   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:38   ingmar
// Initial revision.
// Revision 1.23  2001/08/13 20:29:19  sachin
// changed exceptions ,warnings,debug and information
// according to the new logger class
//
// Revision 1.22  2001/07/25 16:08:39  lihong
// Defect Id: 599
// Added throw ex:: after the function name
//
// Revision 1.21  2001/07/24 23:06:51  lihong
// Defect ID: 599
// new format of exceptions
//
// Revision 1.20  2001/07/23 16:56:27  lihong
// Defect ID: 599
// replaced file version with "$Revision: 1.2 $"
//
// Revision 1.19  2001/07/20 23:07:10  lihong
// Defect ID: 599
// changed abort() to throw ex
//
// Revision 1.18  2001/05/21 00:22:54  dave
// uses & implement. of Error::DebugMsg, Warn, Abort, etc, gone
//
// Revision 1.17  2001/04/09 17:31:26  dmitry
// updated datatypes
//
// Revision 1.16  2001/04/02 19:18:20  binli
// code simplified!
//
// Revision 1.15  2001/03/27 19:49:51  binli
// code walkthrough: name convension
//
// Revision 1.14  2001/03/27 16:55:54  binli
// reflected new code standard.
//
// Revision 1.13  2001/03/23 20:41:20  binli
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
// Revision 1.9  2001/03/01 17:49:52  binli
// changed 'int' to 'bool' in Output_Tri() and Output_TriEx().
//
// Revision 1.8  2001/02/28 21:01:59  binli
// 1> BOOL->bool
// 2> Error Info
//
// Revision 1.7  2001/02/28 18:19:38  binli
// tried to solve 'BOOL' problem in ViaStrip: changed parameter from BOOL to int in Output_Tri().
//
// Revision 1.6  2000/12/08 21:16:41  binli
// removed code to write out 'text' triangle files.
//
// Revision 1.5  2000/12/08 16:56:18  binli
// changed the output of triangle files from text to binary.
// added header to triangle files.
//
// Revision 1.4  2000/11/06 23:57:41  binli
// copied the old version (from mixkit) back to replace 'polygon generation'  in lastest version.
// so as to make the triangle generation modular works.
//
// Revision 1.3  2000/11/02 02:18:26  ingmar
// made int texture -> bool texture
//
// Revision 1.2  2000/10/21 21:19:41  ingmar
// BOOL -> bool and other code cleanup
//
// Revision 1.1.1.1  2000/08/11 22:15:54  ingmar
// unit tests complete
//
// 
// 6     9/07/00 12:11p Binli
// fitted to viatronix standard
// 
// 5     8/29/00 1:36p Antonio
// updated footer and header
// 
// *****************  Version 4  *****************
// User: Binli           Date:  8/24/00  Time: 12:30p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments to Class ViaStrip
// 
// *****************  Version 3  *****************
// User: Antonio         Date:  8/21/00  Time:  6:05p
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
// Created ViaOutput.C
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaOutput.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
// $Id: ViaOutput.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
