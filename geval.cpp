#include "../generella/engine.h"

Valuequad price_quad={0.65,0.25,0.1,0.0};

float get_current_maxsum(const position_t *pos){
    float prices_gone=0;
    for(int i=0;i<pos->winners;i++)
        prices_gone+=((float*)(&price_quad))[i];
    return MAXSUM-prices_gone;
}

float get_maxscore_still_avaible(const position_t *pos){ //för immidiete pruning
    return ((float*)(&price_quad))[pos->winners];
}


Valuequad end_of_game_scores(const position_t *pos) {
	Valuequad vq;
	float* res=(float*)(&vq);
	const float* at_pq= (float*)&price_quad;
	int winners=pos->winners;
	bool winner[4]={0,0,0,0};//denna är bool efter spelare, pos->winner är typ pristagarna i ordning

	for (int i = 0; i < winners; ++i) {
		res[pos->winner[i]]=at_pq[i];
		winner[pos->winner[i]]=true;
	}
	double restsum=0.0;

	for (int i = winners; i < 4; ++i) {
			restsum+=at_pq[i];
		}

	for (int i = 0; i < 4; ++i) {
			if(!winner[i])
				res[i]=restsum/(4-winners);
		}
	return vq;
}

int8 Wing[128]=
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0,
 1, 1, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
 1, 1, 1, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
 1, 1, 1, 1, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0};

