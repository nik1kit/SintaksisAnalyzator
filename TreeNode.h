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
    std::string type; // ��� ����, ��������, "WordsKey", "Id", "Symbols_of_Operation"
    std::vector<TreeNode*> children;
    int level;

public:
    // �����������
    TreeNode(const std::string& nodeName, int nodeLevel, const std::string& nodeType = "")
        : data(nodeName), level(nodeLevel), type(nodeType) {}

    // ���������� ��� ����������� ������������ ������
    ~TreeNode() {
        for (TreeNode* child : children) {
            delete child;
        }
    }

    // ����� ���������� ��������� ����
    TreeNode* addSon(const std::string& nodeName, int nodeLevel, const std::string& nodeType = "") {
        TreeNode* newNode = new TreeNode(nodeName, nodeLevel, nodeType);
        children.push_back(newNode);
        return newNode;
    }

    // ��������� ������ ����
    std::string getData() const { return data; }

    // ��������� ���� ����
    std::string getType() const { return type; }

    // ��������� �������� �����
    const std::vector<TreeNode*>& getChildren() const { return children; }

    // ��������� ������ ����
    int getLevel() const { return level; }

    int getMaxLevel(const TreeNode* node, int currentLevel = 1) const {
        int maxLevel = currentLevel;  // ���������� ������� ������� �������� ������������
        // �������� �� ���� �������� ����� � ���� ������������ �������
        for (const TreeNode* child : node->children) {
            int childLevel = getMaxLevel(child, currentLevel + 1);
            maxLevel = std::max(maxLevel, childLevel);  // ��������� ������������ �������
        }
        return maxLevel;
    }


    // ����� ��� ������ ������

    void printTree(std::ofstream& outFile, int maxLevel, int currentLevel = 1, int indentation = 0) const {
        // ���� ������� ������� ������ maxLevel, ���������� �����
        if (currentLevel > maxLevel) {
            return;
        }

        // �������� �������
        for (int i = 0; i < indentation; ++i) outFile << " ";

        // �������� ������ ����
        outFile << data;
        if (!type.empty()) outFile << " [" << type << "]";
        outFile << std::endl;

        // ���������� �������� ����� ������ �� maxLevel
        for (const TreeNode* child : children) {
            child->printTree(outFile, maxLevel, currentLevel + 1, indentation + 2);
        }
    }


    void printSpecificNode(std::ofstream& outFile, const std::string& nodeName, int indentation = 0) const {
        // ���� ��� �������� ���� ��������� � �������
        if (data == nodeName) {
            // ������� ������� ���� � ��� ���������
            this->printTree(outFile, indentation);
            return;
        }

        // ���������� ���� � �������� �����
        for (const TreeNode* child : children) {
            child->printSpecificNode(outFile, nodeName, indentation);
        }
    }







    static std::vector<std::string> splitByEqualSign(const std::string& line) {
        std::vector<std::string> expressions;
        std::vector<size_t> equal_positions;

        // ������� ��� ������� �������� '=' � ������
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] == '=') {
                equal_positions.push_back(i);
            }
        }

        // ���� � ������ ������ ���� ���� '=', �� ���������� ��� ������ ��� ���� ���������
        if (equal_positions.size() == 1) {
            expressions.push_back(line);
            return expressions;
        }

        // ��������� ������ �� ��������� �� �������� '='
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
                // ���������� ������ ������� (���� ����) � ��������������� ����
            }
        }
    }
};

#endif // TREENODE_H
