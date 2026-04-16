#include <fstream>
#include <iostream>
#include <cstring>

using namespace std;

// ==================== КОНСТАНТЫ ====================
const unsigned N = 10;              // максимальное количество исходных строк
const unsigned M = 60;              // максимальная длина строки на устройстве вывода
const unsigned MAX_WORD_LEN = 30;   // максимальная длина исходной строки (по заданию)

const char* INPUT_FILE = "C:\\Users\\Анечка\\Documents\\2\\in.txt";
const char* OUTPUT_FILE = "C:\\Users\\Анечка\\Documents\\2\\out.txt";

// ==================== СТРУКТУРЫ ====================
struct str {
    unsigned len;           // текущая длина строки
    char A[M + 1];          // массив символов (+1 для '\0')
};

struct text {
    unsigned l;             // количество строк
    str T[N];               // массив строк
};

// ==================== ФУНКЦИИ ДЛЯ РАБОТЫ СО СТРОКАМИ ====================

// Ввод одной строки из файла
void s_inp(ifstream& fin, str& s) {
    s.len = 0;
    char c;
    while (fin.get(c)) {
        if (c == '\n' || fin.eof()) break;
        s.A[s.len] = c;
        s.len++;
        if (s.len >= MAX_WORD_LEN) break;
    }
    s.A[s.len] = '\0';
}

// Вывод одной строки в файл
void s_out(ofstream& fout, const str& s) {
    for (unsigned i = 0; i < s.len; i++) {
        fout << s.A[i];
    }
}

// Длина строки
unsigned s_len(const str& s) {
    return s.len;
}

// Форматирование строки до заданной длины (равномерное добавление пробелов)
void s_format(str& s, unsigned target_len) {
    if (s.len >= target_len) return;

    // Подсчитываем количество слов в строке
    unsigned wordCount = 0;
    bool inWord = false;
    for (unsigned i = 0; i < s.len; i++) {
        if (s.A[i] != ' ') {
            if (!inWord) {
                wordCount++;
                inWord = true;
            }
        }
        else {
            inWord = false;
        }
    }

    if (wordCount <= 1) return;  // одно слово - нечего расширять

    // Сколько пробелов нужно добавить всего
    unsigned totalSpacesToAdd = target_len - s.len;
    unsigned gaps = wordCount - 1;  // количество промежутков между словами

    // Равномерное распределение
    unsigned baseSpaces = totalSpacesToAdd / gaps;   // каждому промежутку
    unsigned extraSpaces = totalSpacesToAdd % gaps;  // первые промежутки получают +1

    // Создаем новую строку
    char newStr[M + 1];
    unsigned newPos = 0;
    unsigned oldPos = 0;
    unsigned gapIndex = 0;

    while (oldPos < s.len) {
        // Копируем слово (до пробела или до конца)
        while (oldPos < s.len && s.A[oldPos] != ' ') {
            newStr[newPos++] = s.A[oldPos++];
        }

        // Если это не последнее слово
        if (oldPos < s.len && s.A[oldPos] == ' ') {
            oldPos++;  // пропускаем исходный пробел

            // Добавляем пробелы: 1 базовый + дополнительные
            unsigned spacesToAdd = 1 + baseSpaces;
            if (gapIndex < extraSpaces) spacesToAdd++;

            for (unsigned j = 0; j < spacesToAdd; j++) {
                newStr[newPos++] = ' ';
            }
            gapIndex++;
        }
    }

    // Копируем обратно
    for (unsigned i = 0; i < newPos; i++) {
        s.A[i] = newStr[i];
    }
    s.len = newPos;
    s.A[newPos] = '\0';
}

// ==================== ФУНКЦИИ ДЛЯ РАБОТЫ С ТЕКСТОМ ====================

// Ввод всего текста из файла
void t_inp(ifstream& fin, text& t) {
    t.l = 0;
    while (t.l < N && !fin.eof()) {
        s_inp(fin, t.T[t.l]);
        if (t.T[t.l].len > 0) {
            t.l++;
        }
    }
}

// Вывод всего текста в файл
void t_out(ofstream& fout, const text& t) {
    for (unsigned i = 0; i < t.l; i++) {
        s_out(fout, t.T[i]);
        fout << '\n';
    }
}

// Основная обработка: склеивание строк в строки заданной длины
void t_process(text& t, unsigned target_len) {
    text result;
    result.l = 0;

    str current;
    current.len = 0;

    for (unsigned i = 0; i < t.l; i++) {
        // Пытаемся добавить очередную исходную строку
        unsigned newLen = current.len;

        if (current.len > 0) {
            newLen += 1;  // добавляем пробел-разделитель
        }
        newLen += t.T[i].len;  // добавляем длину новой строки

        if (newLen <= target_len) {
            // Строка помещается - добавляем
            if (current.len > 0) {
                current.A[current.len] = ' ';
                current.len++;
            }

            // Копируем исходную строку
            for (unsigned j = 0; j < t.T[i].len; j++) {
                current.A[current.len] = t.T[i].A[j];
                current.len++;
            }
            current.A[current.len] = '\0';
        }
        else {
            // Строка не помещается - завершаем текущую и начинаем новую

            // Форматируем текущую строку до нужной длины (добавляем пробелы)
            if (current.len > 0) {
                s_format(current, target_len);
                // Сохраняем в результат
                result.T[result.l] = current;
                result.l++;
            }

            // Начинаем новую строку с текущей исходной строки
            current.len = 0;
            for (unsigned j = 0; j < t.T[i].len; j++) {
                current.A[current.len] = t.T[i].A[j];
                current.len++;
            }
            current.A[current.len] = '\0';
        }
    }

    // Добавляем последнюю строку, если она не пустая
    if (current.len > 0) {
        s_format(current, target_len);
        result.T[result.l] = current;
        result.l++;
    }

    // Заменяем исходный текст результатом
    t = result;
}

// ==================== ФУНКЦИИ ПРОВЕРКИ ФАЙЛОВ ====================

// Проверка существования и содержимого входного файла
bool in_file_check() {
    ifstream file(INPUT_FILE);

    // Проверка 1: существует ли файл и можно ли его открыть
    if (!file.is_open()) {
        cout << "Ошибка: Не удалось открыть входной файл!" << endl;
        cout << "Путь: " << INPUT_FILE << endl;
        return true;  // true = ошибка
    }

    // Проверка 2: есть ли в файле хоть что-то (не пустой ли он)
    file.seekg(0, ios::end);  // перемещаемся в конец файла
    if (file.tellg() == 0) {   // если позиция = 0, значит файл пустой
        cout << "Ошибка: Входной файл пуст!" << endl;
        file.close();
        return true;
    }

    // Проверка 3: есть ли в файле хотя бы одна строка с данными
    file.seekg(0, ios::beg);  // возвращаемся в начало
    char firstChar;
    bool hasContent = false;
    while (file.get(firstChar)) {
        if (firstChar != '\n' && firstChar != '\r' && firstChar != ' ') {
            hasContent = true;
            break;
        }
    }

    if (!hasContent) {
        cout << "Ошибка: Входной файл не содержит значимых данных (только пробелы и пустые строки)!" << endl;
        file.close();
        return true;
    }

    file.close();
    cout << "Входной файл успешно открыт и содержит данные" << endl;
    return false;  // false = всё хорошо
}

// Проверка выходного файла (можно ли создать/перезаписать)
bool out_file_check() {
    // Пытаемся открыть файл для записи (это создаст файл или перезапишет)
    ofstream file(OUTPUT_FILE);
    if (!file.is_open()) {
        cout << "Ошибка: Не удалось создать/открыть выходной файл!" << endl;
        cout << "Путь: " << OUTPUT_FILE << endl;
        return true;
    }

    file.close();
    cout << "Выходной файл готов к записи" << endl;
    return false;
}

// ==================== ГЛАВНАЯ ФУНКЦИЯ ====================

int main() {
    setlocale(LC_ALL, "RU");

    cout << "========================================" << endl;
    cout << "Программа форматирования текста" << endl;
    cout << "========================================" << endl << endl;

    // Проверяем входной файл
    if (in_file_check()) {
        cout << "Программа завершена из-за ошибки входного файла" << endl;
        cout << "Нажмите Enter для выхода...";
        cin.get();
        cin.get();
        return 1;
    }

    // Проверяем выходной файл
    if (out_file_check()) {
        cout << "Программа завершена из-за ошибки выходного файла" << endl;
        cout << "Нажмите Enter для выхода...";
        cin.get();
        cin.get();
        return 1;
    }

    cout << endl;

    // Запрашиваем целевую длину
    unsigned target_len;
    cout << "Введите желаемую длину строк (не более " << M << "): ";
    cin >> target_len;

    if (target_len > M) {
        cout << "Длина не может превышать " << M << ". Установлено " << M << endl;
        target_len = M;
    }

    if (target_len < 1) {
        cout << "Длина должна быть положительной. Установлено 10" << endl;
        target_len = 10;
    }

    cout << endl;

    // Открываем файлы
    ifstream fin(INPUT_FILE);
    ofstream fout(OUTPUT_FILE);

    text A;
    t_inp(fin, A);

    // Дополнительная проверка: есть ли вообще строки после чтения
    if (A.l == 0) {
        cout << "Ошибка: Во входном файле нет ни одной строки с данными!" << endl;
        fin.close();
        fout.close();
        cout << "Нажмите Enter для выхода...";
        cin.get();
        cin.get();
        return 1;
    }

    cout << "Прочитано строк: " << A.l << endl << endl;

    // Выводим исходный текст
    fout << "Исходный текст (строки без пробелов):" << endl;
    t_out(fout, A);
    fout << endl;

    // Обрабатываем
    t_process(A, target_len);

    // Выводим результат
    fout << "Результат (длина строк = " << target_len << "):" << endl;
    t_out(fout, A);

    fin.close();
    fout.close();

    cout << "========================================" << endl;
    cout << "Готово! Результат сохранен в:" << endl;
    cout << OUTPUT_FILE << endl;
    cout << "========================================" << endl;

    cout << "\nНажмите Enter для выхода...";
    cin.get();
    cin.get();

    return 0;
}