long binary_search_it(long *begin, long *end, long value) {
    long *mid;
    long *initial_begin = begin;
    while (begin < end) {
        mid = begin + ((end - begin) / 2);
        if (*mid == value) {
            return mid - initial_begin + 1;
        } else if (*mid < value) {
            begin = mid + 1;
        } else {
            end = mid;
        }
    }
    return -1;
}

long binary_search_rec(long *begin, long *end, long value) {
    long *mid = begin + ((end - begin) >> 1);
    long tmp;
    if (begin >= end) {
        return -1;
    } else if (*mid == value) {
        return mid - begin + 1;
    } else if (*mid < value) {
        tmp = binary_search_rec(mid + 1, end, value);
        if(tmp != -1) {
            return tmp + (mid - begin) + 1;
        } else {
            return tmp;
        }
    } else {
        return binary_search_rec(begin, mid, value);
    }
}

void merge(long *begin1, long *end1, long *begin2, long *end2, long *begin3) {
    while(begin1 < end1 && begin2 < end2) {
        if (*begin1 < *begin2) {
            *begin3++ = *begin1++;
        } else {
            *begin3++ = *begin2++;
        }
    }
    while (begin1 < end1) {
        *begin3++ = *begin1++;
    }
    while (begin2 < end2) {
        *begin3++ = *begin2++;
    }
}

void switch8(long *begin1, long *begin2, long length) {
    while(length--) {
        switch(*begin1 % 8) {
            case 0:
                *begin2 = *begin1+1;
                break;
            case 1:
                *begin2 = *begin1*5;
                break;
            case 2:
                *begin2 = *begin1/2;
                break;
            case 3:
                *begin2 = *begin1%3;
                break;
            case 4:
                *begin2 = *begin1-7;
                break;
            case 5:
                *begin2 = 331;
                break;
            case 6:
                *begin2 = *begin1+10;
                break;
            case 7:
            default:
                *begin2 = *begin1/3;      
        }
        *begin2 += 16;
        begin1++;
        begin2++;
    }
}
