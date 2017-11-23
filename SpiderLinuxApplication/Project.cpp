#include "terasic_os.h"
#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "CSpider.h"
#include "CSpiderLeg.h"
#include "CMotor.h"
#include "BtSppCommand.h"
#include "QueueCommand.h"
#include "PIO_LED.h"
#include "PIO_BUTTON.h"
#include "ADC.h"
#include "mmap.h"

using namespace std;

typedef enum{
	CMD_AT,
	CMD_FORDWARD,
	CMD_BACKWARD,
	CMD_TURN_RIHGT,
	CMD_TURN_LEFT,
	CMD_TURN_RIHGT_DGREE,
	CMD_TURN_LEFT_DGREE,
	CMD_STOP,
	CMD_SPPED,
	CMD_TILTL,
	CMD_TILTR,
	CMD_TILTF,
	CMD_TILTB,
	CMD_TILTN,
	CMD_Query_Version,
	CMD_JOYSTICK,
	CMD_ALL,
	CMD_IDLE,
}COMMAND_ID;

bool stringContains(string str, string subStr) {
	if(str.find(subStr) != string::npos)
		return true;
	else
		return false;
}

static void *bluetooth_spp_thread(void *ptr)
{
	CBtSppCommand BtSppCommand;
	CQueueCommand *pQueueCommand;
	int Command, Param;
	pQueueCommand = (CQueueCommand *)ptr;
	printf("[BT]Start Service\r\n");
	BtSppCommand.RegisterService();
	while(1){
		printf("[BT]Listen...\r\n");
		BtSppCommand.RfcommOpen();
		printf("[BT]Connected...\r\n");
		while(1){
			Command = BtSppCommand.CommandPolling(&Param);
			if (Command != CMD_IDLE){
				// push command to command queue
				if (Command == CMD_STOP)
				   pQueueCommand->Clear();
				// push command to command queue 
				if (!pQueueCommand->IsFull()){
				   pQueueCommand->Push(Command, Param);
				}
			}
		}
		printf("[BT]Disconneected...\r\n");
		BtSppCommand.RfcommClose();
	}
//	pthread_exit(0); /* exit */
	return 0;
}

void Dodge(CSpider Spider) {
	int walked = 0;
	ADC adc;
	bool alt = true;
	bool blocked = false;
	
	while(walked < 8) {
		int distance = adc.GetChannel(1);
		printf("%d\r\n", distance);
		if(distance < 700) {
			Spider.MoveForward(1);
			walked ++;
		}
		else {
			blocked = true;
			while(blocked) {
				if(alt == true)
					Spider.MoveParallelR(3);
				else
					Spider.MoveParallelL(3);
				distance = adc.GetChannel(1);
				if(distance < 700)
					blocked = false;
			}	
			alt = !alt;
		}
	}
}

int main(int argc, char *argv[]) {
	
	CSpider Spider;
    CQueueCommand QueueCommand;
    int Command, Param;
    bool bSleep = false;
    CPIO_LED LED_PIO;
    CPIO_BUTTON BUTTON_PIO;
    pthread_t id0;
    int ret0;
    uint32_t LastActionTime;
    const uint32_t MaxIdleTime = 10*60*OS_TicksPerSecond(); // spider go to sleep when exceed this time
	
	Spider.SetSpeed(50);
	printf("Spider Init & Standup\r\n");
	if (!Spider.Init()){
		printf("Spider Init failed\r\n");
	}
	else{
		if (!Spider.Standup())
			printf("Spider Standup failed\r\n");
	}
	
	/*
	ret0=pthread_create(&id0,NULL,bluetooth_spp_thread, (void *)&QueueCommand);
	if(ret0!=0){
		printf("Creat pthread-0 error!\n");
		//exit(1);	
	}
	*/
	
	printf("\r\n");
	printf("===== Spider Controller =====\r\n");
	printf("Manual Spider Control\r\n");
	printf("Commands:\r\n");
	printf("\tCommand the spider to perform specific action:\r\n");
	printf("\t\tActions: reset, fold, extend, dodge  \r\n");
	printf("\t\t-------dodge walks forward until sensor goes above 700 then moves to the side\r\n");
	printf("\t\tforward, back, right, left\r\n");
	printf("\r\n");

	string command = "";
	printf("SpiderController# ");
	cin >> command;
	printf("\r\n");

	while(command != "exit") {

		// Reset - sets the legs to base position
		if(stringContains(command, "reset")) {
			printf("\tResetting legs...");
			Spider.SetLegsBase();
			printf("DONE\r\n");
		}
		// Extend - extends knees and ankles
		else if(stringContains(command, "extend")) {
			printf("\tExtending legs...");
			Spider.Extend();
			printf("DONE\r\n");
		}
		// Fold - Compactly folds legs for easy storage
		else if(stringContains(command, "fold")) {
			printf("\tFolding legs...");
			Spider.Fold();
			printf("DONE\r\n");
		}
		else if(stringContains(command, "forward")) {
			printf("\tFordward...");
			Spider.MoveForward(4);
			printf("DONE\r\n");
		}
		else if(stringContains(command, "back")) {
			printf("\tBack...");
			Spider.MoveBackward(4);
			printf("DONE\r\n");
		}
		else if(stringContains(command, "right")) {
			printf("\tRight...");
			Spider.MoveParallelR(8);
			printf("DONE\r\n");
		}
		else if(stringContains(command, "left")) {
			printf("\tLeft...");
			Spider.MoveParallelL(8);
			printf("DONE\r\n");
		}
		else if(stringContains(command, "dodge")) {
			printf("\tStarting Dodge Sequence...");
			Dodge(Spider);
			printf("DONE\r\n");				
		}
		// Invalid
		else
			printf("ERROR - Invalid spider command: %s\r\n", command.c_str()); 
		
		// Get the next command
		printf("SpiderController# ");
		cin >> command;
		if(command != "exit")
			printf("\r\n");
	}
	return 0;
}
