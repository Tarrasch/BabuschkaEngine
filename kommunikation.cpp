#include "engine.h"

static position_t RootPosition[1];

static void uci_start(void) {
  RSI->thinking_status = IDLE;
  set_position(RootPosition, STARTPOS);

  printf("id name %s %s\n", ENGINE_NAME, ENGINE_VERSION);

  printf("id author Arash Rouhani\n");
  //print_uci_options();
  print_modes();
  printf("uciok\n");
}

static void uci_set_position(char *str) {
  char *c=str, *m;
  char movestr[10];
  move_t move;
  undo_info_t u[1];

  while(isspace(*c)) c++;
  if(strncasecmp(c, "startpos", 8)==0) {
    c+=8; while(isspace(*c)) c++;
    set_position(RootPosition, STARTPOS);
  }
  else set_position(RootPosition, c);/*if(strncasecmp(c, "fen", 3)==0) {
    c+=3; while(isspace(*c)) c++;
    set_position(RootPosition, (unsigned char*)c);
    while(*c != '\0' && strncasecmp(c, "moves", 5) != 0) c++;
  }*/
  while(isspace(*c)) c++;
  if(strncasecmp(c, "moves", 5)==0) {
    c+=5; while(isspace(*c)) c++;
    while(*c!='\0') {
      m=movestr;
      while(*c!='\0' && !isspace(*c)) *m++ = *c++;
      *m = '\0';
      move = parse_move(RootPosition, movestr);
      if(move) make_move(RootPosition, move, u);
      else printf("Illegal move: %s\n", movestr);
      while(isspace(*c)) c++;
    }
  }
}

static void uci_parse_searchmoves(char *str, move_t moves[]) {
  char *c, movestr[10];
  int i;
  move_t *move = moves;

  c = str;
  while(isspace(*c)) c++;
  while(*c != '\0') {
    i = 0;
    while(*c != '\0' && !isspace(*c) && i < 9) movestr[i++] = *c++;
    if(i >= 4 && 'a' <= movestr[0] && movestr[0] <= 'h' &&
       '1' <= movestr[1] && movestr[1] <= '8' &&
       'a' <= movestr[2] && movestr[2] <= 'h' &&
       '1' <= movestr[3] && movestr[3] <= '8')
      *move++ = parse_move(RootPosition, movestr);
    else break;
    while(isspace(*c)) c++;
  }
  *move = 0;
}


static void uci_go(char *options) {
  char *ponder, *infinite;
  char *c;
  int depth=0, nodes=0, movetime=0;
  move_t moves[MAX_ROOT_MOVES];

  int time=0;

  moves[0] = 0;

  infinite = strstr(options, "infinite");
  ponder = strstr(options, "ponder");

  c = strstr(options, "time");
  if(c != NULL) sscanf(c+5, "%d", &time);

  /*
  c = strstr(options, "btime");
  if(c != NULL) sscanf(c+6, "%d", &btime);
  c = strstr(options, "winc");
  if(c != NULL) sscanf(c + 5, "%d", &winc);
  c = strstr(options, "binc");
  if(c != NULL) sscanf(c + 5, "%d", &binc);
  c = strstr(options, "movestogo");
  if(c != NULL) sscanf(c + 10, "%d", &movestogo);
  */

  c = strstr(options, "depth");
  if(c != NULL) sscanf(c + 6, "%d", &depth);
  c = strstr(options, "nodes");
  if(c != NULL) sscanf(c + 6, "%d", &nodes);
  /*
  c = strstr(options, "mate");
  if(c != NULL) sscanf(c + 5, "%d", &mate);
  c = strstr(options, "movetime");
  if(c != NULL) sscanf(c + 9, "%d", &movetime);
  c = strstr(options, "searchmoves");
  */
  if(c != NULL) uci_parse_searchmoves(c + 12, moves);

  think(RootPosition, infinite != NULL, time, depth, nodes, movetime, moves);
  printf("bestmove "); print_move(RSI->bestmove);

  printf("\n");
  RSI->thinking_status = IDLE;
}
/*
static void uci_setoption(char *string) {
  char option_name[128], option_value[128], *c;

  strncpy(option_name, string, 128);
  c = strstr(option_name, "value");
  if(c == NULL) // Assume that this is is an UCI_BUTTON option
    push_button(option_name);
  else {
    int i = (int)(c - option_name - 1);
    option_name[i] = '\0';
    strncpy(option_value, c + 6, 128);
    set_option_value(option_name, option_value);
  }
}
*/


static void uci_handle_command(char *command) {
  if(strncasecmp(command, "ucinewgame", 10) == 0) return;
  else if(strncasecmp(command, "info", 4) == 0) uci_start();
  else if(strncasecmp(command, "isready", 7) == 0) printf("readyok\n");
  else if(strncasecmp(command, "position", 8) == 0) uci_set_position(command + 9);
  else if(strncasecmp(command, "quit", 4) == 0) quit();
  else if(strncasecmp(command, "go", 2) == 0) uci_go(command + 3);
  else if(strncasecmp(command, "prices", 6) == 0) set_prices(command + 7);
//  else if(strncasecmp(command, "setoption name", 14) == 0) uci_setoption(command + 15);
}

static char Command[4096];

static void uci_wait_for_command(void) {
  if(fgets(Command, 4096, stdin) == NULL) strcpy(Command, "quit\n");
  uci_handle_command(Command);
}

void uci_main_loop(void) {
  while(1) uci_wait_for_command();
}
