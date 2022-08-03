// $Id: Movies.h,v 1.2 2007/03/09 21:07:03 mkontak Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Make Kontak] (mailto:mkontak@viatronix.com)

///pragmas
#pragma once

// namespaces
using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;
using namespace System::Collections;

OPEN_UTILITIES_NS


/**
  * Implements the execute thread event arguements.
  */
public ref class ExtractionEventArgs : public EventArgs
{
public:

  /// Constructor
  ExtractionEventArgs(unsigned int iFrame, unsigned int iTotalFrames, Bitmap ^ mpFrame) : 
  EventArgs(), m_iFrameNumber(iFrame), m_iTotalFrames(iTotalFrames), m_mpFrame(mpFrame), m_mpMessage(String::Empty), m_bCancel(false) { }

  /// Constructor.
  ExtractionEventArgs(System::String ^ mpMessage) : 
  EventArgs(), m_iFrameNumber(0), m_iTotalFrames(0), m_mpFrame(nullptr), m_mpMessage(mpMessage), m_bCancel(false) { }

  /// Constructor.
  ExtractionEventArgs() : 
  EventArgs(), m_iFrameNumber(0), m_iTotalFrames(0), m_mpFrame(nullptr),  m_mpMessage(String::Empty), m_bCancel(false) { }

public:     // Public Properties

  // Abort 
  property bool Cancel
  {
    bool get() { return m_bCancel; }
    void set(bool bValue) { m_bCancel = bValue; }
  } // Cancel


  /// Returns a basic message
  property unsigned int FrameNumber
  {
    unsigned int get() { return m_iFrameNumber; }
  } // FrameNumber

  /// Returns a basic message
  property unsigned int TotalFrames
  {
    unsigned int get() { return m_iTotalFrames; }
  } // TotalFrames


  /// Returns the message (Error Usually)
  property String ^ Message
  {
    String ^ get() { return m_mpMessage; }
    void set(String ^ mpValue) { m_mpMessage = mpValue; }
  } // Message


  /// Gets the frame
  property Bitmap ^ Frame
  {
    Bitmap ^ get() { return m_mpFrame; }
  } // Frame

private:    // Private Members

  /// Error message
  String ^ m_mpMessage;

  unsigned int m_iFrameNumber;

  unsigned int m_iTotalFrames;

  Bitmap ^ m_mpFrame;

  bool m_bCancel;

};  // class ExtractionEventArgs



/// Execute Event Handler Delegate
public delegate void ExtractionEventHandler(System::Object ^ sender, ExtractionEventArgs ^ e);



/**
 * Implements a movies object to extract the frames
 */
public ref class MovieExtracter
{

public:

  /// Construxtor
  MovieExtracter() { }

public:

  /// Returns all the movie files in the path
  static cli::array<System::String ^> ^ GetFiles(System::String ^ mpPath);

  /// Returns true if the file is supported
  static bool Supports(System::String ^ mpMovieFile);

  /// Gets all images from the movie file
  void ExtractFrames(System::String ^ mpMovieFile);

  
protected:

  /// Fires the OnStart event
  virtual void OnExtractionStarted(unsigned int iTotalFrames);

  /// Fires the OnComplete event
  virtual void OnExtractionCompleted(unsigned int iTotalFrames);

  /// Fires the OnFailed event
  virtual void OnExtractionFailed(System::String ^ mpMessage);

  /// Fires the OnUpdated event
  virtual void OnFrameExtracted(unsigned int iFrame, unsigned int iTotalFrames, Bitmap ^ mpBitmap)
  { OnFrameExtracted(gcnew ExtractionEventArgs(iFrame, iTotalFrames, mpBitmap)); }

  /// Fires the OnUpdated event
  virtual void OnFrameExtracted(ExtractionEventArgs ^ mpArgs);

  /// Fires the OnUpdated event
  virtual void OnExtractionAborted(unsigned int IFrame, unsigned int iTotalFrames);

public:

  event ExtractionEventHandler ^ ExtractionStarted;

  event ExtractionEventHandler ^ ExtractionCompleted;

  event ExtractionEventHandler ^ FrameExtracted;

  event ExtractionEventHandler ^ ExtractionAborted;

  event ExtractionEventHandler ^ ExtractionFailed;

private:

  /// Gets all images from a WMV file
  void WmvExtractFrames(System::String  ^ mpMovieFile);

  /// Gets all limages from an AVI file
  void AviExtractFrames(System::String  ^ mpMovieFile);

};  // class Movies


CLOSE_UTILITIES_NS


// $Log: Movies.h,v $
// Revision 1.2  2007/03/09 21:07:03  mkontak
// Coding standards
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Movies.h,v 1.2 2007/03/09 21:07:03 mkontak Exp $
// $Id: Movies.h,v 1.2 2007/03/09 21:07:03 mkontak Exp $
