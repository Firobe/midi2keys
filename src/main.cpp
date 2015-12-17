#include "RtMidi.h"
#include <unistd.h>
#include <signal.h>
#include <cstdlib>
#include <iostream>
#include <stdio.h>

bool done;
static void finish(int ignore) {
    done = true;
    }

int main() {
    RtMidiIn  *midiin = 0;
    int port, nBytes;
    std::vector<unsigned char> message;
    midiin = new RtMidiIn();
    unsigned int nPorts = midiin->getPortCount();
    std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";
    std::string portName;

    for (unsigned int i = 0; i < nPorts; i++) {
        portName = midiin->getPortName(i);
        std::cout << "  Input Port #" << i << ": " << portName << '\n';
        }

    std::cout << "Port ? ";
    std::cin >> port;
    midiin->openPort(port);
    midiin->ignoreTypes(true, true, true);
    done = false;
    char str[100];
    (void) signal(SIGINT, finish);
    std::cout << "Now reading from MIDI input..." << std::endl;

    while (!done) {
        usleep(10);
        midiin->getMessage(&message);
        nBytes = message.size();

        if (nBytes > 0 && (int)message[0] != 248) {
            if (message[2] != 0) {
                std::cout << "Key " << (int)message[1] << " down\n";
                sprintf(str, "xdotool keydown X");
                }
            else {
                std::cout << "Key" << (int)message[1] << " up\n";
                sprintf(str, "xdotool keyup   X");
                }

            str[16] = message[1] + 32;
            system(str);
            }
        }

    delete midiin;
    return 0;
    }
