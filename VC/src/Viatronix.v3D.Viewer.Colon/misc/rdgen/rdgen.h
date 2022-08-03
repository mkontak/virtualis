
#pragma once


#include "resource.h"

class Application : public CWinApp
{
public:

  Application();

	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern Application theApp;