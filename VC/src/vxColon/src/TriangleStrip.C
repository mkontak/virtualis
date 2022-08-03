// $Id: TriangleStrip.C,v 1.5 2006/10/02 21:01:03 dongqing Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Bin Li (binli@viatronix.net)


// includes
#include "stdafx.h"
#include "TriangleStrip.h"
#include "TransferFunctionVC.h"
#include "File.h"
#include "Serialization.h"
#include "VolumePaint.h"


// defines
#define FILE_REVISION "$Revision: 1.5 $"


// namespaces
using namespace std;
using namespace Serialization;
using namespace ReaderLib;


/**
 * Constructor : set up some default values.
 */
TriangleStrip::TriangleStrip()
{
  m_pStrip = NULL;
} // TriangleStrip()


/**
 * Copy constructor
 *
 * @param other   the other instance to be copied.
 */
TriangleStrip::TriangleStrip(const TriangleStrip & other)
{
  if ( other.m_pStrip == NULL ) 
	{
    m_pStrip = NULL;
  } 
	else 
	{
    m_pStrip = new Strip(*other.m_pStrip);
  } // endif
} // TriangleStrip()


/**
 * Assignment operator
 *
 * @param other   the other instance to be assigned.
 */
TriangleStrip & TriangleStrip::operator =(const TriangleStrip & other)
{
  if ( other.m_pStrip == NULL ) 
	{
    m_pStrip = NULL;
  } 
	else 
	{
    if (m_pStrip != NULL) delete m_pStrip;
    m_pStrip = new Strip(*other.m_pStrip);
  } // endif
  return (*this);
} // operator =


/**
 * Destructor : free the data structure.
 */
TriangleStrip::~TriangleStrip()
{
  Clear();
} // ~TriangleStrip()


/**
 * Clear all the data structures.
 */
void TriangleStrip::Clear()
{
  // free memory
  if ( m_pStrip ) 
	{
    // free itself
    delete m_pStrip;
    m_pStrip = NULL;
  }
} // Clear


/**
 * read triangle strips information for all segments and store them in the structure.
 *
 * @param filename      input file name.
 * @return      true when success and false when failed.
 */
bool TriangleStrip::ReadTriangleStripDataSet(const string & sFilename)
{
	int4 iNumOfVerts = 0;
	int4 iNumOfFaces = 0;
	int4 iNumOfNormals = 0;
	int4 iNumber = 0, iVertex = 0;
	int4 viBandBuf[16];
	float4 vfDataBuf[9];
 
  // first pass: get some information: number of vertices/normals/triangles/strips. 
  std::ifstream strips1(sFilename.c_str(), std::ios::binary | std::ios::out);
  if ( !strips1 ) 
  {
    std::string ErrorInfo("Error create strips: Unable to open strips file \"");
    ErrorInfo += sFilename;
    ErrorInfo += "\" for reading.";
    throw ex::VException(LogRec(ErrorInfo,"TriangleStrip","ReadTriangleStripDataSet"));
  } // bad file.

  // Scan the file to see how many vertices, normals and triangles
  // so we can allocate the space properly
	bool bReading = true;
	int1 iCh = strips1.get();
	while (bReading) 
	{
		if (iCh == EOF) 
		{
			break;
		} 
		else if ( iCh=='v' ) 
		{
      // it's a vertex.
			iNumOfVerts ++;
			strips1.read((int1*)(&vfDataBuf[0]), sizeof(float4)*3);
		} 
		else if ( iCh=='n') 
		{
      // it's a normal.
			iNumOfNormals ++;
      strips1.read((int1*)(&vfDataBuf[0]), sizeof(float4)*3);
		} 
		else if (iCh == 't' || iCh == 'q' || iCh == 'f') 
		{
      // new strip(t)/cintinue a strip(q)/single face.
			iNumOfFaces ++;
			iCh = strips1.get();	// skip a space.
			iNumber = 0;
			do 
			{
				strips1.read((int1*)(&viBandBuf[iNumber]), sizeof(int4));
				iNumber ++;
				iCh = strips1.get(); // space or '\n'
				if (iCh == '\n' || iCh == EOF) 
				{
          // end of file or new line.
					if (iCh == EOF) 
					{
            // end of the file, close the stream.
						strips1.close();
						bReading = false;
					} 
					else 
					{
						break;
					}
				} // end-if ('\n' || EOF)
			} while (bReading);
    }
    else if ( iCh=='#' )
		{
      // just a comment.
			int1 viTemp[100];
			strips1.getline(viTemp, 100, '\n');
		} // end-if (ch=='t' || ch=='q').
		iCh = strips1.get();
	} // end-while (bReading).

  Point <float4> *pVertex = NULL, *pVertexBak = NULL;
  Vector<float4> *pNormal = NULL, *pNormalBak = NULL;
  Triple<float4> ** hfIndex;
  Face *pFaces = NULL, *pFaceBak = NULL;

  // allocate space for storing structured triangle strips of one segment
  Strip *pStripPtr = new Strip;
  pVertex = pVertexBak = new Point<float4> [iNumOfVerts];
  pNormal = pNormalBak = new Vector<float4> [iNumOfNormals];
  pFaces = pFaceBak = new Face[iNumOfFaces+1];
  if ( pStripPtr == NULL || pVertex == NULL || pNormal == NULL || pFaces == NULL ) 
	{
    // memory allocation error.
    throw ex::OutOfMemoryException(LogRec("TriangleStrip::ReadTriangleStripDataSet() - StripPtr == NULL || VertPtr == NULL || pNormal == NULL || faces == NULL","TriangleStrip","ReadTriangleStripDataSet"));
	} 

  // second pass: retrieve the triangle strips descriptions.
	bReading = true;
	std::ifstream strips2(sFilename.c_str(), std::ios::binary | std::ios::out);
	if ( !strips2 ) 
	{
		std::string ErrorInfo("Error create strips: Unable to open strips file \"");
		ErrorInfo += sFilename;
		ErrorInfo += "\" for reading.";
    throw ex::VException(LogRec(ErrorInfo,"TriangleStrip","ReadTriangleStripDataSet"));
	} // bad file.

	iCh = strips2.get();
	while (bReading) 
	{
		if ( iCh==EOF ) 
		{
			break;
		}	
		else if ( iCh=='v' ) 
		{
      // a vertex.
			strips2.read((int1*)(&vfDataBuf[0]), sizeof(float4)*3);
			pVertex->m_x = vfDataBuf[0];
			pVertex->m_y = vfDataBuf[1];
			pVertex->m_z = vfDataBuf[2];
			pVertex ++;
		} 
		else if ( iCh=='n' ) 
		{
      // a normal.
			strips2.read((int1*)(&vfDataBuf[0]), sizeof(float4)*3);
			pNormal->m_x = vfDataBuf[0];
			pNormal->m_y = vfDataBuf[1];
			pNormal->m_z = vfDataBuf[2];
			pNormal ++;
		} 
		else if ( iCh=='t' || iCh=='q' || iCh == 'f') 
		{
      // a new strip(t), or continue a strip(q), or a single face(f).
			if (iCh == 't') 
			{
				// a new tri_strip.
        pFaces->m_iTriStrip = 1;	
			} 
			else if (iCh == 'q') 
			{
        // continue the strip.
        pFaces->m_iTriStrip = 2;	
			} 
			else 
			{
        // just a single face.
				pFaces->m_iTriStrip = 0;	
			} // end-if

			iCh = strips2.get();	// skip a space.
			iNumber = 0;
			do 
			{
				strips2.read((int1*)(&viBandBuf[iNumber]), sizeof(int4));
				iNumber ++;
				iCh = strips2.get(); // space or '\n'
				if (iCh == '\n' || iCh == EOF) 
        {
          // new line/end of file.
					pFaces->m_iNumOfFaces = iNumber;
					pFaces->m_hIndex = hfIndex = new (Triple<float4> *[iNumber*2]);
					pFaces->m_piVertexIndex = new uint4 [iNumber];
					if ( pFaces->m_hIndex == NULL )
					{
            throw ex::OutOfMemoryException(LogRec("TriangleStrip::ReadTriangleStripDataSet() - faces->m_pIndex == NULL","TriangleStrip","ReadTriangleStripDataSet"));
					}
					
					// put a triangle into class face.
          for (int4 loop=0; loop<iNumber; loop++) 
					{
						iVertex = viBandBuf[loop] - 1;
						*hfIndex     = reinterpret_cast<Triplef*>(&pVertexBak[iVertex]);
						*(hfIndex+1) = reinterpret_cast<Triplef*>(&pNormalBak[iVertex]);
						hfIndex += 2;
						pFaces->m_piVertexIndex[loop] = iVertex;
          } // endfor.
					pFaces ++;
					if (iCh == EOF) 
					{
            // end of the file, close the stream.
						strips2.close();
						bReading = false;
					} 
					else 
					{
						break;
					}
				} // end-if
			} while (bReading);
		} 
		else if ( iCh=='#' ) 
		{
			int1 viTemp[100];
			strips2.getline(viTemp, 100, '\n');
		} // end-if (ch=='t' || ch=='q').
		iCh = strips2.get();
	} // end-while (bReading)

  pStripPtr->m_pFaces = pFaceBak;
  pStripPtr->m_pNormals = pNormalBak;
  pStripPtr->m_pVertices = pVertexBak;
	pStripPtr->m_iFacesNo = iNumOfFaces;
  pStripPtr->m_iNumOfVertices = iNumOfVerts;
  pStripPtr->m_iNumOfNormals = iNumOfNormals;
  m_pStrip = pStripPtr;

  return true;
} // ReadTriangleStripDataSet()


/**
 * remove the triangle file from the disk.
 *
 * @param filename      input file name.
 * @return      true when success and false when failed.
 */
bool TriangleStrip::RemoveTriangleFile(const std::string & sFilename)
{
	vx::File triangleFile(sFilename); 
	return triangleFile.Delete();
} // RemoveTriangleFile().


/**
 * render triangle strips (Transluent Tissue Projection).
 *
 * @param       mCurrMatrix   float matrix.
 */
void TriangleStrip::RenderSegmentTTP(const Matrix<float4> & mCurrMatrix)
{
	// check for data existence
	if ( m_pStrip == NULL || m_pStrip->m_pFaces == NULL ) 
	{
    throw ex::VException(LogRec("No triangle strip data loaded","TriangleStrip","RenderSegmentTTP"));
	} // error
   
	// set material properties of a segment
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	DisplayStripsTTP(mCurrMatrix);
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
} // RenderSegment()
 

/**
 * render triangle strips (all segments).
 *
 * @param       color   segment color assigned.
 */
void TriangleStrip::RenderSegment(const Triple<uint1> vColor) const
{
  // check for data existence
  if ( m_pStrip == NULL || m_pStrip->m_pFaces == NULL ) 
  {
    throw ex::VException(LogRec("No triangle strip data loaded","TriangleStrip","RenderSegment"));
  } // error
  
  // set material properties of a segment
  glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  const uint1 uOpacity = 76;
  glColor4ub(vColor.m_x, vColor.m_y, vColor.m_z, uOpacity);
  DisplayStrips();
  glDisable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHT0);
} // RenderSegment()


/**
 * render triangle strips (painting).
 *
 * @param puPaintVolume
 * @param iCurrentSeenVal
 * @param vuStep
 * @param vfWorld2vol
 */
void TriangleStrip::RenderSegment(uint1 * puPaintVolume, const uint1 iCurrentSeenVal,
                                  const Triple<uint4> & vuStep, const Point<float4> & vfWorld2vol)
{
  const uint4 uxyStep = vuStep.m_x + vuStep.m_y;
  const uint4 uxzStep = vuStep.m_x + vuStep.m_z;
  const uint4 uyzStep = vuStep.m_y + vuStep.m_z;
  const uint4 uxyzStep = uxyStep + vuStep.m_z;

  // check for data existence
  if ( m_pStrip == NULL || m_pStrip->m_pFaces == NULL ) 
  {
    throw ex::VException( LogRec("No triangle strip data loaded","TriangleStrip","RenderSegment"));
  } // error

  // set material properties of a segment
  glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);

  // render the triangle strips
  Face * pFace = m_pStrip->m_pFaces;
  Point<float4> * pVertices = m_pStrip->m_pVertices;
  Vector<float4> * pNormals = m_pStrip->m_pNormals;
  int4 iQuad = 0;
  for ( int4 i=0; i<m_pStrip->m_iFacesNo; i++ ) 
  {
    if ( pFace->m_iTriStrip == 1 ) 
    {
      // it's a triangle strip
      glBegin(GL_TRIANGLE_STRIP);
    }
    else if ( pFace->m_iTriStrip == 0 ) 
    {
      // it's a individual triangle
      glBegin(GL_POLYGON);
    }

    const uint4 * piVertexIndex = pFace->m_piVertexIndex;
    bool bSeen = true;
    glColor3ubv( & TransferFunctionVC::m_vSeenScheme1.m_x );
    for ( int4 k=0; k<pFace->m_iNumOfFaces; k++ ) 
    {
      const uint4 uIndex = piVertexIndex[k];
      Point<float4> * pVertex = pVertices + uIndex;
      const uint4 x = static_cast<uint4>(pVertex->m_x * vfWorld2vol.m_x);
      const uint4 y = static_cast<uint4>(pVertex->m_y * vfWorld2vol.m_y);
      const uint4 z = static_cast<uint4>(pVertex->m_z * vfWorld2vol.m_z);
      const uint4 uRGBPtr = z*vuStep.m_z + y*vuStep.m_y + x;
      if((puPaintVolume[uRGBPtr        ]& NOTCOUNTMASK) >= iCurrentSeenVal ||
         (puPaintVolume[uRGBPtr + vuStep.m_x]& NOTCOUNTMASK) >= iCurrentSeenVal ||
         (puPaintVolume[uRGBPtr + vuStep.m_y]& NOTCOUNTMASK) >= iCurrentSeenVal ||
         (puPaintVolume[uRGBPtr + uxyStep   ]& NOTCOUNTMASK) >= iCurrentSeenVal ||
         (puPaintVolume[uRGBPtr + vuStep.m_z]& NOTCOUNTMASK) >= iCurrentSeenVal ||
         (puPaintVolume[uRGBPtr + uxzStep   ]& NOTCOUNTMASK) >= iCurrentSeenVal ||
         (puPaintVolume[uRGBPtr + uyzStep   ]& NOTCOUNTMASK) >= iCurrentSeenVal ||
         (puPaintVolume[uRGBPtr + uxyzStep  ]& NOTCOUNTMASK) >= iCurrentSeenVal  ) 
      {
        // try to minimize wardrobe changes - don't set color if same as before
        if ( ! bSeen )
        {
          bSeen = true;
          glColor3ubv( & TransferFunctionVC::m_vSeenScheme1.m_x );
        }
      }
      else 
      {
        if ( bSeen )
        {
          bSeen = false;
          glColor3ubv( & TransferFunctionVC::m_vNotSeenScheme1.m_x );
        }
      }
 			glNormal3fv((GLfloat *)(pNormals  + uIndex));
 			glVertex3fv((GLfloat *)(pVertex));
    } // endfor all vertices

    pFace++;
    if ( pFace->m_iTriStrip != 2 ) 
    {
      iQuad = 0;
      glEnd();
    }
    else
    {
      iQuad = 1;
    } //endif tri_strip != 2
  } // endfor all faces
  if ( iQuad == 1 ) 
  {
    glEnd();
  }
  glDisable(GL_COLOR_MATERIAL);
} // RenderSegment()


/**
 * Display strips.
 */
void TriangleStrip::DisplayStrips() const
{
  // check for data existence
  if ( m_pStrip == NULL || m_pStrip->m_pFaces == NULL ) 
	{
    throw ex::VException(LogRec("No triangle strip data loaded","TriangleStrip","DisplayStrips"));  
  } // error

  Face * pFace = m_pStrip->m_pFaces;
  Point<float4> * pVertices = m_pStrip->m_pVertices;
  Vector<float4> * pNormals = m_pStrip->m_pNormals;
  
  // display triangle strips
  int4 iQuad = 0;
  for ( int4 i=0; i<m_pStrip->m_iFacesNo; i++ ) 
	{
    if ( pFace->m_iTriStrip == 1 )     
		{
			// it's a triangle strip
      glBegin(GL_TRIANGLE_STRIP);
		}
    else if ( pFace->m_iTriStrip == 0 )        // it's a individual triangle
		{  
			glBegin(GL_POLYGON);
		}

    const uint4 * piVertexIndex = pFace->m_piVertexIndex;
    for ( int4 k=0; k<pFace->m_iNumOfFaces; k++ ) 
		{
      const uint4 uIndex = piVertexIndex[k];
      glNormal3fv((GLfloat *)(pNormals  + uIndex));
      glVertex3fv((GLfloat *)(pVertices + uIndex));
    } // endfor all vertices

    pFace++;
    if ( pFace->m_iTriStrip != 2 ) 
    {
      iQuad = 0;
      glEnd();
    } 
    else 
    {
      iQuad = 1;
    } //endif tri_strip != 2
  } // endfor all faces
  if ( iQuad == 1 ) 
  {
    glEnd();
  }
} // DisplayStrips()


/**
 * Display strips (TTP)
 *
 * @param mCurrMatrix   float matrix.
 */
void TriangleStrip::DisplayStripsTTP(const Matrix<float4> & mCurrMatrix)
{
  // check for data existence
  if ( m_pStrip == NULL || m_pStrip->m_pFaces == NULL ) 
	{
    throw ex::VException(LogRec("No triangle strip data loaded.","TriangleStrip","DisplayStripsTTP"));
  } // error

  Face * pFace = m_pStrip->m_pFaces;
  Point<float4> * pVertices = m_pStrip->m_pVertices;
  Vector<float4> * pNormals = m_pStrip->m_pNormals;
  
  // display triangle strips
  int4 iQuad = 0;
  for ( int4 i=0; i<m_pStrip->m_iFacesNo; i++ ) 
	{
    if ( pFace->m_iTriStrip == 1 ) 
		{
			// it's a triangle strip
      glBegin(GL_TRIANGLE_STRIP);
		}
    else if ( pFace->m_iTriStrip == 0 ) 
		{
			// it's a individual triangle
      glBegin(GL_POLYGON);
		}
    
    const uint4 * piVertexIndex = pFace->m_piVertexIndex;
    for ( int4 k=0; k<pFace->m_iNumOfFaces; k++ ) 
		{
      const uint4 uIndex = piVertexIndex[k];
      glNormal3fv((GLfloat *)(pNormals  + uIndex));
      const Matrix<float4> &mRotMatrix = mCurrMatrix;
      Vector<float4> vNormal = mRotMatrix * pNormals[uIndex];
      glColor4f(1.0,1.0,1.0, (1.0-fabs(vNormal.m_z))*0.4 + 0.05);
      glVertex3fv((GLfloat *)(pVertices + uIndex));
    } // endfor all vertices
    
    pFace++;
    if ( pFace->m_iTriStrip != 2 ) 
		{
      iQuad = 0;
      glEnd();
    } 
		else 
		{
      iQuad = 1;
    } //endif tri_strip != 2
  } // endfor all faces
  if ( iQuad == 1 ) 
	{
    glEnd();
  }
} // DisplayStrips()


/**
 * Read triangle strips from disk
 * 
 * @param in  input file stream
 * @return  true when successfully read/false: failed.
 */
bool TriangleStrip::Read(std::ifstream & in)
{
  Clear();

  // make sure the stream is ok before reading
  if (in.bad()) 
	{
    throw ex::VException(LogRec("Trouble beginning to read strips from disk.","TriangleStrip","Read"));
  }

  // read identifier
  const uint4 uBufSize = 1000;
  int1 iBuffer[uBufSize];
  string sIdentifier;
  in >> sIdentifier;
  in.getline(iBuffer, uBufSize);
  if ( sIdentifier.compare("Strip") != 0 ) 
	{
    std::string ErrorInfo("Unable to read this strip from disk.");
    ErrorInfo += "Instead got";
    ErrorInfo += iBuffer;
    throw ex::VException(LogRec(ErrorInfo,"TriangleStrip","Read"));
  }

  // read in if there is any data at all
  int4 iAnyData;
  in >> iAnyData;
  in.getline(iBuffer, uBufSize);
  if (iAnyData == 1) 
	{
    // read in all the strips
    m_pStrip = new Strip;
    bool bResult = m_pStrip->Read(in);
    if (bResult == false) 
		{
      throw ex::VException(LogRec("Unable to read strips from disk","TriangleStrip","Read"));
    } // if bad strip read
  } // if any data

  return true;
} // Read()


/**
 * Write triangle strips to disk
 *
 * @param out   output file stream
 * @return true when successfully write/false when failed.
 */
bool TriangleStrip::Write(std::ofstream & out) const
{
  // make sure the stream is ok before writing
  if ( out.bad() ) 
	{
    throw ex::VException(LogRec("Trouble beginning to write triangle strip.","TriangleStrip","Write"));
  }

  // write identifier
  out << "Strip" << endl;

  // Write out if there is any data at all
  if (m_pStrip == NULL) 
	{
    out << "0 # Segments of triangle strips (contains no data)" << endl;
  } 
	else 
	{
    out << "1 # Segments of triangle strips (contains some data)" << endl;

    // write out all the strips
    bool bResult = m_pStrip->Write(out);
    if (bResult == false) 
		{
      throw ex::VException(LogRec("Unable to write strips to disk.","TriangleStrip","Write"));
    } // end bad write
  } // end any data to write out

  return true;
} // Write()


/**
 * Retrieves the number of bytes of the serialized data.
 *
 * @return  data size.
 */
uint4 TriangleStrip::GetSerializedDataSize() const
{
  uint4 uSum = 0;

  // starting delimiter
  uSum += string("TriangleStrip").length() + 1;

  // bool bAnyData
  uSum += sizeof(bool);

  if (m_pStrip != NULL) 
	{
    uSum += m_pStrip->GetSerializedDataSize();
  } // if any data

  // ending delimiter
  uSum += string("TriangleStrip").length() + 1;

  return uSum;
} // GetSerializedDataSize()


/**
 * Retrieves all the data in serialized form.
 */
bool TriangleStrip::WriteSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd) const
{
  try
  {
    // ensure there is enough space
    if (*hiBuffer + GetSerializedDataSize() > piBufferEnd) 
	  {
      throw ex::OutOfMemoryException(LogRec("<TriangleStrip::WriteSerializedData>:Not enough space","TriangleStrip","WriteSerializedData"));
    } // if not enough space

    // write a delimiter to help verify the data
    SerializeStringData(hiBuffer, piBufferEnd, "TriangleStrip");
    const bool bAnyData = (m_pStrip != NULL);
    SerializeSimpleData(hiBuffer, piBufferEnd, bAnyData);

    if (bAnyData == true) 
	  {
      bool bResult = m_pStrip->WriteSerializedData(hiBuffer, piBufferEnd);
      if (bResult == false) 
		  {
        throw ex::VException(LogRec("<TriangleStrip::WriteSerializedData>: Bad serialization.","TriangleStrip","WriteSerializedData"));
      } // bad serialization
    } // any data to serialize

    // write a delimiter to help verify the data
    SerializeStringData(hiBuffer, piBufferEnd, "TriangleStrip");
  }
  catch (ex::VException & ex)
  {
    throw ex::VException(LogRec( ex.GetLogRecord().GetTextMessage(),"TriangleStrip", "WriteSerializedData"));
  }
  catch (...)
  {
    throw ex::VException(LogRec( "An unspecified error caught at stage 'loading fields'","TriangleStrip", "WriteSerializedData" ));
  }

  return true;
} // WriteSerializedData()


/**
 * Reads in all the data from serialized form.
 */
bool TriangleStrip::ReadSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd)
{
  // attempt to verify the stream by reading a delimiter
  try
  {
    if (DeserializeStringData(hiBuffer, piBufferEnd) != "TriangleStrip") 
      throw ex::DataCorruptionException(LogRec("data corruption:bad starting delimiter.","TriangleStrip","ReadSerializedData"));

    bool bAnyData = true;
    DeserializeSimpleData(hiBuffer, piBufferEnd, bAnyData);

    if (bAnyData == true) 
	  {
      m_pStrip = new Strip;
      bool bResult = m_pStrip->ReadSerializedData(hiBuffer, piBufferEnd);
      if (bResult == false) // bad deserialization
        throw ex::DataCorruptionException(LogRec("data corruption: bad deserialization.","TriangleStrip","ReadSerializedData"));
    } 
	  else 
	  {
      m_pStrip = NULL;
    } // any data to serialize

    // attempt to verify the stream by reading a delimiter
    if (DeserializeStringData(hiBuffer, piBufferEnd) != "TriangleStrip") 
	  {
      throw ex::DataCorruptionException(LogRec("data corruptionbad: ending delimiter.","TriangleStrip","ReadSerializedData"));
    }
  }
  catch (ex::VException & ex)
  {
    throw ex::VException(LogRec( ex.GetLogRecord().GetTextMessage(),"ReadSerializedData", "ReadSerializedData"));
  }
  catch (...)
  {
    throw ex::VException(LogRec( "An unspecified error caught at stage 'triangle strps read'","ReadSerializedData", "ReadSerializedData"));
  }

  return true;
} // ReadSerializedData().


/**
 * Equality operator.
 *
 * @param other reference to object 'strip'
 * @return true when equal, else, false.
 */
bool TriangleStrip::operator ==(const TriangleStrip & other) const
{
  if (m_pStrip == NULL && other.m_pStrip == NULL) 
	{
    return true;
  } // if both empty

  if (m_pStrip == NULL && other.m_pStrip != NULL ||
    m_pStrip != NULL && other.m_pStrip == NULL) 
	{
    return false;
  } // if only one empty
  
  // both are full
  const Strip & strip1 = *m_pStrip;
  const Strip & strip2 = *(other.m_pStrip);
  // negative-negative logic is used, as no operator (!=) be defined.
  if (!(strip1 == strip2)) 
	{
    return false;
  } // if different

  return true;
} // operator ==


/**
 * Verify that all the vertices are inside the active volume.
 *
 * @ return false when NULL, else true;
 */
bool TriangleStrip::Verify(const Triple<uint4> volumeDimension) const
{
  if (m_pStrip == NULL) 
	{
    return true;
  }
	else
	{
    return m_pStrip->Verify(volumeDimension);
  }
} // Verify()


/** 
 * Constructor.
 */
TriangleStrip::Strip::Strip()
{
  m_iFacesNo = 0;
  m_iNumOfVertices = 0;
  m_iNumOfNormals = 0;
  m_pFaces = NULL;
  m_pVertices = NULL;
  m_pNormals = NULL;
} // Strip().


/**
 * Copy constructor.
 *
 * @param other reference to a strip.
 */
TriangleStrip::Strip::Strip(const Strip & other)
{
  // copy simple data
  m_iFacesNo = other.m_iFacesNo;
  m_iNumOfVertices = other.m_iNumOfVertices;
  m_iNumOfNormals = other.m_iNumOfNormals;

  // allocate space
  m_pFaces = new Face[m_iFacesNo];
  m_pVertices = new Point<float4>[m_iNumOfVertices];
  m_pNormals = new Vector<float4>[m_iNumOfNormals];

  // copy data
  uint4 i=0;
  for ( ; i<m_iNumOfVertices; ++i ) 
	{
    m_pVertices[i] = other.m_pVertices[i];
  } // loop over all vertices
  for ( i=0; i<m_iNumOfNormals; ++i ) 
	{
    m_pNormals[i] = other.m_pNormals[i];
  } // loop over all normals
  for ( i=0; i<m_iFacesNo; ++i ) 
	{
    m_pFaces[i] = other.m_pFaces[i];
    m_pFaces[i].ConvertIndicesIntoPointers(m_pVertices, m_pNormals);
  } // loop over all normals
} // Strip().


/**
 * Assignment constructor.
 */
TriangleStrip::Strip & TriangleStrip::Strip::operator =(const Strip & other)
{
  if ( this == &other )
    return (*this);

  // copy simple data
  m_iFacesNo = other.m_iFacesNo;
  m_iNumOfVertices = other.m_iNumOfVertices;
  m_iNumOfNormals = other.m_iNumOfNormals;

  // allocate space (before that, make sure no memory leak!)
  if (m_pFaces != NULL) delete [] m_pFaces;
  if (m_pVertices != NULL) delete [] m_pVertices;
  if (m_pNormals != NULL) delete [] m_pNormals;
  m_pFaces = new Face[m_iFacesNo];
  m_pVertices = new Point<float4>[m_iNumOfVertices];
  m_pNormals = new Vector<float4>[m_iNumOfNormals];

  // copy data
  uint4 i=0;
  for (; i<m_iNumOfVertices; ++i) 
	{
    m_pVertices[i] = other.m_pVertices[i];
  } // loop over all vertices
  for (i=0; i<m_iNumOfNormals; ++i) 
	{
    m_pNormals[i] = other.m_pNormals[i];
  } // loop over all normals
  for (i=0; i<m_iFacesNo; ++i) 
	{
    m_pFaces[i] = other.m_pFaces[i];
    m_pFaces[i].ConvertIndicesIntoPointers(m_pVertices, m_pNormals);
  } // loop over all normals
  return (* this);
} // operator=


/// Destructor.
TriangleStrip::Strip::~Strip()
{
  // free Normal buffer
  if ( m_pNormals != NULL )
  {
		delete [] m_pNormals;
	}
  // free Vertices buffer
  if ( m_pVertices != NULL )
  {
		delete [] m_pVertices;
	}
  // free face buffer
  delete [] m_pFaces;
} // ~Strip()


/**
 * Read strips from disk
 *
 * @param in  reference to istream
 * @return true when success, else, false.
 */
bool TriangleStrip::Strip::Read(std::ifstream & in)
{
  // make sure the stream is ok before reading
  if ( in.bad() ) 
	{
    throw ex::VException(LogRec("Trouble beginning to read strip file.","TriangleStrip::Strip", "Read"));
  }

  // read simple data members
  const uint4 uBufSize = 1000;
  int1 viBuffer[uBufSize];
  in >> m_iFacesNo;
  in.getline(viBuffer, uBufSize);
  in >> m_iNumOfVertices;
  in.getline(viBuffer, uBufSize);
  in >> m_iNumOfNormals;
  in.getline(viBuffer, uBufSize);

  // Read in the faces
  m_pFaces = new Face[m_iFacesNo];
  uint4 i;
  for (i=0; i<m_iFacesNo; ++i) 
	{
    // read in a face
    bool result = m_pFaces[i].Read(in);
    if ( result == false ) 
		{
      throw ex::VException(LogRec("Unable to read faces from disk.","TriangleStrip::Strip", "Read"));
    } // bad face read
  } // loop over all faces

  // read in the vertices
  m_pVertices = new Point<float4>[m_iNumOfVertices];
  in.read((int1*)(m_pVertices), m_iNumOfVertices*sizeof(Point<float4>));

  // read in the normals
  m_pNormals = new Vector<float4>[m_iNumOfNormals];
  in.read((int1*)(m_pNormals), m_iNumOfNormals*sizeof(Vector<float4>));

  // convert the face indices into pointers
  for (i=0; i<m_iFacesNo; ++i) 
	{
    m_pFaces[i].ConvertIndicesIntoPointers(m_pVertices, m_pNormals);
  } // loop over all faces

  // make sure the disk is still ok after all that reading...whew
  if ( in.bad() ) 
	{
    throw ex::VException(LogRec("Trouble reading strip file.","TriangleStrip::Strip", "Read"));
  }

  return true;
} // TriangleStrip::Strip::Read().


/**
 * Write strips from disk
 *
 * @param out  reference to ostream
 * @return true when success, else, false.
 */
bool TriangleStrip::Strip::Write(std::ofstream & out) const
{
  // make sure the stream is ok before writing
  if ( out.bad() ) 
	{
    throw ex::VException(LogRec("Trouble beginning to write strips.","TriangleStrip::Strip", "Write"));
  }

  out << m_iFacesNo << " # The number of faces" << endl;
  out << m_iNumOfVertices << " # The number of vertices" << endl;
  out << m_iNumOfNormals << " # The number of normals" << endl;

  // write out the faces
  for (uint4 uIndex=0; uIndex<m_iFacesNo; ++uIndex ) 
	{
    // write out a face
    bool bResult = m_pFaces[uIndex].Write(out);
    if ( bResult == false ) 
		{
      throw ex::VException(LogRec("Unable to write faces to disk.","TriangleStrip::Strip", "Write" ));
    }
  } // endloop (uIndex)

  // write out the vertices
  out.write((int1*)(m_pVertices), m_iNumOfVertices*sizeof(Point<float4>));

  // write out the normals
  out.write((int1*)(m_pNormals), m_iNumOfNormals*sizeof(Vector<float4>));

  // make sure the stream is still ok after writing
  if ( out.bad() ) 
	{
    throw ex::VException(LogRec("Trouble writing strips.","TriangleStrip::Strip", "Write"));
  }

  return true;
} // :Write().


/**
 * Retrieves the number of bytes of the serialized data.
 *
 * @return  data size.
 */
uint4 TriangleStrip::Strip::GetSerializedDataSize() const
{
  uint4 uSum = 0;

  // starting delimiter
  uSum += string("TriangleStrip::Strip").length() + 1;

  // int m_iFacesNo
  uSum += sizeof(int4);
  
  // int m_iNumOfVertices
  uSum += sizeof(int4);
  
  // int m_iNumOfNormals
  uSum += sizeof(int4);
  
  // Face *m_pFaces
  for (uint4 i=0; i<m_iFacesNo; i++) 
	{
    uSum += m_pFaces[i].GetSerializedDataSize();
  } // loop over all faces
  
  // Point<float> *m_pVertices
  uSum += m_iNumOfVertices * sizeof(Point<float4>);
  
  // Vector<float> *m_pNormals
  uSum += m_iNumOfNormals * sizeof(Vector<float4>);

  // ending delimiter
  uSum += string("TriangleStrip::Strip").length() + 1;

  return uSum;
} // GetSerializedDataSize()


/**
 * Retrieves all the data in serialized form.
 *
 * @param hiBuffer    2-level pointer to the memory buffer which stores the strips.
 * @param piBufferEnd pointer to the end of strips memory buffer.
 * @return true when success, else, false.
 */
bool TriangleStrip::Strip::WriteSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd) const
{
  // ensure there is enough space
  if (*hiBuffer + GetSerializedDataSize() > piBufferEnd) 
  {
    throw ex::VException(LogRec("<TriangleStrip::Strip::WriteSerializedData> Not enough space.","TriangleStrip::Strip", "WriteSerializedData"));
  } // if not enough space

  // write a delimiter to help verify the data
  SerializeStringData(hiBuffer, piBufferEnd, "TriangleStrip::Strip");

  // int m_iFacesNo
  SerializeSimpleData(hiBuffer, piBufferEnd, m_iFacesNo);
  
  // int m_iNumOfVertices
  SerializeSimpleData(hiBuffer, piBufferEnd, m_iNumOfVertices);

  // int m_iNumOfNormals
  SerializeSimpleData(hiBuffer, piBufferEnd, m_iNumOfNormals);
  
  // Face *m_pFaces
  uint4 i=0;
  for (; i<m_iFacesNo; i++) 
	{
    int1 * pBeforeWrite = *hiBuffer;
    bool bResult = m_pFaces[i].WriteSerializedData(hiBuffer, piBufferEnd);
    if (m_pFaces[i].GetSerializedDataSize() != static_cast<int4>(*hiBuffer - pBeforeWrite))
    {
      throw ex::VException(LogRec("<TriangleStrip::Strip::WriteSerializedData> Bad buffer write.","TriangleStrip::Strip", "WriteSerializedData"));
    }
    if (bResult == false) 
		{
      throw ex::VException(LogRec("<TriangleStrip::Strip::WriteSerializedData> Bad serialization.","TriangleStrip::Strip", "WriteSerializedData"));
    } // bad serialization
  } // loop over all faces
  
  // Point<float> *m_pVertices
  for (i=0; i<m_iNumOfVertices; i++) 
	{
    SerializeSimpleData(hiBuffer, piBufferEnd, m_pVertices[i]);
  } // loop over all vertices
  
  // Vector<float> *m_pNormals
  for (i=0; i<m_iNumOfVertices; i++) 
	{
    SerializeSimpleData(hiBuffer, piBufferEnd, m_pNormals[i]);
  } // loop over all vertices

  // write a delimiter to help verify the data
  SerializeStringData(hiBuffer, piBufferEnd, "TriangleStrip::Strip");

  return true;
} // TriangleStrip::Strip::WriteSerializedData()


/**
 * Reads in all the data from serialized form.
 *
 * @param hiBuffer    2-level pointer to the memory buffer which stores the strips.
 * @param piBufferEnd pointer to the end of strips memory buffer.
 * @return true when success, else, false.
 */
bool TriangleStrip::Strip::ReadSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd)
{
  // attempt to verify the stream by reading a delimiter
  try
  {
    if (DeserializeStringData(hiBuffer, piBufferEnd) != "TriangleStrip::Strip") 
      throw ex::DataCorruptionException(LogRec("data corruption data corruption: bad starting delimiter not 'TriangleStrip::Strip'.","TriangleStrip::Strip", "ReadSerializedData"));

    // int m_iFacesNo
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_iFacesNo);
  
    // int m_iNumOfVertices
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_iNumOfVertices);
  
    // int m_iNumOfNormals
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_iNumOfNormals);
  
    // Face *m_pFaces
    m_pFaces = new Face[m_iFacesNo];
	uint4 i=0;
    for (; i<m_iFacesNo; i++) 
	  {
      bool bResult = m_pFaces[i].ReadSerializedData(hiBuffer, piBufferEnd);
      if (bResult == false) // bad deserialization
        throw ex::DataCorruptionException(LogRec("data corruptiondata corruption: bad deserialization.","TriangleStrip::Strip", "ReadSerializedData"));
    } // loop over all faces
  
    // Point<float> *m_pVertices
    m_pVertices = new Point<float4>[m_iNumOfVertices];
    for (i=0; i<m_iNumOfVertices; i++) 
	  {
      DeserializeSimpleData(hiBuffer, piBufferEnd, m_pVertices[i]);
    } // loop over all vertices

    // Vector<float> *m_pNormals
    m_pNormals = new Vector<float4>[m_iNumOfNormals];
    for (i=0; i<m_iNumOfVertices; i++) 
  	{
      DeserializeSimpleData(hiBuffer, piBufferEnd, m_pNormals[i]);
    } // loop over all vertices

    for (i=0; i<m_iFacesNo; i++) 
	  {
      m_pFaces[i].ConvertIndicesIntoPointers(m_pVertices, m_pNormals);
    } // loop over all faces

    // attempt to verify the stream by reading a delimiter
    if (DeserializeStringData(hiBuffer, piBufferEnd) != "TriangleStrip::Strip") 
	  {
      throw ex::DataCorruptionException(LogRec("data corruption data corruption: bad ending delimiter: Not 'TriangleStrip::Strip'.","TriangleStrip::Strip", "ReadSerializedData"));
    }
  }
  catch (ex::OutOfMemoryException & error)
  {
    throw ex::OutOfMemoryException(LogRec("Out of memory" + error.GetLogRecord().GetTextMessage(),"TriangleStrip::Strip", "ReadSerializedData"));
  }
  catch (ex::VException & error)
  {
    throw ex::VException(LogRec(error.GetLogRecord().GetTextMessage(),"TriangleStrip::Strip", "ReadSerializedData"));
  }

  return true;
} // TriangleStrip::Strip::ReadSerializedData()


/**
 * Equality operator.
 *
 * @param other reference to a object 'strip'.
 * @return true when equal, else false;
 */
bool TriangleStrip::Strip::operator ==(const TriangleStrip::Strip & other) const
{
  // same object.
  if (this == &other) return true;

  if (m_iFacesNo != other.m_iFacesNo) // int4 m_iFacesNo
	{
    // <TriangleStrip::Strip::operator ==> facesNo varies.
    return false;
  } // if different
  
  if (m_iNumOfVertices != other.m_iNumOfVertices) // int4 m_iNumOfVertices
	{
    // <TriangleStrip::Strip::operator ==> numVertices varies.
    return false;
  } // if different
  
  if (m_iNumOfNormals != other.m_iNumOfNormals) // int4 m_iNumOfNormals
	{
    // <TriangleStrip::Strip::operator ==> numNormals varies.
    return false;
  } // if different

  uint4 i=0;
  for (; i<m_iFacesNo; i++) // Face *m_pFaces
	{
    if (!(m_pFaces[i] == other.m_pFaces[i])) 
		{
      // <TriangleStrip::Strip::operator ==> content of faces varies.
      return false;
    } // if different
  } // loop over all faces
   
  for (i=0; i<m_iNumOfVertices; i++) // Point<float> *m_pVertices
	{
    if (!(m_pVertices[i] == other.m_pVertices[i])) 
		{
      // <TriangleStrip::Strip::operator ==> content of vertices varies.
      return false;
    } // if different
  } // loop over all vertices
   
  for (i=0; i<m_iNumOfNormals; i++) // Vector<float> *m_pNormals
	{
    if (!(m_pNormals[i] == other.m_pNormals[i])) 
		{
      // <TriangleStrip::Strip::operator ==> content of normals varies.
      return false;
    } // if different
  } // loop over all normals
  
  return true;
} // TriangleStrip::Strip::operator ==


/**
 * Verify that all the vertices are inside the active volume.
 * 
 * @return true when success, else false.
 */
bool TriangleStrip::Strip::Verify(const Triple<uint4> vuVolumeDimension) const
{
  for (uint4 i=0; i<m_iNumOfVertices; i++) 
	{
    Point<float4> & vfVertex = m_pVertices[i];
    stringstream ss;
    ss << i << " " << vfVertex << endl;
    
    LogWrn(ss.str(), "TriangleStrip", "Verify");
    if (vfVertex.m_x > vuVolumeDimension.m_x-1 ||
        vfVertex.m_y > vuVolumeDimension.m_y-1 ||
        vfVertex.m_z > vuVolumeDimension.m_z-1) 
		{
      return true;
    }
  } // loop over all vertices
  return true;
} // Verify()


/// Constructor.
TriangleStrip::Face::Face()
{
  m_iTriStrip = 0;
  m_iNumOfFaces = 0;
  m_hIndex = NULL;
  m_piVertexIndex = NULL;
} // constructor


/**
 * Copy constructor
 *
 * @param other   reference to a Face object
 */
TriangleStrip::Face::Face(const TriangleStrip::Face & other)
{
  // copy simple data
  m_iTriStrip = other.m_iTriStrip;
  m_iNumOfFaces = other.m_iNumOfFaces;

  m_hIndex = new Triple<float4> * [m_iNumOfFaces*2];
  m_piVertexIndex = new uint4 [m_iNumOfFaces];

  // copy data
  uint4 i=0;
  for (; i<2*m_iNumOfFaces; ++i) 
	{
    // must be filled in later by ConvertIndicesIntoPointers()
    m_hIndex[i] = NULL;
  } // end loop over all pointers
  for (i=0; i<m_iNumOfFaces; ++i) 
	{
    m_piVertexIndex[i] = other.m_piVertexIndex[i];
  } // end loop over all indices
} // constructor


/**
 * Assignment operator
 *
 * @param other   reference of a Face object.
 */
TriangleStrip::Face & TriangleStrip::Face::operator =(const TriangleStrip::Face & other)
{
  if (this == &other)
     return (* this);

  // copy simple data
  m_iTriStrip = other.m_iTriStrip;
  m_iNumOfFaces = other.m_iNumOfFaces;

  // allocate space, before that, make sure no memory leak.
  if (m_hIndex != NULL) delete [] m_hIndex;
  if (m_piVertexIndex != NULL)  delete [] m_piVertexIndex;
  m_hIndex = new Triple<float4> * [m_iNumOfFaces*2];
  m_piVertexIndex = new uint4 [m_iNumOfFaces];

  // copy data
  uint4 i=0;
  for (; i<2*m_iNumOfFaces; ++i) 
	{
    // must be filled in later by ConvertIndicesIntoPointers()
    m_hIndex[i] = NULL;
  } // end loop over all pointers
  for (i=0; i<m_iNumOfFaces; ++i) 
	{
    m_piVertexIndex[i] = other.m_piVertexIndex[i];
  } // end loop over all indices

  return (* this);
} // operator =


/// Destructor
TriangleStrip::Face::~Face()
{
  if ( m_hIndex != NULL )
	{    
		delete [] m_hIndex;
	}
  if ( m_piVertexIndex != NULL )
  {
		delete [] m_piVertexIndex;
	}
} // destructor.


/**
 * Write faces to disk.
 */
bool TriangleStrip::Face::Write(std::ofstream & out) const
{
  // make sure the stream is ok before writing
  if (out.bad()) 
	{
    throw ex::VException(LogRec("Trouble beginning to write faces.","TriangleStrip::Face", "Write"));
  }

  out.write((int1*)(&m_iTriStrip), sizeof(int4));
  out.write((int1*)(&m_iNumOfFaces), sizeof(int4));

  // write out the indices
  out.write((int1*)(m_piVertexIndex), m_iNumOfFaces * sizeof(uint4));

  // make sure the stream is still ok after writing
  if (out.bad()) 
	{
    throw ex::VException(LogRec("Trouble writing faces.","TriangleStrip::Face", "Write"));
  }

  return true;
} // Face::Write()


/// Read faces from disk
bool TriangleStrip::Face::Read(std::ifstream & in)
{
  // read in the triangle label
  in.read((int1*)(&m_iTriStrip), sizeof(int4));

  // read in the number of faces
  in.read((int1*)(&m_iNumOfFaces), sizeof(int4));

  // allocate space for the pointers and indices
  m_hIndex = new Triple<float4>*[m_iNumOfFaces*2];
  m_piVertexIndex = new uint4 [m_iNumOfFaces];

  // write out the indices
  in.read((int1*)(m_piVertexIndex), m_iNumOfFaces*sizeof(uint4));

  return true;
} // Face::Read()


/**
 * Retrieves the number of bytes of the serialized data.
 *
 * @return  data size.
 */
uint4 TriangleStrip::Face::GetSerializedDataSize() const
{
  uint4 uSum = 0;

  uSum += sizeof(int4);  // int m_iTriStrip
  uSum += sizeof(int4);  // long m_iNumOfFaces
   
  // nothing to store here - we must reconstruct it from the indices later
  uSum += m_iNumOfFaces * sizeof(uint4);  // Triple<float> **m_pIndex
  
  return uSum;
} // TriangleStrip::Face::GetSerializedDataSize()


/**
 * Retrieves all the data in serialized form.
 */
bool TriangleStrip::Face::WriteSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd) const
{
  // ensure there is enough space
  if (*hiBuffer + GetSerializedDataSize() > piBufferEnd) 
	{
    throw ex::VException(LogRec("<TriangleStrip::Face::WriteSerializedData> Not enough space.","TriangleStrip::Face", "WriteSerializedData"));
  } // if not enough space

  SerializeSimpleData(hiBuffer, piBufferEnd, m_iTriStrip);
  SerializeSimpleData(hiBuffer, piBufferEnd, m_iNumOfFaces);
  
  // nothing to store here - we must reconstruct it from the indices later
  for (uint4 i=0; i<m_iNumOfFaces; i++) 
	{
    SerializeSimpleData(hiBuffer, piBufferEnd, m_piVertexIndex[i]);
  } // loop over all vertex indices

  return true;
} // WriteSerializedData()


/**
 * Reads in all the data from serialized form.
 *
 * @return true when succeed serialization, else, false
 */
bool TriangleStrip::Face::ReadSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd)
{
  try 
  {
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_iTriStrip);
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_iNumOfFaces);
    m_hIndex = new Triple<float4>*[m_iNumOfFaces*2];
    m_piVertexIndex = new uint4 [m_iNumOfFaces];
    // nothing to read here - we must reconstruct it from the indices later

    for (uint4 i=0; i<m_iNumOfFaces; i++) 
	  {
      DeserializeSimpleData(hiBuffer, piBufferEnd, m_piVertexIndex[i]);
    } // loop over all vertex indices
  }
  catch (ex::OutOfMemoryException & error)
  {
    throw ex::OutOfMemoryException(LogRec("Out of memory"+ error.GetLogRecord().GetTextMessage(), "Face", "ReadSerializedData"));
  }
  catch (ex::VException & ep)
  {
    throw ex::VException(LogRec(ep.GetLogRecord().GetTextMessage(),"TriangleStrip::Face", "ReadSerializedData"));
  }
  catch (...)
  {
    throw ex::VException(LogRec("An unspecified error caught at stage 'triangle:strips:faces:read'","TriangleStrip::Face", "ReadSerializedData"));
  }

  return true;
} // ReadSerializedData()


/**
 * Equality operator.
 *
 * @ param other  a reference to 'face'
 * @ return true  when equal, else, false.
 */
bool TriangleStrip::Face::operator ==(const TriangleStrip::Face & other) const
{
  // the same object.
  if (this == &other) return true;

  if (m_iTriStrip != other.m_iTriStrip) 
	{
    // <TriangleStrip::Face::operator ==> tristrip varies.
    return false;
  } // if different
  
  // long m_iNumOfFaces
  if (m_iNumOfFaces != other.m_iNumOfFaces) 
	{
    // <TriangleStrip::Face::operator ==> num varies.
    return false;
  } // if different
  
  // can't compare these directly
  for (uint4 i=0; i<m_iNumOfFaces; i++) 
	{
    if (m_piVertexIndex[i] != other.m_piVertexIndex[i]) 
		{
      // <TriangleStrip::Face::operator ==> content of vertices varies.
      return false;
    } // if different
  } // loop over all vertices
  
  return true;
} // operaor ==


/// Convert indices into much faster pointers
void TriangleStrip::Face::ConvertIndicesIntoPointers(Point<float4> * pfVertices, Vector<float4> * pfNormals)
{
  for (uint4 i=0; i<m_iNumOfFaces; ++i) 
	{
    m_hIndex[2*i  ] = reinterpret_cast<Triplef*>(pfVertices + m_piVertexIndex[i]);
    m_hIndex[2*i+1] = reinterpret_cast<Triplef*>(pfNormals  + m_piVertexIndex[i]);
  } // loop over all faces
} // ConvertIndicesIntoPointers().


/**
 * Rotate a prone TriangleStrip to supine
 *
 * @param worldSize the size of the entire volume in MM
 */
void TriangleStrip::RotateProneToSupine( const Triple<float4> & worldSize )
{

  if ( m_pStrip != NULL )
    m_pStrip->RotateProneToSupine( worldSize );

} // RotateProneToSupine()


/**
 * Rotate a prone Strip to supine
 *
 * @param worldSize the size of the entire volume in MM
 */
void TriangleStrip::Strip::RotateProneToSupine( const Triple<float4> & worldSize )
{

  for ( int4 i=0; i<m_iNumOfVertices; ++i )
  {
    m_pVertices[i].m_x = worldSize.m_x - m_pVertices[i].m_x;
    m_pVertices[i].m_y = worldSize.m_y - m_pVertices[i].m_y;
  }

  for ( int4 i=0; i<m_iNumOfNormals; ++i )
  {
    m_pNormals[i].m_x *= -1.0F;
    m_pNormals[i].m_y *= -1.0F;
  }

} // RotateProneToSupine()


// undefines
#undef FILE_REVISION 


// $Log: TriangleStrip.C,v $
// Revision 1.5  2006/10/02 21:01:03  dongqing
// using NONCOUNTMASK for the unseen value
//
// Revision 1.4  2006/10/02 20:23:37  dongqing
// change the mask for unseen label from 127 to 63.
// This is to synchronize with the change in the polyp segmentation
//
// Revision 1.3  2006/09/13 15:33:15  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.2  2006/06/01 20:28:29  frank
// updated to visual studio 2005
// bitten by suspected OpenGL driver bug
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.3.6.1  2005/04/07 19:07:31  frank
// Issue #3451: further optimized transparent marking
//
// Revision 3.3  2003/01/22 22:45:37  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.2  2002/03/11 17:39:07  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.1.2.1  2002/02/06 23:51:58  kevin
// Added color schemes to handle paintnig information for color blind people (NOTE: we should
// expand this to all our color information in the next release. e.g. centerline color, bookmark color etc)
//
// Revision 3.1  2001/10/26 02:21:07  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:28  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 01 2001 16:10:22   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.0   Sep 16 2001 23:49:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:48   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:07:24  soeren
// renamed datastruct directory
//
// Revision 1.86  2001/08/21 16:45:52  soeren
// removed some warnings
//
// Revision 1.85  2001/08/13 20:05:44  tushar
// removed errors due to the new logger
//
// Revision 1.84  2001/08/13 00:24:28  ingmar
// GetMessageString() -> GetLogRecord().GetTestMessage()
//
// Revision 1.83  2001/08/07 17:02:58  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.82  2001/07/30 15:55:05  liwei
// 6_dataStruct TransferFunction renamed to TransferFunctionVC.
//
// Revision 1.81  2001/07/26 20:28:55  binli
// ID 89: exception handling.
//
// Revision 1.80  2001/07/25 03:30:15  sachin
// fix of exceptions code compiled properly
//
// Revision 1.79  2001/07/16 23:48:30  jmeade
// VException::GetMessage --> GetMessageString
//
// Revision 1.78  2001/07/13 19:52:16  binli
// defect 547: the error message (to user) has simplified.
//
// Revision 1.77  2001/07/12 21:14:15  binli
// defect ID: 547.
// (the error message shold be simplified.)
//
// Revision 1.76  2001/07/03 14:57:36  soeren
// removed stdafx.c and .h
//
// Revision 1.75  2001/06/01 18:18:36  binli
// copy constructor of Strip&Face: no delete before new!!!
//
// Revision 1.74  2001/05/23 15:58:50  binli
// code conventions
//
// Revision 1.73  2001/05/23 14:45:38  binli
// replaced tuple with point2D
//
// Revision 1.72  2001/05/19 00:38:39  jmeade
// Keeping all exceptions localized in Exception.h
//
// Revision 1.71  2001/05/16 15:34:51  binli
// code conventions.
//
// Revision 1.70  2001/05/16 15:26:19  binli
// code conventions.
//
// Revision 1.69  2001/05/16 15:08:32  binli
// code conventions.
//
// Revision 1.68  2001/05/16 15:00:03  binli
// code conventions.
//
// Revision 1.67  2001/05/10 14:45:50  binli
// prepared for code walkthrough
//
// Revision 1.66  2001/04/09 22:25:43  dave
// changing from Abort to use exceptions
//
// Revision 1.65  2001/04/08 21:38:21  dave
// updated doc headers
//
// Revision 1.64  2001/04/05 15:23:11  binli
// name convension
//
// Revision 1.63  2001/04/04 12:09:15  frank
// Why didn't CVS accept these changes...
//
// Revision 1.62  2001/04/04 12:04:55  frank
// Further conformed to variable naming conevention.
//
// Revision 1.61  2001/04/03 21:13:39  binli
// 1st code walkthrough
//
// Revision 1.60  2001/04/03 17:08:00  binli
// added comments.
//
// Revision 1.59  2001/04/02 20:09:02  binli
// reflected "Typedef.h"
//
// Revision 1.58  2001/03/30 15:01:01  binli
// m_iNumOfFaces: long->int
//
// Revision 1.57  2001/03/30 00:05:39  jmeade
// for safety sake, longs should be changed to int4 for MS compatibility
//
// Revision 1.56  2001/03/29 16:18:21  dmitry
// Updated datatypes, implemented proper casting
//
// Revision 1.55  2001/03/27 15:12:30  binli
// reflected Typedef.h
//
// Revision 1.54  2001/03/26 17:21:10  frank
// Moved serialization functions to separate namespace.
//
// Revision 1.53  2001/03/23 16:01:46  frank
// Added some error checking to Serialize function.
//
// Revision 1.52  2001/03/21 13:00:37  frank
// Moved SEEN and NOTSEEN colors out of #define into coding compliant Triple<uint1>
//
// Revision 1.51  2001/03/20 18:52:31  binli
// code walkthrough to follow new code standard
//
// Revision 1.50  2001/03/19 18:36:43  binli
// reflected new code standard.
//
// Revision 1.49  2001/03/16 22:13:52  binli
// renamed some variables:  according to new code standard
// brackets: according to new code standard
//
// Revision 1.48  2001/03/07 17:30:03  binli
// removed the .tri file after skeletonor processing.
//
// Revision 1.47  2001/02/12 17:13:34  jeff
// throw viatronix exceptions for V1K errors instead of aborting
//
// Revision 1.46  2001/02/02 16:45:04  dave
// some Error::Warnlog removed on v2k side
//
// Revision 1.45  2001/01/31 19:03:13  kevin
// User global consts for rendering color
//
// Revision 1.44  2001/01/18 16:10:06  kevin
// Made transparent polygon render be polygon-TTP
//
// Revision 1.43  2001/01/08 16:40:45  frank
// How did this get in there?
//
// Revision 1.42  2000/12/19 17:08:30  sarang
// gcc/linux fixes
//
// Revision 1.41  2000/12/18 23:03:54  kevin
// Fixed paint vooume positional indexes since triangle verticies are now in world coordinates
//
// Revision 1.40  2000/12/18 19:51:52  frank
// Removed stringstream I/O
//
// Revision 1.39  2000/12/18 14:55:57  frank
// Added some simple verification
//
// Revision 1.38  2000/12/18 13:55:02  frank
// Cleared up paint volume size discrepancy
//
// Revision 1.37  2000/12/14 14:53:15  frank
// Modified the interface for serialization/deserialization
//
// Revision 1.36  2000/12/13 15:06:45  frank
// Added equality operators
//
// Revision 1.35  2000/12/12 16:07:48  frank
// Fixed bugs in serialization and deserialization
//
// Revision 1.34  2000/12/12 12:08:20  frank
// Added delimiters to serialized data stream
//
// Revision 1.33  2000/12/09 18:17:25  frank
// Added serialized input
//
// Revision 1.32  2000/12/09 17:49:13  frank
// Added serialized output
//
// Revision 1.31  2000/12/09 15:38:51  frank
// Updated segment reading for new file format
//
// Revision 1.30  2000/12/09 14:43:54  frank
// Stream I/O was too slow, went to temp binary file
//
// Revision 1.29  2000/12/09 14:18:45  frank
// fixed type
//
// Revision 1.28  2000/12/09 12:54:14  frank
// Improved stream I/O
//
// Revision 1.27  2000/12/08 19:24:48  binli
// added code to read header of binary triangle files
//
// Revision 1.26  2000/12/08 17:18:21  binli
// read binary strips file (with header).
//
// Revision 1.25  2000/12/05 14:45:43  frank
// Added generalized stream I/O
//
// Revision 1.24  2000/12/04 19:44:55  binli
// the triangles are described in World coordinates. So no scaling is needed.
//
// Revision 1.23  2000/11/23 21:12:26  ingmar
// made include afx.h Win32 speific
//
// Revision 1.22  2000/11/23 16:18:42  kevin
// Made painting look in neighboprhood to colorize
//
// Revision 1.21  2000/11/21 19:00:15  binli
// replaced triple vertex and normal with pointer.
//
// Revision 1.20  2000/11/21 15:00:38  binli
// removed display list for segment rendering : the method does work.
//
// Revision 1.19  2000/11/21 13:31:26  frank
// Cleaned up, tried to prevent crashes
//
// Revision 1.18  2000/11/20 18:57:52  kevin
// Changed back to Green Painting
//
// Revision 1.17  2000/11/20 03:32:28  kevin
// Switched to "paint volume" schema.
//
// Revision 1.16  2000/11/19 22:46:31  kevin
// Made RGBvol a flag
//
// Revision 1.15  2000/11/18 18:35:24  kevin
// CHanged Triangle indexing to be trunc instead of round
// (Since we have 1/2 coord on voxel coord, adding 0.5, then *2
// actually incremnets the indexes)
//
// Revision 1.14  2000/11/16 17:39:45  kevin
// Round pointer into RGB volume (You know for some unkown reason,
// this seems to cause painting problems)
//
// Revision 1.13  2000/11/13 12:11:24  frank
// Added display lists
//
// Revision 1.12  2000/11/09 19:56:48  binli
// added function 'displaystrips' (the function is extracted from RenderSegment).
// The purpose of this change is for call list and for Supine/Prone works.
//
// Revision 1.11  2000/11/03 12:59:40  frank
// Fixed triangle file path
//
// Revision 1.10  2000/11/02 17:34:33  kevin
// Added drawing with pointers into RGB vol for painting
//
// Revision 1.9  2000/10/31 17:15:22  frank
// kept I/O code
//
// Revision 1.8  2000/10/31 14:15:08  frank
// Merged in new segment structure
//
// Revision 1.7  2000/10/31 13:53:10  frank
// Merged in new skeleton and segment structure
//
// Revision 1.6.12.3  2000/10/30 19:20:27  frank
// fixed copy constructor
//
// Revision 1.6.12.2  2000/10/28 19:31:07  ingmar
// merged in changes from v1k-newSegments branch
//
// Revision 1.6.12.1  2000/10/28 17:38:59  ingmar
// new ierators for new segments
//
// Revision 1.6.8.2  2000/10/27 15:20:54  frank
// Got segment structure working
//
// Revision 1.6.8.1  2000/10/26 13:28:32  frank
// Added new segment structure
//
// Revision 1.6.2.3  2000/10/27 15:19:49  frank
// Updated to reflect v1k
//
// Revision 1.6.2.2  2000/10/26 14:30:27  frank
// Added C++
//
// Revision 1.6.2.1  2000/10/26 11:16:38  frank
// Wrote out new segment structure
//
// Revision 1.6  2000/10/02 16:10:00  frank
// Improved SegmentList I/O
//
// Revision 1.5  2000/10/02 15:51:41  ingmar
// fixed gcc complaints
//
// Revision 1.4  2000/09/30 15:01:33  frank
// Added disk I/O
//
// Revision 1.3  2000/09/28 17:10:00  frank
// Added I/O for segments and segment lists
//
// Revision 1.2  2000/09/27 18:14:18  frank
// Fixed typo
//
// Revision 1.1  2000/09/27 15:07:52  frank
// Added segment data structures
//
// Revision 1.3  2000/09/19 12:51:28  frank
// Added memory allocation check
//
// Revision 1.2  2000/09/13 15:55:32  binli
// fitted to the Viatronix C++ coding conventions and standards
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
//
// 19    9/06/00 10:29a Binli
// fitted to the viatronix standard
//
// 18    8/29/00 2:42p Binli
// adjust the data structure: struct --> class
//
// 17    8/28/00 12:10p Antonio
// updated header and footer
//
//*****************  Version 16  *****************
//User: Binli           Date:  8/22/00  Time: 10:08a
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 15  *****************
//User: Binli           Date:  8/22/00  Time: 10:07a
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 14  *****************
//User: Kevin           Date:  8/17/00  Time:  7:38p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Updated from .C to .cpp
//
//*****************  Version 13  *****************
//User: Binli           Date:  8/10/00  Time:  3:55p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 12  *****************
//User: Kevin           Date:  8/06/00  Time:  7:02p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added Picking of segments for order. To accomplish this, I
//pulled the guts out of some data structures and replaced them
//
//*****************  Version 11  *****************
//User: Binli           Date:  8/03/00  Time: 10:16a
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 10  *****************
//User: Kevin           Date:  8/02/00  Time:  5:52p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added some multi segment paraaaameters
//
//*****************  Version 9  *****************
//User: Binli           Date:  8/02/00  Time:  5:35p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 8  *****************
//User: Binli           Date:  8/02/00  Time: 10:47a
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 7  *****************
//User: Kevin           Date:  8/02/00  Time:  2:33a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Begining of multi-segmnet handling
//
//*****************  Version 6  *****************
//User: Kevin           Date:  8/02/00  Time:  1:33a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Aligned centerline with polygons and cleaned up overview a little
//
//*****************  Version 5  *****************
//User: Kevin           Date:  8/01/00  Time: 10:05p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added num segments to inf file
//
//*****************  Version 4  *****************
//User: Binli           Date:  8/01/00  Time:  2:21p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 3  *****************
//User: Binli           Date:  7/31/00  Time:  9:32a
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 2  *****************
//User: Binli           Date:  7/28/00  Time:  1:05p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 1  *****************
//User: Binli           Date:  7/28/00  Time: 10:35a
//Created TriangleStrip.cpp
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/TriangleStrip.C,v 1.5 2006/10/02 21:01:03 dongqing Exp $
// $Id: TriangleStrip.C,v 1.5 2006/10/02 21:01:03 dongqing Exp $
