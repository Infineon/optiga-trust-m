/**
* \copyright
* MIT License
*
* Copyright (c) 2021 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
* \endcopyright
*
* \author Infineon Technologies AG
*
* \file ecc_key_object_confidentiality.h
*
* \brief   This file provides the data set for protected update of ecc key object with confidentiality.
*
* \ingroup grOptigaExamples
*
* @{
*/

#include <stdint.h>

/**
 * Manifest for ECC key update with integrity and confidentiality
 */
const uint8_t manifest_ecc_key[] =
{
    // COSE Sign1 Trust
    0x84,
        // Protected signed header trust
        0x43,
            // Trust Sign Algorithm
            0xA1, 
            // ECDSA
            0x01, 0x26, 
        // Unprotected signed header trust
        0xA1, 
            // Root of trust
            0x04, 0x42, 
            // Trust Anchor OID
            0xE0, 0xE3, 
        // Payload info Byte string of single byte length
        0x58, 
            // Byte string length of manifest            
            0x9C, 
            // Trust manifest, array of 6 entries
            0x86, 
            // Version(Major Type 0)
            0x01,
            // NULL
            0xF6,
            // NULL
            0xF6,
            // Resources, array of 4 entries
            0x84, 
                // Trust Payload Type
                0x22,
                // Payload Length 
                0x18, 0x66,
                // Trust Payload version
                0x03, 
                // Trust Add info data 
                0x82, 
                    // key algorithm
                    0x03,
                    // key usage
                    0x10, 
                // Trust Processors, array of 2 entries 
                0x82, 
                    // Processing step integrity, array of 2 entries
                    0x82, 
                        // Process( Major Type 1)
                        0x20, 
                        // Parameters, byte string with single byte length
                        0x58, 
                            // Byte string length 
                            0x25, 
                            // IFX Digest info, array of 2 entries
                            0x82, 
                                // Digest Algorithm
                                0x18,
                                    // SHA-256
                                    0x29, 
                                // Digest
                                0x58, 
                                    // Byte string length
                                    0x20, 
                                        // Digest data
                                        0x6B, 0x58, 0x06, 0x91, 0xE2, 0x8D, 0xDE, 0xC5,
                                        0x86, 0x89, 0x89, 0x28, 0xF8, 0xA1, 0xA3, 0xFE,
                                        0xA9, 0xBE, 0x51, 0xC9, 0x4D, 0x6C, 0x34, 0xD6,
                                        0x72, 0x05, 0x9E, 0x86, 0x2B, 0xA9, 0x40, 0x39,
                    // Processing step decrypt, array of 2 entries
                    0x82,
                        // Process( Major Type 0)
                        0x01,
                        // COSE_Encrypt_Trust array of 3 entries
                        0x83,
                            // protected-encrypt-header-Trust
                            0x43,
                                // Map 
                                0xA1,
                                    // Key 
                                    0x01,
                                    // AES-CCM-16-64-128, 128-bit key, 64-bit Authentication(MAC) tag, 13-byte nonce
                                    0x0A,
                            // recipients array of 1
                            0x81,
                                // COSE_Recipient_Trust array of 2
                                0x82,
                                    // protected-recipient-header-Trust
                                    0x58,
                                        // byte str
                                        0x54,
                                            // Map of 3
                                            0xA3,
                                                // Key
                                                0x04,
                                                // Value
                                                0x42,
                                                    // Shared secret OID
                                                    0xF1, 0xD1,
                                                // Key
                                                0x01,
                                                // Value (KeyDerivationAlgorithms)
                                                0x3A, 0x00, 0x01, 0x00, 0xB7,
                                                // Key
                                                0x05,
                                                // Value array of 2
                                                0x82,
                                                    0x44,
                                                        // label(4)
                                                        0x74, 0x65, 0x73, 0x74,
                                                    0x58,
                                                        0x40,
                                                        // seed(64)
                                                        0x24, 0xC3, 0xDE, 0x7B, 0x3E, 0xFB, 0x6F, 0x67,
                                                        0x9E, 0x40, 0xA4, 0xD7, 0x79, 0xAB, 0x9D, 0x19,
                                                        0x5F, 0x9B, 0xE1, 0xDC, 0x05, 0xD1, 0x5F, 0x93,
                                                        0xEB, 0xD7, 0x1A, 0xA3, 0x6D, 0xDB, 0xA9, 0x80,
                                                        0x05, 0xFF, 0x23, 0x26, 0xA7, 0x94, 0x06, 0xE3,
                                                        0xC7, 0x0D, 0x0A, 0xFF, 0x88, 0xA6, 0x9B, 0xC4,
                                                        0x87, 0x03, 0x53, 0x7C, 0x24, 0xAB, 0x3A, 0x55,
                                                        0x87, 0x90, 0x0E, 0x33, 0xE3, 0x0B, 0x54, 0x7D,
                                    // ciphertext-recipient-header-Trust
                                    0xF6,
                            // AdditionalInfo
                            0xF6,
                // Trust Target
                0x82, 
                    // Component identifier(0)
                    0x40,
                    // Storage identifier
                    0x42, 
                    // Optiga target OID
                    0xE0, 0xF1, 
        // Signature info, byte string of single byte length
        0x58, 
            // Byte string length for ECC 256
            0x40, 
            // Signature data
            0xC1, 0x12, 0x3B, 0xCD, 0x23, 0x25, 0xB0, 0x04, 0xF2, 0x23, 0xA8, 0xF7, 0xE5, 0x26, 0x40, 0xD4,
            0x1A, 0x60, 0x4D, 0x64, 0x82, 0xA5, 0x84, 0xFF, 0xF2, 0x36, 0x2E, 0x6A, 0x6B, 0x17, 0x0F, 0xD8,
            0xCF, 0xD4, 0xDB, 0x8B, 0xA6, 0x01, 0x8D, 0x45, 0xA3, 0xFD, 0x23, 0x43, 0xAE, 0xBF, 0x2A, 0x9D,
            0xC2, 0x9F, 0xD5, 0xB8, 0xDA, 0x47, 0x7E, 0x85, 0xC7, 0x53, 0x6D, 0xFD, 0xE8, 0xE3, 0x32, 0xAF, 
};


/**
 * Fragment array for final payload for ECC key update
 */
const uint8_t ecc_key_final_fragment_array[] =
{
    0x6E, 0x8A, 0x12, 0xAF, 0x01, 0x4F, 0xDC, 0xD4, 0x1E, 0x0D, 0xFB, 0xD3, 0xD2, 0xE2, 0xF8, 0xF3,
    0x7B, 0xC6, 0xF9, 0xE1, 0xFC, 0xA2, 0xF5, 0xC1, 0x62, 0x15, 0x67, 0xE7, 0xFA, 0xA1, 0x24, 0xEE,
    0x85, 0xEC, 0x6C, 0x31, 0xE8, 0xD1, 0x4A, 0x67, 0xB7, 0x76, 0x40, 0xFD, 0xBB, 0x51, 0x37, 0xF6,
    0x58, 0xBF, 0xBB, 0xE1, 0x79, 0xA7, 0xCD, 0xD8, 0x29, 0xCC, 0xC8, 0xA1, 0x10, 0x4B, 0x9A, 0x98,
    0x33, 0xEE, 0xB0, 0x68, 0xB7, 0xD5, 0x1F, 0xAD, 0xAF, 0x1A, 0x8A, 0xF6, 0x14, 0x40, 0xB0, 0x85,
    0x2F, 0xE6, 0xFF, 0x31, 0xB1, 0x12, 0x51, 0xB2, 0x9C, 0x10, 0xFC, 0xBA, 0x25, 0xF3, 0x43, 0x28,
    0xA1, 0x99, 0x56, 0xD1, 0xD2, 0xF8, 0xDE, 0xE7, 0xE2, 0xBC, 0x08, 0xF2, 0x67, 0xE7,
};

/**
* @}
*/
