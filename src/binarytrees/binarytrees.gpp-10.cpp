#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <deque>
// #include <apr_pools.h>
#include "arena.h"

struct Node
{
    Node *l, *r;

    Node() : l(nullptr), r(nullptr){};

    int check() const
    {
        if (l)
            return 1 + l->check() + r->check();
        else
            return 1;
    }
};

class NodePool
{
public:
    NodePool()
    {
        // apr_pool_create_unmanaged(&pool);
    }

    ~NodePool()
    {
        // apr_pool_destroy(pool);
    }

    Node *alloc()
    {
        // return (Node *)apr_palloc(pool, sizeof(Node));
        return &pool.alloc(Node());
        // return &pool.emplace_back();
    }

    void clear()
    {
        // apr_pool_clear(pool);
        // pool.clear();
    }

private:
    // apr_pool_t *pool;
    Arena<Node> pool;
    // std::deque<Node> pool;
};

Node *make(int d, NodePool &store)
{
    Node * root = store.alloc();
    if (d > 0)
    {
        root->l = make(d - 1, store);
        root->r = make(d - 1, store);
    }
    return root;
}

int main(int argc, char *argv[])
{
    std::ios_base::sync_with_stdio(false);
    const int min_depth = 4;
    const int max_depth = std::max(min_depth + 2,
                                   (argc == 2 ? atoi(argv[1]) : 10));
    const int stretch_depth = max_depth + 1;

    // Alloc then dealloc stretchdepth tree
    {
        NodePool store;
        Node *c = make(stretch_depth, store);
        std::cout << "stretch tree of depth " << stretch_depth << "\t "
                  << "check: " << c->check() << std::endl;
    }

    NodePool long_lived_store;
    Node *long_lived_tree = make(max_depth, long_lived_store);

    int d, i = 0;
    std::vector<int> cres(max_depth / 2 + 1, 0);
    // #pragma omp parallel for collapse(2) shared(cres, min_depth, max_depth) private(d, i) default(none)
    for (d = min_depth / 2; d <= max_depth / 2; ++d)
    {
        // const auto iterations = ;
        for (i = 1; i <= (1 << (max_depth - (2 * d) + min_depth)); ++i)
        {
            // Create a memory pool for this thread to use.
            NodePool store;
            Node *a = make(2 * d, store);
            // c += a->check();
            cres[d] += a->check();
        }
    }


    // print all results
    for (d = min_depth / 2; d <= max_depth / 2; ++d)
    {
        std::cout << (1 << (max_depth - (2 * d) + min_depth)) << "\t trees of depth " << 2 * d
                  << "\t check: " << cres[d] << std::endl;
    }
    std::cout << "long lived tree of depth " << max_depth << "\t check: " << long_lived_tree->check() << std::endl;
    return 0;
}
