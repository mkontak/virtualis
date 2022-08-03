// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmRange.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmRange.h"
#include "DcmTime.h"
#include "DcmDate.h"

template class DCM_DLL dcm::DcmRange<dcm::DcmTime>;
template class DCM_DLL dcm::DcmRange<dcm::DcmDate>;
template class DCM_DLL dcm::DcmRange<int>;
template class DCM_DLL dcm::DcmRange<unsigned int>;
template class DCM_DLL dcm::DcmRange<float>;
template class DCM_DLL dcm::DcmRange<double>;

