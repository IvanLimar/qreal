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


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <wchar.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <windows.h>

#include "global_vars.h"


// Глобальные переменные

FILE *input, *output;
float numfloat;
int line=0, charnum=1, cur, next, next1, num, numr, hash, repr, keywordsnum=1,
source[SOURCESIZE], lines[LINESSIZE], linespc[LINESSIZE];
int nextchar, curchar, func_def;
int hashtab[256], reprtab[MAXREPRTAB], rp=1, identab[MAXIDENTAB], id = 2, modetab[MAXMODSIZE], md = 2, startmode = 1;
int stack[100], stackop[100], stackoperands[100],
sp=0, sopnd=-1, aux=0, lastid, curid = 2, lg=-1, displ=-2, maxdispl = 3, maxdisplg = 3, type, op, firstdecl, point;
int adcont, adbreak, adcase, notrobot = 1, instring = 0;

int box, boxdispl;
int anst, anstdispl, ansttype;                // anst = VAL  - значение на стеке
											  // anst = ADDR - на стеке адрес значения
											  // anst = IDENT- значение в статике, в anstdisl смещение отl или g
											  // в ansttype всегда тип возвращаемого значения (сейчас только LINT или LFLOAT)

											  // box - запрос на значение
											  // F, если значение можно разместить как угодно,
											  // VAL, если значение нужно поместить на верхушке стека
											  // DECX, если значения не нужно
											  // opassn+11, если нужно присвоить значение по адресу из верхушки стека,
											  // opassn,    если нужно присвоить переменной, смещение в boxdispl

int l=0, g=0, x=0, pc=0, pcline=0, mem[MAXMEMSIZE], functions[FUNCSIZE], funcnum = 2, functype, kw = 0, blockflag = 1,
entry, wasmain = 0, wasret;

extern void cd(int pcl);
extern void tables();
extern void import();
extern void printf_char(int wchar);
extern void printident(int r);
extern int  getnext();
extern void endofline();
extern void endnl();
extern int  nextch();
extern int  scaner();
extern int  scan();

void error(int ernum)
{
	int i;
	tables();
	wprintf(L"line %i) ", line);
	for (i=lines[line]; i<charnum; i++)
		printf_char(source[i]);

	wprintf(L"\n");
	switch (ernum)
	{
		case after_type_must_be_ident:
			wprintf(L"после символа типа должен быть идентификатор или * идентификатор\n");
			break;
		case wait_right_sq_br:
			wprintf(L"ожидалась ]\n");
			break;
		case only_functions_may_have_type_VOID:
			wprintf(L"только функции могут иметь тип ПУСТО\n");
			break;
		case decl_and_def_have_diff_type:
			wprintf(L"прототип функции и ее описание имеют разные типы\n");
			break;
		case decl_must_start_from_ident_or_decl:
			wprintf(L"описание может начинаться только с описателя или идентификатора\n");
			break;
		case no_comma_in_param_list:
			wprintf(L"параметры должны разделяться запятыми\n");
			break;
		case wrong_param_list:
			wprintf(L"неправильный список параметров\n");
			break;
		case no_comma_in_type_list:
			wprintf(L"типы должны разделяться запятыми\n");
			break;
		case wrong_type_list:
			wprintf(L"неправильный список типов\n");
			break;
		case func_def_must_be_first:
			wprintf(L"определение функции должно быть первым в списке описаний\n");
			break;
		case func_def_must_have_param_list:
			wprintf(L"определение функции должно иметь список параметров, а не типов\n");
			break;
		case def_must_end_with_semicomma:
			wprintf(L"список описаний должен заканчиваться ;\n");
			break;
		case func_and_protot_have_dif_num_params:
			wprintf(L"функция и прототип имеют разное количество параметров\n");
			break;
		case param_types_are_dif:
			wprintf(L"не совпадают типы параметров функции и прототипа\n");
			break;
		case wait_ident_after_comma_in_decl:
			wprintf(L"в описании после , ожидается идентификатор или * идентификатор\n");
			break;
		case wait_rightbr_in_call:
			wprintf(L"нет ) в вызове функции\n");
			break;
		case func_decl_req_params:
			wprintf(L"вообще-то я думал, что это предописание функции (нет идентификаторов-параметров), а тут тело функции\n");
			break;
		case wait_while_in_do_stmt:
			wprintf(L"ждем ПОКА а операторе ЦИКЛ\n");
			break;
		case no_semicolon_after_stmt:
			wprintf(L"нет ; после оператора\n");
			break;
		case cond_must_be_in_brkts:
			wprintf(L"условие должно быть в ()\n");
			break;
		case repeated_decl:
			wprintf(L"повторное описание идентификатора ");
			printident(repr);
			wprintf(L"\n");
			break;
		case arr_init_must_start_from_BEGIN:
			wprintf(L"инициализация массива должна начинаться со {\n");
			break;
		case no_comma_in_init_list:
			wprintf(L"между элементами инициализации массива должна быть ,\n");
			break;
		case ident_is_not_declared:
			wprintf(L"не описан идентификатор ");
			printident(repr);
			wprintf(L"\n");
			break;
		case no_rightsqbr_in_slice:
			wprintf(L"не хватает ] в вырезке элемента массива\n");
			break;
		case void_in_expr:
			wprintf(L"в выражении встретился элемент типа ПУСТО\n");
			break;
		case index_must_be_int:
			wprintf(L"индекс элемента массива должен иметь тип ЦЕЛ\n");
			break;
		case slice_not_from_array:
			wprintf(L"попытка вырезки элемента не из массива\n");
			break;
		case call_not_from_function:
			wprintf(L"попытка вызова не функции\n");
			break;
		case no_comma_in_act_params:
			wprintf(L"после фактического параметра должна быть ,\n");
			break;
		case float_instead_int:
			wprintf(L"формальный параметр имеет тип ЦЕЛ, а фактический - ВЕЩ\n");
			break;
		case wrong_number_of_params:
			wprintf(L"неправильное количество фактических параметров\n");
			break;
		case wait_rightbr_in_primary:
			wprintf(L"не хватает ) в первичном выражении\n");
			break;
		case unassignable_inc:
			wprintf(L"++ и -- применимы только к переменным и элементам массива\n");
			break;
		case wrong_addr:
			wprintf(L"операция получения адреса & применима только к переменным\n");
			break;
		case no_colon_in_cond_expr:
			wprintf(L"нет : в условном выражении\n");
			break;
		case not_assignable:
			wprintf(L"слева от присваивания или операции с присваиванием может быть только переменная или элемент массива\n");
			break;
		case func_not_in_call:
			wprintf(L"функция может быть использована только в вызове\n");
			break;
		case no_colon_in_case:
			wprintf(L"после выражения в выборе нет :\n");
			break;
		case case_after_default:
			wprintf(L"встретился выбор после умолчания\n");
			break;
		case no_ident_after_goto:
			wprintf(L"после goto должна быть метка, т.е. идентификатор\n");
			break;
		case no_leftbr_in_for:
			wprintf(L"в операторе цикла ДЛЯ нет (\n");
			break;
		case no_semicolon_in_for:
			wprintf(L"в операторе цикла ДЛЯ нет ;\n");
			break;
		case no_rightbr_in_for:
			wprintf(L"в операторе цикла ДЛЯ нет )\n");
			break;
		case int_op_for_float:
			wprintf(L"операция, применимая только к целым, применена к вещественному аргументу\n");
			break;
		case assmnt_float_to_int:
			wprintf(L"нельзя присваивать целому вещественное значение\n");
			break;
		case more_than_1_main:
			wprintf(L"в программе может быть только 1 идентификатор ГЛАВНАЯ\n");
			break;
		case no_main_in_program:
			wprintf(L"в каждой программе должна быть ГЛАВНАЯ функция\n");
			break;
		case no_leftbr_in_printid:
			wprintf(L"в колманде ПЕЧАТЬИД или ЧИТАТЬИД нет (\n");
			break;
		case no_rightbr_in_printid:
			wprintf(L"в команде ПЕЧАТЬИД или ЧИТАТЬИД нет )\n");
			break;
		case no_ident_in_printid:
			wprintf(L"в команде ПЕЧАТЬИД или ЧИТАТЬИД нет идентификатора\n");
			break;
		case float_in_switch:
			wprintf(L"в условии переключателя нельзя использовать ВЕЩ\n");
			break;
		case init_int_by_float:
			wprintf(L"целая или литерная переменная инициализируется значением типа ВЕЩ\n");
			break;
		case must_be_digit_after_dot:
			wprintf(L"должна быть цифра перед или после .");
			break;
		case no_leftbr_in_setmotor:
			wprintf(L"в команде ПУСКМОТОРА нет(\n");
			break;
		case no_rightbr_in_setmotor:
			wprintf(L"в команде ПУСКМОТОРА нет)\n");
			break;
		case no_comma_in_setmotor:
			wprintf(L"в команде ПУСКМОТОРА после первого параметра нет ,\n");
			break;
		case param_setmotor_not_int:
			wprintf(L"в командах ПУСКМОТОРА, СПАТЬ, ЦИФРДАТЧИК и АНАЛОГДАТЧИК параметры должны быть целыми\n");
			break;
		case no_leftbr_in_sleep:
			wprintf(L"в команде СПАТЬ нет(\n");
			break;
		case no_rightbr_in_sleep:
			wprintf(L"в команде СПАТЬ нет)\n");
			break;
		case no_leftbr_in_stand_func:
			wprintf(L"в вызове  стандартной функции нет (\n");
			break;
		case no_rightbr_in_stand_func:
			wprintf(L"в вызове  стандартной функции нет )\n");
			break;
		case bad_param_in_stand_func:
			wprintf(L"параметры стандартных функций могут быть только целыми и вещественными\n");
			break;
		case no_ret_in_func:
			wprintf(L"в функции, возвращающей непустое значение, нет оператора ВОЗВРАТ со значением\n");
			break;
		case bad_type_in_ret:
			wprintf(L"в функции, возвращающей целое или литерное значение, оператор ВОЗВРАТ со значением ВЕЩ\n");
			break;
		case notvoidret_in_void_func:
			wprintf(L"в функции, возвращающей пустое значение, оператор ВОЗВРАТ со значением\n");
			break;
		case bad_escape_sym:
			wprintf(L"неизвестный служебный символ\n");
			break;
		case no_right_apost:
			wprintf(L"символьная константа не заканчивается символом '\n");
			break;
		case decl_after_strmt:
			wprintf(L"встретилось описание после оператора\n");
			break;
		case too_long_string:
			wprintf(L"слишком длинная строка ( больше, чем MAXSTRINGL)\n");
			break;
		case no_ident_after_aster:
			wprintf(L"в описании параметра функции после * нет идентификатора\n");
			break;
		case aster_before_func:
			wprintf(L"* перед описанием функции\n");
			break;
		case aster_not_for_pointer:
			wprintf(L"операция * применяется не к указателю\n");
			break;
		case aster_with_row:
			wprintf(L"операцию * нельзя применять к массивам\n");
			break;
		case wrong_fun_as_param:
			wprintf(L"неправильная запись функции, передаваемой параметром в другую функцию\n");
			break;
		case no_right_br_in_paramfun:
			wprintf(L"нет ) в функции, передаваемой параметром в другую функцию\n");
			break;
		case no_ident_in_paramfun:
			wprintf(L"нет идентификатора в  параметре определения функции\n");
			break;
		case par_type_void_with_nofun:
			wprintf(L"в параметре функции тип пусто может быть только у параметра-функции\n");
			break;
		case ident_in_declarator:
			wprintf(L"в деклараторах (предописаниях) могут быть только типы, но без идентификаторов-параметров\n");
			break;
		case array_before_func:
			wprintf(L"функция не может выдавать значение типа массив\n");
			break;
		case wait_definition:
			wprintf(L"вообще-то, я думал, что это определение функции, а тут нет идентификатора-параметра\n");
			break;
		case wait_declarator:
			wprintf(L"вообще-то, я думал, что это предописание функции, а тут идентификатор-параметр\n");
			break;
		case two_idents_for_1_declarer:
			wprintf(L"в описании функции на каждый описатель должен быть один параметр\n");
			break;
		case function_has_no_body:
			wprintf(L"есть параметры определения функции, но нет блока, являющегося ее телом\n");
			break;
		case declarator_in_call:
			wprintf(L"предописание нельзя использовать в вызове\n");
			break;
		case diff_formal_param_type_and_actual:
			wprintf(L"типы формального и фактического параметров различаются\n");
			break;


		default: ;
	}
	exit(2);
}

void mustbe(int what, int e)
{
	if (scaner() != what)
		error(e);
}

void tocode(int op)
{
	mem[pc++] = op;
}

void tocodef(int op)
{
	mem[pc++] = op;
	if (ansttype == LFLOAT && ((op >= PLUSASS && op <= DIVASSAT) || (op >= EQEQ && op <= UNMINUS)) )
		mem[pc-1] += 50;
}


int toidentab(int f)                         // f=0, если не ф-ция, f=1, если метка, f=funcnum, если описание ф-ции и -1, если ф-ция-параметр
{
//    wprintf(L"\n repr %i rtab[repr] %i rtab[repr+1] %i rtab[repr+2] %i\n", repr, reprtab[repr], reprtab[repr+1], reprtab[repr+2]);
	int pred;
	lastid = id;
	if (reprtab[repr+1] == 0)                  // это может быть только MAIN
	{
		if (wasmain)
			error(more_than_1_main);
		wasmain = id;
	}
	pred = identab[id] = reprtab[repr+1];      // ссылка на описание с таким же представлением в предыдущем блоке
	if (pred)
		reprtab[repr+1] = id;                  // ссылка на текущее описание с этим представлением ( это в reprtab)
	id++;

	if (pred >= curid)
		if (func_def == 3 ? 1 : identab[pred+1] > 0 ? 1 : func_def ==  1 ? 0 : 1)
			error(repeated_decl);              // только определение функции может иметь 2 описания, т.е. иметь предописание

	identab[id++] = repr;                      // ссылка на представление
											   // дальше тип или ссылка на modetab (для функций и структур)
	identab[id++] = type;                      // тип -1 int, -2 char, -3 float, -5 []int, -6 []char, -7 []float,
											   //                       -15 [][]int, -16 [][]char, -17 [][]float
											   //-9 func int, -10 funcchar, -11 func float, -12 func void, 0-метка
											   // если на 20 меньше, то это указатель на нужный тип
	if (f == 1)                                // если тип > 0, то это ссылка на modetab (для деклараторов и структур)
	{
		identab[lastid+2] = 0;
		identab[id++] = pc;
	}
	else if (f)
	{
			if (f < 0)
			{
				identab[id] = -(++displ);
				maxdispl = (displ > maxdispl) ? displ : maxdispl;
			}
			else                                                   // identtab[lastid+3] - номер функции, если < 0, то это функция-параметр
			{
				identab[id] = (pred >= curid) ? identab[pred+3] : funcnum;
				if (func_def == 2)
					identab[lastid+1] *= -1;
			}
//        wprintf(L"toident id=%i lastid=%i func_def=%i identab[id]=%i\n", id, lastid, func_def, identab[id]);
		id++;
	}
	else
	{
		identab[id++] = displ += lg;            // смещение от l (полож) или от g (отриц), для меток - значение pc, для функций - номер

		if (displ < 0)
			maxdisplg = (-displ > maxdisplg) ? -displ : maxdisplg;
		else
			maxdispl = (displ > maxdispl) ? displ : maxdispl;
	}
	return lastid;
}

void binop(int op)
{
	int rtype = stackoperands[sopnd--];
	int ltype = stackoperands[sopnd];
	if ((ltype == LINT || ltype == LCHAR) && rtype == LFLOAT)
		tocode(WIDEN1);
	if ((rtype == LINT || rtype == LCHAR) && ltype == LFLOAT)
		tocode(WIDEN);
	if (ltype == LFLOAT || rtype == LFLOAT)
		ansttype = LFLOAT;
	stackoperands[sopnd] = ansttype;
	tocodef(op);
}

void toval()
{
	if (anst == IDENT)
	{
		tocode(LOAD);
		tocode(anstdispl);
	}
	else if (anst == ADDR)
		tocode(LAT);
	anst = VAL;
}

void expr();

void exprassn();



void primaryexpr()
{
	if (cur == NUMBER)
	{
		tocode(LI);
		tocode((ansttype == LFLOAT) ? numr : num);   // LINT, LCHAR, ROWOFCHAR
		anst = VAL;    // ansttype задается прямо в сканере
		stackoperands[++sopnd] = ansttype;
	}
	else if (cur == IDENT)
	{
		lastid = reprtab[repr+1];
		if (lastid == 1)
			error(ident_is_not_declared);
		anst = IDENT;
		ansttype  = identab[lastid+2];
		anstdispl = identab[lastid+3];
		stackoperands[++sopnd] = ansttype;
	}
	else if (cur == LEFTBR)
	{
		int oldsp = sp;
		scaner();
		expr();
		mustbe(RIGHTBR, wait_rightbr_in_primary);
		while (sp > oldsp)
			binop(stackop[--sp]);
	}
	else if (cur <= -23)            // стандартная функция
	{
		int func = cur;
		mustbe(LEFTBR, no_leftbr_in_stand_func);
		box = VAL;
		scaner();
		exprassn();
		if (func == GETDIGSENSOR || func == GETANSENSOR)
		{
			notrobot = 0;
			if (ansttype != LINT)
				error(param_setmotor_not_int);
			tocode(func);
		}
		else if (func == ABS)
				if (ansttype == LINT)
					 tocode(ABSI);
				else if (ansttype == LFLOAT)
						 tocode(ABS);
					 else
						 error(bad_param_in_stand_func);
		else
		{
			if (ansttype == LINT)
			{
				stackoperands[sopnd] = LFLOAT;
				tocode(WIDEN);
			}
//            if (ansttype != LINT && ansttype != LFLOAT)
//                error(bad_param_in_stand_func);
			tocode(func);
		}
		mustbe(RIGHTBR, no_rightbr_in_stand_func);
	}
}

void postexpr()
{
	int oldbox = box, olddispl = boxdispl;
	primaryexpr();
	int leftansttype = ansttype, leftanstdispl = anstdispl;
	if (next == LEFTSQBR)
	{
		 if ((anst != VAL || ansttype != ROWOFCHAR) &&
			 (anst != IDENT || (ansttype != ROWOFINT && ansttype != ROWOFCHAR && ansttype != ROWOFFLOAT)) &&
			 (anst != IDENT || (ansttype != ROWROWOFINT && ansttype != ROWROWOFCHAR && ansttype != ROWROWOFFLOAT)))
			 error(slice_not_from_array);
		 if (anst == IDENT)
		 {
			 tocode(LOAD);
			 tocode(anstdispl);
		 }
		 scaner();
		 scaner();
		 box = VAL;
		 expr();
		 mustbe(RIGHTSQBR, no_rightsqbr_in_slice);
		 if (ansttype != LINT && ansttype != LCHAR)
			 error(index_must_be_int);
		tocode(SLICE);
		if (next == LEFTSQBR)
		{
			scaner();
			scaner();
			box = VAL;
			tocode(LAT);
			expr();
			mustbe(RIGHTSQBR, no_rightsqbr_in_slice);
			if (ansttype != LINT && ansttype != LCHAR)
				error(index_must_be_int);
			tocode(SLICE);
			ansttype = leftansttype + 14;
			stackoperands[--sopnd] = ansttype;
		}
		else
		{
			ansttype = leftansttype + 4;
			stackoperands[--sopnd] = ansttype;
		}
		anst = ADDR;
	}
	else if (next == LEFTBR)
	{
		int i, j, n;
		scaner();
		 if (anst != IDENT || leftansttype < 0)
			 error(call_not_from_function);
		if (identab[lastid+1] < 0)
			error(declarator_in_call);

		n = modetab[leftansttype+1];
		 tocode(CALL1);
		 j = leftansttype + 2;
		 for (i=0; i<n; i++)
		 {
			 int lid, mdj = modetab[j];  // это вид формального параметра, в ansttype будет вид фактического параметра
			 box = VAL;
			 scaner();
			 if (mdj > 0)
			 {
				 if (cur != IDENT || identab[(lid = reprtab[repr+1])+2] != mdj)
					 error(diff_formal_param_type_and_actual);
				 tocode(LI);
				 tocode(identab[lid+3]);
			 }
			 else
			 {
				 exprassn();
				 if ((mdj == LINT || mdj == LCHAR) && ansttype == LFLOAT)
					 error(float_instead_int);
				 if (mdj == LFLOAT && (ansttype == LINT || ansttype == LCHAR))
					 tocode(WIDEN);
				 if (ansttype != mdj)
					 error(diff_formal_param_type_and_actual);
			 }
			 if (i < n-1 && scaner() != COMMA)
				 error(no_comma_in_act_params);
			 j++;
		 }
		 mustbe(RIGHTBR, wrong_number_of_params);
		 anst = VAL;
		 ansttype = modetab[leftansttype] + 8;
		 stackoperands[--sopnd] = ansttype;
		 tocode(CALL2);
		 tocode(leftanstdispl);
		 anst = VAL;
	}
	if (anst == IDENT && leftansttype == LVOID)
		error(func_not_in_call);
	if (anst == IDENT || anst == ADDR)
	{
		if (next == INC || next == DEC)
		{
			int op = (next== INC) ? POSTINC : POSTDEC;
			if (anst == ADDR)
				op += 4;
			scaner();
			tocodef(op);
			if (anst == IDENT)
				tocode(anstdispl);
			anst = VAL;    // ansttype какой был, т.е. надо отдельные операции для ВЕЩ
		}
	}
	else if (next == INC || next == DEC)
		error(unassignable_inc);
	box = oldbox;
	boxdispl = olddispl;
}

void unarexpr()
{

	if (cur == LNOT || cur == LOGNOT || cur == LPLUS || cur == LMINUS || cur == LAND || cur == LMULT)
	{
		int op  = cur;
		scaner();
		unarexpr();
		if ((cur == LNOT || cur == LOGNOT) && ansttype == LFLOAT)
			error(int_op_for_float);
		if (op == LAND)
		{
			if (anst != IDENT && anst != ADDR)
				error(wrong_addr);
			if (anst == IDENT)
			{
				tocode((ansttype < -3) ? LOAD : LA);
				tocode(anstdispl);
				anst = ADDR;
			}
			ansttype -= 20;
		}
		else if (op == LMULT)
		{
			if (ansttype > -20 || (anst != ADDR && anst != IDENT))
				error(aster_not_for_pointer);
			ansttype += 20;
		}
		else
		{
			toval();
			if (op == LMINUS)
			{
				tocodef(UNMINUS);
			}
			else if (op == LPLUS)
				;
			else tocode(op);
			anst = VAL;
		}
	}
	else if (cur == INC || cur == DEC)
	{
		int op = cur;
		scaner();
		postexpr();
		if (anst == VAL)
			error(unassignable_inc);
		if (anst == ADDR)
			op += 4;
		tocodef(op);
		if (anst == IDENT)
			tocode(anstdispl);
		anst = VAL;
	}
	else
		postexpr();
}

void exprinbrkts()
{
	mustbe(LEFTBR, cond_must_be_in_brkts);
	scaner();
	box = VAL;
	expr();
	mustbe(RIGHTBR, cond_must_be_in_brkts);
}

int prio(int op)   // возвращает 0, если не операция
{
	return  op == LOGOR ? 1 : op == LOGAND ? 2 : op == LOR ? 3 : op == LEXOR ? 4 : op == LAND ? 5 :
	op == EQEQ ? 6 : op == NOTEQ ? 6 :
	op == LLT  ? 7 : op == LGT ?   7 : op  == LLE ? 7 : op == LGE ? 7 :
	op == LSHL ? 8 : op == LSHR ? 8 : op == LPLUS ? 9 : op == LMINUS ? 9 :
	op == LMULT ? 10 : op == LDIV ? 10 : op == LREM ? 10 : 0 ;
}

void subexpr()
{
	int p, oldsp = sp, oldbox = box;
	while ((p = prio(next)))
	{
		while (sp > oldsp && stack[sp-1] >= p)
			binop(stackop[--sp]);

		stack[sp] = p;
		stackop[sp++] = next;
		scaner();
		scaner();
		box = VAL;
		unarexpr();
		toval();
	}
	while (sp > oldsp)
		binop(stackop[--sp]);
	anst = VAL;
	box = oldbox;
}
int opassn()
{
	op = next;
	return  next == ASS || next == MULTASS || next == DIVASS ||next == REMASS || next == PLUSASS || next == MINUSASS ||
	next == SHLASS || next == SHRASS || next == ANDASS || next == EXORASS || next == ORASS;
}

void condexpr()
{
	int ad1, ad2 = 0, wasfloat = 0;
	subexpr();   // logORexpr();
	if (next == QUEST)
	{
		while (next == QUEST)
		{
			int oldbox = box;
			box = VAL;
			scaner();
			scaner();
			tocode(BE0);
			sopnd--;
			ad1 = pc++;
			expr();            // then
			tocode(NOP);
			sopnd--;
			if (ansttype == LFLOAT)
				wasfloat = 1;
			else
				mem[pc-1] = -NOP;
			tocode(B);
			mem[pc] = ad2;
			ad2 = pc++;
			mem[ad1] = pc;
			mustbe(COLON, no_colon_in_cond_expr);
			scaner();
			unarexpr();
			toval();
			subexpr();   // logORexpr();        else or elif
			box = oldbox;
		}
		if (wasfloat && ansttype != LFLOAT)
			tocode(WIDEN);
		while (ad2)
		{
			int r = mem[ad2];
			mem[ad2] = pc;
			if (mem[ad2-2] < 0)
				mem[ad2-2] = (wasfloat) ? WIDEN : NOP;
			ad2 = r;
		}
		if (wasfloat)
			stackoperands[sopnd] = ansttype = LFLOAT;
	}
}

void exprassn()
{
	int oldbox = box, olddispl = boxdispl;
	box = F;
	unarexpr();
	while (opassn())
	{
		if (anst == VAL)
			error(not_assignable);
		scaner();
		scaner();
		box = op;
		if (anst == ADDR)
			box += (op <= 7 ? 15 : 4);
		boxdispl = anstdispl;
		exprassn();
	}
	box = oldbox;
	boxdispl = olddispl;
	if (box != DECX && ansttype == LVOID)
		error(void_in_expr);
	toval();
	condexpr();
	if (box == F || box == VAL)
		anst = VAL;
	else if (box == DECX)
	{
		tocode(DECX);
		anst = DECX;
		sopnd--;
	}
	else
	{
		int rtype = stackoperands[sopnd--];
		int ltype = stackoperands[sopnd];
		if ((ltype == LINT || ltype == LCHAR) && rtype == LFLOAT)
			error(assmnt_float_to_int);
		if ((rtype == LINT ||rtype == LCHAR) && ltype == LFLOAT)
			tocode(WIDEN);
		if (ltype == LFLOAT || rtype == LFLOAT)
			ansttype = LFLOAT;
		stackoperands[sopnd] = ansttype;
		tocodef(box);
		if (box < 12)
			tocode(boxdispl);
		anst = VAL;
	}
}

void expr()
{
	exprassn();
	while (next == COMMA)
	{
		tocode(DECX);
		scaner();
		scaner();
		exprassn();
	}
}

int arrinit(int decl_type)
{
	int ni = 0;
	scaner();
	mustbe(BEGIN, arr_init_must_start_from_BEGIN);
	while (next != END)
	{
		ni++;
		box = VAL;
		scaner();
		exprassn();
		sopnd--;
		if ((decl_type == LINT || decl_type == LCHAR) && ansttype == LFLOAT)
			error(init_int_by_float);
		if (decl_type == LFLOAT && ansttype != LFLOAT)
			tocodef(WIDEN);
		if (next == COMMA)
			scaner();
		else if (next != END)
			error(no_comma_in_init_list);
	}
	scaner();
	return ni;

}

void decl_id()
{
	int oldid = toidentab(0);
	int decl_type = type;
	int olddispl = displ;
	if (next == ASS)
	{
		scaner();
		scaner();
		box = VAL;
		exprassn();
		sopnd--;
		if ((decl_type == LINT || decl_type == LCHAR) && ansttype == LFLOAT)
			error(init_int_by_float);
		if (decl_type == LFLOAT && ansttype != LFLOAT)
			tocodef(WIDEN);
		sopnd--;
		tocode(ST);
		tocode(olddispl);
	}
	else if (next == LEFTSQBR)
	{
		int olddispl = displ;
		scaner();
		scaner();
		box = VAL;
		unarexpr();
		condexpr();
		sopnd--;
		mustbe(RIGHTSQBR, wait_right_sq_br);
		if (next == ASS)
		{
			tocode(DEFARR);
			tocode(olddispl);
			identab[oldid+2] -= 4;
			int ni = arrinit(decl_type);
			tocode(ASSARR);
			tocodef(olddispl);
			tocode(ni);
		}
		else if (next == LEFTSQBR)
		{
			scaner();
			scaner();
			box = VAL;
			unarexpr();
			condexpr();
			sopnd--;
			mustbe(RIGHTSQBR, wait_right_sq_br);
			tocode(DEFARR2);
			tocode(olddispl);
			identab[oldid+2] -= 14;
			if (next == ASS)
			{
				int ni = arrinit(decl_type);
				tocode(ASSARR2);
				tocodef(olddispl);
				tocode(ni);
			}
		}
		else
		{
			tocode(DEFARR);
			tocode(olddispl);
			identab[oldid+2] -= 4;
		}
	}
}

void adbreakend()
{
	while (adbreak)
	{
		int r = mem[adbreak];
		mem[adbreak] = pc;
		adbreak = r;
	}
}

void adcontend()
{
	while (adcont)
	{
		int r = mem[adcont];
		mem[adcont] = pc;
		adcont = r;
	}
}

void block(int b);
 // если b=1, то это просто блок, иначе (b=0) это блок функции

void statement()
{
	int flagsemicol = 1;
	int oldbox = box;
	box = DECX;
	scaner();
	if ((cur == LINT || cur == LCHAR || cur == LFLOAT || cur == LVOID) && blockflag)
		error(decl_after_strmt);
	if (cur == BEGIN)
	{
		flagsemicol = 0;
		block(1);
	}
	else if (cur == SEMICOLON)
		flagsemicol = 0;
	else if (cur == IDENT && next == COLON)
	{
		int ppc, rr1;
		flagsemicol = 0;
		if ( (rr1 = reprtab[repr+1]) > 0 && (ppc = identab[rr1+3]) < 0)
		{
			ppc = -ppc;
			identab[rr1+3] = pc;
			while (ppc)
			{
				int r = mem[ppc];
				mem[ppc] = pc;
				ppc = r;
			}
		}
		else
			toidentab(1);
		scaner();
		statement();
	}
	else
	{
		blockflag = 1;
		switch (cur)
		{
			case PRINTID:
			{
				mustbe(LEFTBR, no_leftbr_in_printid);
				mustbe(IDENT, no_ident_in_printid);
				tocode(PRINTID);
				tocode(repr);
				tocode(reprtab[repr+1]);
				mustbe(RIGHTBR, no_rightbr_in_printid);
				break;
			}
			case PRINT:
				exprinbrkts();
				tocode(PRINT);
				tocode(ansttype);
				break;
			case GETID:
			{
				mustbe(LEFTBR, no_leftbr_in_printid);
				mustbe(IDENT, no_ident_in_printid);
				tocode(GETID);
				tocode(repr);
				tocode(reprtab[repr+1]);
				mustbe(RIGHTBR, no_rightbr_in_printid);
				break;
			}
			case SETMOTOR:
			{
				notrobot = 0;
				mustbe(LEFTBR, no_leftbr_in_setmotor);
				box = VAL;
				scaner();
				exprassn();
				if (ansttype != LINT)
					error(param_setmotor_not_int);
				mustbe(COMMA, no_comma_in_setmotor);
				box = VAL;
				scaner();
				exprassn();
				if (ansttype != LINT)
					error(param_setmotor_not_int);
				sopnd -= 2;
				tocode(SETMOTOR);
				mustbe(RIGHTBR, no_rightbr_in_setmotor);
				break;
			}
			case SLEEP:
			{
				notrobot = 0;
				mustbe(LEFTBR, no_leftbr_in_sleep);
				box = VAL;
				scaner();
				exprassn();
				if (ansttype != LINT)
					error(param_setmotor_not_int);
				stackoperands[sopnd] = ansttype;
				tocode(SLEEP);
				mustbe(RIGHTBR, no_rightbr_in_sleep);
				break;
			}

			case LBREAK:
			{
				tocode(B);
				mem[pc] = adbreak;
				adbreak = pc++;
			}
				break;
			case LCASE:
			{
				scaner();
				flagsemicol = 0;
				if (adcase < 0)
					error(case_after_default);
				if (adcase)
					mem[adcase] = pc;
				tocode(DOUBLE);
				box = VAL;
				unarexpr();
				condexpr();
				if (ansttype == LFLOAT)
					error(float_in_switch);
				sopnd--;
				if (next != COLON)
					error(no_colon_in_case);
				scaner();
				tocode(EQEQ);
				tocode(BE0);
				adcase = pc++;
				statement();
			}
				break;
			case LCONTINUE:
			{
				tocode(B);
				mem[pc] = adcont;
				adcont = pc++;
			}
				break;
			case LDEFAULT:
			{
				flagsemicol = 0;
				if (adcase)
					mem[adcase] = pc;
				if (next != COLON)
					error(no_colon_in_case);
				scaner();
				adcase = -1;
				statement();
			}
				break;
			case LDO:
			{
				int oldbreak = adbreak, oldcont = adcont, ad = pc;
				adbreak = 0;
				adcont = 0;
				statement();
				if (next == LWHILE)
				{
					scaner();
					adcontend();
					exprinbrkts();
					tocodef(BNE0);
					tocode(ad);
					sopnd--;
				}
				else
					error(wait_while_in_do_stmt);
				adbreakend();
				adbreak = oldbreak;
				adcont = oldcont;
			}
				break;
			case LFOR:
			{
				int adbeg, adinc=0, adbody, adend=0;
				mustbe(LEFTBR, no_leftbr_in_for);
				if (scaner() != SEMICOLON)             // init
				{
					box = DECX;
					expr();
					mustbe(SEMICOLON, no_semicolon_in_for);
				}
				adbeg = pc;
				if (scaner() != SEMICOLON)             // cond
				{
					box = VAL;
					expr();
					mustbe(SEMICOLON, no_semicolon_in_for);
					tocodef(BNE0);
					adbody = pc++;
					sopnd--;
					tocode(B);
					adend = pc++;
				}
				else
				{
					tocode(B);
					adbody = pc++;
				}
				if (scaner() != RIGHTBR)              // incr
				{
					box = DECX;
					adinc = pc;
					expr();
					mustbe(RIGHTBR, no_rightbr_in_for);
					tocode(B);
					tocode(adbeg);
				}
				mem[adbody] = pc;
				flagsemicol = 0;
				statement();
				tocode(B);
				tocode((adinc) ? adinc : adbeg);
				if (adend)
					mem[adend] = pc;
			}
				break;
			case LGOTO:
			{
				int ppc, rr1;
				mustbe(IDENT, no_ident_after_goto);
				tocode(B);

				if ( (rr1 = reprtab[repr+1]) > 0)
				{
					if ( (ppc = identab[rr1+3]) > 0)
						tocode(ppc);
					else
					{
						mem[pc] = -ppc;
						identab[rr1+3] = -pc;
						pc++;
					}
				}
				else
				{
					int labid = toidentab(1);
					identab[labid+3] = -pc;
					mem[pc++] = 0;
				}
			}
				break;
			case LIF:
			{
				int ad;
				flagsemicol = 0;
				exprinbrkts();
				tocodef(BE0);
				ad = pc++;
				sopnd--;
				statement();
				if (next == LELSE)
				{
					scaner();
					mem[ad] = pc + 2;
					tocode(B);
					ad = pc++;
					statement();
				}
				mem[ad] = pc;
			}
				break;
			case LRETURN:
			{
				int ftype = modetab[functype];
				wasret = 1;
				if (next == SEMICOLON)
				{
					if (ftype != FUNCVOID)
						error(no_ret_in_func);
					tocode(RETURNV);
				}
				else
				{
					scaner();
					if (ftype == FUNCVOID)
						error(notvoidret_in_void_func);
					box = VAL;
					expr();
					sopnd--;
					tocode(RETURN);
					if (ftype == FUNCINT && ansttype == LFLOAT)
						error(bad_type_in_ret);
				}
			}
				break;
			case LSWITCH:
			{
				int oldbreak = adbreak, oldcase = adcase;
				adbreak = 0;
				adcase = 0;
				flagsemicol = 0;
				box = VAL;
				exprinbrkts();
				if (ansttype == LFLOAT)
					error(float_in_switch);
				sopnd--;
				statement();
				if (adcase > 0)
					mem[adcase] = pc;
				adcase = oldcase;
				adbreakend();
				adbreak = oldbreak;
			}
				break;
			case LWHILE:
			{
				int oldbreak = adbreak, oldcont = adcont, ad = pc;
				adbreak = 0;
				adcont = 0;
				flagsemicol = 0;
				exprinbrkts();
				tocodef(BE0);
				mem[pc] = adbreak;
				adbreak = pc++;
				sopnd--;
				statement();
				adcontend();
				tocode(B);
				tocode(ad);
				adbreakend();
				adbreak = oldbreak;
				adcont = oldcont;
			}
				break;
			default:
				expr();
				break;
		}
		box = oldbox;
	}
	if (flagsemicol && scaner() != SEMICOLON)
		error(no_semicolon_after_stmt);
}

void idorpnt(int e)
{
	point = 0;
	if (next == LMULT)
	{
		scaner();
		point = -20;
	}
	mustbe(IDENT, e);
}

void block(int b)
{
	int notended = 1, i, olddispl, oldlg = lg, firstdecl;
	if (b)
	{
		olddispl = displ;
		curid = id;
	}
	blockflag = 0;
while ((firstdecl = next) == LINT || firstdecl == LCHAR || firstdecl == LFLOAT)
{
	scaner();
	int repeat = 1;
	idorpnt(after_type_must_be_ident);
	do
	{
		type = firstdecl + point;
		decl_id();
		if (next == COMMA)
		{
			scaner();
			idorpnt(wait_ident_after_comma_in_decl);
		}
		else if (next == SEMICOLON)
			 {
				 scaner();
				 repeat = 0;
			 }
			 else
				 error(def_must_end_with_semicomma);
	}
	while (repeat);
}

	// кончились описания, пошли операторы до }

	do
	{
		box = DECX;
		statement();
		if (next == END)
		{
			scaner();
			notended = 0;
		}
	}
	while (notended);
	if (b)
	{
		for (i=id-4; i>=curid; i-=4)
			reprtab[ identab[i+1]+1] = identab[i];
			displ = olddispl;
	}
		lg = oldlg;
}

int modeeq(int prot, int fun)
{
	int n = modetab[prot+1], i, flag = 1;
//    wprintf(L"modeeq prot=%i fun=%i n=%i\n", prot, fun, n);
	if (modetab[prot] != modetab[fun] || n != modetab[fun+1])
		return 0;
	else
	{
		for (i=0; i<n && flag ; i++)
			flag = modetab[prot+i+2] == modetab[fun+i+2];
		return flag;
	}
}
void function_definition()
{
	int fn = identab[lastid+3], i, pred, oldrepr = repr, ftype, n;
	functype = identab[lastid+2];
	ftype = modetab[functype];
	n = modetab[functype+1];
	wasret = 0;
	displ = 2;
	maxdispl =3;
	lg = 1;
	if ((pred = identab[lastid]) >= 1)            // был прототип
		if (functype != identab[pred+2])
			error(decl_and_def_have_diff_type);
	curid = id;
	for (i=0; i < n; i++)
	{
		type = modetab[functype+i+2];
		repr = functions[fn+i+1];
		if (repr > 0)
			toidentab(0);
		else
		{
			repr = -repr;
			toidentab(-1);
		}
	}
	functions[fn]= pc;
	tocode(FUNCBEG);
	pred = pc;
	pc += 2;
	repr = oldrepr;
	block(0);
	if (ftype == FUNCVOID && mem[pc-1] != RETURNV)
		tocode(RETURNV);
	if ((ftype == FUNCINT || ftype == FUNCCHAR || ftype == FUNCFLOAT) && !wasret)
		error(no_ret_in_func);
	for (i=id-4; i>=curid; i-=4)
		reprtab[ identab[i+1]+1] = identab[i];
	curid = 2;                                 // все функции описываются на одном уровне
	mem[pred] = maxdispl + 1;
	mem[pred+1] = pc;
	lg = -1;
}

int func_declarator(int level, int func_d, int firstdecl)
{
	// на 1 уровне это может быть определением функции или предописанием, на остальных уровнях - только декларатором (без идентов)

	int loc_modetab[100], locmd = 0, numpar = 0, ident, maybe_fun, repeat = 1, i, wastype = 0, old;

	loc_modetab[0] = firstdecl;
	loc_modetab[1] = 0;
	locmd = 2;

	while (repeat)
	{
		if ((type = cur) == LINT || cur == LCHAR || cur == LFLOAT)
		{
			maybe_fun = 0;    // м.б. параметр-ф-ция? 0 - ничего не было, 1 - была *, 2 - была [
			ident = 0;        // = 0 - не было идента, 1 - был статический идент, 2 - был идент-параметр-функция
			wastype = 1;
			point = 0;
			if (next == LMULT)
			{
				scaner();
				type -= 20;
				maybe_fun = 1;
			}
			if (level)
			{
				if (next == IDENT)
				{
					scaner();
					ident = 1;
					functions[funcnum++] = repr;
				}
			}
			else if (next == IDENT)
				error(ident_in_declarator);
			if (next == LEFTSQBR)
			{
				scaner();
				maybe_fun = 2;
				if (type < -20)
					error(aster_with_row);
				mustbe(RIGHTSQBR, wait_right_sq_br);
				if (next == LEFTSQBR)
				{
					scaner();
					mustbe(RIGHTSQBR, wait_right_sq_br);
					type -= 14;
				}
				else
					type -= 4;
			}
		}
		else if ((type = cur) == LVOID)
		{
			wastype = 1;
			if (next != LEFTBR)
				error(par_type_void_with_nofun);
		}
		if (wastype)
		{
			numpar++;
			loc_modetab[locmd++] = type;

			if (next == LEFTBR)
			{
				scaner();
				mustbe(LMULT, wrong_fun_as_param);
				if (next == IDENT)
				{
					if (level)
					{
						scaner();
						if (ident == 0)
							ident = 2;
						else
							error(two_idents_for_1_declarer);
						functions[funcnum++] = -repr;
					}
					else
						error(ident_in_declarator);
				}
				mustbe(RIGHTBR, no_right_br_in_paramfun);
				mustbe(LEFTBR, wrong_fun_as_param);
				scaner();
				if (maybe_fun == 1)
					error(aster_before_func);
				else if (maybe_fun == 2)
					error(array_before_func);

				old = func_def;
				loc_modetab[locmd-1] = func_declarator(0, 2, type-8);
				func_def = old;
			}
//            wprintf(L"level=%i ident1=%i func_d=%i loc_m=%i\n", level, ident, func_d, loc_modetab[locmd-2]);

			if (func_d == 3)
				func_d = ident > 0 ? 1 : 2;
			else if (func_d == 2 && ident > 0)
				error(wait_declarator);
			else if (func_d == 1 && ident == 0)
				error(wait_definition);

			if (scaner() == COMMA)
			{
				scaner();
			}
			else
				if (cur == RIGHTBR)
					repeat = 0;
		}
		else if (cur == RIGHTBR)
		{
			repeat = 0;
			func_def = 0;
		}
			 else
				 error(wrong_param_list);
	}
	func_def = func_d;
	locmd = md;
	loc_modetab[1] = numpar;
	for (i=0; i < numpar+2; i++)
		modetab[md++] = loc_modetab[i];
	old = modetab[startmode];
	while (old)
		if (modeeq(startmode+1, old+1))
			break;
		else
			old = modetab[old];
	if (old)
	{
		md = startmode + 1;
		return old+1;
	}
	modetab[md] = startmode;
	startmode = md++;
	return modetab[startmode]+1;
}

void ext_decl()
{
	do            // top level описания переменных и функций до конца файла
	{
		int repeat = 1, funrepr, first = 1;
		func_def = 3;   // func_def = 0 - (), 1 - определение функции, 2 - это предописание, 3 - не знаем или вообще не функция

		if ((type = scaner()) == LINT || type == LCHAR || type == LFLOAT || type == LVOID)
			idorpnt(after_type_must_be_ident);
		else
		{
			type = LINT;
			point = 0;
			if (cur == LMULT)
			{
				scaner();
				type -= 20;
			}
		}
		firstdecl = type;
		do                       // описываемые объекты через ',' определение функции может быть только одно, никаких ','
		{
			if (cur != IDENT)
				error(decl_must_start_from_ident_or_decl);

			if (firstdecl  < -20 && next == LEFTBR)
				error(aster_before_func);

			if (next == LEFTBR)                // определение или предописание функции
			{
				int oldfuncnum = funcnum++;
				funrepr = repr;
				scaner();
				scaner();
				type = func_declarator(first, 3, firstdecl-8);  // выкушает все параметры до ) включительно
				if (next == BEGIN)
				{
					if (func_def == 0)
						func_def = 1;
				}
				else if (func_def == 0)
					func_def = 2;
										  // теперь я точно знаю, это определение ф-ции или предописание (func_def=1 или 2)
				repr =funrepr;
				toidentab(oldfuncnum);

				if (next == BEGIN)
				{
					scaner();
					if (func_def == 2)
						error(func_decl_req_params);

					function_definition();
					goto ex;
				}
				else
				{
					if (func_def == 1)
					error(function_has_no_body);
				}
			}
			else
				if (firstdecl == LVOID)
					error(only_functions_may_have_type_VOID);

// описания идентов-не-функций

			if (func_def == 3)
				decl_id();
			if (next == COMMA)
			{
				scaner();
				first = 0;
				idorpnt(wait_ident_after_comma_in_decl);
				type = firstdecl + point;
			}
			else if (next == SEMICOLON)
				 {
					 scaner();
					 repeat = 0;
				 }
				 else
					error(def_must_end_with_semicomma);
		}
		while (repeat);
	ex: ;
	}
	while (next != LEOF);

	if (wasmain == 0)
		error(no_main_in_program);
	tocode(CALL1);
	tocode(CALL2);
	tocode(identab[wasmain+3]);
	tocode(STOP);
}

// конец компилятора - начало интерпретатора

int main(int argc, char * argv[])
{
	int i;

	_setmode(_fileno(stdout), _O_U16TEXT);

	if (argc < 2) {
		wprintf(L"Не указано имя теста при вызове RuC\n");
		return 1;
	}

	for (i=0; i<256; i++)
		hashtab[i] = 0;
	output = fopen("out.txt", "wt");

// занесение ключевых слов в reprtab
	input =  fopen("keywords.txt", "r");
	getnext();
	nextch();
	while (scan() != LEOF)
		;
	fclose(input);

	input =  fopen(argv[1], "r");
	if (input == NULL)
	{
		wprintf(L" не найден файл: ");
		wprintf(L"%s\n", argv[1]);
		return 1;
	}

//    while (fscanf(input, "%c", &curchar)!=EOF)
//        wprintf(L"%c %i\n", curchar, curchar);
//    exit(0);

	modetab[1] = 0;
	keywordsnum = 0;
	lines[line = 1] = 1;
	charnum = 1;
	kw = 1;
	getnext();
	nextch();
	next = scan();
	ext_decl();
	cd(0);
//    for (i=1; i<md; i++)
//        wprintf(L"md %i) %i\n", i, modetab[i]);
	tables();
	lines[line+1] = charnum;
	fclose(input);
	fclose(output);

	output = fopen("export.txt", "wt");

	fprintf(output, "%i %i %i %i %i\n", pc, funcnum, id, rp, maxdisplg);

	for (i=0; i<pc; i++)
		fprintf(output, "%i ", mem[i]);
	fprintf(output, "\n");

	for (i=0; i<funcnum; i++)
		fprintf(output, "%i ", functions[i]);
	fprintf(output, "\n");

	for (i=0; i<id; i++)
		fprintf(output, "%i ", identab[i]);
	fprintf(output, "\n");

	for (i=0; i<rp; i++)
		fprintf(output, "%i ", reprtab[i]);
	fprintf(output, "\n");

	fclose(output);

	if (notrobot)
		import();

	return 0;
}

