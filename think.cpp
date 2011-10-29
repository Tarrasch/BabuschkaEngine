#include "engine.h"

void think(const position_t *pos, bool inf, int time,
		  int depth_limit, int node_limit, int exact_time,
		  move_t moves[]) {
  
  init_root_search_info();
  
  // If a node, depth or time limit is given, but no time, use infinite time: 
  if(time == 0 && (depth_limit || node_limit || exact_time)) 
    inf = true;
  //Verkar skumt!!!  

   // Sudden death time control
  	 
      RSI->max_time = time / 40;
      RSI->absolute_max_time = time / 8;
   
  

  RSI->infinite = inf;
  RSI->depth_limit = depth_limit;
  RSI->node_limit = node_limit;
  RSI->exact_time = exact_time;
  RSI->thinking_status = THINKING;
  RSI->nodes_between_polls = RSI->node_limit ?
    min(RSI->node_limit, 20000) : 20000;//varför 20'000 ?

  root_search(pos, moves);
  if(EngineShouldQuit) quit();
}

