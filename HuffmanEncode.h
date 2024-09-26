#pragma once

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

class HuffmanEncode{
private:
    struct Node {
        Node();
        ~Node();
        Node(char c, int count);
        std::optional<char> content; 
        Node *left = nullptr;
        Node *right = nullptr;
        int count = 0;
        bool operator<(const Node& lhs) const {
            return count > lhs.count; // I want them in the reverse order
        }
    };
    // a functor to compare the nodes via their pointers
    struct ComparePointers {
        bool operator()(const Node* lhs, const Node* rhs) {
            return *lhs < *rhs;
        }
    };
public:
    HuffmanEncode();
    ~HuffmanEncode();
    HuffmanEncode(HuffmanEncode &&) = delete;
    HuffmanEncode(const HuffmanEncode &) = delete;
    HuffmanEncode &operator=(HuffmanEncode &&) = delete;
    HuffmanEncode &operator=(const HuffmanEncode &) = delete;

    void Encode(std::ifstream& source, std::ofstream& destination);
private:
    
    void CountCharacters(std::ifstream& source); 
    void GenerateTree(); 
    void FillCharEncode(Node *root, std::string encode);
    void WriteCompressedData(std::ifstream& source, std::ofstream& destination);
    void WriteTree(std::ofstream& destination);
    std::string GetTreeRepresentation();
    void PreorderTraversal(Node *root, std::string& tree_representation);
    std::string WriteBits(std::ofstream& destination, std::string& binary_representation); 
private:
    std::priority_queue<Node*, std::vector<Node*>, ComparePointers> queue;
    Node *tree_root = nullptr;
    std::vector<std::string> charEncode;
};
