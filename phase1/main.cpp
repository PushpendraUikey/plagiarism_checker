#include "match_submissions.hpp"
#include "../tokenizer.hpp"

// You should NOT modify ANYTHING in this file.
extern std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2);

double execute_and_verify_testcase(std::string test_dir) {
    tokenizer_t file_one(test_dir + "/one.cpp");
    tokenizer_t file_two(test_dir + "/two.cpp");
    std::vector<int> submission1 = file_one.get_tokens();
    std::vector<int> submission2 = file_two.get_tokens();

//     {   //// -------------------------------------------------modified part
    std::cout << "submission1 tokens: " << std::endl;
        for(int i=0; i<submission1.size(); i++) std::cout << submission1[i] << ' ';
    std::cout << std::endl;
    std::cout << "submission2 tokens: " << std::endl;
    for(int j=0; j<submission2.size(); j++) std::cout << submission2[j] << ' ';
    std::cout << std::endl;
    return 0;
    }

    std::array<int, 5> output = match_submissions(submission1, submission2);
    std::ifstream in(test_dir + "/expected.txt");
    std::array<int, 5> expected;
    in >> expected[0] >> expected[1] >> expected[2] >> 
            expected[3] >> expected[4];
    in.close();

    /*Here I am modifying file to debug code */
    std::cout << "\n";
    std::cout << test_dir << "\n";
    std::cout << "plagiarised: output " << "\t" << output[0] << "\t\t vs expected: \t" << expected[0] << std::endl;
    std::cout << "totalMatch: output "<< "\t" << output[1] << "\t\t vs expected: \t" << expected[1] << std::endl;
    std::cout << "longestMatch: output "<< "\t" << output[2] << "\t\t vs expected: \t" << expected[2] << std::endl;
    std::cout << "starting index 1: output " << output[3] << "\t\t vs expected: \t" << expected[3] << std::endl;
    std::cout << "starting index 2: output " << output[4] << "\t\t vs expected: \t" << expected[4] << std::endl;
    std::cout << "------------------------------*****************---------------------------" << std::endl;

    std::array<double, 5> results;
    results[0] = (output[0] == expected[0]) ? 1.0 : 0.0;
    results[1] = (1.0 * std::min(output[1], expected[1])) / 
            std::max(output[1], expected[1]);
    results[2] = (1.0 * std::min(output[2], expected[2])) /
            std::max(output[2], expected[2]);
    results[3] = std::pow(1.1, -std::abs(output[3] - expected[3]));
    results[4] = std::pow(1.1, -std::abs(output[4] - expected[4]));
    return (2.0 * results[0] + results[1] + results[2] + 
            0.5 * (results[3] + results[4]));
}

int main(void) {
    double total_score = 0.0;
    double score1 = execute_and_verify_testcase("testcases/one");
    double score2 = execute_and_verify_testcase("testcases/two");
    double score3 = execute_and_verify_testcase("testcases/three");
    // modified for testing code
    double score4 = execute_and_verify_testcase("testcases/four");
    double score5 = execute_and_verify_testcase("testcases/five");
    double score6 = execute_and_verify_testcase("testcases/six");

    std::cout << "Testcase 1: " << score1 << " / 5.0" << std::endl;
    std::cout << "Testcase 2: " << score2 << " / 5.0" << std::endl;
    std::cout << "Testcase 3: " << score3 << " / 5.0" << std::endl;
    total_score += score1 + score2 + score3;
    std::cout << "Total score: " << total_score << " / 15.0" << std::endl;

// // modified
//     std::cout << "total score: " << score1 << std:: endl;
    return 0;
}
