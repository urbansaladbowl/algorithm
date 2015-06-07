#include <stdio.h>
#include <stdlib.h>
#define BUFFER_SIZE 65536
//#define NULL 0
//huffman木のノードの構造体
typedef struct node
{
	char ch;//文字コード
	struct node *parent;//親ノードへのポインタ
	struct node *lnode;//左の枝ノードへのポインタ
	struct node *rnode;//右の枝ノードへのポインタ
	int freq;//出現頻度	
	int code;//ノードが親の左の枝ノードなら0, 右の枝ノードなら1
} Node;

void initialize();
void readfile();
void makehtree();
void getmin2(int n, int *d1, int *d2);
void renewtable(int i,int j);
void codeout();
void encode();
void encode_out();
void decode();
void decode_out();
void bitwrite(int bit);
void bitflush();
int bitread(int byte, int bit);


int table[256];//昇順テーブル
unsigned char data[BUFFER_SIZE];//読み込んだデータ
unsigned char enc_data[BUFFER_SIZE];//符号化したデータ
unsigned char dec_data[BUFFER_SIZE];//復号化したデータ
int top;//頂点データ
Node htnode[256+255];//Huffman木のノード(0~255が葉)
int filelength;//ファイルの長さ
char file[100];//ファイルパス
int enc_filelength;		//復号化したファイルの長さ
int bitpos;


int main()
{
	initialize();//変数の初期化
	readfile();//ファイル読み込み、出現頻度を求める
	makehtree();//Huffman木の構成
	codeout();//復号を出力
	encode();		//符号化
	encode_out();	//符号化したものを出力
	decode();		//復号化
	decode_out();	//復号化したものを出力
	return 0;
}

//変数の初期化
void initialize()
{
	int i;
	for(i=0;i<256;i++)
	{
		htnode[i].ch=i;
	}
	for(i=0;i<256*2-1;i++)
	{
		htnode[i].parent=NULL;
		htnode[i].lnode=NULL;
		htnode[i].rnode=NULL;
		htnode[i].freq=0;
		htnode[i].code=-1;
	}
	filelength=0;
}

//ファイルを読み込み、出現頻度を求める
void readfile()
{
	FILE *fp;
	char c;
	int i,j,tmp;
	printf("file=");
	scanf("%s",file);
	fp=fopen(file,"r");
	while(filelength<BUFFER_SIZE && (c=fgetc(fp))!=EOF)
	{
		data[filelength++]=c;
		htnode[(int)c].freq++;
	}
	table[0]=0;
	for(i=1;i<256;i++)
	{
		table[i]=i;
		j=i;
		while(htnode[table[j-1]].freq>htnode[table[j]].freq && j!=0)
		{
			tmp=table[j];
			table[j]=table[j-1];
			table[j-1]=tmp;
			j--;
		}
	}

}

//Huffman木の構成
void makehtree()
{
	int i;
	int j;
	int d1,d2;
	j=255;
	for(i=0;i<255;i++)
	{
		if(htnode[table[i]].freq==0) //頻度が0の物を除外
			continue;

		j++;
		getmin2(i,&d1,&d2);//頻度が少ないノードを2つ選ぶ

		//htnode[d1],htnode[d2]: 頻度の少ないノード
		//htnode[j]: htnode[d1]とhtnode[d2]の親ノード
		//                       (htnode[d1]を左ノード, htnode[d2]を右ノードとする)

		// 符号を割り当てる
		htnode[d1].code = 0;
		htnode[d2].code = 1;

		// 親ノードへのポインタ
		htnode[d1].parent = &htnode[j];
		htnode[d2].parent = &htnode[j];

		// 親ノードへの出現頻度を設定する
		htnode[j].freq =  htnode[d1].freq + htnode[d2].freq;

		// 左ノード, 右ノードへのポインタを設定する 
		htnode[j].lnode =  &htnode[d1];
		htnode[j].rnode =  &htnode[d2];

		renewtable(i,j);//昇順テーブルの更新
	}
	top=j;
	printf("%c\n", htnode[top].ch);
}

//頻度が少ないノードを2つ選ぶ
void getmin2(int n, int *d1, int *d2)
{
	*d1=table[n];
	*d2=table[n+1];
}

//昇順テーブルの更新
void renewtable(int i, int j)
{
	int s,tmp;
	table[i+1]=j;
	s=i+1;
	while(htnode[table[s]].freq>htnode[table[s+1]].freq && s!=255)
	{
		tmp=table[s];
		table[s]=table[s+1];
		table[s+1]=tmp;
		s++;
	}
}

//符号を出力
void codeout()
{
	int i,j;
	int sum;
	Node *n;
	int c[1000];
	sum=0;
	for(i=0;i<256;i++)
	{
		if(htnode[i].parent==NULL)
			continue;
		n=&htnode[i];
		j=0;
		while(n->parent!=NULL)
		{
			c[j]=n->code;
			n=n->parent;
			j++;
		}
		sum+=j*htnode[i].freq;
		printf("%c :",i);
		while(j>0)
		{
			printf("%d",c[--j]);
		}
		printf("\n");
	}
	printf("平均符号長=%f\n\n",(double)sum/filelength);
}


void encode()
{
	int i,j;
	Node *n;
	int c[1000];

	for(i=0;i<filelength;i++)
	{
		n=&htnode[data[i]];//文字が格納されているノード
		j=0;
		while(n->parent!=NULL)
		{
			c[j]=n->code;
			n=n->parent;
			j++;
		}
		while(j>0)
		{
			bitwrite(c[--j]);
		}
	}
	bitflush();

}

void bitwrite(int bit)
{
	static unsigned char buf=0;
	buf=(buf<<1)|bit;
	bitpos++;
	if(bitpos==8)
	{
		enc_data[enc_filelength++]=buf;
		bitpos=0;
	}
}
void bitflush()
{
	while(bitpos!=0)
		bitwrite(0);
}

void encode_out()
{
	int i;
	printf("符号化\n");
	for(i=0;i<enc_filelength;i++)
	{
		printf("%02x",enc_data[i]);
	}
	printf("\n");
}
void decode()
{
	int i, bit, byte,c;
	Node *n;
	bit=0;
	byte=0;
	for(i=0;i<filelength;i++)
	{
		n=&htnode[top];
		while(n->lnode!=0)
		{
			c=bitread(byte,bit++);
			if(c==0)
				n=n->lnode;
			else
				n=n->rnode;
			if(bit==8)
			{
				bit=0;
				byte++;
			}
		}
		dec_data[i]=n->ch;
	}
}

int bitread(int byte, int bit)
{
	return (enc_data[byte]>>(7-bit))&1;
}

void decode_out()
{
	int i;
	printf("復号化\n");
	for(i=0;i<filelength;i++)
	{
		printf("%c",dec_data[i]);
	}
	printf("\n");
	printf("ファイルサイズ %dByte -> %dByte\n",filelength,enc_filelength);
}
