#include "engine.h"

Valuequad search(position_t *pos, search_stack_t *sstack, int ply, int depth,
    int follow_pv, Valuequad bestall) {

  Valuequad bestvalue, value;
  int legal_moves=0, mate_threat=0, ext;

  int side = (selfplayerid+ply)%4;
  move_t move, pvmove, hashmove;
  move_t searched_moves[256];
  move_stack_t *ms;
  undo_info_t u[1];
  search_stack_t *ss = sstack+ply;


  ss->pv[ply] = NOMOVE;
  if(RSI->thinking_status == ABORTED) return WORSTSCORE;

  if(pos->moves_since_last_legal>3)
	  return evaluate(pos);

  if(ply >= 60) return evaluate(pos);

  if(depth < PLY) {
    return evaluate(pos);
  }

  Valuequad this_state_eval=evaluate(pos);

  if(PerspectiveScore(&this_state_eval,prev_player(pos))<0.01 && depth/PLY<3)//Fallen då man nollats, och inte hinner se ifall man kan förbättra
	  return this_state_eval;

  if(PerspectiveScore(&bestall,side)>0){//ifall den ens fyllts i
	  Valuequad r=search(pos, sstack, ply, depth-PLY*4, follow_pv,bestall);
	  if(PerspectiveScore(&r,side)*1.2<PerspectiveScore(&bestall,side))
		  return r; //Arash Heuristic
  }

  if(follow_pv) pvmove = sstack[0].pv[ply]; else pvmove = NOMOVE;

  init_node(pos, sstack, ply);

  (ss+1)->ms_ptr = generate_moves(pos, ss->ms_ptr);

  order_moves(pos, sstack, ply, pvmove); ms = ss->ms_ptr;

  bestvalue = WORSTSCORE;

  while((move = pick_move(&ms, (ss+1)->ms_ptr, legal_moves < 16,side))) {
      *((float*)&bestall+side)=PerspectiveScore(&bestvalue,side);
    if(move_is_legal(pos, move)) {
      searched_moves[legal_moves] = move;
      legal_moves++;

      ss->move = move;
      make_move(pos, move, u);
      ext = extend(pos, ss, mate_threat);

      if(legal_moves == 1)
	value = search(pos, sstack, ply+1, depth-PLY+ext, follow_pv,bestall);
      else {
	    value = search(pos, sstack, ply+1, depth-PLY+ext, 0,bestall);
	}

      unmake_move(pos, move, u);
      if(PerspectiveScore(&value,side) >= PerspectiveScore(&bestvalue,side)) {
    if(PerspectiveScore(&value,side) >= (get_current_maxsum(pos)-PerspectiveScore(&bestall,prev_player(pos))))
  return value;
    bestvalue = value;
	update_pv(sstack, ply);//tror detta kan va fel!! Men liet svårt att fixa
//	  if(true) inc_history(sstack, ply, move, side, depth, value);
	}

    }
   }
  if(legal_moves == 0) return WORSTSCORE; //Annat tillvägagånssätt måste göras sen //Ska inte kunna ske längre

  if(RSI->thinking_status == ABORTED) return WORSTSCORE; //Annat tillvägagånssätt måste göras sen

  return bestvalue;
}
