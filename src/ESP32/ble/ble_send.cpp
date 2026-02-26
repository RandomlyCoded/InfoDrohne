// ble_send_native.cpp
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

int main() {
    const char* mac = "AA:BB:CC:DD:EE:FF"; // ESP32 MAC
    uint8_t data[12] = {0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x34};

    // Convert MAC
    bdaddr_t bdaddr;
    str2ba(mac, &bdaddr);

    // L2CAP socket
    int sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    if(sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_l2 addr;
    memset(&addr, 0, sizeof(addr));
    addr.l2_family = AF_BLUETOOTH;
    addr.l2_psm = htobs(0x0011); // ATT PSM for BLE
    addr.l2_bdaddr = bdaddr;

    if(connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    if(write(sock, data, sizeof(data)) < 0) {
        perror("write");
        close(sock);
        return 1;
    }

    std::cout << "12-byte BLE packet sent successfully!\n";
    close(sock);
    return 0;
}
