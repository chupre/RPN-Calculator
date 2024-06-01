#define _CRT_SECURE_NO_WARNINGS
#define MAX_INPUT 10000
#define PRECISION 10000000
#pragma warning(disable : 5045)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>


typedef struct stack_object
{
	long double data;
	struct stack_object* next;
} stack;

typedef struct stack_object_ull
{
	unsigned long long data;
	struct stack_object_ull* next;
} stack_ull;

stack* push(stack* top, long double data);
stack* pop(stack* top);
stack_ull* push_ull(stack_ull* top, unsigned long long data);
stack_ull* pop_ull(stack_ull* top);
void parse_error(void);
bool is_sym_num(char sym);
bool is_sym_valid_char(char sym);
bool is_input_valid(char* input);
int get_priority(char op);
void get_rpn(char* rpn, char* input);
bool is_minus_unary(char* rpn, int index);
long double apply_operand(long double n1, long double n2, char operand);
unsigned long long pow_ull(unsigned long long base, unsigned long long exp);
unsigned long long apply_operand_ull(unsigned long long n1, unsigned long long n2, char operand);
bool is_data_certain(char* data);
void calculate(char* rpn, char* result);
void calculate_ull(char* rpn, char* result);

int main(int argc, char* argv[])
{
	char input[MAX_INPUT] = { 0 };
	char rpn[MAX_INPUT] = { 0 };

	if (argc == 1)
	{
		fgets(input, MAX_INPUT, stdin);
		input[strcspn(input, "\n")] = 0;
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			strcat(input, argv[i]);
		}
	}

	if (!is_input_valid(input))
	{
		parse_error();
	}

	get_rpn(rpn, input);

	char result[MAX_INPUT];

	calculate(rpn, result);
	printf("%s", result);

	return EXIT_SUCCESS;
}

stack* push(stack* top, long double data)
{
	stack* ptr = malloc(sizeof(stack));
	ptr->data = data;
	ptr->next = top;
	return ptr;
}


stack* pop(stack* top)
{
	if (top == NULL)
	{
		return top;
	}

	stack* ptr_next = top->next;
	free(top);

	return ptr_next;
}


stack_ull* push_ull(stack_ull* top, unsigned long long data)
{
	stack_ull* ptr = malloc(sizeof(stack_ull));
	ptr->data = data;
	ptr->next = top;
	return ptr;
}


stack_ull* pop_ull(stack_ull* top)
{
	if (top == NULL)
	{
		return top;
	}

	stack_ull* ptr_next = top->next;
	free(top);

	return ptr_next;
}


void parse_error(void)
{
	printf("Error parse");
	exit(EXIT_SUCCESS);
}


bool is_sym_num(char sym)
{
	if (sym > 47 && sym < 58)
	{
		return true;
	}

	return false;
}


bool is_sym_valid_char(char sym)
{
	if (sym == '+' || sym == '-' || sym == '/' || sym == '*' || sym == '^' || sym == '%' || sym == '(' || sym == ')' || sym == '.' || sym == '~')
	{
		return true;
	}

	return false;
}


bool is_input_valid(char* input)
{
	if (!input[0])
	{
		return false;
	}

	stack* parentheses_stack = NULL;
	int open = 0;

	for (int i = 0; input[i] != '\0'; i++)
	{
		if (input[i] == ' ')
		{
			continue;
		}

		if (is_sym_num(input[i]) || is_sym_valid_char(input[i]))
		{
			if (input[i] == '(')
			{
				parentheses_stack = push(parentheses_stack, '(');
				open++;
			}

			if (input[i] == ')')
			{
				if (open)
				{
					parentheses_stack = pop(parentheses_stack);
					open--;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}

	if (parentheses_stack == NULL)
	{
		return true;
	}

	return false;
}


int get_priority(char op)
{
	if (op == '+' || op == '-')
	{
		return 1;
	}

	if (op == '*' || op == '/' || op == '%')
	{
		return 2;
	}

	if (op == '^')
	{
		return 3;
	}

	return 0;
}


void get_rpn(char* rpn, char* input)
{
	stack* operator_stack = NULL;
	int rpn_counter = 0;
	bool unary_minus = false;

	for (int i = 0; input[i] != '\0'; i++)
	{
		if (input[i] == ' ')
		{
			continue;
		}

		if (unary_minus)
		{
			rpn[rpn_counter] = '~';
			rpn_counter++;
			unary_minus = false;
		}

		if (is_sym_num(input[i]))
		{
			while (is_sym_num(input[i]) || input[i] == '.')
			{
				rpn[rpn_counter] = input[i];
				rpn_counter++;
				i++;
			}

			rpn[rpn_counter++] = ' ';
			i--;
			continue;
		}

		if (is_sym_valid_char(input[i]))
		{
			if ((i == 0 || is_minus_unary(input, i)) && input[i] == '-')
			{
				unary_minus = true;
				continue;
			}

			unary_minus = false;

			if (operator_stack == NULL || input[i] == '(')
			{
				operator_stack = push(operator_stack, input[i]);
				continue;
			}

			if (input[i] == ')')
			{
				while (operator_stack->data != '(')
				{
					rpn[rpn_counter] = (char)operator_stack->data;
					rpn_counter++;
					operator_stack = pop(operator_stack);
				}

				operator_stack = pop(operator_stack);
				continue;
			}

			if (input[i] == '^' && operator_stack->data == '^')
			{
				operator_stack = push(operator_stack, input[i]);
				continue;
			}

			while (operator_stack != NULL && get_priority(input[i]) <= get_priority((char)operator_stack->data))
			{
				rpn[rpn_counter] = (char)operator_stack->data;
				rpn_counter++;
				operator_stack = pop(operator_stack);
			}

			operator_stack = push(operator_stack, input[i]);
		}
	}

	while (operator_stack)
	{
		if (operator_stack->data != '(' && operator_stack->data != ')')
		{
			rpn[rpn_counter] = (char)operator_stack->data;
			rpn_counter++;
		}

		operator_stack = pop(operator_stack);
	}
}


bool is_minus_unary(char* input, int index)
{
	bool ans = false;

	for (int i = index + 1; input[i] != '\0'; i++)
	{
		if (input[i] == ' ')
		{
			continue;
		}

		if (input[i] == ')')
		{
			ans = true;
			return ans;
		}
		else
		{
			ans = false;
			break;
		}
	}

	for (int i = index - 1; i >= 0; i--)
	{
		if (input[i] == ' ')
		{
			continue;
		}

		if (input[i] == '(')
		{
			ans = true;
			return true;
		}
		else
		{
			ans = false;
			return ans;
		}
	}

	return ans;
}


long double apply_operand(long double n1, long double n2, char operand)
{
	switch (operand)
	{
	case '+':
		return n1 + n2;
	case '-':
		return n1 - n2;
	case '*':
		return n1 * n2;
	case '/':
		if (n2 == 0)
		{
			exit(EXIT_SUCCESS);
		}

		return n1 / n2;
	case '%':
		return fmodl(n1, n2);
	case '^':
		return powl(n1, n2);

	}

	return 0;
}


unsigned long long pow_ull(unsigned long long base, unsigned long long exp)
{
	unsigned long long pow = 1;

	for (unsigned int i = 0; i < exp; i++)
	{
		pow = pow * base;
	}

	return pow;
}


unsigned long long apply_operand_ull(unsigned long long n1, unsigned long long n2, char operand)
{
	switch (operand)
	{
	case '+':
		return n1 + n2;
	case '-':
		return n1 - n2;
	case '*':
		return n1 * n2;
	case '/':
		if (n2 == 0)
		{
			exit(EXIT_SUCCESS);
		}

		return n1 / n2;
	case '%':
		return n1 % n2;
	case '^':
		return pow_ull(n1, n2);

	}

	return 0;
}


bool is_data_certain(char* data)
{
	double long data_dl = strtold(data, NULL);

	if (floorl(data_dl) == data_dl && (unsigned long long)data_dl != strtoull(data, NULL, 0))
	{
		return false;
	}

	return true;
}


void calculate_ull(char* rpn, char* result)
{
	stack_ull* result_num_ull = NULL;
	char num[MAX_INPUT] = { 0 };
	int digit_counter = 0;
	unsigned long long n1, n2;

	for (int i = 0; rpn[i] != '\0'; i++)
	{
		if (is_sym_num(rpn[i]))
		{
			num[digit_counter] = rpn[i];
			digit_counter++;
			continue;
		}

		if (rpn[i] == ' ')
		{
			result_num_ull = push_ull(result_num_ull, strtoull(num, NULL, 0));
			digit_counter = 0;
			memset(num, 0, MAX_INPUT);
			continue;
		}

		if (rpn[i] == '~')
		{
			continue;
		}

		n1 = result_num_ull->data;
		result_num_ull = pop_ull(result_num_ull);
		n2 = result_num_ull->data;
		result_num_ull = pop_ull(result_num_ull);
		result_num_ull = push_ull(result_num_ull, apply_operand_ull(n2, n1, rpn[i]));
	}

	if (result_num_ull)
	{
		sprintf(result, "%llu", result_num_ull->data);
	}
	else
	{
		parse_error();
	}
}


void calculate(char* rpn, char* result)
{
	stack* result_num = NULL;
	char num[MAX_INPUT] = { 0 };
	int digit_counter = 0;
	int uminus_counter = 0;
	long double n1, n2;

	for (int i = 0; rpn[i] != '\0'; i++)
	{
		if (is_sym_num(rpn[i]) || rpn[i] == '.')
		{
			num[digit_counter] = rpn[i];
			digit_counter++;
			continue;
		}

		if (rpn[i] == ' ')
		{
			if (is_data_certain(num))
			{
				result_num = push(result_num, strtold(num, NULL) * powl(-1, uminus_counter));
			}
			else
			{
				calculate_ull(rpn, result);
				return;
			}

			digit_counter = 0;
			uminus_counter = 0;
			memset(num, 0, MAX_INPUT);
			continue;
		}

		if (rpn[i] == '~')
		{
			uminus_counter++;
			continue;
		}

		n1 = result_num->data;
		result_num = pop(result_num);
		n2 = result_num->data;
		result_num = pop(result_num);
		result_num = push(result_num, apply_operand(n2, n1, rpn[i]));
	}

	if (result_num)
	{
		if (floorl(result_num->data) == result_num->data)
		{
			sprintf(result, "%.0Lf", result_num->data);
		}
		else
		{
			result_num->data = floorl(result_num->data * PRECISION) / PRECISION;
			sprintf(result, "%.7Lf", result_num->data);
		}
	}
	else
	{
		parse_error();
	}
}
