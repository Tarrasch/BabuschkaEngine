#include "engine.h"
#include "root.h"
Valuequad ValuesByIteration[MAX_DEPTH];

extern void init_move_stats(void);

void root_search(const position_t *pos, move_t searchmoves[]) {
  Valuequad value, lastvalue, bestvalue;
  int depth, ext, i, index;
  uint64 nodes;
  position_t p[1];
  search_stack_t sstack[MAX_DEPTH];
  move_stack_t mstack[MOVE_STACK_SIZE];
  move_t move;
  undo_info_t u[1];



  //tt_new_search();
  copy_position(p, pos);
  sstack[0].ms_ptr = mstack;
  /*
  for(i = 0; i < 4096; i++) History[0][i] = History[1][i] = 0;
  sstack[0].check = p->check;
  */

  (sstack+1)->ms_ptr = generate_moves(p, sstack->ms_ptr);

  if(searchmoves[0] == 0) { //ifall söka alla drag?
    move_stack_t *ms;
    for(ms = sstack[0].ms_ptr; ms < sstack[1].ms_ptr; ms++)
      add_root_move(p, sstack, ms->move, &(RSI->root_moves));
  }
  else if(false)
    for(i = 0; searchmoves[i] != 0; i++) //ifall givna drag, ha endast med de i stacken
      add_root_move(p, sstack, searchmoves[i], &(RSI->root_moves));

  do {//slutar då 'ABORTED'
    RSI->problem = 0;
    RSI->iteration++;
    if(RSI->iteration <= 60)//Slutar skriva ifall depth blir 60 typ
      printf("info depth %d\n", RSI->iteration);

    depth = RSI->iteration * PLY;
//    init_move_stats();
    init_node(p, sstack, 0);

    bestvalue = WORSTSCORE;
    sort_root_moves(&(RSI->root_moves), RSI->iteration);
/*
    if(RSI->iteration == 2)
      RSI->easymove = scan_for_easy_move(&(RSI->root_moves));
*/

    while((move = pick_root_move(&(RSI->root_moves)))) {
      if(RSI->iteration > 1 && RSI->iteration <= 60 &&
	 get_time()-RSI->start_time > 1000) {
	printf("info currmove "); print_move(move);
	printf("currmovenumber %d\n", RSI->root_moves.current);
      }

      nodes = RSI->nodes;

      sstack[0].move = move;
      make_move(p, move, u);
      ext = extend(p, sstack, 0); //detaljnivå. Att den adderar depth ifall det är intressant pos

      /*if(RSI->root_moves.current <= Options->multipv) {
	RSI->fail_high = 0;
	alpha = -MATE_VALUE - 1;
	if(RSI->root_moves.current == 1)
	  value = -search(p, sstack, 1, -beta, -alpha, depth-PLY+ext, 1, 1, 0);
	else
	  value = -search(p, sstack, 1, -beta, -alpha, depth-PLY+ext, 0, 1, 0);
	RSI->problem = (value <= lastvalue - 40);
      }
      else*/ {
	RSI->fail_high = 0;
	value = search(p, sstack, 1, depth-PLY+ext, 0);

      unmake_move(p, move, u);
      if(RSI->iteration >= 3) check_for_input(); //sänkte från 5 till 3
      remember_node_count(RSI->nodes-nodes, &(RSI->root_moves));

      if(RSI->thinking_status == ABORTED)
        break; //ifall det avbröts
      index = RSI->root_moves.current - 1;

      RSI->root_moves.moves[index].pv[0] = move;
      for(i = 1; sstack[1].pv[i] != NOMOVE; i++)
	RSI->root_moves.moves[index].pv[i] = sstack[1].pv[i];
      RSI->root_moves.moves[index].pv[i] = NOMOVE;


      /*
      if(value <= alpha) {
	RSI->root_moves.moves[index].score = -MATE_VALUE - 1;
	RSI->root_moves.moves[index].score_type = UPPER_BOUND;
	RSI->root_moves.moves[index].depth = RSI->iteration;
      }
      else*/ {
	RSI->root_moves.moves[index].score = value;
	RSI->root_moves.moves[index].score_type = EXACT;
	RSI->root_moves.moves[index].depth = RSI->iteration;

	if( PerspectiveScore(&value,selfplayerid) > PerspectiveScore(&bestvalue,selfplayerid) ) {
	  bestvalue = value;
	  update_pv(sstack, 0); update_root_pv(sstack[0].pv);
	  //if((*((float*)&bestvalue)+selfplayerid) > (*((float*)&value)+selfplayerid) - 20) RSI->problem = 0;
	  //?? vad e detta?
	}
	if((Options->multipv == 1))
	  print_pv(RSI->pv, RSI->iteration, value, 1);
/*	else {
	  qsort(RSI->root_moves.moves, RSI->root_moves.num,
		sizeof(root_move_t), key3);
	  //print_multipv();
	}*/
      }
    }
  }
    if(RSI->thinking_status == ABORTED) break;

    // Stop searching when there is only a single legal move:
    if(RSI->iteration >= 5 && RSI->root_moves.num == 1 &&
       !RSI->infinite)
      break;

    // Stop searching when the last two iterations returned a mate score:
    if(RSI->iteration >= 3 && RSI->mate_found >= 2 &&
       !RSI->infinite)
        break;

    if(RSI->pv[0] != RSI->easymove) RSI->easymove = 0;

    // Stop search early if one move seems much better than the rest:
    if(RSI->iteration >= 5 && RSI->easymove &&
       ((RSI->root_moves.moves[0].cumulative_nodes > (RSI->nodes*85)/100 &&
         get_time() - RSI->start_time > RSI->max_time/16) ||
        (RSI->root_moves.moves[0].cumulative_nodes > (RSI->nodes*99)/100 &&
         get_time()-RSI->start_time > RSI->max_time/32)) &&
       !RSI->infinite && RSI->thinking_status!=PONDERING) {
      break;
    }

    // Stop search if most of RSI->max_time is consumed at the end
    // of the iteration.  We probably don't have enough time to search the
    // first move at the next iteration anyway.
    if(get_time() - RSI->start_time > (RSI->max_time * 80) / 128 &&
       !RSI->infinite && RSI->thinking_status != PONDERING) {
      break;
    }
    lastvalue = bestvalue;
    if(RSI->iteration < MAX_DEPTH)
      ValuesByIteration[RSI->iteration] = bestvalue;
    if(RSI->iteration < MAX_DEPTH) RSI->bestvalues[RSI->iteration] = bestvalue;
    if(RSI->depth_limit && RSI->iteration >= RSI->depth_limit) break;
  } while(RSI->thinking_status != ABORTED);
}
