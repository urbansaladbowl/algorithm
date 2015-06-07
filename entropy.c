#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define BYTE 256
int a[BYTE];  /* 文字数をカウントする配列 */
double P[BYTE]; /*各文字の出現頻度*/
double I[BYTE]; /*各文字の情報量*/
double H = 0; /*エントロピ*/
double MH; /*最大エントロ*/

int b = 0; /*文章の文字数*/

  /* ファイル中の英文字の出現頻度を調べる */
void Count(FILE *fp)
{
  int i, c;

    /* ファイルの終わりに達するまで１文字読み込む */
  while ((c = getc(fp)) != EOF){
    /* c がアルファベットの小文字なら */
    if (c >= 'A' && c <= 'Z'){
      a[c]++;        /* カウントする */
      b++;
    }else if(c >= 'a' && c <= 'z'){ /*大文字なら*/
      a[c]++; /*カウント*/
      b++;
    }else if(c != ' '){ /*================空白を文字とカウントしない*/
      a[0]++; /*文字以外の記号をカウント*/
      b++; /*記号も全部の文字数にいれる*/
    }
  }

  /*小文字と大文字を区別しない*/
  /*
  for(i=0; i<26; i++){
    a['a' + i] += a['A' + i];
  }
  */

}


void CalcFrequency(void)
{
  int c;

  for (c = 'a'; c <= 'z'; c++)    /* 文字の出現頻度を */
    P[c] = (double)a[c] / b; 

  P[0] = (double)a[0] / b; /*記号の出現頻度の計算*/
}

void CalcInfo(void)
{
  int c;

  for (c = 'a'; c <= 'z'; c++){  /* 文字の情報量を */
    if (P[c] == 0.0)
      I[c] = 0.0;
    else  
    I[c] = -log2(P[c]);
  }

  if (P[0] == 0)
    I[0] = 0.0;
  else
    I[0] = -log2(P[0]);
}

void CalcEntropy(void)
{
  int c;

  for (c = 'a'; c <= 'z'; c++){ /* 文字の出現頻度を */
    if (I[c] == 0.0)
      H += 0.0;
    else   
      H += -P[c]*log2(P[c]);
  }
}

void CalcMaxEntropy(void)
{
  MH = log2(b);
}






void PrintResult(void)
{
  int c;

  for (c = 'a'; c <= 'z'; c++)       /* 小文字の出現回数を */
      printf("%c:%d\t",  c, a[c]);    /* 表示 */
  printf("\n\n");
  printf("mark : %d\t", a[0]); /*記号の出現回数*/
  printf("\n\n");

  printf("各文字の出現頻度\n");
  for (c = 'a'; c <= 'z'; c++)       /* 小文字の出現頻度を */
      printf("%c:%f\t",  c, P[c]);    /* 表示 */
  printf("\n\n");
  printf("mark : %f\t", P[0]); /*記号の出現頻度*/
  printf("\n\n");

  printf("各文字の情報量\n");
  for (c = 'a'; c <= 'z'; c++)       /* 小文字の情報量を */
      printf("%c:%f bit\t",  c, I[c]);    /* 表示 */
  printf("\n\n");
  printf("mark : %f\t", I[0]); /*記号の情報量*/
  printf("\n\n");

  printf("この文章のエントロピ\n");
  printf("%f\n",H);

  printf("この文章の最大エントロピ\n%f\n", MH);
}

int main(void)
{
  FILE *fp;
  FILE *tmpfp;
  char sIn[128]; //入力ファイル名
  char *fileIn;
  int f; //ループ制御変数

  //変換元ファイルの入力
  do{
    printf("変換元ファイル:");
    scanf("%s", sIn);
    
    //指定ファイルが存在するかの確認のため一時的に保存
    tmpfp = fopen(sIn, "r");
    if(tmpfp == NULL){
      printf("%sファイルが開けません\n", sIn);
      f = 0;
    }else{
      fileIn = sIn;
      f = 1;
    }
  }while(f == 0);

  /*入力ファイルと出力ファイルが確定したのでファイルを開く*/
  fp = fopen(fileIn, "r");

  /*ファイルの存在が確定しているが、一応開けるか確認*/
  if(fp == NULL){
    printf("%sファイルが開けません\n", fileIn);
    return (-1);
  }

  printf("%s ファイルにおけるアルファベットの出現頻度\n",fileIn);
  Count(fp) ;        /* 文字の出現頻度を調べて */
  CalcFrequency();
  CalcInfo();
  CalcEntropy();
  CalcMaxEntropy();
  printf("文字数 : %d\n", b);
  PrintResult(  );       /* 表示する */
   
  fclose(fp);            /* ファイルを閉じる */
 }
