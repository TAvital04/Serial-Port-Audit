// Imports
#include <stdio.h>
#include <string.h>

#include <windows.h>

// Functions
void most_recent_windows_error();

int connect_to_port(HANDLE *serial_port);
int configure_port_dcb(const HANDLE serial_port);
int configure_port_timeouts(const HANDLE serial_port);
int clear_buffers(const HANDLE serial_port);
int write_to_port(const HANDLE serial_port, const char *message);
int read_from_port(const HANDLE serial_port);

// Main
int main() {
    // Declare variables
    int status = 0;
    HANDLE serial_port = NULL;

    // Connect to the port
    status = connect_to_port(&serial_port);
    if(status == 0) {
        printf("Successfully connected to the serial port.\n");
    } else {
        goto cleanup;
    }

    // Configure the port dcb
    status = configure_port_dcb(serial_port);
    if(status == 0) {
        printf("Successfully configured the serial port parameters.\n");
    } else {
        goto cleanup;
    }

    // Configure the port timeouts
    status = configure_port_timeouts(serial_port);
    if(status == 0) {
        printf("Successfully configured the serial port timeouts.\n");
    } else {
        goto cleanup;
    }

    // Clear out the port buffers
    status = clear_buffers(serial_port);
    if(status == 0) {
        printf("Successfully cleared out both serial port buffers.\n");
    } else {
        goto cleanup;
    }

    // Write to the serial port
    status = write_to_port(serial_port, "Hello world!");
    if(status == 0) {
        printf("Successfully wrote a message to the serial port.\n");
    } else {
        goto cleanup;
    }

    // Read from the serial port
    status = read_from_port(serial_port);
    if(status == 0) {
        printf("Successfully read from the serial port.\n");
    } else {
        goto cleanup;
    }

    // Clear out the port buffers
    status = clear_buffers(serial_port);
    if(status == 0) {
        printf("Successfully cleared out both serial port buffers.");
    } else {
        goto cleanup;
    }

cleanup:
    if(serial_port && serial_port != INVALID_HANDLE_VALUE) {
        CloseHandle(serial_port);
    }
    
    return status;
}

void most_recent_windows_error() {
    // Declare variables
    DWORD win32_error_code;
    CHAR error_message[256];

    // Get the error code
    win32_error_code = GetLastError();

    // Turn the error code into an error message
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        win32_error_code,
        0,
        error_message,
        sizeof(error_message),
        NULL
    );

    fprintf(stderr, "WINDOWS ERROR: %s", error_message);
}

int connect_to_port(HANDLE *serial_port) {
    // Establish a connection with the serial port
    *serial_port = CreateFileA(
        "\\\\.\\COM3",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    ); // This is more or less a "default" configuration for this setup

    // Validate the connection
    if(*serial_port == INVALID_HANDLE_VALUE) {
        most_recent_windows_error();
        return 1;
    }

    return 0;
}

int configure_port_dcb(const HANDLE serial_port) {
    // Create a zero-ed out dcb structure
    DCB dcb = {0};
    dcb.DCBlength = sizeof(dcb);

    // Fill the structure with the port state
    BOOL status = GetCommState(serial_port, &dcb);

    if(status == FALSE) {
        most_recent_windows_error();
        return 1;
    }

    // Fill the structure with new settings
    dcb.BaudRate = CBR_115200;
    dcb.ByteSize = 8;
    dcb.Parity   = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fBinary  = TRUE;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl  = DTR_CONTROL_ENABLE;
    dcb.fRtsControl  = RTS_CONTROL_ENABLE;

    // Reset the structure
    status = SetCommState(serial_port, &dcb);

    if(status == FALSE) {
        most_recent_windows_error();
        return 1;
    }

    return 0;
}

int configure_port_timeouts(const HANDLE serial_port) {
    // Create a zero-ed out timeouts structure
    COMMTIMEOUTS timeouts = {0};

    // Fill the structure with new settings
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 200;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 200;
    timeouts.WriteTotalTimeoutMultiplier = 0;

    // Reset the structure
    BOOL status = SetCommTimeouts(serial_port, &timeouts);

    // Validate the result
    if(status == FALSE) {
        most_recent_windows_error();
        return 1;
    }

    return 0;
}

int clear_buffers(const HANDLE serial_port) {
    // Clear the receive buffer and validate the result
    BOOL status = PurgeComm(serial_port, PURGE_RXCLEAR);

    if(status == FALSE) {
        most_recent_windows_error();
        return 1;
    }

    // Clear the transmission buffer and validate the result
    status = PurgeComm(serial_port, PURGE_TXCLEAR);

    if(status == FALSE) {
        most_recent_windows_error();
        return 1;
    }

    // PurgeComm(serial_port, PURGE_RXCLEAR | PURGE_TXCLEAR); for both at once

    return 0;
}

int write_to_port(const HANDLE serial_port, const char *message) {
    // Declare variables
    DWORD bytes_written = 0;

    // Write the message
    BOOL status = WriteFile(
        serial_port,
        message,
        strlen(message),
        &bytes_written,
        NULL
    );

    // Validate result
    if(status == FALSE) {
        most_recent_windows_error();
        return 1;
    }

    return 0;
}

int read_from_port(const HANDLE serial_port) {
    // Declare variables
    DWORD bytes_read = 0;
    char receive_data_buffer[128] = {0};

    // Read from the serial port
    BOOL status = ReadFile(
        serial_port,
        receive_data_buffer,
        sizeof(receive_data_buffer),
        &bytes_read,
        NULL
    );

    // Validate result
    if(status == FALSE) {
        most_recent_windows_error();
        return 1;
    }

    // Print the message
    printf("\tMessage:\n\t\t%s\n\tBytes read: %lu\n", receive_data_buffer, bytes_read);
        // This works because I know that my message has a string terminator and 
            // is shorter than my buffer

    return 0;
}