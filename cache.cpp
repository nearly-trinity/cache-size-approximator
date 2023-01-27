#include <iostream>
#include <chrono>
#include <queue>
#include <vector>
#include <cstring>

using namespace std::chrono;
using namespace std;

using Duration = nanoseconds;

inline void clflush(void *p) {
    asm volatile("clflush (%0)" : : "r"(p) : "memory");
}

int getMissIdx(int threshold)
{
    const int l1_arr_size = 1024*1024;
    
    int l1_size = 0;
    int l1_array[l1_arr_size];
    memset(l1_array, 0, sizeof(int) * l1_arr_size);

    // flush L1 cache
    for (int i = 0; i < l1_arr_size; i += 64){
        clflush(&l1_array[i]);
    }

    // find cache miss
    auto last_time = high_resolution_clock::now();
    for(int i = 0; i < l1_arr_size; ++i)
    {
        auto current_time = high_resolution_clock::now();
        Duration diff = duration_cast<Duration>(current_time - last_time);

        if (diff.count() > threshold) {
            return i;
        }
        
        l1_array[i] = i;
        last_time = current_time;
    }

    return -1;
}


int main(int argc, char *argv[])
{

    int threshold = 200;
    int num_iterations = 1000;

    if (argc != 3) {
        cout << "Using default values | threshold = 200, iterations = 1000\n";
        cout << "You may provide custom arguments | ./cache [threshold] [iterations]\n";
    } else {
        threshold = stoi(argv[1]);
        num_iterations = stoi(argv[2]);
    }

    cout << "\n";

    priority_queue<int, vector<int>, less<int>> min_heap;
    priority_queue<int, vector<int>, greater<int>> max_heap;

    vector<int> times;
    for (int i = 0; i < num_iterations; ++i)
    {
        int miss_idx = getMissIdx(threshold);

        if (max_heap.empty() || miss_idx <= max_heap.top())
            max_heap.push(miss_idx);
        else
            min_heap.push(miss_idx);

        if (max_heap.size() > min_heap.size() + 1) {
            min_heap.push(max_heap.top());
            max_heap.pop();
        } else if (max_heap.size() < min_heap.size()) {
            max_heap.push(min_heap.top());
            min_heap.pop();
        }

        times.push_back(miss_idx);
    }

    int median;
    if (max_heap.size() == min_heap.size()) {
        median = (max_heap.top() + min_heap.top()) / 2.0;
    } else {
        median = max_heap.top();
    }

    int cache_size = median*sizeof(int)/1024;

    cout << "After " << num_iterations << " iterations, L1 size approximate is: " << cache_size << endl;
    
    return 0;
}