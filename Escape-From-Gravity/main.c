#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include "gameboard.h"
#include <process.h>    // ��Ƽ������ ó�� �߰�
#include <mmsystem.h>   // ���� �߰�
#pragma comment(lib,"winmm.lib")
// ���� ���
#define BGM_MAIN1 "sound/questiion_lost moons-greatful eights.wav"
#define BGM_MAIN2 "sound/questiion_lost moons-serious as an attack button.wav"
#define BGM_MAIN3 "sound/zagi2_bodenhacke loop.wav"
// ��Ȧ ���
#define BGM_BLACKHOLE "sound/zagi2_space serpent.wav"


#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define SPACE 32
#define CTRL 17

/*���Ӻ��� ũ��*/
#define GBOARD_WIDTH 70
#define GBOARD_HEIGHT 50

/*���Ӻ��� ������*/
#define GBOARD_ORIGIN_X 4
#define GBOARD_ORIGIN_Y 2

/*�߷��� ����*/
#define GUP 0
#define GRIGHT 1
#define GDOWN 2
#define GLEFT 3

char blockModel[][3][3] =
{
    /* �÷��̾�
        �� 97
        �� 98
        �� 99
       ��   �� ��
        -67 -71 97
       ��  ��  ��
           -73
        �� 99
        ��-28
        �� 97
        */

        //�÷��̾��� �Ӹ� �κ��� �߷��� ����

        //�߷��� �������� �ۿ��Ҷ�  0
        {
            {0, 99, 0 },
            {0, 98, 0 },
            {0, 97, 0 },
        }

        ,
    //�߷��� ���������� �ۿ��Ҷ�  1
    {
        {0, 0, 0 },
        {97,-71,-67},
        {0, 0, 0 },
    }

     ,
    //�߷��� �Ʒ������� �ۿ��Ҷ�  2
    {
        {0, 97, 0 },
        {0, -28, 0 },
        {0, 99, 0 },
    }
  ,
    //�߷��� �������� �ۿ��Ҷ�  3
    {
        {0, 0, 0 },
        {-67,-73,97},
        {0, 0, 0 },
    }


};

int curPosX = 0, curPosY = 0;
int block_id; //���� PC���
int speed; //�ӵ�
int score = 0; //���� ?
int mission;
int gravity = 0; //�߷¹���
int health = 3; //ü��
clock_t start, end; //�ð�
int duration;
int min;
int sec;
int detX, detY;//���� �پ����� ��� �Ű��ֱ� ����
int succ = 0; //�������� Ŭ���� ����
int clearflag = 0;// ���ø� �Ⱥ��̰� ���� ���̰� ���� ����
int periodflag = 0;//������ on/off ��
int obstacleSpeed = 0; //� �̵� ����
int isBlackhole = 0; //��Ȧ ����
int mapNum = 1; //���������� ��Ÿ���� ����
int tempmapNum; //��Ȧ ������ �� �� ���������� �ӽ�����
int stageKey[10] = { 0,0,0,2,3,3,0,0,0,0 };// ���������� Ű ���� ���� ����

int acheivecnt = 0;
int In4Min = 0; // 4�� �̳��� Ŭ����
int In3Life = 0; // ��� 3���� Ŭ����
int InNoBlackHole = 0; // ��Ȧ�� �ѹ��� �Ⱥ����� Ŭ����
int InProgamer = 0; // 4�� �̳��� 3�� ������� Ŭ����
int InDrank1Life = 0; // ��� 1������ D��ũ�� Ŭ����
int InStage1clear = 0; //�������� 1 Ŭ����
int InStage4clear = 0; //�������� 4 Ŭ����
int InStage5clear = 0; //�������� 5 Ŭ����
int InBlackHoleAddict = 0; //��Ȧ 3�� �̻� Ŭ����
int InExplorer = 0;  // D��ũ + ��Ȧ 3�� �̻� Ŭ����
int isObstacleCollision = 0; //��� �浹 ����
int slowSpeedTime = 0; //PC�ӵ� ���Ͻð� ����
int gravitytemp = 0;

void SetCurrentCursorPos(int x, int y); //Ŀ������
COORD GetCurrentCursorPos(void);//����Ŀ�� ��������
void RemoveCursor(void);//Ŀ�������
void GameInfoInit(); //����info �ʱ�ȭ
void GameInfoUpdate(); //����info ������Ʈ
void DrawMap(); //�ʱ׸���
void ShowBlock(char blockInfo[3][3]); //PC���
void DeleteBlock(char blockInfo[3][3]); //PC�����
int DetectCollision(int posX, int posY, char blockModel[3][3]); //PC�� NPC�浹 üũ
void CollisionThorn(); //���ÿ� �浹ó��
void RedrawBlocks(char blockInfo[3][3]); //�� �ٽ� �׸���
int ShiftRight(); //�߷¹��� ������
int ShiftLeft(); //����
int ShiftUp(); //��
int ShiftDown(); //�Ʒ�
int IsGameOver(); //���� ���� üũ
void RotateBlock(); //PCȸ��
int ProcessKeyInput(); //key�Է¹ޱ�
void InitialScreen(); //�ʱ� ����ȭ��
void SecondScreen(); //���Ӽ��� �� ��� ȭ��
void GameFinishScreen(); //���� ����ȭ��
void moveObstacle(); //��̵�
void CollisionBlackhole(); //��Ȧ ó��
int CollisionDest(); //������ �浹
void ClearThorn();//������ ����
void CollisionPotion();//���� �ߵ�
void CollisionMeteorite(int x, int y); //� �浹
void CollisionKey(); //Ű �浹 
void drawBar();//�߷¹�
void deleteBar();//�߷¹� ����
COORD pcStartPos[5] = {
   {6,37},      // 0: blackhole
   {7,5},       // 1: map 1
   {11,25},     // 2: map 2
   {13,40},     // 3: map 3
   {3,3},       // 4: map 4
};


unsigned _stdcall Thread_A(void* arg)//������ �ֱ� ī������ ���� ������a
{
    while (1)
    {
        Sleep(3000);//3�� ������
        periodflag = 1;
    }
}

int main()
{
    speed = 5;

    SetConsoleTitle(TEXT("Escape From Gravity"));
    system("mode con: cols=186 lines=54");
    RemoveCursor();

    PlaySound(TEXT(BGM_MAIN3), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    
    //ȭ�� ������
    InitialScreen();
    SecondScreen();

    PlaySound(TEXT(BGM_MAIN2), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

    gravity = 2;
    block_id = 2;

    curPosX = pcStartPos[mapNum].X * 2;
    curPosY = pcStartPos[mapNum].Y;
    SetCurrentCursorPos(curPosX, curPosY);

    _beginthreadex(NULL, 0, Thread_A, 0, 0, NULL);//������a ����

    while (1)
    {
        start = clock();
        DrawMap();
        drawBar();
        GameInfoInit();


        //�׽�Ʈ�� ���ؼ� �������� ���� X

        while (!IsGameOver())
        {

            ProcessKeyInput();
            GameInfoUpdate();

            switch (gravity)
            {
            case GDOWN: //2
                ShiftDown();
                break;
            case GRIGHT: //1
                ShiftRight();
                break;
            case GUP: //0
                ShiftUp();
                break;
            case GLEFT: //3
                ShiftLeft();
                break;
            }

            //������ 
            if (periodflag == 1) {
                ClearThorn();
                periodflag = 0;
            }

            RedrawBlocks(blockModel[block_id]);

            //������������ ��� �ӵ��� ����
            //�������� 2 ~ � ����
            if (obstacleSpeed % 3 == 0 && mapNum >= 2)
            {
                obstacleSpeed = 0;
                moveObstacle(); //� �̵�
            }


            //��� �浹���� �ӵ����� ����
            //������ ������ ���� �ӵ����� �ð� ����
            if (isObstacleCollision == 1 && slowSpeedTime % 5 == 0) {
                speed = 5;
                isObstacleCollision = 0;
                slowSpeedTime = 0;
            }

            obstacleSpeed++;
            slowSpeedTime++;

            end = clock();
            if (IsGameOver()) {
                deleteBar();
                break;
            }

        }
        break;
    }


    GameFinishScreen();

    getchar();

    return 0;

}

void SetCurrentCursorPos(int x, int y)
{
    COORD pos = { x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

COORD GetCurrentCursorPos(void)
{
    COORD curPoint;
    CONSOLE_SCREEN_BUFFER_INFO curInfo;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    curPoint.X = curInfo.dwCursorPosition.X;
    curPoint.Y = curInfo.dwCursorPosition.Y;

    return curPoint;
}

void RemoveCursor(void)
{
    CONSOLE_CURSOR_INFO curInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    curInfo.bVisible = 0;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

// ������ �Լ� �߰�
void ClearThorn() {

    COORD temp = GetCurrentCursorPos();
    int collision;
    if (clearflag == 0) {
        for (int y = 0; y < GBOARD_HEIGHT; y++) {

            for (int x = 0; x < GBOARD_WIDTH; x++) {
                if (mapInfo[mapNum][y][x] == 7) {// �������� ��ġ�� ã�� ������ ���� �� ������ -7�� �ٲ�
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y + y);
                    mapInfo[mapNum][y][x] = -7;
                    printf("  ");
                }
            }
        }
        clearflag = 1;// ���� ���ʰ� ���ø� ������ ���ʶ�� ���� �˷���
    }
    else if (clearflag == 1) {
        int flag = 0;
        collision = 0;
        for (int y = 0; y < GBOARD_HEIGHT; y++) {

            for (int x = 0; x < GBOARD_WIDTH; x++) {
                if (mapInfo[mapNum][y][x] == -7) {//-7�� �� ������ ������ ã�Ƽ� �ٽ� ���ø� ������
                    mapInfo[mapNum][y][x] = 7;
                    if (DetectCollision(curPosX, curPosY, blockModel[block_id]) == 7) {//���ڸ��� ���ִٰ� �����ǰݴ��ϴ� ���
                        SetCurrentCursorPos(curPosX, curPosY);
                        DeleteBlock(blockModel[block_id]);
                        if (detX == 1 && detY == 2) {
                            curPosY -= 1;
                        }
                        else if (detX == 0 && detY == 1) {

                            curPosX += 2;
                        }
                        else if (detX == 2 && detY == 1) {
                            curPosX -= 2;
                        }
                        else if (detX == 1 && detY == 0) {
                            curPosY += 1;
                        }
                        SetCurrentCursorPos(curPosX, temp.Y);
                        ShowBlock(blockModel[block_id]);
                        collision = 1;
                    }
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y + y);
                    if (mapInfo[mapNum][y - 1][x] == 0 && mapInfo[mapNum][y + 1][x] == 1) {
                        printf("��");
                    }
                    else if (mapInfo[mapNum][y - 1][x] == 1 && mapInfo[mapNum][y + 1][x] == 0) {
                        printf("��");
                    }
                    else if (mapInfo[mapNum][y][x - 1] == 1 && mapInfo[mapNum][y][x + 1] == 0) {
                        printf("��");
                    }
                    else if (mapInfo[mapNum][y][x - 1] == 0 && mapInfo[mapNum][y][x + 1] == 1) {
                        printf("��");
                    }
                }
            }
        }
        if (collision == 1) {
            SetCurrentCursorPos(curPosX, curPosY);
            CollisionThorn();
        }
        clearflag = 0;// ���� ���ʰ� ���ø� �Ⱥ��̰� �� ���ʶ�� ���� �˷���
    }
    SetCurrentCursorPos(curPosX, curPosY);
}


void DrawMap() {

    for (int y = 0; y < GBOARD_HEIGHT; y++) {
        SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + y);
        for (int x = 0; x < GBOARD_WIDTH; x++) {
            switch (mapInfo[mapNum][y][x]) {
            case 0:	// �� ����
                printf("  ");
                break;

            case -7: // 11.25 �߰�
                printf("  ");
                break;
            case 1:	// ���� ��

                SetConsoleOutputCP(437);	// CP437: Ȯ�� �ƽ�Ű ��� ���
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // DrawMap - ������ ��� �� �� ����
                printf("%c%c", 219, 219);
                SetConsoleOutputCP(949);	// ����
                break;
            case 2: // ������
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
                printf("��");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                break;
            case 3: // ����

                if (x - 1 >= 0 && x + 1 <= GBOARD_WIDTH - 1) {

                    if ((mapInfo[mapNum][y][x - 1] == 0) && (mapInfo[mapNum][y][x + 1] == 1)) {
                        printf("��");
                        break;
                    }
                    if ((mapInfo[mapNum][y][x + 1] == 0) && (mapInfo[mapNum][y][x - 1] == 1)) {
                        printf("��");
                        break;
                    }
                }
                if (y - 1 >= 0 && y + 1 <= GBOARD_HEIGHT - 1) {

                    if ((mapInfo[mapNum][y - 1][x] == 0) && (mapInfo[mapNum][y + 1][x] == 1)) {
                        printf("��");
                        break;
                    }
                    if ((mapInfo[mapNum][y - 1][x] == 1) && (mapInfo[mapNum][y + 1][x] == 0)) {
                        printf("��");
                        break;
                    }
                }
            case 4: // ����
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
                printf("��");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                break;
            case 5: // ��Ȧ
                printf("��");

                break;
            case 6: // �
                //printf("��");
                printf("  ");
                break;
            case 7: // ���� ����

                if (x - 1 >= 0 && x + 1 <= GBOARD_WIDTH - 1) {

                    if ((mapInfo[mapNum][y][x - 1] == 0) && (mapInfo[mapNum][y][x + 1] == 1)) {
                        printf("��");
                        break;
                    }
                    if ((mapInfo[mapNum][y][x + 1] == 0) && (mapInfo[mapNum][y][x - 1] == 1)) {
                        printf("��");
                        break;
                    }
                }
                if (y - 1 >= 0 && y + 1 <= GBOARD_HEIGHT - 1) {

                    if ((mapInfo[mapNum][y - 1][x] == 0) && (mapInfo[mapNum][y + 1][x] == 1)) {
                        printf("��");
                        break;
                    }
                    if ((mapInfo[mapNum][y - 1][x] == 1) && (mapInfo[mapNum][y + 1][x] == 0)) {
                        printf("��");
                        break;
                    }
                }
            case 8: // Ű
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
                printf("��");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                break;
            }
        }
    }
}

void ShowBlock(char blockInfo[3][3])
{
    int x, y;
    COORD curPos = GetCurrentCursorPos();

    for (y = 0; y < 3; y++)
    {
        for (x = 0; x < 3; x++)
        {
            SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);


            if (blockInfo[y][x] == 97)
                printf("��");
            if (blockInfo[y][x] == 98)
                printf("��");
            if (blockInfo[y][x] == 99)
                printf("��");
            if (blockInfo[y][x] == -71)
                printf("��");
            if (blockInfo[y][x] == -67)
                printf("��");
            if (blockInfo[y][x] == -28)
                printf("��");
            if (blockInfo[y][x] == -73)
                printf("��");

        }
    }
    SetCurrentCursorPos(curPos.X, curPos.Y);

}

int DetectCollision(int posX, int posY, char blockModel[3][3])
{
    int x, y;
    int arrX = (posX - GBOARD_ORIGIN_X) / 2;
    int arrY = posY - GBOARD_ORIGIN_Y;

    detX = 0;
    detY = 0;


    for (x = 0; x < 3; x++)
    {
        for (y = 0; y < 3; y++)
        {

            //���� �浹 �߰�
            if (mapInfo[mapNum][arrY + y][arrX + x] == 4 && blockModel[y][x] != 0) {
                mapInfo[mapNum][arrY + y][arrX + x] = 0;// ���� ������ �������� ����
                CollisionPotion();
                return 4;
            }
            //���� �浹
            if (mapInfo[mapNum][arrY + y][arrX + x] == 1 && blockModel[y][x] != 0) {
                detX = x;
                detY = y;

                return 0;
            }
            //���ÿ� �浹 ����
            if (mapInfo[mapNum][arrY + y][arrX + x] == 3 && blockModel[y][x] != 0) {
                detX = x;
                detY = y;
                return 3;
            }
            //���� ���ÿ� �浹 �߰�
            if (mapInfo[mapNum][arrY + y][arrX + x] == 7 && blockModel[y][x] != 0) {
                detX = x;
                detY = y;
                return 7;
            }
            //
            //��� �浹
            if (mapInfo[mapNum][arrY + y][arrX + x] == 6 && blockModel[y][x] != 0) {
                CollisionMeteorite(arrY + y, arrX + x);
                return 6;
            }


            //�������� �浹
            if (mapInfo[mapNum][arrY + y][arrX + x] == 2 && blockModel[y][x] != 0) {
                succ = 1;
                return 2;
            }

            //��� �浹
            if (mapInfo[mapNum][arrY + y][arrX + x] == 6 && blockModel[y][x] != 0) {
                CollisionMeteorite(arrY + y, arrX + x);
                return 6;
            }
            //��Ȧ�� �浹
            if (mapInfo[mapNum][arrY + y][arrX + x] == 5 && blockModel[y][x] != 0) {

                return 5;
            }
            //Ű �浹 �߰� 11.24
            if (mapInfo[mapNum][arrY + y][arrX + x] == 8 && blockModel[y][x] != 0) {
                mapInfo[mapNum][arrY + y][arrX + x] = 0;//Ű�� ������ ���� ����
                CollisionKey();
                return 8;
            }
        }
    }


    return 1;
}


void CollisionThorn() {
    health--; //ü�� -1
    end = clock();
    GameInfoUpdate();

    if (IsGameOver()) return;

    int deleteCnt = 0;
    int showCnt = 0;

    int moveCnt = 0;

    while (1) {

        if (periodflag == 1) {
            ClearThorn();
            periodflag = 0;
        }
        if (ProcessKeyInput())
            break;

        //11.27���� Sleep(300)�ι��� ������ ���� �ٲ�
        if (deleteCnt % 3 == 0)
            DeleteBlock(blockModel[block_id]);
        if (showCnt % 10 == 0)
            ShowBlock(blockModel[block_id]);

        if (moveCnt % 3 == 0 && mapNum >= 2)
            moveObstacle();

        moveCnt++;
        deleteCnt++;
        showCnt++;

        end = clock();
        GameInfoUpdate();

    }
}

void CollisionPotion() {//�����Լ�

    health++;
    if (health > 3) {//�ִ�ü��
        health = 3;
    }
    GameInfoUpdate();
}
//11.24 �Լ� �߰�
void CollisionKey() {
    stageKey[mapNum]--;
    if (stageKey[mapNum] <= 0) {//�ּ� 0��
        stageKey[mapNum] = 0;
    }
}

void DeleteBlock(char blockInfo[3][3])
{
    int x, y;
    COORD curPos = GetCurrentCursorPos();

    int arrX = (curPos.X - GBOARD_ORIGIN_X) / 2;
    int arrY = curPos.Y - GBOARD_ORIGIN_Y;

    for (y = 0; y < 3; y++)
    {
        for (x = 0; x < 3; x++)
        {
            SetCurrentCursorPos(curPos.X + 2 * x, curPos.Y + y);

            if (mapInfo[mapNum][arrY + y][arrX + x] <= 7 && 1 <= mapInfo[mapNum][arrY + y][arrX + x])
                continue;


            if (blockInfo[y][x] == 97)
                printf("  ");
            if (blockInfo[y][x] == 98)
                printf("  ");
            if (blockInfo[y][x] == 99)
                printf("  ");
            if (blockInfo[y][x] == -71)
                printf("  ");
            if (blockInfo[y][x] == -67)
                printf("  ");
            if (blockInfo[y][x] == -28)
                printf("  ");
            if (blockInfo[y][x] == -73)
                printf("  ");



        }
    }
    SetCurrentCursorPos(curPos.X, curPos.Y);

}
void RedrawBlocks(char blockInfo[3][3])
{
    int x, y;
    COORD curPos = GetCurrentCursorPos();

    int arrX = (curPos.X - GBOARD_ORIGIN_X) / 2;
    int arrY = curPos.Y - GBOARD_ORIGIN_Y;



    for (y = 0; y < 3; y++)
    {
        for (x = 0; x < 3; x++)
        {
            SetCurrentCursorPos(curPos.X + 2 * x, curPos.Y + y);

            if (mapInfo[mapNum][arrY + y][arrX + x] <= 8 && 1 <= mapInfo[mapNum][arrY + y][arrX + x])
                continue;
            else
                printf("  ");
        }
    }
    SetCurrentCursorPos(curPos.X, curPos.Y);

    ShowBlock(blockModel[block_id]);
}

int ShiftRight()
{

    //��� �浹
    if (DetectCollision(curPosX, curPosY, blockModel[block_id]) == 6) {
        return 0;
    }

    //���� �浹
    if (DetectCollision(curPosX + 2, curPosY, blockModel[block_id]) == 0)
        return 0;
    //���ÿ� �浹
    if (DetectCollision(curPosX + 2, curPosY, blockModel[block_id]) == 3) {
        CollisionThorn();
        return 0;
    }

    //�������� �浹
    if (DetectCollision(curPosX + 2, curPosY, blockModel[block_id]) == 2) {
        CollisionDest();
        return 0;
    }

    //��Ȧ�� �浹
    if (DetectCollision(curPosX + 2, curPosY, blockModel[block_id]) == 5) {
        CollisionBlackhole();
        return 0;
    }

    // ���� ���ÿ� �浹 �߰�
    if (DetectCollision(curPosX + 2, curPosY, blockModel[block_id]) == 7)
    {
        CollisionThorn();
        return 0;
    }

    DeleteBlock(blockModel[block_id]);
    curPosX += 2;
    SetCurrentCursorPos(curPosX, curPosY);
    ShowBlock(blockModel[block_id]);
    return 1;

}
int ShiftLeft()
{

    //��� �浹
    if (DetectCollision(curPosX, curPosY, blockModel[block_id]) == 6) {
        return 0;
    }

    //���� �浹
    if (DetectCollision(curPosX - 2, curPosY, blockModel[block_id]) == 0)
        return 0;

    //���ÿ� �浹
    if (DetectCollision(curPosX - 2, curPosY, blockModel[block_id]) == 3) {
        CollisionThorn();
        return 0;
    }

    //�������� �浹
    if (DetectCollision(curPosX - 2, curPosY, blockModel[block_id]) == 2) {

        CollisionDest();

        return 0;
    }

    // ���� ���ÿ� �浹 �߰�
    if (DetectCollision(curPosX - 2, curPosY, blockModel[block_id]) == 7)
    {
        CollisionThorn();
        return 0;
    }

    //��Ȧ�� �浹
    if (DetectCollision(curPosX - 2, curPosY, blockModel[block_id]) == 5) {
        CollisionBlackhole();
        return 0;
    }

    DeleteBlock(blockModel[block_id]);
    curPosX -= 2;
    SetCurrentCursorPos(curPosX, curPosY);
    ShowBlock(blockModel[block_id]);
    return 1;
}

int ShiftUp()
{

    //��� �浹
    if (DetectCollision(curPosX, curPosY, blockModel[block_id]) == 6) {
        return 0;
    }

    //���� �浹
    if (DetectCollision(curPosX, curPosY - 1, blockModel[block_id]) == 0)
        return 0;

    // ���ÿ� �浹
    if (DetectCollision(curPosX, curPosY - 1, blockModel[block_id]) == 3)
    {
        CollisionThorn();
        return 0;
    }

    //��Ȧ�� �浹
    if (DetectCollision(curPosX, curPosY - 1, blockModel[block_id]) == 5) {
        CollisionBlackhole();
        return 0;
    }
    // ���� ���ÿ� �浹
    if (DetectCollision(curPosX, curPosY - 1, blockModel[block_id]) == 7)
    {
        CollisionThorn();
        return 0;
    }



    //�������� �浹
    if (DetectCollision(curPosX, curPosY - 1, blockModel[block_id]) == 2) {
        CollisionDest();
        return 0;
    }

    DeleteBlock(blockModel[block_id]);
    curPosY -= 1;
    SetCurrentCursorPos(curPosX, curPosY);
    ShowBlock(blockModel[block_id]);
    return 1;
}
int ShiftDown()
{

    //��� �浹
    if (DetectCollision(curPosX, curPosY, blockModel[block_id]) == 6) {
        return 0;
    }


    //���� �浹
    if (DetectCollision(curPosX, curPosY + 1, blockModel[block_id]) == 0)
        return 0;

    // ���ÿ� �浹
    if (DetectCollision(curPosX, curPosY + 1, blockModel[block_id]) == 3)
    {
        CollisionThorn();
        return 0;
    }

    // ���� ���ÿ� �浹
    if (DetectCollision(curPosX, curPosY + 1, blockModel[block_id]) == 7)
    {
        CollisionThorn();
        return 0;
    }

    //�������� �浹
    if (DetectCollision(curPosX, curPosY + 1, blockModel[block_id]) == 2) {
        CollisionDest();
        return 0;
    }

    //��Ȧ�� �浹
    if (DetectCollision(curPosX, curPosY + 1, blockModel[block_id]) == 5) {
        CollisionBlackhole();
        return 0;
    }


    DeleteBlock(blockModel[block_id]);
    curPosY += 1;
    SetCurrentCursorPos(curPosX, curPosY);
    ShowBlock(blockModel[block_id]);
    return 1;
}
int IsGameOver()
{
    //������������
    //1. ������ 0�� ���



    //���� 0�� ���
    if (health == 0) {
        succ = -1;
        return 1;
    }

    if (mapNum > 4) return 1;

    return 0;

}

void RotateBlock() {
    int colflag = 0;//�浹����
    int block_rotated = gravity;
    if (DetectCollision(curPosX, curPosY, blockModel[block_rotated]) == 0) {
        colflag = 1;
    }
    else if (DetectCollision(curPosX, curPosY, blockModel[block_rotated]) == 3) {//����
        colflag = 2;
    }
    else if (DetectCollision(curPosX, curPosY, blockModel[block_rotated]) == 7) {//������
        colflag = 2;
    }
    if (DetectCollision(curPosX, curPosY, blockModel[block_rotated]) == 2) {
        //3���������� �ӽñ��
        if (CollisionDest() == -1) {//ȸ���� �������� ������ Ű�� �� ������ ��� 
            gravity--;//ȸ�� �Ұ�����
            drawBar();
            return;
        }
    }
    if (DetectCollision(curPosX, curPosY, blockModel[block_rotated]) == 5) {//ȸ���� ��Ȧ�� ��°��
        CollisionBlackhole();
    }
    //�߰���//

    DeleteBlock(blockModel[block_id]);
    if (colflag == 1 || colflag == 2) {// ���ó� ���� �پ �̵��ϴ� ��� �浹�� �Ͼ�� ������ ��ǥ�� �Ű���
        int detobj;// �����浹 ��ü�ľ�1203
        if (detX == 1 && detY == 2) {
            curPosY -= 1;
            detobj = DetectCollision(curPosX, curPosY, blockModel[block_rotated]);
            if (detobj != 1) {//������ ��ü�� ��� ȸ�� �Ұ��ϰ� 1203
                curPosY -= 1;
                gravity = gravitytemp;
                if (detobj == 3 || detobj == 7 || colflag == 2) {
                    drawBar();
                    CollisionThorn();
                }
            }
        }
        else if (detX == 0 && detY == 1) {

            curPosX += 2;
            detobj = DetectCollision(curPosX, curPosY, blockModel[block_rotated]);
            if (detobj != 1) {//������ ��ü�� ��� ȸ�� �Ұ��ϰ� 1203
                curPosX -= 2;
                gravity = gravitytemp;
                if (detobj == 3 || detobj == 7 || colflag == 2) {
                    drawBar();
                    CollisionThorn();
                }
            }
        }
        else if (detX == 2 && detY == 1) {
            curPosX -= 2;
            detobj = DetectCollision(curPosX, curPosY, blockModel[block_rotated]);
            if (detobj != 1) {//������ ��ü�� ��� ȸ�� �Ұ��ϰ� 1203
                curPosX += 2;
                gravity = gravitytemp;
                if (detobj == 3 || detobj == 7 || colflag == 2) {
                    drawBar();
                    CollisionThorn();
                }
            }
        }
        else if (detX == 1 && detY == 0) {
            curPosY += 1;
            detobj = DetectCollision(curPosX, curPosY, blockModel[block_rotated]);
            if (detobj != 1) {//������ ��ü�� ��� ȸ�� �Ұ��ϰ� 1203
                curPosY -= 1;
                gravity = gravitytemp;
                if (detobj == 3 || detobj == 7 || colflag == 2) {
                    drawBar();
                    CollisionThorn();
                }
            }
        }

    }
    block_id = gravity;
    SetCurrentCursorPos(curPosX, curPosY);
    ShowBlock(blockModel[block_id]);
    drawBar();


    return;
}


int ProcessKeyInput()
{

    int i, key;

    int temp;

    for (i = 0; i < 3; i++)
    {
        if (_kbhit() != 0)
        {

            key = _getch();
            gravitytemp = gravity;
            switch (key)
            {
            case DOWN:
                break;
            case LEFT:
                deleteBar();
                gravity = (gravity + 1) % 4;
                RotateBlock();

                break;
            case RIGHT:
                deleteBar();
                gravity = (gravity + 3) % 4;
                RotateBlock();
                break;
            case UP:
                deleteBar();
                gravity = (gravity + 2) % 4;
                RotateBlock();


            }
            return 1;
        }

        Sleep(speed);
    }

    return 0;
}


// ȭ�� ������
void GameInfoInit()
{
    COORD curPos = GetCurrentCursorPos();

    //�� �߱�
    for (int i = 2; i < GBOARD_HEIGHT + GBOARD_ORIGIN_Y; i++)
    {
        SetCurrentCursorPos(GBOARD_WIDTH * 2 + GBOARD_ORIGIN_X * 2, i);
        printf("��\n");
    }


    //ü��
    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 24, GBOARD_ORIGIN_Y);
    printf("-HEALTH-");

    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 25, GBOARD_ORIGIN_Y + 3);
    for (int i = 0; i < health; i++) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        printf("��");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    }

    //�ð�
    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 25, GBOARD_ORIGIN_Y + 6);
    printf("-TIME-");
    duration = (end - start) / CLOCKS_PER_SEC;
    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 25, GBOARD_ORIGIN_Y + 9);
    printf("%02d:%02d", duration / 60, duration % 60);

    //Ŀ�� �ǵ�������
    SetCurrentCursorPos(curPos.X, curPos.Y);
}
void GameInfoUpdate()
{
    COORD curPos = GetCurrentCursorPos();
    //ü��

    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 25, GBOARD_ORIGIN_Y + 3);
    for (int i = 0; i < 3; i++)
        printf("  "); //���� �ִ� ü�� ������ �����.
    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 25, GBOARD_ORIGIN_Y + 3);
    for (int i = 0; i < health; i++) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        printf("��");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    }
    //�ð�

    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 25, GBOARD_ORIGIN_Y + 9);
    printf("           ");
    duration = (end - start) / CLOCKS_PER_SEC;
    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 25, GBOARD_ORIGIN_Y + 9);
    printf("%02d:%02d", duration / 60, duration % 60);


    //Ű ȹ�� ���� �ӽ��߰� 11.24
    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 22, GBOARD_ORIGIN_Y + 13);
    printf("����");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
    printf(" Key");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    printf(": %d", stageKey[mapNum]);
    //Ŀ�� �ǵ�������
    SetCurrentCursorPos(curPos.X, curPos.Y);

}
void InitialScreen()
{
    int key = -1;
    int skip = 0;
    for (int y = 0; y < GBOARD_HEIGHT; y++) {
        SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + y);
        for (int x = 0; x < GBOARD_WIDTH; x++) {

            if (_kbhit() != 0)
            {
                key = _getch();
                skip = 1;
                key = -1;
            }
            // if (key != -1) return;


            switch (InitialScreenInfo[y][x]) {
            case 0:	// �� ����
                printf("  ");
                break;
            case 1:	// ���� ��

                SetConsoleOutputCP(437);	// CP437: Ȯ�� �ƽ�Ű ��� ���
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // DrawMap - ������ ��� �� �� ����
                printf("%c%c", 219, 219);
                SetConsoleOutputCP(949);	// ����
                break;
            }
        }
        //�� �κ��� �ּ�ó���ϸ� �ִϸ��̼��� �����ϴ�.
        if (skip == 0)
            Sleep(80);
    }


    SetCurrentCursorPos(52, GBOARD_HEIGHT - 6);
    printf("Copyright��2021 SW18 All rights reserved.");


    int tmp = 0;

    while (key == -1)
    {

        if (_kbhit() != 0)
        {
            key = _getch();
        }
        if (key != -1) break;

        if (tmp % 2 == 0)
        {
            SetCurrentCursorPos(38 * 2 - 19, GBOARD_HEIGHT - 8);
            printf("����Ϸ��� �ƹ� Ű�� �Է��ϼ���.");
        }
        else
        {
            SetCurrentCursorPos(38 * 2 - 19, GBOARD_HEIGHT - 8);
            printf("                                             ");
        }
        Sleep(800);
        tmp++;

    }



}
void SecondScreen()
{

    for (int y = 0; y < GBOARD_HEIGHT; y++) {
        SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + y);
        for (int x = 0; x < GBOARD_WIDTH; x++) {
            switch (SecondScreenInfo[y][x]) {
            case 0:	// �� ����
                printf("  ");
                break;
            case 1:	// ���� ��

                SetConsoleOutputCP(437);	// CP437: Ȯ�� �ƽ�Ű ��� ���
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // DrawMap - ������ ��� �� �� ����
                printf("%c%c", 219, 219);
                SetConsoleOutputCP(949);	// ����

                break;
            }
        }
        //�� �κ��� �ּ�ó���ϸ� �ִϸ��̼��� �����ϴ�.
        //Sleep(80);
    }

    int key = -1;
    int skip = 0;

    char message[20][1000] = { "���� ��ǥ : �� �߷��� ���� ���༺�� ����3 �༺�� �浹�� ����3�� �߷����� �������� ���ҽ��ϴ�.",
                                "�輼��(�÷��̾�)�� ���� ���� ��ֹ��� ���� ������(��)�� ������ Ż���ؾ� �մϴ�.",
                                "���۹�    : ��(��), ��(��), ��(��) ��ư�� �̿��� �÷��̾ �����Դϴ�.",
                               "��(��) ��ư�� ������ ���� �߷��� �ݴ� �������� �ٲ�ϴ�.",
                                "��(��) ��ư�� ������ �߷��� �������� 90�� �������ϴ�.",
                               "��(��) ��ư�� ������ �߷��� ���������� 90�� �������ϴ�.",
                               "���� ���� : ����(��,��), �(��),�� ������ ü��(��)�� �ϳ� ���Դϴ�.",
                               "�����Ÿ��� ����(��) �� 3�ʸ��� �ѹ��� �����Դϴ�.",
                               "��Ȧ(��)�� ������ �÷��̾�� �װ��� Ż���ؾ� �մϴ�.",
                               "� ���������� Ż���ϱ����� ����(��)�� �ʿ�� �� �� �ֽ��ϴ�.",
                               "��Ʈ(��)�� �÷��̾��� ü��(��)�� �ϳ� �������� �ݴϴ�."
    };
    int x = 0, y = 0;
    int tmp = 0;
    int state = 0;

    while (key == -1)
    {

        if (_kbhit() != 0)
        {
            key = _getch();
        }
        if (key != -1) return;

        if (state == 0)
        {
            SetCurrentCursorPos(36, 14);
            while (message[y][x] != NULL)
            {
                if (_kbhit() != 0)
                {
                    key = _getch();
                    skip = 1;
                }

                printf("%c", message[y][x]);
                if (skip == 0) Sleep(20);
                x++;
            }
            x = 0;
            y++;
            SetCurrentCursorPos(36 + 12, 16);
            while (message[y][x] != NULL)
            {
                if (_kbhit() != 0)
                {
                    key = _getch();
                    skip = 1;

                }


                if (message[y][x] == ')')
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                printf("%c", message[y][x]);
                if (message[y][x] == '(' && x > 10)
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);



                if (skip == 0) Sleep(20);
                x++;
            }

            x = 0;
            y++;
            SetCurrentCursorPos(36, 20);
            while (message[y][x] != NULL)
            {
                if (_kbhit() != 0)
                {
                    key = _getch();
                    skip = 1;
                }

                printf("%c", message[y][x]);
                if (skip == 0) Sleep(20);
                x++;
            }
            x = 0;
            y++;
            SetCurrentCursorPos(36 + 12, 22);
            while (message[y][x] != NULL)
            {
                if (_kbhit() != 0)
                {
                    key = _getch();
                    skip = 1;
                }

                printf("%c", message[y][x]);
                if (skip == 0) Sleep(20);
                x++;
            }
            x = 0;
            y++;
            SetCurrentCursorPos(36 + 12, 24);
            while (message[y][x] != NULL)
            {

                if (_kbhit() != 0)
                {
                    key = _getch();
                    skip = 1;
                }

                printf("%c", message[y][x]);
                if (skip == 0) Sleep(20);
                x++;
            }
            x = 0;
            y++;
            SetCurrentCursorPos(36 + 12, 26);
            while (message[y][x] != NULL)
            {
                if (_kbhit() != 0)
                {
                    key = _getch();
                    skip = 1;
                }

                printf("%c", message[y][x]);
                if (skip == 0) Sleep(20);
                x++;
            }
            x = 0;
            y++;
            SetCurrentCursorPos(36, 29);
            while (message[y][x] != NULL)
            {

                if (_kbhit() != 0)
                {
                    key = _getch();
                    skip = 1;
                }

                if (message[y][x] == ')')
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                printf("%c", message[y][x]);
                if (message[y][x] == '(' && x > 30)
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);

                if (skip == 0) Sleep(20);
                x++;
            }
            x = 0;
            y++;
            SetCurrentCursorPos(36 + 12, 31);
            while (message[y][x] != NULL)
            {
                if (_kbhit() != 0)
                {
                    key = _getch();
                    skip = 1;
                }

                printf("%c", message[y][x]);
                if (skip == 0) Sleep(20);
                x++;
            }
            x = 0;
            y++;
            SetCurrentCursorPos(36 + 12, 33);
            while (message[y][x] != NULL)
            {
                if (_kbhit() != 0)
                {
                    key = _getch();
                    skip = 1;
                }

                printf("%c", message[y][x]);
                if (skip == 0) Sleep(20);
                x++;
            }
            x = 0;
            y++;
            SetCurrentCursorPos(36 + 12, 35);
            while (message[y][x] != NULL)
            {

                if (_kbhit() != 0)
                {
                    key = _getch();
                    skip = 1;
                }

                if (message[y][x] == ')')
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                printf("%c", message[y][x]);
                if (message[y][x] == '(')
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);

                if (skip == 0) Sleep(20);
                x++;
            }
            x = 0;
            y++;

            SetCurrentCursorPos(36 + 12, 37);
            while (message[y][x] != NULL)
            {
                if (_kbhit() != 0)
                {
                    key = _getch();
                    skip = 1;

                }

                if (message[y][x] == ')')
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                printf("%c", message[y][x]);
                if (message[y][x] == '(')
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);


                if (skip == 0) Sleep(20);
                x++;
            }
            state = 1;
            key = -1;
        }



        /*
        SetCurrentCursorPos(36, 14);
       printf("���� ��ǥ : �� �� ���� ������ ���� ������ �߷��� ��Ʋ���� ���ҽ��ϴ�.");

        SetCurrentCursorPos(36+12, 16);
        //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        printf("�÷��̾�� ���� ���� ��ֹ��� ���� ������(��)�� ������ Ż���ؾ� �մϴ�.");
        //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        SetCurrentCursorPos(36 , 20);
        printf("���۹�    : ��(��), ��(��), ��(��) ��ư�� �̿��� �÷��̾ �����Դϴ�.");
        SetCurrentCursorPos(36 + 12, 22);
        printf("��(��) ��ư�� ������ ���� �߷��� �ݴ� �������� �ٲ�ϴ�.");
        SetCurrentCursorPos(36+ 12, 24);
        printf("��(��) ��ư�� ������ �߷��� �������� 90�� �������ϴ�.");
        SetCurrentCursorPos(36 + 12, 26);
        printf("��(��) ��ư�� ������ �߷��� �������� 90�� �������ϴ�.");
        SetCurrentCursorPos(36 , 29);
        printf("���� ���� : ����(��,��), �(��),�� ������ ü��(��)�� 1�ʿ� �ϳ��� ���Դϴ�.");
        SetCurrentCursorPos(36+12, 31);
        printf("�����Ÿ��� ����(��) �� 3�ʸ��� �ѹ��� �����Դϴ�.");
        SetCurrentCursorPos(36 + 12, 33);
        printf("��Ȧ(��)�� ������ �÷��̾�� ���� �ð� �ȿ� �װ��� Ż���ؾ� �մϴ�.");
        SetCurrentCursorPos(36 + 12, 35);
        printf("� ���������� Ż���ϱ����� ����(��)�� �ʿ�� �� �� �ֽ��ϴ�.");

        SetCurrentCursorPos(36 + 12, 37);
        printf("��ȣ��(��)�� �÷��̾��� ü��(��)�� �ϳ� �������� �ݴϴ�.");
        */

        if (tmp % 2 == 0)
        {
            SetCurrentCursorPos(38 * 2 - 19, GBOARD_HEIGHT - 8);
            printf("����Ϸ��� �ƹ� Ű�� �Է��ϼ���.");
        }
        else
        {
            SetCurrentCursorPos(38 * 2 - 19, GBOARD_HEIGHT - 8);
            printf("                                             ");
        }
        Sleep(800);
        tmp++;
    }
}
void GameFinishScreen()
{
    int key = -1;
    int skip = 0;
    int state = 0;
    int x = 0, y = 0;
    char message[20][1000] = { "�� ���� �÷��� �ð� : ", "���� : ", "�޼��� �������� : "
    };
    {

        for (int y = 0; y < GBOARD_HEIGHT; y++) {
            SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + y);
            for (int x = 0; x < GBOARD_WIDTH; x++) {

                if (_kbhit() != 0)
                {
                    key = _getch();
                    skip = 1;
                    key = -1;
                }
                // if (key != -1) return;

                if (succ == -1)
                {
                    switch (GameOverScreen[y][x]) {
                    case 0:	// �� ����
                        printf("  ");
                        break;
                    case 1:	// ���� ��

                        SetConsoleOutputCP(437);	// CP437: Ȯ�� �ƽ�Ű ��� ���
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // DrawMap - ������ ��� �� �� ����
                        printf("%c%c", 219, 219);
                        SetConsoleOutputCP(949);	// ����

                        break;
                    }
                }
                else
                {
                    switch (GameClearScreen[y][x]) {
                    case 0:	// �� ����
                        printf("  ");
                        break;
                    case 1:	// ���� ��

                        SetConsoleOutputCP(437);	// CP437: Ȯ�� �ƽ�Ű ��� ���
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // DrawMap - ������ ��� �� �� ����
                        printf("%c%c", 219, 219);
                        SetConsoleOutputCP(949);	// ����

                        break;
                    }
                }

            }
            //�� �κ��� �ּ�ó���ϸ� �ִϸ��̼��� �����ϴ�.
        //    if (skip == 0)
         //       Sleep(80);
        }



    }




    while (key == -1)
    {

        if (_kbhit() != 0)
        {
            key = _getch();
        }
        if (key != -1) break;

        if (state == 0)
        {
            SetCurrentCursorPos(36, 31);
            while (message[y][x] != NULL)
            {

                if (_kbhit() != 0)
                {
                    key = _getch();
                    skip = 1;
                }

                printf("%c", message[y][x]);

                if (skip == 0) Sleep(20);
                x++;
            }
            y++;
            x = 0;

            if (state == 0)
            {
                SetCurrentCursorPos(58, 31);
                printf("%02d:%02d", duration / 60, duration % 60);
            }

            SetCurrentCursorPos(36, 33);
            while (message[y][x] != NULL)
            {

                if (_kbhit() != 0)
                {
                    key = _getch();
                    skip = 1;
                }

                printf("%c", message[y][x]);

                if (skip == 0) Sleep(20);
                x++;
            }
            y++;
            x = 0;

            if (state == 0) {
                SetCurrentCursorPos(42, 33);

                if (succ == -1)
                {
                    printf("                 ����");
                }
                else {
                    if (duration <= 300)
                    {
                        printf("                    A");
                    }
                    else if (duration <= 390 && duration > 300)
                    {
                        printf("                    B");
                    }
                    else if (duration <= 480 && duration > 390)
                    {
                        printf("                    C");
                    }
                    else
                    {
                        printf("                    D");
                    }
                }
                SetCurrentCursorPos(36, 35);
                while (message[y][x] != NULL)
                {

                    if (_kbhit() != 0)
                    {
                        key = _getch();
                        skip = 1;
                    }

                    printf("%c", message[y][x]);

                    if (skip == 0) Sleep(20);
                    x++;
                }
                y++;
                x = 0;



                //�������� �޼� ���� Ȯ�� ��Ʈ
                if (succ == 1) {
                    if (duration <= 240) {  //4�� �̳��� Ŭ����
                        In4Min = 1;
                        acheivecnt++;
                    }
                    if (health == 3) {  //��� 3���� Ŭ����
                        In3Life = 1;
                        acheivecnt++;
                    }
                    if (InBlackHoleAddict == 0) // ��Ȧ�� �ѹ��� �Ⱥ����� Ŭ����
                    {
                        InNoBlackHole = 1;
                        acheivecnt++;
                    }
                    if (health == 3 && duration <= 240) //4�� �̳��� 3�� ������� Ŭ���� 
                    {
                        InProgamer = 1;
                        acheivecnt++;
                    }
                    if (health == 1 && duration > 480) { // ��� 1������ D��ũ�� Ŭ����
                        InDrank1Life = 1;
                        acheivecnt++;
                    }
                    if (mapNum >= 2)  //�������� 1 Ŭ����
                    {
                        InStage1clear = 1;
                        acheivecnt++;
                    }

                    if (mapNum >= 4)  //�������� 4 Ŭ����
                    {
                        InStage4clear = 1;
                        acheivecnt++;
                    }


                    if (InBlackHoleAddict >= 3) //��Ȧ 3�� �̻� Ŭ����
                    {
                        acheivecnt++;
                    }
                    if (InBlackHoleAddict >= 3 && duration > 480) // D��ũ + ��Ȧ 3�� �̻� Ŭ����
                    {
                        InExplorer = 1;
                        acheivecnt++;
                    }
                }
                SetCurrentCursorPos(59, 35);
                printf("%d ��", acheivecnt);


                int i = 0;


                SetCurrentCursorPos(80, 31 + 2 * i);

                if (succ == 1) {
                    if (In4Min == 1)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("�������� ������ - 4�� �̳��� Ŭ����");
                        i++;
                    }
                    if (In3Life == 1)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("������ ������ - ü�� 3�� ������ä�� Ŭ����");
                        i++;
                    }
                    if (InBlackHoleAddict == 0)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("��Ȧ ����� - ��Ȧ�� �ѹ��� ������ ����");
                        i++;
                    }
                    if (InProgamer == 1)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("���ΰ��̸� - �������� ������ + ������ ������ �޼�");
                        i++;
                    }
                    if (InDrank1Life == 1)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("�ƽ��ƽ� - ü��1���� D��ũ�� Ŭ����");
                        i++;
                    }
                    if (InStage1clear == 1)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("õ���浵 �Ѱ������� - �������� 1 Ŭ����");
                        i++;
                    }
                    if (InBlackHoleAddict >= 3)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("��Ȧ �ߵ��� - ��Ȧ�� 3�� �̻� ����");
                        i++;
                    }
                    if (InExplorer == 1)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("ȣ��� ���� Ž�谡 - ��Ȧ �ߵ��� + D��ũ �޼�");
                        i++;
                    }
                    if (InStage4clear == 1)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("������ ������ ! - �������� 4 Ŭ����");
                        i++;
                    }


                }
            }

            state = 1;
            key = -1;
        }



    }


}

//��� ���õ� ������

//��� ������ǥ
int obstacleCoord[][4][2] = {


    { //��Ȧ �������� � ����
        {0},
        {0},
        {0}
    },
    { //��������1 � ����
        {0},
        {0},
        {0}
    },

    { //�������� 2
        {11,38}, //27~39  ������ǥ (11, 39)~
        {28,27}, //27~43  ������ǥ (28, 27)~
        {37,30} //27~39   ������ǥ (37, 30)~
    },

    { //�������� 3
        {24, 17}, //7~18����
        {24, 51}, //51~63����
        {32,44},
        {42,46}
    },

    { // �������� 4 // 12.5�߰�
        {17, 30},
        {14, 40}
    }
};

//��̵����� ���� 0, ���� 1, �Ʒ� 2, �� 3
int isRight[][4] = { //��� ������������ -1
    {-1,-1,-1,-1},  // ��Ȧ
    {-1,-1,-1,-1},  // �������� 1
    {0,1,1,0},      // �������� 2
    {0,1,2,3},      // �������� 3
    // 12.5 �߰� (Ȳ����)
    {1,0,0,0}     // �������� 4
};

// � �̵�
void moveObstacle() {

    COORD temp = GetCurrentCursorPos();

    int n = 1;
    if (mapNum == 2) n = 3;
    else if (mapNum == 3) n = 4;
    // 12.5 �߰� (Ȳ����)
    else if (mapNum == 4) n = 2;

    for (int i = 0; i < n; i++) {

        int dx[4] = { 0,0,1,1 };
        int dy[4] = { 0,1,0,1 };

        if (mapInfo[mapNum][obstacleCoord[mapNum][i][0]][obstacleCoord[mapNum][i][1]] != 6) continue;

        for (int j = 0; j < 4; j++)
            mapInfo[mapNum][obstacleCoord[mapNum][i][0] + dx[j]][obstacleCoord[mapNum][i][1] + dy[j]] = 0;



        if (isRight[mapNum][i] == 1) obstacleCoord[mapNum][i][1]++;
        else if (isRight[mapNum][i] == 0)obstacleCoord[mapNum][i][1]--;
        else if (isRight[mapNum][i] == 2) obstacleCoord[mapNum][i][0]++; //�Ʒ� ����
        else if (isRight[mapNum][i] == 3) obstacleCoord[mapNum][i][0]--; //�� ����

        for (int j = 0; j < 4; j++)
            mapInfo[mapNum][obstacleCoord[mapNum][i][0] + dx[j]][obstacleCoord[mapNum][i][1] + dy[j]] = 6;



    }

    for (int i = 0; i < n; i++) {

        if (mapInfo[mapNum][obstacleCoord[mapNum][i][0]][obstacleCoord[mapNum][i][1]] == 6) {

            if (isRight[mapNum][i] == 1) {

                int dx[4] = { -1,0,-1,0 };
                int dy[4] = { 0,0,1,1 };

                for (int j = 0; j < 4; j++) {
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1] + dx[j]) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] + dy[j]);
                    printf("  ");
                }


                for (int j = 0; j < 4; j++) {
                    dx[j]++;
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1] + dx[j]) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] + dy[j]);
                    printf("��");
                }

            }
            else if (isRight[mapNum][i] == 0) {

                int dx[4] = { 1,2,1,2 };
                int dy[4] = { 0,0,1,1 };

                for (int j = 0; j < 4; j++) {
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1] + dx[j]) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] + dy[j]);
                    printf("  ");
                }

                for (int j = 0; j < 4; j++) {
                    dx[j]--;
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1] + dx[j]) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] + dy[j]);
                    printf("��");
                }
            }
            else if (isRight[mapNum][i] == 2) {
                for (int j = 0; j < 4; j++) {
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1]) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0]);
                    printf("  ");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1] + 1) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0]);
                    printf("  ");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1]) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] - 1);
                    printf("  ");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1] + 1) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] - 1);
                    printf("  ");
                }

                for (int j = 0; j < 4; j++) {
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1]) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0]);
                    printf("��");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1]) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] + 1);
                    printf("��");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1] + 1) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0]);
                    printf("��");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1] + 1) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] + 1);
                    printf("��");
                }
            }
            else if (isRight[mapNum][i] == 3) {
                for (int j = 0; j < 4; j++) {
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1]) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] + 1);
                    printf("  ");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1] + 1) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] + 1);
                    printf("  ");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1]) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] + 2);
                    printf("  ");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1] + 1) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] + 2);
                    printf("  ");
                }

                for (int j = 0; j < 4; j++) {
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1]) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0]);
                    printf("��");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1]) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] + 1);
                    printf("��");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1] + 1) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0]);
                    printf("��");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1] + 1) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] + 1);
                    printf("��");
                }
            }

        }
    }


    for (int i = 0; i < n; i++) {

        switch (mapNum)
        {
        case 2:
            //���� ������� ���� �ٲ��ֱ�
            if ((i != 1 && obstacleCoord[mapNum][i][1] == 38) || obstacleCoord[mapNum][i][1] == 43)    isRight[mapNum][i] = 0;
            else if (obstacleCoord[mapNum][i][1] == 27)  isRight[mapNum][i] = 1;
            break;
        case 3:
            if (obstacleCoord[mapNum][i][1] == 17 || obstacleCoord[mapNum][i][1] == 62)    isRight[mapNum][i] = 0;
            else if (obstacleCoord[mapNum][i][1] == 7 || obstacleCoord[mapNum][i][1] == 51)  isRight[mapNum][i] = 1;
            else if (obstacleCoord[mapNum][i][0] == 30) isRight[mapNum][i] = 2;
            else if (obstacleCoord[mapNum][i][0] == 42) isRight[mapNum][i] = 3;


        case 4: // 12.5 �߰� (Ȳ����)
            if (obstacleCoord[mapNum][i][1] == 28) isRight[mapNum][i] = 1;
            else if (obstacleCoord[mapNum][i][1] == 42) isRight[mapNum][i] = 0;

        }


    }

    SetCurrentCursorPos(temp.X, temp.Y);

}


void CollisionBlackhole() {
    deleteBar();    // check
    PlaySound(TEXT(BGM_BLACKHOLE), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

    InBlackHoleAddict++; // ��Ȧ ���� ������Ʈ

    isBlackhole = 1;//���� ��Ȧ ���°� ��

    //�߷� ������ �Ʒ��������� �ʱ�ȭ
    gravity = 2;
    block_id = 2;

    //��Ȧ ������������ PC��ġ �ʱ�ȭ
    curPosX = pcStartPos[0].X * 2;
    curPosY = pcStartPos[0].Y;

    //�� �������� ����
    tempmapNum = mapNum;
    mapNum = 0;

    //��Ȧ �� �׸���
    drawBar();  // check
    DrawMap();
    SetCurrentCursorPos(curPosX, curPosY);
}

//� �浹
void CollisionMeteorite(int x, int y) {

    isObstacleCollision = 1; //��� �浹��
    slowSpeedTime = 1;//�ӵ� ���� �ð� �ʱ�ȭ
    speed = 50; //�ӵ� ����


    health--;
    GameInfoUpdate();

    COORD temp = GetCurrentCursorPos();

    //� �����
    mapInfo[mapNum][x][y] = 0;
    SetCurrentCursorPos(GBOARD_ORIGIN_X + y * 2, GBOARD_ORIGIN_Y + x);
    printf("  ");

    int dx[8] = { -1,1,0,0,-1,1,-1,1 };
    int dy[8] = { 0,0,-1,1,1,-1,-1,1 };



    for (int i = 0; i < 8; i++) {
        if (mapInfo[mapNum][x + dx[i]][y + dy[i]] == 6) {
            mapInfo[mapNum][x + dx[i]][y + dy[i]] = 0;
            SetCurrentCursorPos(GBOARD_ORIGIN_X + (y + dy[i]) * 2, GBOARD_ORIGIN_Y + x + dx[i]);
            printf("  ");
        }

    }

    SetCurrentCursorPos(temp.X, temp.Y);

}

//������ ����
int CollisionDest() {

    //Ű ���� ȹ���ߴ°� 11.24 �߰�
    if (stageKey[mapNum] > 0) {//Ű�� �� �ȸԾ����� ����
        return -1;
    }


    //��Ȧ ������ ���
    if (isBlackhole == 1) {
        isBlackhole = 0;
        mapNum = tempmapNum;
        if (mapNum == 4) { PlaySound(TEXT(BGM_MAIN1), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); }
        else { PlaySound(TEXT(BGM_MAIN2), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); }
    }
    //��Ȧ ���°� �ƴ�
    else {

        succ = 1;
        mapNum++;

        if (mapNum > 4) {
            deleteBar();
            return 0;
        }

    }

    deleteBar();
    DrawMap();

    //���������� �ʱ⼼��
    switch (mapNum)
    {
    case 2:
        curPosX = pcStartPos[2].X * 2;
        curPosY = pcStartPos[2].Y;
        gravity = 2;
        block_id = 2;
        SetCurrentCursorPos(curPosX, curPosY);
        break;
    case 3:
        curPosX = pcStartPos[3].X * 2;
        curPosY = pcStartPos[3].Y;
        gravity = 2;
        block_id = 2;
        SetCurrentCursorPos(curPosX, curPosY);
        break;
    case 4: // 12.5 �߰� (Ȳ����)
        PlaySound(TEXT(BGM_MAIN1), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        curPosX = pcStartPos[4].X * 2;
        curPosY = pcStartPos[4].Y;
        gravity = 2;
        block_id = 2;
        SetCurrentCursorPos(curPosX, curPosY);
    }
    drawBar();//12.03 �߰�

}
void drawBar() {

    COORD temp;
    temp.X = curPosX;
    temp.Y = curPosY;
    int x, y;
    //10 =�ʷ� 11=�ϴ� 12=���� 13=��ũ 14=��� 15=�Ͼ�
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
    switch (gravity) {
    case GDOWN: //2
        for (x = 0; x < GBOARD_WIDTH; x++) {
            SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT);
            printf("��");
        }
        break;
    case GRIGHT: //1
        for (y = 0; y < GBOARD_HEIGHT; y++) {
            SetCurrentCursorPos(GBOARD_ORIGIN_X + GBOARD_WIDTH * 2, GBOARD_ORIGIN_Y + y);
            printf("��");
        }
        break;
    case GUP: //0
        for (x = 0; x < GBOARD_WIDTH; x++) {
            SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y - 1);
            printf("��");
        }
        break;
    case GLEFT: //3
        for (y = 0; y < GBOARD_HEIGHT; y++) {
            SetCurrentCursorPos(GBOARD_ORIGIN_X - 2, GBOARD_ORIGIN_Y + y);
            printf("��");
        }
        break;
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    SetCurrentCursorPos(temp.X, temp.Y);
}
void deleteBar() {
    COORD temp;
    temp.X = curPosX;
    temp.Y = curPosY;
    int x, y;
    switch (gravity) {
    case GDOWN: //2
        for (x = 0; x < GBOARD_WIDTH; x++) {
            SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT);
            printf("  ");
        }
        break;
    case GRIGHT: //1
        for (y = 0; y < GBOARD_HEIGHT; y++) {
            SetCurrentCursorPos(GBOARD_ORIGIN_X + GBOARD_WIDTH * 2, GBOARD_ORIGIN_Y + y);
            printf("  ");
        }
        break;
    case GUP: //0
        for (x = 0; x < GBOARD_WIDTH; x++) {
            SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y - 1);
            printf("  ");
        }
        break;
    case GLEFT: //3
        for (y = 0; y < GBOARD_HEIGHT; y++) {
            SetCurrentCursorPos(GBOARD_ORIGIN_X - 2, GBOARD_ORIGIN_Y + y);
            printf("  ");
        }
        break;
    }
    SetCurrentCursorPos(temp.X, temp.Y);
}