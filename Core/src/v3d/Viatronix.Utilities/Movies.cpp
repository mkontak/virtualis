// $Id: Movies.cpp,v 1.2.2.1 2011/05/18 01:56:58 mkontak Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Maek Kontak] (mailto:mkontak@viatronix.com)


// include
#include "stdafx.h"


#include "Movies.h"
#include "FileExtensions.h"


// namespaces
using namespace System::IO;
using namespace Viatronix::Logging;
using namespace Viatronix::Utilities::IO;


USING_UTILITIES_NS

/**
 * Gets all the movie files from the path speicified
 *
 * @param sPath     Path to extract movie files from
 *
 * @return list of all supported movie files
 */
array<System::String ^> ^ Movies::GetFiles(String ^ sPath)
{
  array<String ^> ^ sAVIFiles = System::IO::Directory::GetFiles(sPath, String::Concat("*",FileExtensions::Avi));
  array<String ^> ^ sWMVFiles = System::IO::Directory::GetFiles(sPath, String::Concat("*",FileExtensions::Wmv));
  array<String ^> ^ sASFFiles = System::IO::Directory::GetFiles(sPath, String::Concat("*",FileExtensions::Asf));

  array<String ^> ^ sFiles  = gcnew array<String ^>(sAVIFiles->Length + sWMVFiles->Length + sASFFiles->Length);

  sAVIFiles->CopyTo(sFiles,0);
  Array::Copy(sWMVFiles,0,sFiles,sAVIFiles->Length,sWMVFiles->Length);
  Array::Copy(sASFFiles,0,sFiles,sWMVFiles->Length + sAVIFiles->Length,sASFFiles->Length);

  return sFiles;

} // GetFiles(String ^ sPath)

/**
 * Indicates if the movie format is supported by checking the extension
 */
bool Movies::Supports(String ^ sMovieFile)
{
  bool bSupports(false);

  String ^ sExtension(Path::GetExtension(sMovieFile));

  if (  String::Compare(sExtension,FileExtensions::Avi,true) == 0 ||
        String::Compare(sExtension,FileExtensions::Wmv, true) == 0 ||
        String::Compare(sExtension,FileExtensions::Asf, true) == 0 )
  {
    bSupports = true;
  }

  return bSupports;

} // Support(String ^ sMovieFile)


/**
 * Gets all the images from the movie file
 *
 * @param sMovieFile      Movie file
 * @param Callback        Callback function for status
 *
 * @return List of images from the movie
 */
ArrayList ^ Movies::GetImages(String ^ sMovieFile, MoviesExtractionCallback ^ pCallback)
{

  if ( ! File::Exists(sMovieFile) )
  {
    String ^ sErrorMessage(String::Concat("Movie file ", sMovieFile, " does not exist"));
    Log::Error(sErrorMessage, "Movies", "GetImages"); 
    throw gcnew System::IO::FileNotFoundException(sErrorMessage);
  }

  String ^ sExtension(Path::GetExtension(sMovieFile));

  if ( String::Compare(sExtension,FileExtensions::Avi,true) == 0 )
  {
    return GetAviImages(sMovieFile, pCallback);
  }
  else if ( String::Compare(sExtension,FileExtensions::Wmv, true) == 0 ||
            String::Compare(sExtension,FileExtensions::Asf, true) == 0 )
  {
    return GetWmvImages(sMovieFile, pCallback);
  }
  else
  {
    Log::Error("Unsupported movie format", "Movies", "GetImages"); 
    throw gcnew System::NotSupportedException("Unsupported movie format");
  }

} // GetImages(String ^ sMovieFile)

/**
 * Retreives all the images from the AVI file
 *
 * @param sMovieFile      Movie file
 * @param Callback        Callback function for status
 *
 * @return List of images from the movie
 */
ArrayList ^ Movies::GetAviImages(String ^ sMovieFile, MoviesExtractionCallback ^ pCallback)
{
  ArrayList ^ pList(gcnew ArrayList());

  PAVIFILE pFile;

  AVIFileInit();

  int iRC(S_OK);

  if ( (iRC = AVIFileOpen(&pFile, ss(sMovieFile).c_str(), OF_READ, NULL)) != S_OK )
  {
    AVIFileExit();
    throw gcnew IOException(String::Format("Could not open the AVI file 0x{0:X}", (iRC)));
  }

  AVIFILEINFO fileInfo;

  if ( (iRC = AVIFileInfo(pFile, &fileInfo, sizeof(AVIFILEINFO))) != S_OK )
  {
    AVIFileClose(pFile);
    AVIFileExit();
    throw gcnew IOException(String::Format("Could not extract file information 0x{0:X}", (iRC)));
  }

  PAVISTREAM pStream;

  if ( (iRC = AVIFileGetStream(pFile, &pStream, streamtypeVIDEO, 0)) != S_OK )
  {
    AVIFileClose(pFile);
    AVIFileExit();
    throw gcnew IOException(String::Format("Failed to open the video stream of the AVI file0x{0:X}", (iRC)));
  }

  AVISTREAMINFO streamInfo;
  if ( (iRC = AVIStreamInfo(pStream, &streamInfo, sizeof(AVISTREAMINFO))) != S_OK )
  {
    AVIStreamClose(pStream);
    AVIFileClose(pFile);
    AVIFileExit();
    throw gcnew IOException(String::Format("Could not extract the stream information 0x{0:X}", (iRC)));
  }

  int iTotalFrames(streamInfo.dwLength);
  int iFramesExtracted(0);

  if ( pCallback != __nullptr )
  {
    pCallback->Invoke(iTotalFrames, iFramesExtracted);
  }

  PGETFRAME pFrame = AVIStreamGetFrameOpen(pStream, NULL);

  HDRAWDIB hdd = DrawDibOpen();								// Handle For Our Dib
  HBITMAP hBitmap = NULL;

  try
  {

    for ( unsigned int i(0); i < streamInfo.dwLength; ++i )
    {

      unsigned char * pDib = reinterpret_cast<unsigned char *>(AVIStreamGetFrame(pFrame, i));


      LPBITMAPINFOHEADER pHeader((LPBITMAPINFOHEADER)(pDib));

      /// bitmap data buffer
      unsigned char * data = 0;

      /// Creates A Compatible Device Context
      HDC hdc = CreateCompatibleDC(0);					

      /// Create the bitmap to write into
	    hBitmap = CreateDIBSection (hdc, (BITMAPINFO *)pHeader, DIB_RGB_COLORS, (void**)(&data), NULL, NULL);

      // Select hBitmap Into the Device Context (hdc)
	    SelectObject (hdc, hBitmap);					

 
      /// Starting image
      unsigned char * pStart(pDib + pHeader->biSize + (pHeader->biClrUsed * sizeof(RGBQUAD)));

	    // Convert Data To Requested Bitmap Format
	    DrawDibDraw (hdd, hdc, 0, 0, pHeader->biWidth, pHeader->biHeight, (LPBITMAPINFOHEADER)(pDib), pStart, 0, 0, pHeader->biWidth, pHeader->biHeight, 0);



      pList->Add(Bitmap::FromHbitmap(System::IntPtr(hBitmap)));

      DeleteObject(hBitmap);

      hBitmap = NULL;

      ++iFramesExtracted;

      if ( pCallback != __nullptr )
      {
        pCallback->Invoke(iTotalFrames, iFramesExtracted);
      }



    } // END ... for each frame in the stream


  }
  catch ( Exception ^ ex)
  {
    Log::Error(String::Concat("Failed to get the AVI frames : ", ex->Message), "Movies", "GetAVIImages"); 
    throw;
  }
  __finally 
  {

    if ( hBitmap != NULL ) DeleteObject(hBitmap);

    /// Close Dib
    DrawDibClose(hdd);

    // Close the Stream Frame
    AVIStreamGetFrameClose(pFrame);

    // Close the Stream
    AVIStreamClose(pStream);

    // Close the file
    AVIFileClose(pFile);

    // Exit the AVI library
    AVIFileExit();

  }



  return pList;
} // GetAviImages(String ^ sMovieFile)


/**
 * Retreives all the images from the WMV file
 *
 * @param sMovieFile      Movie file
 * @param Callback        Callback function for status
 *
 * @return List of images from the movie
 */
ArrayList ^ Movies::GetWmvImages(String ^ sMovieFile, MoviesExtractionCallback ^ pCallback)
{

  ArrayList ^ pList(gcnew ArrayList());

  HRESULT hr(S_OK);


  IWMSyncReader * pSyncReader(__nullptr);
  INSSBuffer *    pSampleBuffer(__nullptr);
  QWORD cnsSampleTime(0);
  QWORD cnsSampleDuration(0);
  DWORD dwFlags(0);
  DWORD dwOutputNumber(0);

  // Initialize COM.
  hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
  if(	FAILED(	hr ) )
  {

      String ^ sErrorMessage(String::Format("Could not initialize COM [ERROR={0:X}]", (int)hr));
      Log::Error(sErrorMessage, "Movies", "GetmvImages"); 
      throw gcnew System::Exception(sErrorMessage);
  }

  // Create	a	reader object, requesting	only playback	rights.
  hr = WMCreateSyncReader( NULL, WMT_RIGHT_PLAYBACK, &pSyncReader	);
  if(	FAILED(	hr ) )
  {
    String ^ sErrorMessage(String::Format("Could not create	Sync Reader [ERROR=0x{0:X}]", (int)hr));
    Log::Error(sErrorMessage, "Movies", "GetmvImages"); 
    throw gcnew System::Exception(sErrorMessage);
  }

  /// Open the file
  hr = pSyncReader->Open(ws(sMovieFile).c_str());
  if ( FAILED(hr) )
  {
    String ^ sErrorMessage(String::Format("Failed to open movie file [FILE={0}]  [ERROR=0x{1:X}]", sMovieFile, (int)hr));
    Log::Error(sErrorMessage, "Movies", "GetmvImages"); 
    throw gcnew System::Exception(sErrorMessage);

  }


  DWORD dwOutputCount(0);
  pSyncReader->GetOutputCount(&dwOutputCount);


  for ( unsigned int i(0); i < dwOutputCount; ++i )
  {

    IWMOutputMediaProps * pProps(NULL);

    hr = pSyncReader->GetOutputProps(i, &pProps);
    if ( FAILED(hr) )
    {
      String ^ sErrorMessage(String::Format("Failed to get output properties for output [{0}]  [ERROR=0x{1:X}]", (i), ((int)hr)));
      Log::Error(sErrorMessage, "Movies", "GetmvImages"); 
      throw gcnew System::Exception(sErrorMessage);
    } // END ... If the GetOutputProps() failed


    WORD wStreamNumber(0);
    hr = pSyncReader->GetStreamNumberForOutput(i, &wStreamNumber);
    if ( FAILED(hr) )
    {
      String ^ sErrorMessage(String::Format("Failed to get stream number for output [{0}]  [ERROR=0x{1:X}]", (i), ((int)hr)));
      Log::Error(sErrorMessage, "Movies", "GetmvImages"); 
      throw gcnew System::Exception(sErrorMessage);
    } // END ... If the GetStreamNumber() failed

    WM_MEDIA_TYPE * pMediaType;
    DWORD dwSize(0);

    hr = pProps->GetMediaType(NULL, &dwSize);
    pMediaType = (WM_MEDIA_TYPE *)malloc(sizeof(char) * dwSize);

    hr = pProps->GetMediaType(pMediaType, &dwSize);
    if ( FAILED(hr) )
    {
      String ^ sErrorMessage(String::Format("Failed to get media type for output [{0}]  [ERROR=0x{1:X}]", (i), ((int)hr)));
      Log::Error(sErrorMessage, "Movies", "GetmvImages"); 
      throw gcnew System::Exception(sErrorMessage);
    } // END ... If the GetMediaType() failed



    if ( pMediaType->majortype == WMMEDIATYPE_Video || pMediaType->majortype == WMMEDIATYPE_Image )
    {

      WORD wVideoStream(wStreamNumber);

      String ^ sErrorMessage(String::Format("Stream [{0}] for output [{1}] contains a video format, processing", (wVideoStream), (i)));
      Log::Information(sErrorMessage, "Movies", "GetWmvImages");

      /// Video Header Information Structure
      WMVIDEOINFOHEADER * pInfoHeader = (WMVIDEOINFOHEADER *)pMediaType->pbFormat;

      System::Drawing::Imaging::PixelFormat pixelFormat;
      if (  pMediaType->subtype == WMMEDIASUBTYPE_RGB555 )
      {
        Log::Debug(String::Format("Stream {0} for output {1} contains a video format of type RGB555", (wVideoStream), (i)), "Movies", "GetWmvImages");
        pixelFormat = System::Drawing::Imaging::PixelFormat::Format16bppRgb555;
      }
      else if ( pMediaType->subtype == WMMEDIASUBTYPE_RGB24 )
      {
        Log::Debug(String::Format("Stream {0} for output {1} contains a video format of type RGB24", (wVideoStream), (i)), "Movies", "GetWmvImages");
        pixelFormat = System::Drawing::Imaging::PixelFormat::Format24bppRgb;
      }
      else if ( pMediaType->subtype == WMMEDIASUBTYPE_RGB32 )
      {
        Log::Debug(String::Format("Stream {0} for output {1} contains a video format of type RGB32", (wVideoStream), (i)), "Movies", "GetWmvImages");
        pixelFormat = System::Drawing::Imaging::PixelFormat::Format32bppRgb;
      }
      else 
      {
        array<unsigned char> ^ data4 = gcnew array<unsigned char>(8);
        for ( int b(0);b < 8; data4[b] = pMediaType->subtype.Data4[b], ++b);

        String ^ sErrorMessage(String::Format("Unsupported sub type [{0}] for output [{1}]", System::Guid(pMediaType->subtype.Data1, pMediaType->subtype.Data2, pMediaType->subtype.Data3, data4).ToString(),(i)));
        Log::Error(sErrorMessage, "Movies", "GetWmvImages");
        throw gcnew System::Exception(sErrorMessage);
      } 

      /// Calculate the iStride
      int iStride = pInfoHeader->bmiHeader.biWidth * pInfoHeader->bmiHeader.biPlanes * pInfoHeader->bmiHeader.biBitCount / 8;


      hr = pSyncReader->SetRange(0,0);

      int iSample(1);

      /////////////////////////////////////////////////////////
      // Loop through all the samples in the specified range.
      /////////////////////////////////////////////////////////
      do
      {

        Log::Debug(String::Format("Getting sample [{0}] from stream [{1}]", (iSample), (wVideoStream)), "Movies", "GetWmvImages");
        iSample++;

        // Get the next sample, regardless of its stream number.
        hr = pSyncReader->GetNextSample(wVideoStream, &pSampleBuffer, &cnsSampleTime, &cnsSampleDuration, &dwFlags, &dwOutputNumber, &wStreamNumber);
        if ( hr == NS_E_NO_MORE_SAMPLES )
        {
          break;
        }
          
        if ( FAILED(hr) )
        {
          String ^ sErrorMessage(String::Format("Failed to get next sample [ERROR=0x{0:X}]", ((int)hr)));
          Log::Error(sErrorMessage, "Movies", "GetWmvImages");
          throw gcnew System::Exception(sErrorMessage);
        } // END ... If the GetNextSample() failed

        BYTE * pBuffer;
        DWORD dwLength;

        hr = pSampleBuffer->GetBufferAndLength(&pBuffer, &dwLength);
        if ( FAILED(hr) )
        {
          String ^ sErrorMessage(String::Format("Failed to get buffer from sample [ERROR=0x{1:X}]", ((int)hr)));
          Log::Error(sErrorMessage, "Movies", "GetWmvImages");
          throw gcnew System::Exception(sErrorMessage);
        } // END ... If the GetBuffer() failed

        char * pImageBuffer = new char [dwLength];
        for( register int k(0); k < dwLength; pImageBuffer[k] = pBuffer[k], ++k);

        Bitmap ^ pBitmap = gcnew Bitmap(pInfoHeader->bmiHeader.biWidth, 
                                      pInfoHeader->bmiHeader.biHeight,
                                      iStride,
                                      pixelFormat,
                                      System::IntPtr(pImageBuffer) );

        pBitmap->RotateFlip(RotateFlipType::RotateNoneFlipY);

        pList->Add(pBitmap);

        pSampleBuffer->Release();
        pSampleBuffer = NULL;
        cnsSampleTime     = 0;
        cnsSampleDuration = 0;
        dwFlags           = 0;
        dwOutputNumber    = 0;

      } while (SUCCEEDED(hr));

    }
    else
    {
      String ^ sErrorMessage(String::Format("Stream [{0}] for output [{1}] does not contain a video format, skipping", (wStreamNumber), (i)));
      Log::Debug(sErrorMessage, "Movies", "GetWmvImages");
    }
  }

  pSyncReader->Release();
  pSyncReader = NULL;



  return pList;


} // GetWmvImages(String ^ sMovieFile, MoviesExtractionCallback ^ pCallback)



// $Log: Movies.cpp,v $
// Revision 1.2.2.1  2011/05/18 01:56:58  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.2  2007/03/09 21:07:03  mkontak
// Coding standards
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Movies.cpp,v 1.2.2.1 2011/05/18 01:56:58 mkontak Exp $
// $Id: Movies.cpp,v 1.2.2.1 2011/05/18 01:56:58 mkontak Exp $

