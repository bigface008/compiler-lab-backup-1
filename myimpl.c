#include "prog1.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

typedef struct table *Table_;
struct table
{
    string id;
    int value;
    Table_ tail;
};
struct IntAndTable
{
    int i;
    Table_ t;
};
Table_ Table(string id, int value, struct table *tail)
{
    Table_ t = malloc(sizeof(*t));
    t->id = id;
    t->value = value;
    t->tail = tail;
    return t;
}

int maxargsInExp(A_exp exp);
int maxargsInExpList(A_expList exps);
int countPrintArgsInExpList(A_expList exps);

Table_ interpStm(A_stm s, Table_ t);
struct IntAndTable interpExp(A_exp e, Table_ t);
Table_ interpExpList(A_expList exps, Table_ t);
int lookup(Table_ t, string key);
Table_ update(Table_ t, string key, int value);

int maxargs(A_stm stm)
{
    int result = 0, num1 = 0, num2 = 0;
    switch (stm->kind)
    {
    case A_compoundStm:
        num1 = maxargs(stm->u.compound.stm1);
        num2 = maxargs(stm->u.compound.stm2);
        result = num1 > num2 ? num1 : num2;
        break;
    case A_assignStm:
        result = maxargsInExp(stm->u.assign.exp);
        break;
    case A_printStm:
        result = maxargsInExpList(stm->u.print.exps);
        break;
    default:
        break;
    }
    return result;
}

void interp(A_stm stm)
{
    // return;
    Table_ t = Table("", 0, NULL);
    interpStm(stm, t);
}

int maxargsInExp(A_exp exp)
{
    int result = 0, num1 = 0, num2 = 0;
    switch (exp->kind)
    {
    case A_idExp:
    case A_numExp:
        result = 0;
        break;
    case A_opExp:
        num1 = maxargsInExp(exp->u.op.left);
        num2 = maxargsInExp(exp->u.op.right);
        result = num1 > num2 ? num1 : num2;
        break;
    case A_eseqExp:
        num1 = maxargs(exp->u.eseq.stm);
        num2 = maxargsInExp(exp->u.eseq.exp);
        result = num1 > num2 ? num1 : num2;
        break;
    default:
        break;
    }
    return result;
}

int maxargsInExpList(A_expList exps)
{
    int result = 0;
    if (exps->kind == A_pairExpList)
    {
        int num1 = maxargsInExp(exps->u.pair.head);
        int num2 = maxargsInExpList(exps->u.pair.tail);
        int num3 = countPrintArgsInExpList(exps->u.pair.tail) + 1;
        if (num1 > num2 && num1 > num3)
            result = num1;
        else if (num2 > num1 && num2 > num3)
            result = num2;
        else
            result = num3;
    }
    else
    {
        result = maxargsInExp(exps->u.last);
    }
    return result;
}

int countPrintArgsInExpList(A_expList exps)
{
    int result = 0;
    if (exps->kind == A_pairExpList)
    {
        result = 1 + countPrintArgsInExpList(exps->u.pair.tail);
    }
    else
    {
        result = 1;
    }
    return result;
}

Table_ interpStm(A_stm s, Table_ t)
{
    struct IntAndTable temp;
    switch (s->kind)
    {
    case A_compoundStm:
        t = interpStm(s->u.compound.stm1, t);
        t = interpStm(s->u.compound.stm2, t);
        break;
    case A_assignStm:
        temp = interpExp(s->u.assign.exp, t);
        t = temp.t;
        t = update(t, s->u.assign.id, temp.i);
        break;
    case A_printStm:
        // printf("t = %p\n", t);
        t = interpExpList(s->u.print.exps, t);
        printf("\n");
        break;
    default:
        break;
    }
    return t;
}

struct IntAndTable interpExp(A_exp e, Table_ t)
{
    struct IntAndTable result, temp1, temp2;
    switch (e->kind)
    {
    case A_idExp:
        result.i = lookup(t, e->u.id);
        result.t = t;
        break;
    case A_numExp:
        result.i = e->u.num;
        result.t = t;
        break;
    case A_opExp:
        temp1 = interpExp(e->u.op.left, t);
        temp2 = interpExp(e->u.op.right, temp1.t);
        result.t = temp2.t;
        switch (e->u.op.oper)
        {
        case A_plus:
            result.i = temp1.i + temp2.i;
            break;
        case A_minus:
            result.i = temp1.i - temp2.i;
            break;
        case A_times:
            result.i = temp1.i * temp2.i;
            break;
        case A_div:
            result.i = temp1.i / temp2.i;
            break;
        default:
            break;
        }
        break;
    case A_eseqExp:
        t = interpStm(e->u.eseq.stm, t);
        result = interpExp(e->u.eseq.exp, t);
        break;
    default:
        break;
    }
    return result;
}

Table_ interpExpList(A_expList exps, Table_ t)
{
    // printf("line = %d; t = %p\n", __LINE__, t);
    struct IntAndTable temp;
    if (exps->kind == A_pairExpList)
    {
        temp = interpExp(exps->u.pair.head, t);
        printf("%d ", temp.i);
        t = temp.t;
        t = interpExpList(exps->u.pair.tail, t);
    }
    else
    {
        temp = interpExp(exps->u.last, t);
        printf("%d ", temp.i);
        t = temp.t;
    }
    return t;
}

int lookup(Table_ t, string key)
{
    while (t != NULL)
    {
        if (!strcmp(t->id, key))
        {
            return t->value;
        }
        t = t->tail;
    }
    printf("Lookup value failed\n");
    return 0;
}

Table_ update(Table_ t, string key, int value)
{
    t = Table(key, value, t);
}