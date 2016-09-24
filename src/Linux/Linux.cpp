#include <iostream>

#include "LinuxBaseLibrary.h"

using namespace std;
using namespace EPRI;

int main(int argc, char *argv[])
{
	LinuxBaseLibrary bl;

	ISerial * pSerial = bl.GetCore()->GetSerial();
	const uint8_t ucBuff[] = "HELLO WORLD!";
	pSerial->Open(LinuxSerial::SERIAL_PORT_1, LinuxSerial::BAUD_9600);
	pSerial->Write(ucBuff, sizeof(ucBuff));
	pSerial->Close();

}