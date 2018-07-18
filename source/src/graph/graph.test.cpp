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

    std::vector<algo::graph::vert_ind> visited;
    const std::vector<algo::graph::vert_ind> expected = {0, 1, 2, 3};
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

    std::vector<algo::graph::vert_ind> visited;
    const std::vector<algo::graph::vert_ind> expected = {0, 1, 2, 3};

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
