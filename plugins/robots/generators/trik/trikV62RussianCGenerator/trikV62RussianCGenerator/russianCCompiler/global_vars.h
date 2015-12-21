/* Copyright 2013 - 2015 A. N. Terekhov
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License. */

///Users/ant/Desktop/RuC/RuC/main.c

#ifndef RuC_global_vars_h
#define RuC_global_vars_h

#include "Defs.h"

extern FILE *input;
extern FILE *output;

extern float numfloat;
extern int line, charnum, cur, next, next1, num, numr, hash, repr, keywordsnum,
source[SOURCESIZE], lines[LINESSIZE], linespc[LINESSIZE];


extern int nextchar, curchar, func_def;
extern int hashtab[256], reprtab[MAXREPRTAB], rp, identab[MAXIDENTAB], id, modetab[MAXMODSIZE], md, functions[], funcnum,
stack[100], stackop[100], stackoperands[100],
    sp, sopnd, aux, lastid, curid, lg, displ, maxdispl, maxdisplg, type, op, firstdecl, point;
extern int adcont, adbreak, adcase, notrobot, instring;

extern int box, boxdispl;
extern int anst, anstdispl, ansttype;
// anst = VAL  - значение на стеке
// anst = ADDR - на стеке адрес значения
// anst = IDENT- значение в статике, в anstdisl смещение отl или g
// в ansttype всегда тип возвращаемого значения (сейчас только LINT или LFLOAT)

// box - запрос на значение
// F, если значение можно разместить как угодно,
// VAL, если значение нужно поместить на верхушке стека
// DECX, если значения не нужно
// opassn+11, если нужно присвоить значение по адресу из верхушки стека,
// opassn,    если нужно присвоить переменной, смещение в boxdispl

extern int l, g, x, pc, pcline, mem[MAXMEMSIZE], functions[FUNCSIZE], funcnum, functype, kw, blockflag,
entry, wasmain, wasret;


#endif
