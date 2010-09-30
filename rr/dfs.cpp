#include "dfs.h"

int level;
int maxlevel;
int abort_time;
int last_time;
int best_move;

class Game{

public:
  tile* tile_target;
  tile** r_list;
  unsigned int goal_color;
  
  int cur_move;
  
  Game(tile** r_list_new, int color_new, tile* target_new) {
    unsigned int x,y,i;
    tile* t;
    r_list = r_list_new;
    goal_color = color_new;
    tile_target = target_new;
  }
	
  Game(const Game &cpy){
    r_list = (tile**) calloc(ROBOTS, sizeof(tile*));
    for(int i=0; i<ROBOTS; ++i) r_list[i] = cpy.r_list[i];
    goal_color = cpy.goal_color;
    tile_target = cpy.tile_target;
  }

  ~Game(){
    delete r_list;
  }

  void setTarget(tile* target) {
    tile_target = target;
  }
	
  int isOver(){
    for(int i=0; i<ROBOTS; ++i) {
      if((i == goal_color) &&
	 (r_list[i] == tile_target)) return 1;
    }
    return 0;
  }
	
  int winner(){
    if(isOver()) {
      return 1;
    }
    else {printf("wtf, check isOver before calling winner you moron\n"); return 0;}		
  }
	
  void reinitMove(){
    cur_move = 0;
  }
	
  int nextMove(){
    if ((maxlevel-level) > 2) {
      while ((cur_move<(ROBOTS*DIRS) &&
	      ((r_list[cur_move/DIRS]->tile_p[cur_move%DIRS]==null) ||
	       (!norobot(r_list[cur_move/DIRS]->tile_p[cur_move%DIRS]))))) {
	++cur_move;
      }
      
      if (!(cur_move < ROBOTS*DIRS)) {
	return -1;
      } else {
	//	printf("first cur_move[%d]\n", cur_move);
	return cur_move++;
      }
    } else {
      //      if(cur_move >= ROBOTS) return -1;
      if(! (cur_move < ROBOTS)) {
	return -1;
      } else {
	//	printf("second cur_move[%d]\n", cur_move);
	return goal_color*DIRS+cur_move++;
      }// this line might be hacked too bad to work...
    }
  }
	
  int norobot(tile *t){
    for(int i=0; i<ROBOTS; ++i){
      if(r_list[i] == t) return 0;
    }
    return 1;
  }
	
  void move(int square){//no checking of legality here
    int mvr = square/DIRS;
    int dir = square%DIRS;
    while ((r_list[mvr]->tile_p[dir]!=null) && 
	   (norobot(r_list[mvr]->tile_p[dir]))) {
      r_list[mvr] = r_list[mvr]->tile_p[dir];
      if((r_list[mvr]->flags & TILE_HAS_PRISM) && r_list[mvr]->prism_color!=mvr){
	dir=PRISM_TRANS[r_list[mvr]->prism_dir][dir];
      }
    }
  }
	
  int turn(){
    return 1;
  }
	
  int hueristic(){
    return 0;
  }
	
  void print(){
    //graphics go here!
    printf("print called\n");
    //		for(int i=0; i<ROBOTS; ++i) printf("
  }
};

//hashing features...
class LL{
public:
  tile** robs;
  LL* next;
  int level;
	
  LL(tile** source, int lev){
    robs = robots_new();
    for(int i=0; i<ROBOTS; ++i) robs[i] = source[i];
    next = null;
    level = lev;
  }
	
  ~LL()
  {
    if(next!=null) delete next;
    delete robs;
  }
	
  int has(tile** source, int lev){
    int ans = 1;
    for(int i=0; i<ROBOTS; ++i){
      if(robs[i] != source[i]) ans = 0;
    }
    if(level>lev){
      if(ans == 1) {
	level = lev;
	//				printf("triggered\n");
      }
      ans=0;
    }
    if(next!=null && ans==0) return next->has(source,lev);
    return ans;
  }

  void add(tile** source, int lev, int depth){
    if(next!=null) next->add(source,lev,depth);
    else next = new LL(source,lev);
  }
	
  void hackplus(){
    ++level;
    if(next!=null) next->hackplus();
  }	

};

class Hash{
public:
  LL* table[hash_size];

  Hash(){
    for(int i=0; i<hash_size; ++i) table[i] = null;
  }
	
  ~Hash(){
    for(int i=0; i<hash_size; ++i) if(table[i]!=null) delete table[i];
  }
	
  int hash(tile** source){

    unsigned long int ans = 1;
    for(int i=0; i<ROBOTS; ++i) {
      ans *= ( (unsigned long)(source[i]) % hash_size);
      ans = ans % hash_size;
    }
    return ans;

    /* 
       unsigned int ans, val;
       ans=0;
       for(int i=0; i<ROBOTS; ++i) {
       val = (unsigned int)(source[i]);
       val >>= (val >> CRAP_BITS) & SHIFT_MASK;
       ans ^= val;
       }
       ans = ans % hash_size;
       return ans;
    */
  }
	
  int has(Game* source, int level){
    if(table[hash(source->r_list)] == null){
      return 0;
    }
    else{
      return table[hash(source->r_list)]->has(source->r_list,level);
    }
  }
	
  void add(Game* source, int level){
    if(table[hash(source->r_list)] == null) {
      table[hash(source->r_list)]= new LL(source->r_list,level);
    }
    else{
      table[hash(source->r_list)]->add(source->r_list,level,0);
    }
  }
	
  void hackplus(){
    for(int i=0; i<hash_size; ++i) if(table[i]!=null) table[i]->hackplus();
  }
	
};

Hash* h;

int t_abort(){//abort if out of time, small script to automate
  return clock() > unsigned(abort_time+last_time);
}

int nodes;

int minmax(Game *g){//updated to use scope of minmax to track alpha and beta
  //  printf("minmax\n");
  int temp, ans = -1;//smaller(bigger) than any valid value
  if(g->isOver()) {
    //    printf("!!! was over\n");
    return g->winner();
  }

  if(h->has(g,level))
    {
      //		printf("level = %d, deadness HERE!\n",level);
      return 0; //the hashing check...
    }
  else {
    h->add(g,level);
    //		printf("got a new one\n");
  }

  ++nodes;
  ++level;
		
  if(level==maxlevel || t_abort()) {
    //    printf("!!! timeout\n");
    --level;
    return 0;
  }
  else{
    g->reinitMove();
    int i=g->nextMove();
    while(i!=-1 && ans<=0){ //loop though moves, -1 means no more
			
      Game* tg = new Game(*g);
      tg->move(i);
      //			printf("level(++) = %d, i = %d\n",level,i);
      temp = minmax(tg);
      delete tg;
			
      if(level==1) if(temp>ans) best_move=i;
      if(ans < temp) ans = temp;
      i = g->nextMove();
    }
  }
  --level;
  //  printf("!!! finished\n");
  return ans;
}

int d_time;
int abort_secs = 60;

tile** go_move(tile** r_list, u_int color, tile* target) {
  int best_known;
  int temp = 0;

  tile** r_list_copy = (tile**) calloc(ROBOTS, sizeof(tile*));
  memcpy(r_list_copy, r_list, ROBOTS * sizeof(tile*));

  print_robots(r_list_copy);

  //  abort_time=60*CLOCKS_PER_SEC; //9 sec, 1 sec of error
  abort_time=abort_secs*CLOCKS_PER_SEC; //9 sec, 1 sec of error

  Game *g = new Game(r_list_copy, color, target); //initial state for the game, yay.
  //	g->goal = 0; // hack initilize the goal...
	
  //  int player_move;

  //  while(!g->isOver()){
  if (g->isOver()) {
    printf("already there idiot!\n");
    return null;
  } else {
    if(h != null) delete h;

    h = new Hash(); // init the hash
    last_time = clock();
    best_known=0;

    for(maxlevel=1; (maxlevel < 100) && !t_abort() && !temp; ++maxlevel){
      level=0;
      h->hackplus();
      temp = minmax(g);
      if(!t_abort()) best_known=best_move;
      printf("maxlevel =%3d   eval =%5d, move =%4d, time = %6d ms, d_time = %6d ms, nodes=%8d\n",maxlevel, temp, best_move, (clock()-last_time)*1000/CLOCKS_PER_SEC, (clock()-d_time)*1000/CLOCKS_PER_SEC,nodes); 
      d_time = clock();
    }

    g->move(best_known);
    printf("MOVE %d\n",best_known);
    nodes=0;
    return g->r_list;
  }
}

//int temp,d_time;

/*
  int main(){
  int seed = time(null);
  //	seed = 1128759956;
  seed = 1130013682;
  srand (seed);
  printf("seed = %d\n",seed);
  for(int i=0; i<10; ++i) rand();

  abort_time=60*CLOCKS_PER_SEC; //9 sec, 1 sec of error
  int best_known;
  Game *g = new Game(); //initial state for the game, yay.
  //	g->goal = 0; // hack initilize the goal...
	
  int player_move;
  g->print();
  while(!g->isOver()){
  if(h!=null) delete h;
  h = new Hash(); // init the hash
  if(g->turn() == 1) {
  last_time=clock();
  best_known=0;
  temp = 0;
  for(maxlevel=1; maxlevel < 100 && !t_abort() && !temp; ++maxlevel){
  level=0;
  h->hackplus();
  temp = minmax(g);
  if(!t_abort()) best_known=best_move;
  printf("maxlevel =%3d   eval =%5d, move =%4d, time = %6d ms, d_time = %6d ms, nodes=%8d\n",maxlevel, temp, best_move, (clock()-last_time)*1000/CLOCKS_PER_SEC, (clock()-d_time)*1000/CLOCKS_PER_SEC,nodes); 
  d_time = clock();
  }
  g->print();
  g->move(best_known);
  printf("MOVE %d\n",best_known);
  g->print();
  printf("\n__________________\n\n");
  nodes=0;
  }
  else{
  scanf("%d",&player_move);
  g->print();
  g->move(player_move);
  g->print();
  }
  }
  }
*/
