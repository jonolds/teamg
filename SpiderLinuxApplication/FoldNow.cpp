#include <iostream>
#include <string.h>
#include "terasic_os.h"
#include "CSpider.h"
#include "CSpiderLeg.h"
#include "CMotor.h"
#include "mmap.h"
using namespace std;


int main(int argc, char *argv[])
{
	CSpider spid(3.8);
	spid.Fold();

	return 0;
}