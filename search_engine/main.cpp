#include <conio.h>
#include <chrono>

#include "engine.cpp"

int main(int argc, char** argv) {
    Searcher searcher;
    for (int i = 1; i < argc; ++i) {
        if (std::string_view(argv[i]) == "-s") {
            searcher.SetIndexSource(argv[i + 1]);
        }
    }
    auto recover_start = std::chrono::high_resolution_clock::now();
    searcher.Recover();
    auto recover_end = std::chrono::high_resolution_clock::now();
    auto recover_duration = std::chrono::duration_cast<std::chrono::milliseconds>(recover_end - recover_start);
    std::cout << "\nRecovered in " << recover_duration << "\n\n";
    std::cout << "Ready to search!\n\n Press 'Ctrl + C' to exit.\n\n";
    while (true) {
        std::string line;
        std::getline(std::cin, line);
        auto start = std::chrono::high_resolution_clock::now();
        auto tokens = searcher.Tokenize(line);
        if (!searcher.IsValid(tokens)) {
            std::cerr << "\nRequest is not correct....\n";
            continue;
        }
        std::cout << std::endl;
        auto suitable_docs = searcher.GetDocsScore(tokens);
        if (suitable_docs.empty()) {
            std::cerr << "Not found...:(\n\n";
            continue;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "Found in documents:\n\n";
        for (const auto& doc : suitable_docs) {
            std::cout << "File name:\n\n\t";
            std::cout << searcher.GetFileName(doc.first.first) << '\n';
            std::cout << "\n";
            std::cout << "In line numbers: \n\n\t";
            for (const auto& ln : doc.first.second) {
                std::cout << ln << "  ";
            }

            std::cout << "\n\n";
        }
        std::cout << "\nProcessed in " << duration << "\n";
        std::cout << std::endl;
    }
}
