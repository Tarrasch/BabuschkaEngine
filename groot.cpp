#include "../generella/engine.h"
  
void add_root_move(position_t *pos, search_stack_t *sstack,
              move_t move, root_move_list_t *list) {
  undo_info_t u[1];
  if(move_is_legal(pos, move)) {
    list->moves[list->num].move = move;
    list->moves[list->num].nodes = 0;
    list->moves[list->num].cumulative_nodes = 0;
    sstack[0].move = move;
    make_move(pos, move, u);
//  list->moves[list->num].depth_1_score = search(pos, sstack, 1, 0, 0);
    list->moves[list->num].score = WORSTSCORE;
    
    unmake_move(pos, move, u);
    list->num++;
  }
}
/*
move_t scan_for_easy_move(root_move_list_t *list) {
  Valuequad best_move_score = list->moves[0].depth_1_score;
  int i;
  for(i = 1; i < list->num; i++)
    if(list->moves[i].depth_1_score >= best_move_score - P_VALUE) return 0;
  return list->moves[0].move;
}
*/

/*//Stöd för qsort och sånt
 * 
int CDECL key1(const void *a, const void *b) {
  if(((root_move_t*)a)->move == RSI->pv[0]) return -1;
  else if(((root_move_t*)b)->move == RSI->pv[0]) return 1;
  else if(((root_move_t*)a)->depth_1_score > ((root_move_t*)b)->depth_1_score)
    return -1; 
  else return 1;
}

int CDECL key2(const void *a, const void *b) {
  if(((root_move_t*)a)->move == RSI->pv[0]) return -1;
  else if(((root_move_t*)b)->move == RSI->pv[0]) return 1;
  else if(((root_move_t*)a)->nodes > ((root_move_t*)b)->nodes)
    return -1; 
  else return 1;
}

int CDECL key3(const void *a, const void *b) {
  if(((root_move_t*)a)->depth > ((root_move_t*)b)->depth &&
     ((root_move_t*)a)->score_type == EXACT) return -1;
  else if(((root_move_t*)b)->depth > ((root_move_t*)a)->depth &&
          ((root_move_t*)b)->score_type == EXACT) return 1;
  else if(((root_move_t*)a)->score > ((root_move_t*)b)->score) return -1;
  else if(((root_move_t*)b)->score > ((root_move_t*)a)->score) return 1;
  else if(((root_move_t*)a)->nodes > ((root_move_t*)b)->nodes) return -1; 
  else return 1;
}
*/
void sort_root_moves(root_move_list_t *list, int iteration) {
  /*if(iteration <= 2) 
    qsort(list->moves, list->num, sizeof(root_move_t), key1);
  else if(Options->multipv == 1 || list->num == 1)
    qsort(list->moves, list->num, sizeof(root_move_t), key2);
  else 
    qsort(list->moves, list->num, sizeof(root_move_t), key3);
    */
  list->current = 0;
  RSI->pv[0] = list->moves[0].move; RSI->pv[1] = NOMOVE;
}
/*
*/
//Dragsortering ens ÄR MÖJLIGT så tas det hand mycket senare

move_t pick_root_move(root_move_list_t *list) {
  if(list->current == list->num) return 0;
  return list->moves[list->current++].move;
}

void remember_node_count(uint64 nodes, root_move_list_t *list) {
  list->moves[list->current - 1].nodes = nodes;  
  list->moves[list->current - 1].cumulative_nodes += nodes;
}

void update_root_pv(const move_t pv[]) {
  int i;
  for(i=0; pv[i] != NOMOVE; i++) RSI->pv[i] = pv[i];
  RSI->pv[i] = NOMOVE;
  RSI->bestmove = RSI->pv[0];
}

void init_root_search_info(void) {
  RSI->start_time = get_time();
  RSI->iteration = 1;
  RSI->easymove = 0;
  RSI->bestmove = 0;
  RSI->pondermove = 0;
  RSI->problem = 0;
  RSI->mate_found = 0;
  RSI->nodes = 0ULL;
  RSI->nodes_since_poll = 0;
  RSI->pv[0] = NOMOVE; RSI->pv[1] = NOMOVE;
  RSI->root_moves.num = 0;
}
