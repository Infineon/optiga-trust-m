
set PATH=..\bin

:: EC_256
%PATH%\protected_update_data_set.exe payload_version=3 trust_anchor_oid=E0E8 target_oid=E0E2 offset=00 write_type=1 sign_algo=ES_256 priv_key=sample_ec_256_prv.pem payload=payload_data.txt

::RSA-SSA-PKCS1-V1_5-SHA-256
::%PATH%\protected_update_data_set.exe payload_version=3 trust_anchor_oid=E0E8 target_oid=E0E2 offset=00 write_type=1 sign_algo=RSA-SSA-PKCS1-V1_5-SHA-256 priv_key=sample_rsa_1024_priv.pem payload=payload_data.txt
