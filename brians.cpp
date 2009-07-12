/******************************************************************************
Game of Brian's
Copyright 2006 Dirk Fortmeier
  
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
				   
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*******************************************************************************/

#include <string>
#include <iostream>
#include <gameoflife.h>

using namespace std;

#ifdef _WIN32 
#undef main 
#include <stdlib.h>
#endif

#define SIZEX 100	// number of cells vertical
#define SIZEY 100       // number of cells horizontal
#define CALCPER 40      // numner of randomly picked cells per calc-cycle
#define SHOWPER 10      // calc-cycles per frame


// the cell structure
struct Cell{
  bool alive;               	// is there a living cell?
  
  string BNA;               	// the Brian N A, a string in extended
				// brainfuck syntax

  signed char feld[20];     	// the 'turing tape'
  
  signed char pfeld;        	// the pointer to the head's position

  signed char ZUP;          	// the CellEnvironmentPointer, pointing 
  				// to one of the surrounding cells
  
  int x,y;                  	// the position of the cell
  unsigned char r,g,b;     	 // the color of the cell
  int ribopos;			// the position of the bna interpreter
  int resswert;  
};

// initialise the cell-array
Cell Feld[SIZEX][SIZEY];

// some SDL code to draw the cell-array
// a funtion returning a pointer to a cell
Cell* GetCell(int x,int y) {
  if (x<0) x+=SIZEX;
  if (y<0) y+=SIZEY;
  if (x>SIZEX-1) x-=SIZEX;
  if (y>SIZEY-1) y-=SIZEY;

  return &Feld[x][y];
}

// a function returning a pointer to a cell the 'mother'-cells ZUP pointing to
Cell* ZUPtoCell(Cell* Zelle){
  int x = Zelle->x;
  int y = Zelle->y;
  if     (Zelle->ZUP == 0) { x   ;  y--; }
  else if(Zelle->ZUP == 1) { x++ ;  y--; }
  else if(Zelle->ZUP == 2) { x++ ;  y; }
  else if(Zelle->ZUP == 3) { x++ ;  y++; }
  else if(Zelle->ZUP == 4) { x   ;  y++; }
  else if(Zelle->ZUP == 5) { x-- ;  y++; }
  else if(Zelle->ZUP == 6) { x-- ;  y; }
  else if(Zelle->ZUP == 7) { x-- ;  y--; }


  return GetCell(x,y);
}

// a function to generate extendend brainfuck commands
string GetToken() {
  string s;
  switch(rand()%10){
        case 0: s =  "<"; break;
        case 1: s =  ">"; break;
        case 2: s =  "+"; break;
        case 3: s =  "-"; break;
        case 4: s =  "{"; break;
        case 5: s =  "}"; break;
        case 6: s =  "["; break;
        case 7: s =  "]"; break;
        case 8: s =  "."; break;
        case 9: s =  ","; break;
  }
  return s;
}
 
// yet another helper funtion for randoms 
bool rnd(int pro){
  if (rand()%pro != 0) return true;
  else return false;
}

//mutation rate
int mutarate=5;
bool dodeathfunc = false;

// this function copies a cell to the location the ZUP points to
void CPCell(Cell* Zelle){
//  if(dodeathfunc && Zelle->resswert < 20 ) return;
  Cell *Celle = ZUPtoCell(Zelle);
  int x = Celle->x; int y = Celle->y;
  
  *Celle = *Zelle;
  Celle->x = x;
  Celle->y = y;
  Celle->ribopos=0;
  Celle->resswert=0;


  for(int n=0; n<20; n++)
         Celle->feld[n] = 0;
  
  //here, the BNA is randomly changed
  if(!rnd(mutarate)){  
  if(rand()%2) 
    Celle->BNA.insert(rand()%Celle->BNA.size(), GetToken());
  else
    if(Celle->BNA.size() >20) Celle->BNA.erase(rand()%Celle->BNA.size(), 1);
  Celle->r += (rand()%3 - 1) * 5;
  Celle->g += (rand()%3 - 1) * 5;
  Celle->b += (rand()%3 - 1) * 5;
  }
  
  if(dodeathfunc) Zelle->resswert-=10;
  //if(dodeathfunc && Zelle->resswert < 5	) Zelle->alive = false;
}

// the cells sensory ',' funtion, sets the value the pfeld is pointing to to
// zero if the ZUP points to a living cell
void CheckCell(Cell* Zelle){
  if(ZUPtoCell(Zelle)->alive) {
    Zelle->feld[Zelle->pfeld] = 0;
  }
}


// the brainfuck (extended) interpreter
// it interprets the following commands:
//  '>' increments the pfeld pointer
//  '<' decrements the pfeld pointer
//  '+' increments the tape's current value
//  '-' decrements the tape's current value
//  '{' increments the ZUP (extended syntax)
//  '}' decrements the ZUP (extended syntax)
//  '.' copies the cell to the ZUP (changed syntax)
//  ',' checks for living cell at the ZUP (changed syntax)
//  '[' loop construct
//  ']' loop construct, to prevent infinite loops it aborts randomly
//  


void BrainFuck(Cell* Zelle){
  if(Zelle->ribopos == Zelle->BNA.size()) Zelle->ribopos = 0;
  int pos = Zelle->ribopos;
	
  if(Zelle->BNA.at(pos) == '>') {
  Zelle->pfeld +=1;
    if(Zelle->pfeld > 19) Zelle->pfeld-=20;
  }
  else if(Zelle->BNA.at(pos) == '<') {
    Zelle->pfeld -=1;
    if(Zelle->pfeld < 0) Zelle->pfeld+=20;
  }
  else if(Zelle->BNA.at(pos) == '+') {
    Zelle->feld[Zelle->pfeld] +=1;
  }
  else if(Zelle->BNA.at(pos) == '-') {
    Zelle->feld[Zelle->pfeld] -=1;
  }
  else if(Zelle->BNA.at(pos) == '{') {
    Zelle->ZUP++;
    if(Zelle->ZUP > 7) Zelle->ZUP-=8;
  }
  else if(Zelle->BNA.at(pos) == '}') {
    Zelle->ZUP--;
    if(Zelle->ZUP < 0) Zelle->ZUP+=8;
  }
  else if(Zelle->BNA.at(pos) == '.') {
    CPCell(Zelle);
  }
  else if(Zelle->BNA.at(pos) == ',') {
    CheckCell(Zelle);
  }
    
  /*else if(Zelle->BNA.at(pos) == '[') {
    if(Zelle->feld[Zelle->pfeld] == 0 ) {
      while(pos < Zelle->BNA.size() && Zelle->BNA.at(pos) != ']' ) {
	pos++;
      }
    }
  }
  else if(Zelle->BNA.at(pos) == ']') {
    if(Zelle->feld[Zelle->pfeld] != 0 && rnd(4) ) {
      while(pos !=0 && Zelle->BNA.at(pos) != '[' ) {
        pos--;
      }
    pos--;
    }
  }*/
  
  pos++;
  Zelle->ribopos=pos;
}
// a nice funtion to generate BNA    
string MakeBNA() {
  string s="####";
//    for(int i=0; i<20; i++){
    while(rnd(10)){
      switch(rand()%10){
        case 0: s = s + "<"; break;
        case 1: s = s + ">"; break;
        case 2: s = s + "+"; break;
        case 3: s = s + "-"; break;
        case 4: s = s + "{"; break;
        case 5: s = s + "}"; break;
        case 6: s = s + "["; break;
        case 7: s = s + "]"; break;
        case 8: s = s + "."; break;
        case 9: s = s + ","; break;
    }
  }
 // cout<<s<<endl;
  return s;
}

// reset all values
void init(){
  srand(time(0));
 for(int x=0; x<SIZEX; x++) {
    for(int y=0; y<SIZEY; y++) {
      Feld[x][y].x = x;
      Feld[x][y].y = y;
      for(int n=0; n<20; n++)
	Feld[x][y].feld[n] = 0;
      
      Feld[x][y].pfeld = 0;
      Feld[x][y].r = rand()%200 + 50;
      Feld[x][y].g = rand()%200 + 50;
      Feld[x][y].b = rand()%200 + 50;
      Feld[x][y].ZUP = 0;
      Feld[x][y].alive = false; //!rnd(9);
      Feld[x][y].ribopos = 0;
      Feld[x][y].resswert = 11;
      Feld[x][y].BNA = MakeBNA();
    }
  }
  // experimental life generation functions ;)
  for(int x=0; x<20; x++) {
    for(int y=0; y<20; y++) {
      Feld[x][y].alive = rnd(2);
    }
  } 
  Feld[80][80].alive = true; //!rnd(9);
  Feld[81][81].alive = true; //!rnd(9);
  Feld[79][81].alive = true; //!rnd(9);
  Feld[80][82].alive = true; //!rnd(9);
  Feld[80][80].BNA = "######.###{##{###.#######";
  Feld[81][81].BNA = "###########";
  Feld[79][81].BNA = "###########";
  Feld[80][82].BNA = "###########";
 
}


bool draw(){
  bool Feld2[SIZEX][SIZEY];
  int showper = SHOWPER; 
    
  for(int k=0; k<50; k++){
    // a game of life like death funtion
    if(dodeathfunc) {
      for(int x=0; x<SIZEX; x++) {
        for(int y=0; y<SIZEY; y++) {
          int c = (int)Feld[x+1][y+1].alive +
                  (int)Feld[x+1][y-1].alive +
                  (int)Feld[x-1][y-1].alive +
                  (int)Feld[x ][y-1].alive +
                  (int)Feld[x ][y+1 ].alive +
                  (int)Feld[x-1][y+1].alive +
                  (int)Feld[x+1][y ].alive +
                  (int)Feld[x-1][y ].alive;
     	  
	  //if(c >= 2 && c <= 5) Feld2[x][y] = true;
	  //  else Feld2[x][y] = false;
	  c = 8 - c;
	  Feld2[x][y] = true;
	  if (c < 1) Feld2[x][y] = false;
	  Feld[x][y].resswert += c;
        }
      } 
      for(int x=0; x<SIZEX; x++)
        for(int y=0; y<SIZEY; y++) 
          if(!Feld2[x][y]) Feld[x][y].alive=false;
           
      
      // calc 'em
      for(int i=0; i<showper; i++) {
        int x=rand()%SIZEX; int y=rand()%SIZEY; 
        if(Feld[x][y].alive) {
          //for(int n=0; n<20; n++)
           // Feld[x][y].feld[n] = 0;
	  BrainFuck(&Feld[x][y]);
	}
      }
    }
   

  for(int y=0; y<SIZEY; y++)
    for(int x=0; x<SIZEX; x++){
      // Feld[x][y]=Feld2[x][y];
       SetCellColor(x,y,Feld[x][y].r,Feld[x][y].g,Feld[x][y].b);
    }				        }
}


int main (int argc, char** argv)
{
  // initialise a lot of uninteresting stuff 	
  init();
  
  InitGame(SIZEX,SIZEY, draw);    

}



