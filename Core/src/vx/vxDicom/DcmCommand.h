#pragma once

namespace dcm
{

// ==========================
// Implements a DcmCommand
// ==========================
class DCM_DLL DcmCommand
{

protected:

  /// Constructor
  DcmCommand(DcmCommandType commandType) :
  m_commandType(commandType)
  { }

  virtual ~DcmCommand() { }

public:

  /// Gets the command type
  const DcmCommand GetCommandType() const
  { return m_commandType; }

private:

  /// Command type
  DcmCommandType m_commandType;

};  // class DcmCommand


}; // namespace dcm