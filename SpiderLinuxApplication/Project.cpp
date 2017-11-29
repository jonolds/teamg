#include "terasic_os.h"
#include <pthread.h>
#include <bitset>
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
ADC adc;
CSpider Spider;
int stp = 0;

typedef enum{
	CMD_AT,
	CMD_FORWARD,
	CMD_BACKWARD,
	CMD_TURN_RIGHT,
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

void Dodge(CSpider &Spider, ADC &adc) {
	printf("\t[project]Starting Dodge Sequence...\r\n");
	int walked = 0;
	bool blocked = false;
	
	while(walked < 8) {
		int distance = adc.GetChannel(1);
		printf("%d\r\n", distance);
		if(distance < 550) {
			Spider.MoveForward(1);
			walked ++;
		}
		else {
			blocked = true;
			while(blocked) {
				Spider.MoveParallelR(1);
				if(adc.GetChannel(1) < 525) {
					Spider.MoveParallelR(5);
					Spider.RotatelRight(1);
					blocked = false;
				}
			}
		}
	}
	printf("\t[project]DONE\r\n");	
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
			printf("%d\n", Command);
			if (Command != CMD_IDLE){
				// push command to command queue
				if (Command == CMD_FORWARD) {
				   Spider.MoveForward(2);
				}
				if (Command == CMD_BACKWARD) {
				   Spider.MoveBackward(2);
				}
				if (Command == CMD_TURN_RIGHT) {
					Spider.MoveParallelR(2);
				}
				if (Command == CMD_TURN_LEFT) {
					Spider.MoveParallelL(2);
				}				
				if (Command == CMD_ALL) {
				   Dodge(Spider, adc);
				   pQueueCommand->Clear();
				}
				if (Command == CMD_STOP) {
				   stp = 1;
				   pQueueCommand->Clear();
				}				
				// push command to command queue 
				if (!pQueueCommand->IsFull()){
				   pQueueCommand->Push(Command, Param);
				}
			}
		}
		printf("[BT]Disconneected...\r\n");
		BtSppCommand.RfcommClose();
	}
//	pthread_exit(0); 
	return 0;
}


bool stringContains(string str, string subStr) {
	if(str.find(subStr) != string::npos)
		return true;
	else
		return false;
}

int getNum() {
	int num;
	printf("[project] Enter int...\r\n");
	cin >> num;
	return num;
}

void adcTest(uint8_t num, ADC &adc) {
	for(int i=0; i < num; i++) {
		printf("Ch1 Sensor Reading: %u\r\n", adc.GetChannel(1));
		sleep(1);
	}
}



int main(int argc, char *argv[]) {

	 //BT Stuff
	CQueueCommand QueueCommand;
    int Command, Param;
    bool bSleep = false;
    CPIO_LED LED_PIO;
    CPIO_BUTTON BUTTON_PIO;
    pthread_t id0;
    int ret0;
    uint32_t LastActionTime;
    const uint32_t MaxIdleTime = 10*60*OS_TicksPerSecond(); // spider go to sleep when exceed this time
	ret0=pthread_create(&id0,NULL,bluetooth_spp_thread, (void *)&QueueCommand);
	if(ret0!=0){
		printf("Creat pthread-0 error!\n");
		//exit(1);	
	} 
	Spider.SetSpeed(50);
	printf("[project] Spider Init & Standup\r\n");
	if (!Spider.Init()){
		printf("[project] Spider Init failed\r\n");
	}
	else{
		if (!Spider.Standup())
			printf("[project] Spider Standup failed\r\n");
	}
	
	printf("\r\n");
	printf("Manual Spider Control Commands:\r\n");
	printf("\tActions: reset, fold, extend, angles\r\n");
	printf("\t  roll, bodyF, bodyN, bodyB\r\n");
	printf("\t  tiltR, tiltL, tiltN, tiltF, tiltB\r\n");
	printf("\t  dodge (walk forward until sensor above 700 then move side)\r\n");
	printf("\tThese ask for an integer after entering command: \r\n");
	printf("\t  adc, forward, back, right, left, speed\r\n");
	printf("\t  rotL, rotR, bodyUD, stomp, bye, dance\r\n");
	printf("\r\n");

	string command = "";
	printf("SpiderController# ");
	//cin >> command;
	
	//printf("\r\n");
	
	while(command != "exit" && stp == 0) {
		//Dodge sequence
		if(stringContains(command, "dodge"))
			Dodge(Spider, adc);
		// Reset - sets the legs to base position
		else if(stringContains(command, "reset"))
			Spider.SetLegsBase();
		// Extend - extends knees and ankles
		else if(stringContains(command, "extend"))
			Spider.Extend();
		// Fold - Compactly folds legs for easy storage
		else if(stringContains(command, "fold"))
			Spider.Fold();
		else if(stringContains(command, "roll"))
			Spider.DEMO_Rollover();
		else if(stringContains(command, "bodyF"))
			Spider.BodyForward();
		else if(stringContains(command, "bodyN"))
			Spider.BodyNone();
		else if(stringContains(command, "bodyB"))
			Spider.BodyBackward();
		else if(stringContains(command, "tiltR"))
			Spider.TiltRight();
		else if(stringContains(command, "tiltL"))
			Spider.TiltLeft();
		else if(stringContains(command, "tiltN"))
			Spider.TiltNone();
		else if(stringContains(command, "tiltF"))
			Spider.TiltForward();
		else if(stringContains(command, "tiltB"))
			Spider.TiltBackward();
		else if(stringContains(command, "angles"))
			Spider.GetAllAngles();
		
		//*****This set asks for an integer input after typing command
		else if(stringContains(command, "adc"))
			adcTest(getNum(), adc);
		else if(stringContains(command, "forward"))
			Spider.MoveForward(getNum());
		else if(stringContains(command, "back"))
			Spider.MoveBackward(getNum());
		else if(stringContains(command, "right"))
			Spider.MoveParallelR(getNum());
		else if(stringContains(command, "left"))
			Spider.MoveParallelL(getNum());
		else if(stringContains(command, "rotR"))
			Spider.RotatelRight(getNum());
		else if(stringContains(command, "rotL"))
			Spider.RotatelLeft(getNum());
		else if(stringContains(command, "halfR"))
			Spider.RotatelRight2(getNum());
		else if(stringContains(command, "halfL"))
			Spider.RotatelLeft2(getNum());
		
		//testing
		else if(stringContains(command, "test")) {
			int num = getNum();
			for(int i = 0; i < num; i++) {
				adcTest(1, adc);
				Spider.RotatelRight2(1);
			}
		}
		
		else if(stringContains(command, "bodyUD"))
			Spider.BodyUpDown(getNum());
		else if(stringContains(command, "stomp"))
			Spider.BodyUpDown(getNum());
		else if(stringContains(command, "bye"))
			Spider.ByeBye(getNum());
		else if(stringContains(command, "dance"))
			Spider.DEMO_Dance(getNum());
		else if(stringContains(command, "speed")) {
			printf("[project] Enter int between 10 and 100...\r\n");
			int num;
			cin >> num;
			Spider.SetSpeed(num);	
		}
		// Invalid
	//	else
			//printf("[project]ERROR - Invalid spider command: %s\r\n", command.c_str()); 
		
		// Get the next command
		//printf("SpiderController# ");
		//cin >> command;
	}
	return 0;
}
