// $Id: VolumeHeader.h,v 1.3.2.3 2008/08/07 19:19:31 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] (mkontak@viatronix.com)


// Pragmes
#pragma once


// Namespaces
OPEN_UTILITIES_NS

/// Dimensions
public __value struct Dimensions
{
public:
  Dimensions() : X(0), Y(0), Z(0) { }

public:

  int X;
  int Y;
  int Z;

  System::String * ToString() { return System::String::Format("({0},{1},{2})",__box(X),__box(Y),__box(Z)); }

};


/// Units
public __value struct Units
{
public:
  Units() : X(0.0F), Y(0.0F), Z(0.0F) { }

public:

  float X;
  float Y;
  float Z;

  System::String * ToString() { return System::String::Format("({0},{1},{2})",__box(X),__box(Y),__box(Z)); }

};

/**
 * Implements a volume header class 
 */
__sealed public __gc class VolumeHeader
{
public:

  /// Default constructor
  VolumeHeader(System::String * sVolumeFile);

  /// Write the header to the logger
  void ToLogger();

  /// COnverts to and array of strings
  System::String * ToArray() [];


private:

  /// Initializes the object from a linear volume
  void InitializeFromLinearVolume(System::String * sVolumeFile);

  /// Initializes the object from ablock volume
  void InitializeFromBlockVolume(System::String * sVolumeFile);

public:     // Properties

  /// Gets the dimensions
  __property Dimensions get_Dimension() { return m_dim; }


    /// Gets the dimensions
  __property Units get_Units() { return m_units; }

  /// Gets the Prperties
  __property System::Collections::DictionaryEntry  get_Properties() [];

  /// Gets the Volumne File
  __property System::String * get_VolumeFile() { return m_sVolumeFile; }

  /// Gets the Type
  __property System::String * get_Type() { return m_sType; }
  
  /// Gets the data type
  __property System::String * get_DataType() { return m_sDataType; }

  /// Gets the study uid
  __property System::String * get_StudyUID() { return m_sStudyUID; }

  /// Gets th series UID
  __property System::String * get_SeriesUID() { return m_sSeriesUID; }

  /// Gets the opertaor name
  __property System::String * get_OperatorName() { return m_sOperatorName; }

  /// Gets the institution name
  __property System::String * get_InstitutionName() { return m_sInstitutionName; }

  /// Gets the patient name
  __property System::String * get_PatientName() { return m_sPatientName; }

  /// Gets the patient id
  __property System::String * get_PatientID() { return m_sPatientID; }

  /// Gets the patient history
  __property System::String * get_PatientHistory() { return m_sPatientHistory; }

  /// Gets the study date
  __property System::String * get_StudyDate() { return m_sStudyDate; }

  /// Gets the serie sdate
  __property System::String * get_SeriesDate() { return m_sSeriesDate; }

  /// Gets the patient DOB
  __property System::String * get_PatientDOB() { return m_sPatientDOB; }

  /// Gets the patient age
  __property System::String * get_PatientAge() { return m_sPatientAge; }

  /// Gets the patient age units
  __property System::String * get_PatientAgeUnits() { return m_sPatientAgeUnits; }

  /// Gets the gender
  __property System::String * get_PatientSex() { return m_sPatientSex; }

  /// Gets the referring physician
  __property System::String * get_ReferringPysician() { return m_sReferringPhysician; }

  /// Gets the dicom header
  __property System::String * get_DicomHeader() { return m_sDicomHeader; }

  /// Gets MA
  __property  System::String * get_MA() { return m_sMA; }

  /// Gets the slice thickness
  __property System::String * get_SliceThickness() { return m_sSliceThickness; }

  ///  Gets the pitch
  __property System::String * get_Pitch() { return m_sPitch; }

  /// Gets the FOV
  __property System::String * get_FOV() { return m_sFOV; }

  /// Gets the version
  __property System::String * get_Version() { return m_sVersion; }

  /// Gets the site id
  __property System::String * get_SiteID() { return m_sSiteID; }

  /// ets the manufacturer
  __property System::String * get_Manufacturer() { return m_sManufacturer; }

  /// Gets the tilt
  __property System::String * get_Tilt() { return m_sTilt; }

  /// Gets the KV
  __property System::String * get_KV() { return m_sKV; }

  /// Gets the patient position
  __property System::String * get_PatientPosition() { return m_sPatientPosition; }

  /// Gets the study type
  __property System::String * get_StudyType() { return m_sStudyType; }

  /// Gets the Z order reversed flag
  __property bool get_ZOrderReversed() { return m_bZOrderReversed; }

  /// Gets the encrypted patient name
  __property System::String * get_EncryptedPatientName() { return m_sEncryptedPatientName; }

  /// Gets the encrypted patient id
  __property System::String * get_EncryptedPatientID() { return m_sEncryptedPatientID; }

  //// Gets the total number of blocks
  __property unsigned int get_TotalNumberOfBlocks() { return m_iTotalNumberOfBlocks; }

  /// Gets the number of unique blocks
  __property unsigned int get_NumberOfUniqueBlocks() { return m_iNumberOfUniqueBlocks; }

  /// Gets the number of voxels per block
  __property unsigned int get_NumberOfVoxelsPerBlock() { return m_iNumberOfVoxelsPerBlock; }

  /// Gets the byte size of index array
  __property unsigned int get_ByteSizeOfIndexArray() { return m_iByteSizeOfIndexArray; }

  /// Gets the body part
  __property System::String * get_BodyPart() { return m_sBodyPart; }


private:

  /// Volume dimensions
  Dimensions m_dim;

  /// Volume units
  Viatronix::v3D::Utilities::Units m_units;

  /// volume file
  System::String * m_sVolumeFile;

  /// type
  System::String * m_sType;

  /// data type
  System::String * m_sDataType;

  /// study uid
  System::String * m_sStudyUID;

  /// series uid
  System::String * m_sSeriesUID;

  /// operator name
  System::String * m_sOperatorName;

  /// institution name
  System::String * m_sInstitutionName;

  /// patient id
  System::String * m_sPatientID;

  /// patient name
  System::String * m_sPatientName;

  /// patient history
  System::String * m_sPatientHistory;

  /// study date
  System::String * m_sStudyDate;

  /// series date
  System::String * m_sSeriesDate;

  /// patient dob
  System::String * m_sPatientDOB;

  /// patient age
  System::String * m_sPatientAge;

  /// patient sex
  System::String * m_sPatientSex;

  /// referring physician
  System::String * m_sReferringPhysician;

  /// dicom header
  System::String * m_sDicomHeader;

  /// ma
  System::String * m_sMA;

  /// slice thickness
  System::String * m_sSliceThickness;

  /// pitch
  System::String * m_sPitch;

  /// fov
  System::String * m_sFOV;

  /// version
  System::String * m_sVersion;

  /// site id
  System::String * m_sSiteID;

  /// manufacturer
  System::String * m_sManufacturer;

  /// tilt
  System::String * m_sTilt;
  
  /// kv
  System::String * m_sKV;

  /// patient postion
  System::String * m_sPatientPosition;

  /// study type
  System::String * m_sStudyType;

  /// anonymous patient dob
  System::String * m_sAnonymousPatientDOB;

  /// z order reveresed flag
  bool m_bZOrderReversed;

  /// encrypted patient name
  System::String * m_sEncryptedPatientName;

  /// encrypted patient id
  System::String * m_sEncryptedPatientID;

  /// total number of blocks
  unsigned int m_iTotalNumberOfBlocks;

  /// number of unqique blocks
  unsigned int m_iNumberOfUniqueBlocks;

  /// number of voxels per block
  unsigned int m_iNumberOfVoxelsPerBlock;

  /// byte size of index array
  unsigned int m_iByteSizeOfIndexArray;

  /// body part
  System::String * m_sBodyPart;

  /// patient age units
  System::String * m_sPatientAgeUnits;


}; // class Service


CLOSE_UTILITIES_NS


// $Log: VolumeHeader.h,v $
// Revision 1.3.2.3  2008/08/07 19:19:31  mkontak
// Added methods for AddStudy and AddSeriesReferences without a connection string
// parameter.
//
// Revision 1.3.2.2  2008/08/07 15:41:53  mkontak
// Added methods for AddStudy and AddSeriesReferences without a connection string
// parameter.
//
// Revision 1.3.2.1  2008/08/07 15:28:02  mkontak
// Added units and dimension
//
// Revision 1.3  2007/03/09 21:07:03  mkontak
// Coding standards
//
// Revision 1.2  2006/02/09 14:10:37  geconomos
// removed using namepace from header
//
// Revision 1.1  2005/11/16 15:32:30  mkontak
// Added VolumeHeader utility
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Utilities/VolumeHeader.h,v 1.3.2.3 2008/08/07 19:19:31 mkontak Exp $
// $Id: VolumeHeader.h,v 1.3.2.3 2008/08/07 19:19:31 mkontak Exp $
