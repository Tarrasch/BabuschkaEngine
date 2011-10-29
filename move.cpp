#include "engine.h"


bool move_is_legal(position_t *pos, move_t m){
	return true;
}
/* Grejen är att funktionen verkar vara ett komplement till generate_moves.
 * Det vore som att generate_moves skapar olagliga drag
 * */

//// move_is_check() is a simple function for determining whether a move
//// checks the opponent.  It does not work for promotions, castling or
//// en passant moves.

void make_move(position_t *pos, move_t m, undo_info_t *u) {
  int from, to;
  int side = pos->side;

  u->last_move = pos->last_move;


  for(int i=0; i<4; i++)
      pos->ply_finished[i]++; //notera, när spelarna är inte klara så bollar den runt med stora negativa tal bara

  if(m!=NOLEGALMOVE){

  pos->moves_since_last_legal=0;

  from=FROM(m); to=TO(m);

  //MovePiece(pos, from, to);
  //min motor har detta inbygt i make_move

  pos->piece_pos[pos->board[from].pieces[pos->board[from].n-1]]=to;
  pos->board[to].pieces[pos->board[to].n++]=pos->board[from].pieces[--(pos->board[from].n)];

  if(pos->board[turnback(0x73,side)].n==3 && side==PiecePlayer(pos->board[turnback(0x73,side)].pieces[1]) &&
     pos->board[turnback(0x74,side)].n==3 && side==PiecePlayer(pos->board[turnback(0x74,side)].pieces[1])){
        //en spelare har kommit i mål
    pos->winner[(pos->winners)++]=side;
    pos->ply_finished[side]=0;
    }
  }
  else pos->moves_since_last_legal--;


  pos->last_move = m;
  pos->gply++;
  pos->side = (pos->side+1)%4;
}

void unmake_move(position_t *pos, move_t m, undo_info_t *u) {//SIDA ÄR OINITSIERAD!!!!!!!!!!!!!
  int from, to;

  pos->gply--;
  pos->side = (pos->side-1+4)%4;
  int side = pos->side; //OK nu är den INITSIERAD men jag är ej 100 detta va altl som behövs (se framework för fung variant)


  for(int i=0; i<4; i++)
      pos->ply_finished[i]--;

  if(m!=NOLEGALMOVE){

  pos->moves_since_last_legal=0;

  pos->last_move = u->last_move; //ska de typ inte va ovanför if-satsen?

  from = FROM(m); to = TO(m);

  pos->piece_pos[pos->board[to].pieces[pos->board[to].n-1]]=from;
  pos->board[from].pieces[pos->board[from].n++]=pos->board[to].pieces[--(pos->board[to].n)];


  if(pos->winners && pos->winner[pos->winners-1]==side){   //en spelare har kommit i mål
    pos->winners--;
    }
  }
  else pos->moves_since_last_legal--;
}
