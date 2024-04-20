#include <iostream>
#include "NorPix_sequence.h"

using namespace std;

int main()
{
    cout << "Hello World!" << endl;

    NorPix_sequence seq_obj;

    seq_obj.open_for_read("test.seq");
    //seq_obj.parse_header();

    return 0;
}
