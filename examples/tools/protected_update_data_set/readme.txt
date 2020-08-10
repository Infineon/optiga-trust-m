1. Usage
	protected_update_data_set.exe input1=<value> input2=<value> ...
	
	For more information run "protected_update_data_set.exe /?" on command prompt
		
2. Sample :
	A sample script demonstrating the usage of the tool is available in ..\samples\sample.bat
	
3. Limitations
	a. Only SHA-256 digest algorithm is supported for hash calculation
	b. Manifest version number is 1	
    	
4. Environment
	a. The available executable is built using microsoft visual studio 2010 in windows 10 (64 bit)
	b. Install Microsoft Visual C++ 2010 x86 redistributable (vcredist_x86.exe) package to execute application on machine which does not have Visual Studio.
	c. mbedTLS 2.16.0 is used for crypto operation.
	d. The following MACRO must be disabled/ enabled when using mbedTLS for this tool.
		i. Enable :
			MBEDTLS_FS_IO
			MBEDTLS_PEM_PARSE_C
			MBEDTLS_BASE64_C
			MBEDTLS_ECDSA_DETERMINISTIC
			MBEDTLS_HMAC_DRBG_C
			MBEDTLS_ECDSA_DETERMINISTIC_DEBUG
			MBEDTLS_RSA_C
			MBEDTLS_PKCS1_V15
			MBEDTLS_PK_RSA_ALT_SUPPORT
			MBEDTLS_TIMING_C
			MBEDTLS_ENTROPY_C
			MBEDTLS_CTR_DRBG_C
			MBEDTLS_ECP_DP_SECP256R1_ENABLED
			MBEDTLS_ECP_DP_SECP384R1_ENABLED