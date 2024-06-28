#pragma once

#include "engine.hpp"

void Searcher::SetIndexSource(const fs::path& path) {
    index_path_ = path;
}

void Searcher::Recover() {
    fs::path file_path = index_path_ / "index.idx";

    std::ifstream file(file_path, std::ios::in);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open a file... :(\n");
    }
    std::size_t amount_of_documents;
    file >> amount_of_documents;
    for (int i = 0; i < amount_of_documents; ++i) {
        std::size_t temp;
        file >> temp;
        fs::path document_path;
        file >> document_path;
        documents_paths_.push_back(document_path);
        std::size_t words_count;
        file >> words_count;
        documents_words_count_.push_back(words_count);
    }

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
}

std::size_t Searcher::GetAmountOfWords(std::size_t document_id) {
    if (document_id >= documents_words_count_.size()) {
        throw std::logic_error("Document id more than size of words count vector...\n");
    }

    return documents_words_count_[document_id];
}

std::size_t Searcher::GetAverageLengthOfDoc() {
    std::size_t sum = 0;
    for (const auto& el : documents_words_count_) {
        sum += el;
    }

    return sum / documents_words_count_.size();
}

double Searcher::GetDocScore(const std::string& term, std::size_t document_id) {
    auto term_docs = index_[term];
    std::set<std::size_t> docs;
    for (const auto& pair : term_docs) {
        docs.insert(pair.first);
    }
    auto doc_frequency = (double) docs.size();
    double term_freq = 0;
    for (const auto& pair : term_docs) {
        if (pair.first == document_id) {
            ++term_freq;
        }
    };
    auto k = 1.2;
    auto b = 0.75;
    auto amount_of_docs = (double) documents_words_count_.size();
    auto document_length = (double) GetAmountOfWords(document_id);
    auto average_doc_size = (double) GetAverageLengthOfDoc();

    auto idf = std::log(((amount_of_docs - doc_frequency + 0.5) / (doc_frequency + 0.5)) + 1);
    auto tf = term_freq / (term_freq + k * (1 - b + b * (document_length / average_doc_size)));
    auto score = idf * (tf * (k + 1));

    return score;
}

std::vector<std::string> Searcher::Tokenize(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;

    while (iss >> token) {
        if (token != "AND" && token != "OR") {
            token = ToLowerCase(token);
        }

        while (token[0] == '(') {
            tokens.emplace_back("(");
            token = ToLowerCase(token.substr(1));
        }

        int bracket_counter = 0;
        while (token[token.size() - 1] == ')') {
            bracket_counter++;
            token = ToLowerCase(token.substr(0, token.size() - 1));
        }
        tokens.push_back(token);
        for (int i = 0; i < bracket_counter; ++i) {
            tokens.emplace_back(")");
        }
    }

    return tokens;
}

bool Searcher::IsValid(const std::vector<std::string>& tokens) const {
    int balance = 0;
    bool expect_operator = false;

    for (const auto& token : tokens) {
        if (token == "(") {
            if (expect_operator) {
                return false;
            }
            ++balance;
        } else if (token == ")") {
            if (!expect_operator || balance == 0) {
                return false;
            }
            --balance;
        } else if (IsOperator(token)) {
            if (!expect_operator) {
                return false;
            }
            expect_operator = false;
        } else if (IsTerm(token)) {
            if (expect_operator) {
                return false;
            }
            expect_operator = true;
        } else {
            return false;
        }
    }

    return balance == 0 && expect_operator;
}

bool Searcher::IsOperator(const std::string& token) {
    return (token == "AND" || token == "OR");
}

bool Searcher::IsTerm(const std::string& token) {
    return (!IsOperator(token) && token != "(" && token != ")");
}

auto Searcher::GetOrScore(const std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>>& first,
                          const std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>>& second,
                          const std::string& term1,
                          const std::string& term2) {

    std::unordered_map<std::size_t, std::set<std::size_t>> first_map;
    auto
        add =
        [&first_map](const std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>>& metric) {
          for (const auto& pair : metric) {
              auto& key = pair.first.first;
              auto& values = pair.first.second;

              first_map[key].insert(values.begin(), values.end());
          }
        };

    add(first);
    add(second);

    std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>> ORvector;
    for (auto& [key, values] : first_map) {
        double score1 = 0.0;
        double score2 = 0.0;
        if (term1 != trash_str) {
            score1 = GetDocScore(term1, key);
        }
        if (term2 != trash_str) {
            score2 = GetDocScore(term2, key);
        }
        ORvector.emplace_back(std::make_pair(key, std::move(values)), score1 + score2);
    }

    return ORvector;
}

auto Searcher::GetAndScore(const std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>>& first,
                           const std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>>& second,
                           const std::string& term1,
                           const std::string& term2) {

    std::unordered_map<std::size_t, std::set<std::size_t>> first_map;
    for (const auto& pair : first) {
        auto& key = pair.first.first;
        auto& values = pair.first.second;

        first_map[key].insert(values.begin(), values.end());
    }
    std::unordered_map<std::size_t, std::set<std::size_t>> second_map;
    for (const auto& pair : second) {
        auto& key = pair.first.first;
        auto& values = pair.first.second;

        second_map[key].insert(values.begin(), values.end());
    }

    std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>> ANDvector;
    for (const auto& index : first_map) {
        if (second_map.find(index.first) != second_map.end()) {
            double score1 = 0.0;
            double score2 = 0.0;
            if (term1 != trash_str) {
                score1 = GetDocScore(term1, index.first);
            }
            if (term2 != trash_str) {
                score2 = GetDocScore(term2, index.first);
            }
            std::set<std::size_t> merged_set = index.second;
            merged_set.insert(second_map[index.first].begin(), second_map[index.first].end());
            ANDvector.emplace_back(std::make_pair(index.first, merged_set), score1 + score2);
        }
    }

    return ANDvector;
}

void Searcher::PushToTermsStack(std::stack<std::string>& terms,
                                std::stack<std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>,
                                                                 double>>>& metrics,
                                std::stack<std::string>& operators) {
    std::string op = operators.top();
    operators.pop();
    std::string right = terms.top();
    terms.pop();
    auto right_metric = metrics.top();
    metrics.pop();
    std::string left = terms.top();
    terms.pop();
    auto left_metric = metrics.top();
    metrics.pop();
    if (op == "AND") {
        metrics.push(GetAndScore(left_metric, right_metric, left, right));
        terms.emplace(trash_str);
    }
    if (op == "OR") {
        metrics.push(GetOrScore(left_metric, right_metric, left, right));
        terms.emplace(trash_str);
    }
}

auto Searcher::GetScore(const std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>>& first,
                        const std::string& term1) {

    std::unordered_map<std::size_t, std::set<std::size_t>> first_map;
    auto
        add =
        [&first_map](const std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>>& metric) {
          for (const auto& pair : metric) {
              auto& key = pair.first.first;
              auto& values = pair.first.second;

              first_map[key].insert(values.begin(), values.end());
          }
        };

    add(first);

    std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>> ORvector;
    for (auto& [key, values] : first_map) {
        double score1 = 0.0;
        double score2 = 0.0;
        if (term1 != trash_str) {
            score1 = GetDocScore(term1, key);
        }
        ORvector.emplace_back(std::make_pair(key, std::move(values)), score1 + score2);
    }

    return ORvector;

}

auto Searcher::GetDocsScore(const std::vector<std::string>& tokens) {

    if (tokens.size() == 1 && IsTerm(tokens[0])) {
        return GetScore(PairToTriple(index_[tokens[0]]), tokens[0]);
    }

    std::stack<std::string> operators;
    std::stack<std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>>> metrics;
    std::stack<std::string> terms;

    for (const std::string& token : tokens) {
        if (IsTerm(token)) {
            metrics.push(PairToTriple(index_[token]));
            terms.push(token);
        } else if (token == "AND" || token == "OR") {
            while (!operators.empty() && operators.top() != "("
                && (operators.top() == "AND" || (operators.top() == "OR" && token == "OR"))) {
                PushToTermsStack(terms, metrics, operators);
            }
            operators.push(token);
        } else if (token == "(") {
            operators.push(token);
        } else if (token == ")") {
            while (operators.top() != "(") {
                PushToTermsStack(terms, metrics, operators);
            }
            operators.pop();
        }
    }

    while (!operators.empty()) {
        PushToTermsStack(terms, metrics, operators);
    }

    auto comparator = [](const std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>& a,
                         const std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>& b) {
      return a.second < b.second;
    };

    auto ans = metrics.top();

    std::sort(ans.begin(), ans.end(), comparator);

    return ans;
}

std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>> Searcher::PairToTriple(const std::vector<
    std::pair<std::size_t, std::size_t>>& post_list) {
    std::vector<std::pair<std::pair<std::size_t, std::set<std::size_t>>, double>> ans;
    for (const auto& el : post_list) {
        ans.emplace_back(std::make_pair(el.first, std::set<std::size_t>{el.second}), 0.0);
    }
    return ans;
}

auto Searcher::GetFileName(std::size_t doc_id) const {
    return documents_paths_[doc_id];
}
