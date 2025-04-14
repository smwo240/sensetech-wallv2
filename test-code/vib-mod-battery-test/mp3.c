#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "mp3.h"

// AUDIO CONSTANTS
#define RESPONSE_SIZE 10
#define PLAYER_BUSY_PIN 15
#define UART0_TX_PIN 17
#define UART0_RX_PIN 16

/* MP3 PLAYBACK FUNCTIONS
##################################### */

// initialize the MP3-TF-16P chip
void mp3_initialize() {
    // initialize UART0 with baud rate 9600
    uart_init(uart0, 9600);

    // set the UART TX and RX pins
    gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART);

    // set the busy GPIO pin
    gpio_init(PLAYER_BUSY_PIN);
    gpio_set_dir(PLAYER_BUSY_PIN, GPIO_IN);
}

// send a command (1 byte) with two parameters (2 bytes)
uint8_t* mp3_send_command(uint8_t command, uint16_t param) {
    // flush uart buffer
    while (uart_is_readable(uart0)) {
        uart_getc(uart0);
    }

    uint8_t packet[10];
    packet[0] = 0x7E;                // start byte
    packet[1] = 0xFF;                // version
    packet[2] = 0x06;                // length
    packet[3] = command;             // command byte
    packet[4] = 0x01;                // feedback (0x01 = feedback required)
    packet[5] = (param >> 8) & 0xFF; // high byte of parameter
    packet[6] = param & 0xFF;        // low byte of parameter

    uint16_t checksum = 0 - (packet[1] + packet[2] + packet[3] + packet[4] + packet[5] + packet[6]);
    packet[7] = (checksum >> 8) & 0xFF; // high byte of checksum
    packet[8] = checksum & 0xFF;        // low byte of checksum

    packet[9] = 0xEF;                // end byte

    printf("Raw Command: ");
    for (int i = 0; i < 10; i++) {
        printf("0x%02X ", packet[i]);
    }
    printf("\n");

    uart_write_blocking(uart0, packet, sizeof(packet));

    uint8_t* response = (uint8_t*)malloc(RESPONSE_SIZE);
    if (response == NULL) {
        printf("Error: Failed to allocate memory for response\n");
        return NULL;
    }

    size_t received = 0;
    while (received < RESPONSE_SIZE) {
        if (uart_is_readable(uart0)) {
            response[received++] = uart_getc(uart0);
        }
    }

    printf("Raw Response: ");
    for (int i = 0; i < 10; i++) {
        printf("0x%02X ", response[i]);
    }
    printf("\n");

    return response;
}

// query the status and printf the response for debugging
void mp3_query_status() {
    // use the command 0x42 to query the current status
    // doesn't require a parameter (0x0000)
    uint8_t *status_response = mp3_send_command(0x42, 0x0000);

    // check if a response was received
    if (status_response == NULL) {
        printf("Error: Failed to get response from MP3 module.\n\n");
        free(status_response);
        return;
    }

    if (status_response[0] == 0x7E && status_response[9] == 0xEF) {
        uint8_t status = status_response[5];
        uint16_t current_track = (status_response[5] << 8) | status_response[6];

        // interpret the status byte
        printf("Interpreted Response: ");
        if (status == 0x01) {
            printf("Status: Playing, ");
            printf("Current Track %d\n", current_track);
        } else if (status == 0x02) {
            printf("Status Paused, ");
            printf("Current Track %d\n", current_track);
        } else if (status == 0x03) {
            printf("Status Stopped\n");
        } else {
            printf("Unknown Status (0x%02X)\n", status);
        }
    } else {
        printf("Error: Invalid response format received: \n");
    }
    printf("\n");

    free(status_response);

    // query the number of files on the TF card using command 0x47
    uint8_t *track_response = mp3_send_command(0x47, 0x0000);

    // Check if a response was received
    if (track_response == NULL) {
        printf("Error: Failed to get the total number of files from MP3 module.\n\n");
        return;
    }

    if (track_response[0] == 0x7E && track_response[9] == 0xEF) {
        uint16_t total_files = (track_response[5] << 8) | track_response[6];
        printf("Total Files on TF Card: %d\n", total_files);
    } else {
        printf("Error: Invalid response format received for file count query.\n");
    }
    printf("\n");

    free(track_response);
}

// play a sound from a specific folder and file
void mp3_play_sound(SOUNDS sound) {
    if (sound < 0 || sound >= sizeof(MP3_LIBRARY) / sizeof(MP3_LIBRARY[0])) {
        printf("Error: Invalid sound id.\n");
        return;
    }

    uint16_t folder = MP3_LIBRARY[sound].folder;
    uint16_t file = MP3_LIBRARY[sound].file;

    uint16_t param = (folder << 8) | file;

    uint8_t* select_response = mp3_send_command(0x0F, param);
    if (select_response == NULL) {
        printf("Error: Failed to select file.\n");
        return;
    }
    free(select_response);

    uint8_t* play_response = mp3_send_command(0x0B, 0x0000);
    if (play_response == NULL) {
        printf("Error: Failed to play selected file.\n");
        return;
    }
    free(play_response);

    printf("Playing file %u from folder %u.\n", file, folder);
}

// set the player volume (0-31)
void mp3_set_volume(uint8_t volume) {
    if (volume > 30) {
        printf("Error: Volume must be between 0 and 30.\n");
        return;
    }

    uint8_t* response = mp3_send_command(0x06, volume);
    if (response == NULL) {
        printf("Error: Failed to set volume.\n");
        return;
    }
    printf("Volume set to %d.\n\n", volume);

    free(response);
}

// check if player is busy
bool mp3_busy() {
    return gpio_get(PLAYER_BUSY_PIN) == 0;  // true if pin is low (busy), false otherwise
}