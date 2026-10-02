#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// --- ANSI Escape Code Constants ---
namespace Color {
    const std::string RESET       = "\033[0m";
    const std::string BOLD        = "\033[1m";
    const std::string DIM         = "\033[2m";
    
    // Foreground colors
    const std::string CYAN        = "\033[36m";
    const std::string GREEN       = "\033[32m";
    const std::string YELLOW      = "\033[33m";
    const std::string MAGENTA     = "\033[35m";
    const std::string BLUE        = "\033[34m";
    const std::string RED         = "\033[31m";
    const std::string WHITE       = "\033[37m";

    // Bold + Colors
    const std::string BOLD_CYAN    = "\033[1;36m";
    const std::string BOLD_GREEN   = "\033[1;32m";
    const std::string BOLD_YELLOW  = "\033[1;33m";
    const std::string BOLD_MAGENTA = "\033[1;35m";
    const std::string BOLD_RED     = "\033[1;31m";
    const std::string BOLD_WHITE   = "\033[1;37m";
}

// Global flag to track whether a line is inside a code block for ANSI syntax highlighting
bool inCodeBlock = false;

// Format streaming lines on the fly with lightweight syntax highlights
void processAndStreamChunk(const std::string& chunk) {
    for (char c : chunk) {
        if (c == '`') {
            // Highlight inline code / backticks
            std::cout << Color::CYAN << c << Color::RESET;
        } else {
            std::cout << c << std::flush;
        }
    }
}

// Callback function to handle streaming output from libcurl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string responseChunk((char*)contents, totalSize);

    std::stringstream ss(responseChunk);
    std::string line;
    while (std::getline(ss, line)) {
        if (line.empty()) continue;
        try {
            auto j = json::parse(line);
            if (j.contains("message") && j["message"].contains("content")) {
                std::string content = j["message"]["content"].get<std::string>();
                processAndStreamChunk(content);
            }
        } catch (const std::exception&) {
            // Skip invalid JSON chunks during network stream
        }
    }
    return totalSize;
}

std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << Color::BOLD_GREEN << "Error: " << Color::RESET 
                  << "Could not open file: " << filepath << "\n";
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void printBanner(const std::string& filepath, const std::string& mode, const std::string& model) {
    std::cout << Color::BOLD_CYAN << "┌───────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ " << Color::BOLD_WHITE << " PlacementBuddy CLI " << Color::BOLD_CYAN << " ─ Native C++ Local AI Placement Bot   │\n";
    std::cout << "└───────────────────────────────────────────────────────────────┘\n" << Color::RESET;
    std::cout << Color::BOLD << "  📄 Target File : " << Color::CYAN << filepath << Color::RESET << "\n";
    std::cout << Color::BOLD << "  🎯 Mode        : " << Color::BOLD_MAGENTA << mode << Color::RESET << "\n";
    std::cout << Color::BOLD << "  🤖 Model       : " << Color::BOLD_GREEN << model << Color::RESET << "\n";
    std::cout << Color::DIM << "─────────────────────────────────────────────────────────────────\n" << Color::RESET;
    std::cout << Color::YELLOW << "⏳ Dispatching payload to local Ollama instance...\n\n" << Color::RESET;
    std::cout << Color::BOLD_WHITE << "━━━ Analysis & Output Stream ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n" << Color::RESET;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << Color::BOLD_WHITE << "Usage: " << Color::RESET 
                  << "./placement_buddy <file_path> [mode: dsa|quiz|explain] [model]\n";
        std::cout << Color::DIM << "Example: ./placement_buddy solution.cpp dsa llama3.2\n" << Color::RESET;
        return 1;
    }

    std::string filePath = argv[1];
    std::string mode = (argc > 2) ? argv[2] : "dsa";
    std::string model = (argc > 3) ? argv[3] : "llama3.2";

    std::map<std::string, std::string> prompts = {
        {"dsa", "You are an expert C++ technical interviewer for tier-1 companies. Analyze this code/note, state time and space complexity, highlight edge cases, and suggest 2 interview variations:\n\n"},
        {"quiz", "You are a C++ technical interviewer. Generate 3 multiple-choice practice questions with detailed explanations based on this content:\n\n"},
        {"explain", "Explain this code line-by-line clearly, highlighting potential bugs or performance bottlenecks:\n\n"}
    };

    std::string systemPrompt = prompts.count(mode) ? prompts[mode] : prompts["dsa"];
    std::string fileContent = readFile(filePath);
    std::string fullPrompt = systemPrompt + fileContent;

    json payload = {
        {"model", model},
        {"messages", json::array({
            {{"role", "user"}, {"content", fullPrompt}}
        })},
        {"stream", true}
    };

    std::string jsonString = payload.dump();

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << Color::BOLD_GREEN << "Failed to initialize cURL\n" << Color::RESET;
        return 1;
    }

    printBanner(filePath, mode, model);

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:11434/api/chat");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "\n" << Color::BOLD_GREEN << "cURL Error: " << Color::RESET 
                  << curl_easy_strerror(res) << "\n";
        std::cerr << Color::YELLOW << "Ensure Ollama is active (`ollama list`).\n" << Color::RESET;
    } else {
        std::cout << "\n\n" << Color::DIM << "─────────────────────────────────────────────────────────────────\n" << Color::RESET;
        std::cout << Color::BOLD_GREEN << "✔ Complete! Response streamed successfully.\n" << Color::RESET;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return 0;
}