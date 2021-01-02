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
void print_undirected_adj_list(const graph::adj_list_t& l, std::ostream& os)
{
    for (const auto& v : l)
        os << "-> " << v;
}
}

void print_undirected_graph(const graph& g, std::ostream& os)
{
    const auto num_vert = g.num_vert();
    for (graph::vert_ind_t i = 0; i < num_vert; ++i)
    {
        os << i;
        print_undirected_adj_list(g.neighbours_of(i), os);
        os << '\n';
    }
}

void bfs_for_each_visited(const graph& g, graph::vert_ind_t initial, std::function<void(graph::vert_ind_t)> f)
{
    boost::contract::check c = boost::contract::function()
        .precondition([&]{ BOOST_CONTRACT_ASSERT(initial < g.num_vert()); });

    if (g.num_vert() == 0u)
        return;

    auto visited = std::make_unique<bool[]>(g.num_vert());
    for (bool* b = visited.get(); b != visited.get() + g.num_vert(); ++b) *b = false;
    std::queue<graph::vert_ind_t> to_visit;

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

void bfs_for_each_visited(const graph& g, graph::vert_ind_t initial, std::function<void(graph::vert_ind_t, graph::vert_ind_t)> f)
{
    boost::contract::check c = boost::contract::function()
        .precondition([&]{ BOOST_CONTRACT_ASSERT(initial < g.num_vert()); });

    if (g.num_vert() == 0u)
        return;

    auto visited = std::make_unique<bool[]>(g.num_vert());
    for (bool* b = visited.get(); b != visited.get() + g.num_vert(); ++b) *b = false;
    std::queue<graph::vert_ind_t> to_visit;

    auto visit = [&](auto v, auto s) {
                     to_visit.push(v);
                     visited[v] = true;
                     if (f) f(v, s);
                 };

    visit(initial, initial);

    while (not to_visit.empty())
    {
        auto current = to_visit.front();
        to_visit.pop();

        for (auto v : g.neighbours_of(current))
        {
            if (not visited[v])
                visit(v, current);
        }
    }
}

namespace
{
void dfs_helper(const graph& g, graph::vert_ind_t current, std::function<void(graph::vert_ind_t)>& f, bool visited[])
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

void dfs_for_each_visited(const graph& g, graph::vert_ind_t initial, std::function<void(graph::vert_ind_t)> f)
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

graph::vert_ind_t find_mother_vertex(const graph& g)
{
    const auto nv = g.num_vert();
    auto visited = std::make_unique<bool[]>(nv);

    graph::vert_ind_t mother_node = graph::npos;

    for (graph::vert_ind_t i = 0; i < nv; ++i)
    {
        if (visited[i]) continue;

        dfs_for_each_visited(g, i, [&](auto v) { visited[v] = true; });
        mother_node = i;
    }

    for (graph::vert_ind_t i = 0; i < nv; ++i) visited[i] = false;

    dfs_for_each_visited(g, mother_node, [&](auto v) { visited[v] = true; });

    if (std::all_of(visited.get(), visited.get() + nv, [](auto b) { return b; }))
    {
        return mother_node;
    }
    else
    {
        return graph::npos;
    }
}

matrix::matrix(index_t r, index_t c, value_type v)
    : rows{r}, cols{c}, data(rows * cols, v)
{
}

matrix::matrix(std::initializer_list<std::initializer_list<int>> init)
    : rows(init.size()), cols(init.begin()->size()), data()
{
    boost::contract::check cc = boost::contract::function()
        . precondition([&]{
                           BOOST_CONTRACT_ASSERT(rows > 0 && cols > 0);
                           for (const auto& il : init)
                           {
                               BOOST_CONTRACT_ASSERT(il.size() == init.begin()->size());
                           }
                       });

    data.reserve(rows * cols);
    for (const auto& il : init)
    {
        data.insert(data.end(), il.begin(), il.end());
    }
}

matrix::value_type& matrix::row::operator[](matrix::index_t col_index)
{
    return m.data[m.indices_to_offset(row_index, col_index)];
}

matrix::row matrix::operator[](matrix::index_t row_index)
{
    return row(*this, row_index);
}

cmp_res matrix::cmp(const matrix& rhs) const
{
    const auto& lhs = *this;
    return (lhs.rows == rhs.rows and lhs.cols == rhs.cols and lhs.data == rhs.data)
        ? cmp_res::EQ
        : cmp_res::NEQ;
}

matrix::index_t matrix::indices_to_offset(matrix::index_t row_index, matrix::index_t col_index)
{
    boost::contract::check contract_check
        = boost::contract::function()
        .precondition([&]{ BOOST_CONTRACT_ASSERT(row_index < rows);
                           BOOST_CONTRACT_ASSERT(col_index < cols); });
    return row_index * cols + col_index;
}


namespace
{
void dfs_transitive_closure_helper(graph const& g, graph::vert_ind_t v_from, graph::vert_ind_t v_to, matrix& cl)
{
    cl[v_from][v_to] = 1;
    for (auto v : g.neighbours_of(v_to))
    {
        if (cl[v_from][v] == 0)
        {
            dfs_transitive_closure_helper(g, v_from, v, cl);
        }
    }
}
}

matrix transitive_closure(graph const& g)
{
    matrix cl(g.num_vert(), g.num_vert(), 0);
    const auto num_vert = g.num_vert();

    for (graph::vert_ind_t i = 0; i < num_vert; ++i)
    {
        dfs_transitive_closure_helper(g, i, i, cl);
    }

    return cl;
}

graph k_cores(graph g, int k)
{
    bool changed = true;

    while (changed)
    {
        changed = false;
        for (graph::vert_ind_t i = 0; i < g.num_vert(); ++i)
        {
            if (g.degree_of(i) < k)
            {
                g.remove_vertex(i);
                changed = true;
            }
        }
    }

    return g;
}

std::vector<graph::dist_t> distances_from(graph const& g, graph::vert_ind_t v)
{
    std::vector<graph::dist_t> dists(g.num_vert(), graph::max_dist);
    dists.at(v) = 0;

    auto vert_visitor = [&](auto current, auto source)
                        {
                            auto& current_distance = dists.at(current);
                            if (current_distance == graph::max_dist)
                                current_distance = 1 + dists.at(source);
                        };

    bfs_for_each_visited(g, v, vert_visitor);

    return dists;
}

std::size_t count_verts_at_distance_from(graph const& g, graph::vert_ind_t v, graph::dist_t d)
{
    auto dists = distances_from(g, v);
    return std::count(dists.begin(), dists.end(), d);
}

namespace
{
void paths_dfs_helper(graph const& g, std::vector<graph::path>& out,
                      graph::path current_path, graph::vert_ind_t target)
{
    if (current_path.last() == target)
    {
        out.push_back(std::move(current_path));
        return;
    }

    for (graph::vert_ind_t v : g.neighbours_of(current_path.last()))
    {
        if (not current_path.would_loop(v))
        {
            //auto ext_path = current_path;
            //ext_path.add_next(v);
            paths_dfs_helper(g, out, current_path.extended_with(v), target);
        }
    }
}
}

std::vector<graph::path> paths_between(graph const& g, graph::vert_ind_t src, graph::vert_ind_t dst)
{
    graph::path init;
    init.add_next(src);

    std::vector<graph::path> result;

    paths_dfs_helper(g, result, init, dst);

    return result;
}

}
