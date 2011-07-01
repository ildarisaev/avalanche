#include "pub_tool_basics.h"
#include "pub_tool_libcbase.h"
#include "pub_tool_mallocfree.h"
#include "libvex_ir.h"

#include <avalanche.h>

extern void instrumentWrTmpCCall_Internal(UInt op, UInt ltmp, 
                                          UShort taintedness, 
                                          const Char* bitVectorModifier,
                                          IRExpr* arg1, IRExpr* arg2,
                                          IRExpr* value1, IRExpr* value2);

extern void instrumentWrTmpLongBinop_Internal(UInt oprt, UInt ltmp,
                                              UShort taintedness,
                                              IRExpr* arg1, IRExpr* arg2,
                                              ULong value1, ULong value2);

extern UShort isPropagation2(IRExpr* arg1, IRExpr* arg2);

#if defined(VGP_amd64_linux)

static
UInt translateNativeCondCode(UInt nativeCC)
{
  switch(nativeCC)
  {
    case X86CondB: 	return BVLT;
    case X86CondNB:	return BVGE;
    case X86CondZ:	return IFT;
    case X86CondNZ:	return IFNOT;
    case X86CondBE:	return BVLE;
    case X86CondNBE:	return BVGT;
    case X86CondL:	return SBVLT;
    case X86CondNL:	return SBVGE;
    case X86CondLE:	return SBVLE;
    case X86CondNLE:	return SBVGT;
    default:		return INVALID;
  }
}

static
void instrumentWrTmpCCall(IRStmt* clone, 
                          HWord size, IRExpr* value1, IRExpr* value2)
{
  IRExpr* arg1 = clone->Ist.WrTmp.data->Iex.CCall.args[2];
  IRExpr* arg2 = clone->Ist.WrTmp.data->Iex.CCall.args[3];
  UShort taintedness = isPropagation2(arg1, arg2);
  if (taintedness)
  {
    UInt nativeOp = 
           clone->Ist.WrTmp.data->Iex.CCall.args[0]->Iex.Const.con->Ico.U32;
    UInt op = translateNativeCondCode(nativeOp);
    UInt ltmp = clone->Ist.WrTmp.tmp;
    Char* bitVectorModifier = VG_(malloc) ("bitVectorModifier", 8);
    VG_(memset) (bitVectorModifier, '\0', 8);
    size %= 4;
    if (size == 0) {
      VG_(strcpy) (bitVectorModifier, "[63:0]");
    }
    else if (size == 1) {
      VG_(strcpy) (bitVectorModifier, "[7:0]");
    }
    else if (size == 2) {
      VG_(strcpy) (bitVectorModifier, "[15:0]");
    }
    else if (size == 3) {
      VG_(strcpy) (bitVectorModifier, "[31:0]");
    }
    instrumentWrTmpCCall_Internal(op, ltmp, taintedness, 
                                  bitVectorModifier,  
                                  arg1, arg2, value1, value2);
    VG_(free) (bitVectorModifier);
  }
}


void instrumentWrTmpCCall_External(IRSB* sbOut, IRStmt* clone, 
                                   IRExpr* value0, IRExpr* value1, 
                                   IRExpr* value2, IRExpr* value3)
{

  if (!VG_(strcmp)(clone->Ist.WrTmp.data->Iex.CCall.cee->name,
                  "amd64g_calculate_condition"))
  {
    IRDirty* di = 
         unsafeIRDirty_0_N(0, "instrumentWrTmpCCall", 
                           VG_(fnptr_to_fnentry)(&instrumentWrTmpCCall), 
                           mkIRExprVec_4(mkIRExpr_HWord((HWord) clone),
                                         value1, value2, value3));
    addStmtToIRSB(sbOut, IRStmt_Dirty(di));
  }
}

static
void instrumentWrTmpLongBinop(IRStmt* clone, ULong value1, ULong value2)

{
  IRExpr* arg1 = clone->Ist.WrTmp.data->Iex.Binop.arg1;
  IRExpr* arg2 = clone->Ist.WrTmp.data->Iex.Binop.arg2;
  UShort taintedness = isPropagation2(arg1, arg2);
  if (taintedness)
  {
    UInt ltmp = clone->Ist.WrTmp.tmp;
    UInt oprt = clone->Ist.WrTmp.data->Iex.Binop.op;
    instrumentWrTmpLongBinop_Internal(oprt, ltmp, taintedness, 
                             arg1, arg2, value1, value2);
  }
}

void instrumentWrTmpLongBinop_External(IRSB* sbOut, IRStmt* clone, 
                                       IRExpr* value1, IRExpr* value2)
{
  IRDirty* di = 
       unsafeIRDirty_0_N(0, "instrumentWrTmpLongBinop",
                         VG_(fnptr_to_fnentry)(&instrumentWrTmpLongBinop),
                         mkIRExprVec_3(mkIRExpr_HWord((HWord) clone), 
                                       value1, value2));
  addStmtToIRSB(sbOut, IRStmt_Dirty(di));
}
#endif
