/* The Computer Language Benchmarks Game
 * https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
 *
 * contributed by Jon Harrop
 * modified by Alex Mizrahi
 * modified by Andreas Schäfer
 * very minor omp tweak by The Anh Tran
 * modified to use apr_pools by Dave Compton
 *  *reset*
 */

#include <iostream>
#include <deque>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <execution>
#include <apr_pools.h>

template <typename T>
struct CountingIterator
{
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = T;
	using pointer = T *;			   // or also value_type*
	using reference = T &;			   // or also value_type&
	using const_reference = const T &; // or also value_type&

	CountingIterator(pointer ptr) : m_value(reinterpret_cast<value_type>(ptr)) {}
	CountingIterator(value_type ptr) : m_value(ptr) {}

	reference operator*() { return m_value; }
	const_reference operator*() const { return m_value; }
	pointer operator->() { return &m_value; }

	// Prefix increment
	CountingIterator &operator++()
	{
		m_value++;
		return *this;
	}

	// Postfix increment
	CountingIterator operator++(int)
	{
		CountingIterator tmp = *this;
		++(*this);
		return tmp;
	}

	friend bool operator==(const CountingIterator &a, const CountingIterator &b) { return a.m_value == b.m_value; };
	friend bool operator!=(const CountingIterator &a, const CountingIterator &b) { return a.m_value != b.m_value; };

private:
	value_type m_value;
};

const size_t LINE_SIZE = 64;

class Apr
{
public:
	Apr()
	{
		apr_initialize();
	}

	~Apr()
	{
		apr_terminate();
	}
};

struct Node
{
	Node *l, *r;

	int check() const
	{
		if (l)
			return l->check() + 1 + r->check();
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
		return &pool.emplace_back();
	}

	void clear()
	{
		// apr_pool_clear(pool);
		pool.clear();
	}

private:
	// apr_pool_t *pool;
	std::deque<Node> pool;
};

Node *make(int d, NodePool &store)
{
	Node *root = store.alloc();

	if (d > 0)
	{
		root->l = make(d - 1, store);
		root->r = make(d - 1, store);
	}
	else
	{
		root->l = root->r = 0;
	}

	return root;
}

int main(int argc, char *argv[])
{
	// Apr apr;
	int min_depth = 4;
	int max_depth = std::max(min_depth + 2,
							 (argc == 2 ? atoi(argv[1]) : 10));
	int stretch_depth = max_depth + 1;

	// Alloc then dealloc stretchdepth tree
	{
		NodePool store;
		Node *c = make(stretch_depth, store);
		std::cout << "stretch tree of depth " << stretch_depth << "\t "
				  << "check: " << c->check() << std::endl;
	}

	NodePool long_lived_store;
	Node *long_lived_tree = make(max_depth, long_lived_store);

	// buffer to store output of each thread
	char *outputstr = (char *)malloc(LINE_SIZE * (max_depth + 1) * sizeof(char));

	std::vector<int> r(max_depth);
	std::iota(r.begin(), r.end(), 0);
	std::for_each(std::execution::par, r.begin(), r.end(),
				  [outputstr, max_depth, min_depth](auto d)
				  {
					  int iterations = 1 << (max_depth - d + min_depth);
					  int c = 0;

					  // Create a memory pool for this thread to use.

					  std::for_each(CountingIterator(1), CountingIterator(iterations + 1), [](auto d){
					  	NodePool store;
					  	Node *a = make(d, store);
					  	// store.clear();
					  	c += a->check();
					  });
					//   for (int i = 1; i <= iterations; ++i)
					//   {
					// 	  Node *a = make(d, store);
					// 	  c += a->check();
					// 	  store.clear();
					//   }

					  // each thread write to separate location
					  sprintf(outputstr + LINE_SIZE * d, "%d\t trees of depth %d\t check: %d\n",
							  iterations, d, c);
				  });

	// print all results
	for (int d = min_depth; d <= max_depth; d += 2)
		printf("%s", outputstr + (d * LINE_SIZE));
	free(outputstr);

	std::cout << "long lived tree of depth " << max_depth << "\t "
			  << "check: " << (long_lived_tree->check()) << "\n";

	return 0;
}
