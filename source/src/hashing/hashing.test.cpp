#include <catch.hpp>

#include "hashing.hpp"

TEST_CASE("longest_zero_sum_subsequence basic test")
{
    const std::vector<int> input = {15, -2, 2, -8, 1, 7, 10, 23};
    auto ans = longest_zero_sum_subsequence(input);
    REQUIRE(std::distance(ans.first, ans.second) == 5);
}
