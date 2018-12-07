#include <iostream>
#include <fstream>
#include <vector>
#import <algorithm>

using namespace std;

void gen_test() {
    ofstream out("input.bin", ios::binary);
    //int n = 10;
    int n = 2;
    out.write((char *)&n, 4);
    //char a[10] = {4, 2, 8, 8, 4, 9, 0, 4, 7, 1};
    char a[2] = {9, 1};
    for (int i = 0; i < n; ++i) {
        out.write((char *)&a[i], 1);
    }

    //int m = 19;
    int m = 2;
    //char b[19] = {8, 6, 1, 2, 2, 3, 2, 4, 1, 6, 7, 2, 5, 1, 1, 7, 1, 5, 5};
    char b[2] = {1, 2};
    out.write((char *)&m, 4);
    for (int i = 0; i < m; ++i) {
        out.write((char *)&b[i], 1);
    }
    out.close();
}

int main()
{
    //gen_test();
    ifstream in("input.bin", ios::binary);
    ofstream out("output_t.bin", ios::binary);



    int n_size, m_size, o_size = 0;
    in.read((char *)&n_size, 4);
    in.seekg(4 + n_size);
    in.read((char *)&m_size, 4);


    int readBytes = 0;
    int chunk = 50000;
    int processed_bytes = 0;
    int carry = 0;

    while (readBytes < n_size + m_size) {

        int n_last = n_size - processed_bytes;
        int n = 0;
        if (n_last > 0) {
            n = n_last >= chunk ? chunk : n_last;
        }


        vector<unsigned char> a(n);
        if (n > 0) {
            in.seekg(4 + n_size - processed_bytes - n);
            in.read((char*)a.data(), n);
        }



        int m_last = m_size - processed_bytes;
        int m = 0;
        if (m_last > 0) {
            m = m_last >= chunk ? chunk : m_last;
        }


        vector<unsigned char> b(m);
        //char b[10];
        int m_offset = 4 + 4 + n_size + m_size - processed_bytes - m;
        if (m > 0) {
            in.seekg(m_offset);
            in.read((char*)&b[0], m);
        }



        vector<unsigned char> sum(max(n, m));
        reverse(a.begin(), a.end());
        reverse(b.begin(), b.end());
        //reverse(b, b + m);

        for (int j = 0; j < max(n, m); ++j) {
            int tempSum = 0;
            if (j < n) {
                tempSum += a[j];
            }

            if (j < m) {
                tempSum += b[j];
            }

            tempSum += carry;
            sum[j] = tempSum % 10;
            carry = tempSum / 10;
        }

        //reverse(sum.begin(), sum.end());

        o_size += sum.size();
        out.seekp(4 + processed_bytes);
        out.write((char *)sum.data(), sum.size());

        readBytes += n + m;
        processed_bytes += sum.size();
    }

    if (carry != 0) {
        o_size++;
        out.seekp(4 + processed_bytes);
        out.write((char *)&carry, 1);
    }

    out.seekp(0);
    out.write((char *)&o_size, 4);
    out.close();


    ifstream in_final("output_t.bin", ios::binary);
    ofstream out_final("output.bin", ios::binary);

    out_final.write((char *)&o_size, 4);

    readBytes = 0;
    int chunk_count = 0;
    while (readBytes < o_size) {
        int to_read = o_size - chunk_count * chunk > chunk ? chunk : o_size - chunk_count * chunk;
        vector<unsigned char> a(to_read);
        in_final.seekg(4 + chunk_count * chunk);
        in_final.read((char*)a.data(), to_read);

        reverse(a.begin(), a.end());
        out_final.seekp(4 + o_size - chunk_count * chunk - to_read);
        out_final.write((char *)a.data(), a.size());

        chunk_count++;
        readBytes += to_read;
    }

    out_final.close();
}