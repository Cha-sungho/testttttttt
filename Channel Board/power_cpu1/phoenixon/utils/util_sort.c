/*
 * util_sort.c
 *
 *  Created on: 2022. 2. 6.
 *      Author: boomw
 */

void util_InsertionSort(float *Data, int Number)
{
    int i, j;
    float Key;

    for (i = 1; i <= Number; i++) {
        Key = Data[i];
        j = i - 1;
        while ((j >= 0) && (Data[j] > Key)) {
            Data[j + 1] = Data[j];
            j--;
        }
        Data[j + 1] = Key;
    }
}
