#include <benchmark/benchmark.h>
#include <random>
#include <gtest/gtest.h>

#include "forward_list.h"

using namespace zstl;

#define NN 1000000

void sort_benchmark(benchmark::State& state) {
  int N = state.range(0);
  
  for (auto _ : state) { 
  ForwardList<int> l2;
  
  std::random_device r;
  std::default_random_engine e(r());
  std::uniform_int_distribution<int> uniform_dist(0, N);
  std::generate_n(std::back_inserter(l2), N, [&]() { return uniform_dist(e); });
  l2.sort();

  EXPECT_TRUE(std::is_sorted(l2.begin(), l2.end()));
  }
}

void sort2_benchmark(benchmark::State& state) {
  int N = state.range(0);
  
  for (auto _ : state) { 
  ForwardList<int> l2;
  
  std::random_device r;
  std::default_random_engine e(r());
  std::uniform_int_distribution<int> uniform_dist(0, N);
  std::generate_n(std::back_inserter(l2), N, [&]() { return uniform_dist(e); });

  l2.sort2();

  EXPECT_TRUE(std::is_sorted(l2.begin(), l2.end()));
  }

}

BENCHMARK(sort_benchmark)->RangeMultiplier(5)->Range(100, NN);
BENCHMARK(sort2_benchmark)->RangeMultiplier(5)->Range(100, NN);

BENCHMARK_MAIN();
