#include <assert.h>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>

using namespace std;

struct Actor {

	int age_;
	string name_;
	Actor(int age, string name) : age_(age), name_(name) {}
	Actor() {}
};

template <typename T> struct server {
public:
	typedef vector<T> data_type;
	server() {}

	server(data_type data) : data_(data) {}

	server operator+(server b) {
		assert(data_.size() == (*b).size());
		data_type tmp;
		tmp.resize(data_.size());
		for (size_t i = 0; i < data_.size(); i++) {
			tmp[i] = data_[i] + (*b)[i];
		}
		server<T> res(tmp);
		return res;
	}
	
	void print() {
		for (size_t i = 0; i < data_.size(); i++) {
			cout << data_[i] << " ";
		}
		cout << endl;
	}

	size_t size() {
		return data_.size();
	}

	data_type &operator*() { return data_; }
private:
	data_type data_;
};

template <typename T> struct dist_object {
public:
	typedef T argument_type;
	typedef vector<argument_type> data_type;
	typedef server<argument_type> server_type;
	typedef vector<shared_ptr<server_type>> server_list_type;

	dist_object() : values_(server_list_type()) { values_.resize(1); }

	dist_object(size_t num_partitions) : values_(server_list_type()) {
		values_.resize(num_partitions);
	}

	dist_object(data_type &&values) : values_(std::move(values)) {}

	dist_object(data_type const &values) : values_(values) {}

	dist_object(data_type const &val, size_t num_partitions) {
		size_t num_partitions_tmp = num_partitions;
		size_t slice_begin = 0;
		size_t last_idx = val.size();
		while (num_partitions_tmp > 0) {			
			size_t block_size = (size_t)ceil(double(last_idx) / double(num_partitions_tmp));
			size_t slice_end = slice_begin + block_size;
			data_type partition_data = slice(val, slice_begin, slice_end);
			server<argument_type> partition(partition_data);
			values_.push_back(make_shared<server_type>(partition));
			slice_begin = slice_end;
			num_partitions_tmp--;
			last_idx -= block_size;
		}
	}

	data_type slice(data_type const &v, size_t m, size_t n)
	{
		auto first = v.cbegin() + m;
		auto last = v.cbegin() + n;

		data_type vec(first, last);
		return vec;
	}

	dist_object operator+(dist_object b) {
		assert(gather_num_partitions() == b.gather_num_partitions());
		dist_object result(gather_num_partitions());
		for (size_t i = 0; i < gather_num_partitions(); i++) {
			server<argument_type> tmp = *(values_[i]) + *(*b)[i];
			(*result)[i] = make_shared<server<argument_type>>(tmp);
		}
		return result;
	}

	server_list_type &operator*() { return values_; }

	size_t gather_num_partitions() { return values_.size(); }

	void print() {
		for (size_t i = 0; i < values_.size(); i++) {
			cout << "Part " << i << endl;
			for (size_t j = 0; j < (*(values_[i])).size(); j++) {
				cout << (*(*(values_[i])))[j] << " ";
			}
			cout << endl;
		}
	}

private:
	server_list_type values_;
};

void test1() {
	//vector<int> a(10, 1);
	//server<int> A(a);
	//cout << (*A)[1] << endl;

	vector<int> a{ 1, 3, 5, 7, 9, 11, 13, 15, 17, 19 };
	vector<int> b{ 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };

	dist_object<int> A(a, 2);
	dist_object<int> B(b, 2);
	cout << "A----" << endl;
	A.print();
	cout << "B----" << endl;
	B.print();
	auto C = A + B;
	cout << "C----" << endl;
	C.print();
	cout << "Done with ints!" << endl;
}

int main() {
	test1();
	system("pause");
}
