// $Id: ViaNewpolve.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViaNewpolve.C  
// Description: Implementation (12/17) of Class ViaStrip
// Created: July 31, 2000
// Author:  Freeware
//			    Rewriten by Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.

#include "stdafx.h"
#include "ViaStrip.h"

static int4 added_quad = 0;
static int4 patch = 0;
static bool resetting = false;
static bool reversed = false;

const int4 MAX_BAND = 10000;

#define FILE_REVISION "$Revision: 1.2 $"

/**
 *	Find the length of the walk
 */
int4 ViaStrip::CalculateWalks(int4 lastvert,int4 y, FFaces * temp2)
{
	// Find the length of the walk
	int4 previous_edge1, previous_edge2;
	register int4 nextvert,numverts,counter,walk=0;
	bool flag;
	FEdges * node;
	ListHead *pListHead;
	static int4 seen = 0;
  
	// Find the edge that we are currently on
	if (y != 3) 
	{
		previous_edge1 = *(temp2->pPolygon +y);
		previous_edge2 = *(temp2->pPolygon + y + 1);
	} 
	else 
	{
		previous_edge1 = *(temp2->pPolygon +y);
		previous_edge2 = *(temp2->pPolygon);
	}
	temp2->seen = seen;
	counter = y;
  
	// Find the adjacent face to this edge
	node = *(temp2->VertandId+y);			
	if (node->edge[2] != lastvert)
		nextvert = node->edge[2];
	else
		nextvert = node->edge[1];
  
	// Keep walking in this direction until we cannot do so
	while ((nextvert != lastvert) && (nextvert != -1)) 
	{
		walk++;
		pListHead = m_ppPolFaces[nextvert];
		temp2 = (FFaces *) PeekList(pListHead,LISTHEAD,0);
		numverts = temp2->nPolSize;
		if ((numverts != 4) || (temp2->seen == seen)) 
		{
			walk--;
			nextvert = -1;
		} 
		else 
		{
			temp2->seen = seen;
			// Find edge that is not adjacent to the previous one 
			counter = 0;
			flag = true;
			while ((counter < 3) && (flag)) 
			{
				if (((*(temp2->pPolygon+counter) == previous_edge1) ||
					(*(temp2->pPolygon+counter+1) == previous_edge2)) ||
					((*(temp2->pPolygon+counter) == previous_edge2) || 
					(*(temp2->pPolygon+counter+1) == previous_edge1)))
					counter++;		
				else
					flag = false;	
			}
			// Get the IDs of the next edge 
			if (counter < 3) 
			{
				previous_edge1 = *(temp2->pPolygon + counter);
				previous_edge2 = *(temp2->pPolygon + counter + 1);
			} 
			else 
			{
				previous_edge1 = *(temp2->pPolygon + counter);
				previous_edge2 = *(temp2->pPolygon);
			}
			node = *(temp2->VertandId + counter);
			if (node->edge[1] == nextvert)
				nextvert = node->edge[2];
			else
				nextvert = node->edge[1];
		}
	}
	seen++; 
	return walk;
}

/**
 *	Check when we last saw the face to the right of the current
 *	one. Do it just before start this strip
 */
bool ViaStrip::Check_Right(int4 last_seen, FFaces * temp2, int4 y, int4 face_id)
{
	// Check when we last saw the face to the right of the current
    // one. We want to have seen it just before we started this strip
	FEdges * node;
	ListHead * pListHead;
	register int4 nextvert,oldy;
	FFaces * t;
  
	oldy = y;
	if (y != 3)
		y = y+1;
	else
		y = 0;
	node = *(temp2->VertandId + y);
	if (face_id == node->edge[1])
		nextvert = node->edge[2];
	else
		nextvert = node->edge[1];
  
	if (nextvert == -1)
		return false;
  
	pListHead = m_ppPolFaces[nextvert];
	t = (FFaces *) PeekList(pListHead,LISTHEAD,0);
	if (t->seen != (last_seen - 1)) 
	{
		// maybe because of the numbering, we are not
		// on the right orientation, so we have to check the
		// opposite one to be sure 
		if (oldy != 0)
			y = oldy-1;
		else
			y = 3;
		node = *(temp2->VertandId + y);
		if (face_id == node->edge[1])
			nextvert = node->edge[2];
		else
			nextvert = node->edge[1];
		if (nextvert == -1)
			return false;
		pListHead = m_ppPolFaces[nextvert];
		t = (FFaces *) PeekList(pListHead,LISTHEAD,0);
		if (t->seen != (last_seen - 1))
			return false;
	} // endif
	return true;
}

/**
 *	Update and Test
 */
int4 ViaStrip::Update_and_Test(FFaces * temp2, int4 y, bool first, int4 distance,
			   int4 lastvert, int4 val)
{
	static int4 last_seen = 17;
	int4 previous_edge1, previous_edge2;
	register int4 original_distance,nextvert,numverts,counter;
	bool flag;
	FEdges  * node;
	ListHead * pListHead;
  
	original_distance = distance;
	// Find the edge that we are currently on
	if (y != 3)
	{
		previous_edge1 = *(temp2->pPolygon +y);
		previous_edge2 = *(temp2->pPolygon + y + 1);
	} 
	else 
	{
		previous_edge1 = *(temp2->pPolygon +y);
		previous_edge2 = *(temp2->pPolygon);
	}
  
	temp2->seen = val;
	temp2->seen2 = val;
  
	node = *(temp2->VertandId+y);                   
	if (lastvert != node->edge[2])
		nextvert = node->edge[2];
	else
		nextvert = node->edge[1];
  
	// Keep walking in this direction until we cannot do so or we go to distance
	while ((distance > 0)  && (nextvert != lastvert) && (nextvert != -1)) 
	{
		distance--;
      
		pListHead = m_ppPolFaces[nextvert];
		temp2 = (FFaces *) PeekList(pListHead,LISTHEAD,0);
		temp2->seen = val;
      
		if (temp2->seen2 == val)
		{
			last_seen++;
			return (original_distance - distance);
		}
		temp2->seen2 = val;
		numverts = temp2->nPolSize;
		if (numverts != 4)
			nextvert = -1;
		else if ((!first) && (!(Check_Right(last_seen,temp2,y,nextvert))))
		{
			last_seen++;
			return (original_distance - distance);
		}
		else 
		{
			// Find edge that is not adjacent to the previous one
			counter = 0;
			flag = true;
			while ((counter < 3) && (flag))
			{
				if (((*(temp2->pPolygon+counter)   == previous_edge1)  ||
						 (*(temp2->pPolygon+counter+1) == previous_edge2)) ||
						((*(temp2->pPolygon+counter)   == previous_edge2)  ||
						 (*(temp2->pPolygon+counter+1) == previous_edge1)) )
					counter++;
				else
					flag = false;
			}
			// Get the IDs of the next edge
			if (counter < 3) 
			{
				previous_edge1 = *(temp2->pPolygon + counter);
				previous_edge2 = *(temp2->pPolygon + counter + 1);
			} 
			else 
			{
				previous_edge1 = *(temp2->pPolygon + counter);
				previous_edge2 = *(temp2->pPolygon);
			}
			if (((*(temp2->walked+counter) == -1) && (*(temp2->walked+counter+2) == -1))) 
			{
				int1 ErrorInfo[120];
				sprintf(ErrorInfo, "There is an error in the walks!\n");
				sprintf(ErrorInfo, "1Code %d %d \n",*(temp2->walked+counter),
									 				 *(temp2->walked+counter+2));
				throw ex::AbortDataProcessingException(LogRec( "ViaStrip", "Update_and_Test", "There is an error in the walks!" ));
			} 
			else 
			{
				if ((*(temp2->walked+counter) == -1) && (*(temp2->walked+counter-2) == -1))
				{
					int1 ErrorInfo[120];
					sprintf(ErrorInfo, "There is an error in the walks!\n");
					sprintf(ErrorInfo, "2Code %d %d \n",*(temp2->walked+counter), *(temp2->walked+counter-2));
					throw ex::AbortDataProcessingException(LogRec("ViaStrip", "Update_and_Test", "There is an error in the walks!"));
				}
			}
			node = *(temp2->VertandId + counter);
			y = counter;
			if (node->edge[1] == nextvert)
				nextvert = node->edge[2];
			else
				nextvert = node->edge[1];
		}
	}
	last_seen++;
	if  (distance != 0) 
	{
		if (((nextvert == -1) || (nextvert == lastvert)) && (distance != 1))
			return (original_distance - distance);
	}
	return original_distance;
}

/**
 *	if first time, then just update the last seen field
 */
int4 ViaStrip::Test_Adj(FFaces * temp2,int4 x,int4 north,int4 distance,int4 lastvert, int4 value)
{
	// if first time, then just update the last seen field
	if (x==1)
		return(Update_and_Test(temp2,north,true,distance,lastvert,value));
	else // else we have to check if we are adjacent to the last strip *
		return(Update_and_Test(temp2,north,false,distance,lastvert,value));
}

/**
 *	find the maximum face
 */
int4 ViaStrip::Find_Max(FFaces * temp2,int4 lastvert,int4 north,int4 left,
					int4 *lastminup,int4 *lastminleft)
{
	int4 temp,walk,counter,minup,x,band_value;
	int4 previous_edge1, previous_edge2;
	FEdges * node;
	ListHead * pListHead;
	bool flag;	
	static int4 last_seen = 0;
	register int4 smallest_so_far,nextvert,max=-1;         
  
	*lastminup = MAX_BAND;
	*lastminleft = 1;
  
	if (left == 3) 
	{
		previous_edge1 = *(temp2->pPolygon + left);
		previous_edge2 = *(temp2->pPolygon);
	} 
	else 
	{
		previous_edge1 = *(temp2->pPolygon + left + 1);
		previous_edge2 = *(temp2->pPolygon + left);
	}
  
	temp2->seen = last_seen;
	walk = *(temp2->walked + left);
	
	for (x=1;x<=(walk+1); x++) 
	{
		// test to see if we have a true band that is, are they adjacent to each other
		minup = *(temp2->walked + north) + 1;
      
		// if we are at the very first face, then we do not
		// have to check the adjacent faces going up
		// and our north distance is the distance of this face's north direction. 
		if (x == 1) 
		{
			*lastminup = minup;
			minup = Test_Adj(temp2,x,north,*lastminup,lastvert,last_seen);
			*lastminup = minup;
			smallest_so_far = minup;	
		}
		// find the largest band that we can have
		if (minup < (*lastminup)) {
			// see if we really can go up all the way 
			// temp should by less than our equal to minup
			// if it is less, then one of the faces was not adjacent to
			// those next to it and the band height will be smaller
			temp = Test_Adj(temp2,x,north,minup,lastvert,last_seen);
			if (temp > minup) 
			{
				throw ex::AbortDataProcessingException(LogRec("ViaStrip", "Find_Max", "there is an error in the test adj." ));
			}
			minup = temp;
			band_value = x * minup;
			if (minup < smallest_so_far)
			{
				if (band_value > max) 
				{
					smallest_so_far = minup;
					*lastminup = minup;
					*lastminleft = x;
					max = band_value;
				} 
				else
					smallest_so_far = minup;
			} 
			else 
			{
				band_value = x * smallest_so_far;
				if (band_value > max) 
				{
					*lastminup = smallest_so_far;
					*lastminleft = x;
					max = band_value;
				}
			}
		}
		else 
		{
			if (x != 1) 
			{
				temp = Test_Adj(temp2,x,north,smallest_so_far,lastvert,last_seen);
				if (temp > smallest_so_far) 
				{
					throw ex::AbortDataProcessingException(LogRec("ViaStrip", "Find_Max", "there is an error in the test adj." ));
				}
				smallest_so_far = temp;
			}
			band_value = x * smallest_so_far; 
			if (band_value > max) 
			{
				*lastminup = smallest_so_far;
				*lastminleft = x;
				max = band_value;
			}
		}
		if ( x != (walk + 1)) 
		{
			node = *(temp2->VertandId+left);
			if (lastvert == node->edge[1])
				nextvert = node->edge[2];
			else
				nextvert = node->edge[1];
	  
			lastvert = nextvert;
	  	if (nextvert == -1) return max;
	  
			pListHead = m_ppPolFaces[nextvert];
			temp2 = (FFaces *) PeekList(pListHead, LISTHEAD, 0);
	  
			// if we have visited this face before, then there is an error
			if (((*(temp2->walked) == -1) && (*(temp2->walked+1) == -1) &&
				(*(temp2->walked+2) == -1) && (*(temp2->walked+3) == -1))
				|| (temp2->nPolSize !=4) || (temp2->seen == last_seen))
			{
				if (lastvert == node->edge[1])
					nextvert = node->edge[2];
				else
					nextvert = node->edge[1];
				if (nextvert == -1)
					return max;
				lastvert = nextvert;
				// Last attempt to get the face ... 
				pListHead = m_ppPolFaces[nextvert];
				temp2 = (FFaces *) PeekList(pListHead, LISTHEAD, 0);
				if (((*(temp2->walked) == -1) && (*(temp2->walked+1) == -1) &&
					(*(temp2->walked+2) == -1) && (*(temp2->walked+3) == -1))
					|| (temp2->nPolSize !=4) || (temp2->seen == last_seen))
					return max;    
				// The polygon was not saved with the edge, not enough room. 
				// We will get the walk when we come to that polygon later.
			} 
			else 
			{
				counter = 0;
				flag = true;
				temp2->seen = last_seen;
				while ((counter < 3) && (flag))
				{
					if ( ((*(temp2->pPolygon+counter) == previous_edge1) ||
						(*(temp2->pPolygon+counter+1) == previous_edge2)) ||
						((*(temp2->pPolygon+counter) == previous_edge2) ||
						(*(temp2->pPolygon+counter+1) == previous_edge1)) )
						counter++;
					else
						flag = false;
				}
			}
			// Get the IDs of the next edge
			left = counter;
			north = left+1;
			if (left ==3)
				north = 0;	
			if (counter < 3) 
			{
				previous_edge1 = *(temp2->pPolygon + counter + 1);
				previous_edge2 = *(temp2->pPolygon + counter);
			} 
			else 
			{
				previous_edge1 = *(temp2->pPolygon + counter);
				previous_edge2 = *(temp2->pPolygon);
			}
		} // endif (x!=(walk+1))
	} // endfor (x-loop)
	last_seen++;
	return max; 
}

/**
 *	mark a face
 */
void ViaStrip::Mark_Face(FFaces * temp2, int4 color1, int4 color2,
		      int4 color3, bool end, int4 *edge1, int4 *edge2, 
		      int4 *face_id, int4 norms, bool texture, std::ofstream & band)
{
	static int4 last_quad[4];
	register int4 x,y,z=0;
	int4 saved[2];
	static int4 output1, output2,last_id;
	bool cptexture;

	int4 bandBuf;
  
	// Are we done with the patch? If so return the last edge that
	// we will come out on, and that will be the edge that we will
	// start to extend upon.
	cptexture = texture;
	if (end) 
	{
		*edge1 = output1;
		*edge2 = output2;
		*face_id = last_id;
		return;
	}
  
	last_id = *face_id;
	*(temp2->walked) = -1;
	*(temp2->walked+1) = -1;
	*(temp2->walked+2) = -1;
	*(temp2->walked+3) = -1;
	added_quad++;
	temp2->nPolSize = 1;
  
	if (patch == 0) 
	{
		// At the first quad in the strip -- save it
		last_quad[0] = *(temp2->pPolygon);
		last_quad[1] = *(temp2->pPolygon+1);
		last_quad[2] = *(temp2->pPolygon+2);
		last_quad[3] = *(temp2->pPolygon+3);
		patch++;
	}
	else 
	{ // Now we have a triangle to output, find the edge in common
		for (x=0; x < 4 ;x++)
		{
			for (y=0; y< 4; y++) 
			{
				if (last_quad[x] == *(temp2->pPolygon+y)) 
				{
					saved[z++] = last_quad[x];               
					if (z > 2)
					{ 
						// This means that there was a non convex or an overlapping polygon
						z--;
						break;
					}
				}
			}
		}
		if (z != 2) 
		{
			int1 ErrorInfo[200];
			sprintf(ErrorInfo, "Z is not 2 %d \n",patch);
			sprintf(ErrorInfo, "4 %d %d %d %d %d %d %d\n",*(temp2->pPolygon),
				*(temp2->pPolygon+1),*(temp2->pPolygon+2),*(temp2->pPolygon+3), color1,color2,color3);
			sprintf(ErrorInfo, "%d %d %d %d\n",last_quad[0],last_quad[1], last_quad[2],last_quad[3]);
			throw ex::AbortDataProcessingException(LogRec("ViaStrip", "Mark_Face", "Z is not 2." ));
		}
		if (patch == 1)
		{
			// First one to output, there was no output edge
			patch++;
			x = Adjacent(saved[0],saved[1],last_quad,4);
			y = Adjacent(saved[1],saved[0],last_quad,4);
	  
			// Data might be mixed and we do not have textures
			// for some of the vertices
			if ((texture) && (((m_pVT[x]) == 0) ||
					((m_pVT[y])==0) || ((m_pVT[saved[1]])==0)))
				cptexture = false;
	  
			if ((!norms) && (!cptexture))
			{
				// 'band' output.
				band << "\nt";

				if (m_iOrient)		// If we want to preserve normal orientation
					PreserveStripOrientation(x+1,y+1,saved[1]+1, band);
				
				// 'band' output.
				band << ' ';
				bandBuf = x+1;
				band.write((int1 *) &bandBuf, sizeof(int4));

				band << ' ';
				bandBuf = y+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
			
				band << ' ';
				bandBuf = saved[1]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));

				band << ' ';
				bandBuf = saved[0]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
			} 
			else if ((norms) && (!cptexture)) 
			{
				// 'band' output.
				band << "\nt";

				if (m_iOrient) // If we want to preserve normal orientation
					PreserveStripOrientationWithNormal(x+1, m_pVN[x]+1, 
							y+1,m_pVN[y]+1, saved[1]+1, m_pVN[saved[1]]+1, band);
              
				// 'band' output.
				band << ' ';
				bandBuf = x+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "//";
				bandBuf = m_pVN[x]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));

				band << ' ';
				bandBuf = y+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "//";
				bandBuf = m_pVN[y]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));

				band << ' ';
				bandBuf = saved[1]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "//";
				bandBuf = m_pVN[saved[1]]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));

				band << ' ';
				bandBuf = saved[0]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "//";
				bandBuf = m_pVN[saved[0]]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
			} 
			else if ((cptexture) && (!norms)) 
			{
				// 'band' output.
				band << "\nt";
				
				if (m_iOrient) // If we want to preserve normal orientation
					PreserveStripOrientationWithTexture(
						x+1, m_pVT[x]+1, 
						y+1, m_pVT[y]+1,	
						saved[1]+1, m_pVT[saved[1]]+1, band);

				// 'band' output.
				band << ' ';
				bandBuf = x+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVT[x]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));

				band << ' ';
				bandBuf = y+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVT[y]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));

				band << ' ';
				bandBuf = saved[1]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVT[saved[1]]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));

				band << ' ';
				bandBuf = saved[0]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "//";
				bandBuf = m_pVT[saved[0]]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
			}
			else 
			{
				// 'band' output.
				band << "\nt";

				if (m_iOrient)		// If we want to preserve normal orientation
					PreserveStripOrientationWithTextureAndNormal(
								   x+1, m_pVT[x]+1, m_pVN[x]+1, y+1, m_pVT[y]+1, m_pVN[y]+1,
								   saved[1]+1, m_pVT[saved[1]]+1, m_pVN[saved[1]]+1, 
									 band);

				// 'band' output.
				band << ' ';
				bandBuf = x+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVT[x]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVN[x]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));

				band << ' ';
				bandBuf = y+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVT[y]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVN[y]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));

				band << ' ';
				bandBuf = saved[1]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVT[saved[1]]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVN[saved[1]]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));

				band << ' ';
				bandBuf = saved[0]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVT[saved[0]]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVN[saved[0]]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
			}
			x = Adjacent(saved[0],saved[1],temp2->pPolygon,4);
			y = Adjacent(saved[1],saved[0],temp2->pPolygon,4);
	  
			// Data might be mixed and we do not have textures for some of the vertices 
			if ((texture) && ( (m_pVT[x] == 0) || (m_pVT[y]==0)))
			{
				if (cptexture) {
					// 'band' output.
					band << "\nq";
				}
				cptexture = false;
			}
			if ((!norms) && (!cptexture)) 
			{
				// 'band' output.
				band << ' ';
				bandBuf = x+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << ' ';
				bandBuf = y+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
			} 
			else if ((norms) && (!cptexture))
			{
				// 'band' output.
				band << ' ';
				bandBuf = x+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "//";
				bandBuf = m_pVN[x]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));

				band << ' ';
				bandBuf = y+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "//";
				bandBuf = m_pVN[y]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
			} 
			else if ((cptexture) && (!norms)) 
			{
				// 'band' output.
				band << ' ';
				bandBuf = x+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVT[x]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));

				band << ' ';
				bandBuf = y+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVT[y]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
			} 
			else 
			{
				// 'band' output.
				band << ' ';
				bandBuf = x+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVT[x]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVN[x]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));

				band << ' ';
				bandBuf = y+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVT[y]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVN[y]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
			}
			output1 = x;
			output2 = y;
		} 
		else 
		{
			x = Adjacent(output2,output1,temp2->pPolygon,4);
			y = Adjacent(output1,output2,temp2->pPolygon,4);
			// Data might be mixed and we do not have textures for some of the vertices
			if ((texture) && ( ((m_pVT[x]) == 0) || ((m_pVT[y])==0) ))
				texture = false;
	  
			if ((!norms) && (!texture)) 
			{
				// 'band' output.
				band << "\nq ";
				bandBuf = x+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << " ";
				bandBuf = y+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
			}
			else if ((norms) && (!texture))
			{
				// 'band' output.
				band << "\nq ";
				bandBuf = x+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "//";
				bandBuf = m_pVN[x]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));

				band << " ";
				bandBuf = y+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "//";
				bandBuf = m_pVN[y]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
			} 
			else if ((texture) && (!norms)) 
			{
				// 'band' output.
				band << "\nq ";
				bandBuf = x+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVT[x]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));

				band << " ";
				bandBuf = y+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVT[y]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
			} 
			else 
			{
				// 'band' output.
				band << "\nq ";
				bandBuf = x+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVT[x]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVN[x]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));

				band << " ";
				bandBuf = y+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVT[y]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
				band << "/";
				bandBuf = m_pVN[y]+1;
				band.write((int1 *) &bandBuf, sizeof(int4));
			}
			output1 = x;
			output2 = y;
		}
		last_quad[0] = *(temp2->pPolygon);
		last_quad[1] = *(temp2->pPolygon+1);
		last_quad[2] = *(temp2->pPolygon+2);
		last_quad[3] = *(temp2->pPolygon+3);
	} // endelse (patch!=0)
}

/**
 *	Go back and do the walk again, but this time save the lengths inside the data structure.
 */
void ViaStrip::AssignWalk(int4 lastvert,FFaces * temp2, int4 front_walk,int4 y, int4 back_walk)
{
	// Go back and do the walk again, but this time save the lengths inside the data structure.
	// y was the starting edge number for the front_walk length
	// back_walk is the length of the walk along the opposite edge

	int4 previous_edge1, previous_edge2;
	register int4 walk = 0,nextvert,numverts,counter;
	bool flag;
	FEdges * node;
	ListHead * pListHead;
	static int4 seen = 0;
	static bool first = true;         
	bool wrap = false, set = false;              
  
	// In the "Fast_Reset" resetting will be true
	if ((resetting) && (first)) 
	{
		seen = 0;
		first = false;
	}
	seen++;

	// Had a band who could be a cycle
	if (front_walk == back_walk)
		wrap = true;
  
	// Find the edge that we are currently on
	if (y != 3) 
	{
		previous_edge1 = *(temp2->pPolygon +y);
		previous_edge2 = *(temp2->pPolygon + y + 1);
	} 
	else 
	{
		previous_edge1 = *(temp2->pPolygon +y);
		previous_edge2 = *(temp2->pPolygon);
	}

	// Assign the lengths
	if (y < 2) 
	{
		*(temp2->walked+y) = front_walk--;
		*(temp2->walked+y+2) = back_walk++;
	} 
	else 
	{
		*(temp2->walked+y) = front_walk--;
		*(temp2->walked+y-2) = back_walk++;
	}
  
	// Find the adjacent face to this edge
	node = *(temp2->VertandId+y);
	if (node->edge[2] != lastvert)
		nextvert = node->edge[2];
	else
		nextvert = node->edge[1];
  
	temp2->seen3 = seen;
  
	// Keep walking in this direction until we cannot do so
	while ((nextvert != lastvert) && (nextvert != -1) && (front_walk >= 0)) 
	{
		walk++;
		pListHead = m_ppPolFaces[nextvert];
		temp2 = (FFaces *) PeekList(pListHead,LISTHEAD,0);
		numverts = temp2->nPolSize;
		if ((numverts != 4)) 
		{
			nextvert = -1;
			// Don't include this face in the walk *
			walk--;
		}
		else 
		{	// Find edge that is not adjacent to the previous one
			counter = 0;
			flag = true;
			while ((counter < 3) && (flag)) 
			{
				if (((*(temp2->pPolygon+counter) == previous_edge1) ||
					(*(temp2->pPolygon+counter+1) == previous_edge2)) ||
					((*(temp2->pPolygon+counter) == previous_edge2) ||
					(*(temp2->pPolygon+counter+1) == previous_edge1)))
					counter++;
				else
					flag = false;
			}
			// Get the IDs of the next edge
			if (counter < 3) 
			{
				previous_edge1 = *(temp2->pPolygon + counter);
				previous_edge2 = *(temp2->pPolygon + counter + 1);
			}
			else 
			{
				previous_edge1 = *(temp2->pPolygon + counter);
				previous_edge2 = *(temp2->pPolygon);
			}
			// Put in the walk lengths
			if (counter < 2)
			{
				if (((*(temp2->walked + counter) >= 0)
				   || (*(temp2->walked +counter + 2) >= 0))) 
				{
					if ((resetting == false) && ((temp2->seen3) != (seen-1))) 
					{
						// If there are more than 2 polygons adjacent
						// to an edge then we can be trying to assign more than
						// once. We will save the smaller one
						temp2->seen3 = seen;
						if ((*(temp2->walked+counter) <= front_walk) &&
							(*(temp2->walked+counter+2) <= back_walk) )
							return;
						if (*(temp2->walked+counter) > front_walk)
							*(temp2->walked+counter) = front_walk--;
						else
							front_walk--;
						if (*(temp2->walked+counter+2) > back_walk)
							*(temp2->walked+counter+2) = back_walk++;
						else
							back_walk++;
					} 
					else if (resetting == false) 
					{
						// if there was a cycle then all lengths are the same
						walk--;
						back_walk--;
						front_walk++;
						temp2->seen3 = seen;
						*(temp2->walked+counter) = front_walk--;
						*(temp2->walked+counter+2) = back_walk++;
					} 
					else if (((temp2->seen3 == (seen-1)) && (wrap) && (walk == 1)) || (set)) 
					{
						// if there was a cycle then all lengths are the same
						set = true;
						walk--;
						back_walk--;
						front_walk++;
						temp2->seen3 = seen;
						*(temp2->walked+counter) = front_walk--;
						*(temp2->walked+counter+2) = back_walk++;
					} 
					else 
					{
						temp2->seen3 = seen;
						*(temp2->walked+counter) = front_walk--;
						*(temp2->walked+counter+2) = back_walk++;
					}
				} // if was > 0
				else 
				{
					temp2->seen3 = seen;
					*(temp2->walked+counter) = front_walk--;
					*(temp2->walked+counter+2) = back_walk++;
				}
			} // end-else : count<2
			else 
			{
				if (((*(temp2->walked + counter) >= 0 )	|| (*(temp2->walked +counter - 2) >= 0)) ) 
				{
					if ((temp2->seen3 != (seen-1))  && (resetting == false)) 
					{
						// If there are more than 2 polygons adjacent
						// to an edge then we can be trying to assign more than
						// once. We will save the smaller one
						temp2->seen3 = seen;
						if ( (*(temp2->walked+counter) <= front_walk) &&
							(*(temp2->walked+counter-2) <= back_walk) )
							return;
						if (*(temp2->walked+counter) > front_walk)
							*(temp2->walked+counter) = front_walk--;
						else
							front_walk--;
						if (*(temp2->walked+counter-2) > back_walk)
							*(temp2->walked+counter-2) = back_walk++;
						else
							back_walk++;
					} 
					else if (resetting == false) 
					{
						walk--;
						back_walk--;
						front_walk++;
						temp2->seen3 = seen;
						*(temp2->walked+counter) = front_walk--;
						*(temp2->walked+counter-2) = back_walk++;
					} 
					else if (((temp2->seen3 == (seen-1)) && (walk == 1) && (wrap)) || (set)) 
					{
						// if there was a cycle then all lengths are the same
						set = true;
						walk--;
						back_walk--;
						front_walk++;
						temp2->seen3 = seen;
						*(temp2->walked+counter) = front_walk--;
						*(temp2->walked+counter-2) = back_walk++;
					} 
					else 
					{
						temp2->seen3 = seen;
						*(temp2->walked+counter) = front_walk--;
						*(temp2->walked+counter-2) = back_walk++;
					}
				} 
				else 
				{
					temp2->seen3 = seen;
					*(temp2->walked+counter) = front_walk--;
					*(temp2->walked+counter-2) = back_walk++;
				}
			}
			if (nextvert != -1) 
			{
				node = *(temp2->VertandId + counter);
				if (node->edge[1] == nextvert)
					nextvert = node->edge[2];
				else
					nextvert = node->edge[1];
			}
		} // end-else : Find edge that is not adjacent to the previous one
	} // endwhile
	if ((EVEN(seen))) seen+=2; 
}

/**
 *	reset
 */
void ViaStrip::Fast_Reset(int4 x)
{
	register int4 y,numverts;
	register int4 front_walk, back_walk;
	ListHead * pListHead;
	FFaces * temp = NULL;
  
	pListHead = m_ppPolFaces[x];
	temp = (FFaces *) PeekList(pListHead,LISTHEAD,0);
	numverts = temp->nPolSize;
  
	front_walk = 0; 
	back_walk = 0;          
	resetting = true;
  
	// we are doing this only for quads
	if (numverts == 4) 
	{
		// for each face not seen yet, do North and South together
		// and East and West together
		for (y=0; y<2; y++) 
		{
			// Check if the opposite sides were seen already
			// Find walk for the first edge
			front_walk = CalculateWalks(x,y,temp);
			// Find walk in the opposite direction
			back_walk = CalculateWalks(x,y+2,temp);
			// Now put into the data structure the numbers that we have found
	  
			AssignWalk(x,temp,front_walk,y,back_walk);
			AssignWalk(x,temp,back_walk,y+2,front_walk);
		}
	}
	resetting = false;
}

/**
 *	reset
 */
void ViaStrip::Reset_Max(FFaces * temp2, int4 face_id, int4 north, int4 last_north,
		      int4 orientation, int4 last_left, int4 color1, int4 color2, int4 color3, 
					bool start, std::ofstream & band)
{
	int4 previous_edge1,previous_edge2;
	FEdges * node;
	ListHead * pListHead;
	int4 f,t,nextvert,counter;
	bool flag;
  
	// Reset walks on faces, since we just found a patch
	if (orientation !=3) 
	{
		previous_edge1 = *(temp2->pPolygon + orientation+1);
		previous_edge2 = *(temp2->pPolygon + orientation );
	} 
	else 
	{
		previous_edge1 = *(temp2->pPolygon + orientation );
		previous_edge2 = *(temp2->pPolygon);
	}
  
	// only if we are going left, otherwise there will be -1 there
	// Find the adjacent face to this edge
	for (t = 0; t <=3 ; t++) 
	{
		node = *(temp2->VertandId+t);
		if (face_id == node->edge[1])
			f = node->edge[2];
		else
			f = node->edge[1];

		if (f != -1) Fast_Reset(f);
	}
	node = *(temp2->VertandId+orientation);
	if (face_id == node->edge[1])
		nextvert = node->edge[2];
	else
		nextvert = node->edge[1];
  
	while ((last_left--) > 1) 
	{
		if (start)
			Reset_Max(temp2,face_id,orientation,last_left,north,last_north,
				color1,color2,color3,false, band);
		face_id = nextvert;
		pListHead = m_ppPolFaces[nextvert];                
		temp2 = (FFaces *) PeekList(pListHead,LISTHEAD,0);
		if ((temp2->nPolSize != 4) && (temp2->nPolSize != 1)) 
		{
			// There is more than 2 polygons on the edge, and we 
			// could have gotten the wrong one
			if (nextvert != node->edge[1])
				nextvert = node->edge[1];
			else
				nextvert = node->edge[2];
			pListHead = m_ppPolFaces[nextvert];          
			temp2 = (FFaces *) PeekList(pListHead,LISTHEAD,0);
			node = *(temp2->VertandId+orientation);
		}
		if (!start) 
		{
			for (t = 0; t <=3 ; t++)
			{
				node = *(temp2->VertandId+t);
				if (face_id == node->edge[1])
					f = node->edge[2];
				else
					f = node->edge[1];
				if (f != -1) Fast_Reset(f);
			}
		}
		counter = 0;
		flag = true;
		while ((counter < 3) && (flag))
		{
			if ( ((*(temp2->pPolygon+counter) == previous_edge1) ||
				(*(temp2->pPolygon+counter+1) == previous_edge2)) ||
				((*(temp2->pPolygon+counter) == previous_edge2) ||
				(*(temp2->pPolygon+counter+1) == previous_edge1)) )
				counter++;
			else
				flag = false;
		}
      
		// Get the IDs of the next edge
		if (counter < 3)
		{
			previous_edge1 = *(temp2->pPolygon + counter+1);
			previous_edge2 = *(temp2->pPolygon + counter);
		} 
		else 
		{
			previous_edge1 = *(temp2->pPolygon + counter);
			previous_edge2 = *(temp2->pPolygon);
		}
		orientation = counter;
		node = *(temp2->VertandId + counter);
		if (node->edge[1] == nextvert)
			nextvert = node->edge[2];
		else
			nextvert = node->edge[1];
      
		if (!reversed) 
		{
			if (counter != 3)
				north = counter +1;
			else
				north = 0;
		} 
		else 
		{
			if (counter != 0)
				north = counter -1;
			else
				north = 3;
		}
	}
	if (start)
		Reset_Max(temp2,face_id,orientation,last_left,north,last_north,
			color1,color2,color3,false, band);
	else if (nextvert != -1)       
		Fast_Reset(nextvert);
}

/**
 *	peel the patch from the model to find the max face
 */
int4 ViaStrip::Peel_Max(FFaces * temp2,int4 face_id,int4 north,int4 last_north, int4 orientation,
		    int4 last_left, int4 color1,int4 color2,int4 color3,
		    bool start, int4 *swaps_added, int4 norms, bool texture,
				std::ofstream & band)
{
	int4 end1,end2, last_id, s=0, walk = 0;
	int4 previous_edge1, previous_edge2;
	int4 static last_seen = 1000;
	FEdges * node;
	ListHead * pListHead;
	int4 nextvert,numverts,counter,dummy,tris=0;
	bool flag;
  
	// Peel the patch from the model.
	// We will try and extend off the end of each strip in the patch. We will return
	// the number of triangles completed by this extension only, and the number of
	// swaps in the extension only.
  
	patch = 0;
	if (orientation !=3) 
	{
		previous_edge1 = *(temp2->pPolygon + orientation+1);
		previous_edge2 = *(temp2->pPolygon + orientation );
	} 
	else 
	{
		previous_edge1 = *(temp2->pPolygon + orientation );
		previous_edge2 = *(temp2->pPolygon);
	}
	
	walk = *(temp2->walked + orientation);
	// only if we are going left, otherwise there will be -1 there
	if ((start) && ((walk+1) < last_left)) 
  {
    int1 ErrorInfo[100];
    sprintf(ErrorInfo, "Create strips: there is an error in the left %d %d\n",walk,last_left);
    throw ex::AbortDataProcessingException(LogRec("ViaStrip", "Peel_Max", "There is an error in the left." ));
  }
  
	// Find the adjacent face to this edge
	node = *(temp2->VertandId+orientation);
	if (face_id == node->edge[1])
		nextvert = node->edge[2];
	else
		nextvert = node->edge[1];
	
	temp2->seen = last_seen;
	
	while ((last_left--) > 1) 
	{
		if (start)
			tris += Peel_Max(temp2,face_id,orientation,last_left,north,last_north,
				color1,color2,color3,false,swaps_added,norms,texture, band); 
		else
			Mark_Face(temp2,color1,color2,color3, false,
				&dummy,&dummy,&face_id,norms,texture, band);
      
		pListHead = m_ppPolFaces[nextvert];      
		temp2 = (FFaces *) PeekList(pListHead,LISTHEAD,0);
		numverts = temp2->nPolSize;
      
		if ((numverts != 4) || (temp2->seen == last_seen) ||  (nextvert == -1)) 
		{
			// There is more than 2 polygons on the edge, and we could 
			// have gotten the wrong one
	 		if (nextvert != node->edge[1])
				nextvert = node->edge[1];
			else
				nextvert = node->edge[2];
			pListHead = m_ppPolFaces[nextvert];
			temp2 = (FFaces *) PeekList(pListHead,LISTHEAD,0);
			numverts = temp2->nPolSize;
			if ((numverts != 4) || (temp2->seen == last_seen)) 
      {
        int1 ErrorInfo[100];
        sprintf(ErrorInfo, "Peel 2 %d\n",numverts);
        throw ex::AbortDataProcessingException(LogRec("ViaStrip", "Peel_Max", "Peel 2 error."));
      }
		}
		face_id = nextvert;
		temp2->seen = last_seen;
		counter = 0;
		flag = true;
		while ((counter < 3) && (flag)) 
		{
			if ( ((*(temp2->pPolygon+counter) == previous_edge1) ||
				(*(temp2->pPolygon+counter+1) == previous_edge2)) ||
				((*(temp2->pPolygon+counter) == previous_edge2) ||
				(*(temp2->pPolygon+counter+1) == previous_edge1)) )
				counter++;
			else
				flag = false;
		} // end-while.
		// Get the IDs of the next edge
		if (counter < 3) 
		{
			previous_edge1 = *(temp2->pPolygon + counter+1);
			previous_edge2 = *(temp2->pPolygon + counter);
		}
		else 
		{
			previous_edge1 = *(temp2->pPolygon + counter);
			previous_edge2 = *(temp2->pPolygon);
		}
		orientation = counter;
      
		node = *(temp2->VertandId + counter);
		if (node->edge[1] == nextvert)
			nextvert = node->edge[2];
		else
			nextvert = node->edge[1];
      
		if (!reversed) 
		{
			if (counter != 3)
				north = counter +1;
			else
				north = 0;
		} 
		else
		{
			if (counter != 0)
				north = counter -1;
			else
				north = 3;
		}
	} // endwhile
	if (start)
		tris += Peel_Max(temp2,face_id,orientation,last_left,north,last_north,
			     color1,color2,color3,false,swaps_added,norms,texture, band);	
	else
		Mark_Face(temp2,color1,color2,color3, false,
	      &dummy,&dummy,&face_id,norms,texture, band);	// do the last face
  
	last_seen++;
  
	// Get the edge that we came out on the last strip of the patch
	Mark_Face(NULL,0,0,0, true,&end1,&end2,&last_id,norms,texture, band);
	tris += ExtendFace(last_id,end1,end2,&s, color1,color2,color3,
		      m_pVN, norms, m_pVT, texture, band);
	*swaps_added = *swaps_added + s;
	
	return tris;
}

/** 
 *	find bands
 */
/*void ViaStrip::FindBands(int4 numfaces, int4 *swaps, int4 *bands, 
				int4 *cost, int4 *tri, int4 norms, int4 *vert_norms, bool texture,
				int4 *vert_texture, std::ofstream & band)
{
	register int4 x,y,max1,max2,numverts,face_id,flag,maximum = 25;
	ListHead *pListHead;
	FFaces * temp = NULL;
	int4 color1 = 0, color2 = 100, color3 = 255;
	int4 smaller;
	int4 north_length1,last_north,left_length1,last_left,north_length2,left_length2;
	int4 total_tri = 0, total_swaps = 0,last_id;
	int4 end1, end2, s=0;
	register int4 cutoff = 20;
  
	// Code that will find the patches. "Cutoff" will be
	// the cutoff of the area of the patches that we will be allowing. After
	// we reach this cutoff length, then we will run the local algorithm on the
	// remaining faces.
  
	// For each faces that is left find the largest possible band that we can
	// have with the remaining faces. Note that we will only be finding patches
	// consisting of quads.

	m_pVN = vert_norms;
	m_pVT = vert_texture;
	y = 1;
	*bands = 0;

	while ((maximum >= cutoff)) 
	{
		y++;
		maximum = -1;
		for (x=0; x<numfaces; x++) 
		{ 
			// Used to produce the triangle strips for each face, get the face
			pListHead = m_ppPolFaces[x];
			temp = (FFaces *) PeekList(pListHead,LISTHEAD,0);
			numverts = temp->nPolSize;
	  
			// we are doing this only for quads
			if (numverts == 4) 
			{
				// We want a face that is has not been used yet,
				// since we know that that face must be part of
				// a band. Then we will find the largest band that
				// the face may be contained in
	      
				//  Doing the north and the left
				if ((*(temp->walked) != -1) && (*(temp->walked+3) != -1))
					max1 = Find_Max(temp,x,0,3,&north_length1,&left_length1);
				if ((*(temp->walked+1) != -1) && (*(temp->walked+2) != -1))
					max2 = Find_Max(temp,x,2,1,&north_length2,&left_length2);
				if ((max1 != (north_length1 * left_length1)) ||
					(max2 != (north_length2 * left_length2))) 
				{
					int1 ErrorInfo[100];
					sprintf(ErrorInfo, "Max1 %d, %d %d	Max2 %d, %d %d\n",
						max1,north_length1,left_length1,max2, north_length2,left_length2);
					throw ex::AbortDataProcessingException(LogRecord(LogRecord::SL_EXCEPTION, Date::GetCurrentDate()
            __FILE__, 1.16, __LINE__,"ViaStrip", "FindBands", UNDEFINED,
            "max error."));
				}
				if ((max1 > max2) && (max1 > maximum)) 
				{
					maximum = max1;
					face_id = x;
					flag = 1; 
					last_north = north_length1;
					last_left = left_length1;
					// so we know we saved max1
				} 
				else if ((max2 > maximum))
				{
					maximum = max2;
					face_id = x;
					flag = 2; 
					last_north = north_length2;
					last_left = left_length2;
					// so we know we saved max2
				}
			}
		} // endfor (x)
		if ((maximum < cutoff) && (*bands == 0))
			return;
		pListHead = m_ppPolFaces[face_id];
		temp = (FFaces *) PeekList(pListHead,LISTHEAD,0);	
		// There are no patches that we found in this pass
		if (maximum == -1)
			break;
      
		if (last_north > last_left)
		{
			smaller = last_left;
		} 
		else 
		{
			smaller = last_north;
		}
      
		if (flag == 1) 
		{
			if (last_north > last_left)
			{
				// go north sequentially 
				total_tri += Peel_Max(temp,face_id,0,last_north,3,last_left,
				    color1,color2,color3,true, &s,norms,texture, band);
				Reset_Max(temp,face_id,0,last_north,3,last_left,
						color1,color2,color3,true, band);
				total_swaps += s;
			} 
			else 
			{
				reversed = true;
				total_tri += Peel_Max(temp,face_id,3,last_left,0,last_north,
				    color1,color2,color3, true, &s,norms,texture, band);
				Reset_Max(temp,face_id,3,last_left,0,last_north,
					color1,color2,color3, true, band);
				reversed = false;
				total_swaps += s;
			}
	  
			// Get the edge that we came out on the last strip of the patch *
			Mark_Face(NULL,0,0,0, true,&end1,&end2,&last_id,norms,texture, band);
			total_tri += ExtendFace(last_id,end1,end2,&s,
					   color1,color2,color3,m_pVN,norms,m_pVT,texture, band);
			total_swaps += s;
		} 
		else 
		{
			if (last_north > last_left) 
			{
				total_tri += Peel_Max(temp,face_id,2,last_north,1,last_left,
						color1,color2,color3,true, &s,norms,texture, band); 
				Reset_Max(temp,face_id,2,last_north,1,last_left,
						color1,color2,color3,true, band); 
				total_swaps += s;
			} 
			else 
			{
				reversed = true;
				total_tri += Peel_Max(temp,face_id,1,last_left,2,last_north,
						color1,color2,color3,true,&s,norms,texture, band);
				Reset_Max(temp,face_id,1,last_left,2,last_north,
						color1,color2,color3,true, band);
				reversed = false;
				total_swaps += s;
			}
			// Get the edge that we came out on on the patch
			Mark_Face(NULL,0,0,0, true, &end1,&end2,&last_id,norms,texture, band);
			total_tri += ExtendFace(last_id,end1,end2,&s,
					   color1,color2,color3,m_pVN,norms,m_pVT,texture, band);
			total_swaps += s;
		}
      
		// Now compute the cost of transmitting this band, is equal to
		// going across the larger portion sequentially,
		// and swapping 3 times per other dimension
		total_tri += (maximum * 2);
		*bands = *bands + smaller;
	} // endwhile
  *cost = total_tri + total_swaps + *bands + *bands;
	*tri = total_tri;
	added_quad = added_quad * 4;
	*swaps = total_swaps;
} */

/**
 *	Put the polygons that are left into a data structure so that we can run the stripping code on it.
 */
/*void ViaStrip::SaveRest(int4 *numfaces)
{
	// Put the polygons that are left into a data structure so that we can run the stripping code on it.
	register int4 x,y=0,numverts;
	ListHead *pListHead;
	FFaces * temp=NULL;
  
	for (x=0; x<*numfaces; x++) 
	{ 
		// for each face, get the face
		pListHead = m_ppPolFaces[x];
		temp = (FFaces *) PeekList(pListHead,LISTHEAD,0);
		numverts = temp->nPolSize;
		// If we did not do the face before add it to data structure with new face id number
		if (numverts != 1) 
		{
			CopyFace(temp->pPolygon,numverts,y+1,temp->pNorms);
			y++;
		}
		// Used it, so remove it
		else
			RemoveList(pListHead,(ListInfo *) temp);
	}
	*numfaces = y;
} */

/**
 *	save the walks
 */
/*void ViaStrip::SaveWalks(int4 numfaces)
{
	int4 x,y,numverts;
	int4 front_walk, back_walk;
	ListHead *pListHead;
	FFaces * temp = NULL;
  
	for (x=0; x<numfaces; x++) 
	{ 
		// for each face, get the face
		pListHead = m_ppPolFaces[x];
		temp = (FFaces *) PeekList(pListHead,LISTHEAD,0);
		numverts = temp->nPolSize;
		front_walk = 0; 
		back_walk = 0;
      
		// we are finding patches only for quads
		if (numverts == 4) 
		{
			// for each face not seen yet, do North and South together
			// and East and West together
			for (y=0;y<2;y++) 
			{
				// Check if the opposite sides were seen already from another
				// starting face, if they were then there is no need to do 
				// the walk again
				if (((*(temp->walked+y) == -1) && (*(temp->walked+y+2) == -1)))
				{
					// Find walk for the first edge
					front_walk = CalculateWalks(x,y,temp);
					// Find walk in the opposite direction
					back_walk = CalculateWalks(x,y+2,temp);
					// Now put into the data structure the numbers that	we have found
          AssignWalk(x,temp,front_walk,y,back_walk);
          AssignWalk(x,temp,back_walk,y+2,front_walk);
				}
			}
		} // end-if(numverts==4)
	} // end-for
} */

// $Log: ViaNewpolve.C,v $
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
// Revision 1.21  2001/08/13 20:29:19  sachin
// changed exceptions ,warnings,debug and information
// according to the new logger class
//
// Revision 1.20  2001/07/25 16:08:39  lihong
// Defect Id: 599
// Added throw ex:: after the function name
//
// Revision 1.19  2001/07/24 23:06:51  lihong
// Defect ID: 599
// new format of exceptions
//
// Revision 1.18  2001/07/23 16:56:27  lihong
// Defect ID: 599
// replaced file version with "$Revision: 1.2 $"
//
// Revision 1.17  2001/07/20 23:07:10  lihong
// Defect ID: 599
// changed abort() to throw ex
//
// Revision 1.16  2001/05/21 00:22:53  dave
// uses & implement. of Error::DebugMsg, Warn, Abort, etc, gone
//
// Revision 1.15  2001/04/09 17:31:26  dmitry
// updated datatypes
//
// Revision 1.14  2001/04/02 19:18:20  binli
// code simplified!
//
// Revision 1.13  2001/03/27 19:49:51  binli
// code walkthrough: name convension
//
// Revision 1.12  2001/03/27 16:55:54  binli
// reflected new code standard.
//
// Revision 1.11  2001/03/23 20:41:20  binli
// code walkthrough
//
// Revision 1.10  2001/03/21 15:24:11  binli
// replaced all structs with classes.
//
// Revision 1.9  2001/02/28 21:01:59  binli
// 1> BOOL->bool
// 2> Error Info
//
// Revision 1.8  2000/12/08 21:16:41  binli
// removed code to write out 'text' triangle files.
//
// Revision 1.7  2000/12/08 16:56:18  binli
// changed the output of triangle files from text to binary.
// added header to triangle files.
//
// Revision 1.6  2000/11/07 18:37:31  sarang
// Removed _errors_ which gcc found.
// 1. BOOL was never defined
// 2. function definition and call mismatch (bool and int4 were not distinguished by VC++!!!)
//
// Revision 1.5  2000/11/06 23:57:41  binli
// copied the old version (from mixkit) back to replace 'polygon generation'  in lastest version.
// so as to make the triangle generation modular works.
//
// Revision 1.4  2000/11/02 02:18:26  ingmar
// made int4 texture -> bool texture
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
// 5     8/29/00 1:39p Antonio
// updated footer and header
// 
// *****************  Version 4  *****************
// User: Binli           Date:  8/24/00  Time: 12:30p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments to Class ViaStrip
// 
// *****************  Version 3  *****************
// User: Antonio         Date:  8/21/00  Time:  6:03p
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
// Created ViaNewpolve.C
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaNewpolve.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
// $Id: ViaNewpolve.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
