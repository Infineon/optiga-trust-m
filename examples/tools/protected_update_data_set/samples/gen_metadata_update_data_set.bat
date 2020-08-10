set PATH=..\bin

:: Metadata update 

%PATH%\protected_update_data_set.exe payload_version=3 trust_anchor_oid=E0E3 target_oid=E0E1 sign_algo=ES_256 priv_key=..\samples\integrity\sample_ec_256_priv.pem payload_type=metadata metadata=..\samples\payload\metadata\metadata.txt content_reset=0 secret=..\samples\confidentiality\secret.txt label="test" enc_algo="AES-CCM-16-64-128" secret_oid=F1D1

::%PATH%\protected_update_data_set.exe payload_version=3 trust_anchor_oid=E0E3 target_oid=E0E1 sign_algo=ES_256 priv_key=..\samples\integrity\sample_ec_256_priv.pem payload_type=metadata metadata=..\samples\payload\metadata\metadata.txt content_reset=0