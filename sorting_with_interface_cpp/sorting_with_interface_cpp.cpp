#include <windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

#define IDC_UNPUT_ARRAY_EDIT 1001
#define IDC_SORT_TYPE_COMBO 1002
#define IDC_SORT_BUTTON 1003
#define IDC_SHOW_STEPS_BUTTON 1004
#define IDC_SHOW_TIME_BUTTON 1005
#define IDC_VIEW_ARRAY_BUTTON 1006

using namespace std; // пространство имен std

// ISort - интерфейс сортировки массива
__interface ISort
{
	void sort(vector<int>&, vector<vector<int>>&); // сортировка
	wstring get_sort_name(); // возвращает название сортировки
};

// ISortManager - интерфейс менеджера сортировок
__interface ISortManager 
{
	void sort_array(const wstring&, int); // сортировка
	vector<ISort*> get_sort_algorithms(); // возвращает список сортировок
	vector<vector<int>> get_steps(); // возвращает ходы сортировки
	vector<int> get_sorted_array(); // возвращает отсортированный массив
	double get_last_sort_time(); // возвращает время сортировки
};

// 
__interface IWindow 
{
	void show(HWND, void*); // отображает окно
};

//Реализация сортировок

//Сортировка пузырьком
class BubbleSort : public ISort 
{
public:
	void sort(vector<int>& arr, vector<vector<int>>& steps) 
	{
		steps.clear(); // очистка ходов
		steps.push_back(arr); // первый ход
		for (size_t i = 0; i < arr.size(); ++i) // перебор элементов
		{
			for (size_t j = 0; j < arr.size() - i - 1; ++j)  // перебор элементов в массиве
			{
				if (arr[j] > arr[j + 1]) // сравнение соседних элементов
				{
					swap(arr[j], arr[j + 1]); // меняем местами
					steps.push_back(arr); // добавляем ход
				}
			}
		}
	}
	wstring get_sort_name() { return L"Сортировка пузырьком"; } // возвращает название сортировки
};

//Быстрая сортировка 
class QuickSort : public ISort 
{
public:
	void sort(vector<int>& arr, vector<vector<int>>& steps)  // сортировка
	{
		steps.clear(); // очистка ходов
		quick_sort(arr, 0, arr.size() - 1, steps); // сортировка
	}
	wstring get_sort_name() { return L"Быстрая сортировка"; } // возвращает название сортировки

private:
	void quick_sort(vector<int>& arr, int left, int right, vector<vector<int>>& steps) 
	{
		if (left < right) // если левая граница меньше правой
		{
			int pivot = partition(arr, left, right); // разделение
			steps.push_back(arr); // добавление хода
			quick_sort(arr, left, pivot - 1, steps); // сортировка левой части
			quick_sort(arr, pivot + 1, right, steps); // сортировка правой части
		}
	}
	int partition(vector<int>& arr, int left, int right) // разделение
	{
		int pivot = arr[right]; // опорный элемент
		int i = left - 1; // левая граница
		for (int j = left; j < right; ++j) // перебор элементов
		{
			if (arr[j] < pivot) // если элемент меньше опорного
			{
				++i; // увеличиваем левую границу
				swap(arr[i], arr[j]); // меняем местами
			}
		}
		swap(arr[i + 1], arr[right]); // меняем местами
		return i + 1; // возвращаем опорный элемент
	}
};

class SelectionSoerr : public ISort 
{
public:
	void sort(vector<int>& arr, vector<vector<int>>& steps)
	{
		steps.clear(); // очистка ходов
		steps.push_back(arr); // первый ход
		for (size_t i = 0; i < arr.size(); ++i)
		{
			size_t min_index = i; // индекс минимального элемента
			for (size_t j = i + 1; j < arr.size(); ++j) 
			{
				if (arr[j] < arr[min_index]) 
				{
					min_index = j; // обновление индекса минимального элемента
				}
			}
			if (min_index != i) 
			{
				swap(arr[i], arr[min_index]); // меняем местами
				steps.push_back(arr); // добавляем ход
			}
		}
	}
	wstring get_sort_name() { return L"Сортировка выбором"; } // возвращает название сортировки
};

// Сортировка вставками
class InsertionSort : public ISort
{
public:
	void sort(vector<int>& arr, vector<vector<int>>& steps)
	{
		steps.clear(); // очистка ходов
		steps.push_back(arr); // первый ход
		for (size_t i = 1; i < arr.size(); ++i)
		{
			int key = arr[i]; // ключевой элемент
			int j = i - 1; // индекс предыдущего элемента
			while (j >= 0 && arr[j] > key)
			{
				arr[j + 1] = arr[j]; // сдвигаем элемент
				--j; // уменьшаем индекс
			}
			arr[j + 1] = key; // вставляем элемент
			steps.push_back(arr); // добавляем ход
		}
	}
	wstring get_sort_name() { return L"Сортировка вставками"; } // возвращает название сортировки
};

// Сортировка слиянием
class MergeSort : public ISort
{
public:
	void sort(vector<int>& arr, vector<vector<int>>& steps)  // сортировка
	{
		steps.clear(); // очистка ходов
		merge_sort(arr, 0, arr.size() - 1, steps); // сортировка
	}
	wstring get_sort_name() { return L"Сортировка слиянием"; } // возвращает название сортировки

private:
	void merge_sort(vector<int>& arr, int left, int right, vector<vector<int>>& steps) // сортировка
	{
		if (left < right) 
		{
			int middle = left + (right - left) / 2; // середина
			merge_sort(arr, left, middle, steps); // сортировка левой части
			merge_sort(arr, middle + 1, right, steps); // сортировка правой части
			merge(arr, left, middle, right, steps); // слияние
		}
	}
	void merge(vector<int>& arr, int left, int middle, int right, vector<vector<int>>& steps) 
	{
		int n1 = middle - left + 1; // размер левой части
		int n2 = right - middle; // размер правой части
		vector<int> L(n1), R(n2); // временные массивы

		for (int i = 0; i < n1; ++i)
		{
			L[i] = arr[left + i]; // копирование левой части
		}
		for (int i = 0; i < n2; ++i)
		{
			R[i] = arr[middle + 1 + i]; // копирование правой части
		}

		int i = 0, j = 0, k = left; // индексы
		while (i < n1 && j < n2) // пока не кончится один из массивов
		{
			if (L[i] <= R[j]) // если элемент из левого массива меньше или равен элементу из правого
			{
				arr[k] = L[i]; // копируем элемент из левого массива
				++i; // увеличиваем индекс левого массива
			}
			else
			{
				arr[k] = R[j]; // копируем элемент из правого массива
				++j; // увеличиваем индекс правого массива
			}
			++k; // увеличиваем индекс
		}
		while (j < n2) 
		{
			arr[k] = R[j]; // копируем оставшиеся элементы из правого массива
			++j; // увеличиваем индекс
			++k; // увеличиваем индекс
		}
		steps.push_back(arr); // добавляем ход
	}
};

//Вспомогательные функции
vector<int> parse_array(const wstring& input) 
{
	vector<int> arr; // массив
	wstringstream ss(input); // строковый поток
	int num; // число
	while (ss >> num) // пока есть числа
	{
		arr.push_back(num); // добавляем число в массив
	}
	return arr; // возвращаем массив
}

//Функция для отображения массива
class SortManager : public ISortManager
{
private:
	vector<ISort*> sort_algorithms; // список сортировок
	vector<vector<int>> steps; // ходы сортировки
	vector<int> sorted_array; // отсортированный массив
	vector<int> original_array; // исходный массив
	double last_sort_time; // время сортировки

public:
	SortManager() : last_sort_time(0) // конструктор
	{
		sort_algorithms.push_back(new BubbleSort()); // добавление сортировки пузырьком
		sort_algorithms.push_back(new QuickSort()); // добавление быстрой сортировки
		sort_algorithms.push_back(new SelectionSoerr()); // добавление сортировки выбором
		sort_algorithms.push_back(new InsertionSort()); // добавление сортировки вставками
		sort_algorithms.push_back(new MergeSort()); // добавление сортировки слиянием
	}

	~SortManager() // деструктор
	{
		for (auto sort_algo : sort_algorithms) // перебор сортировок
		{
			delete sort_algo; // удаление сортировки
		}
	}
	// сортировка массива
	void sort_array(const wstring& input, int sort_index)
	{
		original_array = parse_array(input); // парсинг массива
		sorted_array = original_array; // копирование массива
		steps.clear(); // очистка ходов
		auto start = chrono::high_resolution_clock::now(); // начало отсчета времени
		sort_algorithms[sort_index]->sort(sorted_array, steps); // сортировка
		auto end = chrono::high_resolution_clock::now(); // конец отсчета времени
		chrono::duration<double> diff = end - start; // разница времени
		last_sort_time = diff.count(); // время сортировки
	}

};
