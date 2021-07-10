#include <iterator>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>

using namespace std;

template<typename T>
double Expection(vector<T> const& items){
	double sum = accumulate(items.begin(), items.end(), 0.0);
	return sum / items.size();
}

template<typename T>
double Variance(vector<T> const& items, double Ex){
	double square_sum = accumulate(items.begin(), items.end(), (double)0.0,
			[Ex](double const& init, T const& item){
				return init + (item - Ex) * (item - Ex);
			});
	
	return square_sum / (items.size() - 1);
}

template<typename T>
double StandardVariance(vector<T> const& items, double Ex){
	return sqrt(Variance(items, Ex));
}

template<typename T>
inline double Variance(vector<T> const& items){
	return Variance(items, Expection(items));
}

template<typename T>
inline double StandardVariance(vector<T> const& items){
	return StandardVariance(items, Expection(items));
}

int main(){
	ios::sync_with_stdio(false);
	
	vector<int> items;
	copy(istream_iterator<int>(cin),
		 istream_iterator<int>(),
		 back_inserter(items));

	auto Ex = Expection(items);
	auto Var = Variance(items, Ex);
	auto SDX = StandardVariance(items, Ex);

	cout << "E[X]: " << Ex <<endl;
	cout << "Var(X): " << Var <<endl;
	cout << "SD(X): " << SDX << endl;
}
