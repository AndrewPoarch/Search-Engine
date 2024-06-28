#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>
#include <cctype>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <string_view>

namespace fs = std::filesystem;

class Indexer {
  private:
    fs::path directory_path_;
    fs::path source_path_;
    std::vector<std::size_t> documents_words_count_;
    std::vector<fs::path> documents_paths_;
    std::map<std::string, std::vector<std::pair<std::size_t, std::size_t>>> index_;

    static bool IsValidExtension(const fs::path& path);
  public:
    void ProcessDirectory();

    void ProcessFile(const fs::path& path, std::size_t document_id);

    void SetDirectory(const fs::path& path);

    void SetIndexSource(const fs::path& path);

    void CreateIndex();
};
