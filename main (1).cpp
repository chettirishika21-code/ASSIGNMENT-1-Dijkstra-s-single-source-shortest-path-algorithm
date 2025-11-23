#include <iostream>
using namespace std;

string xor_op(string a, string b) {
    string r="";
    for(int i=1;i<b.size();i++)
        r += (a[i]==b[i])?'0':'1';
    return r;
}

string mod2div(string dividend, string divisor) {
    int pick = divisor.size();
    string tmp = dividend.substr(0, pick);

    while (pick < dividend.size()) {
        if (tmp[0] == '1')
            tmp = xor_op(tmp, divisor) + dividend[pick];
        else
            tmp = xor_op(tmp, string(pick, '0')) + dividend[pick];
        pick++;
    }

    if (tmp[0] == '1')
        tmp = xor_op(tmp, divisor);
    else
        tmp = xor_op(tmp, string(pick, '0'));

    return tmp;
}

int main() {
    string data, div;
    cout << "Enter data bits: ";
    cin >> data;
    cout << "Enter divisor: ";
    cin >> div;

    string appended = data + string(div.size()-1, '0');
    cout << "Appended data: " << appended << endl;

    string rem = mod2div(appended, div);
    cout << "CRC Remainder: " << rem << endl;

    string codeword = data + rem;
    cout << "Codeword: " << codeword << endl;

    cout << "\nEnter received codeword: ";
    string recv;
    cin >> recv;

    string r = mod2div(recv, div);
    if (r.find('1') != string::npos)
        cout << "Error detected.\n";
    else
        cout << "No error detected.\n";
}