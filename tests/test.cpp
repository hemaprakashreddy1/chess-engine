#include <iostream>
#include <string>
#include <vector>
#include <future>

#include "../src/Position.h"

struct TestResult {
    int testId;
    int depth;
    long long expected;
    long long actual;
    bool passed;
};

int main() {
    int t;
    std::cin >> t;

    std::vector<std::future<TestResult>> futures;

    for (int i = 1; i <= t; i++) {
        std::string fen;
        getline(std::cin >> std::ws, fen);

        int n;
        std::cin >> n;

        for (int j = 0; j < n; j++) {
            int depth, count;
            std::cin >> depth >> count;

            futures.push_back(
                std::async(
                    std::launch::async,
                    [fen, depth, count, i]() -> TestResult {
                        Position position(fen);

                        long long res =
                            position.perft(depth, false, true);

                        return {
                            i,
                            depth,
                            count,
                            res,
                            res == count
                        };
                    }
                )
            );
        }
    }

    int pass = 0;
    int fail = 0;

    for (auto &f : futures) {
        TestResult result = f.get();

        if (result.passed) {
            pass++;
        } else {
            fail++;
        }

        std::cout << "Pass : " << pass << ", Fail : " << fail << "\n";

        if (!result.passed) {
            std::cout << "fail test : " << result.testId << ", depth : " << result.depth << ", expected : " << result.expected << ", got : " << result.actual << "\n";
        }
    }

    return 0;
}