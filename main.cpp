#include <iostream>
#include <fstream>
#include <lsl_cpp.h>
#include <string>
#include <vector>
#include <atomic>
#include <memory>
#include <thread>

#include "windows.h"


int main(int argc, char *argv[]) {

    
    //std::cout << "HELLO STARTING PROGRAM" << '\n';

    // Communication settings
    int comPort = 4;
    int baudRate = 9600;
    int samplingRate = 0;
    int chunkSize = 0;
    std::string streamName = "SerialPort";
    int dataBits = 8;
    int parity = 0;
    int stopBits = 1;
    int readIntervalTimeout = 500;
    int readTotalTimeoutConstant = 50;
    int readTotalTimeoutMult = 10;

    //std::cout << "creating handle\n";

    // Creating Handle
    HANDLE hSerial = CreateFile("COM4", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    try
    {
        // Check for com port errors 
        if (hSerial == INVALID_HANDLE_VALUE)
        {
            if (GetLastError() == ERROR_FILE_NOT_FOUND)
            {
                std::cout << "SERIAL PORT DOES NOT EXIST\n";
                throw std::runtime_error("Could not open Serial Port, check that device ready and correct COM port");
            }
            std::cout << "ERROR OTHER ERROR OTHER\n";
        }

        //std::cout << "Settings parms\n";

        // Setup device control block parms, start by initializing object
        DCB dcbSerialParms = {0};

        // Check for parm errors
        if (!GetCommState(hSerial, &dcbSerialParms))
        {
            std::cout << "Error getting state\n";
            throw std::runtime_error("ERROR, could not get COM port State");
        }

        // Set parm states
        dcbSerialParms.BaudRate = CBR_9600;
        dcbSerialParms.ByteSize = 8;
        dcbSerialParms.StopBits = ONESTOPBIT;
        dcbSerialParms.Parity = NOPARITY;

        // Check for errors setting parm state
        if (!SetCommState(hSerial, &dcbSerialParms))
        {
            std::cout << "Error settign serial port state\n";
            throw std::runtime_error("ERROR, could not set parms (including baud)");
        }

        // Set timeouts
        COMMTIMEOUTS timeouts = {0};

        if (!GetCommTimeouts(hSerial, &timeouts))
        {
            throw std::runtime_error("ERROR, could not get COM port timeouts");
        }


        timeouts.ReadIntervalTimeout = 50;
        timeouts.ReadTotalTimeoutConstant = 50;
        timeouts.ReadTotalTimeoutMultiplier = 10;

        //timeouts.WriteTotalTimeoutConstant = 50;
        //timeouts.WriteTotalTimeoutMultiplier = 10;

        if (!SetCommTimeouts(hSerial, &timeouts))
        {
            std::cout << "ERROR on timeouts\n";
            throw std::runtime_error("ERROR, could not set COM port timeouts");
        }

        //create lsl objects

        
        // create info object
        lsl::stream_info info("ARDUINOSTREAM", "Raw", 1, samplingRate, lsl::cf_int16, "SERIAL_PORT_ARDUINO");

        // set meta data
        lsl::xml_element channels = info.desc().append_child("channels");

        channels.append_child("channel").append_child_value("label", "Channel1").append_child_value("type", "Raw").append_child_value("unit", "Integer");
        info.desc().append_child("acquistion").append_child_value("com_port", std::to_string(comPort)).append_child_value("baud_rate", std::to_string(baudRate));

        // create outlet object
        lsl::stream_outlet outlet(info, chunkSize);

        // Transmit
        const int n = 8;
        char szBuff[n+1] = {0};
        DWORD dwBytesRead = 0;
        float sample;

        std::cout << "OUTPUT OF COM PORT: \n" << std::flush;

        while(1)
        {
            if (!ReadFile(hSerial, szBuff, n, &dwBytesRead, NULL))
            {
                std::cout << "ERROR READING\n" <<std::flush;
                throw std::runtime_error("ERROR READING< DISCONNECT");
            }
            // THIS IS PROBABLY BREAKING STUFF DOWNSTREAM, THINK ABOUT WHAT TYPE THIS SHOULD BE
            sample = atof(szBuff);
            //printf(szBuff);
            //read_thread(hSerial, 4, 9600, 0, 0, "ARDUINOSTREAM");
            std::cout << szBuff << '\n' << std::flush;
            if (dwBytesRead)
            {
                outlet.push_sample(&sample);
            }
        }

        std::cout << "closing handle\n";

        CloseHandle(hSerial);

        


    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        std::cout << "closing handle\n";

        CloseHandle(hSerial);
    }
    

    

	return 0;
}