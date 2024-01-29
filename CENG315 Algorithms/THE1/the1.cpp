#include "the1.h"

template <typename T>
void my_swap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}
double my_floor(double x) {
    int xi = static_cast<int>(x); 
    return (x < 0 && x != xi) ? xi - 1 : xi;
}
template <typename T>
T my_max(T a, T b) {
    return (a > b) ? a : b;
}
template <typename T>
T my_min(T a, T b) {
    return (a < b) ? a : b;
}

void swapAndUpdate(unsigned short& a, unsigned short& b, long& swap, double& total_dist, double& avg_dist, double& max_dist, int i, int j) 
{
    my_swap(a, b);
    swap= swap +1;
    double dist =  j-i;
    total_dist += dist;
    avg_dist = total_dist / swap;
    max_dist = my_max(max_dist, dist);
}

int lomutoPartition(unsigned short* arr, int low, int high, long& swap, double& total_dist, double& avg_dist, double& max_dist) {
    unsigned short pivot = arr[high];
    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] >= pivot) {
            i++;
            swapAndUpdate(arr[i], arr[j], swap, total_dist, avg_dist, max_dist, i, j);
        }
    }
    swapAndUpdate(arr[i + 1], arr[high], swap, total_dist, avg_dist, max_dist, i + 1, high);
    return i + 1;
}

int hoarePartition(unsigned short* arr, int low, int high, long& swap, double& total_dist, double& avg_dist, double& max_dist) {
    int mid = my_floor((low+high)/2);
    unsigned short pivot = arr[mid];
    int i = low - 1;
    int j = high + 1;
    while (true) {
        do {
            j--;
        } while (arr[j] < pivot);
        do {
            i++;
        } while (arr[i] > pivot);
        if (i < j) {
            swapAndUpdate(arr[i], arr[j], swap, total_dist, avg_dist, max_dist, i, j);
        } else {
            return j;
        }
    }
}

int medianOfThree(unsigned short* arr, int low, int high) {

    int mid = my_floor((low+high)/2);
    if(low != mid && low != high && mid != high)
    {
        int median = my_max(my_min(arr[low],arr[mid]), my_min(my_max(arr[low],arr[mid]),arr[high]));
        if (median == arr[low] && median != arr[mid] && median != arr[high]){return low;}
        if (median == arr[mid] && median != arr[low] && median != arr[high]){return mid;}
        if (median == arr[high]&& median != arr[low] && median != arr[mid]){return high;}
    }
    return -1;
}

int quickSortRec(unsigned short* arr, int low, int high, long& swap, double& total_dist, double& avg_dist, double& max_dist, bool hoare, bool median_of_3) {
    if (low < high) {
        int p;
        int mid;

        if (hoare) 
        {
            if(median_of_3 && high > 2)
            {
                int pivot_hoare = my_floor((low+high)/2);
                mid = medianOfThree(arr, low, high);
                if(mid != pivot_hoare && mid != -1)
                {
                    swapAndUpdate(arr[mid], arr[pivot_hoare], swap, total_dist, avg_dist, max_dist, mid, pivot_hoare);
                }
            }
            p = hoarePartition(arr, low, high, swap, total_dist, avg_dist, max_dist);
            return  1 + quickSortRec(arr, low, p, swap, total_dist, avg_dist, max_dist, hoare, median_of_3) + quickSortRec(arr, p + 1, high, swap, total_dist, avg_dist, max_dist, hoare, median_of_3);
        } 
        else if (!hoare)
        {
            if (median_of_3)
            {
                int pivot_lomuto = high;
                mid = medianOfThree(arr, low, high);
                if(mid != pivot_lomuto && mid != -1)
                {
                    swapAndUpdate(arr[mid], arr[pivot_lomuto], swap, total_dist, avg_dist, max_dist, mid, pivot_lomuto);    
                }
            }
            p = lomutoPartition(arr, low, high, swap, total_dist, avg_dist, max_dist);
            return  1 + quickSortRec(arr, low, p - 1, swap, total_dist, avg_dist, max_dist, hoare, median_of_3) + quickSortRec(arr, p + 1, high, swap, total_dist, avg_dist, max_dist, hoare, median_of_3);
        }
    }
    return 1;
}

int quickSort(unsigned short* arr, long &swap, double &avg_dist, double &max_dist, bool hoare, bool median_of_3, int size) {
    swap = 0;
    avg_dist = 0.0;
    max_dist = 0.0;
    double total_dist = 0.0; 
    return quickSortRec(arr, 0, size - 1, swap, total_dist, avg_dist, max_dist, hoare, median_of_3);
}
