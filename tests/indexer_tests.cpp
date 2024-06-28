#include <gtest/gtest.h>

#include "indexer/indexer.cpp"

TEST(IndexerTests, IndexerTest1) {
    Indexer indexer;
    indexer.SetIndexSource("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/");
    indexer.SetDirectory("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/test_directory/");
    indexer.ProcessDirectory();
    indexer.CreateIndex();

    fs::path file_path = "C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/index.idx";
    std::ifstream file(file_path, std::ios::in);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open a file... :(\n");
    }
    std::size_t amount_of_documents;
    file >> amount_of_documents;
    std::vector<std::size_t> documents_words_count;
    std::vector<fs::path> documents_paths;
    for (int i = 0; i < amount_of_documents; ++i) {
        std::size_t temp;
        file >> temp;
        fs::path document_path;
        file >> document_path;
        documents_paths.push_back(document_path);
        std::size_t words_count;
        file >> words_count;
        documents_words_count.push_back(words_count);
    }

    std::map<std::string, std::vector<std::pair<std::size_t, std::size_t>>> index_;
    while (!file.eof()) {
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream StringStream(line);
            std::string term;
            std::size_t document_id;
            std::size_t string_num;
            StringStream >> term;
            while (StringStream >> document_id >> string_num) {
                index_[term].emplace_back(document_id, string_num);
            }
        }
    }

    ASSERT_EQ(amount_of_documents, 2);
    ASSERT_EQ(documents_words_count[0], 5);
    ASSERT_EQ(index_["word1"].size(), 1);
    ASSERT_EQ(index_["word2"].size(), 1);
    ASSERT_EQ(index_["word3"].size(), 1);
    ASSERT_EQ(index_["another"].size(), 1);
    ASSERT_EQ(index_["one"].size(), 1);
    ASSERT_EQ(index_["another"][0].first, 0);
    ASSERT_EQ(index_["one"][0].second, 2);
    ASSERT_EQ(index_["word1"][0].first, 0);
    ASSERT_EQ(index_["word1"][0].second, 1);
}