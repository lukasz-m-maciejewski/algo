#pragma once

#include <vector>
#include <set>
#include <iosfwd>
#include <functional>
#include <limits>
#include <initializer_list>

namespace algo
{

enum class cmp_res
{
    LT,
    GT,
    EQ,
    NEQ
};

class graph
{
public:
    using vert_ind_t = std::size_t;
    using adj_list_t = std::vector<vert_ind_t>;
    using sz_t = std::int64_t;
    using dist_t = std::ptrdiff_t;

    constexpr static vert_ind_t npos = std::numeric_limits<vert_ind_t>::max();
    constexpr static dist_t max_dist = std::numeric_limits<dist_t>::max();

    class path
    {
    public:
        bool add_next(vert_ind_t v)
        {
            if (would_loop(v))
                return false;
            else
            {
                path.push_back(v);
                return true;
            }
        }

        path extended_with(vert_ind_t v)
        {
            auto cp = *this;
            cp.add_next(v);
            return cp;
        }

        bool would_loop(vert_ind_t v) const
        {
            auto it = std::find(path.begin(), path.end(), v);
            return it != path.end();
        }

        std::vector<vert_ind_t> get_verts() const
        {
            return path;
        }

        vert_ind_t last() const
        {
            return path.back();
        }

    private:
        std::vector<vert_ind_t> path;
    };

    graph(std::size_t N)
        : adj_lists(N), undirected{true}
    {}

    vert_ind_t num_vert() const
    {
        return adj_lists.size();
    }

    void add_undirected_edge(vert_ind_t a, vert_ind_t b)
    {
        adj_lists[a].insert(adj_lists[a].end(), b);
        adj_lists[b].insert(adj_lists[b].end(), a);
    }

    void add_directed_edge(vert_ind_t source, vert_ind_t target)
    {
        undirected = false;
        adj_lists[source].insert(adj_lists[source].end(), target);
    }

    const adj_list_t& neighbours_of(vert_ind_t source) const
    {
        return adj_lists.at(source);
    }

    bool is_undirected() const
    {
        return undirected;
    }

    sz_t degree_of(vert_ind_t v) const
    {
        return static_cast<sz_t>(adj_lists[v].size());
    }

    void remove_edge(vert_ind_t a, vert_ind_t b)
    {
        remove_edge_from_list(adj_lists[a], b);

        if (undirected)
        {
            remove_edge_from_list(adj_lists[b], a);
        }
    }

    void remove_vertex(vert_ind_t v)
    {
        auto adj_list = adj_lists[v];
        for (auto t : adj_list)
            remove_edge(v, t);

        for (auto& al : adj_lists)
        {
            for (auto& t : al)
            {
                if (t > v) --t;
            }
        }

        adj_lists.erase(adj_lists.begin() + v);
    }

private:
    void remove_edge_from_list(adj_list_t& l, vert_ind_t target)
    {
        auto it = std::find(l.begin(), l.end(), target);

        if (it != l.end())
        {
            std::swap(*it, l.back());
            l.pop_back();
        }
    }


    std::vector<adj_list_t> adj_lists;
    bool undirected;
};


std::vector<graph> undirected_graph_from_text_input(std::istream&);
void print_undirected_graph(const graph&, std::ostream&);

inline std::ostream& operator<<(std::ostream& out, const graph& g)
{
    print_undirected_graph(g, out);
    return out;
}

void bfs_for_each_visited(const graph&, graph::vert_ind_t, std::function<void(graph::vert_ind_t)>);
void bfs_for_each_visited(const graph&, graph::vert_ind_t, std::function<void(graph::vert_ind_t, graph::vert_ind_t)>);
void dfs_for_each_visited(const graph&, graph::vert_ind_t, std::function<void(graph::vert_ind_t)>);

graph::vert_ind_t find_mother_vertex(const graph& g);

class matrix
{
public:
    using index_t = std::size_t;
    using value_type = int;

private:
    struct row
    {
        row(matrix& m_init, index_t row_index_init)
            : m{m_init}, row_index{row_index_init}
        {}

        value_type& operator[](index_t c);

        matrix& m;
        index_t row_index;
    };
public:
    matrix(index_t r, index_t c, value_type v = 0);
    matrix(std::initializer_list<std::initializer_list<int>>);

    row operator[](index_t);

    cmp_res cmp(const matrix& rhs) const;

private:

    index_t indices_to_offset(index_t, index_t);

    index_t rows;
    index_t cols;

    std::vector<value_type> data;
};

inline bool operator==(matrix const& lhs, matrix const& rhs) { return (lhs.cmp(rhs) == cmp_res::EQ); }
inline bool operator!=(matrix const& lhs, matrix const& rhs) { return not (lhs == rhs); }

matrix transitive_closure(graph const& g);

graph k_cores(graph g, int k);

std::vector<graph::dist_t> distances_from(graph const& g, graph::vert_ind_t v);

std::size_t count_verts_at_distance_from(graph const& g, graph::vert_ind_t v, graph::dist_t d);

std::vector<graph::path> paths_between(graph const& g, graph::vert_ind_t src, graph::vert_ind_t dst);
}
