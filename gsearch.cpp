#include "../generella/engine.h"
/*
#define HINDEX2(pos, x) (COMPRESS(TO(x))|((pos->board[FROM(x)]-1)<<6))
#define HINDEX3(pos, x) (COMPRESS(TO(x))|((pos->board[TO(x)]-1)<<6))

*/

/*
struct move_stats_t {
  int success, failure;
};

static move_stats_t MoveStats[64*BK];

void init_move_stats(void) {
  memset(MoveStats, 0, 64*BK*sizeof(move_stats_t));
}

*/

/*
static void success(const position_t *pos, move_t m) {
  MoveStats[HINDEX2(pos, m)].success++;
}

static void failure(const position_t *pos, move_t m) {
  MoveStats[HINDEX2(pos, m)].failure++;
}

static int success_count(const position_t *pos, move_t m) {
  return MoveStats[HINDEX3(pos, m)].success;
}

static int failure_count(const position_t *pos, move_t m) {
  return MoveStats[HINDEX3(pos, m)].failure;
}
*/

void inc_history(search_stack_t *ss, int ply, move_t m, int side,
            int depth, Valuequad value) {
//  if(!CAPTURE(m) && !PROMOTION(m)) {
    HISTORY(side, m) += (depth>>5);
    ss[ply].killer2 = ss[ply].killer; ss[ply].killer = m;

}


static void check_for_timeout(void) {
  int t;
  static int last_info_time;

  t = get_time() - RSI->start_time;
  if(t<1000) last_info_time = 0;
  else if(t-last_info_time>=1000) {
    mutex_lock(IOLock);
    printf("info nodes " llu_format " nps " llu_format " time %d\n",
           RSI->nodes, (RSI->nodes*1000ULL)/((uint64) t), t);
    mutex_unlock(IOLock);
    last_info_time = t;
  }
  if(RSI->iteration >= 2) {
    if(!RSI->infinite &&
       (t > RSI->absolute_max_time
        || (RSI->root_moves.current == 1 && t > RSI->max_time)
        || (!RSI->fail_high && t > 6 * RSI->max_time)))
      RSI->thinking_status = ABORTED;
    if(RSI->node_limit && RSI->nodes >= RSI->node_limit)
      RSI->thinking_status = ABORTED;
    if(RSI->exact_time && t >= RSI->exact_time)
      RSI->thinking_status = ABORTED;
  }
}

void init_node(const position_t *pos, search_stack_t *sstack, int ply) {
  search_stack_t *ss = sstack+ply;
  RSI->nodes++; RSI->nodes_since_poll++;
  if(RSI->nodes_since_poll >= RSI->nodes_between_polls) {
    check_for_input();
    check_for_timeout();
    RSI->nodes_since_poll = 0;
  }

  ss->pv[ply] = ss->move = NOMOVE;
  (ss+1)->pv[ply+1] = NOMOVE;
  (ss+2)->killer = (ss+2)->killer2 = NOMOVE;
}

void update_pv(search_stack_t *sstack, int ply) {
  search_stack_t *ss = sstack+ply;
  int i;
  ss->pv[ply] = ss->move;
  for(i=ply+1; (ss+1)->pv[i] != NOMOVE; i++)
    ss->pv[i] = (ss+1)->pv[i];
  ss->pv[i] = NOMOVE;
}

//Vi låter denna va för tillfället
int extend(const position_t *pos, search_stack_t *ss, int mate_threat) {
  int extension = 0;
  move_t m = ss->move;
  extension = 0;
  /*if(mate_threat) extension += 45;
  if(PIECE(m)==PAWN && PawnRank[pos->xside][TO(m)] == RANK_7)
    extension += 45;
  if(extension > PLY) extension = PLY;
  */
  return extension;
}

static bool ok_to_reduce(const position_t *pos, const search_stack_t *ss,
             bool pvnode, int ply, move_t m) {
  //if(PROMOTION(m) || CAPTURE(m)) return false;
  //if(success_count(pos, m) > 2*failure_count(pos, m)) return false;
  return true;
}//ingen aning vad detta är (18-10-08)

