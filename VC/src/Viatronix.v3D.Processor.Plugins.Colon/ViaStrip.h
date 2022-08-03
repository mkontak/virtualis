// $Id: ViaStrip.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// File: ViaStrip.h
// Description: interface for the ViaStrip class.
//              This class will transfer polygons individually described
//              (.obj format as described in wavefront)
//              to triangle strips.
// Created: July 31, 2000
// Author: Freeware,
//      rewriten by Bin Li (binli@viatronix.net):  change it from C version to C++ version
//
// Complete History at bottom of file.


#ifndef VIA_STRIP_H
#define VIA_STRIP_H

const float8 PI = 3.1415926573;
const int4 MAX1 = 60;

class ViaStrip{
public:

	enum SWAPTYPE { ON, OFF };
	enum TRIANGULATIONOPTIONS { PARTIAL, WHOLE };
	enum TIEOPTIONS { _FIRST, _RANDOM, _ALTERNATE, _LOOK, _SEQUENTIAL };
	enum LISTMARK { LISTHEAD = 0, LISTTAIL = 1 };

	#define EVEN(x) (((x) & 1) == 0)
	#define PEEKFROMHEAD(lh, ind)   (PeekList((lh), LISTHEAD, (ind)))
	#define PEEKFROMTAIL(lh, ind)   (PeekList((lh), LISTTAIL, (ind)))
	#define EMPTYLIST(lh)           (((lh)->LHeaders[LISTHEAD] == NULL))
	#define NumOnList(lh)           (((lh)->NumList))
	#define GetNextNode(pli)        (((pli)->ListNode.Next))
	#define GetPrevNode(pli)        (((pli)->ListNode.Previous))

	typedef void * PVOID;

	class vert_added 
	{
	public:
		int4 num;
		int4 *normal;
	};

	class TriNode
	{
	public:
		void *Next;
		void *Previous;
	};

	class Strips  
	{
	public:
		TriNode ListNode;
		int4 face_id;
	};

	class ListInfo  
	{
	public:
		TriNode ListNode;       // link to the next Listinfo Structure
	};

	class ListHead  
	{
	public:
		ListInfo * LHeaders[2];
		int4 NumList;
	};

	// polverts.h
	class ADJACENCIES  
	{
	public:
		TriNode ListNode;
		int4 face_id;
	};
	
	class FVerts  
	{
	public:
		TriNode ListNode;
		int4 *pPolygon;
		int4 nPolSize;
		int4 nId;
	};

	class FEdges  
	{
	public:
		TriNode ListNode;
		int4 edge[3];
	};

	class FFaces  
	{
	public:
		TriNode ListNode;
		int4 *pPolygon, *pNorms;
		int4 seen, seen2, seen3;
		int4 nPolSize, nOrgSize;
		FEdges ** VertandId;
		int4 *marked, *walked;
	};

	class FVertices  
	{
	public:
		TriNode ListNode;
		FFaces * face;
	};

	class  FACE_ADJACENCIES  
	{
	public:
		ADJACENCIES * pfNode;
		int4 bucket;
		ListHead *head;
	};

// main
  // Constructor
  ViaStrip();
  // Destructor
  virtual ~ViaStrip();

  // generate triangle strips from a obj. file
	void CreateStrips(Triple<float4> & rUnits, const std::string & fileName);

  // set the number of vertices, normals, and faces (polygon);
  void SetupPara(const int4 num_vertex, const int4 num_normals, const int4 num_faces);

  // set up the pointer to the polygon buffer);
  void SetPtrToPlgBuf(int1 *ptr);

	// get the pointer to the polygon buffer.
	int1 * GetPtrToBuf() const { return m_pPlgBuf; }

private:

	// the units of the data volume.
	Triple <float4> m_units;

  // the number of vertices, normals, and faces in the polygon buffer
  int4 m_iVertexNum;
  int4 m_iNormalNum;
  int4 m_iFaceNum;
  // the pointer to the input polygon buffer;
  int1 *m_pPlgBuf;

  // global.h
	Triple <float8> * m_pVertices;
  Triple <float8> * m_pNVertices;
  Triple <float8> * m_pPVertices;
  Triple <float8> * m_pPNVertices;

  // a buffer that stores numbered vertices (id) of a polygon
  int4 m_IDs[MAX1];
  int4 m_iNorms[MAX1];
  int4 * m_pVertNorms;
  int4 * m_pVertTexture;

  // output.h
  int4 * m_pVN, *m_pVT;

  // polverts.h
	int4 m_iOrient;
  ListHead ** m_ppPolFaces;
  ListHead ** m_ppPolEdges;
  ListHead *  m_pArray[60];
	ListHead *  m_pStrips;
  ListHead ** m_plVertices;					// Pointers from vertex_id to face
	FACE_ADJACENCIES * m_pFaceArray;	// Pointers from face_id to face
  
// implemented in ViaStripe.C
  // get to know the number of vertices, normals, faces(polygons), and number of buffers (lines).
  // others, such as texture, are ignored in this application.
//  void GetFileDataInfo(const int1 *fname, int1 *file_open,
//                       int4 *num_vert, int4 *num_nvert,int4 *num_texture,
//                       int4 *num_faces,int4 *num_tris, int4 *num_buffers);

  // Allocate structures for the storage of vertices, normals, and faces
  void AllocateStruct(int4 num_faces,int4 num_vert,int4 num_nvert);

  // read the polygon file and transfer it to triangle strips description
  // kernel of this class
//  void ReadFile(const int1 *fname, int1 *file_open, int4 num_vert,int4 num_nvert, bool texture,
//							int4 num_faces, int4 num_tris,int4 num_buffers,int4 f, bool g,int4 t,int4 tr,
//							float4 norm_difference, bool *quads, int4 *cost, std::ofstream & band);

	// read the polygon file and transfer it to triangle strips description
  // kernel of this class
  void ReadFromBuf(int4 num_vert, int4 num_nvert, int4 num_faces,
                   int4 *cost, std::ofstream & band);

	// add header to the triangle file.
	void AddHeader(const std::string & filename, std::ofstream & band, int4 verts, int4 faces);

// implemented in ViaInit.C
  // initialize and build face (polygon) table
  void StartFaceStruct(int4 numfaces);

  // initialize and build vertex table
  void StartVertexStruct(int4 numverts);

  // initialize and build edge table
  void StartEdgeStruct(int4 numverts);

  // Initialize vertex/normal array to have all zeros to start with.
  void InitVertNorms(int4 num_vert);

  // Initialize vertex/texture array to have all zeros to start with.
  // void InitVertTexture(int4 num_vert);

  // Initialize the strip table
  void InitStripTable();

  // This routine will initialize the table that will
  // have the faces sorted by the number of adjacent polygons to it.
  void InitTableSGI(int4 numfaces);

  // initialize face table
  bool InitFaceTable(int4 nSize);

  // Initialize the face table
  bool InitVertexTable(int4 nSize);

  // Initialize the edge table
  bool InitEdgeTable(int4 nSize);

  // build face (polygon) table
  void BuildFaceTable(int4 nSize);

  // build vertex table
  void BuildVertexTable(int4 nSize);

  // build edge table
  void BuildEdgeTable(int4 nSize);

// ViaQueue.C
  // Init List
  bool InitList(ListHead * LHead);

  // Add Head
  bool AddHead(ListHead *, ListInfo *);

  // Add Tail
  bool AddTail(ListHead *, ListInfo *);

  // Peek List
  ListInfo * PeekList(ListHead *, int4, int4);

  // Remove list
  ListInfo * RemoveList( ListHead * LHead, ListInfo * LInfo);

  // Insert node                 
  bool InsertNode(ListHead * LHead, int4 nPos, ListInfo * LInfo);

  // Remove tail
  ListInfo * RemTail(ListHead *);

  // Remove Head
  ListInfo * RemHead(ListHead *);

  // Try to find a specific node in the queue whose key matches with searching key.
  // Return the pointer to that node if found, return NULL otherwise
  ListInfo * SearchList(ListHead * lpListHead, PVOID lpSKey, int4 (*CompareCallBack)(PVOID, PVOID));

// ViaAdd.C
  // add normal id to the structure norms
  void AddNormId(int4 id, int4 index_count);

  // Save the texture with its vertex for future use when outputting
//  void AddTexture(int id, bool vertex);

  // Add vertex id
  int4 AddVertId(int4 id, int4 index_count);

  // add adjacent edges
  void AddAdjEdge(int4 v1,int4 v2,int4 fnum,int4 index1);

  // Add a new face into our face data structure
  void AddNewFace(int4 ids[MAX1], int4 vert_count, int4 face_id, int4 norms[MAX1]);

  // Copy a face node into a new node, used after the global algorithm is run
  void CopyFace(int4 ids[MAX1], int4 vert_count, int4 face_id, int4 norms[MAX1]);

  // Check to see if we already added this vertex and normal
  bool CheckVN(int4 vertex,int4 normal, vert_added *added);

  // Fill the pointer with the id of the normal
  bool NormArray(int4 id, int4 vertex, float8 normal_difference,
                  // struct vert_struct *n, int4 num_vert);
									Triple <float8> *n, int4 num_vert);

  bool NewVertex(float8 difference, int4 id1,int4 id2, Triple <float8> *n);

// ViaCommon.C
  // find adjacent faces
  void FindAdjacencies(int4 num_faces);

  // determine how many polygons share this edge.
  int4  Num_Adj(int4 id1, int4 id2);

  // Given edge whose endpoints are specified by id1 and id2,
  // determine how many polygons share this edge and return that
  // number minus one (since we do not want to include the polygon
  // that the caller has already).
  int4  Number_Adj(int4 id1, int4 id2, int4 curr_id);

  // Used for the lookahead to break ties.
  int4  Min_Adj(int4 id);

  // Find the bucket that the face_id is currently in.
  int4  Old_Adj(int4 face_id);

  // add the face to the proper bucket, depending on
  // how many faces are adjacent to it (what the value bucket should be).
  void AddSgiAdj(int4 bucket,int4 face_id);

  // Just save the triangle for later
  // void Add_Id_Strips(int4 id, int4 end);
	void AddIdStrips(int4 id, bool end);

  // change the face, that was face_id, to a triangle
  void New_Face (int4 face_id, int4 v1, int4 v2, int4 v3);

  // change the face that was face_id, change it to a triangle
  void NewSizeFace(int4 face_id);

  // Find the face that is adjacent to the edge and is not the current face.
  int4  FindFace(int4 current_face, int4 id1, int4 id2, int4 *bucket);

  // See if the endpoints of the edge specified by id1 and id2
  // are adjacent to the face with face_id
  bool LookUp(int4 id1,int4 id2,int4 face_id);

  // Check to see the adjacencies by going into a polygon that has greater than 4 sides.
  void CheckInPolygon(int4 face_id, int4 *min, int4 size);

  // Check to see what the adjacencies are for the polygons that are inside the quad
  void CheckInQuad(int4 face_id,int4 *min);

  // Return the vertex adjacent to either input1 or input2 that
  // is adjacent to the least number of polygons on the edge that
  // is shared with either input1 or input2.
  int4 GetOutputEdge(int4 face_id, int4 size, int4 *index,int4 id2,int4 id3);

  // pick one edge as the input one from the polygon without an input edge
  void GetInputEdge(int4 *index,int4 id1,int4 id2,int4 id3,
									  int4 *new1,int4 *new2,int4 size,int4 face_id);

  //pick one of the edges with the least number of adjacencies to be the input edge.
  void EdgeLeast(int4 *index,int4 *new1,int4 *new2,int4 face_id,int4 size);

// ViaSgi_triang.C
  // Return the vertex that is adjacent to id1,
  // but is not id2, in the list of integers.
  int4 Adjacent(int4 id2,int4 id1, int4 *list, int4 size);

  // Build a table that has the polygons sorted by the number of adjacent polygons.
  void Build_SGI_Table(int4 num_faces);

  // save sides in temp array, we need it so we know about swaps.
  void BlindTriangulate(int4 size, int4 *index, bool begin, int4 end);

  // When a polygon to be triangulated, and it cannot be done blindly,
  // try to come out on the edge that has the least number of adjacencies
  void Non_Blind_Triangulate(int4 size, int4 *index,
                             int4 next_face_id, int4 face_id, int4 end,
                             int4 color1, int4 color2, int4 color3,
														 std::ofstream & band);

  // find the edge to start on when in the middle of a strip,
  // the edge is the last edge that had been transmitted.
  void Rearrange_Index(int4 *index, int4 size);

  // triangulate a polygon
  void Triangulate(int4 out_edge1, int4 out_edge2, int4 in_edge1, int4 in_edge2,
									 int4 size, int4 *index, int4 reversed, int4 face_id, int4 end,
                   int4 color1, int4 color2,int4 color3, std::ofstream & band);

  // recursively triangulate polygons
  void TriangulatePolygon(int4 out_edge1, int4 out_edge2, int4 in_edge1, int4 in_edge2,
                          int4 size,int4 *index, int4 reversed, int4 face_id, int4 end,
                          int4 color1,int4 color2,int4 color3, std::ofstream & band);

  // This routine will nonblindly triangulate a quad
  void Triangulate_Quad(int4 out_edge1, int4 out_edge2, int4 in_edge1, int4 in_edge2, 
												int4 size, int4 *index, int4 reversed, int4 end);

  // Delete the occurence of id in the list. (list has size size)
  void Delete_From_List(int4 id,int4 *list, int4 *size);

// ViaSgi_triangEx.C
  void Rearrange_IndexEx(int4 *index, int4 size);

  // save sides in temp array, we need it so we know about swaps.
  void BlindTriangulateEx(int4 size, int4 *index, bool begin, int4 end, std::ofstream & band);

  // When triangulate a polygon but cannot do it blindly, try to come out
  // on the edge that has the least number of adjacencies
  void NonBlindTriangulateEx(int4 size, int4 *index, int4 next_face_id, int4 face_id, int4 end, 
                             std::ofstream & band);

  // triangulate a polygon
  void TriangulateEx(int4 out_edge1, int4 out_edge2, int4 in_edge1, int4 in_edge2,
                     int4 size, int4 *index, int4 reversed, int4 face_id, int4 end,
                     std::ofstream & band);

  // nonblindly triangulate a quad
  void Triangulate_QuadEx(int4 out_edge1, int4 out_edge2, int4 in_edge1, int4 in_edge2, 
													int4 size, int4 *index, int4 reversed, int4 end, std::ofstream & band);

  // nonblindly triangulate a polygon, recursively do that
  void Triangulate_PolygonEx(int4 out_edge1, int4 out_edge2, int4 in_edge1, int4 in_edge2,
														 int4 size,int4 *index, int4 reversed, int4 face_id, int4 end,
														 std::ofstream & band);

  // Return the vertex that is adjacent to id1,
  // but is not id2, in the list of integers.
  int4 AdjacentEx(int4 id2,int4 id1, int4 *list, int4 size);

  // Delete the occurence of id in the list. (list has size size)
  void Delete_From_ListEx(int4 id,int4 *list, int4 size);

// ViaUtil.C
  // get a lower value and put it in x
  void SwitchLower (int4 *x, int4 *y);

  // preserve strip orientation
  void PreserveStripOrientation(int4 vertex1, int4 vertex2, int4 vertex3, std::ofstream & band);

  // preserve strip orientation with normal
  void PreserveStripOrientationWithNormal(int4 vertex1, int4 normal1,	int4 vertex2, int4 normal2,
																					int4 vertex3, int4 normal3,	std::ofstream & band);

  // preserve strip orientation with texture & normal
  void PreserveStripOrientationWithTextureAndNormal(int4 vertex1, int4 texture1, int4 normal1,
																									  int4 vertex2, int4 texture2, int4 normal2,
																									  int4 vertex3, int4 texture3, int4 normal3,
																									  std::ofstream & band);

  // preserve strip orientation with texture
  void PreserveStripOrientationWithTexture(int4 vertex1, int4 texture1,	int4 vertex2, int4 texture2,
																					 int4 vertex3, int4 texture3, std::ofstream & band);

  // Search through face to match orignal vertices, Start with vertex1's Vertices struct
  void FindTriangleOrientation(int4 vertex1, int4 vertex2, int4 vertex3, int4 *original_vertex);

  // need the last edge that we had
  void LastEdge(int4 *id1, int4 *id2, int4 *id3, bool save);

  // Get the first triangle in the strip we just found, we will use this to
  // try to extend backwards in the strip
  void FirstEdge(int4 *id1,int4 *id2, int4 *id3);

  // We have a list index of 4 numbers and we wish to return the number that is not id1,id2 or id3
  int4 GetOtherVertex(int4 id1,int4 id2,int4 id3,int4 *index);

  // Find the vertex in the first 3 numbers that does not exist in the last three numbers
  int4 Different(int4 id1,int4 id2,int4 id3,int4 id4,int4 id5, int4 id6, int4 *x, int4 *y);
                         
  // Check to see whether the polygon with face_id was used
  // already, return NULL if it was, otherwise return a pointer to the face.
  ListInfo * Done(int4 face_id, int4 *bucket);

  // determine if x is in the triangle specified by id1,id2,id3
  bool member(int4 x, int4 id1, int4 id2, int4 id3);

  // Does the edge specified by id1 and id2 exist in this
  // face currently? Maybe we deleted in partial triangulation
  bool Exist(int4 face_id, int4 id1, int4 id2);

  // We have a triangle and want to know the third vertex of it
  int4 ReturnOther(int4 *index, int4 e1, int4 e2);

// ViaNewpolve.C
  // save the walks
//  void SaveWalks(int4 numfaces);

  // Put the polygons that are left into a data structure so that we can run the stripping code on it.
//  void SaveRest(int4 *numfaces);

  // Go back and do the walk again, but this time save the lengths inside the data structure.
  void AssignWalk(int4 lastvert, FFaces * temp2, int4 front_walk,int4 y, int4 back_walk);

  // Find the length of the walk
  int4  CalculateWalks(int4 lastvert, int4 y, FFaces * temp2);

  // find bands
//  void FindBands(int4 numfaces, int4 *swaps, int4 *bands, int4 *cost,
//                 int4 *tri, int4 norms, int4 *vert_norms, bool texture, int4 *vert_texture,
//                 std::ofstream & band);

  // find max walks
  int4 Find_Max(FFaces * temp2, int4 lastvert, int4 north, int4 left, int4 *lastminup, int4 *lastminleft);

  // peel the patch from the model to find the max face
  int4 Peel_Max(FFaces * temp2, int4 face_id, int4 north, int4 last_north, int4 orientation,
               int4 last_left, int4 color1, int4 color2, int4 color3,
               bool start, int4 *swaps_added, int4 norms, bool texture,
							 std::ofstream & band);

  // mark a face
  void Mark_Face(FFaces * temp2, int4 color1, int4 color2, int4 color3, bool end, 
                 int4 *edge1, int4 *edge2, int4 *face_id, int4 norms, bool texture,
								 std::ofstream & band);

  // reset
  void Reset_Max(FFaces * temp2, int4 face_id, int4 north, int4 last_north,
                 int4 orientation, int4 last_left,
                 int4 color1, int4 color2, int4 color3, bool start,
								 std::ofstream & band);

  // reset
  void Fast_Reset(int4 x);

  // Update and Test
  int4 Update_and_Test(FFaces * temp2, int4 y, bool first, int4 distance, int4 lastvert, int4 val);

  // if first time, then just update the last seen field
  int4 Test_Adj(FFaces * temp2, int4 x, int4 north, int4 distance, int4 lastvert, int4 value);

  // Check when we last saw the face to the right of the current
  // one. Do it just before start this strip
  bool Check_Right(int4 last_seen, FFaces * temp2, int4 y, int4 face_id);

// ViaOutput.C
  // All the triangles are finished, now is time to output to the data file.
  int4 Finished(int4 *swap, int4 startnewstrip, std::ofstream & band);

  // Try to extend backwards off of the local strip that we just found
  int4 ExtendFace(int4 face_id, int4 e1, int4 e2, int4 *swaps,
                 int4 color1, int4 color2, int4 color3, int4 *vert_norm, int4 normals,
                 int4 *vert_texture, int4 texture, std::ofstream & band);

  // output a polygon
  int4 PolygonOutput(ADJACENCIES * temp, int4 face_id, int4 bucket,
                    ListHead *pListHead, bool first, int4 *swaps,
                    int4 color1, int4 color2, int4 color3, bool global, bool end,
                    std::ofstream & band);

  // save everything into a list, rather than output at once,
  void OutputTri(int4 id1, int4 id2, int4 id3, bool end);
	// void Output_Tri(int4 id1, int4 id2, int4 id3, int4 end);

// ViaOutputEx.C
  // save everything into a list, rather than output at once,
  // void Output_TriEx(int4 id1, int4 id2, int4 id3, int4 flag, int4 end, std::ofstream & band);
	void OutputTriEx(int4 id1, int4 id2, int4 id3, int4 flag, bool end, std::ofstream & band);

  // We just made a strip, now we are going to see if we can extend
  // backwards from the starting face, which had 2 or more adjacencies
  // to start with.
  void ExtendBackwardsEx(int4 face_id, int4 *ties, int4 tie, int4 triangulate, int4 swaps,
													int4 *next_id, std::ofstream & band);

  // output a polygon, the id is face id, the number of adjacent polygons to it is bucket.
  void PolygonOutputEx(ADJACENCIES * temp,int4 face_id,int4 bucket,
                        ListHead *pListHead, int4 *ties, int4 tie,
                        int4 triangulate, int4 swaps, int4 *next_id, bool end,
												std::ofstream & band);

// ViaFree.C
  // call it to free edge table
  void EndEdgeStruct(int4 numverts);

  // call it to face edge table
  void EndFaceStruct(int4 numfaces);

  // free strips
  void FreeStrips();

  // free edge table
  void FreeEdgeTable(int4 nSize);

  // free face table
  void FreeFaceTable(int4 nSize);

  // parse and free list
  void ParseAndFreeList(ListHead *pListHead);

// ViaStruct.C
  // Give the face with id face_id, decrement all the faces that are adjacent to it.
  int4 UpdateAdjacencies(int4 face_id,int4 *next_bucket,int4 *e1,int4 *e2,int4 *ties);

  // decrement a face id
  void UpdateFace(int4 *next_bucket, int4 *min_face, int4 face_id, int4 *e1,
                  int4 *e2,int4 temp1,int4 temp2,int4 *ties);

  // Find the face that is adjacent to the edge and is not the
  // current face. Delete one adjacency from it. Save the min adjacency seen so far.
  void DeleteAdj(int4 id1, int4 id2,int4 *next_bucket,int4 *min_face,
                 int4 current_face,int4 *e1,int4 *e2,int4 *ties);

  // Put the edge that is adjacent to face_id into edge1 and edge2.
  // For each edge see if it is adjacent to face_id.
  int4 GetEdge(int4 *edge1,int4 *edge2,int4 *index,int4 face_id,
               int4 size, int4 id1, int4 id2);

// ViaStructEx.C
  // Put the edge that is adjacent to face_id into edge1
  // and edge2. For each edge see if it is adjacent to face_id.
  int4 GetEdgeEx(int4 *edge1,int4 *edge2,int4 *index,int4 face_id,int4 size, int4 id1, int4 id2);

  // Find the face that is adjacent to the edge and is not the current face.
  // Delete one adjacency from it. Save the min adjacency seen so far.
  void DeleteAdjEx(int4 id1, int4 id2,int4 *next_bucket,int4 *min_face,
                   int4 current_face,int4 *e1,int4 *e2,int4 *ties);

  // Used for the Partial triangulation to find the next face.
  // It will return the minimum adjacency face id found at this face.
  int4 MinFaceAdjEx(int4 face_id, int4 *next_bucket, int4 *ties);

  // Give the face with id face_id, decrement all the faces that are adjacent to it.
  int4  UpdateAdjacenciesEx(int4 face_id, int4 *next_bucket,int4 *e1, int4 *e2, int4 *ties);

  // decremente a face (id)
  void UpdateFaceEx(int4 *next_bucket, int4 *min_face, int4 face_id,
                    int4 *e1,int4 *e2,int4 temp1,int4 temp2,int4 *ties);

  // Find the face that is adjacent to the edge and is not the
  // current face. Save the min adjacency seen so far.
  void FindAdjTallyEx(int4 id1, int4 id2, int4 *next_bucket, int4 *min_face, int4 current_face, int4 *ties);

  // When doing a partial triangulation and we need to
  // put the new face of triangle into the correct bucket
  int4 ChangeFaceEx(int4 face_id, int4 in1, int4 in2, ListHead *pListHead, bool no_check);

// ViaTies.C
  // Alternate in what we choose to break the tie
  // We are just alternating between the first and second thing that we found
  int4 AlternateTie();

  // have a tie to add to the buffer
  void AddTies(int4 id);

  // Clear the buffer, because we do not have the tie any more that we had before
  void ClearTies();

  // Get the next face depending on what  the user specified
  int4 Get_Next_Face(int4 t, int4 face_id, int4 triangulate);

  // Look ahead at this face and save the minimum adjacency
  // of all the faces that are adjacent to this face.
  int4 Look_Ahead(int4 id);

  // Look ahead and find the face to go to that will give the least number of adjacencies
  int4 Look_Ahead_Tie();

  // Break the tie by choosing the face that will not give us a swap
  // and is sequential. If there s not one, then do the lookahead to break the tie.
  int4 Sequential_Tie(int4 face_id,int4 triangulate);

  // have a triangle and need to break the ties at it.
  // choose the edge that is sequential.
  int4 Sequential_Tri(int4 *index);

  // have a quad that need to break its ties,
  // try and choose a side that is sequential, otherwise use lookahead
  int4 Sequential_Quad(int4 *index, int4 triangulate);

  // We have a polygon of greater than 4 sides and wish to break the
  // tie in the most sequential manner.
  int4 Sequential_Poly(int4 size, int4 *index, int4 triangulate);

  // had a tie within a tie in the lookahead, break it randomly
  int4 Random_Look(int4 id[],int4 count);

  // Randomly choose the next face with which to break the tie
  int4 Random_Tie();

  // Used to sequentially break ties in the whole triangulation for polygons greater than 4 sides.
  void Whole_Output(int4 in1, int4 *index, int4 size, int4 *out1, int4 *out2);

// ViaLocal.C
  // Build a table that has all the polygons sorted by the number of polygons adjacent to it.
  void SGIStrip(int4 num_faces, int4 ties,int4 triangulate, std::ofstream & band);

  // This routine will peel off the strips from the model
  void FindStripsEx(int4 *ties, int4 tie, int4 triangulate, 
										int4 swaps, int4 *next_id, std::ofstream & band);

// ViaPartial.C
  // If there is a a polygon that is greater than 4 sides, and
  // that it is better to go inside the polygon for the next one,
  // since inside will have less adjacencies than going outside.
  // So, we are not doing partial for a part of the polygon.
  void Inside_Polygon(int4 size,int4 *index,int4 face_id,ListHead *pListHead, int4 end, std::ofstream & band);

  // A polygon has to be triangulated but cannot do it blindly,
  // try to come out on the edge that has the least number of adjacencies,
  void Partial_Triangulate(int4 size,int4 *index, int4 next_face_id,int4 face_id,
                           int4 *next_id,ListHead *pListHead, ADJACENCIES * temp, int4 end,
													 std::ofstream & band);

  // do triangulate
  void P_Triangulate(int4 out_edge1,int4 out_edge2,int4 in_edge1,
                     int4 in_edge2,int4 size,int4 *index, int4 reversed,int4 face_id,
                     int4 *next_id,ListHead *pListHead, ADJACENCIES * temp, int4 end,
										 std::ofstream & band);

  // When there is a polygon greater than 4 sides, partially triangulate it
  void P_TriangulatePolygon(int4 out_edge1,int4 out_edge2,int4 in_edge1,
                            int4 in_edge2,int4 size, int4 *index, int4 reversed,int4 face_id,
                            int4 *next_id, ListHead *pListHead, ADJACENCIES * temp2, int4 end,
                            std::ofstream & band);

  // nonblindly triangulate a quad
  void ViaStrip::P_Triangulate_Quad(int4 out_edge1,int4 out_edge2,int4 in_edge1,
                                    int4 in_edge2,int4 size,int4 *index, int4 reversed,
                                    int4 face_id,ListHead *pListHead, ADJACENCIES * temp, int4 end,
																		std::ofstream & band);

  // The polygon had an input edge, specified by input1 and input2
  void Input_Edge(int4 face_id, int4 *index, int4 size, int4 in_edge1,
									int4 in_edge2, ListHead *pListHead, int4 end, std::ofstream & band);
};

#endif // VIA_STRIP_H


// $Log: ViaStrip.h,v $
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
//    Rev 2.1   Oct 01 2001 16:10:08   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.0   Sep 16 2001 23:39:10   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:42   ingmar
// Initial revision.
// Revision 1.39  2001/07/25 16:16:42  lihong
// Defect Id: 599
// Added throw ex:: after the function name
//
// Revision 1.38  2001/04/12 18:07:34  dmitry
// modified base class for documentation
//
// Revision 1.37  2001/04/12 14:38:47  dmitry
// Sec_j_tri base class added
//
// Revision 1.36  2001/04/09 17:31:25  dmitry
// updated datatypes
//
// Revision 1.35  2001/04/02 19:18:21  binli
// code simplified!
//
// Revision 1.34  2001/03/27 23:21:00  binli
// no message
//
// Revision 1.33  2001/03/27 19:49:51  binli
// code walkthrough: name convension
//
// Revision 1.32  2001/03/27 16:55:54  binli
// reflected new code standard.
//
// Revision 1.31  2001/03/27 15:25:31  binli
// 'const' functions for safety.
//
// Revision 1.30  2001/03/23 20:41:20  binli
// code walkthrough
//
// Revision 1.29  2001/03/22 21:15:22  binli
// renamed some variables according to new code standard.
//
// Revision 1.28  2001/03/21 17:15:12  binli
// 1> replaced triple variable with standard Triple.
// 2> changed struct Strips to class Strips.
//
// Revision 1.27  2001/03/21 16:36:14  binli
// moved classes into ViaStrip.
//
// Revision 1.26  2001/03/21 15:24:11  binli
// replaced all structs with classes.
//
// Revision 1.25  2001/03/02 15:06:55  binli
// removed unused code.
//
// Revision 1.24  2001/03/01 17:49:52  binli
// changed 'int' to 'bool' in Output_Tri() and Output_TriEx().
//
// Revision 1.23  2001/02/28 21:01:59  binli
// 1> BOOL->bool
// 2> Error Info
//
// Revision 1.22  2001/02/28 18:19:38  binli
// tried to solve 'BOOL' problem in ViaStrip: changed parameter from BOOL to int in Output_Tri().
//
// Revision 1.21  2001/02/09 18:24:42  binli
// // defect 000186: turn the temporal file 'triangle.smf' into in-memory buffer
//
// Revision 1.20  2001/01/19 15:48:59  binli
// moved temproal files: triangle.tri.and triangle.smf, from common work directory to their own directory.
//
// Revision 1.19  2000/12/19 17:10:54  sarang
// loop bugs fixed (scope of variables)
// fix for gcc/linux
//
// Revision 1.18  2000/12/11 19:41:43  binli
// applied float8 operation to the output  of 'world coordinate' described triangle files.
// --- no twice scale is needed in v1k.
//
// Revision 1.17  2000/12/08 21:16:41  binli
// removed code to write out 'text' triangle files.
//
// Revision 1.16  2000/12/08 16:56:18  binli
// changed the output of triangle files from text to binary.
// added header to triangle files.
//
// Revision 1.15  2000/12/04 18:42:12  binli
// write out triangles in World coordinates.
//
// Revision 1.14  2000/11/07 18:37:33  sarang
// Removed _errors_ which gcc found.
// 1. BOOL was never defined
// 2. function definition and call mismatch (bool and int were not distinguished by VC++!!!)
//
// Revision 1.13  2000/11/06 23:57:03  binli
// copied the old version (from mixkit) back to replace 'polygon generation'  in lastest version.
// so as to make the triangle generation modular works.
//
// Revision 1.12  2000/11/06 03:59:19  dave
// convert back to real bool
//
// Revision 1.11  2000/11/05 00:05:52  dave
// fixed one segfault, need to remove or re-write code entirely
//
// Revision 1.10  2000/11/02 07:35:27  antonio
// added boolean variable costPositive to ViaStrip::DoStripe,and made g into bool.
//
// Revision 1.9  2000/11/02 04:54:47  antonio
// changed int where -> bool end for functions in ViaStrip.h
//
// Revision 1.8  2000/11/02 02:18:26  ingmar
// made int texture -> bool texture
//
// Revision 1.7  2000/10/21 22:24:25  ingmar
// code cleanup
//
// Revision 1.6  2000/10/21 21:00:24  ingmar
// BOOL -> bool and other code cleanup
//
// Revision 1.5  2000/10/21 20:29:30  ingmar
// filenaming cleanup
//
// Revision 1.4  2000/10/21 06:06:04  ingmar
// merged mixkit branch back into main trunk
//
// Revision 1.2.10.1  2000/10/19 20:37:02  binli
// code optimization
//
// Revision 1.2  2000/09/20 15:29:47  binli
// Analysized the 'Strip' module to fix the memory leaking problem.
//
// Revision 1.1.1.1  2000/08/11 22:15:54  ingmar
// unit tests complete
//
//
// 11    9/05/00 4:21p Binli
// change the sub-pipeline of polygon generation (3) : gap the 'data
// format transfer' submodule and 'getting triangle strips' submodule, so
// no temporary file(s) '.obj' is (are) generated.
//
// 10    8/29/00 12:41p Antonio
// updated footer
//
// *****************  Version 9  *****************
// User: Binli           Date:  8/24/00  Time: 12:30p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments to Class ViaStrip
//
// *****************  Version 8  *****************
// User: Binli           Date:  8/23/00  Time:  5:04p
// Checked in $/v2k/src/Triangles
// Comment:
//   added comments
//
// *****************  Version 7  *****************
// User: Binli           Date:  8/23/00  Time: 12:57p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments
//
// *****************  Version 6  *****************
// User: Binli           Date:  8/23/00  Time: 10:34a
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments
//
// *****************  Version 5  *****************
// User: Binli           Date:  8/22/00  Time:  3:16p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added Comments
//
// *****************  Version 4  *****************
// User: Antonio         Date:  8/21/00  Time:  6:34p
// Checked in $/v2k/src/Triangles
// Comment:
//   added header and footer
//
// *****************  Version 3  *****************
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
// *****************  Version 2  *****************
// User: Binli           Date:  7/31/00  Time:  9:55p
// Checked in $/v2k/src/Triangles
// Comment:
//   Muiti-Seg preprocessing
//
// *****************  Version 1  *****************
// User: Binli           Date:  7/31/00  Time:  5:13p
// Created ViaStrip.h
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaStrip.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $
// $Id: ViaStrip.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $
