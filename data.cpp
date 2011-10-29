#include "engine.h"

uint8 PawnRank[2][128];

uint8 Distance_[256];
uint8 *Distance = Distance_+128;

uint32 History[2][4096];

root_search_info_t RSI[1];

engine_options_t Options[1];

eval_params_t eval_params[1];

int selfplayerid;

int InitialKSQ, InitialKRSQ, InitialQRSQ;

bool EngineShouldQuit = false;

char startposition[24*3+1+1]="0d10e11d11e12d12e10a50a41a51a42a52a40e80d81e81d82e82d80h40h51h41h52h42h51";/*{
     Bfo(0,0,3),Bfo(0,0,4),Bfo(1,0,3),Bfo(1,0,4),Bfo(2,0,3),Bfo(2,0,4),
     Bfo(0,3,0),Bfo(0,4,0),Bfo(1,3,0),Bfo(1,4,0),Bfo(2,3,0),Bfo(2,4,0),
     Bfo(0,7,3),Bfo(0,7,4),Bfo(1,7,3),Bfo(1,7,4),Bfo(2,7,3),Bfo(2,7,4),
     Bfo(0,3,7),Bfo(0,4,7),Bfo(1,3,7),Bfo(1,4,7),Bfo(2,3,7),Bfo(2,4,7),
     '1','\0'};
*/

