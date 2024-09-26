#include "HuffmanEncode.h"
#include <ios>
#include <iostream>
#include <string>

HuffmanEncode::Node::Node() {}
HuffmanEncode::Node::Node(char c, int count):content(c), count(count) {}
HuffmanEncode::Node::~Node() {
    delete left;
    delete right;
}

HuffmanEncode::HuffmanEncode() {
    charEncode.reserve(255);
    for (int i = 0; i < 255; i++) {
        charEncode.push_back("");
    }
}
HuffmanEncode::~HuffmanEncode() {
    delete tree_root;
}

void HuffmanEncode::Encode(std::ifstream& source, std::ofstream& destination) {
    CountCharacters(source); // count each character in the source and create a node for each one 
    GenerateTree(); // generate the tree based on the primary nodes in the queue field 
    FillCharEncode(tree_root, ""); // fill the charEncode map
    WriteCompressedData(source, destination); // write the compressed version o the file to destination
}

void HuffmanEncode::CountCharacters(std::ifstream& source) {
    char buff[1024];
    int hashSet[255] = {};
    // calculating the count of each character in the file (hashSet[c] = count of c in the file)
    while (source.read(buff, 1024) || source.gcount() > 0) {
        for (int i = 0; i < source.gcount(); i++) {
            hashSet[buff[i]]++; 
        }
    }
    // constructing primary nodes and push them to the priority queue
    for (int i = 0; i < 255; i++) {
        if (hashSet[i] != 0) {
            queue.push(new Node((char)i, hashSet[i]));
        }
    }
}

void HuffmanEncode::GenerateTree() {
    while (queue.size() != 1) { // the last node will be the root
        auto new_node = new Node(); 
        new_node->left = queue.top();
        queue.pop();
        new_node->right = queue.top();
        queue.pop();
        new_node->count = new_node->left->count + new_node->right->count;
        queue.push(new_node);
    }
    tree_root = queue.top();
    queue.pop(); // so the priority queue stay empty 
}



void HuffmanEncode::FillCharEncode(Node *root, std::string encode) {
    if (root->content.has_value()) {
        charEncode[root->content.value()] = encode;
    }
    if (root->left)
        FillCharEncode(root->left, encode + "0"); // 0 means left
    if (root->right)
        FillCharEncode(root->right, encode + "1"); // 1 means right
}

void HuffmanEncode::WriteTree(std::ofstream& destination) {
    std::string tree_representation = GetTreeRepresentation();
    short tree_size = (tree_representation.size() + 7) / 8; // the number of bytes in the tree representation
    std::string tree_size_representation = std::bitset<16>(tree_size).to_string();
    WriteBits(destination, tree_size_representation); 
    std::string not_writen_bits = WriteBits(destination, tree_representation);
    if (!not_writen_bits.empty()) {
        int useless_bits_num =  8 - not_writen_bits.size();
        not_writen_bits += std::string(useless_bits_num, '0'); // Pad with zeros to make it 8 bits
        std::bitset<8> last_bits(not_writen_bits);
        char c = static_cast<char>(last_bits.to_ulong());
        destination.write(&c, sizeof(c));
    } 
}

std::string HuffmanEncode::GetTreeRepresentation() {
    std::string tree_representation;
    PreorderTraversal(tree_root, tree_representation);
    return tree_representation;
}

void HuffmanEncode::PreorderTraversal(Node *root, std::string& tree_representation) {
    if (root->content.has_value()) { // leaf node
        tree_representation += '1' + std::bitset<8>(root->content.value()).to_string();
        return;
    }
    tree_representation += '0';
    PreorderTraversal(root->left, tree_representation);
    PreorderTraversal(root->right, tree_representation);
}

std::string HuffmanEncode::WriteBits(std::ofstream& destination, std::string& binary_representation) {
    size_t size = binary_representation.size();
    std::string line;
    line.reserve(size / 8);
    // taking every 8 bits convert it to char and write it to the destination file 
    for (int i = 0; i <= size - 8; i += 8) {
        char value = 0;
        for (int j = 0; j < 8; ++j) {
            value <<= 1;  // Shift left by 1 bit
            if (binary_representation[i + j] == '1') {
                value |= 1;  // Set the least significant bit to 1
            }
        }
        char c = value;
        line += c;
    }
    destination.write(line.c_str(), line.size());
    if (size % 8 != 0)
        return binary_representation.substr(size - size % 8, size % 8); // returning the bits (as string) that can not be encoded
    return "";
}

void HuffmanEncode::WriteCompressedData(std::ifstream& source, std::ofstream& destination) {
    char buff[32768];
    source.clear(); // clear the EOF state
    source.seekg(0); // move the file pointer to the beginning of the file
    char c = ' ';
    destination.write(&c, sizeof(char)); // reserving one byte to indicate the number of bits that will be 
                                        // ignored in the end of the file when decompressing
    WriteTree(destination); // write the tree representation
    std::string binary_representation_1; // the binary representation of the buffer when encoded
    while (source.read(buff, 32768) || source.gcount() > 0) {
        std::string binary_representation_2; // the binary representation of the buffer when encoded
        std::streamsize bytes_read = source.gcount();
        int buff_half = bytes_read / 2;
        binary_representation_1.reserve(bytes_read * 5); // reserving a reasonable amount of memory
        binary_representation_2.reserve(buff_half * 5);
        // generating the binary representation
        std::thread th1([&]() {
            for (int i = 0; i < buff_half; i++) {
                binary_representation_1 += charEncode[buff[i]];
            }
        });
        std::thread th2([&]() {
            for (int i = buff_half; i < bytes_read; i++) {
                binary_representation_2 += charEncode[buff[i]];
            }
        });
        th1.join();
        th2.join();
        binary_representation_1 += binary_representation_2;
        binary_representation_1 = WriteBits(destination, binary_representation_1);
    }
    // Writing the remaining bits as the last byte (if any)
    if (!binary_representation_1.empty()) {
        int useless_bits_num =  8 - binary_representation_1.size();
        binary_representation_1 += std::string(useless_bits_num, '0'); // Pad with zeros to make it 8 bits
        std::bitset<8> last_bits(binary_representation_1);
        char c = static_cast<char>(last_bits.to_ulong());
        destination.write(&c, sizeof(c));
        destination.seekp(0, std::ios::beg); // moving the write pointer to the beginning of the file
        c = useless_bits_num;
        destination.write(&c, sizeof(char)); // indicating the number of bits that will be ignored at the end of the file 
    } else { // if there is no useless bits write 0 in place
        char c = '0';
        destination.seekp(0, std::ios::beg);
        destination.write(&c, sizeof(char));
    }
}


