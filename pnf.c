#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <math.h>

#define MAX_SIZE 50

const char operations[] = "!*+@=^|#"; 						//символы операций, использующиеся для ввода с клавиатуры
const int prior[] = {120, 100, 80, 60, 40, 20, 20, 20};		//приоритеты операций
const int ch_prior = 140;									//приоритет скобок
int priorities[MAX_SIZE];									//храниение приоритетов формулы
char vars[10];												//строка с переменными
int num_vars;												//кол-во переменных (длина строки vars)

/*
*Проверяет введенные данные
*/
int check(char const *str);

/*
*Подсчитывает кол-во переменных и записывает их в строку vars
*/
void countVar(char const *str);

/*
*Устанавливает приоритеты операций
*/
void setPriorities(char const *str);

/*
*Рекурсивно получаем результат от набора
*/
int result(char const *task, int set);

/*
*Печать таблицы истинности
*/
void printTable(char const *task);

/*
*Печать СКНФ
*/
void printSKNF(char const *task);

/*
*Печать СДНФ
*/
void printSDNF(char const *task);

/*
*Ввод данных
*/
void input(char in[MAX_SIZE]);

int main() 
{
	char task[MAX_SIZE];
	int run = 1;
	input(task);
	while (run)
	{
		printf("\nSelect action:\n1: Enter new formula\n2: Truth table\n3: SKNF\n4: SDNF\n5: All information\n6: Exit\n");
		int action;
		while (scanf("%d", &action) != 1 || action < 1 || action > 6)
		{
			printf("Command not found, please try again\n");
			while (getchar() != '\n');
		}
		switch (action)
		{
			case 1:
				input(task);
			break;
			case 2:
				printTable(task);
			break;
			case 3:
				printSKNF(task);
			break;
			case 4:
				printSDNF(task);
			break;
			case 5:
				printTable(task);
				printSKNF(task);
				printSDNF(task);
			break;
			case 6:
				run = 0;
			break;
		}
	}
	return 0;
}

int check(char const *str) 
{
	int state = 1;
	while (*str && state != 0) 
	{
		char c = *str;
		switch (state) 
		{
			case 1:
				if (isalpha(c)) 
				{
					state = 2;
				} 
				else 
				{
					if (c == '!' || c == '(')
					{
						state = 1;
					}
					else
					{
						state = 0;
					}
				}	
			break;
			case 2:
				if (strchr(operations, c) && c != '!')
				{
					state = 1;
				}
				else
				{
					if (c == ')') 
					{
						state = 2;
					}
					else
					{
						state = 0;
					}
				}
			break;
			default: 
				printf("\nImposible state");
			break;
		}
		str++;
	}
	return state;
}

void countVar(char const *str)
{
	while (*str)
	{
		char c = *str;
		if (isalpha(c) && !strchr(vars, c))
		{
			vars[strlen(vars)] = c;
		}
		str++;
	}
	num_vars = strlen(vars);
	for (int i = 0; i < num_vars - 1; i++)
	{
		for (int j = i + 1; j < num_vars; j++)
		{
			if (vars[i] > vars[j])
			{
				char tmp = vars[i];
				vars[i] = vars[j];
				vars[j] = tmp;
			}
		}
	}
}

void setPriorities(char const *str)
{
	int cur_prior = 0;
	for (int i = 0; str[i]; i++)
	{
		char c = str[i];
		if (c == '(') cur_prior += ch_prior;
		if (c == ')') cur_prior -= ch_prior;
		if (strchr(operations, c))
		{
			int pr = cur_prior + prior[strchr(operations, c) - operations];
            for (int j = 0; j < MAX_SIZE; j++)
            {
                if (priorities[j] == pr) pr--;
            }
            priorities[i] = pr;
			//priorities[i] = cur_prior + prior[strchr(operations, c) - operations];
		}
	}
}
 
int getIndOfOper(int l, int r)
{
    int min = 0;
    int index;
    for (int i = l; i < r; i++)
    {
        if (min == 0 || priorities[i] > 0 && priorities[i] < min)
        {
            min = priorities[i];
            index = i;
        }
    }
    if (min == 0) return -1;
    return index;
}

int res(char const *task, int l, int r, int set)
{
    int index = getIndOfOper(l, r);
    if (index == -1)
    {
        char tmp[MAX_SIZE];
        strncpy(tmp, task + l, r - l);
        return (set >> (num_vars - 1 - (int) (strchr(vars, *strpbrk(tmp, vars)) - vars))) % 2;
    }
    switch (task[index])
    {
        case '!':
            return !res(task, index + 1, r, set);
        break;
        case '*':
            return res(task, l, index, set) && res(task, index + 1, r, set);
        break;
        case '+':
            return res(task, l, index, set) || res(task, index + 1, r, set);
        break;
        case '@':
            return !res(task, l, index, set) || res(task, index + 1, r, set);
        break;
        case '=':
            return res(task, l, index, set) == res(task, index + 1, r, set);
        break;
        case '^':
            return res(task, l, index, set) != res(task, index + 1, r, set);
        break;
        case '|':
            return !(res(task, l, index, set) && res(task, index + 1, r, set));
        break;
        case '#':
            return !(res(task, l, index, set) || res(task, index + 1, r, set));
        break;
        default:
            printf("\nError calculate");
        break;
    }
}

void calculTable(char const *task, int **tbl, int *size)
{
	countVar(task);
	setPriorities(task);
	
	*size = pow(2, num_vars);
	*tbl = (int*) malloc(*size * sizeof(int));
	
	for (int i = 0; i < *size; i++)
	{
		(*tbl)[i] = result(task, i);
	}
}

void printTable(char const *task)
{
	int size;
	int *table;
	calculTable(task, &table, &size);
	
	printf("\n  Table:\n%s | %s\n", vars, task);
	for (int i = 0; i < size; i++)
	{
		for (int j = num_vars - 1; j >= 0; j--)
		{
			printf("%c", (i >> j) % 2 ? '1' : '0');
		}
		printf(" |\t%d\n", table[i]);
	}
}

void printSKNF(char const *task)
{
	int sknf = 0;
	int size;
	int *table;
	calculTable(task, &table, &size);
		
	for (int i = 0; i < size; i++)
	{
		sknf += !table[i];
	}
		
	printf("\nSKNF:\n");
	if (sknf)
	{
		for (int i = 0; i < size; i++)
		{
			if (!table[i])
			{				
				sknf--;
				printf("(");
				for (int j = 0; j < num_vars; j++)
				{
					if ((i >> num_vars - 1 - j) % 2)
					{
						printf("!");
					}
					printf("%c", vars[j]);
					if (j < num_vars - 1)
					{
						printf("%c", operations[2]);
					}
				}
				printf(")");
				if (sknf) printf("*");
			}
		}
		printf("\n");
	}	
	else 
	{
		printf("Doesn't exist\n");
	}
}

void printSDNF(char const *task)
{
	int sdnf = 0;
	int size;
	int *table;
	calculTable(task, &table, &size);
		
	for (int i = 0; i < size; i++)
	{
		sdnf += table[i];
	}
	
	printf("\nSDNF:\n");
	if (sdnf)
	{
		for (int i = 0; i < size; i++)
		{
			if (table[i])
			{				
				sdnf--;
				printf("(");
				for (int j = 0; j < num_vars; j++)
				{
					if ((i >> num_vars - 1 - j) % 2 == 0)
					{
						printf("!");
					}
					printf("%c", vars[j]);
					if (j < num_vars - 1)
					{
						printf("%c", operations[1]);
					}
				}
				printf(")");
				if (sdnf) printf("+");
			}
		}
		printf("\n");
	}
	else 
	{
		printf("Doesn't exist\n");
	}
}

void clear()
{
	num_vars = 0;
	for (int i = 0; i < 10; i++)
	{
		vars[i] = 0;
	}
	for (int i = 0; i < MAX_SIZE; i++)
	{
		priorities[i] = 0;	
	}
}

void input(char in[MAX_SIZE])
{
	clear();
	printf("Enter the formula:\n");
	scanf("%s", in);
	while (!check(in))
	{
		printf("Incorrect input, please try again\n");
		scanf("%s", in);
	}
}

int result(char const *task, int set)
{
	res(task, 0, strlen(task), set);
}
