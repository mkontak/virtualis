
// includes
#include "StdAfx.h"
#include <vector>

#include "Cosines.h"
#include "DcmCosines.h"

// namespaces
USING_DICOM_NS

/**
 * Constructor.
 */
Cosines::Cosines() : m_pCosines(new dcm::DcmCosines())
{
} // Cosines()


/**
 * Destructor.
 */
Cosines::~Cosines()
{
  this->!Cosines();
} // ~Cosines()


/**
 * Finalizer
 */
Cosines::!Cosines()
{
  if(m_pCosines != NULL)
  {
    delete m_pCosines;
    m_pCosines = NULL;
  }
} // !Cosines()


/**
 * Gets row x.
 */
double Cosines::RowX::get()
{
  return System::Convert::ToDouble(m_pCosines->GetRowX());
} // RowX::get()


/**
 * Sets row x.
 */
void Cosines::RowX::set(double value)
{
  return m_pCosines->SetRowX(value);
} // RowX::set( value )


/**
 * Gets row y.
 */
double Cosines::RowY::get()
{
  return System::Convert::ToDouble(m_pCosines->GetRowY());
} // RowY::get()


/**
 * Sets row y.
 */
void Cosines::RowY::set(double value)
{
  return m_pCosines->SetRowY(value);
} // RowY::set( value )


/**
 * Gets row z.
 */
double Cosines::RowZ::get()
{
  return System::Convert::ToDouble(m_pCosines->GetRowZ());
} // RowZ::get()


/**
 * Sets row z.
 */
void Cosines::RowZ::set(double value)
{
  return m_pCosines->SetRowZ(value);
} // RowZ::set( value )


/**
 * Gets column x.
 */
double Cosines::ColumnX::get()
{
  return System::Convert::ToDouble(m_pCosines->GetColumnX());
} // ColumnX::get()


/**
 * Sets column x.
 */
void Cosines::ColumnX::set(double value)
{
  m_pCosines->SetColumnX(value);
} // ColumnX::set( value )


/**
 * Gets column y.
 */
double Cosines::ColumnY::get()
{
  return System::Convert::ToDouble(m_pCosines->GetColumnY());
} // ColumnY::get()


/**
 * Sets column y.
 */
void Cosines::ColumnY::set(double value)
{
  m_pCosines->SetColumnY(value);
} // ColumnY::set( value )


/**
 * Gets column z.
 */
double Cosines::ColumnZ::get()
{
  return System::Convert::ToDouble(m_pCosines->GetColumnZ());
} // ColumnZ::get()


/**
 * Sets column z.
 */
void Cosines::ColumnZ::set(double value)
{
  m_pCosines->SetColumnZ(value);
} // ColumnZ::set( value )


/**
 * Loads the array of values into the internal object.
 *
 * @param mpValues  The array of values to load.
 * @exception       System.ArgumentException if mpValues has length less than or more than 6.
 */
void Cosines::Load(cli::array<double> ^ mpValues)
{
  if(mpValues->Length == 6)
  {
    RowX    = mpValues[0];
    RowY    = mpValues[1];
    RowZ    = mpValues[2];
    ColumnX = mpValues[3];
    ColumnY = mpValues[4];
    ColumnZ = mpValues[5];
  }
  else
  {
    throw gcnew System::ArgumentException(System::String::Format("The list of cosines does not have the correct length.  [EXPECTED = 6; RECEIVED = {0}]", mpValues->Length));
  }
} // Load( mpValues )