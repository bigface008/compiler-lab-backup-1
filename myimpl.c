#include "prog1.h"

int countPrintInExp(A_exp exp);
int countPrintInExpList(A_expList exps);

int maxargs(A_stm stm)
{
    int result = 0;
    switch (stm->kind)
    {
    case A_compoundStm:
        result = maxargs(stm->u.compound.stm1) + maxargs(stm->u.compound.stm2);
        break;
    case A_assignStm:
        result = countPrintInExp(stm->u.assign.exp);
    case A_printStm:
        result = 1 + countPrintInExpList(stm->u.print.exps);
        break;
    default:
        break;
    }
    return result;
}

void interp(A_stm stm)
{
}

int countPrintInExp(A_exp exp)
{
    int result = 0;
    switch (exp->kind)
    {
    case A_idExp:
    case A_numExp:
        result = 0;
        break;
    case A_opExp:
        result = countPrintInExp(exp->u.op.left) + countPrintInExp(exp->u.op.right);
        break;
    case A_eseqExp:
        result = maxargs(exp->u.eseq.stm) + countPrintInExp(exp->u.eseq.exp);
        break;
    default:
        break;
    }
    return result;
}

int countPrintInExpList(A_expList exps) {
    int result = 0;
    if (exps->kind == A_pairExpList) {
        result = countPrintInExp(exps->u.pair.head) + countPrintInExpList(exps->u.pair.tail);
    } else {
        result = countPrintInExp(exps->u.last);
    }
    return result;
}