#ifndef CRC32PROCESS_H
#define CRC32PROCESS_H
   #include "stdint.h"
#define TOPBIT_MASK 0x80000000
//3line block of special declaration to use C code with C++ compiler
//see https://stackoverflow.com/a/17448074
#ifdef __cplusplus
extern "C" {
#endif
//code for CRC32 processing. Now streamed 
// http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html
typedef struct {
    uint32_t Initial;
    uint8_t InputReflected;
    uint8_t ResultReflected;
    uint32_t FinalXor;
    uint32_t Polynomial;
    
} CRCModelTypeDef;
//CRCtable_in always has 256 uint32_t elements
//here we'll try to make a streamed CRC32 architecture. Table-based algorithm is told to be fast, but it would require 32b*256=8kb RAM
//shift register does not use this, but it is slower. Please, choose one way
//====================TABLE-BASED CRC32 algorithms====================
uint32_t Compute_CRC32(uint8_t* bytes, uint16_t bytearray_len, CRCModelTypeDef* crcModel, uint32_t* CRCtable_in);
/**
 * use this to calculate CRC32 in streamed order. Produces intermediate value of CRC, suitable for next call of this routine
 * @param CRCvalue_in current value of CRC. if starting algorithm then pass Init value
 * @param byte2update byte which is used to update CRC32
 * @param crcModel config
 * @param CRCtable_in pointer to array with 256 of uint32
 * @return updated value of CRC32
 */
uint32_t Compute_CRC32_streamed(uint32_t CRCvalue_in, uint8_t byte2update, CRCModelTypeDef* crcModel, uint32_t* CRCtable_in);
/**
 * Transform value of CRC32 according to model. Produces a final value of CRC
 * @param CRC_in this value of CRC32
 * @param crcModel config, model
 * @return new value rdy 2 use
 */
uint32_t Finalize_CRC32_streamed(uint32_t CRC_in, CRCModelTypeDef* crcModel);
void CalculateCrcTable(uint32_t* out_crcTable, CRCModelTypeDef* in_crcModel);
//====================
/**
 * Calculate crc32 without table. May be less speedy, but claims less ram. Works only with zero initialized value
 * @param bytes array to process
 * @param bytearray_len length of array
 * @param crcModel parameters: ptr to structure
 * @return crc32
 */
uint32_t Compute_CRC32_Simple(uint8_t* bytes, uint16_t bytearray_len, CRCModelTypeDef* crcModel);
/**
 * streamed variant of crc32 without table (on offset register)
 * @param CRCvalue_in - current value of CRC
 * @param byte2update - byte to be used to update value of CRC
 * @param crcModel - parameters of CRC (ptr to struct)
 * @return 
 */
uint32_t Compute_CRC32_Simple_Streamed(uint32_t CRCvalue_in, uint8_t byte2update, CRCModelTypeDef* crcModel);
/**
 * get Initial value of shift register using value from crcModel and at most 4 bytes of data. used in simple crc32 algorithm
 * @param bytes - pointer to array on buffer which is used to initialized shift register
 * @param bytesLength - length of array
 * @param crcModel - config
 * @return 
 */
uint32_t GetInitialShiftRegister(uint8_t* bytes, uint16_t bytesLength, CRCModelTypeDef * crcModel);
/**
 * compute crc32 over array, without using a precalculated table. Calls GetInitialShiftRegister inside
 * @param bytes
 * @param bytesLength
 * @param crcModel
 * @return finalized checksum value
 */
uint32_t ComputeCRC32_Simple_ShiftReg(uint8_t* bytes, uint16_t bytesLength, CRCModelTypeDef * crcModel);
/**
 * streamed variant of crc32. on first step pass here value from GetInitialShiftRegister, and skip first 4 bytes in array
 * @param CRCvalue_in
 * @param byte2update
 * @param crcModel
 * @return 
 */
uint32_t ComputeCRC32_Simple_ShiftReg_Streamed(uint32_t CRCvalue_in, uint8_t byte2update, CRCModelTypeDef* crcModel);
//====================
uint8_t Reflect8(uint8_t val);
uint16_t Reflect16(uint16_t val);
uint32_t Reflect32(uint32_t val);

//3line block of special declaration to use C code with C++ compiler
#ifdef __cplusplus
}
#endif

#endif /* CRC32PROCESS_H */

