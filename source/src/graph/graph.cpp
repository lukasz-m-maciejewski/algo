#include "graph.hpp"
#include <istream>
#include <ostream>
#include <queue>

#include <boost/contract.hpp>

namespace algo
{

namespace
{
graph parse_single_undirected_graph(std::istream& in)
{
    int num_vertex;
    int num_edges;
    in >> num_vertex >> num_edges;

    graph g(num_vertex);

    for (int i = 0; i < num_edges; ++i)
    {
        int a;
        int b;
        in >> a >> b;
        g.add_undirected_edge(a, b);
    }

    return g;
}
}

std::vector<graph> undirected_graph_from_text_input(std::istream& in)
{
    int num_graphs;
    in >> num_graphs;

    std::vector<graph> graphs;
    graphs.reserve(num_graphs);

    for (int i = 0; i < num_graphs; ++i)
    {
        graphs.push_back(parse_single_undirected_graph(in));
    }

    return graphs;
}

namespace
{
void print_undirected_adj_list(const graph::adj_list& l, std::ostream& os)
{
    for (const auto& v : l)
        os << "-> " << v;
}
}

void print_undirected_graph(const graph& g, std::ostream& os)
{
    const auto num_vert = g.num_vert();
    for (graph::vert_ind i = 0; i < num_vert; ++i)
    {
        os << i;
        print_undirected_adj_list(g.neighbours_of(i), os);
        os << '\n';
    }
}

void for_each_node_bfs(const graph& g, graph::vert_ind initial, std::function<void(graph::vert_ind)> f)
{
    boost::contract::check c = boost::contract::function()
        .precondition([&]{ BOOST_CONTRACT_ASSERT(initial < g.num_vert()); });

    if (g.num_vert() == 0u)
        return;

    auto visited = std::make_unique<bool[]>(g.num_vert());
    for (bool* b = visited.get(); b != visited.get() + g.num_vert(); ++b) *b = false;
    std::queue<graph::vert_ind> to_visit;

    auto visit = [&](auto v) {
        to_visit.push(v);
        visited[v] = true;
        if (f) f(v);
    };

    visit(initial);

    while (not to_visit.empty())
    {
        auto current = to_visit.front();
        to_visit.pop();

        for (auto v : g.neighbours_of(current))
        {
            if (not visited[v])
                visit(v);
        }
    }
}

namespace
{
void dfs_helper(const graph& g, graph::vert_ind current, std::function<void(graph::vert_ind)>& f, bool visited[])
{
    for (auto v : g.neighbours_of(current))
    {
        if (visited[v]) continue;

        visited[v] = true;
        if (f) f(v);
        dfs_helper(g, v, f, visited);
    }
}
}

void for_each_node_dfs(const graph& g, graph::vert_ind initial, std::function<void(graph::vert_ind)> f)
{
    boost::contract::check c = boost::contract::function()
        .precondition([&]{ BOOST_CONTRACT_ASSERT(initial < g.num_vert()); });

    if (g.num_vert() == 0u)
        return;

    auto visited = std::make_unique<bool[]>(g.num_vert());
    for (bool* b = visited.get(); b != visited.get() + g.num_vert(); ++b) *b = false;

    visited[initial] = true;
    if (f) f(initial);
    dfs_helper(g, initial, f, visited.get());
}

graph::vert_ind find_mother_vertex(const graph& g)
{
    const auto nv = g.num_vert();
    auto visited = std::make_unique<bool[]>(nv);

    graph::vert_ind mother_node = graph::npos;

    for (graph::vert_ind i = 0; i < nv; ++i)
    {
        if (visited[i]) continue;

        for_each_node_dfs(g, i, [&](auto v) { visited[v] = true; });
        mother_node = i;
    }

    for (graph::vert_ind i = 0; i < nv; ++i) visited[i] = false;

    for_each_node_dfs(g, mother_node, [&](auto v) { visited[v] = true; });

    if (std::all_of(visited.get(), visited.get() + nv, [](auto b) { return b; }))
    {
        return mother_node;
    }
    else
    {
        return graph::npos;
    }
}

matrix::value_type& matrix::row::operator[](matrix::index col_index)
{
    return m.data[m.indices_to_offset(row_index, col_index)];
}

matrix::row matrix::operator[](matrix::index row_index)
{
    return row(*this, row_index);
}

matrix::index matrix::indices_to_offset(matrix::index row_index, matrix::index col_index)
{
    boost::contract::check contract_check
        = boost::contract::function()
        .precondition([&]{ BOOST_CONTRACT_ASSERT(row_index < rows);
                           BOOST_CONTRACT_ASSERT(col_index < cols); });
    return row_index * cols + col_index;
}
}
