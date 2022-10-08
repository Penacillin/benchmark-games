#include <vector>
#include <cmath>
#include <ranges>
#include <iterator>

template <typename T>
class Arena
{
    static constexpr size_t INITIAL_SIZE = 1024;

public:
    Arena(size_t n)
    {
        mCurrent.reserve(n);
    }

    Arena()
        : Arena(INITIAL_SIZE / (std::max(1lu, sizeof(T)))){};

    size_t len() const
    {
        size_t res = 0;
        for (const auto &v : mRest)
            res += v.size();
        return res + mCurrent.size();
    }

    T &alloc(T &&value)
    {
        if (mCurrent.size() < mCurrent.capacity())
        {
            mCurrent.emplace_back(std::forward<T>(value));
            return mCurrent.back();
        }
        else
        {
            this->reserve(1);
            mCurrent.emplace_back(std::forward<T>(value));
            return mCurrent.back();
            // return this->alloc_extend({std::forward<T>(value)}).front();
        }
    }

    // auto alloc_extend(std::initializer_list<T> iterable)
    // {
    //     size_t next_item_index;
    //     if (iterable.size() > mCurrent.capacity() - mCurrent.size())
    //     {
    //         this->reserve(iterable.size());
    //         mCurrent.insert(mCurrent.begin(), iterable.begin(), iterable.end());
    //     }
    //     else
    //     {
    //         next_item_index = mCurrent.size();
    //         size_t i = 0;
    //         for (auto iter = iterable.begin(); iter != iterable.end(); ++iter)
    //         {
    //             if (mCurrent.size() == mCurrent.capacity())
    //             {
    //                 this->reserve(i + 1);
    //                 auto &previous_chunk = mRest.back();
    //                 mCurrent.insert(mCurrent.begin(),
    //                                 std::make_move_iterator(previous_chunk.begin()),
    //                                 std::make_move_iterator(previous_chunk.end()));
    //                 previous_chunk.erase(previous_chunk.begin(), previous_chunk.end());
    //                 mCurrent.push_back(*iter);
    //                 mCurrent.insert(mCurrent.end(),
    //                                 std::make_move_iterator(iter + 1),
    //                                 std::make_move_iterator(iterable.end()));
    //                 next_item_index = 0;
    //                 break;
    //             }
    //             else
    //             {
    //                 mCurrent.push_back(*iter);
    //             }
    //         }
    //     }
    //     return std::ranges::subrange(mCurrent.begin() + next_item_index, mCurrent.end());
    // }

private:
    constexpr size_t nxt_po2(size_t n) const { return 1 + (n |= (n |= (n |= (n |= (n |= (n -= 1) >> 1) >> 2) >> 4) >> 8) >> 16); }

    void reserve(size_t additional)
    {
        const auto double_cap = mCurrent.capacity() * 2lu;
        const auto required_cap = nxt_po2(additional);
        const auto new_capacity = std::max(double_cap, required_cap);
        mRest.emplace_back(std::move(mCurrent));
        new (&mCurrent) std::vector<T>();
        mCurrent.reserve(new_capacity);
        // mCurrentSize = mCurrent.size();
        // mCurrentCapacity = mCurrent.capacity();
    }

    std::vector<T> mCurrent;
    std::vector<std::vector<T>> mRest;
};
