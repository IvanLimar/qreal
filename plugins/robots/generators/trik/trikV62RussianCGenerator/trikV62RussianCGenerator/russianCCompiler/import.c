/* Copyright 2013 - 2015 A. N. Terekhov, Ivan Limar
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


//#define ROBOT 1
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "global_vars.h"

#define I2CBUFFERSIZE 50

#define index_out_of_range  1
#define wrong_kop           2
#define wrong_arr_init      3
#define wrong_motor_num     4
#define wrong_motor_pow     5
#define wrong_digsensor_num 6
#define wrong_ansensor_num  7
#define wrong_robot_com     8

#ifdef WITHDIGITALSENSORS
FILE *f1, *f2;   // файлы цифровых датчиков
const char* JD1 = "/sys/devices/platform/da850_trik/sensor_d1";
const char* JD2 = "/sys/devices/platform/da850_trik/sensor_d2";
#endif

//int l, g, x, pc, mem[MAXMEMSIZE], functions[FUNCSIZE], funcnum;

//int reprtab[MAXREPRTAB], rp, identab[MAXIDENTAB], id, maxdisplg;

void printf_char(int wchar)
{
	if (wchar<128)
		wprintf(L"%c", wchar);
	else
	{
//		unsigned char first = (wchar >> 6) | 0b11000000;
//		unsigned char second = (wchar & 0b111111) | 0b10000000;

		wprintf(L"%c", wchar);
	}
}

void fprintf_char(FILE *f, unsigned int wchar)
{
	if (wchar<128)
		fprintf(f, "%c", wchar);
	else
	{
		unsigned char first = (wchar >> 6) | 0b11000000;
		unsigned char second = (wchar & 0b111111) | 0b10000000;

		fprintf(f, "%c%c", first, second);
	}
}


int dspl(int d)
{
	return d < 0 ? g-d : l+d;
}

int dsp()
{
	return dspl(mem[pc++]);
}

void runtimeerr(int e, int i, int r)
{
	switch (e)
	{
		case index_out_of_range:
			wprintf(L"индекс %i за пределами границ массива %i\n", i, r);
			break;
		case wrong_kop:
			wprintf(L"команду %i я пока не реализовал\n", i);
			break;
		case wrong_arr_init:
			wprintf(L"массив с %i элементами инициализируется %i значениями\n", i, r);
			break;
		case wrong_motor_num:
			wprintf(L"номер силового мотора %i, а должен быть от 1 до 4\n", i);
			break;
		case wrong_motor_pow:
			wprintf(L"задаваемая мощность мотора %i равна %i, а должна быть от -100 до 100\n", i, r);
			break;
		case wrong_digsensor_num:
			wprintf(L"номер цифрового сенсора %i, а должен быть 1 или 2\n", i);
			break;
		case wrong_ansensor_num:
			wprintf(L"номер аналогового сенсора %i, а должен быть от 1 до 6\n", i);
			break;
		case wrong_robot_com:
			wprintf(L"робот не может исполнить команду\n");
			break;


		default:
			break;
	}
	exit(3);
}

void printident(int r)
{
	r += 2;
	do
		printf_char(reprtab[r++]);
	while (reprtab[r] != 0);
	wprintf(L" ");
}

void prmem()
{
	int i;
	wprintf(L"mem=\n");
	for (i=g; i<=x; i++)
		wprintf(L"%i ) %i\n",i, mem[i]);
	wprintf(L"\n");

}

int rungetcommand(const char *command)
{
	FILE *fp;
	int x = -1;
	char path[100] = {'\0'};

	/* Open the command for reading. */
	fp = popen(command, "r");
	if (fp == NULL)
		runtimeerr(wrong_robot_com, 0,0);

	/* Read the output a line at a time - output it. */
	while (fgets(path, sizeof(path)-1, fp) != NULL)
	{
		x = strtol(path, NULL, 16);
		wprintf(L"[%s] %d\n", path, x);
	}
	pclose(fp);
	return x;                   // ??????
}


int auxpr(int r, int t)
{
	float rf;
	int j, flag=0;
	for (j=0; j<mem[r-1]; j++)
		if (t == ROWOFINT)
			wprintf(L" %i", mem[r+j]);
		else if (t == ROWOFCHAR)
			wprintf(L"%c", mem[r+j]);
		else if(t == ROWOFFLOAT)
		{
			rf = (float)mem[r + j];
			//memcpy(&rf, &mem[r+j], sizeof(int));
			wprintf(L" %f", rf);
		}
		else
			flag = 1;
	return flag;
}

void auxprint(int r, int t)
{
	float rf;
	if (t == LINT)
		wprintf(L" %i", r);
	else if (t == LCHAR)
		wprintf(L"%c", r);
	else if (t == LFLOAT)
	{
		rf = (float)r;
		//memcpy(&rf, &r, sizeof(int));
		wprintf(L" %f", rf);
	}
	else
	{
		int i;
		if (auxpr(r, t))
			// двухмерные массивы
			for (i=0; i<mem[r-1]; i++)
			{
				auxpr(mem[r+i], t+10);
				wprintf(L"\n");
			}
	}
	wprintf(L"\n");
}

int auxgt(int *r, int t)
{
	float rf;
	int j, flag=0;
	for (j=0; j<mem[(*r)-1]; j++)
		if (t == ROWOFINT)
			scanf(" %i", &mem[*r+j]);
		else if (t == ROWOFCHAR)
		{
			mem[*r+j] = 0;
			scanf("%c", &mem[*r+j]);
		}
		else if(t == ROWOFFLOAT)
		{
			scanf(" %f", &rf);
			mem[*r + j] = (int)rf;
			//memcpy(&mem[*r+j], &rf, sizeof(num));
		}
		else
			flag = 1;
	return flag;
}

void auxget(int *r, int t)
{
	float rf;
	if (t == LINT)
		scanf(" %i", r);
	else if (t == LCHAR)
	{
		r = 0;
		scanf("%c", r);
	}
	else if (t == LFLOAT)
	{
		scanf(" %f", &rf);
		r = (int)rf;
		//memcpy(&r, &rf, sizeof(int));
	}
	else
	{
		int i;
		if (auxgt(r, t))
			// двухмерные массивы
			for (i=0; i<mem[*r-1]; i++)
				auxgt(&mem[*r+i], t+10);
	}
}

void import()
{
	FILE *input;
	int i,r, n, flagstop = 1, entry, num;
	float lf, rf;
	char i2ccommand[I2CBUFFERSIZE];

#ifdef ROBOT
#ifdef WITHDIGITALSENSORS
	f1 = fopen(JD1, "r");                       // файлы цифровых датчиков
	f2 = fopen(JD2, "r");
#endif
	wprintf(L"stage 1\n");
	system("i2cset -y 2 0x48 0x10 0x1000 w");   // инициализация силовых моторов
	system("i2cset -y 2 0x48 0x11 0x1000 w");
	system("i2cset -y 2 0x48 0x12 0x1000 w");
	system("i2cset -y 2 0x48 0x13 0x1000 w");
#endif

	input = fopen("export.txt", "r");

	fscanf(input, "%i %i %i %i %i\n", &pc, &funcnum, &id, &rp, &maxdisplg);

	for (i=0; i<pc; i++)
		fscanf(input, "%i ", &mem[i]);

	for (i=0; i<funcnum; i++)
		fscanf(input, "%i ", &functions[i]);

	for (i=0; i<id; i++)
		fscanf(input, "%i ", &identab[i]);

	for (i=0; i<rp; i++)
		fscanf(input, "%i ", &reprtab[i]);

	fclose(input);

	l = g = pc;
	mem[g] = mem[g+1] = 0;
	x = g + maxdisplg;
	pc = 0;
	while (flagstop)
	{
		rf = (float)mem[x];
		//memcpy(&rf, &mem[x], sizeof(num));
//        wprintf(L"pc=%i mem[pc]=%i x=%i mem[x]=%i rf=%f\n", pc, mem[pc], x, mem[x], rf);
		switch (mem[pc++])
		{
			case STOP:
				flagstop = 0;
				break;

			case SETMOTOR:
				r = mem[x--];
				n = mem[x--];
				if (n < 1 || n > 4)
					runtimeerr(wrong_motor_num, n, 0);
				if (r < -100 || r > 100)
					runtimeerr(wrong_motor_pow, n, r);
				memset(i2ccommand, '\0', I2CBUFFERSIZE);
				wprintf(L"i2cset -y 2 0x48 0x%x 0x%x b\n", 0x14 + n - 1, r);
				snprintf(i2ccommand, I2CBUFFERSIZE, "i2cset -y 2 0x48 0x%x 0x%x b", 0x14 + n - 1, r);
				system(i2ccommand);
				break;
#ifdef WITHDIGITALSENSORS
			case GETDIGSENSOR:
				n = mem[x];
				if (n < 1 || n > 2)
					runtimeerr(wrong_digsensor_num, n, 0);
				if (n == 1)
					fscanf(f1, "%i", &i);
				else
					fscanf(f2, "%i", &i);
				mem[x] = i;
				break;
#endif
			case GETANSENSOR:
				n = mem[x];
				if (n < 1 || n > 6)
					runtimeerr(wrong_ansensor_num, n, 0);
				memset(i2ccommand, '\0', I2CBUFFERSIZE);
				wprintf(L"i2cget -y 2 0x48 0x%x\n", 0x26 - n);
				snprintf(i2ccommand, I2CBUFFERSIZE, "i2cget -y 2 0x48 0x%x", 0x26 - n);
				mem[x] = rungetcommand(i2ccommand);
				break;
			case SLEEP:
				sleep(mem[x--]);
				break;

			case FUNCBEG:
				pc = mem[pc+1];
				break;
			case PRINTID:
				printident(r = mem[pc++]);  // ссылка на reprtab
				i = mem[pc++];              // ссылка на identtab
				r = dspl(identab[i+3]);
				auxprint(mem[r],identab[i+2]);
				break;
			case PRINT:
				auxprint(mem[x--], mem[pc++]);
				break;
			case GETID:
				wprintf(L"\n");
				printident(r = mem[pc++]);
				i = mem[pc++];
				r = dspl(identab[i+3]);
				auxget(&mem[r], identab[i+2]);
				break;
			case ABSI:
				mem[x] = abs(mem[x]);
				break;
			case ABS:
				rf = fabs(rf);
				mem[x] = (int)rf;
				//memcpy(&mem[x], &rf, sizeof(num));
				break;
			case SQRT:
				rf = sqrtf(rf);
				mem[x] = (int)rf;
				//memcpy(&mem[x], &rf, sizeof(num));
				break;
			case EXP:
				rf = expf(rf);
				mem[x] = (int)rf;
				//memcpy(&mem[x], &rf, sizeof(num));
				break;
			case SIN:
				rf = sinf(rf);
				mem[x] = (int)rf;
				//memcpy(&mem[x], &rf, sizeof(num));
				break;
			case COS:
				rf = cosf(rf);
				mem[x] = (int)rf;
				//memcpy(&mem[x], &rf, sizeof(num));
				break;
			case LOG:
				rf = logf(rf);
				mem[x] = (int)rf;
				//memcpy(&mem[x], &rf, sizeof(num));
				break;
			case LOG10:
				rf = log10f(rf);
				mem[x] = (int)rf;
				//memcpy(&mem[x], &rf, sizeof(num));
				break;
			case ASIN:
				rf = asinf(rf);
				mem[x] = (int)rf;
				//memcpy(&mem[x], &rf, sizeof(num));
				break;

			case DECX:
				x--;
				break;
			case DEFARR:
				mem[dsp()] = x + 1;
				x += mem[x];
				break;
			case DEFARR2:
			{
				int oldx = x, m;
				int n = mem[x--], i;
				m = mem[x];
				mem[dsp()] = oldx;
				x += m;
				for (i=0; i<m; i++)
				{
					mem[++x] = n;
					mem[oldx+i] = x +1;
					x += n;
				}
				break;
			}
			case LI:
				mem[++x] = mem[pc++];
				break;
			case LOAD:
				mem[++x] = mem[dsp()];
				break;
			case LAT:
				mem[x] = mem[mem[x]];
				break;
			case ST:
				mem[dsp()] = mem[x--];
				break;
			case LA:
				mem[++x] = dsp();
				break;
			case CALL1:
				mem[l+1] = ++x;
				mem[x++] = l;
				mem[x++] = 0;    // следующая статика
				mem[x] = 0;      // pc в момент вызова
				break;
			case CALL2:
				i = mem[pc++];
				l = mem[l+1];
				entry = functions[i > 0 ? i : mem[l-i]];
				x = l + mem[entry+1] - 1;
				mem[l+2] = pc;
				pc = entry + 3;
				break;
			case RETURN:
				pc = mem[l+2];
				r = l;
				l = mem[l];
				mem[l+1] = 0;
				mem[r] = mem[x];
				x = r ;
				break;
			case RETURNV:
				pc = mem[l+2];
				x = l-1;
				l = mem[l];
				mem[l+1] = 0;
				break;
			case NOP:
				;
				break;
			case B:
			case STRING:
				pc = mem[pc];
				break;
			case BE0:
				pc = (mem[x--]) ?  pc + 1 : mem[pc];
				break;
			case BNE0:
				pc = (mem[x--]) ? mem[pc] : pc + 1;
				break;
			case SLICE:
				i = mem[x--];   // index
				r = mem[x];     // array
				if (i < 0 || i >= mem[r-1])
					runtimeerr(index_out_of_range, i, mem[r-1]);
				mem[x] = r + i;
				break;
			case ASSARR:
				r = mem[dsp()];
				n = mem[pc++];
				if (mem[r-1] != n)
					runtimeerr(wrong_arr_init, mem[r-1], n);
				r += n;
				for (i=n; i>0; i--)
					mem[--r] = mem[x--];
				break;
			case ASSARR2:
			{
				int nr, m, n, i, j, rr;
				r = mem[dsp()];
				m = mem[r-1];
				n = mem[mem[r]-1];
				nr = mem[pc++];
				if (m*n != nr)
					runtimeerr(wrong_arr_init, m*n, nr);
				r += m - 1;
				for (i=0; i<m; i++)
				{
					rr = mem[r] + n;
					for (j=n; j>0; j--)
						mem[--rr] = mem[x--];
					r--;
				}
				break;
			}
			case WIDEN:
				rf = (float)mem[x];
				mem[x] = (int)rf;
				//memcpy(&mem[x], &rf, sizeof(num));
				break;
			case WIDEN1:
				rf = (float)mem[x-1];
				mem[x - 1] = (int)rf;
				//memcpy(&mem[x-1], &rf, sizeof(num));
				break;
			case DOUBLE:
				r = mem[x];
				mem[++x] = r;
				break;
			case ASS:
				mem[dsp()] = mem[x];
				break;
			case REMASS:
				r = mem[dsp()] %= mem[x];
				mem[x] = r;
				break;
			case SHLASS:
				r = mem[dsp()] <<= mem[x];
				mem[x] = r;
				break;
			case SHRASS:
				r = mem[dsp()] >>= mem[x];
				mem[x] = r;
				break;
			case ANDASS:
				r = mem[dsp()] &= mem[x];
				mem[x] = r;
				break;
			case EXORASS:
				r = mem[dsp()] ^= mem[x];
				mem[x] = r;
				break;
			case ORASS:
				r = mem[dsp()] |= mem[x];
				mem[x] = r;
				break;
			case PLUSASS:
				r = mem[dsp()] += mem[x];
				mem[x] = r;
				break;
			case MINUSASS:
				r = mem[dsp()] -= mem[x];
				mem[x] = r;
				break;
			case MULTASS:
				r = mem[dsp()] *= mem[x];
				mem[x] = r;
				break;
			case DIVASS:
				r = mem[dsp()] /= mem[x];
				mem[x] = r;
				break;
			case ASSAT:
				r = mem[mem[x-1]] = mem[x];
				mem[--x] = r;
				break;
			case REMASSAT:
				r = mem[mem[x-1]] %= mem[x];
				mem[--x] = r;
				break;
			case SHLASSAT:
				r = mem[mem[x-1]] <<= mem[x];
				mem[--x] = r;
				break;
			case SHRASSAT:
				r = mem[mem[x-1]] >>= mem[x];
				mem[--x] = r;
				break;
			case ANDASSAT:
				r = mem[mem[x-1]] &= mem[x];
				mem[--x] = r;
				break;
			case EXORASSAT:
				r = mem[mem[x-1]] ^= mem[x];
				mem[--x] = r;
				break;
			case ORASSAT:
				r = mem[mem[x-1]] |= mem[x];
				mem[--x] = r;
				break;
			case PLUSASSAT:
				r = mem[mem[x-1]] += mem[x];
				mem[--x] = r;
				break;
			case MINUSASSAT:
				r = mem[mem[x-1]] -= mem[x];
				mem[--x] = r;
				break;
			case MULTASSAT:
				r = mem[mem[x-1]] *= mem[x];
				mem[--x] = r;
				break;
			case DIVASSAT:
				r = mem[mem[x-1]] /= mem[x];
				mem[--x] = r;
				break;
			case LOGOR:
				mem[x-1] = mem[x-1] || mem[x];
				x--;
				break;
			case LOGAND:
				mem[x-1] = mem[x-1] && mem[x];
				x--;
				break;
			case LOR:
				mem[x-1] |= mem[x];
				x--;
				break;
			case LEXOR:
				mem[x-1] ^= mem[x];
				x--;
				break;
			case LAND:
				mem[x-1] &= mem[x];
				x--;
				break;
			case LSHR:
				mem[x-1] >>= mem[x];
				x--;
				break;
			case LSHL:
				mem[x-1] <<= mem[x];
				x--;
				break;
			case LREM:
				mem[x-1] %= mem[x];
				x--;
				break;
			case EQEQ:
				mem[x-1] = mem[x-1] == mem[x];
				x--;
				break;
			case NOTEQ:
				mem[x-1] = mem[x-1] != mem[x];
				x--;
				break;
			case LLT:
				mem[x-1] = mem[x-1] < mem[x];
				x--;
				break;
			case LGT:
				mem[x-1] = mem[x-1] > mem[x];
				x--;
				break;
			case LLE:
				mem[x-1] = mem[x-1] <= mem[x];
				x--;
				break;
			case LGE:
				mem[x-1] = mem[x-1] >= mem[x];
				x--;
				break;
			case LPLUS:
				mem[x-1] += mem[x];
				x--;
				break;
			case LMINUS:
				mem[x-1] -= mem[x];
				x--;
				break;
			case LMULT:
				mem[x-1] *= mem[x];
				x--;
				break;
			case LDIV:
				mem[x-1] /= mem[x];
				x--;
				break;
			case POSTINC:
				mem[++x] = mem[r=dsp()];
				mem[r]++;
				break;
			case POSTDEC:
				mem[++x] = mem[r=dsp()];
				mem[r]--;
				break;
			case INC:
				mem[++x] = ++mem[dsp()];
				break;
			case DEC:
				mem[++x] = --mem[dsp()];
				break;
			case POSTINCAT:
				mem[x] = mem[r=mem[x]];
				mem[r]++;
				break;
			case POSTDECAT:
				mem[x] = mem[r=mem[x]];
				mem[r]--;
				break;
			case INCAT:
				mem[x] = ++mem[mem[x]];
				break;
			case DECAT:
				mem[x] = --mem[mem[x]];
				break;
			case UNMINUS:
				mem[x] = -mem[x];
				break;
			case PLUSASSR:
				lf = (float)mem[i = dsp()];
				//memcpy(&lf, &mem[i=dsp()], sizeof(num));
				lf += rf;
				mem[x] = (int)lf;
				mem[i] = mem[x];
				//memcpy(&mem[x], &lf, sizeof(num));
				//memcpy(&mem[i], &lf, sizeof(num));
				break;
			case MINUSASSR:
				lf = (float)mem[i = dsp()];
				//memcpy(&lf, &mem[i=dsp()], sizeof(num));
				lf -= rf;
				mem[x] = (int)lf;
				mem[i] = mem[x];
				//memcpy(&mem[x], &lf, sizeof(num));
				//memcpy(&mem[i], &lf, sizeof(num));
				break;
			case MULTASSR:
				lf = (float)mem[i = dsp()];
				//memcpy(&lf, &mem[i=dsp()], sizeof(num));
				lf *= rf;
				mem[x] = (int)lf;
				mem[i] = mem[x];
				//memcpy(&mem[x], &lf, sizeof(num));
				//memcpy(&mem[i], &lf, sizeof(num));
				break;
			case DIVASSR:
				lf = (float)mem[i = dsp()];
				//memcpy(&lf, &mem[i=dsp()], sizeof(num));
				lf /= rf;
				mem[x] = (int)lf;
				mem[i] = mem[x];
				//memcpy(&mem[x], &lf, sizeof(num));
				//memcpy(&mem[i], &lf, sizeof(num));
				break;
			case PLUSASSATR:
				rf = (float)mem[--x];
				lf = (float)mem[i = mem[x]];
				//memcpy(&rf, &mem[x--], sizeof(num));
				//memcpy(&lf, &mem[i=mem[x]], sizeof(num));
				lf += rf;
				mem[x] = (int)lf;
				mem[i] = mem[x];
				//memcpy(&mem[x], &lf, sizeof(num));
				//memcpy(&mem[i], &lf, sizeof(num));
				break;
			case MINUSASSATR:
				rf = (float)mem[--x];
				lf = (float)mem[i = mem[x]];
				//memcpy(&rf, &mem[x--], sizeof(num));
				//memcpy(&lf, &mem[i=mem[x]], sizeof(num));
				lf -= rf;
				mem[x] = (int)lf;
				mem[i] = mem[x];
				//memcpy(&mem[x], &lf, sizeof(num));
				//memcpy(&mem[i], &lf, sizeof(num));
				break;
			case MULTASSATR:
				rf = (float)mem[x--];
				lf = (float)mem[i = mem[x]];
				//memcpy(&rf, &mem[x--], sizeof(num));
				//memcpy(&lf, &mem[i=mem[x]], sizeof(num));
				lf *= rf;
				mem[x] = (int)lf;
				mem[i] = mem[x];
				//memcpy(&mem[x], &lf, sizeof(num));
				//memcpy(&mem[i], &lf, sizeof(num));
				break;
			case DIVASSATR:
				rf = (float)mem[x--];
				lf = (float)mem[i = mem[x]];
				//memcpy(&rf, &mem[x--], sizeof(num));
				//memcpy(&lf, &mem[i=mem[x]], sizeof(num));
				lf /= rf;
				mem[x] = (int)lf;
				mem[i] = mem[x];
				//memcpy(&mem[x], &lf, sizeof(num));
				//memcpy(&mem[i], &lf, sizeof(num));
				break;
			case EQEQR:
				lf = (float)mem[--x];
				//memcpy(&lf, &mem[--x], sizeof(num));
				mem[x] = lf == rf;
				break;
			case NOTEQR:
				lf = (float)mem[--x];
				//memcpy(&lf, &mem[--x], sizeof(num));
				mem[x] = lf != rf;
				break;
			case LLTR:
				lf = (float)mem[--x];
				//memcpy(&lf, &mem[--x], sizeof(num));
				mem[x] = lf < rf;
				break;
			case LGTR:
				lf = (float)mem[--x];
				//memcpy(&lf, &mem[--x], sizeof(num));
				mem[x] = lf > rf;
				break;
			case LLER:
				lf = (float)mem[--x];
				//memcpy(&lf, &mem[--x], sizeof(num));
				mem[x] = lf <= rf;
				break;
			case LGER:
				lf = (float)mem[--x];
				//memcpy(&lf, &mem[--x], sizeof(num));
				mem[x] = lf >= rf;
				break;
			case LPLUSR:
				lf = (float)mem[--x];
				//memcpy(&lf, &mem[--x], sizeof(num));
				lf += rf;
				mem[x] = (int)lf;
				//memcpy(&mem[x], &lf, sizeof(num));
				break;
			case LMINUSR:
				lf = (float)mem[--x];
				//memcpy(&lf, &mem[--x], sizeof(num));
				lf -= rf;
				mem[x] = (int)lf;
				//memcpy(&mem[x], &lf, sizeof(num));
				break;
			case LMULTR:
				lf = (float)mem[--x];
				//memcpy(&lf, &mem[--x], sizeof(num));
				lf *= rf;
				mem[x] = (int)lf;
				//memcpy(&mem[x], &lf, sizeof(num));
				break;
			case LDIVR:
				lf = (float)mem[--x];
				//memcpy(&lf, &mem[--x], sizeof(num));
				lf /= rf;
				mem[x] = (int)lf;
				//memcpy(&mem[x], &lf, sizeof(num));
				break;
			case POSTINCR:
				rf = (float)mem[i = dsp()];
				mem[++x] = (int)rf;
				//memcpy(&rf, &mem[i=dsp()], sizeof(num));
				//memcpy(&mem[++x], &rf, sizeof(num));
				++rf;
				mem[i] = (int)rf;
				//memcpy(&mem[i], &rf, sizeof(num));
				break;
			case POSTDECR:
				rf = (float)mem[i = dsp()];
				mem[++x] = (int)rf;
				//memcpy(&rf, &mem[i=dsp()], sizeof(num));
				//memcpy(&mem[++x], &rf, sizeof(num));
				--rf;
				mem[i] = (int)rf;
				//memcpy(&mem[i], &rf, sizeof(num));
				break;
			case INCR:
				rf = (float)mem[i = dsp()];
				//memcpy(&rf, &mem[i=dsp()], sizeof(num));
				++rf;
				mem[++x] = (int)rf;
				mem[i] = mem[x];
				//memcpy(&mem[++x], &rf, sizeof(num));
				//memcpy(&mem[i], &rf, sizeof(num));
				break;
			case DECR:
				rf = (float)mem[i = dsp()];
				//memcpy(&rf, &mem[i=dsp()], sizeof(num));
				--rf;
				mem[++x] = (int)rf;
				mem[i] = mem[x];
				//memcpy(&mem[++x], &rf, sizeof(num));
				//memcpy(&mem[i], &rf, sizeof(num));
				break;
			case POSTINCATR:
				rf = (float)mem[i = mem[x]];
				mem[++x] = (int)rf;
				//memcpy(&rf, &mem[i=mem[x]], sizeof(num));
				//memcpy(&mem[++x], &rf, sizeof(num));
				++rf;
				mem[i] = (int)rf;
				//memcpy(&mem[i], &rf, sizeof(num));
				break;
			case POSTDECATR:
				rf = (float)mem[i = mem[x]];
				mem[++x] = (int)rf;
				//memcpy(&rf, &mem[i=mem[x]], sizeof(num));
				//memcpy(&mem[++x], &rf, sizeof(num));
				--rf;
				mem[i] = (int)rf;
				//memcpy(&mem[i], &rf, sizeof(num));
				break;
			case INCATR:
				rf = (float)mem[i = mem[x]];
				//memcpy(&rf, &mem[i=mem[x]], sizeof(num));
				++rf;
				mem[++x] = (int)rf;
				mem[i] = mem[x];
				//memcpy(&mem[++x], &rf, sizeof(num));
				//memcpy(&mem[i], &rf, sizeof(num));
				break;
			case DECATR:
				rf = (float)mem[i = mem[x]];
				//memcpy(&rf, &mem[i=mem[x]], sizeof(num));
				--rf;
				mem[++x] = (int)rf;
				mem[i] = mem[x];
				//memcpy(&mem[++x], &rf, sizeof(num));
				//memcpy(&mem[i], &rf, sizeof(num));
				break;
			case UNMINUSR:
				rf = -rf;
				mem[x] = (int)rf;
				//memcpy(&mem[x], &rf, sizeof(num));
				break;
			case LNOT:
				mem[x] = ~ mem[x];
				break;
			case LOGNOT:
				mem[x] = ! mem[x];
				break;

			default:
				runtimeerr(wrong_kop, mem[pc-1], 0);
				break;
		}
	}

#ifdef ROBOT
	wprintf(L"111");
	system("i2cset -y 2 0x48 0x10 0 w");   // отключение силовых моторов
	system("i2cset -y 2 0x48 0x11 0 w");
	system("i2cset -y 2 0x48 0x12 0 w");
	system("i2cset -y 2 0x48 0x13 0 w");
#ifdef WITHDIGITALSENSORS
	fclose(f1);
	fclose(f2);
#endif
#endif


}
