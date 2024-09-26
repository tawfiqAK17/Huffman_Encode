#include "HuffmanDecode.h"
#include <cstdlib>
#include <iostream>


HuffmanDecode::Node::~Node() {
    delete left;
    delete right;
}

HuffmanDecode::~HuffmanDecode() {
    delete tree_root;
}
void HuffmanDecode::Decode(std::ifstream& source, std::ofstream& destination) {
    char buff[1024];
    std::string previous_buff_rest = ""; // the bits in the previous buff that are not a complete path to a character in the tree
    int useless_bits_num = source.get(); // get the number of useless bits at the end of the file
    LoadTree(source);
    long decode_buff = 0;
    long forloop = 0;
    while (source.read(buff, 1024) || source.gcount() > 0) {
        auto start = std::chrono::high_resolution_clock::now();
        std::streamsize bytesRead = source.gcount();
        std::string buff_as_bits = previous_buff_rest; // the buffer bits represented as string
        for (std::streamsize i = 0; i < bytesRead; i++) {
            std::bitset<8> bits(buff[i]);
            buff_as_bits += bits.to_string();
        }
        auto end = std::chrono::high_resolution_clock::now();
        forloop += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        
        if (source.eof())
            buff_as_bits.erase(buff_as_bits.size() - useless_bits_num, useless_bits_num); // removing the useless bits at the end
        start = std::chrono::high_resolution_clock::now();
        previous_buff_rest = DecodeBuffer(destination, buff_as_bits);
        end = std::chrono::high_resolution_clock::now();
        decode_buff += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }
}

void HuffmanDecode::LoadTree(std::ifstream& source) {
    // getting the tree size
    short tree_representation_size = source.get();
    tree_representation_size <<= 8;
    tree_representation_size |= source.get();
    std::string tree_representation;
    for (int i = 0; i < tree_representation_size; i++) {
        tree_representation += std::bitset<8>(source.get()).to_string();
    }
    int idx = 0;
    tree_root = RecreatTree(tree_representation, idx); 
}


HuffmanDecode::Node * HuffmanDecode::RecreatTree(std::string& binary_representation, int& idx) {
    if (idx >= binary_representation.size()) {
        return nullptr;
    }
    if (binary_representation[idx] == '0') {
        auto node = new Node();
        node->content.reset();
        idx++;
        node->left = RecreatTree(binary_representation, idx);
        node->right = RecreatTree(binary_representation, idx);
        return node;
    }
    
    auto node = new Node();
    char c = std::bitset<8>(binary_representation.substr(idx + 1, 8)).to_ulong();
    node->content = c;
    idx += 9;
    return node; 
}

std::string HuffmanDecode::DecodeBuffer(std::ofstream& destination, std::string& binary_representation) {
    auto parser = tree_root;
    std::string line;
    std::string path;
    for (const char c : binary_representation) {
        path += c;
        if (c == '0')
            parser = parser->left; // 0 means left
        if (c == '1')
            parser = parser->right; // 1 means right
        if (parser->content.has_value()) {
            line += parser->content.value();
            parser = tree_root;
            path = "";
        }
    }
    destination << line;
    return path; // that wile be a path that the parser takes but doesn't find a value so it will be included in the next time
}
