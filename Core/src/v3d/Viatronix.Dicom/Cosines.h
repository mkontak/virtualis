#pragma once

#include "scoped_ptr.h"

// foreward declarations
class dcm::DcmCosines;

// namespaces
OPEN_DICOM_NS

/**
 * Wrapper for low level cosines object.
 */
public ref class Cosines
{
// construction
public:

  /// Default constructor.
  Cosines();

  /// Destructor.
  ~Cosines();

  /// Finalizer.
  !Cosines();

// properties
public:

  /// Gets or sets the row x.
  property double RowX
  {
    double get();
    void set(double value);
  } // RowX


  /// Gets or sets the row y.
  property double RowY
  {
    double get();
    void set(double value);
  } // RowY


  /// Gets or sets the row z.
  property double RowZ
  {
    double get();
    void set(double value);
  } // RowZ


  /// Gets or sets the column x.
  property double ColumnX
  {
    double get();
    void set(double value);
  } // ColumnX


  /// Gets or sets the column y.
  property double ColumnY
  {
    double get();
    void set(double value);
  } // ColumnY


  /// Gets or sets the column z.
  property double ColumnZ
  {
    double get();
    void set(double value);
  } // ColumnZ

// methods
public:

  /// Loads the values into the internal structures.
  void Load(cli::array<double> ^ mpValues);

// fields
private:

  // The low level object.
  dcm::DcmCosines * m_pCosines;

}; // class Cosines

CLOSE_DICOM_NS
