// includes
#include "StdAfx.h"
#include "FileVersionReader.h"
#include "Windows.h"

// namespaces
using namespace System;
using namespace System::IO;
using namespace System::Xml;
using namespace System::Configuration;
using namespace System::Collections::Generic;

USING_MANIFEST_NS
using namespace Viatronix::Utilities::GenerateManifest::Base;

/**
 * Constructor.
 */
FileVersionReader::FileVersionReader() : m_mpFileVersions(gcnew List<Tuple<String ^, String ^, String ^> ^>())
{
} // FileVersionReader()


/**
 * Reads the directory and its subdirectories and extracts the version for each file.
 *
 * @param  mpInputDirectory  The directory to read.
 */
void FileVersionReader::ReadDirectory(String ^ mpInputDirectory)
{
  bool bOutputUnversioned = false;

  try
  {
    bOutputUnversioned = Convert::ToBoolean(ConfigurationManager::AppSettings["OutputUnversionedFiles"]);
  }
  catch(Exception ^ mpException)
  {
    // Couldn't get the configured value, just keep going.
  }

  DirectoryInfo ^ mpDirectoryInfo = gcnew DirectoryInfo(mpInputDirectory);
  cli::array<FileInfo ^> ^ mpFiles = mpDirectoryInfo->GetFiles("*.*", SearchOption::AllDirectories);

  for each(FileInfo ^ mpFileInfo in mpFiles)
  {
    Tuple<System::String ^, System::String ^> ^ mpVersions = VersionInfo::RetrieveVersionInformation(mpFileInfo->FullName);
    if(bOutputUnversioned || (mpVersions->Item1 != String::Empty || mpVersions->Item2 != String::Empty))
    {
      m_mpFileVersions->Add(gcnew Tuple<String ^, String ^, String ^>(mpFileInfo->FullName, mpVersions->Item1, mpVersions->Item2));
    }
  }
} // ReadDirectory( mpInputDirectory )


/**
 * Writes out the manifest.
 *
 * @param mpInputDirectory The input directory.
 * @param mpOutputFile     The file to write the manifest details to.
 */
void FileVersionReader::WriteManifest(String ^ mpInputDirectory, String ^ mpOutputFile)
{
  Dictionary<String ^, XmlNode ^> ^ mpDirectoryNameNodeMap = gcnew Dictionary<String ^, XmlNode ^>();
  
  XmlDocument ^ mpDoc = gcnew XmlDocument();

  String ^ mpRootDirectory = Path::GetFileName(mpInputDirectory);
  System::Xml::XmlNode ^ mpDocumentElement = mpDoc->CreateElement(mpRootDirectory);
  mpDoc->AppendChild(mpDocumentElement);

  // need to add to the dictionary if the input directory has files.
  mpDirectoryNameNodeMap->Add(mpInputDirectory, mpDocumentElement);

  for each(Tuple<String ^, String ^, String ^> ^ mpFileVersion in m_mpFileVersions)
  {
    // ----------------------------------------------------------------------------------------------
    // Step 1: Build up the directory structure.
    // ----------------------------------------------------------------------------------------------

    // Retrieve all intermediate directories between the immediate directory and the base directory.
    String ^ mpDirectory = Path::GetFileName(Path::GetDirectoryName(mpFileVersion->Item1));

    List<String ^> ^ mpDirectories = gcnew List<String ^>();
    String ^ mpWorkingDirectory = mpFileVersion->Item1;
    while((mpWorkingDirectory = Path::GetDirectoryName(mpWorkingDirectory)) != mpInputDirectory)
    {
      mpDirectories->Add(mpWorkingDirectory);
    }
    
    // Append each child to the parent node.
    mpDirectories->Reverse();
    XmlNode ^ mpParent = mpDocumentElement;
    for(int i = 0; i < mpDirectories->Count; ++i)
    {
      XmlNode ^ mpDirectoryNode = nullptr;
      String ^ mpDirectoryName = mpDirectories[i];
      if(!mpDirectoryNameNodeMap->TryGetValue(mpDirectoryName, mpDirectoryNode))
      {
        mpDirectoryNode = mpDoc->CreateElement(Path::GetFileName(mpDirectoryName->Replace(' ', '_')));
        mpParent->AppendChild(mpDirectoryNode);

        mpDirectoryNameNodeMap->Add(mpDirectoryName, mpDirectoryNode);
      }

      mpParent = mpDirectoryNode;
    }
  }

  for each(Tuple<String ^, String ^, String ^> ^ mpFileVersion in m_mpFileVersions)
  {
    // ----------------------------------------------------------------------------------------------
    // Step 2: Append the files to the directory node.
    // ----------------------------------------------------------------------------------------------

    XmlNode ^ mpDirectoryNode = nullptr;
    mpDirectoryNameNodeMap->TryGetValue(Path::GetDirectoryName(mpFileVersion->Item1), mpDirectoryNode);
    XmlNode ^ mpFilesNode = mpDirectoryNode->SelectSingleNode("files");
    if(mpFilesNode == nullptr)
    {
      mpFilesNode = mpDoc->CreateElement("files");
      mpDirectoryNode->AppendChild(mpFilesNode);
    }

    XmlNode ^ mpFileNode = mpDoc->CreateElement("file");

    XmlAttribute ^ mpFilenameAttribute = mpDoc->CreateAttribute("name");
    mpFilenameAttribute->Value = Path::GetFileName(mpFileVersion->Item1);
    mpFileNode->Attributes->Append(mpFilenameAttribute);

    XmlAttribute ^ mpFileVersionAttribute = mpDoc->CreateAttribute("fileversion");
    mpFileVersionAttribute->Value = mpFileVersion->Item2;
    mpFileNode->Attributes->Append(mpFileVersionAttribute);

    XmlAttribute ^ mpProductVersionAttribute = mpDoc->CreateAttribute("productversion");
    mpProductVersionAttribute->Value = mpFileVersion->Item3;
    mpFileNode->Attributes->Append(mpProductVersionAttribute);

    XmlAttribute ^ mpDateModifiedAttribute = mpDoc->CreateAttribute("datemodified");
    mpDateModifiedAttribute->Value = File::GetLastWriteTime(mpFileVersion->Item1).ToString("o");
    mpFileNode->Attributes->Append(mpDateModifiedAttribute);

    mpFilesNode->AppendChild(mpFileNode);
  }

  mpDoc->Save(mpOutputFile);
} // WriteManifest( mpInputDirectory, mpOutputFile )
