#include "List.h"


int main() {
    List<unsigned> list;
    for(int i = 1; i<= 16; i++){
    list.insertBack(i);
  }

    list.print();
    std::cout << std::endl;

    list.reverseNth(5);


    list.print();
    std::cout << std::endl;







  return 0;
}
