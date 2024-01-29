#include "the2.h"

void my_reverse(std::string* arr, int size) 
{
    int start = 0;
    int end = size - 1;

    while (start < end) 
    {
        std::string temp = arr[start];
        arr[start] = arr[end];
        arr[end] = temp;
        start++;
        end--;
    }
}

int counting_sort(std::string* arr, int size, int exp, bool ascending) 
{
    const int k = 27; 
    int iterations = 0;
    int* count = new int[k]();

    for (int i = 0; i < size; i++) 
    {
        int index = (exp < arr[i].length()) ? (arr[i][exp] - 'A' + 1) : 0;
        count[index]++;
        iterations++;
    }

    for (int i = 1; i < k; i++) 
    {
        count[i] += count[i - 1];
        iterations++;
    }

    std::string* output = new std::string[size];

    for (int i = size - 1; i >= 0; i--) 
    {
        int index = (exp < arr[i].length()) ? (arr[i][exp] - 'A' + 1) : 0;
        output[count[index] - 1] = arr[i];
        count[index]--;
        iterations++;
    }

    for (int i = 0; i < size; i++) 
    {
        arr[i] = output[i];
        iterations++; 
    }

    delete[] output;
    delete[] count;
    return iterations;
}

int radix_string_sort(std::string* arr, int size, bool ascending) 
{
    int max_length = 0;
    for (int i = 0; i < size; i++) 
    {
        max_length = std::max(max_length, static_cast<int>(arr[i].length()));
    }

    int total_iterations = 0;

    for (int digit = max_length-1; digit >= 0; digit--) 
    {
        total_iterations += counting_sort(arr, size, digit, ascending);
    }
    
    if(!ascending){my_reverse(arr,size);}
    return total_iterations;
}
