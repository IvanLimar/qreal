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
#include <string.h>

#include "global_vars.h"
void fprintf_char(FILE *f, unsigned int wchar);

void tables()
{
	int i=0, j;

	fprintf(output, "\n%s\n", "source");
	for (i=1; i<line; i++)
	{
		fprintf(output, "line %i) ", i);
		for (j=lines[i]; j<lines[i+1]; j++)
		{
			fprintf_char(output, source[j]);
		}
	}
	fprintf(output, "\n");
	/*    fprintf(output, "%s\n", "hashtab");
	 for (i=0; i<256; i++)
	 if (hashtab[i])
	 fprintf(output, "hash= %i hashtab[hash]= %i\n", i, hashtab[i]);
	 */
	fprintf(output, "\n%s\n", "reprtab");
	i = 1125;
	while (i < rp)
	{
		fprintf(output, "repr %i) %i\n", i, reprtab[i]);
		fprintf(output, "repr %i) %i\n", i+1, reprtab[i+1]);
		i += 2;
		do
		{
			fprintf(output, "repr %i) ", i);
			fprintf_char(output, reprtab[i]);
			fprintf(output, "\n");
		}
		while (reprtab[i++]);
	}

	fprintf(output, "\n%s\n", "identab");
	i = 2;
	while (i < id)
	{
		for (j=0; j<4; j++)
			fprintf(output, "id %i) %i\n", i +j, identab[i+j]);
		fprintf(output, "\n");
		i +=4;
	}

	fprintf(output, "\n%s\n", "functions");
	for (i=1; i<=funcnum; i++)
		fprintf(output, "fn %i) %i\n", i, functions[i]);

	fprintf(output, "\n%s\n", "modetab");
	for (i=0; i<md; i++)
		fprintf(output, "md %i) %i\n", i, modetab[i]);

	fprintf(output, "%s\n", "mem");
	i = 0;
	while (i < pc)
	{
		fprintf(output, "pc %i) ", i);
		switch (mem[i++])
		{
			case PRINTID:
				fprintf(output, "PRINTID ");
				fprintf(output, "%i ", mem[i++]);
				fprintf(output, "%i\n", mem[i++]);
				break;
			case PRINT:
				fprintf(output, "PRINT ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case GETID:
				fprintf(output, "GETID ");
				fprintf(output, "%i ", mem[i++]);
				fprintf(output, "%i\n", mem[i++]);
				break;
			case SETMOTOR:
				fprintf(output, "SETMOTOR\n");
				break;
			case SLEEP:
				fprintf(output, "SLEEP\n");
				break;

			case GETDIGSENSOR:
				fprintf(output, "GETDIGSENSOR\n");
				break;
			case GETANSENSOR:
				fprintf(output, "GETANSENSOR\n");
				break;
			case ABS:
				fprintf(output, "ABS\n");
				break;
			case ABSI:
				fprintf(output, "ABSI\n");
				break;
			case SQRT:
				fprintf(output, "SQRT\n");
				break;
			case EXP:
				fprintf(output, "EXP\n");
				break;
			case SIN:
				fprintf(output, "SIN\n");
				break;
			case COS:
				fprintf(output, "COS\n");
				break;
			case LOG:
				fprintf(output, "LOG\n");
				break;
			case LOG10:
				fprintf(output, "LOG10\n");
				break;
			case ASIN:
				fprintf(output, "ASIN\n");
				break;

			case DECX:
				fprintf(output, "DECX\n");
				break;
			case DEFARR:
				fprintf(output, "DEFARR ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case DEFARR2:
				fprintf(output, "DEFARR2 ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case NOP:
				fprintf(output, "NOP\n");
				break;
			case LI:
				fprintf(output, "LI ");
				numfloat = (float)mem[i];
				//memcpy(&numfloat, &mem[i], sizeof(int));
				fprintf(output, "%i %f\n", mem[i++], numfloat);
				break;
			case LOAD:
				fprintf(output, "LOAD ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case LAT:
				fprintf(output,"L@\n");
				break;
			case ST:
				fprintf(output, "ST ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case LA:
				fprintf(output, "LA ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case LOGOR:
				fprintf(output, "||\n");
				break;
			case LOGAND:
				fprintf(output, "&&\n");
				break;
			case ORASS:
				fprintf(output, "|= ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case ORASSAT:
				fprintf(output, "|=@\n");
				break;
			case LOR:
				fprintf(output, "|\n");
				break;
			case EXORASS:
				fprintf(output, "^= ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case EXORASSAT:
				fprintf(output, "^=@\n");
				break;
			case LEXOR:
				fprintf(output, "^\n");
				break;
			case ANDASS:
				fprintf(output, "&= ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case ANDASSAT:
				fprintf(output, "&=@\n");
				break;
			case LAND:
				fprintf(output, "&\n");
				break;

			case EQEQ:
				fprintf(output, "==\n");
				break;
			case NOTEQ:
				fprintf(output, "!=\n");
				break;
			case LLT:
				fprintf(output, "<\n");
				break;
			case LGT:
				fprintf(output, ">\n");
				break;
			case LLE:
				fprintf(output, "<=\n");
				break;
			case LGE:
				fprintf(output, ">=\n");
				break;
			case EQEQR:
				fprintf(output, "==f\n");
				break;
			case NOTEQR:
				fprintf(output, "!=f\n");
				break;
			case LLTR:
				fprintf(output, "<f\n");
				break;
			case LGTR:
				fprintf(output, ">f\n");
				break;
			case LLER:
				fprintf(output, "<=f\n");
				break;
			case LGER:
				fprintf(output, ">=f\n");
				break;

			case SHRASS:
				fprintf(output, ">>= ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case SHRASSAT:
				fprintf(output, ">>=@\n");
				break;
			case LSHR:
				fprintf(output, ">>\n");
				break;
			case SHLASS:
				fprintf(output, "<<= ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case SHLASSAT:
				fprintf(output, "<<=@\n");
				break;
			case LSHL:
				fprintf(output, "<<\n");
				break;
			case ASS:
				fprintf(output, "= ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case ASSAT:
				fprintf(output, "=@\n");
				break;

			case PLUSASS:
				fprintf(output, "+= ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case PLUSASSAT:
				fprintf(output, "+=@\n");
				break;
			case LPLUS:
				fprintf(output, "+\n");
				break;
			case MINUSASS:
				fprintf(output, "-= ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case MINUSASSAT:
				fprintf(output, "-=@\n");
				break;
			case LMINUS:
				fprintf(output, "-\n");
				break;
			case MULTASS:
				fprintf(output, "*= ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case MULTASSAT:
				fprintf(output, "*=@\n");
				break;
			case LMULT:
				fprintf(output, "*\n");
				break;
			case DIVASS:
				fprintf(output, "/= ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case DIVASSAT:
				fprintf(output, "/=@\n");
				break;
			case LDIV:
				fprintf(output, "/\n");
				break;

			case PLUSASSR:
				fprintf(output, "+=f ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case PLUSASSATR:
				fprintf(output, "+=@f\n");
				break;
			case LPLUSR:
				fprintf(output, "+f\n");
				break;
			case MINUSASSR:
				fprintf(output, "-=f ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case MINUSASSATR:
				fprintf(output, "-=@f\n");
				break;
			case LMINUSR:
				fprintf(output, "-f\n");
				break;
			case MULTASSR:
				fprintf(output, "*=f ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case MULTASSATR:
				fprintf(output, "*=@f\n");
				break;
			case LMULTR:
				fprintf(output, "*f\n");
				break;
			case DIVASSR:
				fprintf(output, "/=f ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case DIVASSATR:
				fprintf(output, "/=@f\n");
				break;
			case LDIVR:
				fprintf(output, "/f\n");
				break;

			case REMASS:
				fprintf(output, "%%= ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case REMASSAT:
				fprintf(output, "%%=@\n");
				break;
			case LREM:
				fprintf(output, "%%\n");
				break;
			case CALL1:
				fprintf(output, "CALL1\n");
				break;
			case CALL2:
				fprintf(output, "CALL2 ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case STOP:
				fprintf(output, "STOP\n");
				break;
			case RETURN:
				fprintf(output, "RETURN\n");
				break;
			case RETURNV:
				fprintf(output, "RETURNV\n");
				break;
			case B:
				fprintf(output, "B ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case STRING:
			{
				int j, n;
				fprintf(output,"STRING ");
				fprintf(output, "%i\n", mem[i++]);
				fprintf(output, "n=%i\n", n = mem[i++]);
				for (j=0; j<n; j++)
					fprintf(output, "%c\n", mem[i++]);
				fprintf(output,"%i\n", mem[i++]);
				break;
			}
				break;
			case BE0:
				fprintf(output, "BE0 ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case BNE0:
				fprintf(output, "BNE0 ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case SLICE:
				fprintf(output, "SLICE\n");
				break;
			case ASSARR:
				fprintf(output, "ASSARR ");
				fprintf(output, "%i ", mem[i++]);
				fprintf(output, "%i\n", mem[i++]);
				break;
			case ASSARR2:
				fprintf(output, "ASSARR2 ");
				fprintf(output, "%i ", mem[i++]);
				fprintf(output, "%i\n", mem[i++]);
				break;
			case WIDEN:
				fprintf(output, "WIDEN\n");
				break;
			case WIDEN1:
				fprintf(output, "WIDEN1\n");
				break;
			case DOUBLE:
				fprintf(output, "DOUBLE\n");
				break;
			case INC:
				fprintf(output, "INC ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case DEC:
				fprintf(output, "DEC ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case POSTINC:
				fprintf(output, "POSTINC ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case POSTDEC:
				fprintf(output, "POSTDEC ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case INCAT:
				fprintf(output, "INC@\n");
				break;
			case DECAT:
				fprintf(output, "DEC@\n");
				break;
			case POSTINCAT:
				fprintf(output, "POSTINC@\n");
				break;
			case POSTDECAT:
				fprintf(output, "POSTDEC@\n");
				break;
			case INCR:
				fprintf(output, "INCf ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case DECR:
				fprintf(output, "DECf ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case POSTINCR:
				fprintf(output, "POSTINCf ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case POSTDECR:
				fprintf(output, "POSTDECf ");
				fprintf(output, "%i\n", mem[i++]);
				break;
			case INCATR:
				fprintf(output, "INC@f\n");
				break;
			case DECATR:
				fprintf(output, "DEC@f\n");
				break;
			case POSTINCATR:
				fprintf(output, "POSTINC@f\n");
				break;
			case POSTDECATR:
				fprintf(output, "POSTDEC@f\n");
				break;
			case LNOT:
				fprintf(output, "BITNOT\n");
				break;
			case LOGNOT:
				fprintf(output, "NOT\n");
				break;
			case UNMINUS:
				fprintf(output, "UNMINUS\n");
				break;
			case UNMINUSR:
				fprintf(output, "UNMINUSf\n");
				break;

			case FUNCBEG:
				fprintf(output, "FUNCBEG ");
				fprintf(output, "%i ", mem[i++]);
				fprintf(output, "%i\n", mem[i++]);
				break;


			default:
				fprintf(output, "%i\n", mem[i-1]);
				break;
		}
	}

}

void cd(int pcl)
{
	int i = pcl;
	while (i < pc)
	{
		wprintf(L"pc %i) ", i);
		switch (mem[i++])
		{
			case PRINTID:
				wprintf(L"PRINTID ");
				wprintf(L"%i ", mem[i++]);
				wprintf(L"%i\n", mem[i++]);
				break;
			case PRINT:
				wprintf(L"PRINT ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case GETID:
				wprintf(L"GETID ");
				wprintf(L"%i ", mem[i++]);
				wprintf(L"%i\n", mem[i++]);
				break;
			case SETMOTOR:
				wprintf(L"SETMOTOR\n");
				break;
			case SLEEP:
				wprintf(L"SLEEP\n");
				break;

			case GETDIGSENSOR:
				wprintf(L"GETDIGSENSOR\n");
				break;
			case GETANSENSOR:
				wprintf(L"GETANSENSOR\n");
				break;
			case ABS:
				wprintf(L"ABS\n");
				break;
			case ABSI:
				wprintf(L"ABSI\n");
				break;
			case SQRT:
				wprintf(L"SQRT\n");
				break;
			case EXP:
				wprintf(L"EXP\n");
				break;
			case SIN:
				wprintf(L"SIN\n");
				break;
			case COS:
				wprintf(L"COS\n");
				break;
			case LOG:
				wprintf(L"LOG\n");
				break;
			case LOG10:
				wprintf(L"LOG10\n");
				break;
			case ASIN:
				wprintf(L"ASIN\n");
				break;

			case DECX:
				wprintf(L"DECX\n");
				break;
			case NOP:
				wprintf(L"NOP\n");
				break;
			case DEFARR:
				wprintf(L"DEFARR ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case DEFARR2:
				wprintf(L"DEFARR2 ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case LI:
				wprintf(L"LI ");
				numfloat = (float)mem[i];
				//memcpy(&numfloat, &mem[i], sizeof(num));
				wprintf(L"%i %f\n", mem[i++], numfloat);
				break;
			case LOAD:
				wprintf(L"LOAD ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case LAT:
				wprintf(L"L@\n");
				break;
			case ST:
				wprintf(L"ST ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case LA:
				wprintf(L"LA ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case LOGOR:
				wprintf(L"||\n");
				break;
			case LOGAND:
				wprintf(L"&&\n");
				break;
			case ORASS:
				wprintf(L"|= ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case ORASSAT:
				wprintf(L"|=@\n");
				break;
			case LOR:
				wprintf(L"|\n");
				break;
			case EXORASS:
				wprintf(L"^= ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case EXORASSAT:
				wprintf(L"^=@\n");
				break;
			case LEXOR:
				wprintf(L"^\n");
				break;
			case ANDASS:
				wprintf(L"&= ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case ANDASSAT:
				wprintf(L"&=@\n");
				break;
			case LAND:
				wprintf(L"&\n");
				break;
			case EQEQ:
				wprintf(L"==\n");
				break;
			case NOTEQ:
				wprintf(L"!=\n");
				break;
			case LLT:
				wprintf(L"<\n");
				break;
			case LGT:
				wprintf(L">\n");
				break;
			case LLE:
				wprintf(L"<=\n");
				break;
			case LGE:
				wprintf(L">=\n");
				break;
			case EQEQR:
				wprintf(L"==f\n");
				break;
			case NOTEQR:
				wprintf(L"!=f\n");
				break;
			case LLTR:
				wprintf(L"<f\n");
				break;
			case LGTR:
				wprintf(L">f\n");
				break;
			case LLER:
				wprintf(L"<=f\n");
				break;
			case LGER:
				wprintf(L">=f\n");
				break;

			case SHRASS:
				wprintf(L">>= ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case SHRASSAT:
				wprintf(L">>=@\n");
				break;
			case LSHR:
				wprintf(L">>\n");
				break;
			case SHLASS:
				wprintf(L"<<= ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case SHLASSAT:
				wprintf(L"<<=@\n");
				break;
			case LSHL:
				wprintf(L"<<\n");
				break;
			case ASS:
				wprintf(L"= ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case ASSAT:
				wprintf(L"=@\n");
				break;
			case PLUSASS:
				wprintf(L"+= ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case PLUSASSAT:
				wprintf(L"+=@\n");
				break;
			case LPLUS:
				wprintf(L"+\n");
				break;
			case MINUSASS:
				wprintf(L"-= ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case MINUSASSAT:
				wprintf(L"-=@\n");
				break;
			case LMINUS:
				wprintf(L"-\n");
				break;
			case MULTASS:
				wprintf(L"*= ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case MULTASSAT:
				wprintf(L"*=@\n");
				break;
			case LMULT:
				wprintf(L"*\n");
				break;
			case DIVASS:
				wprintf(L"/= ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case DIVASSAT:
				wprintf(L"/=@\n");
				break;
			case LDIV:
				wprintf(L"/\n");
				break;
			case PLUSASSR:
				wprintf(L"+=f ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case PLUSASSATR:
				wprintf(L"+=@f\n");
				break;
			case LPLUSR:
				wprintf(L"+f\n");
				break;
			case MINUSASSR:
				wprintf(L"-=f ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case MINUSASSATR:
				wprintf(L"-=@f\n");
				break;
			case LMINUSR:
				wprintf(L"-f\n");
				break;
			case MULTASSR:
				wprintf(L"*=f ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case MULTASSATR:
				wprintf(L"*=@f\n");
				break;
			case LMULTR:
				wprintf(L"*f\n");
				break;
			case DIVASSR:
				wprintf(L"/=f ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case DIVASSATR:
				wprintf(L"/=@f\n");
				break;
			case LDIVR:
				wprintf(L"/f\n");
				break;

			case REMASS:
				wprintf(L"%%= ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case REMASSAT:
				wprintf(L"%%=@\n");
				break;
			case LREM:
				wprintf(L"%%\n");
				break;
			case CALL1:
				wprintf(L"CALL1\n");
				break;
			case CALL2:
				wprintf(L"CALL2 ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case STOP:
				wprintf(L"STOP\n");
				break;
			case RETURN:
				wprintf(L"RETURN\n");
				break;
			case RETURNV:
				wprintf(L"RETURNV\n");
				break;
			case B:
				wprintf(L"B ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case STRING:
			{
				int j, n;
				wprintf(L"STRING ");
				wprintf(L"%i\n", mem[i++]);
				wprintf(L"n=%i\n", n = mem[i++]);
				for (j=0; j<n; j++)
					wprintf(L"%c\n", mem[i++]);
				wprintf(L"%i\n", mem[i++]);
				break;
			}
			case BE0:
				wprintf(L"BE0 ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case BNE0:
				wprintf(L"BNE0 ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case SLICE:
				wprintf(L"SLICE\n");
				break;
			case ASSARR:
				wprintf(L"ASSARR ");
				wprintf(L"%i ", mem[i++]);
				wprintf(L"%i\n", mem[i++]);
				break;
			case ASSARR2:
				wprintf(L"ASSARR2 ");
				wprintf(L"%i ", mem[i++]);
				wprintf(L"%i\n", mem[i++]);
				break;
			case WIDEN:
				wprintf(L"WIDEN\n");
				break;
			case WIDEN1:
				wprintf(L"WIDEN1\n");
				break;
			case DOUBLE:
				wprintf(L"DOUBLE\n");
				break;
			case INC:
				wprintf(L"INC ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case DEC:
				wprintf(L"DEC ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case POSTINC:
				wprintf(L"POSTINC ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case POSTDEC:
				wprintf(L"POSTDEC ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case INCAT:
				wprintf(L"INC@\n");
				break;
			case DECAT:
				wprintf(L"DEC@\n");
				break;
			case POSTINCAT:
				wprintf(L"POSTINC@\n");
				break;
			case POSTDECAT:
				wprintf(L"POSTDEC@\n");
				break;
			case INCR:
				wprintf(L"INCf ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case DECR:
				wprintf(L"DECf ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case POSTINCR:
				wprintf(L"POSTINCf ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case POSTDECR:
				wprintf(L"POSTDECf ");
				wprintf(L"%i\n", mem[i++]);
				break;
			case INCATR:
				wprintf(L"INC@f\n");
				break;
			case DECATR:
				wprintf(L"DEC@f\n");
				break;
			case POSTINCATR:
				wprintf(L"POSTINC@f\n");
				break;
			case POSTDECATR:
				wprintf(L"POSTDEC@f\n");
				break;
			case LNOT:
				wprintf(L"BITNOT\n");
				break;
			case LOGNOT:
				wprintf(L"NOT\n");
				break;
			case UNMINUS:
				wprintf(L"UNMINUS\n");
				break;
			case UNMINUSR:
				wprintf(L"UNMINUSf\n");
				break;
			case FUNCBEG:
				wprintf(L"FUNCBEG ");
				wprintf(L"%i ", mem[i++]);
				wprintf(L"%i\n", mem[i++]);
				break;

			default:
				wprintf(L"\n wrong cop %i\n", mem[i-1]);
				break;
		}
	}
}

