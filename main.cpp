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
#define MAU_BIEN        119
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
struct Tree{
    int data;
    Tree* left;
    Tree* right;
};

void Free(Tree* root);
int LeftOf(const Tree* root, const int value);
int RightOf(const Tree* root, const int value);
Tree* CreateTree(Tree* T, const int value);
int MaxTree(Tree* t);
int DemNode(Tree* root);
void LNR(Tree* root);

//Xu li File



/// GameSetting
void SetWindowConsole(SHORT width, SHORT height);
void SetBuffer();
void SetTank(VatThe &tank);
void HienThi(VatThe tank, VatThe zome, List listDan, List listTuong, int score);
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
void VeDiem(int score);

// music
void playBackgroundMusic(LPCSTR path);
void playSound(LPCSTR path);

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

    // Run Game
    while(game)
    {

        // Hien Thi
        HienThi(tank, zome, listDan, listTuong, SCORE);

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
        //Xu li Tree
        Tree* root = NULL;
        //Xu ly thang thua
        chamTank = XuLyChamTank(listTuong, tank);
        chamBien = XuLyChamBien(listTuong);
        if(chamTank==-1 || chamBien==-1) {
            root = CreateTree(root,SCORE); // Khi tank cham gach hoac ganh cham bien thi them score vao tree
            RemoveFirst(listTuong);
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
//Tree
void Free(Tree* root){
	if(root == NULL)	return;
	Free(root->left);
	Free(root->right);
	Free(root);
}

int LeftOf(const Tree* root, const int value){
	return value <= root->data;
}

int RightOf(const Tree* root, const int value){
	return value > root->data;
}
Tree* CreateTree(Tree* T, const int value){
	if(T == NULL){
		Tree* node = (Tree*) malloc (sizeof(Tree));
		node->left = NULL;
		node->right = NULL;
		node->data = value;
		return node;
	}
	if(LeftOf(T,value))
		T->left = CreateTree(T->left,value);
	else if(RightOf(T,value))
		T->right = CreateTree(T->right,value);
	return T;
}
int MaxTree(Tree* t){
    if (t->right == NULL)
    {
        return t->data;
    }
    return MaxTree(t->right);
}

int DemNode(Tree* root){
    if(root == NULL){
        return 0;
    }
    return DemNode(root->left)+ DemNode(root->right);
}
void LNR(Tree* root){
    if(root == NULL)    return;
    LNR(root->left);
    cout << root->data << endl;
    LNR(root->right);
}
//Xu li File



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

void HienThi(VatThe tank, VatThe zome, List listDan, List listTuong, int score)
{
    // Ve bien
    for(int i = 0; i < CONSOLE_HEIGHT; i++)
    {
        buffer[i][0].kitu = '*';
        buffer[i][CONSOLE_WIDTH-30].kitu = '*';
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
    VeDiem(score);

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
void VeDiem(int score)
{
    string s = "SCORE";
    string s1 = "320";
    string s2 = "210";
    int a[3] = {0};
    int x = (CONSOLE_WIDTH + 28) / 2  - s.length();
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
    int value = a[0] + a[1] * 10 + a[2] *100;
}
