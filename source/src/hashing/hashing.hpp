#pragma once
#include <vector>
#include <utility>
#include <unordered_map>

using seq = std::vector<int>;
using iter = seq::const_iterator;

std::pair<iter, iter> longest_zero_sum_subsequence(std::vector<int> const& in)
{
    std::unordered_map<int, iter> sum_to_pos{};

    int sum = 0;
    sum_to_pos[0] = in.begin();
    auto result = std::make_pair(in.begin(), in.begin());

    const auto end = in.end();
    for (auto it = in.begin(); it != end; ++it)
    {
        sum += *it;
        auto stp = sum_to_pos.find(sum);
        auto best_current = std::distance(result.first, result.second);
        if (stp != sum_to_pos.end() and std::distance(stp->second, std::next(it)) > best_current)
        {
            result = std::make_pair(stp->second, std::next(it));
        }
        else
        {
            sum_to_pos[sum] = std::next(it);
        }

    }

    return result;
}
