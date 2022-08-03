// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmList.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmList.h"
#include "DcmName.h"

using namespace dcm;

/**
 * Add template for the type of list you are using. Forces the compiler to generate the code.
 */
template class DCM_DLL dcm::DcmList<dcm::DcmName>;
template class DCM_DLL dcm::DcmList<std::string>;
template class DCM_DLL dcm::DcmList<int>;
template class DCM_DLL dcm::DcmList<unsigned int>;
template class DCM_DLL dcm::DcmList<float>;
template class DCM_DLL dcm::DcmList<double>;

