#include "graph.hpp"
#include "catch.hpp"

#include <string>
#include <sstream>
#include <algorithm>

TEST_CASE("graph input")
{
    using namespace std::string_literals;
    std::stringstream input(R"(1
5 7
0 1
0 4
1 2
1 3
1 4
2 3
3 4)"s);

    std::string expected_output = R"(0-> 1-> 4
1-> 0-> 2-> 3-> 4
2-> 1-> 3
3-> 1-> 2-> 4
4-> 0-> 1-> 3
)";

    std::vector<algo::graph> gs = algo::undirected_graph_from_text_input(input);
    REQUIRE(gs.size() == 1u);
    auto& g = gs[0];

    std::stringstream output;
    print_undirected_graph(g, output);

    REQUIRE(output.str() == expected_output);
}

TEST_CASE("bfs visits all nodes")
{
    algo::graph g(4);
    g.add_directed_edge(0, 1);
    g.add_directed_edge(0, 2);
    g.add_directed_edge(1, 2);
    g.add_directed_edge(2, 0);
    g.add_directed_edge(2, 3);
    g.add_directed_edge(3, 3);

    std::vector<algo::graph::vert_ind_t> visited;
    const std::vector<algo::graph::vert_ind_t> expected = {0, 1, 2, 3};
    bfs_for_each_visited(g, 0, [&](auto v) { visited.push_back(v); });

    std::sort(visited.begin(), visited.end());
    REQUIRE(visited == expected);
}

TEST_CASE("dfs visits all nodes")
{
    algo::graph g(4);
    g.add_directed_edge(0, 1);
    g.add_directed_edge(0, 2);
    g.add_directed_edge(1, 2);
    g.add_directed_edge(2, 0);
    g.add_directed_edge(2, 3);
    g.add_directed_edge(3, 3);

    std::vector<algo::graph::vert_ind_t> visited;
    const std::vector<algo::graph::vert_ind_t> expected = {0, 1, 2, 3};

    dfs_for_each_visited(g, 2, [&](auto v) { visited.push_back(v); });

    std::sort(visited.begin(), visited.end());
    REQUIRE(visited == expected);
}

TEST_CASE("finding mother node")
{
    algo::graph g(7);
    g.add_directed_edge(0, 1);
    g.add_directed_edge(0, 2);
    g.add_directed_edge(1, 3);
    g.add_directed_edge(4, 1);
    g.add_directed_edge(6, 4);
    g.add_directed_edge(5, 6);
    g.add_directed_edge(5, 2);
    g.add_directed_edge(6, 0);

    auto mother_node = algo::find_mother_vertex(g);
    REQUIRE(mother_node == 5u);
}

TEST_CASE("when matrix is constructed then it is initialized with zeros and accessible")
{
    algo::matrix m(2u, 3u);
    REQUIRE(m[0][0] == 0);
    REQUIRE(m[0][1] == 0);
    REQUIRE(m[0][2] == 0);
    REQUIRE(m[1][0] == 0);
    REQUIRE(m[1][1] == 0);
    REQUIRE(m[1][2] == 0);
}

TEST_CASE("given small example graph when transitive_closure is called, it returns square matrix with trasitive closure")
{
    algo::graph g(4);
    g.add_directed_edge(0, 1);
    g.add_directed_edge(0, 2);
    g.add_directed_edge(1, 2);
    g.add_directed_edge(2, 0);
    g.add_directed_edge(2, 3);
    g.add_directed_edge(3, 3);

    algo::matrix result = algo::transitive_closure(g);

    REQUIRE(result[0][0] == 1);
    REQUIRE(result[3][0] == 0);

    const algo::matrix expected_result = {{ 1, 1, 1, 1},
                                         { 1, 1, 1, 1},
                                         { 1, 1, 1, 1},
                                         { 0, 0, 0, 1}};

    REQUIRE(result == expected_result);
}

TEST_CASE("when edge is removed vertex becomes unreachable")
{
    algo::graph g(2);
    g.add_undirected_edge(0, 1);

    int before_remove_ct = 0;
    dfs_for_each_visited(g, 0, [&](algo::graph::vert_ind_t) { ++before_remove_ct; });
    REQUIRE(before_remove_ct == 2);

    g.remove_edge(0, 1);

    int after_remove_ct = 0;
    dfs_for_each_visited(g, 0, [&](algo::graph::vert_ind_t) { ++after_remove_ct; });
    REQUIRE(after_remove_ct == 1);
}

TEST_CASE("will drop vertices with degree less than given")
{
    algo::graph g(9);
    g.add_undirected_edge(0, 1);
    g.add_undirected_edge(0, 2);
    g.add_undirected_edge(1, 2);
    g.add_undirected_edge(1, 5);
    g.add_undirected_edge(2, 3);
    g.add_undirected_edge(2, 4);
    g.add_undirected_edge(2, 5);
    g.add_undirected_edge(2, 6);
    g.add_undirected_edge(3, 4);
    g.add_undirected_edge(3, 6);
    g.add_undirected_edge(3, 7);
    g.add_undirected_edge(4, 6);
    g.add_undirected_edge(4, 7);
    g.add_undirected_edge(5, 6);
    g.add_undirected_edge(5, 8);
    g.add_undirected_edge(6, 7);
    g.add_undirected_edge(6, 8);

    int before_cores_ct = 0;
    dfs_for_each_visited(g, 0, [&](algo::graph::vert_ind_t) { ++before_cores_ct; });
    REQUIRE(before_cores_ct == 9);

    auto k_core_graph = k_cores(g, 3);

    int after_cores_ct = 0;
    dfs_for_each_visited(k_core_graph, 0, [&](algo::graph::vert_ind_t) { ++after_cores_ct; });
    REQUIRE(after_cores_ct == 5);
}

TEST_CASE("will compute distance from a given vertex")
{
    algo::graph g(7);
    g.add_undirected_edge(0, 1);
    g.add_undirected_edge(0, 2);
    g.add_undirected_edge(1, 3);
    g.add_undirected_edge(1, 4);
    g.add_undirected_edge(1, 5);
    g.add_undirected_edge(2, 6);

    auto distances = distances_from(g, 0);
    const auto expected_distances = std::vector<algo::graph::dist_t>{0, 1, 1, 2, 2, 2, 2};

    REQUIRE(distances == expected_distances);
}

TEST_CASE("will count number of vertices at given distance")
{
    algo::graph g(6);
    g.add_undirected_edge(0, 1);
    g.add_undirected_edge(0, 2);
    g.add_undirected_edge(1, 3);
    g.add_undirected_edge(2, 4);
    g.add_undirected_edge(2, 5);

    auto vert_count_at_dist_2 = algo::count_verts_at_distance_from(g, 0, 2);

    REQUIRE(vert_count_at_dist_2 == 3);
}

TEST_CASE("will return all paths between given vertices")
{
    algo::graph g(5);
    g.add_directed_edge(0, 1);
    g.add_directed_edge(0, 2);
    g.add_directed_edge(0, 4);
    g.add_directed_edge(1, 3);
    g.add_directed_edge(1, 4);
    g.add_directed_edge(2, 4);
    g.add_directed_edge(3, 2);

    auto paths = paths_between(g, 0, 4);

    REQUIRE(paths.size() == 4);

    std::set<std::vector<algo::graph::vert_ind_t>> result;
    std::transform(paths.begin(), paths.end(), std::inserter(result, result.begin()),
                   [](const auto& p)
                   {
                       return p.get_verts();
                   });
    const std::set<std::vector<algo::graph::vert_ind_t>> expected =
        {
            {0, 4},
            {0, 1, 4},
            {0, 2, 4},
            {0, 1, 3, 2, 4}
        };

    REQUIRE(result == expected);
}
