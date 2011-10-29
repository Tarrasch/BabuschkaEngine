#include "engine.h"

//Dragflaggor:
//xxx_yyy_xxx_yyy

bool linearSearch( const square_t array[], square_t key, int sizeOfArray );





bool linearSearch( const square_t array[], square_t key, int sizeOfArray )
{
   for ( int j = 0; j < sizeOfArray; j++ ){
      if ( array[ j ] == key ){
         return true;
      }
   }
   return false;
}

void addmovesjumps(square_t startsquare, square_t movelist[], int& n,
		square_container_t *board, int size){

	square_t list[16];
	int now=0;
	int next=1;
	list[0]=startsquare;

	bool tagen[4][4];
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			tagen[i][j]=false;
		}
	}

	while(now<next){
		square_t atsquare=list[now];
		int x=file(atsquare)/2;
		int y=rank(atsquare)/2;
		for(int xf=-1; xf<2;xf++){
			for(int yf=-16; yf<17;yf+=16){
				if(!tagen[x+xf][y+yf/16] && ((atsquare+2*xf+2*yf)>=0) && ((atsquare+2*xf+2*yf)<128) && ((((7&atsquare)+2*xf)&16)==0) ){
					square_t tempsquare= atsquare+2*xf+2*yf;
					square_t tempsquare2= atsquare+xf+yf;
					if(board[tempsquare2].n>0 && legaltojumpthrough(tempsquare,size,board)){
						list[next++]=tempsquare;
						tagen[x+xf][y+yf/16]=true;
					}
				}
			}
		}
		now++;
	}

	for (int i = 0; i < now; ++i) {
		movelist[n++]=list[i];
	}

//kan optimeras mycket, med bredden-först liknande (ingen rekursioon) och 4*4 boolarray
/*
if (linearSearch(movelist,atsquare,n))
	return;

if(atsquare!=startsquare)
movelist[n++]=atsquare;

for(int xf=-1; xf<2;xf++)
	for(int yf=-16; yf<17;yf+=16){

		if((xf!=0 || yf!=0) && ((atsquare+2*xf+2*yf)>=0) && ((atsquare+2*xf+2*yf)<128) && ((((7&atsquare)+2*xf)&16)==0) ){


         square_t tempsquare= atsquare+2*xf+2*yf;
         square_t tempsquare2= atsquare+xf+yf;


        if(board[tempsquare2].n>0 && legaltojumpthrough(tempsquare,size,board))
          addmovesjumps(tempsquare,startsquare, movelist, n, board, size); //Ifall det finns en pjäs mellan

        }
	}
	*/
}

move_stack_t *generate_moves(const position_t *pos, move_stack_t *ms) {
  square_t from;
  int piece;
  int side = pos->side;
  move_stack_t* startms=ms;

//#define TopSizeForSquare(square) ((pos->board[square].n>0) && PeiceSize(pos->board[square].peices[pos->board[square].n-1]))


  for(piece = side*6; piece < (side+1)*6; piece++) {
	from = pos->piece_pos[piece];
	int size = PieceSize(piece);

    if(size==TopSizeForSquares(pos->board,from)){ //Endast ifall dockan är överst

	square_t movelist[20]; //för varje enskild pjäs, lägger in alla drag
	int moves=0;

	addmovesjumps(pos->piece_pos[piece], movelist, moves, pos->board, size);

    int turnedsquare=unturn(from,side);

    if(rank(turnedsquare)<7){
	movelist[moves++]=turnback(turnedsquare-1+16,side);
	movelist[moves++]=turnback(turnedsquare+16,side);
	movelist[moves++]=turnback(turnedsquare+1+16,side);
    }//annars kommer turnback bli nått cp, när de är över förväntad gräns
	while(moves--){
        if(legaltomoveto(movelist[moves],piece,(position_t*)pos))
    		(ms++)->move =(from<<7) | movelist[moves];
        }

    }
	}
  /*
    if(startms==ms){ //Ifall inga drag e möjliga får man ibland göra side-moves
        bool allow_side_moves = true;
        square_t morhph_l= unturn(0x30,side),morhph_r= unturn(0x40,side);
   }
   */
  //beslutat att det är GG ifall man fastnat


    if(startms==ms)
        (ms++)->move = NOLEGALMOVE; //nullmove ifall INGA drag var möjliga för någon pjäs

  return ms; //sista steget
}
