#include "engine.h"

const int ANTALOEXISTERANDERUTOR=12;
     square_t OEXISTERANDERUTOR[ANTALOEXISTERANDERUTOR]=

     //Illegala rutor från alla spelares perspektiv (i.o.m. symmetrin)

     {

     0x00,
     0x01,
     0x06,
     0x07,
     0x10,
     0x17,
     0x60,
     0x67,
     0x70,
     0x71,
     0x76,
     0x77
     };

const int ANTALOREKOMMENDERADERUTOR=12+12;
     square_t OREKOMMENDERADERUTOR[ANTALOREKOMMENDERADERUTOR]=

     //Illegala rutor från alla spelares perspektiv (i.o.m. symmetrin)

     {

     0x00,
     0x01,
     0x06,
     0x07,
     0x10,
     0x17,
     0x60,
     0x67,
     0x70,
     0x71,
     0x76,
     0x77 //Där ingen ruta finns

     ,
     0x02,
     0x11,
     0x20,
     0x05,
     0x16,
     0x27,
     0x72,
     0x61,
     0x50,
     0x75,
     0x66,
     0x57 //Där motorn inte bör gå (för att inte fastna o.s.v
     };

bool cantmovetoarray[128];
bool dontmovetoarray[128];


void init_position(position_t *pos) {
  for(int i=0; i<256; i++)
	pos->board_[i] =  ((i-64)&0x88)? FULL:EMPTY;

  pos->board = pos->board_ + 64;

  for(int i=0; i<ANTALOEXISTERANDERUTOR; i++)
  	pos->board[OEXISTERANDERUTOR[i]] = FULL;

  pos->winners=0;

  for(int i=0; i<4; i++)
      pos->ply_finished[i]=-1000; //bara det är <-2

  pos->moves_since_last_legal=0;
}

void copy_position(position_t *dst, const position_t *src) {
  memcpy(dst, src, sizeof(position_t));
  dst->board = dst->board_ + 64;
}

bool legaltojumpthrough(square_t square, int size, square_container_t *board){
        return size>TopSizeForSquares(board,square);
}

bool has_finished(const position_t *pos,int side){
       /*
        for(int i=0; i<pos->winners;i++)
            if(pos->winner[i]==side)
                return true;
        return false;
      */
        return pos->ply_finished[side]>=0;
}

int prev_player(const position_t *pos){ //previous player, taking account players have finished
    int side = pos->side;

    while(has_finished(pos,--side)){
        side=(side+4)%4;
        if(side==pos->side)
        	return side;
    }

    return side;
}

bool end_of_game(const position_t *pos) {
	if(pos->winners>=3 || pos->moves_since_last_legal>=4)
		return true;
	return false;
}


bool legaltomoveto(square_t square, doll_t doll , position_t *pos){

        if(!legaltojumpthrough(square,PieceSize(doll),pos->board))
        	return false;//Man kan aldrig flytta till en ruta man ej kan hoppa igenom

        if(cantmovetoarray[square])
            return false;

        if(!squareaheadof(square,pos->piece_pos[doll],PieceColor(doll)))
			return false;

        square_t tempkoord=unturn(square,PieceColor(doll));
/*
        if(is_in(tempkoord,ILLEGALARUTOR,ANTALILLEGALARUTOR))
        	return false;
*/
        if( ((file(tempkoord)==0) || (file(tempkoord)==7)) && ((rank(tempkoord)==3) || (rank(tempkoord)==4)))
            return false; //ifall man anländar på de andras morph


        if(rank(tempkoord)==7){ //på sista raden (mål)
           if(pos->board[square].n==0)
              if(PieceSize(doll)!=1)
         	    return false; //Ifall den första anländande inte är en minsta storlek

                   else;
           else
           if (PiecesHaveOppositeColor(doll,(pos->board[square].pieces[pos->board[square].n-1])) or ((PieceSize(doll)-1)!=TopSizeForSquares(pos->board,square)))
                  return false;
           if(tempkoord==SQUARE(2,7) || tempkoord==SQUARE(5,7))
        	   return false;
        }


        return true;
}



int TopSizeForSquares(const square_container_t *board, square_t square){

	 if (board[square].n>0)
        return PieceSize(board[square].pieces[board[square].n-1]);
     return 0;
}


