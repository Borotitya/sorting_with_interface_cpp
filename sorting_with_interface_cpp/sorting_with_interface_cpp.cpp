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

#define IDC_INPUT_ARRAY_EDIT 1001
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
	vector<ISort*> get_sort_algorithms() { return sort_algorithms; } // возвращает список сортировок
	vector<vector<int>> get_steps() { return steps; } // возвращает ходы сортировки
	vector<int> get_sorted_array() { return sorted_array; } // возвращает отсортированный массив
	double get_last_sort_time() { return last_sort_time; } // возвращает время сортировки
};

// Окно сортировки массива 
class StepWindow : public IWindow 
{
public:
	void show(HWND parent, void* data) 
	{
		ISortManager* manager = static_cast<ISortManager*>(data); // менеджер сортировок

		WNDCLASS wc = { 0 }; // класс окна
		wc.lpfnWndProc = StepWindowProc; // оконная процедура
		wc.hInstance = GetModuleHandle(NULL); // дескриптор приложения
		wc.lpszClassName = L"StepWindowClass"; // имя класса
		RegisterClass(&wc); // регистрация класса

		HWND hwnd = CreateWindowW(L"StepWindowClass", L"Этапы сортировки", WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_THICKFRAME,
			CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, parent, NULL, GetModuleHandle(NULL), manager); // создание окна

		ShowWindow(hwnd, SW_SHOW); // отображение окна
		UpdateWindow(hwnd); // обновление окна
	}

	// оконная процедура
	static LRESULT CALLBACK StepWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
	{
		static ISortManager* manager = nullptr; // менеджер сортировок
		
		if (message == WM_CREATE) 
		{
			CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam; // создание структуры окна 
			manager = static_cast<ISortManager*>(pCreate->lpCreateParams); // менеджер сортировок 
		}

		switch(message)
		{
			case WM_CREATE:
				if (manager) 
				{
					int y = 10; // координата y 
					for (const auto& step : manager->get_steps()) 
					{
						wstringstream ss; // строковый поток
						for (int num : step)
						{
							ss << num << L" "; // добавление числа в строку
						}
						CreateWindowW(L"STATIC", ss.str().c_str(), WS_CHILD | WS_VISIBLE, 10, y, 360, 20, hwnd, NULL, NULL, NULL); // создание статического текста
						y += 30; // увеличение координаты y
					}
				}
				break; // завершение обработки сообщения
			case WM_DESTROY:
				break; // завершение обработки сообщения
			default:
				return DefWindowProc(hwnd, message, wParam, lParam); // обработка сообщения по умолчанию
		}
		return 0; // возврат значения
		return 0; // возврат значения
	}
};


// Окно времени сортировки
class TimeWindow : public IWindow
{
public:

	void show(HWND parent, void* data) 
	{
		ISortManager* manager = static_cast<ISortManager*>(data); // менеджер сортировок

		WNDCLASS wc = { 0 }; // класс окна
		wc.lpfnWndProc = TimeWindowProc; // оконная процедура
		wc.hInstance = GetModuleHandle(NULL); // дескриптор приложения
		wc.lpszClassName = L"TimeWindowClass"; // имя класса
		RegisterClass(&wc); // регистрация класса

		HWND hwnd = CreateWindowW(L"TimeWindowClass", L"Время сортировки", WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_THICKFRAME,
						CW_USEDEFAULT, CW_USEDEFAULT, 300, 200, parent, NULL, GetModuleHandle(NULL), manager); // создание окна

		ShowWindow(hwnd, SW_SHOW); // отображение окна
		UpdateWindow(hwnd); // обновление окна
	}
	static LRESULT CALLBACK TimeWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		static ISortManager* manager = nullptr; // менеджер сортировок

		if (message == WM_CREATE)
		{
			CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam; // создание структуры окна
			manager = static_cast<ISortManager*>(pCreate->lpCreateParams); // менеджер сортировок
		}

		switch (message)
		{
		case WM_CREATE:
			if (manager)
			{
				wstringstream ss; // строковый поток
				ss << L"Время сортировки: " << manager->get_last_sort_time() << L" мс"; // добавление времени сортировки
				CreateWindowW(L"STATIC", ss.str().c_str(), WS_CHILD | WS_VISIBLE, 10, 10, 260, 20, hwnd, NULL, NULL, NULL); // создание статического текста
			}
			break; // завершение обработки сообщения
		case WM_DESTROY:
			break; // завершение обработки сообщения
		default:
			return DefWindowProc(hwnd, message, wParam, lParam); // обработка сообщения по умолчанию
		}
		return 0; // возврат значения
	}
};

class ArrayViewWindow : public IWindow 
{
public:

	void show(HWND parent, void* data)
	{
		ISortManager* manager = static_cast<ISortManager*>(data); // менеджер сортировок

		WNDCLASS wc = { 0 }; // класс окна
		wc.lpfnWndProc = ArrayViewWindowProc; // оконная процедура
		wc.hInstance = GetModuleHandle(NULL); // дескриптор приложения
		wc.lpszClassName = L"ArrayViewWindowClass"; // имя класса
		RegisterClass(&wc); // регистрация классa

		HWND hwnd = CreateWindowW(L"ArrayViewWindowClass", L"Отсортированный массив", WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_THICKFRAME,
						CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, parent, NULL, GetModuleHandle(NULL), manager); // создание окна

		ShowWindow(hwnd, SW_SHOW); // отображение окна
		UpdateWindow(hwnd); // обновление окна
	}
	static LRESULT CALLBACK ArrayViewWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
	{
		static ISortManager* manager = nullptr; // менеджер сортировок 

		if (message == WM_CREATE) 
		{
			CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam; // создание структуры окна
			manager = static_cast<ISortManager*>(pCreate->lpCreateParams); // менеджер сортировок
		}

		switch (message) 
		{
			case WM_CREATE:
				if (message) 
				{
					wstringstream ssOriginal, ssSorted; // строковые потоки 
					ssOriginal << L"Исходный массив: "; // добавление текста
					for (int num : manager->get_sorted_array())
					{
						ssOriginal << num << L" "; // добавление числа
					}
					ssSorted << L"Отсортированный массив: "; // добавление текста
					for (int num : manager->get_sorted_array())
					{
						ssSorted << num << L" "; // добавление числа
					}

					CreateWindowW(L"STATIC", ssOriginal.str().c_str(), WS_CHILD | WS_VISIBLE, 10, 10, 360, 20, hwnd, NULL, NULL, NULL); // создание статического текста
					CreateWindowW(L"STATIC", ssSorted.str().c_str(), WS_CHILD | WS_VISIBLE, 10, 40, 360, 20, hwnd, NULL, NULL, NULL); // создание статического текста
				}
				break; 
			case WM_DESTROY:
				break;
			default: 
				return DefWindowProc(hwnd, message, wParam, lParam); // обработка сообщения по умолчанию
		}
		return 0;
	}
};

// Главное окно
class MainWindow
{
private: 
	HWND hMainWnd; // дескриптор главного окна
	ISortManager* sort_manager; // менеджер сортировок
	IWindow* step_window; // окно с ходами сортировки
	IWindow* time_window; // окно с временем сортировки
	IWindow* array_view_window; // окно с отсортированным массивом	

public:
	MainWindow() 
	{
		sort_manager = new SortManager(); // создание менеджера сортировок
		step_window = new StepWindow(); // создание окна с ходами сортировки
		time_window = new TimeWindow(); // создание окна с временем сортировки
		array_view_window = new ArrayViewWindow(); // создание окна с отсортированным массивом
	}
	~MainWindow() 
	{
		delete sort_manager; // удаление менеджера сортировок
		delete step_window; // удаление окна с ходами сортировки
		delete time_window; // удаление окна с временем сортировки
		delete array_view_window; // удаление окна с отсортированным массивом
	}
	void init_ui(HWND hwnd) {
		hMainWnd = hwnd;

		CreateWindowEx(0, L"STATIC", L"Введите массив (числа через пробел):", WS_CHILD | WS_VISIBLE,
			10, 10, 300, 20, hMainWnd, NULL, NULL, NULL);

		CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE,
			10, 40, 300, 60, hMainWnd, (HMENU)IDC_INPUT_ARRAY_EDIT, NULL, NULL);

		CreateWindowEx(0, L"STATIC", L"Выберите тип сортировки:", WS_CHILD | WS_VISIBLE,
			10, 120, 200, 20, hMainWnd, NULL, NULL, NULL);

		HWND hCombo = CreateWindowEx(0, L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE,
			10, 140, 200, 100, hwnd, (HMENU)IDC_SORT_TYPE_COMBO, NULL, NULL);

		for (auto sort_algo : sort_manager->get_sort_algorithms()) {
			SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)sort_algo->get_sort_name().c_str());
		}

		CreateWindowEx(0, L"BUTTON", L"Сортировать", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
			10, 180, 100, 30, hMainWnd, (HMENU)IDC_SORT_BUTTON, NULL, NULL);

		CreateWindowEx(0, L"BUTTON", L"Показать этапы", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
			120, 180, 120, 30, hMainWnd, (HMENU)IDC_SHOW_STEPS_BUTTON, NULL, NULL);

		CreateWindowEx(0, L"BUTTON", L"Показать время", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
			250, 180, 120, 30, hMainWnd, (HMENU)IDC_SHOW_TIME_BUTTON, NULL, NULL);

		CreateWindowEx(0, L"BUTTON", L"Показать массивы", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
			10, 220, 120, 30, hMainWnd, (HMENU)IDC_VIEW_ARRAY_BUTTON, NULL, NULL);
	}

	void sort_array() {
		wchar_t input[1024];
		GetDlgItemTextW(hMainWnd, IDC_INPUT_ARRAY_EDIT, input, 1024);
		int sort_index = SendDlgItemMessageW(hMainWnd, IDC_SORT_TYPE_COMBO, CB_GETCURSEL, 0, 0);
		if (sort_index == CB_ERR) {
			MessageBoxW(hMainWnd, L"Пожалуйста, выберите тип сортировки.", L"Ошибка", MB_OK | MB_ICONERROR);
			return;
		}

		sort_manager->sort_array(input, sort_index);

		MessageBoxW(hMainWnd, L"Сортировка завершена.", L"Уведомление", MB_OK | MB_ICONINFORMATION);
	}

	// отображение окна с ходами сортировки
	void show_steps_window() 
	{
		step_window->show(hMainWnd, sort_manager); // отображение окна с ходами сортировки
	}

	// отображение окна с временем сортировки
	void show_time_window()
	{
		time_window->show(hMainWnd, sort_manager); // отображение окна с временем сортировки
	}

	//  отображение окна с отсортированным массивом
	void show_array_view_window()
	{
		array_view_window->show(hMainWnd, sort_manager); // отображение окна с отсортированным массивом
	}
};

// Прототипы функций
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	static  MainWindow* main_window = nullptr; // главное окно

	switch (message)
	{
		case WM_CREATE:
			main_window = new MainWindow(); // создание главного окна
			main_window->init_ui(hwnd); // инициализация интерфейса
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_SORT_BUTTON:
					main_window->sort_array(); // сортировка массива
					break;
				case IDC_SHOW_STEPS_BUTTON:
					main_window->show_steps_window(); // отображение окна с ходами сортировки
					break;
				case IDC_SHOW_TIME_BUTTON:
					main_window->show_time_window(); // отображение окна с временем сортировки
					break;
				case IDC_VIEW_ARRAY_BUTTON:
					main_window->show_array_view_window(); // отображение окна с отсортированным массивом
					break;
			}
			break;
		case WM_DESTROY:
			delete main_window; // удаление главного окна
			PostQuitMessage(0); // посылка сообщения о завершении приложения
			break;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam); // обработка сообщения по умолчанию
	}
	return 0; // возврат значения
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSW wc = { 0 }; // класс окна
	wc.lpszClassName = L"MainWindowClass"; // имя класса
	wc.lpfnWndProc = MainWndProc; // оконная процедура
	wc.hInstance = hInstance; // дескриптор приложения
	RegisterClassW(&wc); // регистрация класса

	HWND hwnd = CreateWindowW(L"MainWindowClass", L"Сортировщик", WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_THICKFRAME,
		CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, hInstance, NULL); // создание окна

	ShowWindow(hwnd, nCmdShow); // отображение окна
	UpdateWindow(hwnd); // обновление окна

	MSG msg; // сообщение
	while (GetMessage(&msg, NULL, 0, 0)) // получение сообщения
	{
		TranslateMessage(&msg); // трансляция сообщения
		DispatchMessage(&msg); // отправка сообщения
	}
	return 0;
}
