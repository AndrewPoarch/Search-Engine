#include <windows.h>
#include <chrono>

#include "indexer.cpp"

int main(int argc, char** argv) {
    Indexer indexer;
    for (int i = 1; i < argc; ++i) {
        if (std::string_view(argv[i]) == "-d") {
            indexer.SetDirectory(argv[i + 1]);
        }
        if (std::string_view(argv[i]) == "-s") {
            indexer.SetIndexSource(argv[i + 1]);
        }
    }
    std::cout << "\nIndexing directory...\n";
    auto start = std::chrono::high_resolution_clock::now();
    indexer.ProcessDirectory();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "\nProcessed directory in " << duration << "\n";

    start = std::chrono::high_resolution_clock::now();
    indexer.CreateIndex();
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "\nCreated inverted index in " << duration << "\n";

    return 0;
}
