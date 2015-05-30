
#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>
#include <string>
#include <thread>

template<typename ForwardIterator, typename T>
struct accumulate_block {
    auto operator()(ForwardIterator first, ForwardIterator last,
                    T & result)
        -> void
    {
        result = std::accumulate(first, last, result);
    }
};


    
template<typename ForwardIterator, typename T>
auto parallel_accumulate(ForwardIterator first, ForwardIterator last, T init)
    -> T
{
    using namespace std;
    const unsigned long length = distance(first, last);

    if (length == 0) {
        return init;
    }

    const unsigned long min_per_thread = 25;
    const unsigned long max_threads = (length + min_per_thread - 1) / min_per_thread;
    const unsigned long hardware_threads = thread::hardware_concurrency();
    const unsigned long num_threads = min( hardware_threads == 0 ? 2 : hardware_threads
                                           , max_threads );
    const unsigned long block_size = length / num_threads;

    cout << "Splitting " << length << " items over " << num_threads << " threads" << endl;
    vector<T> results(num_threads);
    vector<thread> threads(num_threads - 1);
    
    ForwardIterator block_first = first;
    for ( unsigned long i = 0 ; i < num_threads-1 ; i++ ) {
        ForwardIterator block_end = block_first;
        advance(block_end, block_size);
        threads[i] = thread(accumulate_block<ForwardIterator, T>(),
                            block_first, block_end, ref(results[i]));
        block_first = block_end;
    }
    accumulate_block<ForwardIterator, T>()
        (block_first, last, results[num_threads-1]);
    for_each(begin(threads), end(threads), mem_fn(&thread::join));
    return accumulate(begin(results), end(results), init);
}


int main(int argc, char ** argv) {
    using namespace std;
    vector<string> args;
    for_each(&argv[1], argv + argc, [&](const char * arg) {
         args.push_back(string(arg));
     });
    auto acc = parallel_accumulate(begin(args), end(args), string());
    cout << acc << endl;
    return 0;
}
