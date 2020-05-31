#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <WaspLoRaWAN.h>
#include <WaspFrame.h>

/*
�� ������ OTAA�� ����ؼ� ��Ʈ��ũ(�� ������ ������ ��Ʈ��ũ ������ ACK�� ����Ͽ� ����� ������ ��, LoRaWAN ����Ʈ���̿� �������� �۽��ϴ� �ڵ��Դϴ�.
*/

// ���� ���
uint8_t socket = SOCKET0;

// ����̽� �Ķ���� ����
char DEVICE_EUI[] = "";	
char APP_EUI[] = "";
char APP_KEY[] = "";

// �������� ����� ��Ʈ ����
uint8_t PORT = 3;

// ���� ����
uint8_t error;
uint8_t error_config = 0;

// �ͽ���Ʈ ID ����
char moteID[] = "node_01";

void setup()
{
	USB.ON();
	USB.println(F("LoRaWAN �ۼ��� ���� - Ȯ�ε� ��Ŷ ACK �۽�\n"));

	USB.println(F("------------------------------------"));
	USB.println(F("��� ����"));
	USB.println(F("------------------------------------\n"));

	// 1. ����ġ ON
	error = LoRaWAN.ON(socket);

	// ���� �˻�
	if (error == 0)
		USB.println(F("1. Switch ON OK"));
	else
	{
		USB.print(F("1. Switch ON error = "));
		USB.println(error, DEC);
		error_config = 1;
	}

	// ������ ���ۼӵ� ����
	error = LoRaWAN.setDataRate(3);

	// ���� �˻�
	if (error == 0)
		USB.println(F("2. Data rate set OK"));
	else
	{
		USB.print(F("2. Data rate set error= "));
		USB.println(error, DEC);
		error_config = 2;
	}

	// 3. ����̽� EUI ����
	error = LoRaWAN.setDeviceEUI(DEVICE_EUI);

	// ���� �˻�
	if (error == 0)
		USB.println(F("3. Device EUI set OK"));
	else
	{
		USB.print(F("3. Device EUI set error = "));
		USB.println(error, DEC);
		error_config = 3;
	}

	// 4. ���ø����̼� EUI ����
	error = LoRaWAN.setAppEUI(APP_EUI);

	// ���� �˻�
	if (error == 0)
		USB.println(F("4. Application EUI set OK"));
	else
	{
		USB.print(F("4. Application EUI set error = "));
		USB.println(error, DEC);
		error_config = 4;
	}

	// 5. ���ø����̼� ���� Ű ����
	error = LoRaWAN.setAppKey(APP_KEY);

	// ���� �˻�
	if (error == 0)
		USB.println(F("5. Application Key set OK"));
	else
	{
		USB.print(F("5. Application Key set error = "));
		USB.println(error, DEC);
		error_config = 5;
	}

	// 6. OTAA�� �����Ͽ� ������ Ű ����
	error = LoRaWAN.joinOTAA();

	// ���� �˻�
	if (error == 0)
		USB.println(F("6. Join network OK"));
	else
	{
		USB.print(F("6. Join network error = "));
		USB.println(error, DEC);
		error_config = 6;
	}

	// 7. ���� ����
	error = LoRaWAN.saveConfig();

	// ���� �˻�
	if (error == 0)	
		USB.println(F("7. Save configuration OK"));
	else
	{
		USB.print(F("7. Save configuration error = "));
		USB.println(error, DEC);
		error_config = 7;
	}

	// 8. ����ġ OFF
	error = LoRaWAN.OFF(socket);

	// ���� �˻�
	if (error == 0)	
		USB.println(F("8. Switch OFF OK"));
	else
	{
		USB.print(F("8. Switch OFF error = "));
		USB.println(error, DEC);
		error_config = 8;
	}

	if (error_config == 0) {
		USB.println(F("\n---------------------------------------------------------------"));
		USB.println(F("Module configured"));
		USB.println(F("After joining through OTAA, the module and the network exchanged "));
		USB.println(F("the Network Session Key and the Application Session Key which "));
		USB.println(F("are needed to perform communications. After that, 'ABP mode' is used"));
		USB.println(F("to join the network and send messages after powering on the module"));
		USB.println(F("---------------------------------------------------------------\n"));
		USB.println();
	}
	else {
		USB.println(F("\n---------------------------------------------------------------"));
		USB.println(F("Module not configured"));
		USB.println(F("Check OTTA parameters and reestart the code."));
		USB.println(F("If you continue executing the code, frames will not be sent."));
		USB.println(F("\n---------------------------------------------------------------"));
	}

	frame.setID(moteID);
}

void loop()
{
	// 1. �� ������ ����

	// ACC �ʱ�ȭ
	ACC.ON();

	USB.println(F("1. Creating an BINARY frame"));

	// �� ������ ����
	frame.createFrame(BINARY);

	// ������ �ʵ� ���� (���͸� ���� - uint8_t)
	frame.addSensor(SENSOR_BAT, PWR.getBatteryLevel());

	// ������ ������ �ʵ� ����
	frame.addSensor(SENSOR_ACC, ACC.getX(), ACC.getY(), ACC.getZ());

	// ������ ���
	frame.showFrame();

	// ���ӵ��� OFF
	ACC.OFF();

	// 2. ����ġ ON
	error = LoRaWAN.ON(socket);

	// ���� �˻�
	if (error == 0)	
		USB.println(F("2. Switch ON OK"));
	else
	{
		USB.print(F("2. Switch ON error = "));
		USB.println(error, DEC);
	}

	// 3. ��Ʈ��ũ ����
	error = LoRaWAN.joinABP();

	// ���� �˻�
	if (error == 0)
	{
		USB.println(F("3. Join network OK"));

		error = LoRaWAN.getMaxPayload();

		if (error == 0)
		{
			// 4. ������ ������ Ÿ�̴� ������ ����
			USB.print(F("4.1. LoRaWAN maximum payload: "));
			USB.println(LoRaWAN._maxPayload, DEC);

			// �ִ� ���̷ε� ����
			frame.setTinyLength(LoRaWAN._maxPayload);

			boolean end = false;
			uint8_t pending_fields = 0;

			while (end == false)
			{
				pending_fields = frame.generateTinyFrame();

				USB.print(F("4.2. Tiny frame generated:"));
				USB.printHexln(frame.bufferTiny, frame.lengthTiny);

				// 5. Ȯ�ε� ��Ŷ ����
				USB.println(F("5. LoRaWAN confirmed sending..."));
				error = LoRaWAN.sendConfirmed(PORT, frame.bufferTiny, frame.lengthTiny);

				// ���� �޽���:
				/*
				  '6' : ����� ��Ʈ��ũ�� ���ԵǾ� ���� ����
				  '5' : ���� ����
				  '4' : ������ ���� ����
				  '2' : ��� ���� ����
				  '1' : ��� �ۼ��� ����
				*/

				// ���� �˻�
				if (error == 0)
				{
					USB.println(F("5.1. LoRaWAN send packet OK"));
					if (LoRaWAN._dataReceived == true)
					{
						USB.print(F("  There's data on port number: "));
						USB.print(LoRaWAN._port, DEC);
						USB.print(F("\r\n  Data: "));
						USB.println(LoRaWAN._data);
					}
				}
				else
				{
					USB.print(F("5.1. LoRaWAN send packet error = "));
					USB.println(error, DEC);
				}

				if (pending_fields > 0)
				{
					end = false;
					delay(10000);
				}
				else
					end = true;
				
			}
		}
		else
		{
			USB.println(F("4. LoRaWAN error getting the maximum payload"));
		}
	}
	else
	{
		USB.print(F("2. Join network error = "));
		USB.println(error, DEC);
	}

	// 6. ����ġ OFF
	error = LoRaWAN.OFF(socket);

	// ���� �˻�
	if (error == 0)
		USB.println(F("6. Switch OFF OK"));
	else
	{
		USB.print(F("6. Switch OFF error = "));
		USB.println(error, DEC);
	}

	USB.println();
	delay(10000);
}