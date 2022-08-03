#include "Stdafx.h"
#include "Tag.h"


USING_DICOM_NS


/**
 * Compares the two objects
 * 
 * @param mpObject  Object to compare against
 *
 * @return 1
 */
int Tag::CompareTo(Object ^ mpObject)
{
  if ( !  mpObject->GetType()->Equals(Tag::typeid) )
    throw gcnew InvalidDataException("Invalid type");

  return CompareTo(dynamic_cast<Tag ^>(mpObject));

} // CompareTo(Object ^ mpObject)

/**
 * Compares the two objects
 * 
 * @param mpTag  Tag to compare against
 *
 * @return 1
 */
int Tag::CompareTo(Tag ^ mpTag)
{

  return ( this->Id < mpTag->Id ? 1 : ( this->Id > mpTag->Id ? -1 : 0 ) ); 

} // CompareTo(Object ^ mpObject)

/**
 * Determines equality
 * 
 * @param mpObject  Object to compare against
 *
 * @return Returns true if equal, false if not
 */
bool Tag::Equals(Object ^ mpObject)
{
  if ( !  mpObject->GetType()->Equals(Tag::typeid) )
    throw gcnew InvalidDataException("Invalid type");
  return Equals(dynamic_cast<Tag ^>(mpObject));

} // Equal(Object ^ mpObject)

/**
 * Determines if the tags are equal
 *
 * @param mpTag     Tag to compare to
 *
 * @return Returns true if equal, false otherwise
 */
bool Tag::Equals(Tag ^ mpTag)
{
  return ( mpTag->Id == this->Id );
} // Equals(Tag ^ mpTag)


/** 
 * Creates the tag object using the string representation of the tag
 *
 * @param msTag       string representation of the tag ... 0008,0008
 *
 * @return Tag
 */
Tag ^ Tag::Create(String ^ msTag)
{

  Tag ^ mpTag(gcnew Tag());

  array<Char> ^ sperator = { ',' };

  array<String ^> ^ mpFields = msTag->Split(sperator);

  if ( mpFields->Length != 2 )
    throw gcnew DicomException("Invalid tag [" + msTag + "]");

  if ( mpFields[0]->Length != 4 && mpFields[1]->Length != 4 )
    throw gcnew DicomException("Invalid tag [" + msTag + "]");

  String ^ msGroup(mpFields[0]);
  String ^ msField(mpFields[1]);
  int iShift(12);

  for ( int i(0); i < 4; ++i, iShift -= 4 )
  {
    mpTag->m_iGroup     += (ToUInt16(msGroup[i]) << iShift);
    mpTag->m_iElement   += (ToUInt16(msField[i]) << iShift);
  }

  return mpTag;

} // Create(String ^ msTag)

/** 
 * Converts the character to a unsigned short
 */
unsigned short Tag::ToUInt16(Char ch)
{

  unsigned short iCh = (unsigned short)ch;

  if ( iCh >= 48 && iCh <= 57 )
    return iCh - 48;
  else if ( iCh >= 65 && iCh <= 70 )
    return iCh - 55;
  else if ( iCh >= 97 && iCh <= 102 )
    return iCh - 87;
  else
    return iCh;

} // ToUInt16(Char ch)