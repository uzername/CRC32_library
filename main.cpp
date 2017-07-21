#include <cstdlib>
#include <cstdio>

#include "CRC32process.h"

using namespace std;

uint8_t test_array[] = {0x32, 0x30, 0x31, 0x37, 0x2D, 0x30, 0x37, 0x2D, 0x31, 
0x39, 0x20, 0x31, 0x34, 0x3A, 0x34, 0x35, 0x3A, 0x30, 0x35, 0x2E, 0x34, 0x32, 
0x36, 0x0A, 0x20, 0x2D, 0x2D, 0x3E, 0x32, 0x30, 0x31, 0x37, 0x2D, 0x30, 0x37, 
0x2D, 0x31, 0x39, 0x20, 0x31, 0x34, 0x3A, 0x34, 0x35, 0x3A, 0x30, 0x35, 0x2E, 
0x34, 0x33, 0x36, 0x0A, 0x20, 0x2D, 0x2D, 0x3E, 0x32, 0x30, 0x31, 0x37, 0x2D, 
0x30, 0x37, 0x2D, 0x31, 0x39, 0x20, 0x31, 0x34, 0x3A, 0x34, 0x35, 0x3A, 0x30, 
0x35, 0x2E, 0x34, 0x34, 0x37, 0x0A, 0x20, 0x2D, 0x2D, 0x3E, 0x32, 0x30, 0x31, 
0x37, 0x2D, 0x30, 0x37, 0x2D, 0x31, 0x39, 0x20, 0x31, 0x34, 0x3A, 0x34, 0x35, 
0x3A, 0x30, 0x35, 0x2E, 0x34, 0x35, 0x38, 0x0A, 0x20, 0x2D, 0x2D, 0x3E, 0x32, 
0x30, 0x31, 0x37, 0x2D, 0x30, 0x37, 0x2D, 0x31, 0x39, 0x20, 0x31, 0x34, 0x3A, 
0x34, 0x35, 0x3A, 0x30, 0x35, 0x2E, 0x34, 0x37, 0x33}; //135 elements
uint32_t crcTable[256];
int main(int argc, char** argv) {
    printf("HANDLING CRC32 IN C (calculate CRC over test data, streamed and nonstreamed)\n");
    CRCModelTypeDef crcConfigInstance;
    crcConfigInstance.InputReflected = 1; crcConfigInstance.ResultReflected = 1;
    crcConfigInstance.Polynomial = 0x04C11DB7;
    crcConfigInstance.Initial = 0xFFFFFFFF;
    crcConfigInstance.FinalXor = 0xFFFFFFFF;
    printf("===Table-based CRC32===\n");
    CalculateCrcTable(crcTable, &crcConfigInstance);
    uint32_t testPerformed = Compute_CRC32(test_array, 135, &crcConfigInstance, crcTable);
    printf("CRC32 = %x (%d)\n", testPerformed,testPerformed);
    
    uint32_t testPerformed2 = crcConfigInstance.Initial;
    for (int i = 0; i<135; i++) {
        testPerformed2 = Compute_CRC32_streamed(testPerformed2, test_array[i], &crcConfigInstance, crcTable);
    }
    testPerformed2 = Finalize_CRC32_streamed(testPerformed2, &crcConfigInstance);
    
    printf("CRC32(streamed) = %x (%d)\n",testPerformed2,testPerformed2);
    printf("===Offset-based CRC32===\n");
    //use crc table from previous stage
    uint32_t testPerformed3 = ComputeCRC32_Simple_ShiftReg(test_array, 135, &crcConfigInstance);
    printf("CRC32 = %x (%d)\n", testPerformed3,testPerformed3);
    
    uint32_t testPerformed4 = GetInitialShiftRegister(test_array,4, &crcConfigInstance);
    //if length of test_array is less than4 ,then pass the true length to GetInitialShiftRegister and finalize obtained value
    //we have skipped first 4 bytes when determined initial state of shift register but algorithm should iterate No of times equal to length of array
    for (int i = 4; i<135+4; i++) {
        uint8_t byte2use = (i<135) ? test_array[i] : (uint8_t)0;
        testPerformed4 = ComputeCRC32_Simple_ShiftReg_Streamed(testPerformed4, byte2use, &crcConfigInstance);
    }
    testPerformed4 = Finalize_CRC32_streamed(testPerformed4, &crcConfigInstance);
    printf("CRC32 (streamed) = %x (%d)\n", testPerformed4,testPerformed4);
    
    getchar();
    return 0;
}

