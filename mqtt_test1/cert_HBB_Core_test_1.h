#ifndef certs_h
#define certs_h

const char AWS_CERT_CA[] = "-----BEGIN CERTIFICATE-----\n" \
"MIIDWTCCAkGgAwIBAgIUdInwUkeaUiK4xUXl7huVUHoIziAwDQYJKoZIhvcNAQEL\n" \
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n" \
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE5MTAxNDAyMDI1\n" \
"M1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n" \
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANG/oPPhXURWJg4gRtR4\n" \
"fFScJw/+/7xlncIAGb1FBbEAjedq8SQKvRrTZxMghHyq8jPqO9JXoi/3phEsOmgr\n" \
"G911v7DGntNDWP24ElaYjOrZOXxCu2kwp30i0lc7zFI1v8a+nBrYzGTXgrN9Nikv\n" \
"nkL0sxWL6QB1K0exedq8biEKr87IfxhVK9bIfCQkNWy/Bl0/AIkLiB4ufJdzC/0X\n" \
"i//McW1+0XQ18DqJn8dzS6qQsvVLUET3tQbj5ZaoNkdd7MKBuSYEYP4CjK3/BsKO\n" \
"k0OlvO1fhsKC0rU7hmdN+B4etgpUuh2TrKEtHiLBIBuDSKrqjGTGfT2q7qcWF9hJ\n" \
"cq0CAwEAAaNgMF4wHwYDVR0jBBgwFoAUZED9x1atrDhqpdmrSzPLCut9DWAwHQYD\n" \
"VR0OBBYEFJg9Jw80thRlbpVDfHvSKWQ9dMzDMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n" \
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCWQF8PvhPj0SEEVpR5Kkkh8npO\n" \
"r+jX/5UWC/1gmMGQMiV/Y15aNbtsRUeGV+JopMtf3+j68GQpcsjvt+4KKibhay3Z\n" \
"vWQcGmNUIX3EgDScgJAGNnbxV9kpZBsUzZs1+5qiDlLIvM9tybMtveWNCOjAn5Ox\n" \
"lIzac/2Iu6/KmlIQNp4YDzuvxigOY2ZlpVgW5VPXUkSWjjI5qRoRIxmUS8eco/89\n" \
"NjHhWR+OxswzeAidvesNjcycq2CKACmbxehkWGeIYfRH+0dKueB4Salv8aN+woy5\n" \
"HdHv+ZagWQCcLZTFNBG9gAimkEk2wu1kwVf/qJ41GwDWRJIM/915PQhthzCk\n" \
"-----END CERTIFICATE-----\n";

// The private key for your device
const char AWS_CERT_PRIVATE[] = "-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEpAIBAAKCAQEA0b+g8+FdRFYmDiBG1Hh8VJwnD/7/vGWdwgAZvUUFsQCN52rx\n" \
"JAq9GtNnEyCEfKryM+o70leiL/emESw6aCsb3XW/sMae00NY/bgSVpiM6tk5fEK7\n" \
"aTCnfSLSVzvMUjW/xr6cGtjMZNeCs302KS+eQvSzFYvpAHUrR7F52rxuIQqvzsh/\n" \
"GFUr1sh8JCQ1bL8GXT8AiQuIHi58l3ML/ReL/8xxbX7RdDXwOomfx3NLqpCy9UtQ\n" \
"RPe1BuPllqg2R13swoG5JgRg/gKMrf8Gwo6TQ6W87V+GwoLStTuGZ034Hh62ClS6\n" \
"HZOsoS0eIsEgG4NIquqMZMZ9ParupxYX2ElyrQIDAQABAoIBAQDA1lqG/fJ+qmmQ\n" \
"YTJdk2xTacEsHrhGIHKa8va3IWcrJaRRV+vdxBW6PRQt7J6B87R78GURbMpLKXc/\n" \
"oJcOQFv9Do2vW9yXxPpjQcFyZgxgulf0m3GsMzQ5gzM5Slv+VTY85e+lax73s3+R\n" \
"v5N06DervUeMjrYoWpDw+AVdChKCQqLVAcWlk8MQ+WYOAYYoyKD+xgNVjh9gs+rU\n" \
"UJE5qX8ov78QIp5ILYMnQqIaMIeXB35zKd/RlwsZa5og1w7nhzjp8q1dkw0XvO4R\n" \
"hyJH/dJ6eYrYRhNZJ4ydaPP3xTLcGu4ILd/lIUDe9jsi09COxWiaNYQYfpp0b5fG\n" \
"Fi8TSSvhAoGBAPjkuyuPH/HucT4hnXVxbfWpBGs1Zy4mk4thOWajVF6MOqaxfKV+\n" \
"qq2/nj0XRj6c5aQV89+Wyfd0q2GT7TB1NhjAIbNlRuPBZc5VJQb/D5b0IpyO6rjh\n" \
"FAiG5i3iov4q8nwtEG3wzF+tvEceCqa2ahw20NwMyqZLROM8Ff9lTiX5AoGBANe8\n" \
"xUKuABfEB2u6nSGzoN3Barvj5dz7gRyf49K1lm0cBBEwDT4ADmeMFu8EReg5DC64\n" \
"9yjG3Dms24IXtLKK+/U6wxKAmFOozWjY9po8JtFYkPNVPszJphEm/TdxYLoNN+9c\n" \
"rKr11UH60PEm+H5woBY/Nh2uVi5ZBujw0s6wV09VAoGATmqCLe8h+W9ObmzY1y+t\n" \
"nUBrC/v/TiMJSViQZS4AXul/cRVqWBnYXHWKvWSf1QpJr7Da7Q7tAvHXLR8WE7Za\n" \
"HOIrvAGknA1Krf9VcybZGo5AeWLtwOHpFeaqjrpg8ANH8qegDYNYYKy0LRPh9zRx\n" \
"qgH7O9kr38jHI3DC3XxlasECgYAp1gSW4rFH6o7Z2E2CbJgkiM6O8ynvSaKHD4aZ\n" \
"pPcv33/2aVAYFuC0Odl0rmp0FyQCp4HemXm5gYjr2HcFLKihrKyJmQBxMMnsrC5e\n" \
"SI3qiPdLCNvt0KjwqJPwPX9ZdRjExT8CxDt/2QjNyxXjRyn0RnvTE6zIviD16UWo\n" \
"KVA1JQKBgQClNW/s8VD7OlBZ3M73ZE69F/I69oRTtKhXyizRyxuXoBqiLPBXbiKB\n" \
"U+Rkv2hHULO7lkS9WZgEyBK4Z/8ZSYg0TdPfbBCq0dS5D0dWXztIPoqM932gFUIa\n" \
"X78E+kyA+WobfSx6rlFFkJ2/Jf2SdTC7FQ5RNxuscwrzPLvacoVXVA==\n" \
"-----END RSA PRIVATE KEY-----\n";

// The certificate for your device
const char AWS_CERT_CRT[] = "-----BEGIN PUBLIC KEY-----\n" \
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA0b+g8+FdRFYmDiBG1Hh8\n" \
"VJwnD/7/vGWdwgAZvUUFsQCN52rxJAq9GtNnEyCEfKryM+o70leiL/emESw6aCsb\n" \
"3XW/sMae00NY/bgSVpiM6tk5fEK7aTCnfSLSVzvMUjW/xr6cGtjMZNeCs302KS+e\n" \
"QvSzFYvpAHUrR7F52rxuIQqvzsh/GFUr1sh8JCQ1bL8GXT8AiQuIHi58l3ML/ReL\n" \
"/8xxbX7RdDXwOomfx3NLqpCy9UtQRPe1BuPllqg2R13swoG5JgRg/gKMrf8Gwo6T\n" \
"Q6W87V+GwoLStTuGZ034Hh62ClS6HZOsoS0eIsEgG4NIquqMZMZ9ParupxYX2Ely\n" \
"rQIDAQAB\n" \
"-----END PUBLIC KEY-----\n";

#endif