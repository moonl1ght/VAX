//
// Created by Alexander Lakhonin on 12.02.2025.
//

#include "TESTCPP.hpp"

#include <iostream>
#include <vector>

using namespace std;

struct Data {
  int value;
};

class MyClass {
public:
  int offset;
  Data& data; // Reference to Data
//  int offset;
  MyClass(Data& d, int offset) : data(d), offset(offset) {}
};

void runTESTCPP() {
  cout << "run test" << endl;
  vector<MyClass> vec;
  Data d1(1), d2(2), d3(3);
  vec.push_back(MyClass(d1, 0));
  vec.push_back(MyClass(d2, 1));
  vec.push_back(MyClass(d3, 2));

  Data* ptrData = &vec[0].data;

  MyClass* clp = vec.data();

  Data * const * pd = &ptrData;
  for (int i = 0; i < 3; ++i) {
    cout << clp[i].data.value << endl;
  }

  for (int i = 0; i < 3; ++i) {
    cout << pd[i]->value << endl;
  }

  cout << "end run test" << endl;
}
