#pragma once

#include <cstddef>
#include <unistd.h>
#include <fstream>
#include <optional>
#include <memory>
#include <queue>
#include <fstream>
#include <cstdint>
#include <string>
#include <optional>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <bitset>
#include <unordered_set>

class HuffmanDecode {
private:
    struct Node {
        Node() = default;
        ~Node();
        std::optional<char> content;
        Node *left = nullptr;
        Node *right = nullptr;
    };

public:
    HuffmanDecode() = default;
    ~HuffmanDecode();
    void Decode(std::ifstream& source, std::ofstream& destination);
private:
    void LoadTree(std::ifstream& source);
    Node *RecreatTree(std::string& binary_representation, int& idx);
    std::string DecodeBuffer(std::ofstream& destination, std::string& binary_representation);

private:
    Node *tree_root = nullptr;
};
