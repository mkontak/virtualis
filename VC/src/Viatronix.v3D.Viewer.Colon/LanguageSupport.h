// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// File: LanguageSupport.h
//
// Owner: Greg Davidson 

#pragma once

//
//  CLanguageSupport : Language selection support and resource DLLs management.
//
//  Author : Serge Wautier.
//  Source : Code project article (http://www.codeproject.com/cpp/LanguageMenu.asp)
//
//  Looking for a localization tool ? Check out http://www.apptranslator.com
//

#include <afxcoll.h>

#define ID_LANGUAGE_FIRST 0x6F00
#define ID_LANGUAGE_LAST  0x6FFF

class CLanguageSupport
{
public:
	/// constructor
  CLanguageSupport();
	/// destructor
  ~CLanguageSupport();

	/// Called from Language menu item's update handler
  void CreateMenu(CCmdUI *pCmdUI, UINT nFirstItemId= ID_LANGUAGE_FIRST);
  /// Called from the language sub-menu items handler
  void OnSwitchLanguage(UINT nId, bool bLoadNewLanguageImmediately= false); 
  
	/// Called on start-up to load the required language dll
  void LoadBestLanguage(); 

	/// get instance
  static inline CLanguageSupport & GetInstance()
  {
    static CLanguageSupport language;
    return language;
  }

	/// get handle
  inline HINSTANCE GetResourceHandle() const { return m_hDll; }

protected:
	/// default language
  CWordArray m_aLanguages;
	/// other language option
  LANGID m_nExeLanguage;
	/// current language
  LANGID m_nCurrentLanguage;
  
	/// Satellite DLL.
  HINSTANCE m_hDll; 
  
	/// Registry value name for current language
  static const TCHAR szLanguageId[]; 
	/// get id
  static LANGID GetLangIdFromFile(LPCTSTR pszFilename);
	/// get language name
  static CString GetLanguageName(LANGID wLangId);

	/// get user UI
  static LANGID GetUserUILanguage();
	/// get system UI
  static LANGID GetSystemUILanguage();

	/// Fills m_aLanguages with the list of languages available (as resource DLLs + the language of the exe)
  void GetAvailableLanguages(); 

	/// Try to load the language m_nCurrentLanguage. If it doesn't exist, try to fallback on the neutral or default sublanguage for this language.
  bool LoadLanguage(); 

	/// Identifies and loads the resource DLL for language m_nCurrentLanguage
  bool LoadLanguageDll(); 

	/// Initializes m_nExeLanguage with the original language of the app (stored in the exe)
  void LookupExeLanguage(); 

	/// Unloads the current resource DLL (if any)
  void UnloadResourceDll(); 

	/// Tells MFC where the resources are stored.
  static void SetResourceHandle(HINSTANCE hDll); 
};
  