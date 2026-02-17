#include <iostream>
#include <unistd.h>
#include <gattlib.h>

#define ESP_MAC "AA:BB:CC:DD:EE:FF"  // change this
#define CHAR_UUID "abcdefab-1234-5678-1234-abcdefabcdef"

int main() {
    gatt_connection_t* connection =
        gattlib_connect(NULL, ESP_MAC, GATTLIB_CONNECTION_OPTIONS_LEGACY_DEFAULT);

    if (!connection) {
        std::cerr << "Failed to connect\n";
        return 1;
    }

    uint8_t on = 1;
    uint8_t off = 0;

    while (true) {
        std::cout << "LED ON\n";
        gattlib_write_char_by_uuid(connection, CHAR_UUID, &on, 1);
        sleep(1);

        std::cout << "LED OFF\n";
        gattlib_write_char_by_uuid(connection, CHAR_UUID, &off, 1);
        sleep(1);
    }

    gattlib_disconnect(connection);
    return 0;
}
