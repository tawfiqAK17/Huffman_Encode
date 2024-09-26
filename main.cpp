#include "HuffmanEncode.h"
#include "HuffmanDecode.h"
#include <fstream>
#include <iostream>
#include <cstring>
int main (int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "invalide arguments" << std::endl;
        std::cerr << "usage: compressor <file type> <compress/decompress> <source> <optional: destination>" << std::endl;
        return -1;
    }
    
    // if compressing a text
    if (strcmp(argv[1], "txt") == 0) {
        std::ifstream source(argv[3], std::ios::binary); // open the source file in read mode
        if (!source.is_open()) {
            std::cerr << "unable to open " << argv[3] << std::endl;
            return -1;
        }
        std::ofstream destination;
        // if the user specify the output file
        if (argc == 5) {
            destination.open(argv[4]);
            if (!destination.is_open()) {
                std::cerr << "unable to open " << argv[4] << std::endl;
                return -1;
            }
        }
        // if the user doesn't specify the output file 
        else {
            destination.open(std::string(argv[2]) + ".out");
            if (!destination.is_open()) {
                std::cerr << "unable creat the output file \"" << argv[2] << "\" " << std::endl;
                return -1;
            }
        }
        
        if (strcmp(argv[2], "compress") == 0) {
            std::cout << "hint: if the file is too small the compressed version of it might be larger in size due to the information needed for decoding it!" << std::endl;
            HuffmanEncode huffmanEncode;
            auto start = std::chrono::high_resolution_clock::now();
            huffmanEncode.Encode(source, destination);
            destination.close();
            source.close();
            auto end = std::chrono::high_resolution_clock::now();
            std::cout << "time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
        }
        if (strcmp(argv[2], "decompress") == 0) {
            HuffmanDecode huffmanDecode;
            auto start = std::chrono::high_resolution_clock::now();
            huffmanDecode.Decode(source, destination);
            destination.close();
            source.close();
            auto end = std::chrono::high_resolution_clock::now();
            std::cout << "time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms"<< std::endl;
        }
    }

}
