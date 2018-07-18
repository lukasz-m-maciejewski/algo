#pragma once

#include <vector>
#include <set>
#include <iosfwd>
#include <functional>
#include <limits>

namespace algo
{

class graph
{
public:
    using vert_ind = std::size_t;
    using adj_list = std::vector<vert_ind>;
    graph(std::size_t N)
        : adj_lists(N), undirected{true}
    {}

    std::size_t num_vert() const
    {
        return adj_lists.size();
    }

    void add_undirected_edge(vert_ind a, vert_ind b)
    {
        adj_lists[a].insert(adj_lists[a].end(), b);
        adj_lists[b].insert(adj_lists[b].end(), a);
    }

    void add_directed_edge(vert_ind source, vert_ind target)
    {
        undirected = false;
        adj_lists[source].insert(adj_lists[source].end(), target);
    }

    const adj_list& neighbours_of(vert_ind source) const
    {
        return adj_lists.at(source);
    }

    bool is_undirected() const
    {
        return undirected;
    }

    constexpr static vert_ind npos = std::numeric_limits<vert_ind>::max();

private:
    std::vector<adj_list> adj_lists;
    bool undirected;
};

std::vector<graph> undirected_graph_from_text_input(std::istream&);
void print_undirected_graph(const graph&, std::ostream&);

void for_each_node_bfs(const graph&, graph::vert_ind, std::function<void(graph::vert_ind)>);
void for_each_node_dfs(const graph&, graph::vert_ind, std::function<void(graph::vert_ind)>);

graph::vert_ind find_mother_vertex(const graph& g);


class matrix
{
public:
    using index = std::size_t;
    using value_type = int;

private:
    struct row
    {
        row(matrix& m_init, index row_index_init)
            : m{m_init}, row_index{row_index_init}
        {}

        value_type& operator[](index c);

        matrix& m;
        index row_index;
    };
public:
    matrix(index r, index c, value_type v = 0)
        : rows{r}, cols{c}, data(rows * cols, v)
    {
    }

    row operator[](index);

private:

    index indices_to_offset(index, index);

    index rows;
    index cols;

    std::vector<value_type> data;
};
}