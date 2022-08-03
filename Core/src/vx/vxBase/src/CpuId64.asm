; $Id: CpuId64.asm,v 1.1 2007/09/21 15:43:22 geconomos Exp $
;
; Copyright © 2006, Viatronix Inc., All Rights Reserved.
;
; This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
; may not be disclosed to third parties, copied or duplicated in any form,
; in whole or in part, without the prior written permission Viatronix Inc.
;
; Author: George Economos (george@viatronix.com)

PUBLIC cpuid64
.CODE
  ALIGN     8
cpuid64	PROC FRAME
  sub			rsp, 32
  .allocstack 32
  push		rbx
  .pushreg	rbx
  .endprolog
        
  mov	r8, rcx
  mov eax, DWORD PTR [r8+0]
  mov ecx, DWORD PTR [r8+8]
  cpuid
  mov DWORD PTR [r8+0], eax
  mov DWORD PTR [r8+4], ebx
  mov DWORD PTR [r8+8], ecx
  mov DWORD PTR [r8+12], edx
  pop      rbx         
  add      rsp, 32     
  ret                  
  ALIGN     8
cpuid64 ENDP

_TEXT ENDS
END

