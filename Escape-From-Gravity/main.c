#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include "gameboard.h"
#include <process.h>    // 멀티스레드 처리 추가
#include <mmsystem.h>   // 사운드 추가
#pragma comment(lib,"winmm.lib")
// 메인 브금
#define BGM_MAIN1 "sound/questiion_lost moons-greatful eights.wav"
#define BGM_MAIN2 "sound/questiion_lost moons-serious as an attack button.wav"
#define BGM_MAIN3 "sound/zagi2_bodenhacke loop.wav"
// 블랙홀 브금
#define BGM_BLACKHOLE "sound/zagi2_space serpent.wav"


#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define SPACE 32
#define CTRL 17

/*게임보드 크기*/
#define GBOARD_WIDTH 70
#define GBOARD_HEIGHT 50

/*게임보드 시작점*/
#define GBOARD_ORIGIN_X 4
#define GBOARD_ORIGIN_Y 2

/*중력의 방향*/
#define GUP 0
#define GRIGHT 1
#define GDOWN 2
#define GLEFT 3

char blockModel[][3][3] =
{
    /* 플레이어
        ◎ 97
        △ 98
        ∥ 99
       ＝   ▷ ◎
        -67 -71 97
       ◎  ◁  ＝
           -73
        ∥ 99
        ▽-28
        ◎ 97
        */

        //플레이어의 머리 부분이 중력의 방향

        //중력이 위쪽으로 작용할때  0
        {
            {0, 99, 0 },
            {0, 98, 0 },
            {0, 97, 0 },
        }

        ,
    //중력이 오른쪽으로 작용할때  1
    {
        {0, 0, 0 },
        {97,-71,-67},
        {0, 0, 0 },
    }

     ,
    //중력이 아래쪽으로 작용할때  2
    {
        {0, 97, 0 },
        {0, -28, 0 },
        {0, 99, 0 },
    }
  ,
    //중력이 왼쪽으로 작용할때  3
    {
        {0, 0, 0 },
        {-67,-73,97},
        {0, 0, 0 },
    }


};

int curPosX = 0, curPosY = 0;
int block_id; //현재 PC모습
int speed; //속도
int score = 0; //점수 ?
int mission;
int gravity = 0; //중력방향
int health = 3; //체력
clock_t start, end; //시간
int duration;
int min;
int sec;
int detX, detY;//벽에 붙어있을 경우 옮겨주기 위함
int succ = 0; //스테이지 클리어 여부
int clearflag = 0;// 가시를 안보이게 할지 보이게 할지 결정
int periodflag = 0;//투명가시 on/off 용
int obstacleSpeed = 0; //운석 이동 제어
int isBlackhole = 0; //블랙홀 상태
int mapNum = 1; //스테이지를 나타내는 변수
int tempmapNum; //블랙홀 상태일 때 현 스테이지를 임시저장
int stageKey[10] = { 0,0,0,2,3,3,0,0,0,0 };// 스테이지별 키 개수 조정 가능

int acheivecnt = 0;
int In4Min = 0; // 4분 이내로 클리어
int In3Life = 0; // 목숨 3개로 클리어
int InNoBlackHole = 0; // 블랙홀에 한번도 안빠지고 클리어
int InProgamer = 0; // 4분 이내에 3개 목숨으로 클리어
int InDrank1Life = 0; // 목숨 1개남고 D랭크로 클리어
int InStage1clear = 0; //스테이지 1 클리어
int InStage4clear = 0; //스테이지 4 클리어
int InStage5clear = 0; //스테이지 5 클리어
int InBlackHoleAddict = 0; //블랙홀 3번 이상 클리어
int InExplorer = 0;  // D랭크 + 블랙홀 3번 이상 클리어
int isObstacleCollision = 0; //운석과 충돌 여부
int slowSpeedTime = 0; //PC속도 저하시간 제어
int gravitytemp = 0;

void SetCurrentCursorPos(int x, int y); //커서설정
COORD GetCurrentCursorPos(void);//현재커서 가져오기
void RemoveCursor(void);//커서지우기
void GameInfoInit(); //게임info 초기화
void GameInfoUpdate(); //게임info 업데이트
void DrawMap(); //맵그리기
void ShowBlock(char blockInfo[3][3]); //PC출력
void DeleteBlock(char blockInfo[3][3]); //PC지우기
int DetectCollision(int posX, int posY, char blockModel[3][3]); //PC와 NPC충돌 체크
void CollisionThorn(); //가시와 충돌처리
void RedrawBlocks(char blockInfo[3][3]); //맵 다시 그리기
int ShiftRight(); //중력방향 오른쪽
int ShiftLeft(); //왼쪽
int ShiftUp(); //위
int ShiftDown(); //아래
int IsGameOver(); //게임 종료 체크
void RotateBlock(); //PC회전
int ProcessKeyInput(); //key입력받기
void InitialScreen(); //초기 시작화면
void SecondScreen(); //게임설명 및 방법 화면
void GameFinishScreen(); //게임 종료화면
void moveObstacle(); //운석이동
void CollisionBlackhole(); //블랙홀 처리
int CollisionDest(); //목적지 충돌
void ClearThorn();//투명가시 조절
void CollisionPotion();//포션 발동
void CollisionMeteorite(int x, int y); //운석 충돌
void CollisionKey(); //키 충돌 
void drawBar();//중력바
void deleteBar();//중력바 제거
COORD pcStartPos[5] = {
   {6,37},      // 0: blackhole
   {7,5},       // 1: map 1
   {11,25},     // 2: map 2
   {13,40},     // 3: map 3
   {3,3},       // 4: map 4
};


unsigned _stdcall Thread_A(void* arg)//투명가시 주기 카운팅을 위한 스레드a
{
    while (1)
    {
        Sleep(3000);//3초 지나면
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
    
    //화면 구현부
    InitialScreen();
    SecondScreen();

    PlaySound(TEXT(BGM_MAIN2), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

    gravity = 2;
    block_id = 2;

    curPosX = pcStartPos[mapNum].X * 2;
    curPosY = pcStartPos[mapNum].Y;
    SetCurrentCursorPos(curPosX, curPosY);

    _beginthreadex(NULL, 0, Thread_A, 0, 0, NULL);//스레드a 시작

    while (1)
    {
        start = clock();
        DrawMap();
        drawBar();
        GameInfoInit();


        //테스트를 위해서 게임종료 적용 X

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

            //투명가시 
            if (periodflag == 1) {
                ClearThorn();
                periodflag = 0;
            }

            RedrawBlocks(blockModel[block_id]);

            //나머지연산은 운석의 속도를 제어
            //스테이지 2 ~ 운석 존재
            if (obstacleSpeed % 3 == 0 && mapNum >= 2)
            {
                obstacleSpeed = 0;
                moveObstacle(); //운석 이동
            }


            //운석과 충돌이후 속도저하 해제
            //나머지 연산을 통해 속도저하 시간 제어
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

// 투명가시 함수 추가
void ClearThorn() {

    COORD temp = GetCurrentCursorPos();
    int collision;
    if (clearflag == 0) {
        for (int y = 0; y < GBOARD_HEIGHT; y++) {

            for (int x = 0; x < GBOARD_WIDTH; x++) {
                if (mapInfo[mapNum][y][x] == 7) {// 투명가시의 위치를 찾고 지워준 다음 맵 정보를 -7로 바꿈
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y + y);
                    mapInfo[mapNum][y][x] = -7;
                    printf("  ");
                }
            }
        }
        clearflag = 1;// 다음 차례가 가시를 보여줄 차례라는 것을 알려줌
    }
    else if (clearflag == 1) {
        int flag = 0;
        collision = 0;
        for (int y = 0; y < GBOARD_HEIGHT; y++) {

            for (int x = 0; x < GBOARD_WIDTH; x++) {
                if (mapInfo[mapNum][y][x] == -7) {//-7이 된 가시의 흔적을 찾아서 다시 가시를 보여줌
                    mapInfo[mapNum][y][x] = 7;
                    if (DetectCollision(curPosX, curPosY, blockModel[block_id]) == 7) {//제자리에 서있다가 가시피격당하는 경우
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
                        printf("△");
                    }
                    else if (mapInfo[mapNum][y - 1][x] == 1 && mapInfo[mapNum][y + 1][x] == 0) {
                        printf("▽");
                    }
                    else if (mapInfo[mapNum][y][x - 1] == 1 && mapInfo[mapNum][y][x + 1] == 0) {
                        printf("▷");
                    }
                    else if (mapInfo[mapNum][y][x - 1] == 0 && mapInfo[mapNum][y][x + 1] == 1) {
                        printf("◁");
                    }
                }
            }
        }
        if (collision == 1) {
            SetCurrentCursorPos(curPosX, curPosY);
            CollisionThorn();
        }
        clearflag = 0;// 다음 차례가 가시를 안보이게 할 차례라는 것을 알려줌
    }
    SetCurrentCursorPos(curPosX, curPosY);
}


void DrawMap() {

    for (int y = 0; y < GBOARD_HEIGHT; y++) {
        SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + y);
        for (int x = 0; x < GBOARD_WIDTH; x++) {
            switch (mapInfo[mapNum][y][x]) {
            case 0:	// 빈 공간
                printf("  ");
                break;

            case -7: // 11.25 추가
                printf("  ");
                break;
            case 1:	// 고정 벽

                SetConsoleOutputCP(437);	// CP437: 확장 아스키 모드 사용
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // DrawMap - 꽉찬벽 사용 시 색 설정
                printf("%c%c", 219, 219);
                SetConsoleOutputCP(949);	// 복구
                break;
            case 2: // 목적지
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
                printf("★");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                break;
            case 3: // 가시

                if (x - 1 >= 0 && x + 1 <= GBOARD_WIDTH - 1) {

                    if ((mapInfo[mapNum][y][x - 1] == 0) && (mapInfo[mapNum][y][x + 1] == 1)) {
                        printf("◀");
                        break;
                    }
                    if ((mapInfo[mapNum][y][x + 1] == 0) && (mapInfo[mapNum][y][x - 1] == 1)) {
                        printf("▶");
                        break;
                    }
                }
                if (y - 1 >= 0 && y + 1 <= GBOARD_HEIGHT - 1) {

                    if ((mapInfo[mapNum][y - 1][x] == 0) && (mapInfo[mapNum][y + 1][x] == 1)) {
                        printf("▲");
                        break;
                    }
                    if ((mapInfo[mapNum][y - 1][x] == 1) && (mapInfo[mapNum][y + 1][x] == 0)) {
                        printf("▼");
                        break;
                    }
                }
            case 4: // 물약
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
                printf("♥");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                break;
            case 5: // 블랙홀
                printf("＠");

                break;
            case 6: // 운석
                //printf("▣");
                printf("  ");
                break;
            case 7: // 투명 가시

                if (x - 1 >= 0 && x + 1 <= GBOARD_WIDTH - 1) {

                    if ((mapInfo[mapNum][y][x - 1] == 0) && (mapInfo[mapNum][y][x + 1] == 1)) {
                        printf("◁");
                        break;
                    }
                    if ((mapInfo[mapNum][y][x + 1] == 0) && (mapInfo[mapNum][y][x - 1] == 1)) {
                        printf("▷");
                        break;
                    }
                }
                if (y - 1 >= 0 && y + 1 <= GBOARD_HEIGHT - 1) {

                    if ((mapInfo[mapNum][y - 1][x] == 0) && (mapInfo[mapNum][y + 1][x] == 1)) {
                        printf("△");
                        break;
                    }
                    if ((mapInfo[mapNum][y - 1][x] == 1) && (mapInfo[mapNum][y + 1][x] == 0)) {
                        printf("▽");
                        break;
                    }
                }
            case 8: // 키
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
                printf("ⓚ");
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
                printf("◎");
            if (blockInfo[y][x] == 98)
                printf("△");
            if (blockInfo[y][x] == 99)
                printf("∥");
            if (blockInfo[y][x] == -71)
                printf("▷");
            if (blockInfo[y][x] == -67)
                printf("＝");
            if (blockInfo[y][x] == -28)
                printf("▽");
            if (blockInfo[y][x] == -73)
                printf("◁");

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

            //물약 충돌 추가
            if (mapInfo[mapNum][arrY + y][arrX + x] == 4 && blockModel[y][x] != 0) {
                mapInfo[mapNum][arrY + y][arrX + x] = 0;// 물약 먹은걸 맵정보에 적용
                CollisionPotion();
                return 4;
            }
            //벽과 충돌
            if (mapInfo[mapNum][arrY + y][arrX + x] == 1 && blockModel[y][x] != 0) {
                detX = x;
                detY = y;

                return 0;
            }
            //가시와 충돌 수정
            if (mapInfo[mapNum][arrY + y][arrX + x] == 3 && blockModel[y][x] != 0) {
                detX = x;
                detY = y;
                return 3;
            }
            //투명 가시와 충돌 추가
            if (mapInfo[mapNum][arrY + y][arrX + x] == 7 && blockModel[y][x] != 0) {
                detX = x;
                detY = y;
                return 7;
            }
            //
            //운석과 충돌
            if (mapInfo[mapNum][arrY + y][arrX + x] == 6 && blockModel[y][x] != 0) {
                CollisionMeteorite(arrY + y, arrX + x);
                return 6;
            }


            //목적지와 충돌
            if (mapInfo[mapNum][arrY + y][arrX + x] == 2 && blockModel[y][x] != 0) {
                succ = 1;
                return 2;
            }

            //운석과 충돌
            if (mapInfo[mapNum][arrY + y][arrX + x] == 6 && blockModel[y][x] != 0) {
                CollisionMeteorite(arrY + y, arrX + x);
                return 6;
            }
            //블랙홀과 충돌
            if (mapInfo[mapNum][arrY + y][arrX + x] == 5 && blockModel[y][x] != 0) {

                return 5;
            }
            //키 충돌 추가 11.24
            if (mapInfo[mapNum][arrY + y][arrX + x] == 8 && blockModel[y][x] != 0) {
                mapInfo[mapNum][arrY + y][arrX + x] = 0;//키를 맵정보 에서 지움
                CollisionKey();
                return 8;
            }
        }
    }


    return 1;
}


void CollisionThorn() {
    health--; //체력 -1
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

        //11.27수정 Sleep(300)두번을 다음과 같이 바꿈
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

void CollisionPotion() {//포션함수

    health++;
    if (health > 3) {//최대체력
        health = 3;
    }
    GameInfoUpdate();
}
//11.24 함수 추가
void CollisionKey() {
    stageKey[mapNum]--;
    if (stageKey[mapNum] <= 0) {//최소 0개
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

    //운석과 충돌
    if (DetectCollision(curPosX, curPosY, blockModel[block_id]) == 6) {
        return 0;
    }

    //벽과 충돌
    if (DetectCollision(curPosX + 2, curPosY, blockModel[block_id]) == 0)
        return 0;
    //가시와 충돌
    if (DetectCollision(curPosX + 2, curPosY, blockModel[block_id]) == 3) {
        CollisionThorn();
        return 0;
    }

    //목적지와 충돌
    if (DetectCollision(curPosX + 2, curPosY, blockModel[block_id]) == 2) {
        CollisionDest();
        return 0;
    }

    //블랙홀과 충돌
    if (DetectCollision(curPosX + 2, curPosY, blockModel[block_id]) == 5) {
        CollisionBlackhole();
        return 0;
    }

    // 투명 가시와 충돌 추가
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

    //운석과 충돌
    if (DetectCollision(curPosX, curPosY, blockModel[block_id]) == 6) {
        return 0;
    }

    //벽과 충돌
    if (DetectCollision(curPosX - 2, curPosY, blockModel[block_id]) == 0)
        return 0;

    //가시와 충돌
    if (DetectCollision(curPosX - 2, curPosY, blockModel[block_id]) == 3) {
        CollisionThorn();
        return 0;
    }

    //목적지와 충돌
    if (DetectCollision(curPosX - 2, curPosY, blockModel[block_id]) == 2) {

        CollisionDest();

        return 0;
    }

    // 투명 가시와 충돌 추가
    if (DetectCollision(curPosX - 2, curPosY, blockModel[block_id]) == 7)
    {
        CollisionThorn();
        return 0;
    }

    //블랙홀과 충돌
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

    //운석과 충돌
    if (DetectCollision(curPosX, curPosY, blockModel[block_id]) == 6) {
        return 0;
    }

    //벽과 충돌
    if (DetectCollision(curPosX, curPosY - 1, blockModel[block_id]) == 0)
        return 0;

    // 가시와 충돌
    if (DetectCollision(curPosX, curPosY - 1, blockModel[block_id]) == 3)
    {
        CollisionThorn();
        return 0;
    }

    //블랙홀과 충돌
    if (DetectCollision(curPosX, curPosY - 1, blockModel[block_id]) == 5) {
        CollisionBlackhole();
        return 0;
    }
    // 투명 가시와 충돌
    if (DetectCollision(curPosX, curPosY - 1, blockModel[block_id]) == 7)
    {
        CollisionThorn();
        return 0;
    }



    //목적지와 충돌
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

    //운석과 충돌
    if (DetectCollision(curPosX, curPosY, blockModel[block_id]) == 6) {
        return 0;
    }


    //벽과 충돌
    if (DetectCollision(curPosX, curPosY + 1, blockModel[block_id]) == 0)
        return 0;

    // 가시와 충돌
    if (DetectCollision(curPosX, curPosY + 1, blockModel[block_id]) == 3)
    {
        CollisionThorn();
        return 0;
    }

    // 투명 가시와 충돌
    if (DetectCollision(curPosX, curPosY + 1, blockModel[block_id]) == 7)
    {
        CollisionThorn();
        return 0;
    }

    //목적지와 충돌
    if (DetectCollision(curPosX, curPosY + 1, blockModel[block_id]) == 2) {
        CollisionDest();
        return 0;
    }

    //블랙홀과 충돌
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
    //게임종료조건
    //1. 생명이 0인 경우



    //생명 0인 경우
    if (health == 0) {
        succ = -1;
        return 1;
    }

    if (mapNum > 4) return 1;

    return 0;

}

void RotateBlock() {
    int colflag = 0;//충돌여부
    int block_rotated = gravity;
    if (DetectCollision(curPosX, curPosY, blockModel[block_rotated]) == 0) {
        colflag = 1;
    }
    else if (DetectCollision(curPosX, curPosY, blockModel[block_rotated]) == 3) {//가시
        colflag = 2;
    }
    else if (DetectCollision(curPosX, curPosY, blockModel[block_rotated]) == 7) {//투명가시
        colflag = 2;
    }
    if (DetectCollision(curPosX, curPosY, blockModel[block_rotated]) == 2) {
        //3스테이지용 임시기능
        if (CollisionDest() == -1) {//회전시 목적지에 닿지만 키를 다 못먹은 경우 
            gravity--;//회전 불가판정
            drawBar();
            return;
        }
    }
    if (DetectCollision(curPosX, curPosY, blockModel[block_rotated]) == 5) {//회전시 블랙홀에 닿는경우
        CollisionBlackhole();
    }
    //추가끝//

    DeleteBlock(blockModel[block_id]);
    if (colflag == 1 || colflag == 2) {// 가시나 벽에 붙어서 이동하는 경우 충돌이 일어나기 때문에 좌표를 옮겨줌
        int detobj;// 이중충돌 물체파악1203
        if (detX == 1 && detY == 2) {
            curPosY -= 1;
            detobj = DetectCollision(curPosX, curPosY, blockModel[block_rotated]);
            if (detobj != 1) {//양쪽이 물체인 경우 회전 불가하게 1203
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
            if (detobj != 1) {//양쪽이 물체인 경우 회전 불가하게 1203
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
            if (detobj != 1) {//양쪽이 물체인 경우 회전 불가하게 1203
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
            if (detobj != 1) {//양쪽이 물체인 경우 회전 불가하게 1203
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


// 화면 구현부
void GameInfoInit()
{
    COORD curPos = GetCurrentCursorPos();

    //선 긋기
    for (int i = 2; i < GBOARD_HEIGHT + GBOARD_ORIGIN_Y; i++)
    {
        SetCurrentCursorPos(GBOARD_WIDTH * 2 + GBOARD_ORIGIN_X * 2, i);
        printf("│\n");
    }


    //체력
    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 24, GBOARD_ORIGIN_Y);
    printf("-HEALTH-");

    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 25, GBOARD_ORIGIN_Y + 3);
    for (int i = 0; i < health; i++) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        printf("♥");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    }

    //시간
    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 25, GBOARD_ORIGIN_Y + 6);
    printf("-TIME-");
    duration = (end - start) / CLOCKS_PER_SEC;
    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 25, GBOARD_ORIGIN_Y + 9);
    printf("%02d:%02d", duration / 60, duration % 60);

    //커서 되돌려놓기
    SetCurrentCursorPos(curPos.X, curPos.Y);
}
void GameInfoUpdate()
{
    COORD curPos = GetCurrentCursorPos();
    //체력

    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 25, GBOARD_ORIGIN_Y + 3);
    for (int i = 0; i < 3; i++)
        printf("  "); //원래 있던 체력 정보를 지운다.
    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 25, GBOARD_ORIGIN_Y + 3);
    for (int i = 0; i < health; i++) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        printf("♥");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    }
    //시간

    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 25, GBOARD_ORIGIN_Y + 9);
    printf("           ");
    duration = (end - start) / CLOCKS_PER_SEC;
    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 25, GBOARD_ORIGIN_Y + 9);
    printf("%02d:%02d", duration / 60, duration % 60);


    //키 획득 여부 임시추가 11.24
    SetCurrentCursorPos(GBOARD_WIDTH * 2 + 22, GBOARD_ORIGIN_Y + 13);
    printf("남은");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
    printf(" Key");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    printf(": %d", stageKey[mapNum]);
    //커서 되돌려놓기
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
            case 0:	// 빈 공간
                printf("  ");
                break;
            case 1:	// 고정 벽

                SetConsoleOutputCP(437);	// CP437: 확장 아스키 모드 사용
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // DrawMap - 꽉찬벽 사용 시 색 설정
                printf("%c%c", 219, 219);
                SetConsoleOutputCP(949);	// 복구
                break;
            }
        }
        //이 부분을 주석처리하면 애니메이션이 꺼집니다.
        if (skip == 0)
            Sleep(80);
    }


    SetCurrentCursorPos(52, GBOARD_HEIGHT - 6);
    printf("Copyrightⓒ2021 SW18 All rights reserved.");


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
            printf("계속하려면 아무 키를 입력하세요.");
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
            case 0:	// 빈 공간
                printf("  ");
                break;
            case 1:	// 고정 벽

                SetConsoleOutputCP(437);	// CP437: 확장 아스키 모드 사용
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // DrawMap - 꽉찬벽 사용 시 색 설정
                printf("%c%c", 219, 219);
                SetConsoleOutputCP(949);	// 복구

                break;
            }
        }
        //이 부분을 주석처리하면 애니메이션이 꺼집니다.
        //Sleep(80);
    }

    int key = -1;
    int skip = 0;

    char message[20][1000] = { "게임 목표 : 반 중력을 가진 소행성과 지구3 행성의 충돌로 지구3의 중력장이 무너지고 말았습니다.",
                                "김세종(플레이어)은 여러 가지 장애물을 피해 목적지(★)에 도달해 탈출해야 합니다.",
                                "조작법    : 상(↑), 좌(←), 우(→) 버튼을 이용해 플레이어를 움직입니다.",
                               "상(↑) 버튼을 누르면 현재 중력이 반대 방향으로 바뀝니다.",
                                "좌(←) 버튼을 누르면 중력이 왼쪽으로 90도 기울어집니다.",
                               "우(→) 버튼을 누르면 중력이 오른쪽으로 90도 기울어집니다.",
                               "게임 진행 : 가시(▲,△), 운석(▣),에 닿으면 체력(♥)이 하나 깎입니다.",
                               "깜빡거리는 가시(△) 는 3초마다 한번씩 깜빡입니다.",
                               "블랙홀(＠)에 빠지면 플레이어는 그곳을 탈출해야 합니다.",
                               "어떤 스테이지는 탈출하기위해 열쇠(ⓚ)를 필요로 할 수 있습니다.",
                               "하트(♥)는 플레이어의 체력(♥)을 하나 증가시켜 줍니다."
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
       printf("게임 목표 : 알 수 없는 이유로 우주 공간의 중력이 뒤틀리고 말았습니다.");

        SetCurrentCursorPos(36+12, 16);
        //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        printf("플레이어는 여러 가지 장애물을 피해 목적지(★)에 도달해 탈출해야 합니다.");
        //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        SetCurrentCursorPos(36 , 20);
        printf("조작법    : 상(↑), 좌(←), 우(→) 버튼을 이용해 플레이어를 움직입니다.");
        SetCurrentCursorPos(36 + 12, 22);
        printf("상(↑) 버튼을 누르면 현재 중력이 반대 방향으로 바뀝니다.");
        SetCurrentCursorPos(36+ 12, 24);
        printf("좌(←) 버튼을 누르면 중력이 왼쪽으로 90도 기울어집니다.");
        SetCurrentCursorPos(36 + 12, 26);
        printf("우(→) 버튼을 누르면 중력이 왼쪽으로 90도 기울어집니다.");
        SetCurrentCursorPos(36 , 29);
        printf("게임 진행 : 가시(▲,△), 운석(▣),에 닿으면 체력(♥)이 1초에 하나씩 깎입니다.");
        SetCurrentCursorPos(36+12, 31);
        printf("깜빡거리는 가시(△) 는 3초마다 한번씩 깜빡입니다.");
        SetCurrentCursorPos(36 + 12, 33);
        printf("블랙홀(＠)에 빠지면 플레이어는 일정 시간 안에 그곳을 탈출해야 합니다.");
        SetCurrentCursorPos(36 + 12, 35);
        printf("어떤 스테이지는 탈출하기위해 열쇠(ⓚ)를 필요로 할 수 있습니다.");

        SetCurrentCursorPos(36 + 12, 37);
        printf("보호막(ⓢ)은 플레이어의 체력(♥)을 하나 증가시켜 줍니다.");
        */

        if (tmp % 2 == 0)
        {
            SetCurrentCursorPos(38 * 2 - 19, GBOARD_HEIGHT - 8);
            printf("계속하려면 아무 키를 입력하세요.");
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
    char message[20][1000] = { "총 게임 플레이 시간 : ", "점수 : ", "달성한 도전과제 : "
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
                    case 0:	// 빈 공간
                        printf("  ");
                        break;
                    case 1:	// 고정 벽

                        SetConsoleOutputCP(437);	// CP437: 확장 아스키 모드 사용
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // DrawMap - 꽉찬벽 사용 시 색 설정
                        printf("%c%c", 219, 219);
                        SetConsoleOutputCP(949);	// 복구

                        break;
                    }
                }
                else
                {
                    switch (GameClearScreen[y][x]) {
                    case 0:	// 빈 공간
                        printf("  ");
                        break;
                    case 1:	// 고정 벽

                        SetConsoleOutputCP(437);	// CP437: 확장 아스키 모드 사용
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // DrawMap - 꽉찬벽 사용 시 색 설정
                        printf("%c%c", 219, 219);
                        SetConsoleOutputCP(949);	// 복구

                        break;
                    }
                }

            }
            //이 부분을 주석처리하면 애니메이션이 꺼집니다.
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
                    printf("                 없음");
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



                //도전과제 달성 여부 확인 파트
                if (succ == 1) {
                    if (duration <= 240) {  //4분 이내로 클리어
                        In4Min = 1;
                        acheivecnt++;
                    }
                    if (health == 3) {  //목숨 3개로 클리어
                        In3Life = 1;
                        acheivecnt++;
                    }
                    if (InBlackHoleAddict == 0) // 블랙홀에 한번도 안빠지고 클리어
                    {
                        InNoBlackHole = 1;
                        acheivecnt++;
                    }
                    if (health == 3 && duration <= 240) //4분 이내에 3개 목숨으로 클리어 
                    {
                        InProgamer = 1;
                        acheivecnt++;
                    }
                    if (health == 1 && duration > 480) { // 목숨 1개남고 D랭크로 클리어
                        InDrank1Life = 1;
                        acheivecnt++;
                    }
                    if (mapNum >= 2)  //스테이지 1 클리어
                    {
                        InStage1clear = 1;
                        acheivecnt++;
                    }

                    if (mapNum >= 4)  //스테이지 4 클리어
                    {
                        InStage4clear = 1;
                        acheivecnt++;
                    }


                    if (InBlackHoleAddict >= 3) //블랙홀 3번 이상 클리어
                    {
                        acheivecnt++;
                    }
                    if (InBlackHoleAddict >= 3 && duration > 480) // D랭크 + 블랙홀 3번 이상 클리어
                    {
                        InExplorer = 1;
                        acheivecnt++;
                    }
                }
                SetCurrentCursorPos(59, 35);
                printf("%d 개", acheivecnt);


                int i = 0;


                SetCurrentCursorPos(80, 31 + 2 * i);

                if (succ == 1) {
                    if (In4Min == 1)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("누구보다 빠르게 - 4분 이내에 클리어");
                        i++;
                    }
                    if (In3Life == 1)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("생명을 소중히 - 체력 3개 유지한채로 클리어");
                        i++;
                    }
                    if (InBlackHoleAddict == 0)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("블랙홀 멸시자 - 블랙홀에 한번도 빠지지 않음");
                        i++;
                    }
                    if (InProgamer == 1)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("프로게이머 - 누구보다 빠르게 + 생명을 소중히 달성");
                        i++;
                    }
                    if (InDrank1Life == 1)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("아슬아슬 - 체력1개와 D랭크로 클리어");
                        i++;
                    }
                    if (InStage1clear == 1)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("천리길도 한걸음부터 - 스테이지 1 클리어");
                        i++;
                    }
                    if (InBlackHoleAddict >= 3)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("블랙홀 중독자 - 블랙홀에 3번 이상 빠짐");
                        i++;
                    }
                    if (InExplorer == 1)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("호기심 많은 탐험가 - 블랙홀 중독자 + D랭크 달성");
                        i++;
                    }
                    if (InStage4clear == 1)
                    {
                        SetCurrentCursorPos(80, 31 + 2 * i);
                        printf("정상이 보여요 ! - 스테이지 4 클리어");
                        i++;
                    }


                }
            }

            state = 1;
            key = -1;
        }



    }


}

//운석과 관련된 변수들

//운석의 시작좌표
int obstacleCoord[][4][2] = {


    { //블랙홀 스테이지 운석 없음
        {0},
        {0},
        {0}
    },
    { //스테이지1 운석 없음
        {0},
        {0},
        {0}
    },

    { //스테이지 2
        {11,38}, //27~39  시작좌표 (11, 39)~
        {28,27}, //27~43  시작좌표 (28, 27)~
        {37,30} //27~39   시작좌표 (37, 30)~
    },

    { //스테이지 3
        {24, 17}, //7~18까지
        {24, 51}, //51~63까지
        {32,44},
        {42,46}
    },

    { // 스테이지 4 // 12.5추가
        {17, 30},
        {14, 40}
    }
};

//운석이동방향 좌측 0, 우측 1, 아래 2, 위 3
int isRight[][4] = { //운석이 존재하지않음 -1
    {-1,-1,-1,-1},  // 블랙홀
    {-1,-1,-1,-1},  // 스테이지 1
    {0,1,1,0},      // 스테이지 2
    {0,1,2,3},      // 스테이지 3
    // 12.5 추가 (황민혁)
    {1,0,0,0}     // 스테이지 4
};

// 운석 이동
void moveObstacle() {

    COORD temp = GetCurrentCursorPos();

    int n = 1;
    if (mapNum == 2) n = 3;
    else if (mapNum == 3) n = 4;
    // 12.5 추가 (황민혁)
    else if (mapNum == 4) n = 2;

    for (int i = 0; i < n; i++) {

        int dx[4] = { 0,0,1,1 };
        int dy[4] = { 0,1,0,1 };

        if (mapInfo[mapNum][obstacleCoord[mapNum][i][0]][obstacleCoord[mapNum][i][1]] != 6) continue;

        for (int j = 0; j < 4; j++)
            mapInfo[mapNum][obstacleCoord[mapNum][i][0] + dx[j]][obstacleCoord[mapNum][i][1] + dy[j]] = 0;



        if (isRight[mapNum][i] == 1) obstacleCoord[mapNum][i][1]++;
        else if (isRight[mapNum][i] == 0)obstacleCoord[mapNum][i][1]--;
        else if (isRight[mapNum][i] == 2) obstacleCoord[mapNum][i][0]++; //아래 방향
        else if (isRight[mapNum][i] == 3) obstacleCoord[mapNum][i][0]--; //위 방향

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
                    printf("▣");
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
                    printf("▣");
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
                    printf("▣");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1]) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] + 1);
                    printf("▣");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1] + 1) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0]);
                    printf("▣");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1] + 1) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] + 1);
                    printf("▣");
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
                    printf("▣");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1]) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] + 1);
                    printf("▣");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1] + 1) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0]);
                    printf("▣");
                    SetCurrentCursorPos(GBOARD_ORIGIN_X + (obstacleCoord[mapNum][i][1] + 1) * 2, GBOARD_ORIGIN_Y + obstacleCoord[mapNum][i][0] + 1);
                    printf("▣");
                }
            }

        }
    }


    for (int i = 0; i < n; i++) {

        switch (mapNum)
        {
        case 2:
            //끝에 닿은경우 방향 바꿔주기
            if ((i != 1 && obstacleCoord[mapNum][i][1] == 38) || obstacleCoord[mapNum][i][1] == 43)    isRight[mapNum][i] = 0;
            else if (obstacleCoord[mapNum][i][1] == 27)  isRight[mapNum][i] = 1;
            break;
        case 3:
            if (obstacleCoord[mapNum][i][1] == 17 || obstacleCoord[mapNum][i][1] == 62)    isRight[mapNum][i] = 0;
            else if (obstacleCoord[mapNum][i][1] == 7 || obstacleCoord[mapNum][i][1] == 51)  isRight[mapNum][i] = 1;
            else if (obstacleCoord[mapNum][i][0] == 30) isRight[mapNum][i] = 2;
            else if (obstacleCoord[mapNum][i][0] == 42) isRight[mapNum][i] = 3;


        case 4: // 12.5 추가 (황민혁)
            if (obstacleCoord[mapNum][i][1] == 28) isRight[mapNum][i] = 1;
            else if (obstacleCoord[mapNum][i][1] == 42) isRight[mapNum][i] = 0;

        }


    }

    SetCurrentCursorPos(temp.X, temp.Y);

}


void CollisionBlackhole() {
    deleteBar();    // check
    PlaySound(TEXT(BGM_BLACKHOLE), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

    InBlackHoleAddict++; // 블랙홀 업적 업데이트

    isBlackhole = 1;//현재 블랙홀 상태가 됨

    //중력 방향을 아래방향으로 초기화
    gravity = 2;
    block_id = 2;

    //블랙홀 스테이지에서 PC위치 초기화
    curPosX = pcStartPos[0].X * 2;
    curPosY = pcStartPos[0].Y;

    //현 스테이지 저장
    tempmapNum = mapNum;
    mapNum = 0;

    //블랙홀 맵 그리기
    drawBar();  // check
    DrawMap();
    SetCurrentCursorPos(curPosX, curPosY);
}

//운석 충돌
void CollisionMeteorite(int x, int y) {

    isObstacleCollision = 1; //운석과 충돌함
    slowSpeedTime = 1;//속도 저하 시간 초기화
    speed = 50; //속도 저하


    health--;
    GameInfoUpdate();

    COORD temp = GetCurrentCursorPos();

    //운석 사라짐
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

//목적지 도달
int CollisionDest() {

    //키 전부 획득했는가 11.24 추가
    if (stageKey[mapNum] > 0) {//키를 다 안먹었으면 리턴
        return -1;
    }


    //블랙홀 상태인 경우
    if (isBlackhole == 1) {
        isBlackhole = 0;
        mapNum = tempmapNum;
        if (mapNum == 4) { PlaySound(TEXT(BGM_MAIN1), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); }
        else { PlaySound(TEXT(BGM_MAIN2), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); }
    }
    //블랙홀 상태가 아님
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

    //스테이지별 초기세팅
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
    case 4: // 12.5 추가 (황민혁)
        PlaySound(TEXT(BGM_MAIN1), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        curPosX = pcStartPos[4].X * 2;
        curPosY = pcStartPos[4].Y;
        gravity = 2;
        block_id = 2;
        SetCurrentCursorPos(curPosX, curPosY);
    }
    drawBar();//12.03 추가

}
void drawBar() {

    COORD temp;
    temp.X = curPosX;
    temp.Y = curPosY;
    int x, y;
    //10 =초록 11=하늘 12=빨강 13=핑크 14=노랑 15=하얀
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
    switch (gravity) {
    case GDOWN: //2
        for (x = 0; x < GBOARD_WIDTH; x++) {
            SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT);
            printf("▼");
        }
        break;
    case GRIGHT: //1
        for (y = 0; y < GBOARD_HEIGHT; y++) {
            SetCurrentCursorPos(GBOARD_ORIGIN_X + GBOARD_WIDTH * 2, GBOARD_ORIGIN_Y + y);
            printf("▶");
        }
        break;
    case GUP: //0
        for (x = 0; x < GBOARD_WIDTH; x++) {
            SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y - 1);
            printf("▲");
        }
        break;
    case GLEFT: //3
        for (y = 0; y < GBOARD_HEIGHT; y++) {
            SetCurrentCursorPos(GBOARD_ORIGIN_X - 2, GBOARD_ORIGIN_Y + y);
            printf("◀");
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