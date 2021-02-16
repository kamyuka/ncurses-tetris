#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define CLEAR 40 //クリアのライン数

/* 移動物体構造体 */
typedef struct {
  double px, py;  //位置
  double vx, vy;  //速度
  double sx, sy;  //サイズ
} Mobj;  //Mobile Object (移動物体)

/*移動物体の初期化*/
void InitMobj(Mobj *obj,
	      double px, double py,
	      double vx, double vy,
	      double sx, double sy)
{
  obj->px = px; obj->py = py;
  obj->vx = vx; obj->vy = vy; 
  obj->sx = sx; obj->sy = sy;
}

/*物体の運動*/
void Mobe(Mobj *obj)
{
  obj->px += obj->vx;
  obj->py += obj->vy;
}


#define FIELD_H 23
#define FIELD_W 12
#define RWALL 11 //右壁
#define LWALL 13 //左壁
#define FLOOR 20 //床
#define R RWALL
#define L LWALL
#define F FLOOR

#define SCORE_OVER 9999 //ランキング上限スコア
#define S SCORE_OVER

int id = 0; 	//テトリミノのid
int ids[7] = {1,2,3,4,5,6,7};
int idcnt=7;	//7つのidをカウント
int rotate = 0; //回転判定
int quick = 0; 	//クイックドロップ判定
int erase_line;	//消したライン数
int timecnt = 0;//タイム
int game_clear = 0; //クリア判定
int clear_score[5] = {S,S,S,S,S}; //ランキング
int score_check = 1; //ランキングの作成判定

/*ゲームフィールド*/
int field[FIELD_H][FIELD_W] = {
  { 9,0,0,0,0,0,0,0,0,0,0,9 },
  { 9,0,0,0,0,0,0,0,0,0,0,9 },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { L,0,0,0,0,0,0,0,0,0,0,R },
  { F,F,F,F,F,F,F,F,F,F,F,F }
};


/*テトリミノの定義*/
int mino[8][4][4] = {
  {
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0}
  },
  {
    {0,0,0,0},
    {1,1,1,1},
    {0,0,0,0},
    {0,0,0,0}
  },
  {
    {0,0,0,0},
    {0,2,2,0},
    {0,2,2,0},
    {0,0,0,0}
  },
  {
    {0,0,0,0},
    {0,0,3,3},
    {0,3,3,0},
    {0,0,0,0}
  },
  {
    {0,0,0,0},
    {0,4,4,0},
    {0,0,4,4},
    {0,0,0,0}
  },
  {
    {0,0,0,0},
    {0,5,0,0},
    {0,5,5,5},
    {0,0,0,0}
  },
  {
    {0,0,0,0},
    {0,0,6,0},
    {6,6,6,0},
    {0,0,0,0}
  },
  {
    {0,0,0,0},
    {0,0,7,0},
    {0,7,7,7},
    {0,0,0,0}
  }
};

int now_mino[4][4];	//現在のテトリミノを格納


/*フィールドの表示*/
void DrawField(int h, int w)
{
  int i, j;
  int flx, fly;

  for(i = 0; i < FIELD_H; i++){
    for(j = 0; j < FIELD_W; j++){
      fly = i + 4;
      flx = j + w/2 - 6;
      if(field[i][j] == RWALL || field[i][j] == LWALL){
	attrset(COLOR_PAIR(9) | A_REVERSE | A_BOLD);
        mvaddch(fly, flx, '#');
      }
      if(field[i][j] == FLOOR){
	attrset(COLOR_PAIR(9) | A_REVERSE | A_BOLD);
        mvaddch(fly, flx, '#');
      }
      if(field[i][j] == 1){
	attrset(COLOR_PAIR(1) | A_REVERSE | A_BOLD);
	mvaddch(fly, flx, '*');
      }
      if(field[i][j] == 2){
	attrset(COLOR_PAIR(2) | A_REVERSE | A_BOLD);
	mvaddch(fly, flx, '*');
      }
      if(field[i][j] == 3){
	attrset(COLOR_PAIR(3) | A_REVERSE | A_BOLD);
	mvaddch(fly, flx, '*');
      }
      if(field[i][j] == 4){
	attrset(COLOR_PAIR(4) | A_REVERSE | A_BOLD);
	mvaddch(fly, flx, '*');
      }
      if(field[i][j] == 5){
	attrset(COLOR_PAIR(5) | A_REVERSE | A_BOLD);
	mvaddch(fly, flx, '*');
      }
      if(field[i][j] == 6){
	attrset(COLOR_PAIR(6) | A_REVERSE | A_BOLD);
	mvaddch(fly, flx, '*');
      }
      if(field[i][j] == 7){
	attrset(COLOR_PAIR(7) | A_REVERSE | A_BOLD);
	mvaddch(fly, flx, '*');
      }
      if(field[i][j] == 0 ){
	attrset(COLOR_PAIR(8));
	mvaddch(fly, flx, '#');
      }
    }
  }
}

/*idのシャッフル*/
void ShuffleId()
{

  int  i, j, tmp;
  
  for(i = 0; i < 7; i++){
    int j = rand() % 7;
    tmp = ids[i];
    ids[i] = ids[j];
    ids[j] = tmp;
  }
}

/*テトリミノの初期設定(位置,id)*/

void SetMino (int h, int w, Mobj *m)
{
  int i, j;

  if (idcnt > 6){    
    ShuffleId();
    idcnt = 0;
  }
  
  id = ids[idcnt];
  idcnt++;
    
  
  InitMobj(m, (double)w/2.0-2, (double)(5), 0.0, 0.0, 4.0, 0.5);

  for(i = 0; i < 4; i++){
    for(j = 0; j < 4; j++){
      now_mino[i][j] = mino[id][i][j];
    }
  }
  
}



/*テトリミノの表示*/
void DrawMino (Mobj *m)
{
  int p, i, j;

  attrset(COLOR_PAIR(id) | A_REVERSE |  A_BOLD);
  
  for(i = 0; i < 4; i++){
    for(j = 0; j < 4; j++){
	move((int)(m->py)+i, (int)(m->px)+j);
	if(now_mino[i][j]){
	  addch('*');
	}
    }
  }
}

/*削除判定*/
void EraseCheck()
{
  int i, j, k, l, check;

  for(i = 3; i < FIELD_H-1; i++){
    check = 0;
    for(j = 1; j < FIELD_W-1; j++){

      if(field[i][j] != 0)
	check += 1;      
    }

    if(check == 10){
      for(k = i; k > 3; k--){
	for(l = 1; l < FIELD_W-1; l++){
	  field[k][l] = field[k-1][l];
	}
      }
      erase_line++;
    }
    
  }

}

/*ブロックの固定*/
void FixMino(int h, int w, Mobj *m)
{
  int i, j; 
  
  for(i = 0; i < 4; i++){
    for(j = 0; j < 4; j++){
      field[(int)(m->py)+i-4-1][(int)(m->px)+j-w/2+6] += now_mino[i][j];
    }
  }

  EraseCheck();
}

/*接触判定*/
int Touch(int optx, Mobj *m)    //opt = -1:左, +1:右, 0:その場
{

  int h, w;   
  int i, j;
  int touch_field = 0;
  
  getmaxyx(stdscr, h, w);

  for(i = 0; i < 4; i++){
    for(j = 0; j < 4; j++){
      
      touch_field += now_mino[i][j] * field[(int)(m->py)+i-4][(int)(m->px)+j-w/2+6+optx];

    }
  }

  return(touch_field);

}

/*下方向の接触*/
void CheckHit(Mobj *m)
{
  int hit;
  int h, w;
  getmaxyx(stdscr, h, w);

  
  hit = Touch(0, m);

  if(hit != 0){
    FixMino(h, w, m);
    SetMino(h, w, m);
    return;
  }
  
}

/*テトリミノの回転*/
void RotateMino()
{
  int i, j;
  int tmp[4][4];


  if(rotate == 1){
    
    for(i = 0; i < 4; i ++){
      for(j = 0; j < 4; j++){
	tmp[3-j][i] = now_mino[i][j];
      }
    }
    for(i = 0; i < 4; i ++){
      for(j = 0; j < 4; j++){
	now_mino[i][j] = tmp[i][j];
      }
    }
    
  }
  
  if(rotate == -1){

    for(i = 0; i < 4; i ++){
      for(j = 0; j < 4; j++){
	tmp[j][3-i] = now_mino[i][j];
      }
    }
    for(i = 0; i < 4; i ++){
      for(j = 0; j < 4; j++){
	now_mino[i][j] = tmp[i][j];
      }
    }
  }
  
}

/*回転による接触*/
void OverMino(Mobj *m)
{
  int over;

  over = Touch(0, m);

  if(over != 0){
    if(over % RWALL == 0){
      m->px--;
    } else if(over % LWALL == 0){
      m->px++;
    } else {
      rotate = rotate * -1;
      RotateMino();
    }
  }
}
 
/* テトリミノの移動 */
void MoveMino(Mobj *m)
{
  int rfield=0, lfield=0, i;

  rfield = Touch(1, m);
  lfield = Touch(-1, m);
  
  // 移動
  if(rfield != 0 && m->vx > 0){
    m->py += m->vy;
  } else if(lfield != 0 && m->vx < 0){
    m->py += m->vy;
  } else{
    m->px += m->vx;
    m->py += m->vy;
  }
  
  // 回転
  if(rotate != 0){
    RotateMino();
    OverMino(m);
    rotate = 0;
  }

  //クイックドロップ
  if(quick == 1){
    while(Touch(0,m) == 0){
      m->py++;
      DrawMino(m);
    }
    quick = 0;
  }
  
}


/* テトリミノ等の操作 */
int Control(Mobj *m)
{
  int key;
  
  key = getch();
  m->vx = 0.0;  m->vy = 0.05;
  switch (key) {
  case KEY_UP : case 'a' : case 'k' : rotate =  1; break;
  case 's' :                    rotate = -1; break;
  case 'n' :		        quick  =  1; break;
  case KEY_DOWN : case 'j' :   	m->vy =  1.0; break;
  case KEY_LEFT : case 'h' :   	m->vx = -1.0; break;
  case KEY_RIGHT : case 'l' :   m->vx =  1.0; break;
  case 'q': case 'Q': case '\e': return ('q'); break;
  default: break;
  }
  return (key);
}


/*スコアの表示*/
void PrintScore(int h, int w)
{
  int y, x;

  attrset(COLOR_PAIR(10));
	  
  y = FIELD_H + 4;
  x = w/2 - 6;

  mvprintw(y, x, "Score: %d", erase_line);
  mvprintw(y+1, x, "Time : %d", timecnt/35);
}

/*クリア判定*/
int GameCheck()
{
  int i;
  
  if(erase_line >= CLEAR){
    score_check = 1;
    return 1;
  }
  
  for(i = 1; i < FIELD_W-1; i++){
    if(field[2][i] != 0){
      return -1;
    }
  } 
  
}

/*フィールドのリセット*/
void reset_field()
{
  int i, j;
  
  for(i = 1; i < FIELD_H-1; i++){
    for(j = 1; j < FIELD_W-1; j++){
      field[i][j] = 0;
    }
  }
  
}


/*スコアのランキングを作成*/
void RankingScore(int new){

  int i, j;
  
  for(i = 0; i < 5; i++){

    if(new < clear_score[i]){
      for(j = 4; j > i; j--){
	clear_score[j] = clear_score[j-1];
      }
      clear_score[i] = new;
      break;
    }
    
  }

}

/*タイトル画面*/
int Title()
{
  int  i, h, w, key;
  int clear_time;
  
  getmaxyx(stdscr, h, w);

  erase();

  if(game_clear == 1){
    attrset(COLOR_PAIR(3) | A_BOLD);
    clear_time = timecnt/35;
    mvprintw(h/2-5, w/2-5,"GAME CLEAR");
    mvprintw(h/2-4, w/2-7,"CLEAR TIME : %d", clear_time);
    if(score_check == 1){
      RankingScore(clear_time);
      score_check = -1;
    }
    attrset(COLOR_PAIR(10));
  }

  if(game_clear == -1){
    attrset(COLOR_PAIR(4) | A_BOLD);
    mvprintw(h/2-5, w/2-5,"GAME OVER!");
  }

  mvprintw(h/2-7, w/2-3,"TETRIS");
  mvprintw(h/2, w/2-8,"Press space key.");
  

  for(i = 0; i < 5;  i++){
    if(clear_score[i] < SCORE_OVER){
      if(clear_score[i] == clear_time){
	attrset(COLOR_PAIR(3) | A_BOLD);
	mvprintw(h/2+5+i, w/2+15,"No%d : Time %d", i+1, clear_score[i]);
	attrset(COLOR_PAIR(10));
      } else {
	mvprintw(h/2+5+i, w/2+15,"No%d : Time %d", i+1, clear_score[i]);
      }
    }
  }
  
  move(0, 0);

  timeout(-1);
  key = getch();

  return (key);
}

/*ゲーム*/
void Game()
{
  Mobj c, m;
  int w, h;
  int key;
  int i, j; 
  
  getmaxyx(stdscr, h, w);
  srand(time(NULL));
  
  SetMino(h, w,&m);
  timeout(0);

  erase_line = 0;
  timecnt = 0;
  game_clear = 0;
  
  reset_field();
  
  while (1) {
    // キャラクタの表示
    erase();
    DrawField(h, w);
    DrawMino(&m);
    PrintScore(h, w);
    move(0, 0);		// カーソルを隅へ移動
    refresh();
    
    // キー入力／キャラクタの移動
    if (Control(&m) == 'q') break;
    MoveMino(&m);
    
    // 動作速度調
    usleep(20000);
    timecnt++;
    
    //終了条件の判定
    game_clear = GameCheck();
    if(game_clear != 0){
      usleep(1000000);
      break;
    }
    
    // キャラクタ間の相互作用（衝突判定）
    CheckHit(&m);

  }
}


int main(void)
{
  int start;
  
  /* curses の設定 */
  initscr();
  curs_set(0);		// カーソルを表示しない
  noecho();		// 入力されたキーを表示しない
  cbreak();		// 入力バッファを使わない(Enter 不要の入力)
  keypad(stdscr, TRUE);	// カーソルキーを使用可能にする
  
  /* カラーの設定 */	
  start_color();
  
  init_pair(1, COLOR_CYAN, COLOR_BLACK);	// 水色の■
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);	// 黄色の■
  init_pair(3, COLOR_GREEN, COLOR_BLACK);    	// 緑色の■
  init_pair(4, COLOR_RED, COLOR_BLACK);        	// 赤色の■
  init_pair(5, COLOR_BLUE, COLOR_BLACK);	// 青色の■
  init_pair(6, COLOR_WHITE, COLOR_BLACK);      	// 白色の■
  init_pair(7, COLOR_MAGENTA, COLOR_BLACK);  	// 紫色の■
  init_pair(8, COLOR_BLACK, COLOR_BLACK);    	// 黒色の■
  init_pair(9, COLOR_BLACK, COLOR_WHITE);	// 黒字/白地
  
  init_pair(10, COLOR_WHITE, COLOR_BLACK);	// 白字／黒地
  bkgd(COLOR_PAIR(10));

  /* ゲーム本体 */
  while (1) {
    start = Title();
    if (start == 'q'){
      break;
    } else if(start == ' '){
      Game();
    }
  }
  
  
  /* 終了 */
  endwin();
  
  return 0;
  
}
