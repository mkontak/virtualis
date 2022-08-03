// includes
#include "StdAfx.h"
#include "BaseUtilities.h"

#include "VarMap.h"
#include "vxVolumeHeader.h"
#include "vxBlockVolume.h"

// namespaces
USING_IMPBASE_NS

/**
 * Reads the dicom header from the volume file.  Needed for older studies.
 *
 * @param mpVolumeFile  The volume file to read.
 * @return  The dicom header from the volume file.
 */
System::String ^ BaseUtilities::ReadDicomHeaderFromVolumeFile(System::String ^ mpVolumeFile)
{
  vxBlockVolume<uint2> * pBlockVolume = NULL;

  try
  {
    std::string result;
    pBlockVolume = new vxBlockVolume<uint2>(ss(mpVolumeFile));
    if(pBlockVolume->GetHeader().GetVariableVolumeHeader().GetHdrVar("dicomHeader", result))
    {
      return gcnew System::String(result.c_str());
    }
    else
    {
      throw gcnew System::ApplicationException(System::String::Format("Provided volume file did not include the dicom header.  File: {0}", mpVolumeFile));
    }
  }
  finally
  {
    if(pBlockVolume != NULL)
    {
      delete pBlockVolume;
      pBlockVolume = NULL;
    }
  }
} // ReadDicomHeaderFromVolumeFile( mpVolumeFile )


/**
 * Reads the dicom header from the volume file.  Needed for older studies.
 *
 * @param mpVolumeFile  The volume file to read.
 * @return  The dicom header from the volume file.
 */
System::Tuple<System::Int32, System::Int32, System::Int32> ^ BaseUtilities::ReadVolumeDimensionsFromVolumeFile(System::String ^ mpVolumeFile)
{
  vxBlockVolume<uint2> * pBlockVolume = NULL;

  try
  {
    std::string result;
    pBlockVolume = new vxBlockVolume<uint2>(ss(mpVolumeFile));
    
    Triplei dim = pBlockVolume->GetHeader().GetVolDim();
    return System::Tuple::Create<System::Int32, System::Int32, System::Int32>(System::Convert::ToInt32(dim.X()), System::Convert::ToInt32(dim.Y()), System::Convert::ToInt32(dim.Z()));
  }
  finally
  {
    if(pBlockVolume != NULL)
    {
      delete pBlockVolume;
      pBlockVolume = NULL;
    }
  }
} // ReadDicomHeaderFromVolumeFile( mpVolumeFile )
