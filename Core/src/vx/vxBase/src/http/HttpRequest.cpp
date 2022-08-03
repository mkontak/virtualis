#include "stdafx.h"
#include "http\HttpRequest.h"
#include "Exception.h"
#include "vxBuffer.h"
#include "SystemInfo.h"

using namespace http;



/**
 * Constructor
 */
HttpRequest::HttpRequest(HttpVerb verb, const std::string & resource, const std::string & operation, 
                                        const std::string & contextId, const std::shared_ptr<Buffer> & message) :
m_resource(resource),
m_operation(operation),
m_contextId(contextId),
m_source(""),
m_system(""),
m_encoding(Encoding::ENCODING_ASCII),
m_verb(verb),
m_application(""),
m_version("1.0"),
m_message(message)
{

  m_application = SystemInfo::GetModuleName();

  TCHAR buffer[MAX_PATH]; 
  
  if ( ::GetModuleFileName(NULL, buffer, MAX_PATH) )
    m_source = util::IO::GetFilenameWithoutExt(buffer);
  
  RtlZeroMemory(buffer, MAX_PATH);
  DWORD size(0);
  ::GetComputerName(buffer,&size);
  m_system = std::string(buffer, size);

  if ( m_message != __nullptr && m_message->GetEncoding() == BufferEncoding::ENCODING_BASE64 )
    m_encoding = Encoding::ENCODING_BASE64;

}


std::shared_ptr<HttpRequest> HttpRequest::Create( HttpVerb verb, const std::string & resource, const std::string & operation, 
                                                                const std::string & contextId, const std::shared_ptr<Buffer> & message)
{
  std::shared_ptr<HttpRequest> requestData = std::shared_ptr<HttpRequest>(new HttpRequest(verb, resource, operation, contextId, message));

  if ( requestData == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocated HttpRequest","HttpRequest","Create"));

  /// Set the default version for the requests message
  requestData->AddAttribute("ver","3.0");
  
  return requestData;
}


/**
 * COnverts the request to a string using the <rq>MESSAGE</rq> format
 */
std::string HttpRequest::ToString()
{
  return FormatRequest(__nullptr)->ToString(); 
} // ToString()

/**
 * Try and get the attribute specified
 *
 * @param sName     Attribute name
 * @param sVlaue    Attribute value 
 *
 * @return true if found, false otherwise
 */
bool HttpRequest::TryGetAttribute(const std::string & sName, std::string & sValue)
{
  bool found(false);

  std::map<std::string, std::string>::iterator itFind = m_attributes.find(sName);

  if ( itFind != m_attributes.end() )
  {
    found = true;
    sValue = (*itFind).second;
  }

  return found;
} // TryGetAttribute(const std::string & sName, std::string & sValue)


/**
 * Adds the attribute to the resource list. If it exists then the value is changed
 *
 * @param sName     Name of attribute
 * @param sValue    Value of the attribute
 */
void HttpRequest::AddAttribute(const std::string & sName, const std::string & sValue)
{ 
  std::string sTrimmed(trim_right(sValue, "\r\n"));

  if ( m_attributes.find(sName) == m_attributes.end() )
    m_attributes.insert(std::pair<std::string, std::string>(sName, sTrimmed));
  else
    m_attributes[sName] = sTrimmed;
} // AddAttribute(const std::string & sName, const std::string & sValue)

/**
 * Adds the ranged attribute to the resource list. If it exists then the value is changed
 *
 * @param sName     Name of attribute
 * @param sFrom     From Value of the attribute
 * @param sTo       To Value of the attribute
 */
void HttpRequest::AddRangeAttribute(const std::string & sName, const std::string & sFrom, const std::string & sTo)
{ 
  if ( m_attributes.find(sName) == m_attributes.end() )
    m_attributes.insert(std::pair<std::string,std::string>(sName, util::Strings::Format("RANGE:%s,%s", sFrom.c_str(), sTo.c_str()))); 
  else
    m_attributes[sName] = util::Strings::Format("RANGE:%s,%s", sFrom.c_str(), sTo.c_str());
} // AddAttribute(const std::string & sName, const std::string & sValue)


/**
 * Gets the ruquest portion of the request message (xml)
 *
 * @return Returns the header portion of the request
 */
std::shared_ptr<Buffer> HttpRequest::FormatRequest(const std::shared_ptr<Buffer> & message)
{
  std::shared_ptr<Buffer> request( Buffer::Create( ( message != __nullptr ? message->GetLength() : 0 ) + 512 ) );
 
  int iRangeCount(0);

  if ( m_verb == HTTP_POST || m_verb == HTTP_PUT )
  {

    sprintf((char *)request->GetData(), "<rq cid=\"%s\" src=\"%s\" app=\"%s\" sys=\"%s\" op=\"%s\" enc=\"%d\" ver=\"%s\" ><%s ", m_contextId.c_str(),      
            m_source.c_str(), m_application.c_str(),  m_system.c_str(), m_operation.c_str(), (int)m_encoding , m_version.c_str(), m_resource.c_str() );
    request->SetLength(strlen((const char *)request->GetData()));

    std::vector<std::pair<std::string, std::string>> xmlAttributes;

    // Do all non-ranged attributes first
    std::for_each(m_attributes.begin(), m_attributes.end(), [this, &iRangeCount, &request, &xmlAttributes](std::pair<std::string, std::string> item)
    {
      const char * ptr(item.second.c_str());

      if (strncmp(ptr, "RANGE:", 6) == 0)
        iRangeCount++;
      else if ( ptr[0] == '<' && ptr[item.second.size() - 1] == '>' )
        xmlAttributes.push_back(item);
      else
       request->Append(util::Strings::Format("%s=\"%s\" ", item.first.c_str(), item.second.c_str()));
    });

    // ===========================================
    // If there are range attribute add them now
    // ===========================================
    if ( iRangeCount > 0 || xmlAttributes.size() > 0 )
    {
      request->Append('>');


      // Range attributes
      std::for_each(m_attributes.begin(), m_attributes.end(), [this, &request] ( std::pair<std::string, std::string> item) 
      {
        if ( strncmp(item.second.c_str(), "RANGE:", 6) == 0 )
        {
          std::vector<std::string> values( util::Strings::Split(item.second.substr(6),',') );

          request->Append(util::Strings::Format("<%s from=\"%s\" to=\"%s\" />", item.first.c_str(), values[0].c_str(), values[1].c_str()));
        }
      });
    

      // Xml Attrbutes
      std::for_each(xmlAttributes.begin(), xmlAttributes.end(), [this, &request](std::pair<std::string, std::string> item)
      {
        request->Append(util::Strings::Format("<%s>%s</%s>", item.first.c_str(), item.second.c_str(), item.first.c_str()));
      });


      request->Append(util::Strings::Format("</%s>", m_resource.c_str()));

    }
    else
      request->Append("/>");


    request->Append(message);

    request->Append("</rq>");
    
  } // END ... if ( m_verb == HTTP_POST || m_verb == HTTP_PUT )
  



  return request;

}

/**
 * Gets the send data for the specified verb specified
 *
 * @return Returns the Buffer to be sent
 */
std::shared_ptr<Buffer>  HttpRequest::GetRequest()
{
  return  FormatRequest(m_message);
}

/**
 * Gets the send data for the specified verb specified
 */
std::string HttpRequest::GetPath()
{

  std::string sPath("");

  if ( m_verb != HTTP_POST && m_verb != HTTP_PUT )
  {
    sPath = m_resource + "?" + util::Strings::Format("rqcid=%s&rqapp=%s&rqsrc=%s", m_contextId.c_str(), m_application.c_str(), m_source.c_str());

    std::for_each(m_attributes.begin(), m_attributes.end(), [&sPath] ( std::pair<std::string, std::string> item) 
    {
      sPath.append("&" + item.first + "=" + item.second); 
    });


    // Pops the last character
    //if ( m_attributes.size() > 0 )
    //  sPath.pop_back();;
 
  }

  return sPath;

}

/**
 * Converts the HttpVerb to a string
 *
 * @param verb      Verb to be converted
 */
std::string HttpRequest::GetVerbAsString()
{

  std::string sVerb("");

  switch(m_verb)
  {
  case HTTP_GET:
    sVerb = "GET";
    break;
  case HTTP_PUT:
    sVerb = "PUT";
    break;
  case HTTP_POST:
    sVerb = "POST";
    break;
  case HTTP_DELETE:
    sVerb  = "DELETE";
    break;
  } // Switch on verb

  return sVerb;
} // std::string HttpRequest::GetVerbAsString()
