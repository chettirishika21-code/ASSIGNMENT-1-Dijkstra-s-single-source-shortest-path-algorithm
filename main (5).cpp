#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>

using namespace std;

// Forward Declarations
void display_packet(const vector<int>& packet, const string& title);
string vector_to_string(const vector<int>& bits); // NEW: Added forward declaration
int determine_redundant_bits(int m);
vector<int> encode_data(const string& data_str, int& r);
vector<int> set_parity_bits(vector<int> data_packet, int r);
int decode_and_correct_error(vector<int>& received_packet, int r);

/**
 * @brief Helper function to convert a vector<int> of bits back to a string.
 */
string vector_to_string(const vector<int>& bits) {
    string result = "";
    for (int bit : bits) {
        result += to_string(bit);
    }
    return result;
}


/**
 * @brief Displays the data packet in a user-friendly format, indicating parity and data bits.
 * @param packet The vector of bits to display.
 * @param title A descriptive title for the output.
 */
void display_packet(const vector<int>& packet, const string& title) {
    cout << "\n" << title << ":" << endl;
    cout << "Index (Pos): ";
    for (size_t i = 1; i <= packet.size(); ++i) {
        cout << (i < 10 ? " " : "") << i << " ";
    }
    cout << endl;
    
    cout << "Bit (Value): ";
    for (size_t i = 0; i < packet.size(); ++i) {
        // Mark parity positions with P or R (Redundant)
        int pos = i + 1;
        bool is_parity = (pos & (pos - 1)) == 0;
        
        if (is_parity) {
            // log2(pos) gives the power 'k' (0, 1, 2, 3...)
            cout << "P" << (int)log2(pos) + 1 << " "; // Cast log2 result to int
        } else {
            cout << "  "; // Data bit
        }
    }
    cout << endl;

    cout << "Bit (Value): ";
    for (int bit : packet) {
        cout << bit << "  ";
    }
    cout << endl;
}

/**
 * @brief Determines the number of redundant bits (r) required for 'm' data bits.
 * The relationship is 2^r >= m + r + 1.
 * @param m The number of data bits.
 * @return int The required number of redundant bits (r).
 */
int determine_redundant_bits(int m) {
    int r = 0;
    while (pow(2, r) < m + r + 1) {
        r++;
    }
    return r;
}

/**
 * @brief Inserts redundant bits into the correct 2^k positions.
 * @param data_str The input data string.
 * @param r Output parameter to store the determined number of redundant bits.
 * @return vector<int> The data packet with redundant bit placeholders (0).
 */
vector<int> encode_data(const string& data_str, int& r) {
    int m = data_str.length();
    r = determine_redundant_bits(m);
    int n = m + r; // Total length of the packet

    vector<int> data_packet(n);
    int data_idx = 0;

    // Place data bits and redundant bit placeholders (0)
    for (int i = 1; i <= n; ++i) {
        // Check if the position is a power of 2 (parity position)
        if ((i & (i - 1)) == 0) {
            data_packet[i - 1] = 0; // Placeholder for parity bit
        } else {
            // Place data bit
            data_packet[i - 1] = data_str[data_idx] - '0';
            data_idx++;
        }
    }
    
    return data_packet;
}

/**
 * @brief Calculates and sets the parity bits using even parity logic.
 * @param data_packet The packet with placeholder redundant bits.
 * @param r The number of redundant bits.
 * @return vector<int> The final encoded packet.
 */
vector<int> set_parity_bits(vector<int> data_packet, int r) {
    int n = data_packet.size();

    for (int i = 0; i < r; ++i) {
        int parity_pos = pow(2, i); // Parity position (1, 2, 4, 8, ...)
        int count = 0;

        // Iterate through all bits whose position index (1-based) has a 1 in the i-th bit position
        for (int j = 1; j <= n; ++j) {
            // The position is j. Check if the i-th bit of j is 1.
            // E.g., for r1 (i=0), check bits with index 1, 3, 5, 7...
            if (j & parity_pos) {
                // If it's the parity position itself, skip it for calculation
                if (j == parity_pos) continue; 
                
                // Count the number of 1s
                if (data_packet[j - 1] == 1) {
                    count++;
                }
            }
        }
        
        // Even Parity: Parity bit is set such that the total number of 1s in the checked positions is even.
        // The value is simply count % 2.
        data_packet[parity_pos - 1] = count % 2; 
    }
    
    return data_packet;
}

/**
 * @brief Simulates the receiver, recalculates parity, detects error, and corrects it.
 * @param received_packet The data packet as received. This vector is modified if an error is corrected.
 * @param r The number of redundant bits.
 * @return int The position of the error (0 if no error).
 */
int decode_and_correct_error(vector<int>& received_packet, int r) {
    int n = received_packet.size();
    vector<int> syndrome_bits(r); // To store the parity check results (P_r...P_1)
    int error_pos = 0; // The decimal value of the syndrome

    for (int i = 0; i < r; ++i) {
        int parity_pos = pow(2, i);
        int count = 0;
        
        // Recalculate parity (including the parity bit itself)
        for (int j = 1; j <= n; ++j) {
            // Check all bits whose position index (1-based) has a 1 in the i-th bit position
            if (j & parity_pos) {
                if (received_packet[j - 1] == 1) {
                    count++;
                }
            }
        }

        // Syndrome bit: Should be 0 for even parity if correct
        // If the total count of 1s is odd, the syndrome bit is 1 (error).
        int syndrome_bit = count % 2;
        syndrome_bits[i] = syndrome_bit;
        
        // Build the error position (syndrome) in decimal form
        // Note: The syndrome bits are stored in the order P1, P2, P4... (Least significant bit first)
        if (syndrome_bit == 1) {
            error_pos += parity_pos;
        }
    }

    // The error_pos is the 1-based index of the error.
    if (error_pos != 0) {
        cout << "\n--- ERROR DETECTED ---" << endl;
        // The syndrome bits are P1, P2, P4... so we reverse for typical display (P_r...P1)
        reverse(syndrome_bits.begin(), syndrome_bits.end());
        cout << "Syndrome (P" << r << "...P1): " << vector_to_string(syndrome_bits) << endl;
        
        cout << "Error Position (Decimal): " << error_pos << endl;

        // Correct the single-bit error
        if (error_pos <= n) {
            received_packet[error_pos - 1] = received_packet[error_pos - 1] ^ 1; // Flip the bit
            cout << "Error Corrected: Bit at position " << error_pos << " flipped." << endl;
        }
    } else {
        cout << "\n--- NO ERROR DETECTED ---" << endl;
    }
    
    return error_pos;
}


int main() {
    string data_str;
    cout << "Enter a binary data string (e.g., 1011001): ";
    // Check if input is empty or invalid before processing (a simple read)
    if (!(cin >> data_str)) {
        cerr << "Error: Failed to read input." << endl;
        return 1;
    }

    if (data_str.empty()) {
        cerr << "Error: Input data cannot be empty." << endl;
        return 1;
    }
    // Check if input contains only 0s and 1s
    if (data_str.find_first_not_of("01") != string::npos) {
        cerr << "Error: Input must be a binary string (only '0' and '1')." << endl;
        return 1;
    }

    int r;
    
    // --- STEP 1: ENCODING (Sender Side) ---
    cout << "\n=====================================" << endl;
    cout << "        SENDER: ENCODING DATA        " << endl;
    cout << "=====================================" << endl;

    // Determine redundant bits and insert placeholders
    vector<int> packet_with_placeholders = encode_data(data_str, r);
    int m = data_str.length();
    int n = packet_with_placeholders.size();
    
    cout << "Data Bits (m): " << m << endl;
    cout << "Redundant Bits (r): " << r << " (2^" << r << " >= " << m << " + " << r << " + 1)" << endl;
    cout << "Total Packet Length (n): " << n << endl;

    // Calculate and set the parity bits
    vector<int> final_transmitted_packet = set_parity_bits(packet_with_placeholders, r);
    display_packet(final_transmitted_packet, "Final Transmitted Data Packet (Encoded)");
    
    // Display the transmitted string explicitly as requested
    cout << "\nActual Transmitted Bit String (1-based index):" << endl;
    cout << vector_to_string(final_transmitted_packet) << endl;


    // --- STEP 2: SIMULATING ERROR (Channel) ---
    cout << "\n=====================================" << endl;
    cout << "     CHANNEL: SIMULATING ERROR     " << endl;
    cout << "=====================================" << endl;
    
    vector<int> received_packet = final_transmitted_packet;
    int error_pos_simulated = 0;

    // Example 1: No error (Default test case)
    cout << "Simulating NO error for the first test." << endl;

    // Example 2: Introduce single-bit error (Uncomment to test correction)
    // The previous code had a syntax error here, which is now fixed.
    /*
    error_pos_simulated = 7;
    if (error_pos_simulated > 0 && error_pos_simulated <= n) {
        received_packet[error_pos_simulated - 1] ^= 1; // Flip the bit
        cout << "Simulating single-bit error at position (1-based): " << error_pos_simulated << endl;
        display_packet(received_packet, "Received Packet (with simulated error)");
    }
    */


    // --- STEP 3: DECODING (Receiver Side) ---
    cout << "\n=====================================" << endl;
    cout << "       RECEIVER: DECODING DATA       " << endl;
    cout << "=====================================" << endl;

    // Error detection and correction logic
    int detected_error_pos = decode_and_correct_error(received_packet, r);

    if (detected_error_pos == 0) {
        cout << "Result: Correct data packet received." << endl;
    } else {
        cout << "Result: Error detected at position " << detected_error_pos << " and corrected." << endl;
        display_packet(received_packet, "Corrected Data Packet");
    }
    
    return 0;
}

OUTPUT:
[?2004l
Enter a binary data string (e.g., 1011001): 101100011

=====================================
        SENDER: ENCODING DATA        
=====================================
Data Bits (m): 9
Redundant Bits (r): 4 (2^4 >= 9 + 4 + 1)
Total Packet Length (n): 13

Final Transmitted Data Packet (Encoded):
Index (Pos):  1  2  3  4  5  6  7  8  9 10 11 12 13 
Bit (Value): P1 P2   P3       P4           
Bit (Value): 1  1  1  0  0  1  1  0  0  0  0  1  1  

Actual Transmitted Bit String (1-based index):
1110011000011

=====================================
     CHANNEL: SIMULATING ERROR     
=====================================
Simulating NO error for the first test.

=====================================
       RECEIVER: DECODING DATA       
=====================================

--- NO ERROR DETECTED ---
Result: Correct data packet received.
[?2004h