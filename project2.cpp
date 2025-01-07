#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <zlib.h>
#include <cstring>

// Function to compress a chunk of data
void compressChunk(const char* inputData, size_t inputSize, std::vector<char>& outputData) {
    uLongf compressedSize = compressBound(inputSize); // Maximum compressed size
    outputData.resize(compressedSize);

    int ret = compress(reinterpret_cast<Bytef*>(outputData.data()), &compressedSize,
                        reinterpret_cast<const Bytef*>(inputData), inputSize);

    if (ret != Z_OK) {
        std::cerr << "Compression failed!" << std::endl;
        return;
    }

    outputData.resize(compressedSize); // Resize to actual compressed size
}

// Function to decompress a chunk of data
void decompressChunk(const char* inputData, size_t inputSize, std::vector<char>& outputData, size_t originalSize) {
    outputData.resize(originalSize);

    int ret = uncompress(reinterpret_cast<Bytef*>(outputData.data()), &originalSize,
                          reinterpret_cast<const Bytef*>(inputData), inputSize);

    if (ret != Z_OK) {
        std::cerr << "Decompression failed!" << std::endl;
        return;
    }

    outputData.resize(originalSize); // Resize to actual decompressed size
}

// Function to compress a file using multiple threads
void compressFile(const std::string& inputFile, const std::string& outputFile, int numThreads) {
    std::ifstream input(inputFile, std::ios::binary);
    if (!input.is_open()) {
        std::cerr << "Failed to open input file for compression!" << std::endl;
        return;
    }

    // Read file data into memory
    input.seekg(0, std::ios::end);
    size_t fileSize = input.tellg();
    input.seekg(0, std::ios::beg);
    std::vector<char> fileData(fileSize);
    input.read(fileData.data(), fileSize);
    input.close();

    // Split the data into chunks
    size_t chunkSize = fileSize / numThreads;
    std::vector<std::thread> threads;
    std::vector<std::vector<char>> compressedChunks(numThreads);

    // Start threads to compress each chunk
    for (int i = 0; i < numThreads; ++i) {
        size_t start = i * chunkSize;
        size_t end = (i == numThreads - 1) ? fileSize : (i + 1) * chunkSize;
        size_t length = end - start;

        threads.push_back(std::thread([start, length, &fileData, &compressedChunks, i]() {
            compressChunk(fileData.data() + start, length, compressedChunks[i]);
        }));
    }

    // Join all threads
    for (auto& th : threads) {
        th.join();
    }

    // Write compressed data to output file
    std::ofstream output(outputFile, std::ios::binary);
    if (!output.is_open()) {
        std::cerr << "Failed to open output file for compression!" << std::endl;
        return;
    }

    for (const auto& chunk : compressedChunks) {
        output.write(chunk.data(), chunk.size());
    }

    output.close();
    std::cout << "Compression completed successfully." << std::endl;
}

// Function to decompress a file using multiple threads
void decompressFile(const std::string& inputFile, const std::string& outputFile, int numThreads) {
    std::ifstream input(inputFile, std::ios::binary);
    if (!input.is_open()) {
        std::cerr << "Failed to open input file for decompression!" << std::endl;
        return;
    }

    // Read compressed data into memory
    input.seekg(0, std::ios::end);
    size_t fileSize = input.tellg();
    input.seekg(0, std::ios::beg);
    std::vector<char> compressedData(fileSize);
    input.read(compressedData.data(), fileSize);
    input.close();

    // Assuming the original size is known or can be deduced.
    size_t originalSize = fileSize * 10; // Example, replace with actual size if known

    // Split the compressed data into chunks
    size_t chunkSize = fileSize / numThreads;
    std::vector<std::thread> threads;
    std::vector<std::vector<char>> decompressedChunks(numThreads);

    // Start threads to decompress each chunk
    for (int i = 0; i < numThreads; ++i) {
        size_t start = i * chunkSize;
        size_t end = (i == numThreads - 1) ? fileSize : (i + 1) * chunkSize;
        size_t length = end - start;

        threads.push_back(std::thread([start, length, &compressedData, &decompressedChunks, i, originalSize]() {
            decompressChunk(compressedData.data() + start, length, decompressedChunks[i], originalSize);
        }));
    }

    // Join all threads
    for (auto& th : threads) {
        th.join();
    }

    // Write decompressed data to output file
    std::ofstream output(outputFile, std::ios::binary);
    if (!output.is_open()) {
        std::cerr << "Failed to open output file for decompression!" << std::endl;
        return;
    }

    for (const auto& chunk : decompressedChunks) {
        output.write(chunk.data(), chunk.size());
    }

    output.close();
    std::cout << "Decompression completed successfully." << std::endl;
}

int main() {
    std::string inputFile = "example.txt";    // File to compress/decompress
    std::string outputFile = "example.gz";    // Output compressed file
    int numThreads = 4;  // Number of threads for parallel processing

    int choice;
    std::cout << "1. Compress file\n2. Decompress file\nEnter choice: ";
    std::cin >> choice;

    if (choice == 1) {
        compressFile(inputFile, outputFile, numThreads);
    } else if (choice == 2) {
        decompressFile(outputFile, "decompressed.txt", numThreads);
    } else {
        std::cerr << "Invalid choice.\n";
    }

    return 0;
}
