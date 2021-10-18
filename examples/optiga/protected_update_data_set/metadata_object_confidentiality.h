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
* \file metadata_object_confidentiality.h
*
* \brief   This file provides the data set for protected update of metadata object with confidentiality.
*
* \ingroup grOptigaExamples
*
* @{
*/

/**
 * Manifest for metadata update with integrity protected and confidentiality
 */
const uint8_t manifest_metadata[] = 
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
            0x9B, 
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
                0x21,
                // Payload Length 
                0x0D,
                // Trust Payload version
                0x03, 
                // Trust Add info data 
                0x82, 
                    // content reset
                    0x00,
                    // additional flag
                    0x00,
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
                                        0x8D, 0x4F, 0xD1, 0x71, 0x7A, 0xDC, 0xD5, 0x3F,
                                        0x47, 0x17, 0x3D, 0x89, 0x29, 0xC8, 0x87, 0xDD,
                                        0x64, 0x73, 0xD0, 0x0F, 0x3B, 0x74, 0x7B, 0xA9,
                                        0x0A, 0x55, 0xEA, 0x95, 0x35, 0x35, 0xBF, 0xD2,
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
                                                        0x43, 0xBB, 0xB9, 0xEC, 0x7F, 0x97, 0xE1, 0xA2,
                                                        0xE3, 0x69, 0x85, 0xC6, 0x2A, 0x3B, 0x30, 0x88,
                                                        0x06, 0xBC, 0x1E, 0x56, 0xB5, 0x92, 0x2D, 0x37,
                                                        0x40, 0x64, 0x02, 0xBD, 0xA6, 0x68, 0x49, 0x9F,
                                                        0xC6, 0x4F, 0x00, 0x02, 0xDD, 0x3D, 0x28, 0xE8,
                                                        0x1D, 0x42, 0x04, 0x98, 0xF9, 0x93, 0xAC, 0x79,
                                                        0x89, 0x41, 0x00, 0xE6, 0x65, 0x56, 0xEF, 0x34,
                                                        0x51, 0x9B, 0xED, 0x56, 0xBA, 0xBC, 0xB8, 0xC8,
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
                    0xE0, 0xE2, 
        // Signature info, byte string of single byte length
        0x58, 
            // Byte string length for ECC 256
            0x40, 
            // Signature data
            0xEE, 0xBD, 0x47, 0x34, 0x4E, 0x7A, 0x30, 0x9D, 0x85, 0x55, 0x3D, 0xEE, 0xDD, 0x73, 0xCF, 0xD3,
            0x92, 0x54, 0x2D, 0x07, 0x36, 0x8C, 0xFD, 0x9B, 0x5E, 0x4B, 0x69, 0xF0, 0xEC, 0x34, 0x15, 0xE0,
            0x55, 0x11, 0xC9, 0xE5, 0xD6, 0x50, 0xC7, 0xB9, 0x4C, 0xA0, 0xEE, 0x46, 0x1A, 0x1A, 0xE9, 0x2C,
            0xD8, 0x37, 0x84, 0x2B, 0x13, 0x94, 0x29, 0x30, 0xF2, 0xE0, 0x1E, 0xC7, 0xB9, 0xBA, 0x82, 0x42,
};

/**
 * Fragment array 
 */
const uint8_t metadata_final_fragment_array[] = 
{
        0x38, 0x3B, 0xFE, 0x8C, 0x8C, 0x95, 0x3D, 0x1B, 0x84, 0x91, 0x85, 0x57, 0xC1, 0xCA, 0x9D, 0xED,
        0xF6, 0xF8, 0x9B, 0xBC, 0x58,
};

/**
* @}
*/
