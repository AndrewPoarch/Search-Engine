#include <gtest/gtest.h>

#include "search_engine/engine.cpp"

TEST(EngineTests, SearcherTest1) {
    Indexer indexer;
    indexer.SetIndexSource("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/");
    indexer.SetDirectory("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/test_directory/");
    indexer.ProcessDirectory();
    indexer.CreateIndex();
    Searcher searcher;
    searcher.SetIndexSource("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/");
    searcher.Recover();
    std::string line = "(word1 AND word2)";
    auto tokens = searcher.Tokenize(line);
    auto suitable_docs = searcher.GetDocsScore(tokens);

    ASSERT_EQ(tokens.size(), 5);
    ASSERT_TRUE(!suitable_docs.empty());
    ASSERT_EQ(suitable_docs.size(), 1);
}

TEST(EngineTests, SearcherTest2) {
    Indexer indexer;
    indexer.SetIndexSource("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/");
    indexer.SetDirectory("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/test_directory/");
    indexer.ProcessDirectory();
    indexer.CreateIndex();
    Searcher searcher;
    searcher.SetIndexSource("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/");
    searcher.Recover();
    std::string line = "(word1 AND word2) OR (one)";
    auto tokens = searcher.Tokenize(line);
    auto suitable_docs = searcher.GetDocsScore(tokens);

    ASSERT_TRUE(!suitable_docs.empty());
    ASSERT_EQ(suitable_docs.size(), 2);
    ASSERT_EQ(suitable_docs[0].first.first, 1);
    ASSERT_TRUE(suitable_docs[0].second < suitable_docs[1].second);
}

TEST(EngineTests, Lev) {
    Indexer indexer;
    indexer.SetIndexSource("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/");
    indexer.SetDirectory("C:/Users/Andreas/Desktop/warandpeace/");
    indexer.ProcessDirectory();
    indexer.CreateIndex();
    Searcher searcher;
    searcher.SetIndexSource("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/");
    searcher.Recover();
    std::string line = "horse AND house";
    auto tokens = searcher.Tokenize(line);
    auto suitable_docs = searcher.GetDocsScore(tokens);

    ASSERT_TRUE(!suitable_docs.empty());
    ASSERT_EQ(suitable_docs.size(), 2);
    ASSERT_EQ(suitable_docs[0].first.first, 1);
    ASSERT_TRUE(suitable_docs[0].second < suitable_docs[1].second);
}

TEST(EngineTests, EmptyTest) {
    Indexer indexer;
    indexer.SetIndexSource("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/");
    indexer.SetDirectory("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/test_directory/");
    indexer.ProcessDirectory();
    indexer.CreateIndex();
    Searcher searcher;
    searcher.SetIndexSource("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/");
    searcher.Recover();
    std::string line = "(garden OR peace)";
    auto tokens = searcher.Tokenize(line);
    auto suitable_docs = searcher.GetDocsScore(tokens);

    ASSERT_TRUE(suitable_docs.empty());
}

TEST(EngineTests, EmptyTest1) {
    Indexer indexer;
    indexer.SetIndexSource("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/");
    indexer.SetDirectory("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/test_directory/");
    indexer.ProcessDirectory();
    indexer.CreateIndex();
    Searcher searcher;
    searcher.SetIndexSource("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/");
    searcher.Recover();
    std::string line = "(a AND word2)";
    auto tokens = searcher.Tokenize(line);
    auto suitable_docs = searcher.GetDocsScore(tokens);

    ASSERT_TRUE(suitable_docs.empty());
}

TEST(EngineTests, NotEmpty) {
    Indexer indexer;
    indexer.SetIndexSource("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/");
    indexer.SetDirectory("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/test_directory/");
    indexer.ProcessDirectory();
    indexer.CreateIndex();
    Searcher searcher;
    searcher.SetIndexSource("C:/Users/Andreas/CLionProjects/labwork11-AndrewPoarch/tests/");
    searcher.Recover();
    std::string line = "(a OR word2)";
    auto tokens = searcher.Tokenize(line);
    auto suitable_docs = searcher.GetDocsScore(tokens);

    ASSERT_TRUE(!suitable_docs.empty());
    ASSERT_EQ(suitable_docs.size(), 2);
    ASSERT_EQ(suitable_docs[0].first.first, 0);
    ASSERT_TRUE(suitable_docs[0].second < suitable_docs[1].second);
}
