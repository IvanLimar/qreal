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

#ifndef RuC_Defs_h
#define RuC_Defs_h

#define MAXREPRTAB 10000
#define MAXIDENTAB 10000
#define MAXMEMSIZE 10000
#define MAXMODSIZE 10000
#define FUNCSIZE   1000
#define SOURCESIZE 10000
#define LINESSIZE  3000
#define MAXSTRINGL 128

// Лексемы

#define ASS       1
#define REMASS    2
#define SHLASS    3
#define SHRASS    4
#define ANDASS    5
#define EXORASS   6
#define ORASS     7

#define PLUSASS   8
#define MINUSASS  9
#define MULTASS   10
#define DIVASS    11

#define PLUSASSAT 12
#define MINUSASSAT 13
#define MULTASSAT 14
#define DIVASSAT  15

#define ASSAT     16   // все операции с присваиванием оставляют значение на стеке
#define REMASSAT  17
#define SHLASSAT  18
#define SHRASSAT  19
#define ANDASSAT  20
#define EXORASSAT 22
#define ORASSAT   23


#define LOGOR     24
#define LOGAND    25
#define LOR       26
#define LEXOR     27
#define LAND      28
#define LSHR      29
#define LSHL      30
#define LREM      31

#define EQEQ      32
#define NOTEQ     33
#define LLT       34
#define LGT       35
#define LLE       36
#define LGE       37
#define LPLUS     38
#define LMINUS    39
#define LMULT     40
#define LDIV      41
#define POSTINC   42
#define POSTDEC   43
#define INC       44
#define DEC       45
#define POSTINCAT 46
#define POSTDECAT 47
#define INCAT     48
#define DECAT     49
#define UNMINUS   50

#define PLUSASSR  58
#define MINUSASSR 59
#define MULTASSR  60
#define DIVASSR   61
#define PLUSASSATR  62
#define MINUSASSATR 63
#define MULTASSATR  64
#define DIVASSATR   65

#define EQEQR     82
#define NOTEQR    83
#define LLTR      84
#define LGTR      85
#define LLER      86
#define LGER      87
#define LPLUSR    88
#define LMINUSR   89
#define LMULTR    90
#define LDIVR     91
#define POSTINCR  92
#define POSTDECR  93
#define INCR      94
#define DECR      95
#define POSTINCATR 96
#define POSTDECATR 97
#define INCATR     98
#define DECATR     99
#define UNMINUSR  100

#define COMMA    149
#define QUEST    101
#define COLON    102
#define LEFTBR   103
#define RIGHTBR  104
#define LEFTSQBR 105
#define RIGHTSQBR 106
#define STRING   107
#define NUMBER   108
#define IDENT    109
#define VAL      110
#define ADDR     111
#define F        112


#define LNOT     113
#define LOGNOT   114
#define BEGIN    115
#define END      116
#define SEMICOLON 117
#define LERROR   118
#define LAPOST   119
#define LQUOTE   120

// Ключевые слова

#define LMAIN   0
#define LINT   -1
#define LCHAR  -2
#define LFLOAT -3
#define LVOID  -4

#define ROWOFINT   -5
#define ROWOFCHAR  -6
#define ROWOFFLOAT -7
#define ROWROWOFINT   -15
#define ROWROWOFCHAR  -16
#define ROWROWOFFLOAT -17

#define FUNCINT    -9
#define FUNCCHAR  -10
#define FUNCFLOAT -11
#define FUNCVOID  -12

#define LBREAK -5
#define LCASE  -6
#define LCONTINUE -7
#define LDEFAULT  -8
#define LDO    -9
#define LELSE  -10
#define LENUM  -11
#define LFOR   -12
#define LGOTO  -13
#define LIF    -14
#define LRETURN -15
#define LSIZEOF -16
#define LSWITCH -17
#define LWHILE  -18
#define PRINTID -19
#define PRINT   -20
#define GETID   -21
#define SETMOTOR  -22
#define SLEEP     -23     // до этого места операторы, а затем функции
#define GETDIGSENSOR -24
#define GETANSENSOR -25
#define ABS      -26
#define ABSI     -27
#define SQRT     -28
#define EXP      -29
#define SIN      -30
#define COS      -31
#define LOG      -32
#define LOG10    -33
#define ASIN     -34

#define LEOF    -111

// Коды ошибок

#define after_type_must_be_ident           201
#define wait_right_sq_br                   202
#define only_functions_may_have_type_VOID  203
#define decl_and_def_have_diff_type        204
#define decl_must_start_from_ident_or_decl 205
#define no_comma_in_param_list             206
#define wrong_param_list                   207
#define no_comma_in_type_list              208
#define wrong_type_list                    209
#define func_def_must_be_first             210
#define func_def_must_have_param_list      211
#define def_must_end_with_semicomma        212
#define func_and_protot_have_dif_num_params 213
#define param_types_are_dif                214
#define wait_ident_after_comma_in_decl     215
#define wait_rightbr_in_call               216
#define func_decl_req_params               217
#define wait_while_in_do_stmt              218
#define no_semicolon_after_stmt            219
#define cond_must_be_in_brkts              220
#define repeated_decl                      221
#define arr_init_must_start_from_BEGIN     222
#define no_comma_in_init_list              223
#define ident_is_not_declared              224
#define no_rightsqbr_in_slice              225
#define void_in_expr                       226
#define index_must_be_int                  227
#define slice_not_from_array               228
#define call_not_from_function             229
#define no_comma_in_act_params             230
#define float_instead_int                  231
#define wrong_number_of_params             232
#define wait_rightbr_in_primary            233
#define unassignable_inc                   234
#define wrong_addr                         235
#define no_colon_in_cond_expr              236
#define not_assignable                     237
#define func_not_in_call                   238
#define no_colon_in_case                   239
#define case_after_default                 240
#define no_ident_after_goto                241
#define no_leftbr_in_for                   242
#define no_semicolon_in_for                243
#define no_rightbr_in_for                  244
#define int_op_for_float                   245
#define assmnt_float_to_int                246
#define more_than_1_main                   247
#define no_main_in_program                 248
#define no_leftbr_in_printid               249
#define no_rightbr_in_printid              250
#define no_ident_in_printid                251
#define float_in_switch                    252
#define init_int_by_float                  253
#define must_be_digit_after_dot            254
#define no_leftbr_in_setmotor              255
#define no_rightbr_in_setmotor             256
#define no_comma_in_setmotor               257
#define param_setmotor_not_int             258
#define no_leftbr_in_sleep                 259
#define no_rightbr_in_sleep                260
#define no_leftbr_in_stand_func            261
#define no_rightbr_in_stand_func           262
#define bad_param_in_stand_func            263
#define no_ret_in_func                     264
#define bad_type_in_ret                    265
#define notvoidret_in_void_func            266
#define bad_escape_sym                     267
#define no_right_apost                     268
#define decl_after_strmt                   269
#define too_long_string                    270
#define no_ident_after_aster               271
#define aster_before_func                  272
#define aster_not_for_pointer              273
#define aster_with_row                     274

#define wrong_fun_as_param                 276
#define no_right_br_in_paramfun            277
#define no_ident_in_paramfun               278
#define par_type_void_with_nofun           279
#define ident_in_declarator                280
#define array_before_func                  281
#define wait_definition                    282
#define wait_declarator                    283
#define two_idents_for_1_declarer          284
#define function_has_no_body               285
#define declarator_in_call                 286
#define diff_formal_param_type_and_actual  287

// Коды операций виртуальной машины, кроме операций C

#define DECX 151
#define empty 152
#define NOP   153
#define DEFARR 154
#define LI   155
#define LOAD 156
#define LAT  157       // это L@
#define STOP 158
#define ST   159       // снимает значение со стека
#define FUNCBEG 160    // обозначает начало кода функции
#define LA      161
#define CALL1   162
#define CALL2   163
#define RETURN  164
#define RETURNV 165
#define B       166
#define BE0     167
#define BNE0    168
#define SLICE   169
#define ASSARR  170
#define WIDEN   171
#define WIDEN1  172
#define DOUBLE  173
#define DEFARR2 174
#define ASSARR2 175


#endif
