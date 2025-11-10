#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

// Function to perform XOR operation on two equal-length binary strings
string xor_op(string a, string b) {
    string result = "";
    // Note: Both strings must be of the same length (generator length)
    for (size_t i = 0; i < a.length(); i++) {
        if (a[i] == b[i]) {
            result += '0';
        } else {
            result += '1';
        }
    }
    return result;
}

// Function to perform CRC calculation
string calculateCRC(string data, string generator) {
    int data_len = data.length();
    int gen_len = generator.length();
    int k = gen_len - 1; // Degree of the generator

    // 1. Augment the data by appending k zeros
    string augmented_data = data + string(k, '0');

    // Make a mutable copy for the division process
    string dividend = augmented_data;

    // 2. Perform the Modulo-2 (XOR) Division
    for (int i = 0; i < data_len; i++) {
        // Only proceed if the leading bit is '1'
        if (dividend[i] == '1') {
            // XOR the current segment of the dividend with the generator
            string segment = dividend.substr(i, gen_len);
            string remainder_segment = xor_op(segment, generator);

            // Replace the segment in the dividend with the remainder, 
            // but keep the leading 0 (which is always 0 after a '1' is XORed with a '1')
            // This effectively performs the "shift" in the division
            dividend.replace(i, gen_len, remainder_segment);
        }
    }

    // 3. The last k bits of the final dividend is the CRC remainder (checksum)
    string remainder = dividend.substr(data_len, k); 
    
    return remainder;
}

int main() {
    // Input parameters for the problem
    string data_stream = "1101011011";
    // Generator G(x) = x^4 + x + 1 => 1*x^4 + 0*x^3 + 0*x^2 + 1*x^1 + 1*x^0
    string generator_bits = "10011"; 

    cout << "--- CRC Codeword Calculation ---" << endl;
    cout << "Data Stream (M):       " << data_stream << endl;
    cout << "Generator (G):         " << generator_bits << " (x^4 + x + 1)" << endl;
    
    // Calculate the CRC remainder
    string crc_checksum = calculateCRC(data_stream, generator_bits);

    // Form the actual transmitted bit string
    string transmitted_codeword = data_stream + crc_checksum;

    cout << "---" << endl;
    cout << "Calculated Checksum (R): " << crc_checksum << endl;
    cout << "Actual Bit String Transmitted (T): " << transmitted_codeword << endl;
    
    return 0;
}