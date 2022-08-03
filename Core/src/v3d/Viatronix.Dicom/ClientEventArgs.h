// Pragmas
#pragma once


#include "BaseEventArgs.h"

// Namespaces
using namespace System;

OPEN_DICOM_NS

ref class Image;

/**
 * Implements a basis set of thread event arguments.
 */
[Serializable]
public ref class ClientEventArgs : public BaseEventArgs
{

#pragma region constructor 

private:

  ClientEventArgs();

#pragma endregion

#pragma region methods

public:

  //// Creates the client args using the callback data
  static ClientEventArgs ^ Create(System::IntPtr dcmCallbackParametersPtr, System::IntPtr lpCallbackDataPtr );

#pragma endregion

#pragma region properties

public:

  /// Gets/Sets teh remaining value
  property int Remaining
  {
    int get() { return m_iRemaining; }
  } // Remaining

  /// Gets/Sets teh warning value
  property int Warning
  {
    int get() { return m_iWarning; }
  } // Warning

  //// Gets/Sets the failed value
  property int Failed
  {
    int get() { return m_iFailed; }
  } // Failed


  /// Gets the study uid
  property String ^ StudyUid
  {
    String ^ get() { return m_msStudyUid; }
  } // StudyUid

  /// Gets the series uid
  property String ^ SeriesUid
  {
    String ^ get() { return m_msSeriesUid; }
  } // SeriesUid

  /// Gets the instance uid
  property String ^ InstanceUid
  {
    String ^ get() { return m_msInstanceUid; }
  } // InstanceUid

    /// Gets the destination
  property String ^ Destination
  {
    String ^ get() { return m_msDestination; }
  } // Destination

  /// Gets the dataset
  property Viatronix::Dicom::Image ^ Image
  {
    Viatronix::Dicom::Image ^ get() { return m_mpImage; }
  } // Image

  /// Gets the query results
  property String ^ QueryResults
  {
    String ^ get() { return m_msQueryResults; }
  } // QueryResults

  property String ^ QueryLevel
  {
    String ^ get() { return m_msQueryLevel; }
  }

#pragma endregion


#pragma region fields

private:

  /// Query level
  String ^ m_msQueryLevel;

  /// Study Uid
  String ^ m_msStudyUid;

  /// Series Uid
  String ^ m_msSeriesUid;

  /// Instnace UID
  String ^ m_msInstanceUid;

  /// Destination
  String ^ m_msDestination;

  /// Remaining items
  int m_iRemaining;

  /// warning items
  int m_iWarning;

  ///  items
  int m_iFailed;

  /// Image
  Viatronix::Dicom::Image ^ m_mpImage;

  /// Query rsults
  String ^ m_msQueryResults;
 

#pragma endregion

};  // class EventArgs

/// Thread Event Handler Delegate
public delegate void ClientEventHandler(System::Object ^ mpSender, ClientEventArgs ^ e );




CLOSE_DICOM_NS
