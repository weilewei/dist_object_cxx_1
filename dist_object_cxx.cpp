#include <vector>
#include <iostream>

template <typename T>
struct dist_object {
public:
	typedef T argument_type;
	dist_object() :value_(0) 
	{}

	dist_object(T value)
	{
		value_ = value;
	}

	T& operator*() {
		return value_;
	}

	const T* operator->() const {
		return &value_;
	}
private:
	argument_type value_;
};


void test1() {
	std::vector<std::vector<int> > M(3, std::vector<int>(3, 1));
	dist_object<std::vector<std::vector<int> > > dist_matrix(M);
	dist_object<std::vector<std::vector<int> > >* dp = &dist_matrix;

	for (int i = 0; i < M.size(); i++) {
		for (int j = 0; j < M[0].size(); j++) {
			(*dist_matrix)[i][j] += 1;
			std::cout << dist_matrix->size() << "\t";
		}
		std::cout << std::endl;
	}
}


int main() {
	test1();
	system("pause");
}