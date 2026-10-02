# PlacementBuddy CLI 🚀

> **A high-performance, native C++ terminal client for local AI placement assistance and code analysis.**

`PlacementBuddy CLI` is a lightweight tool built in pure C++17 that interfaces directly with locally running open-weight LLMs via **Ollama**. Designed for students preparing for campus recruitment drives and technical interviews, it analyzes C++ code files or concept notes to provide real-time complexity analysis, edge-case checks, and interview practice questions right inside your terminal—with **zero API fees**, **zero latency**, and **100% offline privacy**.

Built during **Hacktoberfest 2026** for the DEV.to **"Build for a Friend"** Weekend Challenge.

---

## ✨ Features

* **⚡ Pure C++ Native Performance:** Built using C++17, `libcurl`, and `nlohmann/json` for minimal overhead and instant execution.
* **🔒 100% Local & Private:** Connects to your local Ollama instance (`http://localhost:11434`). No API keys or external servers required.
* **🌊 Real-time Stream Output:** Uses asynchronous HTTP streaming callbacks to print model responses line-by-line in real time.
* **🎯 Mode Switching:**
  * `dsa` (Default): Analyzes code for time/space complexity, edge cases, and suggests 2 modified interview variations.
  * `quiz`: Generates 3 multiple-choice practice questions with detailed explanations.
  * `explain`: Explains code line-by-line, highlighting potential logical bugs or performance bottlenecks.

---

## 🛠️ Prerequisites & Dependencies

Ensure you have the following installed on your system (Debian/Ubuntu/Linux):

1. **GCC / Clang** supporting C++17
2. **CMake** (v3.10+)
3. **libcurl** development files
4. **nlohmann-json** header library
5. **Ollama** running locally with a model pulled (e.g., `llama3.2` or `gemma`)

### System Packages Installation

On Debian / Ubuntu / Linux Mint:
```bash
sudo apt update
sudo apt install build-essential cmake libcurl4-openssl-dev nlohmann-json3-dev