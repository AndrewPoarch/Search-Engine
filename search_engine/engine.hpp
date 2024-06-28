#pragma once

#include "../indexer/indexer.hpp"
#include "../indexer/indexer.cpp"

#include <algorithm>
#include <stack>
#include <set>
#include <unordered_set>
#include <unordered_map>

class Searcher {
  private:
    const std::string trash_str = "%^#%^#%^**(&^)(*_+(*&)*&^";
    fs::path index_path_;
    std::vector<std::size_t> documents_words_count_;
    std::vector<fs::path> documents_paths_;
    std::map<std::string, std::vector<std::pair<std::size_t, std::size_t>>> index_;

    std::size_t GetAmountOfWords(std::size_t document_id);

    std::size_t GetAverageLengthOfDoc();

    static bool IsOperator(const std::string& token);

    static bool IsTerm(const std::string& token);

    auto GetOrScore(const std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>>& first,
                    const std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>>& second,
                    const std::string& term1,
                    const std::string& term2);

    auto GetAndScore(const std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>>& first,
                     const std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>>& second,
                     const std::string& term1,
                     const std::string& term2);

    auto GetScore(const std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>>& first,
                  const std::string& term1);

    void PushToTermsStack(std::stack<std::string>& terms,
                          std::stack<std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>,
                                                           double>>>& metrics,
                          std::stack<std::string>& operators);

    static std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>> PairToTriple(const std::vector<
        std::pair<std::size_t, std::size_t>>& post_list);

    double GetDocScore(const std::string& term, std::size_t document_id);
  public:
    void SetIndexSource(const fs::path& path);

    void Recover();

    std::vector<std::string> Tokenize(const std::string& input);

    bool IsValid(const std::vector<std::string>& tokens) const;

    auto GetDocsScore(const std::vector<std::string>& tokens);

    [[nodiscard]] auto GetFileName(std::size_t doc_id) const;
};
