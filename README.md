# Plagiarism Checker

This project is a comprehensive plagiarism detection system developed as part of the CS293 Data Structures and Algorithms Lab at IIT Bombay, aimed at identifying non-original code submissions. With plagiarism posing challenges in academia, this tool helps detect cases where code is copied without citation or attribution.

## Project Structure

The Plagiarism Checker is designed in three main phases:

1. **Pairwise Code Comparison:** A base algorithm identifies plagiarism by comparing two code files tokenized into sequences of integers, focusing on accurately matching patterns within specific length ranges.
2. **Scalable Bulk Detection:** Designed to handle numerous submissions, this phase implements asynchronous processing for efficiency and leverages API-based modularity to compare submissions in parallel.
3. **Robustness Testing (Hack Detection):** This phase assesses the checker’s robustness by attempting to generate cases that evade detection, evaluating against common bypass techniques.

### Key Features

- **Plagiarism Detection Types:** Detects various plagiarism types such as global, direct, paraphrasing, and patchwork.
- **Pattern Matching:** Identifies exact and approximate matches in code patterns, flagging potential plagiarism based on configurable thresholds.
- **Concurrency:** Utilizes multithreading to efficiently handle high-volume submissions and flags results in real-time.
- **API Integration:** Modular design allowing interaction with student and professor APIs for notifications of plagiarism cases.

### Requirements

- **C++ Compiler:** `clang++` or `g++` (recommended)
- **Libraries:** C++ STL, including multithreading and chrono libraries.
  
