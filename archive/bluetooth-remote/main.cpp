#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

void blast_packets(int socket_fd, int pps, int seconds) {
    // Prepare a dummy 12-signal packet (-128 to 127 values)
    std::vector<uint8_t> packet = {12}; // Header: 12 signals
    for(int i=0; i<12; ++i) packet.push_back(static_cast<uint8_t>(i * 10 - 60));

    int total_packets = pps * seconds;
    auto interval = std::chrono::microseconds(1000000 / pps);
    
    std::cout << "Blasting " << total_packets << " packets at " << pps << " PPS..." << std::endl;

    auto start_time = std::chrono::steady_clock::now();
    for (int i = 0; i < total_packets; ++i) {
        auto next_tick = std::chrono::steady_clock::now() + interval;

        // Send the 13 bytes
        if (write(socket_fd, packet.data(), packet.size()) < 0) {
            perror("Write failed");
            break;
        }

        // Busy-wait or sleep until next interval for maximum precision
        std::this_thread::sleep_until(next_tick);
    }

    auto end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << "Done! Sent in " << elapsed.count() << " seconds." << std::endl;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <mac_address>\n";
        return 1;
    }

    struct sockaddr_rc addr = { 0 };
    int s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba(argv[1], &addr.rc_bdaddr);

    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Connect failed");
        return 1;
    }

    while (true) {
        int pps, duration;
        std::cout << "\nEnter Packets Per Second (e.g., 50, 100, 200): ";
        std::cin >> pps;
        std::cout << "Enter duration in seconds: ";
        std::cin >> duration;

        blast_packets(s, pps, duration);

        std::cout << "Run again? (y/n): ";
        char c; std::cin >> c;
        if (c == 'n') break;
    }

    close(s);
    return 0;
}
