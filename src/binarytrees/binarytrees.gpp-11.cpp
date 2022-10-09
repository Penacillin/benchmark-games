#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <execution>
#include "arena.h"

constexpr size_t LINE_SIZE = 64;

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

template <typename T>
struct CountingIterator
{
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = T;
    using value_type = T;
    using pointer = T *;               // or also value_type*
    using reference = T &;             // or also value_type&
    using const_reference = const T &; // or also value_type&

    CountingIterator(value_type ptr) : m_value(ptr) {}

    reference operator*() { return m_value; }
    const_reference operator*() const { return m_value; }
    pointer operator->() { return &m_value; }
    value_type operator[](difference_type off) const { return m_value + off; }

    // Prefix increment
    CountingIterator &operator++()
    {
        ++m_value;
        return *this;
    }

    // Postfix increment
    CountingIterator operator++(int)
    {
        CountingIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    CountingIterator &operator--()
    {
        --m_value;
        return *this;
    }

    CountingIterator operator--(int)
    {
        CountingIterator tmp = *this;
        --*this;
        return tmp;
    }

    CountingIterator &operator+=(difference_type off)
    {
        m_value += off;
        return *this;
    }
    CountingIterator operator+(difference_type off) const { return CountingIterator(m_value + off); }
    friend CountingIterator operator+(difference_type off, const CountingIterator &right)
    {
        return CountingIterator(off + right.m_value);
    }

    CountingIterator &operator-=(difference_type off)
    {
        m_value -= off;
        return *this;
    }

    CountingIterator operator-(difference_type off) const
    {
        return CountingIterator(m_value - off);
    }

    difference_type operator-(const CountingIterator &right) const
    {
        return m_value - right.m_value;
    }

    friend bool operator==(const CountingIterator &a, const CountingIterator &b) { return a.m_value == b.m_value; };
    friend bool operator!=(const CountingIterator &a, const CountingIterator &b) { return a.m_value != b.m_value; };
    bool operator<(const CountingIterator &r) const
    {
        return m_value < r.m_value;
    }

    bool operator<=(const CountingIterator &r) const
    {
        return m_value <= r.m_value;
    }

    bool operator>(const CountingIterator &r) const
    {
        return m_value > r.m_value;
    }

    bool operator>=(const CountingIterator &r) const
    {
        return m_value >= r.m_value;
    }

private:
    value_type m_value;
};

Node *make(int d, Arena<Node> &store)
{
    Node *root = &store.alloc(Node());
    if (d > 0)
    {
        root->l = make(d - 1, store);
        root->r = make(d - 1, store);
    }
    return root;
}

int main(int argc, char *argv[])
{
    constexpr int min_depth = 4;
    const int max_depth = std::max(min_depth + 2, (argc == 2 ? atoi(argv[1]) : 10));

    // Alloc then dealloc stretchdepth tree
    {
        Arena<Node> store;
        const int stretch_depth = max_depth + 1;
        Node *c = make(stretch_depth, store);
        printf("stretch tree of depth %d\t check: %d\n", stretch_depth, c->check());
    }

    Arena<Node> long_lived_store;
    Node *long_lived_tree = make(max_depth, long_lived_store);

    // buffer to store output of each thread
    char *__restrict__ outputstr = reinterpret_cast<char *>(malloc(LINE_SIZE * (max_depth / 2 + 1) * sizeof(char)));

    std::for_each(
        std::execution::par,
        CountingIterator(min_depth / 2), CountingIterator(max_depth / 2 + 1),
        [total_d = max_depth + min_depth, outputstr](const auto d)
        {
            const auto iterations = 1 << (total_d - (2 * d));
            const auto c = std::transform_reduce(
                std::execution::par, CountingIterator(0),
                CountingIterator(iterations), 0L, std::plus{},
                [depth = 2 * d](auto _)
                {
                    Arena<Node> store;
                    Node *a = make(depth, store);
                    return a->check();
                });
            // each thread write to separate location
            sprintf(outputstr + LINE_SIZE * d, "%d\t trees of depth %d\t check: %ld\n",
                    iterations, d, c);
        });

    for (int d = min_depth / 2; d <= max_depth / 2; ++d)
        printf("%s", outputstr + (d * LINE_SIZE));

    printf("long lived tree of depth %d\t check: %d\n", max_depth, long_lived_tree->check());
    free(outputstr);
    return 0;
}
