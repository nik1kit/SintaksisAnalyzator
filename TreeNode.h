#ifndef TREENODE_H
#define TREENODE_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

class TreeNode {
private:
    std::string data;
    std::string type; // Тип узла, например, "WordsKey", "Id", "Symbols_of_Operation"
    std::vector<TreeNode*> children;
    int level;

public:
    // Конструктор
    TreeNode(const std::string& nodeName, int nodeLevel, const std::string& nodeType = "")
        : data(nodeName), level(nodeLevel), type(nodeType) {}

    // Деструктор для корректного освобождения памяти
    ~TreeNode() {
        for (TreeNode* child : children) {
            delete child;
        }
    }

    // Метод добавления дочернего узла
    TreeNode* addSon(const std::string& nodeName, int nodeLevel, const std::string& nodeType = "") {
        TreeNode* newNode = new TreeNode(nodeName, nodeLevel, nodeType);
        children.push_back(newNode);
        return newNode;
    }

    // Получение данных узла
    std::string getData() const { return data; }

    // Получение типа узла
    std::string getType() const { return type; }

    // Получение дочерних узлов
    const std::vector<TreeNode*>& getChildren() const { return children; }

    // Получение уровня узла
    int getLevel() const { return level; }

    int getMaxLevel(const TreeNode* node, int currentLevel = 1) const {
        int maxLevel = currentLevel;  // Изначально текущий уровень является максимальным
        // Проходим по всем дочерним узлам и ищем максимальный уровень
        for (const TreeNode* child : node->children) {
            int childLevel = getMaxLevel(child, currentLevel + 1);
            maxLevel = std::max(maxLevel, childLevel);  // Обновляем максимальный уровень
        }
        return maxLevel;
    }


    // Метод для вывода дерева

    void printTree(std::ofstream& outFile, int maxLevel, int currentLevel = 1, int indentation = 0) const {
        // Если текущий уровень больше maxLevel, прекращаем вывод
        if (currentLevel > maxLevel) {
            return;
        }

        // Печатаем отступы
        for (int i = 0; i < indentation; ++i) outFile << " ";

        // Печатаем данные узла
        outFile << data;
        if (!type.empty()) outFile << " [" << type << "]";
        outFile << std::endl;

        // Рекурсивно печатаем детей только до maxLevel
        for (const TreeNode* child : children) {
            child->printTree(outFile, maxLevel, currentLevel + 1, indentation + 2);
        }
    }


    void printSpecificNode(std::ofstream& outFile, const std::string& nodeName, int indentation = 0) const {
        // Если имя текущего узла совпадает с искомым
        if (data == nodeName) {
            // Выводим текущий узел и его поддерево
            this->printTree(outFile, indentation);
            return;
        }

        // Рекурсивно ищем в дочерних узлах
        for (const TreeNode* child : children) {
            child->printSpecificNode(outFile, nodeName, indentation);
        }
    }







    static std::vector<std::string> splitByEqualSign(const std::string& line) {
        std::vector<std::string> expressions;
        std::vector<size_t> equal_positions;

        // Находим все позиции символов '=' в строке
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] == '=') {
                equal_positions.push_back(i);
            }
        }

        // Если в строке только один знак '=', то возвращаем всю строку как одно выражение
        if (equal_positions.size() == 1) {
            expressions.push_back(line);
            return expressions;
        }

        // Разбиваем строку на выражения по позициям '='
        size_t start_pos = 0;
        for (size_t i = 0; i < equal_positions.size(); ++i) {
            size_t end_pos = (i + 1 < equal_positions.size()) ? equal_positions[i + 1] - 1 : line.size();
            expressions.push_back(line.substr(start_pos, end_pos - start_pos));
            start_pos = end_pos;
        }

        return expressions;
    }

    static void processForToDo(TreeNode* forNode, const std::string& line, int level) {
        std::istringstream lineStream(line);
        std::string token;

        while (lineStream >> token) {
            if (token == "TO") {
                forNode->addSon(token, level, "WordsKey");
            }
            else if (token == "DO") {
                forNode->addSon(token, level, "WordsKey");
                TreeNode* nestedCycleNode = forNode->addSon("NestedCycle", level);
                if (!nestedCycleNode) return;
                nestedCycleNode->addSon("Operators", level + 1);
            }
            else {
                // Добавление других токенов (если есть) в соответствующие узлы
            }
        }
    }
};

#endif // TREENODE_H
