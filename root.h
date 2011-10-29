#ifndef ROOT_H_
#define ROOT_H_

#include "../generella/engine.h"

void add_root_move(position_t *pos, search_stack_t *sstack,
              move_t move, root_move_list_t *list);

void sort_root_moves(root_move_list_t *list, int iteration);
/*
*/
//Dragsortering ens ÄR MÖJLIGT så tas det hand mycket senare

move_t pick_root_move(root_move_list_t *list);

void remember_node_count(uint64 nodes, root_move_list_t *list);

void update_root_pv(const move_t pv[]);


#endif /*ROOT_H_*/
