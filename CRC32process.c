#include "CRC32process.h"
uint16_t Reflect16(uint16_t val) {
    uint16_t resVal = 0;

    for (int i = 0; i < 16; i++)
    {
        if ((val & (1 << i)) != 0)
        {
            resVal |= (uint16_t)(1 << (15 - i));
        }
    }

    return resVal;
}

uint32_t Reflect32(uint32_t val) {
    uint32_t resVal = 0;
    for (int i = 0; i < 32; i++)
    {
        if ((val & (1 << i)) != 0)
        {
            resVal |= (uint32_t)(1 << (31 - i));
        }
    }
    return resVal;
}

uint8_t Reflect8(uint8_t val) {
uint8_t resVal = 0;
    for (int i = 0; i < 8; i++)
    {
        if ((val & (1 << i)) != 0)
        {
            resVal |= (uint8_t)(1 << (7 - i));
        }
    }
    return resVal;
}
/*
 * Sets values for crcTable array. For better memory handling allocate space for crcTable before calling subroutine and then deallocate it after calculation of CRC
 */
void CalculateCrcTable(uint32_t* out_crcTable, CRCModelTypeDef* in_crcModel) {
            //crcTable = new uint32_t[256];

            for (uint32_t divident = 0; divident < 256; divident++)
            {
                uint32_t curByte = (uint32_t)(divident << 24);
                for (uint8_t bit = 0; bit < 8; bit++)
                {
                    if ((curByte & TOPBIT_MASK) != 0)
                    {
                        curByte <<= 1;
                        curByte ^= in_crcModel->Polynomial;
                    }
                    else
                    {
                        curByte <<= 1;
                    }
                }

                out_crcTable[divident] = curByte;
            }
}

uint32_t Compute_CRC32(uint8_t* bytes, uint16_t bytearray_len, CRCModelTypeDef* crcModel, uint32_t* CRCtable_in)  {
    uint32_t crc = crcModel->Initial; /* CRC is set to specified initial value */
    for (uint16_t i = 0; i<bytearray_len; i++) {
        /* reflect input byte if specified, otherwise input byte is taken as it is */
        uint8_t curByte = (crcModel->InputReflected ? Reflect8(bytes[i]) : bytes[i]);

        /* XOR-in next input byte into MSB of crc and get this MSB, that's our new intermediate divident */
        uint8_t pos = (uint8_t)((crc ^ (curByte << 24)) >> 24);
        /* Shift out the MSB used for division per lookuptable and XOR with the remainder */
        crc = (uint32_t)((crc << 8) ^ (uint32_t)(CRCtable_in[pos]));
    }
    /* reflect result crc if specified, otherwise calculated crc value is taken as it is */
    crc = (crcModel->ResultReflected ? Reflect32(crc) : crc);
    /* Xor the crc value with specified final XOR value before returning */
    return (uint32_t)(crc ^ crcModel->FinalXor);
}

uint32_t Compute_CRC32_streamed(uint32_t CRCvalue_in, uint8_t byte2update, CRCModelTypeDef* crcModel, uint32_t* CRCtable_in) {
        uint32_t crc = CRCvalue_in;
        /* reflect input byte if specified, otherwise input byte is taken as it is */
        uint8_t curByte = (crcModel->InputReflected ? Reflect8(byte2update) : byte2update);
        /* XOR-in next input byte into MSB of crc and get this MSB, that's our new intermediate divident */
        uint8_t pos = (uint8_t)((crc ^ (curByte << 24)) >> 24);
        /* Shift out the MSB used for division per lookuptable and XOR with the remainder */
        crc = (uint32_t)((crc << 8) ^ (uint32_t)(CRCtable_in[pos]));
        return crc;
}

uint32_t Finalize_CRC32_streamed(uint32_t CRC_in, CRCModelTypeDef* crcModel) {
    uint32_t crc = (crcModel->ResultReflected ? Reflect32(CRC_in) : CRC_in);
    /* Xor the crc value with specified final XOR value before returning */
    return (uint32_t)(crc ^ crcModel->FinalXor);
}

uint32_t Compute_CRC32_Simple(uint8_t* bytes, uint16_t bytearray_len, CRCModelTypeDef* crcModel) {
        uint32_t crc = crcModel->Initial;
        for (uint16_t j = 0; j<bytearray_len; j++) {
            uint8_t curByte = (crcModel->InputReflected ? Reflect8(bytes[j]) : bytes[j]);
            crc ^= (uint8_t)(curByte << 24); /* move byte into MSB of 32bit CRC */
                for (int i = 0; i < 8; i++) {
                    if ((crc & TOPBIT_MASK) != 0) {
                        crc = (uint32_t)((crc << 1) ^ crcModel->Polynomial);
                    }
                    else {
                        crc <<= 1;
                    }
                }
            }
            crc = (crcModel->ResultReflected ? Reflect32(crc) : crc);
            return (uint32_t)(crc ^ crcModel->FinalXor);
}

uint32_t Compute_CRC32_Simple_Streamed(uint32_t CRCvalue_in, uint8_t byte2update, CRCModelTypeDef* crcModel) {
    uint8_t curByte = (crcModel->InputReflected ? Reflect8(byte2update) : byte2update);
    uint32_t crc = CRCvalue_in;
    crc ^= (uint8_t)(curByte << 24);
    for (int i = 0; i < 8; i++) {
        if ((crc & TOPBIT_MASK) != 0) {
            crc = (uint32_t)((crc << 1) ^ crcModel->Polynomial);
        } else {
            crc <<= 1;
            }
    }
    return crc;
}
/**
 * get Initial value of shift register using value from crcModel and at most 4 bytes of data
 * @param bytes - pointer to array on buffer which is used to initialized shift register
 * @param bytesLength - length of array
 * @param crcModel - config
 * @return 
 */
uint32_t GetInitialShiftRegister(uint8_t* bytes, uint16_t bytesLength, CRCModelTypeDef * crcModel) {
            uint8_t b1 = 0, b2 = 0, b3 = 0, b4 = 0;
            // fill the register with the initial value ^ the first two bytes of the input stream
            if (bytesLength >= 1) {
                b1 = crcModel->InputReflected ? Reflect8(bytes[0]) : bytes[0];
            }

            if (bytesLength >= 2)            {
                b2 = crcModel->InputReflected ? Reflect8(bytes[1]) : bytes[1];
            }

            if (bytesLength >= 3)            {
                b3 = crcModel->InputReflected ? Reflect8(bytes[2]) : bytes[2];
            }

            if (bytesLength >= 4)            {
                b4 = crcModel->InputReflected ? Reflect8(bytes[3]) : bytes[3];
            }
            //XOR initial value with 32 byte number composed with b1 ... b4
            return (uint32_t)(crcModel->Initial ^ (uint32_t)((uint32_t)b1 << 24 | (uint32_t)b2 << 16 | (uint32_t)b3 << 8 | b4));
}
/**
 * compute crc32 over array, without using a precalculated table
 * @param bytes
 * @param bytesLength
 * @param crcModel
 * @return finalized checksum value
 */
uint32_t ComputeCRC32_Simple_ShiftReg(uint8_t* bytes, uint16_t bytesLength, CRCModelTypeDef * crcModel) {
            uint32_t crc = GetInitialShiftRegister(bytes, (bytesLength>=4)? 4 : bytesLength%4, crcModel);
            /* skip first four bytes, already inside crc register */
            for (uint16_t byteIndex = 4; byteIndex < bytesLength + 4; byteIndex++)
            {
                uint8_t curByte = (byteIndex < bytesLength) ? (crcModel->InputReflected ? Reflect8(bytes[byteIndex]) : bytes[byteIndex]) : (uint8_t)0;
                for (int i = 7; i >= 0; i--)   {
                    if ((crc & TOPBIT_MASK) != 0)
                    {
                        crc = (uint32_t)(crc << 1);
                        crc = ((uint32_t)(curByte & (1 << i)) != 0) ? (uint32_t)(crc | 0x00000001) : (uint32_t)(crc & 0xFFFFFFFE);
                        crc = (uint32_t)(crc ^ crcModel->Polynomial);
                    }
                    else
                    {
                        crc = (uint32_t)(crc << 1);
                        crc = ((uint32_t)(curByte & (1 << i)) != 0) ? (uint32_t)(crc | 0x00000001) : (uint32_t)(crc & 0xFFFFFFFE);
                    }
                }
            }
            crc = (crcModel->ResultReflected ? Reflect32(crc) : crc);
            return (uint32_t)(crc ^ crcModel->FinalXor);
}

uint32_t ComputeCRC32_Simple_ShiftReg_Streamed(uint32_t CRCvalue_in, uint8_t byte2update, CRCModelTypeDef* crcModel) {
    uint32_t crc = CRCvalue_in;
    uint8_t curByte = (crcModel->InputReflected==1) ? Reflect8(byte2update) : byte2update;
    for (int i = 7; i >= 0; i--)   {
                    if ((crc & TOPBIT_MASK) != 0)  {
                        crc = (uint32_t)(crc << 1);
                        crc = ((uint32_t)(curByte & (1 << i)) != 0) ? (uint32_t)(crc | 0x00000001) : (uint32_t)(crc & 0xFFFFFFFE);
                        crc = (uint32_t)(crc ^ crcModel->Polynomial);
                    }
                    else  {
                        crc = (uint32_t)(crc << 1);
                        crc = ((uint32_t)(curByte & (1 << i)) != 0) ? (uint32_t)(crc | 0x00000001) : (uint32_t)(crc & 0xFFFFFFFE);
                    }
    }
    return crc;
}
