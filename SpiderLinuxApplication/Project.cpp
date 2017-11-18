#include "terasic_os.h"
#include <pthread.h>
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

int main(int argc, char *argv[]){



  //####################################
  /*
    DO NOT TAKE THIS SECTION AWAY. SPIDER'S BEHAVIOR
    IS NOT AS EXPECTED WITHOUT THIS INITIALIZATION STEP
  */
  //####################################

	/*
	printf("Spider Init & Standup\r\n");
	if (!Spider.Init()){
		printf("Spilder Init failed\r\n");
	}else{
		if (!Spider.Standup())
			printf("Spilder Standup failed\r\n");
	}
	Spider.SetSpeed(50);

  //####################################
  */
  
  printf("===== Group <G> Final Project =====\r\n");
  CSpider Spider;
  Spider.Fold();
	
  // Uncomment to use the ADC class for reading IR sensor
   ADC adc;


	return 0;
}
