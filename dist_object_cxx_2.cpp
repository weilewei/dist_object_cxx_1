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
  typedef T argument_type;
  server() {}

  server(argument_type data) : data_(data) {}

  server operator+(server b) {
    assert(data_.size() == (*b).size());
    argument_type tmp;
    for (size_t j = 0; j < data_.size(); j++) {
      tmp.push_back(data_[j] + (*b)[j]);
    }
    server<argument_type> res(tmp);

    return res;
  }

  argument_type &operator*() { return data_; }

  argument_type data_;
};

ostream &operator<<(ostream &os, const Actor &dt) {
  os << dt.name_ << " is " << dt.age_ << " years old";
  return os;
}

Actor operator+(Actor &lhs, Actor &rhs) {
  Actor out = Actor(lhs.age_ + rhs.age_, lhs.name_ + rhs.name_);
  return out;
}

template <typename T> struct dist_object {
public:
  typedef T argument_type;
  typedef vector<shared_ptr<server<argument_type>>> server_list_type;

  dist_object() : values_(server_list_type()) { values_.resize(1); }

  dist_object(size_t num_partitions) : values_(server_list_type()) {
    values_.resize(num_partitions);
  }

  dist_object(server_list_type &&values) : values_(std::move(values)) {}

  dist_object(server_list_type const &values) : values_(values) {}

  dist_object(argument_type &val, size_t num_partitions) {
    auto it_first = val.begin();

    size_t last_idx = val.size();
    size_t tmp_block_nums = num_partitions;

    while (tmp_block_nums > 0) {
      argument_type tmp = argument_type();
      size_t double_block_sz =
          (size_t)ceil(double(last_idx) / double(tmp_block_nums));
      tmp.resize(double_block_sz);
      auto it_second = tmp.begin();
      auto next_ = std::next(it_first, double_block_sz);
      std::copy(it_first, next_, it_second);
      server<argument_type> tmp_server(tmp);
      values_.push_back(make_shared<server<argument_type>>(tmp_server));
      it_first = next_;
      tmp_block_nums--;
      last_idx -= double_block_sz;
    }
  }

  dist_object operator+(dist_object b) {
    assert(gather_num_partitions() == b.gather_num_partitions());
    dist_object ret(gather_num_partitions());
    for (size_t i = 0; i < gather_num_partitions(); i++) {
      server<argument_type> tmp = *(values_[i]) + *(*b)[i];
      (*ret)[i] = make_shared<server<argument_type>>(tmp);
    }
    return ret;
  }

  server_list_type &operator*() { return values_; }

  size_t gather_num_partitions() { return values_.size(); }

  void print() {
    for (size_t i = 0; i < values_.size(); i++) {
      cout << "Part " << i << endl;
      for (size_t j = 0; j < (**(values_[i])).size(); j++) {
        cout << (**values_[i])[j] << " ";
      }
      cout << endl;
    }
  }

private:
  server_list_type values_;
};

void test1() {

  vector<int> a{1, 3, 5, 7, 9, 11, 13, 15, 17, 19}; //(10, 1);
  vector<int> b{0, 2, 4, 6, 8, 10, 12, 14, 16, 18}; // (10, 2);

  dist_object<vector<int>> A(a, 5);
  dist_object<vector<int>> B(b, 5);
  // dist_object<vector<int>> C (5);

  auto C = A + B;
  C.print();
  cout << "Done with ints!" << endl;
  vector<string> d{"1",  "3",  "5",  "7",  "9",
                   "11", "13", "15", "17", "19"}; //(10, 1);
  vector<string> e{"0",  "2",  "4",  "7",  "9",
                   "11", "13", "15", "17", "19"}; //(10, 1);

  dist_object<vector<string>> D(d, 5);
  dist_object<vector<string>> E(e, 5);
  dist_object<vector<string>> F(5);
  F = D + E;
  F.print();

  Actor cTatum = Actor(21, "Channing Tatum");
  Actor gClooney = Actor(45, "George Clooney");
  Actor aBaldwin = Actor(45, "Alec Baldwin");
  vector<Actor> actors{cTatum, gClooney, aBaldwin};

  Actor jConnelly = Actor(45, "Jennifer Connelly");
  Actor dParton = Actor(55, "Dolly Parton");
  Actor cher = Actor(55, "Cher");
  vector<Actor> actresses{jConnelly, dParton, cher};

  dist_object<vector<Actor>> dudes(actors, 3);
  dist_object<vector<Actor>> chicks(actresses, 3);
  dist_object<vector<Actor>> couples(3);

  couples = dudes + chicks;
  couples.print();
}

int main() {
  test1();
  system("pause");
}
