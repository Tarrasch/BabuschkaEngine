#include "engine.h"


square_t unturn(square_t square,int side){
	if(side)
	 return unturn((7-rank(square))|(file(square)<<4),side-1);
	return square;
}


square_t turnback(square_t square,int side){
	return unturn(square,(4-side)%4);
}

bool squareaheadof(square_t square,square_t square0, int side){
	if(rank(unturn(square,side))>rank(unturn(square0,side)))
		return true;
	return false;
	}
