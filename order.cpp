#include "engine.h"

void order_moves(const position_t *pos, search_stack_t *ss, 
		 int ply, move_t pvmove) {
  move_stack_t *ms;
  move_t m;
  return;
}

move_t pick_move(move_stack_t **start, move_stack_t *end, bool look_for_best,int player) {
  move_stack_t *ms, tmp, *best = NULL;
  Valuequad best_score = WORSTSCORE;
  if(*start == end) return 0; 
  if(!look_for_best || look_for_best) return ((*start)++)->move; //Ändra sen förstås, orginal var '!look_for_best'
  for(ms = *start; ms < end; ms++)
    if(PerspectiveScore(&(ms->score),player) > PerspectiveScore(&best_score,player)) {
      best = ms; best_score = best->score;
    }
  if(best != NULL) {
    tmp = **start; **start = *best; *best = tmp;
    return ((*start)++)->move;
  }
  else return 0; 
}
