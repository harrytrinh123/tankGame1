#include <iostream>
#include <time.h>
#include <stdlib.h>
#include "console.h"
#include <math.h>
#include <conio.h>
#include <thread>
#include <windows.h>
#include <string>
#pragma comment(lib, "winmm.lib")
#include <fstream>

#define CONSOLE_HEIGHT  25
#define CONSOLE_WIDTH   50
#define MAU_BIEN        110
#define MAU_NEN         0
#define MAU_TANK        153
#define MAU_GACH        68
#define MAU_MENU        10

using namespace std;

enum TrangThai {Phai, Trai, Len, Xuong};

struct MotO
{
    char kitu;
    int mau;
} buffer[CONSOLE_HEIGHT][CONSOLE_WIDTH];

struct VatThe
{
    char kitu;
    int x, y;
    int mau;
    int diem;
    TrangThai ngang;
    TrangThai doc;
};

struct Node
{
    VatThe data;
    Node *link;
};

struct List
{
    Node *first, *last;
};

/// List
void Init(List &l);
Node *GetNode(VatThe x);
void AddLast(List &l, Node *x);
void RemoveFirst(List &l);
void RemoveAW(List &l, Node *p);
Node * FindNode(List &L, int x, int y);

/// Tree
struct TNode {
	int data;
	TNode *left, *right;
};

typedef TNode *Tree;

void Init(Tree &T);
TNode * createNode(int x);
void insertNode(Tree &T, TNode * p);
void Nhap(Tree &T);
void RNL(Tree T, int &x, int &y);
int demNode(Tree T);
void xoaNhoNhat(Tree &T);
int isDupplicate(Tree T, int x);

///Xu li File
int docFile(Tree &T);
void ghiRNL(Tree T, FILE *f);


/// GameSetting
void SetWindowConsole(SHORT width, SHORT height);
void SetBuffer();
void SetTank(VatThe &tank);
void HienThi(VatThe tank, VatThe zome, List listDan, List listTuong, int score, Tree T);
void VeMotO(int dong, int cot, char kytu, int mau);
void VeTank(VatThe tank);
void VeList(List l);
void DanBay(List &l);
void ThemTuong(List &listTuong, int diem);
void TuongRoi(List &listTuong);

/// Xu ly va cham
void XuLyDanChamGach(List &listTuong, List &listDan, int &SCORE);
int XuLyChamTank(List l_gach, VatThe Tank);
int XuLyChamBien(List &l_gach);
void XuLyChamZome(List &l_dan, VatThe zome);

///Zombie
void SetZome(VatThe &zome);
void VeZome(VatThe zome);
void ZomeDiChuyen(VatThe &zome);

// Scores
void VeDiem(int score, Tree T);

// music
void playBackgroundMusic(LPCSTR path);
void playSound(LPCSTR path);
// Menu
void Menu(Tree &T, bool game);

int main()
{
    // Setup Game
    thread threadPlayBGM(playBackgroundMusic, ".\\music\\e_-_Against_The_Current_128kbps_MP3_.wav");
    threadPlayBGM.detach();

    SetWindowConsole(CONSOLE_WIDTH, CONSOLE_HEIGHT);
    SetBuffer();
    srand(time(NULL));

    VatThe tank;
    SetTank(tank);
    VatThe zome;
    SetZome(zome);

    List listDan;
    Init(listDan);

    List listTuong;
    Init(listTuong);

    int time_wait = 0;
    int SCORE = 0;
    bool game = true;

    // Xu ly cham tank, cham bien
    int chamTank, chamBien;

    // doc file diem
    Tree T;
    Init(T);
    docFile(T);


    // Run Game
    while(game)
    {

        // Hien Thi
        HienThi(tank, zome, listDan, listTuong, SCORE, T);

        // Xu li phim nhan vao
        int hit = inputKey();
        if(hit == key_Left && tank.x > 2)
        {
            tank.x--;
        }
        else if(hit == key_Right && tank.x < CONSOLE_WIDTH-32)
        {
            tank.x++;
        }
        else if(hit == ' ')
        {
            VatThe dan;
            dan.x = tank.x;
            dan.y = tank.y-1;
            dan.kitu = '#';
            dan.mau = MAU_TANK;
            AddLast(listDan, GetNode(dan));
        };

        // Xu li Game
        if(listDan.first && listDan.first->data.y == 1)
            RemoveFirst(listDan);


        if(time_wait == 3000)
        {
            int diem = (rand() % 100) >= 80 ? 2 : 1;
            ThemTuong(listTuong, diem);
            time_wait = 0;
        }

        // Chuyen dong
        DanBay(listDan);
        TuongRoi(listTuong);
        ZomeDiChuyen(zome);
        // Xu dan cham gach
        XuLyDanChamGach(listTuong, listDan, SCORE);
        XuLyChamZome(listDan, zome);
        //Xu ly thang thua
        chamTank = XuLyChamTank(listTuong, tank);
        chamBien = XuLyChamBien(listTuong);
        if(chamTank==-1 || chamBien==-1) {
            RemoveFirst(listTuong);
            insertNode(T, createNode(12));
            xoaNhoNhat(T);
            SCORE = 0;

            /// Xu ly menu
            gotoXY(CONSOLE_WIDTH-17, 2);
            TextColor(112);
            cout << "GAMEOVER";
            gotoXY(CONSOLE_WIDTH-15,8);
            cout<<"MENU";
            gotoXY(CONSOLE_WIDTH-20,10);
            cout<<"Choi tiep (Press any key else)";
            gotoXY(CONSOLE_WIDTH-20,11);
            cout<<"Bang HighScores(SPACE)";
            gotoXY(CONSOLE_WIDTH-20,12);
            cout<<"Thoat(ESC)";
            int choose = getch();
            // Xu li menu
            switch(choose){
                case 23:{
                    // Bang HighScore
                    system("pause");
                    break;
                }
                case 27:{// Thoat
                    TextColor(MAU_NEN);
                    system("cls");
                    game = false;
                }
                default : SCORE = 0;
            }

        }

        time_wait += 100;
        Sleep(100);
    }

    threadPlayBGM.~thread();

    return 0;
}

/// List
void Init(List &l)
{
    l.first = l.last = NULL;
}

Node *GetNode(VatThe x)
{
    Node *p = new Node;
    if(!p)
        return NULL;

    p->data = x;
    p->link = NULL;
    return p;
}

void AddLast(List &l, Node *x)
{
    if(l.first == NULL)
        l.first=l.last=x;
    else
    {
        l.last->link = x;
        l.last = x;
    }
}

void RemoveFirst(List &l)
{
    if(l.first == NULL)
        return;
    Node *p = l.first;
    l.first = l.first->link;
    delete(p);
    if(l.first == NULL)
        l.last == NULL;
}

Node * FindNode(List &L, int x, int y) {
    Node *temp = L.first;
    while(temp!=NULL) {
        if(temp->data.x==x && temp->data.y) {
            return temp;
        }
        temp=temp->link;
    }
    return NULL;
}

void RemoveAW(List &l, Node *p)
{
    if(l.first == NULL)
        return;

    Node *q = l.first;
    Node *k;
    while(q!= NULL && q != p)
    {
        k = q;
        q = q->link;
    }

    if(q == l.first)
        RemoveFirst(l);
    else
    {
        k->link = q->link;
        if(q == l.last)
        {
            l.last = k;
        }
        delete(q);
    }

}

///Tree
void Free(Tree & root){
	if(root == NULL)	return;
	Free(root->left);
	Free(root->right);
	Free(root);
}

void Init(Tree &T) {
	T = NULL;
}

TNode * createNode(int x) {
	TNode *p = new TNode;
	if(p==NULL) return NULL;
	p->data = x;
	p->left = p->right = NULL;
	return p;
}


void insertNode(Tree &T, TNode *p) {
	if(T==NULL) {
        T = new TNode;
		T=p;
	}
	else {
		if(T->data == p->data) return;
		else if(p->data<T->data) insertNode(T->left, p);
		else if(p->data>T->data) insertNode(T->right, p);
	}
}
void Nhap(Tree &T) {
	while(1) {
		int x;
		cout << "Nhap x: ";
		cin >> x;
		if(x!=0) {
			TNode *p = createNode(x);
			insertNode(T, p);
		}
		else
			break;
	}
}

int demNode(Tree T) {
	if(T==NULL) return 0;
	return 1 + demNode(T->left) + demNode(T->right);
}

void xoaNhoNhat(Tree &T) {
	TNode *p = T;
	TNode *q = NULL;
	while(p->left != NULL) {
		q = p;
		p=p->left;
	}
	if(p->right == NULL) {
		q->left = NULL;
		delete(p);
	}
	else {
		q->left = p->right;
		delete(p);
	}
}

int isDupplicate(Tree T, int x) {
    if(T==NULL) return 0;
    if(T->data == x) return 1;
    isDupplicate(T->left, x);
    isDupplicate(T->right, x);
}


///Xu li File
int docFile(Tree &T) {
	int num;
	FILE *f = fopen("hightscore.txt", "r");
	if(!f)
		return 0;
	while(!feof(f)) {
		fscanf(f,"%d\n", &num);
		insertNode(T, createNode(num));
	}
	fclose(f);
}

void ghiRNL(Tree T, FILE *f) {
	if(T!=NULL) {
		ghiRNL(T->right, f);
		fprintf(f, "%d\n", T->data);
		ghiRNL(T->left, f);
	}
}



/// Game setting
void SetWindowConsole(SHORT width, SHORT height)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    SMALL_RECT WindowSize;
    WindowSize.Top = 0;
    WindowSize.Left = 0;
    WindowSize.Right = width;
    WindowSize.Bottom = height;

    SetConsoleWindowInfo(hStdout, 1, &WindowSize);

    HWND hWnd = GetConsoleWindow();
    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_SIZEBOX);
    ShowScrollBar(hWnd, SB_BOTH, true);

    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

void SetBuffer()
{
    for(int i = 0; i < CONSOLE_HEIGHT; i++)
        for(int j = 0; j <= CONSOLE_WIDTH-1; j++)
        {

            buffer[i][j].kitu = ' ';
        }
}

void SetTank(VatThe &tank)
{
    tank.x = (CONSOLE_WIDTH - 30) / 2;
    tank.y = CONSOLE_HEIGHT -2;
    tank.mau = MAU_TANK;
    tank.kitu = '#';
}

void HienThi(VatThe tank, VatThe zome, List listDan, List listTuong, int score, Tree T)
{
    // Ve bien
    for(int i = 0; i < CONSOLE_HEIGHT; i++)
    {
        buffer[i][0].kitu = '*';
        buffer[i][CONSOLE_WIDTH-30].kitu = '*';
        buffer[i][CONSOLE_WIDTH-1].kitu = '*';
    }
    for(int i = 0; i < CONSOLE_WIDTH; i++)
    {
        buffer[0][i].kitu = '*';
        buffer[CONSOLE_HEIGHT-1][i].kitu = '*';
    }

    // Ve Tank
    VeTank(tank);

    // Ve zome
    VeZome(zome);

    // Ve Dan
    VeList(listDan);

    // Ve Tuong
    VeList(listTuong);

    // Ve diem
    VeDiem(score, T);

    // In buffer
    gotoXY(0,0);
    for(int i = 0; i < CONSOLE_HEIGHT; i++)
    {
        for(int j = 0; j < CONSOLE_WIDTH; j++)
        {
            if(buffer[i][j].kitu == ' ')
                TextColor(MAU_NEN);
            else if(buffer[i][j].kitu == '*')
                TextColor(MAU_BIEN);
            else
                TextColor(buffer[i][j].mau);

            putchar(buffer[i][j].kitu);
            buffer[i][j].kitu = ' ';
        }

        if(i < CONSOLE_HEIGHT-1)
            putchar('\n');
    }
}

void VeMotO(int dong, int cot, char kytu, int mau)
{
    buffer[dong][cot].kitu = kytu;
    buffer[dong][cot].mau = mau;
}

void VeTank(VatThe tank)
{
    VeMotO(tank.y-1, tank.x, tank.kitu, tank.mau);
    VeMotO(tank.y, tank.x-1, tank.kitu, tank.mau);
    VeMotO(tank.y, tank.x, tank.kitu, tank.mau);
    VeMotO(tank.y, tank.x+1, tank.kitu, tank.mau);
}

void VeList(List l)
{
    Node *p = l.first;
    while(p != NULL)
    {
        buffer[p->data.y][p->data.x].kitu = p->data.kitu;
        buffer[p->data.y][p->data.x].mau = p->data.mau;

        p = p->link;
    }
}

void DanBay(List &l)
{
    Node *p = l.first;
    while(p != NULL)
    {
        p->data.y--;
        p = p->link;
    }
}

void ThemTuong(List &listTuong, int diem)
{
    VatThe a;
    a.x = rand() % (CONSOLE_WIDTH - 33) + 2;
    a.y = 0;
    a.kitu = '#';
    a.diem = diem;

    if(diem == 1)
        a.mau = MAU_GACH;
    else
        a.mau = 238;

    AddLast(listTuong, GetNode(a));
}

void TuongRoi(List &listTuong)
{
    Node *p = listTuong.first;
    while(p != NULL)
    {

        p->data.y++;
        p = p->link;
    }
}

// Xu ly dan cham gach
void XuLyDanChamGach(List &l_gach, List &l_dan, int &SCORE) {

    Node *p = l_gach.first;
    Node *q = l_dan.first;
    Node *l =p;
    while(p!=NULL) {
        while(q!=NULL) {
            if(p ->data.x == q->data.x && (q->data.y - p->data.y)<1) {
                SCORE += p->data.diem;
                q=q->link;
                RemoveFirst(l_gach);
//                if(l_dan.first)
//                {
                    RemoveFirst(l_dan);
//                    Node *k = q;
//                    q=q->link;
//                    RemoveAW(l_dan, k);
//                }
            }
            else
                q=q->link;
        }
        p=l->link;
    }
}

// Xu ly cham va cham tank, va cham bien
int XuLyChamTank(List l_gach, VatThe Tank) {
    if(l_gach.first) {
        int dX = abs(Tank.x - l_gach.first->data.x);
        int dY = abs(Tank.y - l_gach.first->data.y);
        if(dX<1 && dY<1) {
            return -1;
        }
    }
    return 0;
}

int XuLyChamBien(List &l_gach) {
    if(l_gach.first && l_gach.first->data.y == CONSOLE_HEIGHT-1) {
        return -1;
    }
    return 0;
}

// Xu ly cham zome
void XuLyChamZome(List &l_dan, VatThe zome) {
    Node *temp = l_dan.first;
    while(temp!=NULL) {
        int dX = abs(temp->data.x - zome.x);
        int dY = abs(temp->data.y - zome.y);
        if(dX <2 && dY < 2) {
            Node * nodeXoa = FindNode(l_dan, temp->data.x, temp->data.y);
            if(nodeXoa!=NULL)
                RemoveAW(l_dan, nodeXoa);
        }
        temp = temp->link;
    }
}

// Khoi tao va ve zomebie
void SetZome(VatThe &zome)
{
    TextColor(0);
    zome.x = (CONSOLE_WIDTH - 30)-3;
    zome.y = CONSOLE_HEIGHT - 10;
    zome.mau = 85;
    zome.kitu = '#';
    zome.ngang = Trai;
    zome.doc = Len;
}

void VeZome(VatThe zome)
{
    VeMotO(zome.y, zome.x, zome.kitu, zome.mau);
    VeMotO(zome.y, zome.x-1, '@', zome.mau);
    VeMotO(zome.y, zome.x+1, '@', zome.mau);
    VeMotO(zome.y+1, zome.x, '@', zome.mau);
    VeMotO(zome.y-1, zome.x, '@', zome.mau);
}

void ZomeDiChuyen(VatThe &zome) {
    int len = 1;
    if(zome.x == 2)
        zome.ngang = Phai;
    else if(zome.x == CONSOLE_WIDTH-32)
        zome.ngang = Trai;
    if(zome.ngang == Trai) {
        zome.x--;
        if(zome.doc==Len) {
            zome.y-=2;
            zome.doc=Xuong;
        }
        else if(zome.doc==Xuong) {
            zome.y+=2;
            zome.doc=Len;
        }
    }
    else if(zome.ngang == Phai){
        zome.x++;
        if(zome.doc==Len) {
            zome.y--;
            zome.doc=Xuong;
        }
        else if(zome.doc==Xuong) {
            zome.y++;
            zome.doc=Len;
        }
    }
}

// music
void playBackgroundMusic(LPCSTR path)
{
    PlaySound(path, NULL, SND_LOOP);
}

// Scores
void RNL(Tree T, int &x, int &y) {
	if(T!=NULL) {
        y++;
		RNL(T->right, x, y);

		int a[3] = {0};

        int score = T->data;
        int i = 0;
        while(score)
        {
            a[i] = score % 10;
            score/= 10;
            i++;
        }

        VeMotO(y+1, x, a[2] + '0', 11);
        VeMotO(y+1, x+1, a[1] + '0', 11);
        VeMotO(y+1, x+2, a[0] + '0', 11);

		RNL(T->left, x, y);
	}
}
void VeDiem(int score, Tree T)
{
    string s = "SCORE";
    int a[3] = {0};
    int x = (CONSOLE_WIDTH +28)/2 - s.length();
    int y = 5;
    VeMotO(y, x, s[0], MAU_MENU);
    VeMotO(y, x+1, s[1], MAU_MENU);
    VeMotO(y, x+2, s[2], MAU_MENU);
    VeMotO(y, x+3, s[3], MAU_MENU);
    VeMotO(y, x+4, s[4], MAU_MENU);


    int i = 0;
    while(score)
    {
        a[i] = score % 10;
        score/= 10;
        i++;
    }

    x++;

    VeMotO(y+1, x, a[2] + '0', 11);
    VeMotO(y+1, x+1, a[1] + '0', 11);
    VeMotO(y+1, x+2, a[0] + '0', 11);

    string duongke = "------------";
    string hightScores = "HIGHT SCORES";
    int tempX = (CONSOLE_WIDTH +32)/2 - duongke.length();
    y = 7;
    for(int i=0; i< duongke.length(); i++) {
        VeMotO(y, ++tempX, duongke[i], MAU_MENU);
    }
    y=8;
    tempX = (CONSOLE_WIDTH +32)/2 - duongke.length();
    for(int i=0; i< duongke.length(); i++) {
        VeMotO(y, ++tempX, hightScores[i], MAU_MENU);
    }

    RNL(T, x, y);
    Menu(T,true);

}

/// Menu game
void Menu(Tree &t, bool game){
    string s = "MENU";
    int x = (CONSOLE_WIDTH +27)/2 - s.length();
    int y = 16;
    VeMotO(y, x, s[0], MAU_MENU);
    VeMotO(y, x+1, s[1], MAU_MENU);
    VeMotO(y, x+2, s[2], MAU_MENU);
    VeMotO(y, x+3, s[3], MAU_MENU);
    string s1 = "Tiep tuc";
    int x1 = (CONSOLE_WIDTH +30)/2 - s1.length();
    int y1 = 17;
    VeMotO(y1, x1, s1[0], MAU_MENU);
    VeMotO(y1, x1+1, s1[1], MAU_MENU);
    VeMotO(y1, x1+2, s1[2], MAU_MENU);
    VeMotO(y1, x1+3, s1[3], MAU_MENU);
    VeMotO(y1, x1+4, s1[4], MAU_MENU);
    VeMotO(y1, x1+5, s1[5], MAU_MENU);
    VeMotO(y1, x1+6, s1[6], MAU_MENU);
    VeMotO(y1, x1+7, s1[7], MAU_MENU);
    VeMotO(y1, x1+8, s1[8], MAU_MENU);

    string s2 = "Thoat";
    int x2 = (CONSOLE_WIDTH + 25)/2 - s2.length();
    int y2 = 18;
    VeMotO(y2, x2, s2[0], MAU_MENU);
    VeMotO(y2, x2+1, s2[1], MAU_MENU);
    VeMotO(y2, x2+2, s2[2], MAU_MENU);
    VeMotO(y2, x2+3, s2[3], MAU_MENU);
    VeMotO(y2, x2+4, s2[4], MAU_MENU);

}
