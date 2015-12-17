#include "RtMidi.h"
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#define MAX_SIGNAL 100
#define MAX_INSTRUCTION 10

bool done;
static void finish(int ignore) {
    done = true;
    }

int endFile(FILE * file) {
    char carac = fgetc(file);
    fseek(file, -1, SEEK_CUR);
    return carac == EOF;
    }

int main() {
    char str[100];
    char buffer[10];
    int a;
    char keymap[MAX_SIGNAL][MAX_INSTRUCTION];

    for (int i = 0 ; i < MAX_SIGNAL ; i++)
        strcpy(keymap[i], "UNDEF");

    FILE* file = fopen("keymap.txt", "r");

    while (!endFile(file)) {
        fscanf(file, "%d %s", &a, buffer);
        strcpy(keymap[a], buffer);
        }

    fclose(file);
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
    done = false;
    (void) signal(SIGINT, finish);
    std::cout << "1. Start\n2. Map keys\nChoice : ";
    std::cin >> nBytes;
    midiin->openPort(port);
    midiin->ignoreTypes(true, true, true);
    std::cout << "Now reading from MIDI input..." << std::endl;

    if (nBytes == 1) {

        while (!done) {
            usleep(10);
            midiin->getMessage(&message);
            nBytes = message.size();

            if (nBytes > 0 && (int)message[0] != 248) {
                if (message[2] != 0) {
                    std::cout << "Key " << (int)message[1] << " down\n";
                    sprintf(str, "xdotool keydown ");
                    }
                else {
                    std::cout << "Key " << (int)message[1] << " up\n";
                    sprintf(str, "xdotool keyup ");
                    }

                if (strcmp("UNDEF", keymap[message[1]]) != 0) {
                    strcat(str, keymap[message[1]]);
                    system(str);
                    }
                else std::cout << "Key not mapped !" << std::endl;
                }
            }
        }
    else {
        file = fopen("keymap.txt", "a+");

        while (!done) {
            usleep(10);
            midiin->getMessage(&message);
            nBytes = message.size();

            if (nBytes > 0 && (int)message[0] != 248) {
                if (message[2] != 0) {
                    midiin->closePort();
                    std::cout << "String to map with key" << (int)message[1] << " : ";
                    std::cin >> buffer;
                    fprintf(file, "%d %s\n", (int)message[1], buffer);
                    std::cout << "Successfully mapped" << std::endl;
                    midiin->openPort(port);
                    }
                }
            }

        fclose(file);
        }

    delete midiin;
    return 0;
    }
