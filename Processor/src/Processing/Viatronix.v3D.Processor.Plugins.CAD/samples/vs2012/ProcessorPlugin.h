#pragma once


/**
 * ProcessorPlugin 
 *
 * Description: Implements the required IProcessorPlugin interface. All work is done in the Run() method. 
 *              Make sure to "ToDo" items in this header.
 *              
 */
[
	coclass,
	threading( "apartment" ),
  vi_progid( "ICAD.VERALOOK" ), // TODO: replace with your company name 
  progid( "COM.ICAD.CAD.COLON.VIATRONIX" ), // TODO: replace with your company name 
	uuid( "AC54474D-6ED9-4EB8-8A96-70013BFB14EB" ), // TODO: Replace with your own guid
  aggregatable("never"),
  default("IProcessorPlugin") 
]
class ATL_NO_VTABLE ProcessorPlugin : public IDispatchImpl< vxComCAD::IProcessorPlugin, &__uuidof( vxComCAD::IProcessorPlugin ) >
{
/// ATL macros and defines
public:

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  
// IProcessorPlugin implementation
public:

  /// run 
  STDMETHOD( raw_Run )( vxComCAD::IProcessor * piProcessor )
  {
    vxComCAD::IProcessorPtr processor( piProcessor );
    return Run( processor );
  }

// member functions
private:

  // internal run
  HRESULT Run(  vxComCAD::IProcessorPtr & processor );

}; // class ProcessorPlugin

