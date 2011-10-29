#if !defined(H_INCLUDED)
#define H_INCLUDED
//#define _MSC_VER
// MicroSoft Version ?

// Sökalgoritmer

//#define SEARCH_BRUTE
#define SEARCH_SHALLOW

// Evalueringsfunktioner

//#define EVAL_HEIGHT
#define EVAL_TWO

////
//// Includes
////

#undef CDECL
#ifdef _MSC_VER
#define llu_format  "%I64u"
#  include <sys/time.h>
extern int      gettimeofday(struct timeval * , struct timezone * );
#define CDECL __cdecl
#else
#define llu_format  "%llu"
#define CDECL
#endif


#ifndef _MSC_VER
#  include <unistd.h>
#  include <sys/time.h>
#  include <signal.h>
#  include <pthread.h>
#else
#  include <windows.h>
#  define inline __inline
//int             strncasecmp(const char *, const char *, size_t);
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>



////
//// Windows thread compatibility:
////

#  define mutex_init(x, y)
#  define mutex_lock(x)
#  define mutex_unlock(x)
#  define mutex_destroy(x)


////
//// More Windows compatibility magic.  Thanks to Dann Corbit!  :-)
////

#undef CDECL
#ifdef _MSC_VER
#define llu_format  "%I64u"
extern int      gettimeofdaybb(struct timeval * , struct timezone * );
#define CDECL __cdecl
#else
#define llu_format  "%llu"
#define CDECL
#endif


#define SIZES 3
#define DOLLS 24

#define EXPAND(x) ((x)+((x)&~7))
#define COMPRESS(x) (((x)+((x)&7))>>1) //har med historian att göra, för att få killer?

#define HINDEX(x) ((COMPRESS(FROM(x))<<6)|(COMPRESS(TO(x))))
#define HISTORY(x,y) (History[x][HINDEX(y)])

////
//// Constants and macros
////

#define ENGINE_NAME "Babuschka-Motor"
#define ENGINE_VERSION "0.1"

#define peice piece;

//babuposformat
#define Bfo(h,y,x) (h)+'0',(y)+'a',(x)+'1'

extern char startposition[24*3+1+1];

#define STARTPOS startposition

#define MAX_DEPTH 64
#define MAX_GAME_LENGTH 1024
#define MOVE_STACK_SIZE 2048

#define PLY 60


#define PieceSize(piece) (((piece) / 2) % SIZES +1)
//#define PieceSize(piece) (PeiceSize(piece))
#define PieceColor(piece) ((piece) / (2*SIZES))
#define PiecePlayer(piece) (PieceColor(piece))




#define PiecesHaveSameColor(piece1, piece2) ((PieceColor(piece1)) == (PieceColor(piece2)))
#define PiecesHaveOppositeColor(piece1, piece2) (PieceColor(piece1)) != (PieceColor(piece2))

#define NULLMOVE 0
#define NOMOVE 1
#define NOLEGALMOVE 2
#define FROM(x) (((x)>>7)&127)
#define TO(x) ((x)&127)

#define max(x,y) (((x)>(y))?(x):(y))
#define min(x,y) (((x)<(y))?(x):(y))

#define file(x) ((x)&15) //x
#define rank(x) ((x)>>4) //y

#define SQUARE(x,y) ((x)|((y)<<4)) //y

enum {LOWER_BOUND=1, UPPER_BOUND=2, EXACT=3};

enum {IDLE, THINKING, PONDERING, PONDERHIT, ABORTED};

extern const int MAXSUM;

/*
#define DISTANCE(x, y) (Distance[(x)-(y)])
*/

////
//// Types
////

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed long int32;
typedef unsigned long uint32;
typedef signed long long int64;
typedef unsigned long long uint64;

struct Valuequad{
	float v0,v1,v2,v3;
};
typedef float* Valuequad_p;

#define WORSTSCORE (Valuequad){0.0,0.0,0.0,0.0}

inline float PerspectiveScore(Valuequad* score, int player){
	return *(((float*)score)+player);
}

typedef int move_t;
typedef uint8 square_t; //xxx_yyy_
typedef uint8 doll_t;

struct list_t {
  uint8 p, n;
};

struct square_container_t{
	doll_t pieces[SIZES];
	uint8 n;
	};

struct position_t {
  square_container_t board_[256];//256 från början, why?
  square_container_t *board;
  square_t piece_pos[24];
  move_t last_move;

  int ply_finished[4];
  int winner[4];   //borde ju egentligen va 3or men för mycket if 'n' butts för att vara krashfritt
  int winners;

  int side;
  int gply;

  int moves_since_last_legal;

  //int psq[4];
};

struct move_stack_t {
  move_t move;
  Valuequad score;
};

struct undo_info_t {
  move_t last_move;
};

struct search_stack_t {
  move_stack_t *ms_ptr;
  move_t pv[MAX_DEPTH];
  move_t move;
  move_t killer, killer2;
};

struct engine_options_t {
  int aggressiveness, cowardice, pp_weight, e_pp_weight;
  int ps_weight, mob_weight, e_mob_weight, development_weight;
  int ponder, frc;
  int split_depth, threads;
  int multipv;
};

struct eval_params_t {
	double height_koefficient[3]; //psq= y*hk 0<=y<=7
	double height_even_bonus[3]; //psq+= hev	där y är jämnt (eg udda)
	double steps_from_center_bonus[3]; //psq=psq0 + psq0*(sfc*sfcb). 0<=sfc<=3
	double psq_predef_d[3][128];

	double mid_on_small_punishment;
	double large_on_small_punishment;
	double large_on_mid_punishment;
	double mosp_row[8];//nan = mosp
	double losp_row[8];
	double lomp_row[8];
};

#define MAX_ROOT_MOVES 512

struct root_move_t {
  move_t move;
  Valuequad depth_1_score, score;
  int score_type, depth;
  move_t pv[MAX_DEPTH];
  uint64 nodes, cumulative_nodes;
};

struct root_move_list_t {
  root_move_t moves[MAX_ROOT_MOVES];
  int num, current;
};

struct root_search_info_t {
  root_move_list_t root_moves;
  unsigned node_limit;
  int depth_limit;
  int exact_time;
  int max_time, absolute_max_time;
  int infinite;
  int thinking_status;
  int start_time;
  move_t pv[MAX_DEPTH];
  move_t bestmove, pondermove, easymove;
  int iteration;
  Valuequad bestvalues[MAX_DEPTH];
  int problem;
  int fail_high;
  int mate_found;
  uint64 nodes;
  int nodes_between_polls;
  int nodes_since_poll;
  move_t search_moves[MAX_ROOT_MOVES];
};


typedef void split_point_t;

const square_container_t EMPTY={{0,0,0},0};
const square_container_t FULL={{5,5,5},3};


struct thread_t {
  split_point_t *split_point;
  volatile uint64 nodes;
  volatile bool stop;
  volatile bool running;
  volatile bool idle;
  volatile bool work_is_waiting;
  volatile bool print_currline;
};


////
//// Global variables
////

extern int8 PSqTables[SIZES][128];
extern uint32 History[2][4096];
extern root_search_info_t RSI[1];
extern engine_options_t Options[1];
extern int selfplayerid; //spelarn man är 0..3
extern char params_as_string[];
extern eval_params_t eval_params[1];


extern bool cantmovetoarray[128];
extern bool dontmovetoarray[128];
extern const int ANTALOREKOMMENDERADERUTOR;
extern square_t OREKOMMENDERADERUTOR[];
extern const int ANTALOEXISTERANDERUTOR;
extern square_t OEXISTERANDERUTOR[];

extern Valuequad price_quad;

extern bool EngineShouldQuit;

extern int8 Wing[128];

////
//// Functions
////

//Tillagda
extern bool linearSearch( const square_t array[], square_t key, int sizeOfArray );
extern square_t unturn(square_t square,int side);
extern square_t turnback(square_t square,int side);
extern bool squareaheadof(square_t square,square_t square0, int side);
extern int TopSizeForSquares(const square_container_t *board, square_t square);
extern bool legaltojumpthrough(square_t square, int size, square_container_t *board);
extern bool legaltomoveto(square_t square, doll_t doll , position_t *pos);
extern void set_prices(char *fen);
extern bool has_finished(const position_t *pos,int side);

//OEgna
extern void init(void);

extern char *move2str(move_t move, char *str);

extern void print_modes(void);
extern void print_move(move_t move);
extern void print_pv(move_t pv[], int depth, Valuequad score, int num);
extern void print_multipv(void);
extern void print_currline(const search_stack_t *ss, int ply, int thread_id);
extern void set_position(position_t *pos, char *fen);
extern void quit(void);
extern move_t parse_move(const position_t *pos, const char movestr[]);
extern int parse_square(const char str[]);
extern void check_for_input(void);
extern int get_time(void);

extern int extend(const position_t *pos, search_stack_t *sst, int mate_threat);

extern void init_node(const position_t *pos, search_stack_t *sstack, int ply);
extern void update_pv(search_stack_t *sstack, int ply);

#if defined(SEARCH_BRUTE)
    extern Valuequad search(position_t *pos, search_stack_t *sstack, int ply, int depth,
             int follow_pv);
#endif

#if (defined(SEARCH_SHALLOW) or defined(SEARCH_SHALLOW))
    extern Valuequad search(position_t *pos, search_stack_t *sstack, int ply, int depth,
         int follow_pv, Valuequad bestall=WORSTSCORE);
#endif

extern void think(const position_t *pos, bool inf, int time,
		  int depth_limit, int node_limit, int exact_time,
		  move_t moves[]);

extern void init_root_search_info(void);
extern void root_search(const position_t *pos, move_t searchmoves[]);

extern move_stack_t *generate_moves(const position_t *pos, move_stack_t *ms);

extern bool move_is_legal(position_t *pos, move_t);
extern void make_move(position_t *pos, move_t m, undo_info_t *u);
extern void unmake_move(position_t *pos, move_t m, undo_info_t *u);
extern void make_nullmove(position_t *pos, undo_info_t *);
extern void unmake_nullmove(position_t *pos, undo_info_t *);

extern int compute_psq(const position_t *pos, int side);
extern Valuequad evaluate(const position_t *pos);
extern bool end_of_game(const position_t *pos);
extern Valuequad end_of_game_scores(const position_t *pos);

extern void order_moves(const position_t *pos, search_stack_t *ss,
			int ply, move_t pvmove);
extern void order_qmoves(const position_t *pos, search_stack_t *ss,
			 int ply, move_t pvmove);
extern move_t pick_move(move_stack_t **start, move_stack_t *end,
			bool look_for_best, int player);

extern void init_zobrist(void);

extern void init_mersenne(void);
extern uint32 genrand_int32(void);
extern uint64 genrand_int64(void);

extern void uci_main_loop(void);

extern void init_position(position_t *pos);
extern void init_piece_lists(position_t *pos);
extern void copy_position(position_t *dst, const position_t *src);

extern float get_current_maxsum(const position_t *pos);
extern float get_maxscore_still_avaible(const position_t *pos);
extern int prev_player(const position_t *pos);

#endif // !defined(SHOOTRANGE_H_INCLUDED)



