#include "engine.h"
#include <sstream>
#include <string>
using namespace std;

/*
 * static void init_tables(void) {
  int i;
  for(i=0; i<128; i++)
    PawnRank[WHITE][i] = PawnRank[BLACK][i^0x70] = rank(i);
}
*/


static void init_cantmoveto(void) {
  int i;
  for(i=0; i<128; i++)
    cantmovetoarray[i] = (i&8)>0;
  for(i=0; i<ANTALOEXISTERANDERUTOR; i++)
    cantmovetoarray[OEXISTERANDERUTOR[i]] = true;

  for(i=0; i<128; i++)
	  dontmovetoarray[i] = (i&8)>0;
  for(i=0; i<ANTALOREKOMMENDERADERUTOR; i++)
	  dontmovetoarray[OREKOMMENDERADERUTOR[i]] = true;
}

static void init_params(void) {
	eval_params_t* cep = eval_params;
	stringstream ss(params_as_string);
	for (unsigned int i = 0; i < sizeof(eval_params_t)/sizeof(double); ++i) {
		string s;

		ss >> s;
		if(s=="nan")
			((double*)cep)[i]=NAN;
		else{
			stringstream ss;
			ss << s;
			ss >> ((double*)cep)[i];
		}
	}
}




/*
static void init_psq_tables(void) {
  int i, j;
  for(j=BP; j<=BK; j++)
    for(i=0; i<128; i++)
      if((i & 0x88) == 0)
    PSqTables[j][i ^ 0x70] = PSqTables[j-8][i];
}
*/

void init(void) {
init_cantmoveto();
init_params();
//init_tables();
//init_psq_tables();
}

