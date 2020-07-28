const int DEFAULT_SERIAL_BAUD_RATE = 115200;

// components


// pings
const unsigned long SLAVE_PING_INTERVAL = 1000 * 10;        // 10s
const unsigned long SLAVE_PING_DROPPED_DURATION = 1000 * 1; // 1s after sending out ping
const unsigned int SLAVE_MAX_DROPPED_PINGS = 3;

// serial communications
const String HEADER_DELIMITER = ",";
const String BODY_DELIMITER = "-";
struct SlaveCommsFormat
{
    String uuid;
    int msgLength;          // message length including STX and ETX characters
    String componentId;     // Identifies slave component to target
    String componentAction; // enum containing component action to perform
    String instructions;    // instructions associated with action to perform
};