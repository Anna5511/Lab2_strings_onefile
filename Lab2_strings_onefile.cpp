#include <fstream>
#include <iostream>

//макс. кол-во строк в файле
const unsigned N = 50;
//макс. длина формируемой строки
const unsigned M = 100;
//макс. длина строки на входе
const unsigned max_in = 30;

const char* inp_file = "C:\\Users\\Анечка\\Documents\\2\\in.txt";
const char* out_file = "C:\\Users\\Анечка\\Documents\\2\\out.txt";

struct str {
    unsigned len;
    char A[M + 1];
};

struct text {
    unsigned l;
    str T[N];
};

// Ввод одной строки из файла
void s_inp(std::ifstream& inp_file, str& s) {
    s.len = 0;
    char c;
    while (inp_file.get(c)) {
        if (c == '\n' || inp_file.eof()) break;
        s.A[s.len] = c;
        s.len++;
        if (s.len >= max_in) break;
    }
    s.A[s.len] = '\0';
}

// Вывод одной строки в файл
void s_out(std::ofstream& out_file, const str& s) {
    for (unsigned i = 0; i < s.len; i++) {
        out_file << s.A[i];
    }
}

// Форматирование строки до заданной длины (равномерное добавление пробелов)
void s_format(str& s, unsigned new_str_len) {
    if (s.len >= new_str_len) return;

    // Подсчитываем количество слов в строке
    unsigned word_count = 0;
    //находимся ли мы внутри слова?
    bool inWord = false;
    for (unsigned i = 0; i < s.len; i++) {
        if (s.A[i] != ' ') {
            if (!inWord) {
                word_count++;
                inWord = true;
            }
        }
        else {
            inWord = false;
        }
    }

    if (word_count <= 1) return;

    // Сколько пробелов нужно добавить всего
    unsigned spaces_add = new_str_len - s.len;
    //количество промежутков между словами
    unsigned gaps = word_count - 1;

    unsigned common_spaces = spaces_add / gaps;   // каждому промежутку
    unsigned extra_spaces = spaces_add % gaps;  // первые промежутки получают +1

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
            unsigned spacesToAdd = 1 + common_spaces;
            if (gapIndex < extra_spaces) spacesToAdd++;

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

// Ввод всего текста из файла
void t_inp(std::ifstream& inp_file, text& t) {
    t.l = 0;
    while (t.l < N && !(inp_file.eof())) {
        s_inp(inp_file, t.T[t.l]);
        if (t.T[t.l].len > 0) {
            t.l++;
        }
    }
}

// Вывод всего текста в файл
void t_out(std::ofstream& fout, const text& t) {
    for (unsigned i = 0; i < t.l; i++) {
        s_out(fout, t.T[i]);
        fout << '\n';
    }
}

//////////////////////////
void t_process(text& t, unsigned new_str_len) {
    text result;
    result.l = 0;

    str string;
    string.len = 0;

    for (unsigned i = 0; i < t.l; i++) {
        unsigned newLen = string.len;

        if (string.len > 0) {
            // добавляем пробел-разделитель
            newLen += 1;
        }
        // добавляем длину новой строки
        newLen += t.T[i].len;

        if (newLen <= new_str_len) {
            // Строка помещается - добавляем
            if (string.len > 0) {
                string.A[string.len] = ' ';
                string.len++;
            }

            // Копируем исходную строку
            for (unsigned j = 0; j < t.T[i].len; j++) {
                string.A[string.len] = t.T[i].A[j];
                string.len++;
            }
            string.A[string.len] = '\0';
        }
        else {
            // Строка не помещается - завершаем текущую и начинаем новую

            // Форматируем текущую строку до нужной длины (добавляем пробелы)
            if (string.len > 0) {
                s_format(string, new_str_len);
                // Сохраняем в результат
                result.T[result.l] = string;
                result.l++;
            }

            // Начинаем новую строку с текущей исходной строки
            string.len = 0;
            for (unsigned j = 0; j < t.T[i].len; j++) {
                string.A[string.len] = t.T[i].A[j];
                string.len++;
            }
            string.A[string.len] = '\0';
        }
    }

    // Добавляем последнюю строку, если она не пустая
    if (string.len > 0) {
        s_format(string, new_str_len);
        result.T[result.l] = string;
        result.l++;
    }

    // Заменяем исходный текст результатом
    t = result;
}

bool in_file_check() {
    std::ifstream file(inp_file);

    //Существует ли файл?
    if (!file.is_open()) {
        std::cout << "Ошибка открытия входного файла";
        return true;
    }

    //Не пустой ли файл?
    file.seekg(0, std::ios::end);
    if (file.tellg() == 0) {
        std::cout << "Ошибка: Входной файл пуст";
        file.close();
        return true;
    }

    //Есть ли в файле данные?
    file.seekg(0, std::ios::beg);
    char first_char;
    bool flag = false;
    while (file.get(first_char)) {
        if (first_char != '\n' && first_char != '\r' && first_char != ' ') {
            flag = true;
            break;
        }
    }

    if (!flag) {
        std::cout << "Ошибка: Входной файл не содержит значимых данных (только пробелы и пустые строки)" << std::endl;
        file.close();
        return true;
    }

    file.close();
    return false;
}

bool out_file_check() {
    std::ofstream file(out_file);
    if (!file.is_open()) {
        std::cout << "Ошибка открытия выходного файла";
        return true;
    }

    file.close();
    return false;
}

int main() {
    setlocale(LC_ALL, "RU");

    if (in_file_check()) return 0;
    if (out_file_check()) return 0;

    // Ввод длины формируемой строки
    unsigned new_str_len;
    std::cout << "Введите длину формируемых строк (не более " << M << "): ";
    std::cin >> new_str_len;

    if (new_str_len > M) {
        std::cout << "Ошибка: лина не может превышать " << M << "!";
        return 0;
    }

    if (new_str_len < 1) {
        std::cout << "Ошибка: длина должна быть положительной!";
        return 0;
    }
    // Открываем файлы
    std::ifstream in(inp_file);
    std::ofstream out(out_file);

    text A;
    t_inp(in, A);
    std::cout << "Прочитано строк: " << A.l;
    t_process(A, new_str_len);
    t_out(out, A);

    in.close();
    out.close();
    return 0;
}