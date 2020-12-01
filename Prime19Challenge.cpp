// Prime19Challenge.cpp : This file contains the 'main' function. Program execution begins and ends there.
// MPMP19: THE 19 CHALLENGE Solution
// by Christoph Beeler
// Switzerland

#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <stdio.h>

using namespace std;

#include <mpir.h>

//Set location of prime output file
string primeFile = "primeOutputBin.bin";
//string primeFile = "C:\\Temp\\primeOutputBin.bin";
//string primeFile = "C:\\Daten\\primeOutputBin.bin";

//Set max value for prime search
uint64_t primeMax = (uint64_t)400000;  //5e10 works 

//sieveOfEratosthenesFile adapted from https://www.geeksforgeeks.org/sieve-of-eratosthenes/
void sieveOfEratosthenesFile(uint64_t n)
{
    // Create a boolean array "prime[0..n]" and initialize all entries as true. 
    // A value in prime[i] will finally be false if i is Not a prime, else true. 
    vector<bool> pBool(n + 1, true);

    // Open binary file for prime output
    ofstream binfile(primeFile, ios::out | ios::binary | ios::ate);

    if (binfile.is_open())
    {
        chrono::steady_clock::time_point begin = chrono::steady_clock::now();
        for (uint64_t p = 2; p * p <= n; p++)
        {
            // If pBool[p] is not changed, then it is a prime 
            if (pBool[p] == true)
            {
                // Update all multiples of p greater than or equal to the square of it 
                // numbers which are multiple of p and are less than p^2 have already been marked.  
                for (uint64_t i = p * p; i <= n; i += p)
                    pBool[i] = false;
            }
        }
        chrono::steady_clock::time_point end = chrono::steady_clock::now();
        cout << "Sieve Loop: " << chrono::duration_cast<chrono::microseconds>(end - begin).count() << "us" << endl;

        begin = chrono::steady_clock::now();
        // Print all prime numbers 
        for (uint64_t p = 2; p <= n; p++)
        {
            if (pBool[p])
            {
                binfile.write((char*)&p, sizeof(uint64_t));
            }
        }
        binfile.close();
        end = chrono::steady_clock::now();
        cout << "File Fill: " << chrono::duration_cast<chrono::microseconds>(end - begin).count() << "us" << endl;
    }
}

void sumModCheckFile(vector<uint64_t>& outN)
{
    //Init Multiple Precision Integers
    //sum of primes up to n
    mpz_t sum;
    mpz_init(sum);
    mpz_set_ui(sum, 0);

    //sum after mod n
    mpz_t sum_mod;
    mpz_init(sum_mod);
    mpz_set_ui(sum_mod, 0);

    //variable for prime p
    mpz_t p_v;
    mpz_init(p_v);

    //open prime values file again
    ifstream binfile(primeFile, ios::in | ios::binary | ios::ate);

    //start timer
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

    //get size of file and calculate number of primes
    uint64_t size = binfile.tellg();
    uint64_t n = size / sizeof(uint64_t);
    uint64_t p;
    binfile.seekg(0, ios::beg);    
    for (uint64_t i = 1; i < n; i++)
    {
        //read next prime
        binfile.read((char*)&p, sizeof(uint64_t));

        
        mpz_set_ui(p_v, p);     //set prime
        mpz_mul(p_v, p_v, p_v);     //square prime   
        mpz_add(sum, sum, p_v);     //add to sum of all primes

        mpz_mod_ui(sum_mod, sum, i);        //calculate mod i (n)

        if (mpz_sgn(sum_mod) == 0)          //if 0, new value found
        {
            cout << "Found: " << i << endl;
            outN.push_back(i);
        }

        if (i % 10000000 == 0)      //show progress
            cout << i << " of " << n << " " << p << "\n";
    }
    //output duration
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    cout << "Mod Loop: " << chrono::duration_cast<chrono::microseconds>(end - begin).count() << "us" << endl;
}


int main(void)
{
    vector<uint64_t> outN;

    cout << "Input top bound for prime search (e.g. 400000):";

    cin >> primeMax;

    cout << "Starting Prime generation:\n";

    sieveOfEratosthenesFile(primeMax);

    sumModCheckFile(outN);

    cout << "\n-----------------------------------\n";
    cout << "Found: ";
    for (auto v : outN)
        std::cout << v << ' ';
    cout << "\n";
    cout << "-----------------------------------\n";

    return 0;
}
