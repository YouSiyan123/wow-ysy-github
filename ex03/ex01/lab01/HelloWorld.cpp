#include <iostream>
#include <string>
using namespace std;

int main() {
    cout << "Hello, World!" << endl;
    string name;
    cout << "请输入你的名字：";
    getline(cin, name); 
    cout << "你好，" << name << "！" << endl;
    return 0;
}
