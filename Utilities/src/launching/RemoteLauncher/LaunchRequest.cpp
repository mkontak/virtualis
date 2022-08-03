// Copyright © 2014, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: LaunchRequest.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "stdafx.h"
#include "LaunchRequest.h"


/**
 * Constructor
 */
LaunchRequest::LaunchRequest() :
m_sResource("launch"),
m_sContext(""),
m_sPassword(""),
m_sServer(""),
m_sUids(""),
m_sDescription(""),
m_sNewSession(""),
m_sUserName(""),
m_sApplication(""),
m_iPort(0)
{

}

/**
 * Return true if the request is empty
 */
bool LaunchRequest::IsEmpty()
{
  return ( ! ContainsParameters() && m_sServer.IsEmpty()  && m_iPort == 0);

}

/**
 * Returns true is the request contains parameters
 */
bool LaunchRequest::ContainsParameters()
{
  return (!m_sContext.IsEmpty() || !m_sUids.IsEmpty() || !m_sUserName.IsEmpty() || !m_sApplication.IsEmpty() || !m_sPassword.IsEmpty() || !m_sDescription.IsEmpty() );

}

/**
 * Gets the Entire URL string
 */
CString LaunchRequest::GetUrl()
{
  CString url(GetUrlBase());

  if (!url.IsEmpty())
  {
    url.Append("launch");

    if (ContainsParameters())
    {
      url.Append("?");

      if (!m_sApplication.IsEmpty())
        url.AppendFormat("app=%s", m_sApplication);

      if (!m_sUids.IsEmpty())
        url.AppendFormat("&uids=%s", m_sUids);

      if (!m_sUserName.IsEmpty())
      {
        CString username = m_sUserName;
        username.Replace(" ", "%20");
        url.AppendFormat("&user=%s", username);
      }

      if (!m_sPassword.IsEmpty())
        url.AppendFormat("&pwd=%s", m_sPassword);

      if (!m_sContext.IsEmpty())
        url.AppendFormat("&cid=%s", m_sContext);

      if (!m_sDescription.IsEmpty())
      {
        CString desc = m_sDescription;
        desc.Replace(" ", "%20");

        url.AppendFormat("&desc=%s", desc);
      }

      if (!m_sNewSession.IsEmpty())
        url.AppendFormat("&new=%s", m_sNewSession);

    }
  }

  return url;
} // GetUrl()

/**
 * Sets the URL 
 */
void LaunchRequest::SetUrl(CString sUrl)
{
  
}

/**
 * Gets the URL base
 */
CString LaunchRequest::GetUrlBase()
{
  CString url("");

  if ( ! m_sServer.IsEmpty() )
  {

    url  = "http://";

    url.Append(m_sServer);

    if (m_iPort > 0)
      url.AppendFormat(":%d", m_iPort);

    url.Append("/ViewerService/");
  }

  return url;
} // GetUrlBase()

