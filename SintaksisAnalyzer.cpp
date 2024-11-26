#include "SintaksisAnalyzer.h"
#include "TokenList.h"
#include <iostream>
#include <string>
#include <cstring> // для memset

SintaksisAnalyzer::SintaksisAnalyzer() {
    outputFile.open("parsing_tree.txt");
    if (!outputFile.is_open()) {
        throw std::ios_base::failure("Failed to open the file.");
    }
}

SintaksisAnalyzer::~SintaksisAnalyzer() {
    if (outputFile.is_open()) {
        outputFile.close();
    }
}

void SintaksisAnalyzer::check_error() {
    if (!is_valid_end) {
        clear_tree();
        error(1234, "", "Отсутствует конец программы(END id_name)");
        throw std::ios_base::failure("An error was encountered in the input file.");
    }

    // Переменные для отслеживания состояния порядка лексем
    bool found_start = false;
    bool found_end = false;
    bool found_description = false;
    bool found_operator = false;

    // Проверяем порядок лексем
    for (int i = 0; i < count_types_lexeme; ++i) {
        const std::string& lexeme = types_lexeme[i];

        if (lexeme == "start") {
            // Убедимся, что "start" встречается только один раз
            if (found_start) {
                clear_tree();
                error(1234, "", "Множественные вхождения start");
                throw std::ios_base::failure("An error was encountered in the input file.");
            }
            found_start = true;
        }
        else if (lexeme == "end") {
            // Убедимся, что "end" встречается только один раз
            if (found_end) {
                clear_tree();
                error(1234, "", "Множественные вхождения end");
                throw std::ios_base::failure("An error was encountered in the input file.");
            }
            found_end = true;
        }
        else if (lexeme == "description") {
            // description должно идти только после start и до оператора или end
            if (!found_start || found_end || found_operator) {
                clear_tree();
                error(1234, "", "Ошибка: описание должно идти после start и до оператора или end");
                throw std::ios_base::failure("An error was encountered in the input file.");
            }
            found_description = true;
        }
        else if (lexeme == "operator") {
            // operator должно идти только после description и до end
            if (!found_start || !found_description || found_end) {
                clear_tree();
                error(1234, "", "Ошибка: оператор должен идти после description и до end");
                throw std::ios_base::failure("An error was encountered in the input file.");
            }
            found_operator = true;
        }
    }
}


void SintaksisAnalyzer::clear_tree() {
    outputFile.close();
    std::ofstream treeFile("parsing_tree.txt", std::ios::trunc);
    if (!treeFile.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл errors.txt для очистки." << std::endl;
    }
    treeFile.close(); // Закрываем файл после очистки
}

bool SintaksisAnalyzer::isValidIdentifier(const std::string word) {
    for (char c : word) {
        if (!isalpha(c)) { // Если символ не является буквой
            return false;
        }
    }
    return true;
}

// Проверка, является ли строка валидным выражением (пока допускаем только числа)
bool SintaksisAnalyzer::isValidExpression(const std::string expr) {
    return !expr.empty() && std::all_of(expr.begin(), expr.end(), ::isdigit);
}

std::string* SintaksisAnalyzer::split(const std::string line, int& numWords) {
    static const int MAX_WORDS = 30;  // Максимальное количество слов
    static std::string words[MAX_WORDS];  // Статический массив слов

    // Очистка массива перед использованием
    for (int i = 0; i < MAX_WORDS; i++) {
        words[i].clear();
    }

    size_t start = 0;
    size_t end = line.find(' '); // Находим первый пробел
    numWords = 0;                // Счётчик слов

    // Разделение строки на слова
    while (end != std::string::npos && numWords < MAX_WORDS) {
        std::string word = line.substr(start, end - start);
        if (!word.empty()) { // Добавляем слово, только если оно не пустое
            words[numWords++] = word;
        }
        start = end + 1;
        end = line.find(' ', start); // Находим следующий пробел
    }

    // Добавляем последнее слово, если оно есть
    if (start < line.length()) {
        std::string word = line.substr(start);
        if (!word.empty() && numWords < MAX_WORDS) {
            words[numWords++] = word;
        }
    }

    return words;  // Возвращаем указатель на массив слов
}

// Проверка, является ли строка корректным оператором (пока простая проверка)
bool SintaksisAnalyzer::isValidOperator(const std::string opLine) {
    int count_words = 0;
    // Например, проверяем, что это строка вида "id = id + expr" или "id = expr"
    std::string* tokens = split(opLine, count_words);
    // Проверяем форматы "id = id + expr" или "id = expr"
    if (count_words == 3 && isValidIdentifier(tokens[0]) && tokens[1] == "=" &&
        (isValidExpression(tokens[2]) || isValidIdentifier(tokens[2]))) {
        return true;
    }
    else if (count_words == 5 && isValidIdentifier(tokens[0]) && tokens[1] == "=" &&
        (isValidExpression(tokens[2]) || isValidIdentifier(tokens[2])) &&
        (tokens[3] == "+" || tokens[3] == "-") &&
        (isValidExpression(tokens[4]) || isValidIdentifier(tokens[4]))) {
        return true;
    }
    return false;
}


// Проверка, является ли строка корректным оператором (пока простая проверка)
bool SintaksisAnalyzer::isValidOperator(const std::string opLine, const int count_line) {
    std::string error_;
    int count_words = 0;
    // Разделяем строку на токены
    std::string* tokens = split(opLine, count_words);

    // Проверяем, что строка не пустая и содержит минимум "id = что-то"
    if (count_words < 3) {
        return false;
    }

    // Проверяем второй токен как оператор присваивания
    if (tokens[1] != "=") {
        return false;
    }

    // Проверяем первый токен как идентификатор
    if (!isValidIdentifier(tokens[0])) {
        error_ = "Id не соответствует правилам, встречен посторонний символ -> " + tokens[0];
        is_error_flag = true;
        error(count_line, opLine, error_);
        return false;
    }

    // Проверяем оставшиеся токены после '='
    bool expectingOperand = true; // Указывает, ожидаем ли операнд или оператор
    for (int i = 2; i < count_words; ++i) {
        if (expectingOperand) {
            // Ожидаем операнд: идентификатор или выражение
            if (!isValidExpression(tokens[i]) && !isValidIdentifier(tokens[i])) {
                error_ = "Ожидается идентификатор или выражение, встречено -> " + tokens[i];
                is_error_flag = true;
                error(count_line, opLine, error_);
                return false;
            }
            expectingOperand = false; // Следующий токен должен быть оператором
        }
        else {
            // Ожидаем оператор: '+' или '-'
            if (tokens[i] != "+" && tokens[i] != "-") {
                error_ = "Ожидается оператор '+' или '-', встречено -> " + tokens[i];
                is_error_flag = true;
                error(count_line, opLine, error_);
                return false;
            }
            expectingOperand = true; // Следующий токен должен быть операндом
        }
    }

    // Если цикл завершился, но последним токеном был оператор, это ошибка
    if (expectingOperand) {
        error_ = "Строка заканчивается на оператор без операнда.";
        is_error_flag = true;
        error(count_line, opLine, error_);
        return false;
    }

    // Если дошли до сюда, строка корректна
    return true;
}

void SintaksisAnalyzer::building_tree(const int count_line, const std::string line) {
    if (is_error_flag) {
        clear_tree();
        std::cout << "An error has been detected, take a look at the file <errors.txt> to get acquainted." << "\n";
        throw std::ios_base::failure("An error was encountered in the input file.");
    }
    if ( is_cycle(line, count_line) ) {
        if (!is_valid_start) {
            clear_tree();
            error(1234, "", "Отсутствует старт программы(PROGRAM id_name)");
            throw std::ios_base::failure("An error was encountered in the input file.");
        }
        else if (!is_valid_descriptions) {
            clear_tree();
            error(1234, "", "Отсутствуют описания переменных(INTEGER VarList)");
            throw std::ios_base::failure("An error was encountered in the input file.");
        }
        types_lexeme[count_types_lexeme] = "operator";
        count_types_lexeme++;
        is_valid_operator = true;
        draw_cycle(line);
    }
    else if ( is_start_program(line, count_line) ) {
        types_lexeme[count_types_lexeme] = "start";
        count_types_lexeme++;
        is_valid_start = true;
        draw_start_program(line);
    }
    else if ( is_end_program(line, count_line) ) {
        if (!is_valid_start) {
            clear_tree();
            error(1234, "", "Отсутствует старт программы(PROGRAM id_name)");
            throw std::ios_base::failure("An error was encountered in the input file.");
        }
        else if (!is_valid_descriptions) {
            clear_tree();
            error(1234, "", "Отсутствуют описания переменных(INTEGER VarList)");
            throw std::ios_base::failure("An error was encountered in the input file.");
        }
        else if (!is_valid_operator) {
            clear_tree();
            error(1234, "", "Отсутствуют Operators");
            throw std::ios_base::failure("An error was encountered in the input file.");
        }
        types_lexeme[count_types_lexeme] = "end";
        count_types_lexeme++;
        is_valid_end = true;
        draw_end_program(line);
    }
    else if ( is_descriptions(line, count_line) ) {
        if (!is_valid_start) {
            clear_tree();
            error(1234, "", "Отсутствует старт программы(PROGRAM id_name)");
            throw std::ios_base::failure("An error was encountered in the input file.");
        }
        types_lexeme[count_types_lexeme] = "description";
        count_types_lexeme++;
        is_valid_descriptions = true;
        draw_descriptions(line);
    }
    else if (isValidOperator(line, count_line)) {
        if (!is_valid_start) {
            clear_tree();
            error(1234, "", "Отсутствует старт программы(PROGRAM id_name)");
            throw std::ios_base::failure("An error was encountered in the input file.");
        }
        else if (!is_valid_descriptions) {
            clear_tree();
            error(1234, "", "Отсутствуют описания переменных(INTEGER VarList)");
            throw std::ios_base::failure("An error was encountered in the input file.");
        }
        types_lexeme[count_types_lexeme] = "operator";
        count_types_lexeme++;
        is_valid_operator = true;
        draw_operators(line);
    }
    //else if (is_VarList(line)) {
    //    //std::cout << "TYTYTYTYTYTYTYTY" << "\n";
    //    draw_varlist(line);
    //}
    else {
        if (!is_error_flag && line != "") {
            int count_words = 0;
            std::string* splitting_line = split(line, count_words);
            if (count_words == 1) {
                is_error_flag = true;
                error(count_line, splitting_line[0], "Недопускается писать 1 переменную/число");
            }
            else {
                is_error_flag = true;
                error(count_line, line, "Нераспознанные индентификатор, проверьте его корректность.");
            }
        }
    }
    
}

void SintaksisAnalyzer::error(const int count_line, const std::string line, const std::string type_error) {
    // Режим добавления
    std::ofstream errorFile("errors.txt", std::ios::app);

    if (!errorFile.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл errors.txt для записи." << std::endl;
        return;
    }

    // Формируем строку с ошибкой
    errorFile << "Ошибка в строке #" << count_line << " - " << type_error << ": " << line << std::endl;

    errorFile.close();
}


bool SintaksisAnalyzer::is_cycle(const std::string line, const int count_line) {
    int count_words = 0;
    int is_do = 0;
    int is_to = 0;
    bool flag = false;
    std::string* lines = split(line, count_words); // Разбиваем строку на слова

    for (int i = 0; i < count_words; ++i) {
        if (lines[i] == "FOR" || lines[i] == "TO" || lines[i] == "DO") {
            flag = true;
            if (lines[i] == "DO") is_do = i;
            if (lines[i] == "TO") is_to = i;
        }
        else if (lines[i] == "for" || lines[i] == "to" || lines[i] == "do") {
            is_error_flag = true;
            error(count_line, line, "Ключевое слово(FOR, TO, DO) нужно писать капсом");
            return false;
        }
    }

    if (flag) {
        if (count_words < 7) { // Минимальная форма: "for i = 1 to i = 10 do"
            is_error_flag = true;
            error(count_line, line, "Не хватает символов для соответствия правилу лексемы(FOR ID = Expr TO EXPR DO Operators)");
            return false;
        }
    }
    else return false;

    // Проверка минимального количества токенов
    if (count_words < 7) { // Минимальная форма: "for i = 1 to i = 10 do"
        return false;
    }
    /*std::cout << line << "\n";
    for (int i = 0; i < count_words; ++i) {
        std::cout << lines[i] << " ";
    }
    std::cout << "\n";*/
    // Проверяем структуру "for ... to ... do"
    if (lines[0] != "FOR" || lines[is_to] != "TO" || lines[is_do] != "DO") {
        is_error_flag = true;
        error(count_line, line, "Не хватает ключевого слова FOR/TO/DO");
        return false;
    }

    // Проверка первой части "id = expr"
    if (!isValidIdentifier(lines[1]) || lines[2] != "=" || !isValidExpression(lines[3])) {
        is_error_flag = true;
        error(count_line, line, "Несоответствие первой части <id = expr>");
        return false;
    }

    // Проверка второй части "id = expr" или "expr"
    for (int i = is_to + 1; i < is_do; ++i) {
        // Проверяем первый элемент выражения
        if (i == is_to + 1) {
            if (!isValidIdentifier(lines[i]) && !isValidExpression(lines[i])) {
                is_error_flag = true;
                error(count_line, line, "Ошибка в начале второй части: ожидалось число или идентификатор.");
                return false;
            }
        }
        else {
            // Проверяем чередование операторов и идентификаторов/выражений
            if (i % 2 == 0) { // Ожидаем оператор на четных позициях
                if (lines[i] != "+" && lines[i] != "-") {
                    is_error_flag = true;
                    error(count_line, line, "Ожидался оператор '+' или '-'.");
                    return false;
                }
            }
            else { // Ожидаем идентификатор или выражение на нечетных позициях
                if (!isValidIdentifier(lines[i]) && !isValidExpression(lines[i])) {
                    is_error_flag = true;
                    error(count_line, line, "Ошибка в выражении: ожидалось число или идентификатор.");
                    return false;
                }
            }
        }
    }

    // Проверка оператора после "do"
    std::string operatorLine = "";
    for (int i = is_do + 1; i < count_words; ++i) { // После "do" всё считается оператором
        operatorLine += lines[i] + " ";
    }

    if (!isValidOperator(operatorLine)) {
        is_error_flag = true;
        error(count_line, line, "Operators после DO несоответствует правилам.");
        return false;
    }

    return true; // Строка прошла все проверки
}

// Проверяет, соответствует ли строка началу программы (PROGRAM <идентификатор>)
bool SintaksisAnalyzer::is_start_program(const std::string line, const int count_line) {
    int count_words = 0;
    std::string* words = split(line, count_words);

    // Проверка: строка должна состоять ровно из двух слов
    if (count_words != 2) {
        return false;
    }

    // Первое слово должно быть "PROGRAM", второе - валидным идентификатором
    if (words[0] == "PROGRAM" && isValidIdentifier(words[1])) {
        return true;
    }

    return false;
}

// Проверяет, соответствует ли строка завершению программы (END PROGRAM <идентификатор>)
bool SintaksisAnalyzer::is_end_program(const std::string line, const int count_line) {
    int count_words = 0;
    std::string* words = split(line, count_words);

    // Проверка: строка должна состоять ровно из трех слов
    if (count_words != 2) {
        return false;
    }

    // Первое слово должно быть "END", второе - "PROGRAM", третье - валидным идентификатором
    if (words[0] == "END" && isValidIdentifier(words[1])) {
        return true;
    }

    return false;
}

bool SintaksisAnalyzer::is_descriptions(const std::string line, const int count_line) {
    int count_words = 0;
    std::string* words = split(line, count_words);

    // Проверка: строка должна начинаться с ключевого слова "INTEGER"
    if (count_words < 2 || words[0] != "INTEGER") {
        if (words[0] == "INTEGER") { 
            is_error_flag = true;
            error(count_line, line, "Нет VarList после INTEGER");
            is_error_flag = true;
        }
        return false;
    }
    // Проверяем каждое слово после "INTEGER"
    for (int i = 1; i < count_words; ++i) {
        // Удаляем запятую, если она есть в конце слова
        
            if (words[i].back() == ',') {
                words[i].erase(words[i].size() - 1); // Убираем последнюю запятую
            }
        
        // Проверяем валидность идентификатора
        if (!isValidIdentifier(words[i])) {
            is_error_flag = true;
            error(count_line, line, "Недопустимый индентификатор: " + words[i]);
            return false;
        }
    }
    return true;
}


void SintaksisAnalyzer::draw_operators(const std::string line) {
    outputFile << " Operators\n";
    outputFile << "  Op\n";

    // Разделяем строку на токены
    int count_words = 0;
    std::string* tokens = split(line, count_words);

    if (count_words < 3) {
        outputFile << "   ERROR: Invalid expression.\n";
        delete[] tokens; // Освобождаем память, если массив выделен динамически
        return;
    }

    // Первая часть — это переменная
    std::string left_var = tokens[0];
    std::string equal_sign = tokens[1];

    // Выводим левую часть
    outputFile << "   " << left_var << "  [Id]\n";
    outputFile << "   =  [Symbols_of_Operation]\n";
    outputFile << "   Expr\n";

    // Разбор правой части
    bool expectOperator = false; // Для чередования операндов и операторов
    for (int i = 2; i < count_words; ++i) {
        std::string token = tokens[i];

        if (expectOperator) {
            // Ожидаем оператор
            if (token == "+" || token == "-") {
                outputFile << "     " << token << "  [Symbols_of_Operation]\n";
                expectOperator = false; // Следующий токен должен быть операндом
            }
        }
        else {
            // Ожидаем операнд
            outputFile << "    SimpleExpr\n";
            if (isValidIdentifier(token))
                outputFile << "     " << token << "  [Id]\n";
            else
                outputFile << "     " << token << "  [const]\n";
            expectOperator = true; // Следующий токен должен быть оператором
        }
    }
}




void SintaksisAnalyzer::draw_operators_cycle(const std::string line) {
    outputFile << "    Operators\n";
    outputFile << "     Op\n";

    // Разделяем строку на левый и правый операнд
    size_t equal_pos = line.find('=');
    std::string left_var = line.substr(0, equal_pos); // Левая часть
    std::string right_expr = line.substr(equal_pos + 1); // Правая часть

    outputFile << "      " << left_var << "  [Id]\n";
    outputFile << "      =  [Symbols_of_Operation]\n";
    outputFile << "      Expr\n";

    // Найдем первый оператор + или -
    size_t operator_pos = right_expr.find_first_of("+-");

    if (operator_pos != std::string::npos) {
        // Левый операнд
        std::string left_operand = right_expr.substr(0, operator_pos - 1);
        // Оператор
        std::string operator_char = right_expr.substr(operator_pos, 1);
        // Правый операнд
        std::string right_operand = right_expr.substr(operator_pos + 2);

        // Выводим левый операнд
        outputFile << "       SimpleExpr\n";
        outputFile << "        " << left_operand << "  [Id]\n";
        // Выводим оператор
        outputFile << "        " << operator_char << "  [Symbols_of_Operation]\n";
        // Выводим правый операнд
        outputFile << "       SimpleExpr\n";
        outputFile << "        " << right_operand << "  [Id]\n";
    }
    else {
        // Если оператора нет, просто выводим один операнд
        outputFile << "       SimpleExpr\n";
        outputFile << "        " << right_expr << "  [Id]\n";
    }
}

void SintaksisAnalyzer::draw_expression(const std::string& expr, const std::string& indent) {
    std::string trimmed_expr = expr;
    trim(trimmed_expr); // Убираем пробелы

    outputFile << indent << "SimpleExpr\n";
    size_t operator_pos = trimmed_expr.find_first_of("+-");
    if (operator_pos == std::string::npos) {
        // Если нет операторов, это простое выражение
        outputFile << indent << " " << trimmed_expr << "  [Const]\n";
    }
    else {
        // Разбираем выражение с операторами
        std::string left = trimmed_expr.substr(0, operator_pos);
        std::string op = trimmed_expr.substr(operator_pos, 1);
        std::string right = trimmed_expr.substr(operator_pos + 1);

        trim(left);
        trim(right);

        outputFile << indent << " " << left << "  [Const]\n";
        outputFile << indent << " " << op << "  [Symbols_of_Operation]\n";
        draw_expression(right, indent + " ");
    }
}


void SintaksisAnalyzer::draw_cycle(const std::string line) {
    outputFile << " Operators\n";
    outputFile << "   Op\n";

    size_t for_pos = line.find("FOR");
    size_t to_pos = line.find("TO");
    size_t do_pos = line.find("DO");

    if (for_pos == std::string::npos || to_pos == std::string::npos || do_pos == std::string::npos) {
        outputFile << "Ошибка: ключевые слова FOR, TO или DO отсутствуют\n";
        return;
    }

    // Извлекаем идентификатор переменной
    size_t var_start = for_pos + 4; // Пропускаем "FOR "
    size_t var_end = line.find('=', var_start);
    std::string var = line.substr(var_start, var_end - var_start);
    trim(var); // Убираем лишние пробелы

    // Извлекаем начальное значение
    size_t from_start = var_end + 1; // Пропускаем '='
    size_t from_end = to_pos;        // До ключевого слова TO
    std::string from = line.substr(from_start, from_end - from_start);
    trim(from);

    // Извлекаем конечное значение или выражение
    size_t to_start = to_pos + 3; // Пропускаем "TO "
    size_t to_end = do_pos;       // До ключевого слова DO
    std::string to = line.substr(to_start, to_end - to_start);
    trim(to);

    // Извлекаем тело цикла
    size_t body_start = do_pos + 3; // Пропускаем "DO "
    std::string body = line.substr(body_start);
    trim(body);

    // Рисуем дерево
    outputFile << "    FOR  [WordsKey]\n";
    outputFile << "    " << var << "  [Id]\n";
    outputFile << "    =  [Symbols_of_Operation]\n";
    outputFile << "    Expr\n";
    outputFile << "     SimpleExpr\n";
    outputFile << "      " << from << "  [Const]\n";
    outputFile << "    TO  [WordsKey]\n";
    outputFile << "    Expr\n";
    draw_expression(to, "     "); // Используем функцию для разбора выражения
    outputFile << "    DO  [WordsKey]\n";
    outputFile << "    Operators\n";

    draw_operators_cycle(body); // Вызываем функцию для обработки тела цикла
}




void SintaksisAnalyzer::draw_start_program(const std::string line) {
    outputFile << "Program\n";
    outputFile << " Begin\n";

    size_t prog_pos = line.find("PROGRAM");
    std::string program_name = line.substr(prog_pos + 8); // Имя программы

    outputFile << "  PROGRAM  [WordsKey]\n";
    outputFile << "  " << program_name << "  [Id]\n";
}

void SintaksisAnalyzer::draw_end_program(const std::string line) {
    outputFile << " End\n";

    size_t end_pos = line.find("END");
    std::string program_name = line.substr(end_pos + 4); // Имя программы

    outputFile << "  END  [WordsKey]\n";
    outputFile << "  " << program_name << "  [Id]\n";
}

void SintaksisAnalyzer::draw_varlist(const std::string line) {
    size_t pos = 0;
    size_t comma_pos;

    // Разделение на переменные и вывод с учетом вложенности
    while ((comma_pos = line.find(',', pos)) != std::string::npos) {
        outputFile << "    " << line.substr(pos, comma_pos - pos) << "  [Id]\n";
        outputFile << "    ,  [Symbols_of_Separating]\n";
        outputFile << "   VarList\n";
        pos = comma_pos + 1;
    }

    // Последняя переменная, после последней запятой
    outputFile << "    " << line.substr(pos) << "  [Id]\n";
}

void SintaksisAnalyzer::draw_descriptions(const std::string line) {
    outputFile << " Descriptions\n";
    outputFile << "  Descr\n";

    size_t type_pos = line.find("INTEGER");
    std::string vars = line.substr(type_pos + 8); // Переменные после типа

    outputFile << "   Type\n";
    outputFile << "    INTEGER  [WordsKey]\n";

    // Вложенные Varlist
    outputFile << "   Varlist\n";
    draw_varlist(vars);
}





