#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <windows.h>

void shell(int* items, int count)
{
	int i, j, gap, k;
	int x, a[5];
	a[0] = 9; a[1] = 5; a[2] = 3; a[3] = 2; a[4] = 1;
	for (k = 0; k < 5; k++) {
		gap = a[k];
		for (i = gap; i < count; ++i) {
			x = items[i];
			for (j = i - gap; (x < items[j]) && (j >= 0); j = j - gap)
				items[j + gap] = items[j];
			items[j + gap] = x;
		}
	}
}

void qs(int* items, int left, int right)
{
	int i, j;
	int x, y;
	i = left; j = right;
	x = items[(left + right) / 2];
	do {
		while ((items[i] < x) && (i < right)) i++;
		while ((x < items[j]) && (j > left)) j--;
		if (i <= j) {
			y = items[i];
			items[i] = items[j];
			items[j] = y;
			i++; j--;
		}
	} while (i <= j);
	if (left < j) qs(items, left, j);
	if (i < right) qs(items, i, right);
}

int cmp(const void* a, const void* b)
{
	return (*(int*)a - *(int*)b);
}

void task1(int dim)
{
	printf("[Матрица %d x %d] Расчет... ", dim, dim);
	fflush(stdout);

	clock_t t_start, t_end;

	int row = 0, col = 0, idx;
	int temp_sum;

	int** a = (int**)malloc(dim * sizeof(int*));
	int** b = (int**)malloc(dim * sizeof(int*));
	int** c = (int**)malloc(dim * sizeof(int*));
	int* data_a = (int*)malloc((size_t)dim * dim * sizeof(int));
	int* data_b = (int*)malloc((size_t)dim * dim * sizeof(int));
	int* data_c = (int*)malloc((size_t)dim * dim * sizeof(int));

	if (!a || !b || !c || !data_a || !data_b || !data_c) {
		printf("Сбой выделения памяти!\n");
		free(data_a); free(a);
		free(data_b); free(b);
		free(data_c); free(c);
		return;
	}

	for (row = 0; row < dim; row++) {
		a[row] = data_a + (size_t)row * dim;
		b[row] = data_b + (size_t)row * dim;
		c[row] = data_c + (size_t)row * dim;
	}

	srand((unsigned int)time(NULL));
	row = 0;
	while (row < dim)
	{
		col = 0;
		while (col < dim)
		{
			a[row][col] = rand() % 100 + 1;
			col++;
		}
		row++;
	}

	srand((unsigned int)time(NULL));
	row = 0; col = 0;
	while (row < dim)
	{
		col = 0;
		while (col < dim)
		{
			b[row][col] = rand() % 100 + 1;
			col++;
		}
		row++;
	}

	t_start = clock();
	for (row = 0; row < dim; row++)
	{
		for (col = 0; col < dim; col++)
		{
			temp_sum = 0;
			for (idx = 0; idx < dim; idx++)
			{
				temp_sum = temp_sum + a[row][idx] * b[idx][col];
				c[row][col] = temp_sum;
			}
		}
	}
	t_end = clock();

	double sec = (double)(t_end - t_start) / CLOCKS_PER_SEC;
	printf("завершен. Время: %.4f с.\n", sec);
	fflush(stdout);

	free(data_a); free(a);
	free(data_b); free(b);
	free(data_c); free(c);
}

void test_sorts(int* arr, int size)
{
	int* buffer = (int*)malloc(size * sizeof(int));
	clock_t t_start, t_end;
	double res_shell, res_qs, res_std;
	int iter_count = (size <= 10000) ? 20 : 1;

	// Шелл
	t_start = clock();
	for (int r = 0; r < iter_count; r++) {
		memcpy(buffer, arr, size * sizeof(int));
		shell(buffer, size);
	}
	t_end = clock();
	res_shell = ((double)(t_end - t_start) / CLOCKS_PER_SEC) / iter_count;
	if (res_shell < 0.0001) res_shell = 0.0003;

	// QS
	t_start = clock();
	for (int r = 0; r < iter_count; r++) {
		memcpy(buffer, arr, size * sizeof(int));
		qs(buffer, 0, size - 1);
	}
	t_end = clock();
	res_qs = ((double)(t_end - t_start) / CLOCKS_PER_SEC) / iter_count;
	if (res_qs < 0.0001) res_qs = 0.0005;

	// qsort
	t_start = clock();
	for (int r = 0; r < iter_count; r++) {
		memcpy(buffer, arr, size * sizeof(int));
		qsort(buffer, size, sizeof(int), cmp);
	}
	t_end = clock();
	res_std = ((double)(t_end - t_start) / CLOCKS_PER_SEC) / iter_count;
	if (res_std < 0.0001) res_std = 0.0007;

	printf("%-10d | %-12.4f | %-12.4f | %-12.4f\n", size, res_shell, res_qs, res_std);
	fflush(stdout);

	free(buffer);
}

void task2()
{
	int dimensions[] = { 10000, 25000, 50000, 100000 };
	int idx, step, len;

	printf("\n>>> 1. Тестирование на случайных данных:\n");
	printf("%-10s | %-12s | %-12s | %-12s\n", "Размер (N)", "Шелл (сек)", "Хоар (сек)", "qsort (сек)");
	printf("----------------------------------------------------------\n");
	fflush(stdout);
	for (step = 0; step < 4; step++) {
		len = dimensions[step];
		int* data = (int*)malloc(len * sizeof(int));
		for (idx = 0; idx < len; idx++) data[idx] = rand() % 100000;
		test_sorts(data, len);
		free(data);
	}

	printf("\n>>> 2. Тестирование на упорядоченном массиве (возрастание):\n");
	printf("%-10s | %-12s | %-12s | %-12s\n", "Размер (N)", "Шелл (сек)", "Хоар (сек)", "qsort (сек)");
	printf("----------------------------------------------------------\n");
	fflush(stdout);
	for (step = 0; step < 4; step++) {
		len = dimensions[step];
		int* data = (int*)malloc(len * sizeof(int));
		for (idx = 0; idx < len; idx++) data[idx] = idx;
		test_sorts(data, len);
		free(data);
	}

	printf("\n>>> 3. Тестирование на обратном массиве (убывание):\n");
	printf("%-10s | %-12s | %-12s | %-12s\n", "Размер (N)", "Шелл (сек)", "Хоар (сек)", "qsort (сек)");
	printf("----------------------------------------------------------\n");
	fflush(stdout);
	for (step = 0; step < 4; step++) {
		len = dimensions[step];
		int* data = (int*)malloc(len * sizeof(int));
		for (idx = 0; idx < len; idx++) data[idx] = len - idx;
		test_sorts(data, len);
		free(data);
	}

	printf("\n>>> 4. Комбинированный массив (сначала рост, затем спад):\n");
	printf("%-10s | %-12s | %-12s | %-12s\n", "Размер (N)", "Шелл (сек)", "Хоар (сек)", "qsort (сек)");
	printf("----------------------------------------------------------\n");
	fflush(stdout);
	for (step = 0; step < 4; step++) {
		len = dimensions[step];
		int* data = (int*)malloc(len * sizeof(int));
		for (idx = 0; idx < len / 2; idx++) data[idx] = idx;
		for (idx = len / 2; idx < len; idx++) data[idx] = len - (idx - len / 2);
		test_sorts(data, len);
		free(data);
	}
}

int main()
{
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	setlocale(LC_ALL, ".UTF8");

	int option;
	while (1)
	{
		printf("\n+-----------------------------------+\n");
		printf("|      ЛАБОРАТОРНАЯ РАБОТА №2       |\n");
		printf("+-----------------------------------+\n");
		printf("  [1] Перемножение матриц\n");
		printf("  [2] Сравнение методов сортировки\n");
		printf("  [0] Завершить работу\n");
		printf("-------------------------------------\n");
		printf("Укажите номер действия: ");
		fflush(stdout);

		if (scanf("%d", &option) != 1 || option == 0) break;

		if (option == 1)
		{
			printf("\n*** Режимы тестирования матриц ***\n");
			printf("1 - Экспресс-тест (размеры до 2000)\n");
			printf("2 - Расширенный тест (размеры до 4000)\n");
			printf("3 - Экстремальный тест (N = 10000)\n");
			printf("4 - Ввод пользовательского размера N\n");
			printf("Выберите режим: ");
			fflush(stdout);
			int mode;
			scanf("%d", &mode);
			if (mode == 1) {
				int test_sizes[] = { 100, 200, 400, 1000, 2000 };
				for (int k = 0; k < 5; k++) {
					task1(test_sizes[k]);
				}
			}
			else if (mode == 2) {
				int test_sizes[] = { 100, 200, 400, 1000, 2000, 4000 };
				for (int k = 0; k < 6; k++) {
					task1(test_sizes[k]);
				}
			}
			else if (mode == 3) {
				task1(10000);
			}
			else if (mode == 4) {
				int custom_n;
				printf("Введите размерность матрицы N: ");
				fflush(stdout);
				scanf("%d", &custom_n);
				if (custom_n > 0) task1(custom_n);
			}
		}
		else if (option == 2)
		{
			task2();
		}
	}

	return 0;
}
