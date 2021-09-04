#include <pgmspace.h>

#define SECRET
#define THINGNAME "esp32_IOT"

const char WIFI_SSID[] = "Prateek";
const char WIFI_PASSWORD[] = "nencheppa";
const char AWS_IOT_ENDPOINT[] = "a10hovg0k0bhol-ats.iot.ap-south-1.amazonaws.com";

// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";

// Device Certificate
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)KEY";

// Device Private Key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----

-----END RSA PRIVATE KEY-----
)KEY";
