#include <iostream>
#include <chrono>
#include <queue>
#include <vector>
using namespace std::chrono;
using namespace std;

using Duration = nanoseconds;

int getMissIdx(int threshold)
{
    const int l1_arr_size = 1024*1024;
    
    int l1_size = 0;
    int l1_array[1024*1024];

    auto last_time = high_resolution_clock::now();
    for(int i = 0; i < 1024*1024; ++i)
    {
        auto current_time = high_resolution_clock::now();
        Duration diff = duration_cast<Duration>(current_time - last_time);

        if (diff.count() > threshold) {
            return i;
        }
        
        l1_array[i] = i;
        last_time = current_time;
    }
    cout << "never found something over threshold" << endl;

    return -1;
}


int main()
{
    const int threshold = 60;
    const int num_iterations = 10000;

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

    cout << "L1 size is: " << cache_size << endl;
    
    return 1;
}