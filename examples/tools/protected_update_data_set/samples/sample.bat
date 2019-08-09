
set PATH=..\bin

:: ES_256 (ECC-256)
%PATH%\protected_update_data_set.exe payload_version=3 trust_anchor_oid=E0E3 target_oid=E0E1 offset=00 write_type=1 sign_algo=ES_256 priv_key=sample_ec_256_priv.pem payload=payload_data.txt

:: ES_256 (ECC-384)
::%PATH%\protected_update_data_set.exe payload_version=3 trust_anchor_oid=E0E3 target_oid=E0E1 offset=00 write_type=1 sign_algo=ES_256 priv_key=sample_ec_384_priv.pem payload=payload_data.txt

::RSA-SSA-PKCS1-V1_5-SHA-256(RSA-1024)
::%PATH%\protected_update_data_set.exe payload_version=3 trust_anchor_oid=E0E3 target_oid=E0E1 offset=00 write_type=1 sign_algo=RSA-SSA-PKCS1-V1_5-SHA-256 priv_key=sample_rsa_1024_priv.pem payload=payload_data.txt

::RSA-SSA-PKCS1-V1_5-SHA-256(RSA-2048)
::%PATH%\protected_update_data_set.exe payload_version=3 trust_anchor_oid=E0E3 target_oid=E0E1 offset=00 write_type=1 sign_algo=RSA-SSA-PKCS1-V1_5-SHA-256 priv_key=sample_rsa_2048_priv.pem payload=payload_data.txt