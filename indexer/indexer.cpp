#pragma once

#include "indexer.hpp"

namespace fs = std::filesystem;

void Indexer::SetDirectory(const std::filesystem::path& path) {
    directory_path_ = path;
}

void Indexer::SetIndexSource(const fs::path& path) {
    source_path_ = path;
}

std::string ToLowerCase(const std::string& input) {
    std::string result;
    result.reserve(input.size());

    for (char c : input) {
        result += std::tolower(c);
    }

    return result;
}

void Indexer::ProcessDirectory() {
    if (!fs::exists(directory_path_) || !fs::is_directory(directory_path_)) {
        throw std::runtime_error("Path does not exists or not a directory... :/\n");
    }
    try {
        std::size_t document_id = 0;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory_path_)) {
            if (!fs::is_directory(entry.status()) && IsValidExtension(entry.path())) {
                ProcessFile(entry.path(), document_id++);
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Path1: " << e.path1() << std::endl;
    }
}

void Indexer::ProcessFile(const fs::path& path, std::size_t document_id) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open a file... :(\n");
    }

    std::string line;
    std::size_t line_number = 1;
    std::size_t words_counter = 0;
    while (std::getline(file, line)) {
        std::istringstream StringStream(line);
        std::string word;
        while (StringStream >> word) {
            index_[ToLowerCase(word)].emplace_back(document_id, line_number);
            ++words_counter;
        }
        ++line_number;
    }
    documents_words_count_.push_back(words_counter);
    documents_paths_.push_back(path);

    file.close();
}

void Indexer::CreateIndex() {
    fs::path file_path = source_path_ / "index.idx";

    std::ofstream file(file_path, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open a file... :(\n");
    }

    file << documents_words_count_.size() << std::endl;
    for (int i = 0; i < documents_words_count_.size(); ++i) {
        file << i << ' ' << documents_paths_[i] << ' ' << documents_words_count_[i] << std::endl;
    }
    for (const auto& key_value : index_) {
        file << key_value.first << ' ';
        for (const auto& did_num : key_value.second) {
            file << did_num.first << ' ' << did_num.second << ' ';
        }
        file << std::endl;
    }
}
bool Indexer::IsValidExtension(const fs::path& path) {
    return path.extension() != ".exe" && path.extension() != ".dll" && path.extension() != ".obj"
        && path.extension() != ".pdb" && path.extension() != ".ilk" && path.extension() != ".idx"
        && path.extension() != ".pdf" && path.extension() != ".bin"
        && path.filename().string()[0] != '.' && path.filename().string()[0] != '_';
}
