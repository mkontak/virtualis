// $Id: MovieExtracter.cpp,v 1.2.2.1 2011/05/18 01:56:58 mkontak Exp $
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


#include "MovieExtracter.h"
#include "FileExtensions.h"


// namespaces
using namespace System;
using namespace System::Text;
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
cli::array<System::String ^> ^ MovieExtracter::GetFiles(String ^ mpPath)
{
  cli::array<String ^> ^ mpAVIFiles = System::IO::Directory::GetFiles(mpPath, String::Concat("*",FileExtensions::Avi));
  cli::array<String ^> ^ mpWMVFiles = System::IO::Directory::GetFiles(mpPath, String::Concat("*", FileExtensions::Wmv));
  cli::array<String ^> ^ mpASFFiles = System::IO::Directory::GetFiles(mpPath, String::Concat("*", FileExtensions::Asf));

  cli::array<String ^> ^ mpFiles = gcnew cli::array<String ^>(mpAVIFiles->Length + mpWMVFiles->Length + mpASFFiles->Length);

  mpAVIFiles->CopyTo(mpFiles, 0);
  Array::Copy(mpWMVFiles, 0, mpFiles, mpAVIFiles->Length, mpWMVFiles->Length);
  Array::Copy(mpASFFiles, 0, mpFiles, mpWMVFiles->Length + mpAVIFiles->Length, mpASFFiles->Length);

  return mpFiles;
} // GetFiles( mpPath )


/**
 * Indicates if the movie format is supported by checking the extension
 */
bool MovieExtracter::Supports(String ^ mpMovieFile)
{
  bool bSupports(false);

  String ^ mpExtension(Path::GetExtension(mpMovieFile));

  if (  String::Compare(mpExtension, FileExtensions::Avi, true) == 0 ||
        String::Compare(mpExtension, FileExtensions::Wmv, true) == 0 ||
        String::Compare(mpExtension, FileExtensions::Asf, true) == 0 )
  {
    bSupports = true;
  }

  return bSupports;
} // Support( mpMovieFile )


/**
 * Gets all the images from the movie file
 *
 * @param mpMovieFile      Movie file
 * @param Callback        Callback function for status
 *
 * @return List of images from the movie
 */
void MovieExtracter::ExtractFrames(String ^ mpMovieFile)
{

  if ( ! File::Exists(mpMovieFile) )
  {
    String ^ mpErrorMessage(String::Concat("Movie file ", mpMovieFile, " does not exist"));
    Log::Error(mpErrorMessage, "MovieExtracter", "GetImages"); 
    throw gcnew System::IO::FileNotFoundException(mpErrorMessage);
  }

  String ^ mpExtension(Path::GetExtension(mpMovieFile));

  if ( String::Compare(mpExtension, FileExtensions::Avi, true) == 0 )
  {
    AviExtractFrames(mpMovieFile);
  }
  else if ( String::Compare(mpExtension, FileExtensions::Wmv, true) == 0 ||
            String::Compare(mpExtension, FileExtensions::Asf, true) == 0 )
  {
    WmvExtractFrames(mpMovieFile);
  }
  else
  {
    Log::Error("Unsupported movie format", "MovieExtracter", "GetImages"); 
    throw gcnew System::NotSupportedException("Unsupported movie format");
  }

} // ExtractFrames( mpMovieFile )

/**
 * Extract all the frames from the AVI file
 *
 * @param sMovieFile      Movie file
 * @param Callback        Callback function for status
 *
 * @return List of images from the movie
 */
void MovieExtracter::AviExtractFrames(String ^ mpMovieFile)
{

  PAVIFILE pAviFile(NULL);

  AVIFileInit();

  int iRC(AVIERR_OK);
  HBITMAP hBitmap(NULL);

  unsigned char * data(__nullptr);
  unsigned char * pDib(__nullptr);
  PAVISTREAM pAviStream(NULL);
  PGETFRAME pAviFrame(NULL);
  String ^ mpErrorMessage(String::Empty);

  /// Open the DIB drawing handle
  HDRAWDIB hdd(DrawDibOpen());

  /// Create the Device COntext
  HDC hdc(CreateCompatibleDC(0));


  try
  {
    // Opeb th AVI file
    if ( (iRC = AVIFileOpen(&pAviFile, ss(mpMovieFile).c_str(), OF_READ, NULL)) != AVIERR_OK )
      throw gcnew IOException(String::Format("Could not open the AVI file 0x{0:X}", Convert::ToString(iRC)));

    AVIFILEINFO fileInfo;

    // Get the File AVI file infor
    if ( (iRC = AVIFileInfo(pAviFile, &fileInfo, sizeof(AVIFILEINFO))) != AVIERR_OK )
      throw gcnew IOException(String::Format("Could not extract file information 0x{0:X}", Convert::ToString(iRC)));


    if ( fileInfo.dwLength > 2205 )
      throw gcnew Exception("AVI movies greater the 2200 frames are not supported, must be converted to WMV format. Please contact customer support.");

    System::Text::StringBuilder ^ mpBuilder = gcnew System::Text::StringBuilder();
    mpBuilder->Append("AVI File Information\n");
    mpBuilder->Append(String::Format("Dimension:  {0}x{1}\n", Convert::ToString((unsigned int)fileInfo.dwHeight), Convert::ToString((unsigned int)fileInfo.dwWidth)));
    mpBuilder->Append(String::Format("Length:  {0} frames\n", Convert::ToString((unsigned int)fileInfo.dwLength)));
    mpBuilder->Append(String::Format("Max Bytes/Second:  {0}\n", Convert::ToString((unsigned int)fileInfo.dwMaxBytesPerSec)));
    mpBuilder->Append(String::Format("Samples/Second:  {0}\n", Convert::ToString((unsigned int)(fileInfo.dwRate / fileInfo.dwScale))));
    mpBuilder->Append(String::Format("Streams:  {0}\n", Convert::ToString((unsigned int)fileInfo.dwStreams)));
    mpBuilder->Append(String::Format("File Type:  {0}", gcnew String(fileInfo.szFileType)));


    Log::Debug(mpBuilder->ToString(), "MovieExtracter", "ExtractAviFrames");

    // Open the AVI file stream
    if ( (iRC = AVIFileGetStream(pAviFile, &pAviStream, streamtypeVIDEO, 0)) != AVIERR_OK )
       throw gcnew IOException(String::Format("Failed to open the video stream of the AVI file0x{0:X}", Convert::ToString(iRC)));

    AVISTREAMINFO streamInfo;

    // Get the AVI stream infor
    if ( (iRC = AVIStreamInfo(pAviStream, &streamInfo, sizeof(AVISTREAMINFO))) != AVIERR_OK )
       throw gcnew IOException(String::Format("Could not extract the stream information 0x{0:X}", Convert::ToString(iRC)));

    // Set the total frames in the AVI stream
    const unsigned int iTotalFrames(streamInfo.dwLength);
 
    OnExtractionStarted(iTotalFrames);

    /// Get the frame stream
    pAviFrame = AVIStreamGetFrameOpen(pAviStream,  PBITMAPINFOHEADER(AVIGETFRAMEF_BESTDISPLAYFMT));

    
    register int iFrame(1);

    // =============================================
    // Loop through all the frames in the AVI stream
    // ==================================================
    for ( register unsigned int i = 0; i < streamInfo.dwLength; ++i, ++iFrame )
    {

      Log::Debug(String::Format("Extracting frame {0} or {1} ", Convert::ToString(iFrame), Convert::ToString(iTotalFrames)), "MovieExtracter", "ExtractAviFrames");

      // Get a pointer to the next frame
      for ( int retry(0); retry < 5; ++retry)
      {
        if ( ( pDib = reinterpret_cast<unsigned char *>(AVIStreamGetFrame(pAviFrame, i))) != __nullptr )
          break;
       }

      if ( pDib == nullptr )
        throw gcnew System::Exception(String::Format("Failed to retrieve avi frame {0} or {1}, NULL returned", Convert::ToString(iFrame), Convert::ToString(iTotalFrames)));

   
      LPBITMAPINFOHEADER pHeader((LPBITMAPINFOHEADER)(pDib));

      data = __nullptr;

      /// Create the bitmap to write into
	    hBitmap = CreateDIBSection (hdc, (BITMAPINFO *)pHeader, DIB_RGB_COLORS, (void**)(&data), NULL, NULL);

      // Select hBitmap Into the Device Context (hdc)
	    SelectObject (hdc, hBitmap);					

 
      /// Starting image
      unsigned char * pStart(pDib + pHeader->biSize + (pHeader->biClrUsed * sizeof(RGBQUAD)));

	    // Convert Data To Requested Bitmap Format
	    DrawDibDraw (hdd, hdc, 0, 0, pHeader->biWidth, pHeader->biHeight, pHeader, pStart, 0, 0, pHeader->biWidth, pHeader->biHeight, 0);

      /// Create the bitmap
      Bitmap ^ mpBitmap = Bitmap::FromHbitmap(System::IntPtr(hBitmap));

      // Cleanup the DIB memeory
      DeleteObject(hBitmap);
      hBitmap = NULL;


      ExtractionEventArgs ^ mpArgs = gcnew ExtractionEventArgs(iFrame, iTotalFrames, mpBitmap);
      OnFrameExtracted(mpArgs);
     

      // Dispose of the bitmap
      delete mpBitmap;
      mpBitmap = nullptr;

      // If error message contains data then throw exception
      if ( mpArgs->Cancel ) 
        throw gcnew Exception(mpArgs->Message);


    } // END ... for each frame in the stream

    OnExtractionCompleted(iTotalFrames);
  }
  catch ( Exception ^ ex)
  {
    Log::Error(String::Concat("Failed to extract the AVI frames : ", ex->Message), "MovieExtracter", "ExtractAviFrames"); 
    OnExtractionFailed(ex->Message);
    throw;
  }
  finally 
  {

    // Delete the bitmap object
    if ( hBitmap != NULL ) 
      DeleteObject(hBitmap);

    // Delete the DC
    DeleteDC(hdc);

    /// Close Dib
    DrawDibClose(hdd);

    // Close the Stream Frame
    if ( pAviFrame != NULL )
      AVIStreamGetFrameClose(pAviFrame);

    // Close the Stream
    if ( pAviStream != NULL )
      AVIStreamClose(pAviStream);

    // Close the file
    if ( pAviFile!=NULL )
       AVIFileRelease(pAviFile);

    // Exit the AVI library
    AVIFileExit();

  }


} // ExtractAviFrames( mpMovieFile )


/**
 * Retreives all the images from the WMV file
 *
 * @param mpMovieFile      Movie file
 * @param Callback        Callback function for status
 *
 * @return List of images from the movie
 */
void MovieExtracter::WmvExtractFrames(String ^ sMovieFile)
{

 
  HRESULT hr(S_OK);

  unsigned int iTotalFrames(9999);

  IWMSyncReader * pSyncReader(NULL);
  INSSBuffer * pSampleBuffer(NULL);
  QWORD cnsSampleTime(0);
  QWORD cnsSampleDuration(0);
  DWORD dwFlags(0);
  DWORD dwOutputNumber(0);


  try
  {
    // Initialize COM.
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if(	FAILED(	hr ) )
      throw gcnew System::Exception(String::Format("Could not initialize COM [ERROR={0:X}]", Convert::ToString((int)hr)));
     // Create	a	reader object, requesting	only playback	rights.
    hr = WMCreateSyncReader( NULL, WMT_RIGHT_PLAYBACK, &pSyncReader	);
    if(	FAILED(	hr ) )
      throw gcnew System::Exception(String::Format("Could not create	Sync Reader [ERROR=0x{0:X}]", Convert::ToString((int)hr)));
 
    /// Open the file
    hr = pSyncReader->Open(ws(sMovieFile).c_str());
    if ( FAILED(hr) )
      throw gcnew System::Exception(String::Format("Failed to open movie file [FILE={0}]  [ERROR=0x{1:X}]", sMovieFile, Convert::ToString((int)hr)));

    DWORD dwOutputCount(0);
    pSyncReader->GetOutputCount(&dwOutputCount);

    OnExtractionStarted(iTotalFrames);


    for ( unsigned int i(0); i < dwOutputCount; ++i )
    {

      IWMOutputMediaProps * pProps(NULL);

      hr = pSyncReader->GetOutputProps(i, &pProps);
      if ( FAILED(hr) )
        throw gcnew System::Exception(String::Format("Failed to get output properties for output [{0}]  [ERROR=0x{1:X}]", Convert::ToString(i), Convert::ToString((int)hr)));
 
      WORD wStreamNumber(0);
      hr = pSyncReader->GetStreamNumberForOutput(i, &wStreamNumber);
      if ( FAILED(hr) )
        throw gcnew System::Exception(String::Format("Failed to get stream number for output [{0}]  [ERROR=0x{1:X}]", Convert::ToString(i), Convert::ToString((int)hr)));

      WM_MEDIA_TYPE * pMediaType;
      DWORD dwSize(0);

      // Gets the type of media first
      hr = pProps->GetMediaType(NULL, &dwSize);
      pMediaType = (WM_MEDIA_TYPE *)malloc(sizeof(char) * dwSize);

      // Gets the properties for the media type returned from the call above
      hr = pProps->GetMediaType(pMediaType, &dwSize);
      if ( FAILED(hr) )
        throw gcnew System::Exception(String::Format("Failed to get media type for output [{0}]  [ERROR=0x{1:X}]", Convert::ToString(i), Convert::ToString((int)hr)));

      // ============================================
      // Only support media type of video and image
      // ============================================
      if ( pMediaType->majortype == WMMEDIATYPE_Video || pMediaType->majortype == WMMEDIATYPE_Image )
      {

        WORD wVideoStream(wStreamNumber);

        Log::Information(String::Format("Stream [{0}] for output [{1}] contains a video format, processing", Convert::ToString(wVideoStream), Convert::ToString(i)), "MovieExtracter", "WmvExtractFrames");

        /// Video Header Information Structure
        WMVIDEOINFOHEADER * pInfoHeader = (WMVIDEOINFOHEADER *)pMediaType->pbFormat;

        System::Drawing::Imaging::PixelFormat pixelFormat;
        if (  pMediaType->subtype == WMMEDIASUBTYPE_RGB555 )
        {
          Log::Debug(String::Format("Stream {0} for output {1} contains a video format of type RGB555", Convert::ToString(wVideoStream), Convert::ToString(i)), "MovieExtracter", "WmvExtractFrames");
          pixelFormat = System::Drawing::Imaging::PixelFormat::Format16bppRgb555;
        }
        else if ( pMediaType->subtype == WMMEDIASUBTYPE_RGB24 )
        {
          Log::Debug(String::Format("Stream {0} for output {1} contains a video format of type RGB24", Convert::ToString(wVideoStream), Convert::ToString(i)), "MovieExtracter", "WmvExtractFrames");
          pixelFormat = System::Drawing::Imaging::PixelFormat::Format24bppRgb;
        }
        else if ( pMediaType->subtype == WMMEDIASUBTYPE_RGB32 )
        {
          Log::Debug(String::Format("Stream {0} for output {1} contains a video format of type RGB32", Convert::ToString(wVideoStream), Convert::ToString(i)), "MovieExtracter", "WmvExtractFrames");
          pixelFormat = System::Drawing::Imaging::PixelFormat::Format32bppRgb;
        }
        else 
        {
          cli::array<unsigned char> ^ data4 = gcnew cli::array<unsigned char>(8);
          for ( int b(0);b < 8; data4[b] = pMediaType->subtype.Data4[b], ++b);
          throw gcnew System::Exception(String::Format("Unsupported sub type [{0}] for output [{1}]", System::Guid(pMediaType->subtype.Data1, pMediaType->subtype.Data2, pMediaType->subtype.Data3, data4).ToString(), Convert::ToString(i)));
        } 

        /// Calculate the iStride
        int iStride = pInfoHeader->bmiHeader.biWidth * pInfoHeader->bmiHeader.biPlanes * pInfoHeader->bmiHeader.biBitCount / 8;


        hr = pSyncReader->SetRange(0,0);

        unsigned int iSample(1);

        /////////////////////////////////////////////////////////
        // Loop through all the samples in the specified range.
        /////////////////////////////////////////////////////////
        do
        {

          Log::Debug(String::Format("Getting sample [{0}] from stream [{1}]", Convert::ToString(iSample), Convert::ToString(wVideoStream)), "MovieExtracter", "WmvExtractFrames");
  
          // Get the next sample, regardless of its stream number.
          hr = pSyncReader->GetNextSample(wVideoStream, &pSampleBuffer, &cnsSampleTime, &cnsSampleDuration, &dwFlags, &dwOutputNumber, &wStreamNumber);
          if ( hr == NS_E_NO_MORE_SAMPLES )
            break;
          
          if ( FAILED(hr) )
          {
            pSyncReader->Close();
            pSyncReader->Release();

            String ^ mpErrorMessage(String::Format("Failed to get next sample [ERROR=0x{0:X}]", Convert::ToString((int)hr)));
            Log::Error(mpErrorMessage, "MovieExtracter", "WmvExtractFrames");
            throw gcnew System::Exception(mpErrorMessage);
          } // END ... If the GetNextSample() failed

          BYTE * pBuffer;
          DWORD dwLength;

          hr = pSampleBuffer->GetBufferAndLength(&pBuffer, &dwLength);
          if ( FAILED(hr) )
          {
            pSampleBuffer->Release();
            pSyncReader->Close();
            pSyncReader->Release();

            String ^ mpErrorMessage(String::Format("Failed to get buffer from sample [ERROR=0x{1:X}]", Convert::ToString((int)hr)));
            Log::Error(mpErrorMessage, "MovieExtracter", "WmvExtractFrames");
            throw gcnew System::Exception(mpErrorMessage);
          } // END ... If the GetBuffer() failed

          unsigned char * buffer = new unsigned  char [dwLength];    
          for( register int k(0); k < dwLength; buffer[k] = pBuffer[k], ++k );

 
          unsigned char * ptr(buffer + (iStride * ( pInfoHeader->bmiHeader.biHeight - 1 ) ) );
          Bitmap ^ mpBitmap = gcnew Bitmap(pInfoHeader->bmiHeader.biWidth, 
                                        pInfoHeader->bmiHeader.biHeight,
                                        iStride,
                                        pixelFormat,
                                        System::IntPtr( buffer ) );

          mpBitmap->RotateFlip(RotateFlipType::RotateNoneFlipY);


          // DEBUG (Uncomment to save bitmap to file for testing)
          //pBitmap->Save("D:\\Temp\\Test.bmp");

 
          pSampleBuffer->Release();
          pSampleBuffer     = NULL;
          cnsSampleTime     = 0;
          cnsSampleDuration = 0;
          dwFlags           = 0;
          dwOutputNumber    = 0;
          delete [] buffer;

          ExtractionEventArgs ^ mpArgs = gcnew ExtractionEventArgs(iSample, iTotalFrames, mpBitmap);
          OnFrameExtracted(mpArgs);
 
          delete mpBitmap;
          
          if ( mpArgs->Cancel )
            throw gcnew Exception(mpArgs->Message);
              
          iSample++;

        } while (SUCCEEDED(hr));


        OnExtractionCompleted(iSample);

      }
      else
      {
        String ^ mpMessage(String::Format("Stream [{0}] for output [{1}] does not contain a video format, skipping", Convert::ToString(wStreamNumber), Convert::ToString(i)));
        OnExtractionFailed(mpMessage);
        Log::Debug(mpMessage, "MovieExtracter", "WmvExtractFrames");
      }

    }


  }
  catch ( Exception ^ ex )
  {
    Log::Error(String::Format("Movie extraction failed : {0}", ex->Message), "MovieExtracter", "WmvExtractFrames");
 
    throw;
  }
  finally
  {
    if ( pSyncReader != NULL )
    {
      pSyncReader->Close();
      pSyncReader->Release();
    }

    if (pSampleBuffer != NULL)
    {
      pSampleBuffer->Release();
    }
  }
} // WmvExtractFrames(String * sMovieFile, MovieExtracterExtractionCallback * pCallback)


/**
 * Fires the OnStart event.
 */
void MovieExtracter::OnExtractionStarted(UInt32 iTotalFrames)
{
  MovieExtracter::ExtractionStarted(this, gcnew ExtractionEventArgs(0, iTotalFrames, nullptr));
} // OnStart()


/**
 * Fires the OnComplete event.
 */
void MovieExtracter::OnExtractionCompleted(UInt32 iTotalFrames)
{
  MovieExtracter::ExtractionCompleted(this, gcnew ExtractionEventArgs(iTotalFrames, iTotalFrames, nullptr));
} // OnComplete()


/**
 * Fires the OnAborted event.
 */
void MovieExtracter::OnExtractionAborted(UInt32 iFrame, UInt32 iTotalFrames)
{
  MovieExtracter::ExtractionAborted(this, gcnew ExtractionEventArgs(iFrame, iTotalFrames, nullptr));
} // OnAborted()


/**
 * Fires the OnFailed event.
 */
void MovieExtracter::OnExtractionFailed(String ^ mpMessage)
{
  MovieExtracter::ExtractionFailed(this, gcnew ExtractionEventArgs(mpMessage));
} // OnFailed()


/**
 * Fires the OnUpdated event.
 */
void MovieExtracter::OnFrameExtracted(ExtractionEventArgs ^ mpArgs)
{
  MovieExtracter::FrameExtracted(this, mpArgs);
} // OnFrameExtracted()
