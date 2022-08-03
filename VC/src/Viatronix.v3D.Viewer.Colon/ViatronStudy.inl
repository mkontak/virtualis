// ViatronStudy.inl: implementation of the ViatronStudy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIATRONSTUDYINL_H__D3531DFE_7E82_11D4_B6E2_00D0B7BF4092__INCLUDED_)
#define AFX_VIATRONSTUDYINL_H__D3531DFE_7E82_11D4_B6E2_00D0B7BF4092__INCLUDED_

inline bool BitCheck(const ViewModesValue &a, const ViewModesValue &b)
{ 
  return (a&b)==b; 
} // BitCheck()

// all inline functions of class VtxViewModes have moved to VtxViewModes.inl. Bin Li, 04/10/2001.

#endif // !defined(AFX_VIATRONSTUDYINL_H__D3531DFE_7E82_11D4_B6E2_00D0B7BF4092__INCLUDED_)
