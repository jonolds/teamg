#include <iostream>
#include <string.h>
#include "terasic_os.h"
#include "CSpider.h"
#include "CSpiderLeg.h"
#include "CMotor.h"
#include "mmap.h"
using namespace std;

//just folds spider by running ./fold
int main(int argc, char *argv[])
{
	CSpider spid;
	spid.SetSpeed(50);
	spid.Fold();
	return 0;
}