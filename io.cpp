#include "engine.h"



char *move2str(move_t move, char *str) {
  if((move == NULLMOVE) || (move == NOLEGALMOVE) ) sprintf(str, "0000");
  else if(move == NOMOVE) sprintf(str, "(none)");
  else sprintf(str, "%c%c%c%c",
               (char)file(FROM(move))+'a', (char)rank(FROM(move))+'1',
               (char)file(TO(move))+'a', (char)rank(TO(move))+'1');
  return str;
}

void print_modes(void){

}

void print_move(move_t move) {
  char str[10];
  printf("%s ", move2str(move, str));
}

void print_pv(move_t pv[], int depth, Valuequad score, int num) {
  int i, t;
  static Valuequad last_score;

  if(depth > 60) return;

  if(RSI->thinking_status == ABORTED)
    score = last_score;
  else
    last_score = score;

  t = get_time()-RSI->start_time;
//  if(abs(score) < MATE_VALUE - 300)
    printf("info multipv %d depth %d score %f %f %f %f time %d nodes " llu_format " " ,
           num, depth, score.v0,score.v1,score.v2,score.v3 , t, RSI->nodes);

  if(t > 0) printf("nps " llu_format " ",
		   (RSI->nodes*1000ULL)/((uint64) t));
  printf("pv ");

  for(i = 0; pv[i] != NOMOVE && pv[i] != NULLMOVE; i++) print_move(pv[i]);
  printf("\n");
}

void print_multipv(void) {
  int i, k;
  k = min(RSI->root_moves.num, Options->multipv);
  for(i = 0; i < k; i++)
    print_pv(RSI->root_moves.moves[i].pv,
             RSI->root_moves.moves[i].depth,
             RSI->root_moves.moves[i].score,
             i + 1);
}

void set_position(position_t *pos, char *fen) {
  init_position(pos);
  int i;
  bool won;
  for(i=0; i<DOLLS; i++) {
    if((i%6)==0)
        won =true;
    pos->piece_pos[i]=(*(fen+1)-'a') | ((*(fen+2)-'1') << 4);
    pos->board[pos->piece_pos[i]].pieces[*fen - '0']=i;
    pos->board[pos->piece_pos[i]].n=max(pos->board[pos->piece_pos[i]].n, *fen-'0'+1);
    fen+=3;

    if((i%6)==5){
    square_t square1=turnback(0x73,PiecePlayer(i));
    square_t square2=turnback(0x74,PiecePlayer(i));
    if((i%6)==5 &&
       pos->board[square1].n==3 && PiecesHaveSameColor(i,(pos->board[square1].pieces[1])) &&
       pos->board[square2].n==3 && PiecesHaveSameColor(i,(pos->board[square2].pieces[1]))){
        //en spelare har kommit i mål
    pos->winner[(pos->winners)++]=PiecePlayer(i);
    pos->ply_finished[PiecePlayer(i)]=0;
    }}
  }
//fen++;//pos->board[pos->piece_pos[23]].n
  pos->side=selfplayerid=*fen-'1';
return;

/*
  pos->psq[WHITE] = compute_psq(pos, WHITE);
  pos->psq[BLACK] = compute_psq(pos, BLACK);
  pos->gply = 0;
  */
}

void set_prices(char *fen) {
  Valuequad new_prices;
  Valuequad_p at_new_prices=(float*)&new_prices;
  int i;
  for(i=0; i<4; i++) {
    fen+=1;
    *(at_new_prices++)=strtod(fen,NULL);
    fen=strstr(fen, " ");
  }
  if( fen==NULL || *fen=='\0' || *(++fen)=='\0')
    price_quad=new_prices;
return;

/*
  pos->psq[WHITE] = compute_psq(pos, WHITE);
  pos->psq[BLACK] = compute_psq(pos, BLACK);
  pos->gply = 0;
  */
}



void quit(void) {
#if defined(SMP)
  stop_threads();
  mutex_destroy(SMPLock);
  mutex_destroy(IOLock);
  destroy_split_point_stack();
#endif
//  free_tt();
//  free_pawn_hash_table();
  exit(0);
}

//parse_move är ju mycket svårare i schack då den måste identifiera rockad-flaggor etc.
move_t parse_move(const position_t *pos, const char movestr[]) {

return (parse_square(movestr) << 7) | parse_square(movestr+2);

}

int parse_square(const char str[]) {
  if(str[0] >= 'a' && str[0] <= 'h' && str[1] >= '1' && str[1] <= '8')
    return str[0]-'a'+(str[1]-'1')*16;
  else return -1;
}

int get_time(void) {
  struct timeval t;
#ifdef _MSC_VER
  gettimeofdaybb(&t, NULL);
#else
  gettimeofday(&t, NULL);
#endif
  return t.tv_sec*1000 + t.tv_usec/1000;
}


int Bioskey(void);

void check_for_input(void)
{
  int             data;
  char            input[256];

  data = Bioskey();
  if (data) {
    if (feof(stdin))
      strcpy(input, "quit\n");
    if (strncasecmp(input, "quit", 4) == 0) {
      RSI->thinking_status = ABORTED;
      EngineShouldQuit = true;
    }
    else if (strncasecmp(input, "stop", 4) == 0)
      RSI->thinking_status = ABORTED;
  }
}

/*
  From Beowulf, from Olithink
*/
#ifndef _WIN32
/* Non-windows version */
int Bioskey(void)
{
  fd_set          readfds;
  struct timeval  timeout;

  FD_ZERO(&readfds);
  FD_SET(fileno(stdin), &readfds);
  /* Set to timeout immediately */
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  select(16, &readfds, 0, 0, &timeout);

  return (FD_ISSET(fileno(stdin), &readfds));
}

#else
/* Windows-version */
#include <windows.h>
#include <conio.h>
int Bioskey(void)
{
    static int      init = 0,
                    pipe;
    static HANDLE   inh;
    DWORD           dw;
    /* If we're running under XBoard then we can't use _kbhit() as the input
     * commands are sent to us directly over the internal pipe */

#if defined(FILE_CNT)
    if (stdin->_cnt > 0)
        return stdin->_cnt;
#endif
    if (!init) {
        init = 1;
        inh = GetStdHandle(STD_INPUT_HANDLE);
        pipe = !GetConsoleMode(inh, &dw);
        if (!pipe) {
            SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
            FlushConsoleInputBuffer(inh);
        }
    }
    if (pipe) {
        if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL))
            return 1;
        return dw;
    } else {
        GetNumberOfConsoleInputEvents(inh, &dw);
        return dw <= 1 ? 0 : dw;
    }
}
#endif
