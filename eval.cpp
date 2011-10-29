//#include "../../generella/engine.h"
#include "engine.h"
#include <algorithm>

const int MAXSUM = 1;

int f_sfc[8]={3,2,1,0,0,1,2,3};
int sub_to_mirror[8]={0,0,0,0,1,3,5,7};
//atm bara summman av avståndet man gått (y-led)
int compute_psq(const position_t *pos, const int side, const  eval_params_t* p) {
	double summa=0;
  for(int i=0+6*side;i<6*(side+1);i++){
	  square_t sq=unturn(pos->piece_pos[i],side);
	  sq=sq-sub_to_mirror[sq&15];
	  int size=PieceSize(i)-1;
	  if(false){
		  double val;
		  int y=rank(sq);
		  val=y*p->height_koefficient[size];
		  if((y%2)==1)
			  val+=p->height_even_bonus[size];
		  int sfc=f_sfc[file(sq)];
		  val+=sfc*p->steps_from_center_bonus[size];
		  //p->psq_predef_d[size][sq]=val;
	  }
	  summa+=p->psq_predef_d[size][sq];

  }
  return summa;
}


#define GRAIN_SIZE 4 // Must be a power of 2


Valuequad evaluate(const position_t *pos) {

  if(end_of_game(pos))
	  return end_of_game_scores(pos);

  eval_params_t* p=eval_params;

    double score[4]={1,1,1,1};
    double totsum=0;


    for(int cp=0;cp<4;cp++){//summera för alla spelare
    	if(has_finished(pos, cp)/*är i mål*/){
			goto end_of_ind_evaluate;
    	}

    	//Här börjar en kontroll för att nolla ifall man löper risk att ha fastnat
  		//0. Dockor på orekommenderade rutor
  		for (int doll = cp*6; doll < (cp*6+6); ++doll) {
  			if(dontmovetoarray[pos->piece_pos[doll]]){
  				score[cp]=1;
  				goto end_of_ind_evaluate;
  			}
  		}

  		//1. Dockor av samma färg på varandra vid de 4 rutorna angränsande målet
  		for(int angruta=SQUARE(2,6);angruta<SQUARE(6,6);angruta++){
  			int ruta= turnback(angruta,cp);
  			if(pos->board[ruta].n>1 && PieceColor(pos->board[ruta].pieces[0])==cp && PieceColor(pos->board[ruta].pieces[1])==cp){
  				score[cp]=1;
  				goto end_of_ind_evaluate;
  			}
  		}

  		//2. Ojämna flygeluppdelnigen
  		for(int s=0;s<3;s++){ //storlek s
  			int temp=Wing[unturn(pos->piece_pos[cp*6+s*2],cp)]+Wing[unturn(pos->piece_pos[cp*6+s*2 +1],cp)];
  			if(temp == 2 || temp==6){
  				score[cp]=1;
  				goto end_of_ind_evaluate;
  			}
  		}
  		//3. Long diagonal specialfallet
  		//denna är så komplicerad, så vi nöjer oss med att nolla long diagonal
  			//vänsterflygel
  		{
  			int lsq,msq,ssq, lsq2,msq2,ssq2;
  			lsq=turnback(SQUARE(3,6),cp);
  			msq=turnback(SQUARE(2,5),cp);
  			ssq=turnback(SQUARE(1,4),cp);
  			lsq2=turnback(SQUARE(4,6),cp);
  			msq2=turnback(SQUARE(5,5),cp);
  			ssq2=turnback(SQUARE(6,4),cp);
  			if((
  					(pos->piece_pos[cp*6+4]==lsq || pos->piece_pos[cp*6+5]==lsq)
  				&&	(pos->piece_pos[cp*6+2]==msq || pos->piece_pos[cp*6+3]==msq)
  				&&	(pos->piece_pos[cp*6+0]==ssq || pos->piece_pos[cp*6+1]==ssq))
  			||(
  					(pos->piece_pos[cp*6+4]==lsq2 || pos->piece_pos[cp*6+5]==lsq2)
  				&&	(pos->piece_pos[cp*6+2]==msq2 || pos->piece_pos[cp*6+3]==msq2)
  				&&	(pos->piece_pos[cp*6+0]==ssq2 || pos->piece_pos[cp*6+1]==ssq2) )
  			){
  				score[cp]=1;
  				goto end_of_ind_evaluate;
  			}
  		}
    	score[cp] += compute_psq(pos,cp,p);
    	int doll;

   	//Bestraffningar för att vara under en motståndardocka
    	for (doll = cp*6+0; doll < cp*6+2; ++doll) {//fallen då en liten docka är under andra dockor
    		if(pos->board[pos->piece_pos[doll]].n==3){
    			if(PiecesHaveOppositeColor(doll, pos->board[pos->piece_pos[doll]].pieces[2])){
    				int oprank=rank(unturn(pos->piece_pos[doll], PiecePlayer(pos->board[pos->piece_pos[doll]].pieces[2])));
    				score[cp]-=isnan(p->losp_row[oprank]) ? p->large_on_small_punishment : p->losp_row[oprank];
    			}//Annan Stor docka på en liten
    			if(PiecesHaveOppositeColor(doll, pos->board[pos->piece_pos[doll]].pieces[1])){
    				int oprank=rank(unturn(pos->piece_pos[doll], PiecePlayer(pos->board[pos->piece_pos[doll]].pieces[1])));
    	  			score[cp]-=isnan(p->mosp_row[oprank]) ? p->mid_on_small_punishment : p->mosp_row[oprank];
    	  		}//Mellan docka på en liten (när en stor med godtycklig färg redan finns på)
			}
    		else if(pos->board[pos->piece_pos[doll]].n==2){
    			int piece_on=pos->board[pos->piece_pos[doll]].pieces[1];
    			int oprank=rank(unturn(pos->piece_pos[doll], PiecePlayer(piece_on)));
    			if(PiecesHaveOppositeColor(doll, piece_on)){
    				if(PieceSize(piece_on)==3) //Annan Stor docka på en liten
	    				score[cp]-=isnan(p->losp_row[oprank]) ? p->large_on_small_punishment : p->losp_row[oprank];
    				else //Mellan docka på en liten (utan att annan står på)
    					score[cp]-=isnan(p->mosp_row[oprank]) ? p->mid_on_small_punishment : p->mosp_row[oprank];
    			}
    		}
    	}
    	for (doll = cp*6+2; doll < cp*6+4; ++doll) {//fallen då en mellan docka är under annan docka
    		if(TopSizeForSquares(pos->board, pos->piece_pos[doll])==3){
    			int oprank=rank(unturn(pos->piece_pos[doll], PiecePlayer(pos->board[pos->piece_pos[doll]].pieces[1])));
    			if(PiecesHaveOppositeColor(doll, pos->board[pos->piece_pos[doll]].pieces[pos->board[pos->piece_pos[doll]].n-1])){
    				score[cp]-=isnan(p->lomp_row[oprank]) ? p->large_on_mid_punishment : p->lomp_row[oprank];
    			}//Stor docka på en mellan
    		}
    	}

    	score[cp]+=50000;
      end_of_ind_evaluate:
    	totsum += score[cp];
    }
/*
    if(totsum<=0)
      printf("totsum<=0!!!!\n");
*/


    Valuequad result;
    Valuequad_p res_p=(float*)&result;

    	const float* at_pq= (float*)&price_quad;
    	int winners=pos->winners;
    	bool winner[4]={0,0,0,0};//denna är bool efter spelare, pos->winner är typ pristagarna i ordning

    	for (int i = 0; i < winners; ++i) {
    		res_p[pos->winner[i]]=at_pq[i];
    		winner[pos->winner[i]]=true;
    	}
    	double restsum=0.0;

    	//notera att totsum nu bara är summan av de som inte gått i mål
    	for (int i = winners; i < 4; ++i) {
    			restsum+=at_pq[i];
    		}

    	for (int i = 0; i < 4; ++i) {
    			if(!winner[i])
    				res_p[i]=restsum*(float)score[i]/totsum;;
    		}
/*
      bool tagen[4]={0,0,0,0};

      for(int i=0;i<4;i++){
        int tiniest=i;
        for(int j=0;j<4;j++)
            if(tagen[tiniest] || (score[tiniest]>score[j] && !tagen[j]))
                tiniest=j;
        tagen[tiniest]=true;
        res_p[tiniest]=((float*)(&price_quad))[4-1-i];
        }
*/
/*
    for(int i=0;i<4;i++){
  	  res_p[i]=(float)score[i]/totsum;
    }
*/
  return result;
}

