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

using namespace std;

// ISort - интерфейс сортировки массива
__interface ISort
{
	void sort(vector<int>&, vector<vector<int>>&); // сортировка
	wstring get_sort_name(); 
};

// ISortManager - интерфейс менеджера сортировок
__interface ISortManager 
{
	void sort_array(const wstring&, int); // сортировка
	vector<ISort*> get_sort_algorithms(); // возвращает список сортировок
	vector<vector<int>> get_steps(); // возвращает ходы сортировки
	vector<int> get_sorted_array(); // возвращает отсортированный массив
	double get_last_sort_time(); 
};

// 
__interface IWindow 
{
	void show(HWND, void*);
};

//Реализация сортировок
class BubbleSort : public ISort 
{
public:
	void sort(vector<int>& arr, vector<vector<int>>& steps) 
	{
		steps.clear(); // очистка ходов
		steps.push_back(arr); // первый ход
		for (size_t i = 0; i < arr.size(); ++i) // 
		{
			for (size_t j = 0; j < arr.size() - i - 1; ++j)  //
			{
				if (arr[j] > arr[j + 1]) // 
				{
					swap(arr[j], arr[j + 1]); //
					steps.push_back(arr); //
				}
			}
		}
	}
	wstring get_sort_name() { return L"Сортировка пузырьком"; }
};

class QuickSort : public ISort 
{
public:
	void sort(vector<int>& arr, vector<vector<int>>& steps) 
	{
		steps.clear();
		quick_sort(arr, 0, arr.size() - 1, steps);
		wstring get_sort_name() { return L"Быстрая сортировка"; } 
	}
private:
	void quick_sort(vector<int>& arr, int left, int right, vector<vector<int>>& steps) 
	{
		if (left < right) 
		{
			int pivot = partition(arr, left, right);
			steps.push_back(arr);
			quick_sort(arr, left, pivot - 1, steps);
			quick_sort(arr, pivot + 1, right, steps);
		}
	}
	int partition(vector<int>& arr, int left, int right) 
	{
		int pivot = arr[right];
		int i = left - 1; 
		for (int j = left; j < right; ++j) 
		{
			if (arr[j] < pivot) {
				++i;
				swap(arr[i], arr[j]);
			}
		}
		swap(arr[i + 1], arr[right]);
		return i + 1;
	}
};

class SelectionSoerr : public ISort 
{

};