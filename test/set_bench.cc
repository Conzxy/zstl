#include "set.h"
#include "tool.h"

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <map>
#include <set>
#include "functional.h"

using namespace zstl;

#define N 1000000

template<typename T>
void 
insert_benchmark(benchmark::State& state) {
	int length = state.range(0);
	T set;

	for (auto _ : state) {
		for (int i = 0; i != length; ++i) {
			set.insert(getRandomString(10));
    }
	}
}

static inline void
MySetInsert(benchmark::State& state) {
	insert_benchmark<Set<std::string>>(state);
}

static inline void
STLSetInsert(benchmark::State& state) {
	insert_benchmark<std::set<std::string>>(state);
}

template<typename T>
void
find_benchmark(benchmark::State& state) {
	for (auto _ : state) {
		state.PauseTiming();

		int length = state.range(0);

		T set;
		for (int i = 0; i != length; ++i) {
			auto res = set.insert(i);	
			EXPECT_TRUE(res.second);
		
		}
		state.ResumeTiming();
	
		for (int i = 0; i != length; ++i) {
			auto f1 = set.find(i);
			EXPECT_NE(f1, set.end());
			EXPECT_EQ(*f1, i);
		}
	}

}

static inline void
MySetFind(benchmark::State& state) {
	find_benchmark<Set<int>>(state);
}

static inline void
STLSetFind(benchmark::State& state) {
	find_benchmark<std::set<int>>(state);
}

template<typename T>
void erase_benchmark(benchmark::State& state) {
	T set;	
	int length = state.range(0);


	for (auto _ : state) {
		state.PauseTiming();

		for (int i = 0; i != length; ++i) {
			set.insert(i);
		}

		state.ResumeTiming();

		for (int i = 0; i != length; ++i) {
			set.erase(set.begin());
		}
	}
}

static inline void 
STLSetErase(benchmark::State& state) {
	erase_benchmark<std::set<int>>(state);
}

static inline void
MySetErase(benchmark::State& state) {
	erase_benchmark<Set<int>>(state);
}

BENCHMARK(MySetErase)->RangeMultiplier(10)->Range(1, N);
BENCHMARK(STLSetErase)->RangeMultiplier(10)->Range(1, N);
BENCHMARK(MySetInsert)->RangeMultiplier(10)->Range(1, N);
BENCHMARK(STLSetInsert)->RangeMultiplier(10)->Range(1, N);
BENCHMARK(STLSetFind)->RangeMultiplier(10)->Range(1, N);
BENCHMARK(MySetFind)->RangeMultiplier(10)->Range(1, N);

BENCHMARK_MAIN();
