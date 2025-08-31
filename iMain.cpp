#include "iGraphics.h"
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include "iSound.h"
#define screenx 1500
#define screenY 750
#define GAMEOVER 8
#define BGSPEED_EASY 1
#define BGSPEED_MEDIUM 2
#define BGSPEED_HARD 4
#define SLEEPTIME 60
#define PAUSETIME 40
#define MAINSOUND 40
#define GAMESOUND 100
int gamestate = -1;
int idx = 0;
int laststate;
int sleep = 0;
int sleep_counter = 0;

int bird_jump = 50;
int birdx = 100;
int birdy = 350;
double birdVelocity = 0;
double gravity = -0.4;
double flapPower = 7;
double pillerSpeed = 0.5;
int score = 0;
int nextpiller= 0;

char birdimage[8][20] = {"bird1.png", "bird2.png", "bird3.png", "bird4.png", "bird5.png", "bird6.png", "bird7.png", "bird8.png"};
int pillerSpace = 180;
int pillerWidth = 80;

int pillerX[4] = {800, 1200, 1500, 1800};
int piller_height_down[4] = {50, 250, 100, 150};

char playerName[50] = "";
int nameLen = 0;
char names[100][50];
int scores[100];
int count = 0;

int birdWidth = 93;
int birdHeight = 52;

Image easy,medium,hard;
Image rain1,rain2,rain3,snow1,snow2,snow3, bgeffect;
int bg_x1 = 0;
int bg_x2 = screenx*2;

int main_bgm, game_bgm, main_bgm_status = 0, game_bgm_status = 0;

int pause = 0;
int start = 0;

void bg_effect_handle(void)
{
    if(gamestate == 6)//medium
    {
        if(idx%3 == 0) bgeffect = rain1;
        else if(idx%3 == 1) bgeffect = rain2;
        else if(idx%3 == 2) bgeffect = rain3;
    }
    else if(gamestate == 7)//hard
    {
        if(idx%6 == 0 || idx%6 == 1) bgeffect = snow1;
        else if(idx%6 == 2 || idx%6 == 3) bgeffect = snow2;
        else if(idx%6 == 4 || idx%6 == 5) bgeffect = snow3;
    }
}

void update_bird_images()
{
    if(sleep == 1 || pause == 1) return;
    idx = (idx + 1) % 8; //pause
}
void startPipes()
{
    int firstX = 800;
    int gapX = 400;
    score = 0;
    nextpiller = 0;

    for (int i = 0; i < 4; i++)
    {
        pillerX[i] = firstX + i * gapX;
        piller_height_down[i] = rand() % 250 + 50;
    }
}

void gameover()
{
    if (laststate == 5)
        iShowLoadedImage(0, 0, &easy);
    else if (laststate == 6)
        iShowLoadedImage(0, 0, &medium);
    else if (laststate == 7)
        iShowLoadedImage(0, 0, &hard);

    iSetColor(255, 0, 0);
    iText(600, 400, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);

    char scoreStr[50];
    sprintf(scoreStr, "Your Score: %d", score);
    iTextAdvanced(600, 350, scoreStr,0.3,2);

    FILE *fp = fopen("leaderboard.txt", "r");
    int high = 0, s;
    char n[50];
    if (fp != NULL)
    {
        while (fscanf(fp, "%s %d", n, &s) != EOF)
        {
            if (s > high)
                high = s;
        }
        fclose(fp);
    }

    char highStr[50];
    sprintf(highStr, "High Score: %d", high);
    iText(600, 320, highStr);

    iTextAdvanced(600, 280, "Enter your name & press ENTER:",0.2,2);
    iTextAdvanced(600, 250, playerName,0.2,2);
}

void change1()
{
    if (gamestate != 5 && gamestate != 6 && gamestate != 7)
    {
        return;
    }
    else if(sleep == 1) return;
    else if(pause == 1) return;
    //pause

    birdVelocity += gravity;
    birdy += birdVelocity;
    if (birdy < 0)
    {
        birdy = 0;
        PlaySound(TEXT("hit sound.wav"), NULL, SND_ASYNC);
        //gamestate = GAMEOVER;
        if(sleep == 0) sleep_counter = SLEEPTIME;
        sleep = 1;
    }
    if (birdy > screenY - 100)
        birdy = screenY - 150;

    int i = 0;
    while (i != 4)
    {
        pillerX[i] -= pillerSpeed;

        if (pillerX[i] + pillerWidth < 0)
        {
            pillerX[i] = screenx;
            piller_height_down[i] = rand() % 250 + 50;
        }
        if (birdx + birdWidth >= pillerX[i] && birdx <= pillerX[i] + pillerWidth)
        {
            if (birdy <= piller_height_down[i] || birdy + birdHeight >= piller_height_down[i] + pillerSpace)
            {
                PlaySound(TEXT("hit sound.wav"), NULL, SND_ASYNC);
                //gamestate = GAMEOVER;
                if(sleep == 0) sleep_counter = SLEEPTIME;
                sleep = 1;
            }
        }

        i++;
    }

    if (pillerX[nextpiller % 4] + pillerWidth < birdx)
    {
        score += 10;
        nextpiller++;
    }
}

void easystate()
{
    if(sleep == 1) //for gameover delay
    {
        sleep_counter--;
        if(sleep_counter <= 0) gamestate = (start == 0) ? GAMEOVER : gamestate, sleep = 0, start = 0 , bg_x1 = 0, bg_x2 = screenx*2, birdx = 100;
    }
    iShowLoadedImage(bg_x1, 0, &easy);
    iShowLoadedImage(bg_x2, 0, &easy);
    if(sleep == 0 && pause == 0) bg_x1 -= BGSPEED_EASY, bg_x2 -= BGSPEED_EASY; //pause
    if(bg_x1 == screenx*-2) bg_x1 = screenx*2;
    if(bg_x2 == screenx*-2) bg_x2 = screenx*2;

    iShowImage(birdx, birdy, birdimage[idx]);
    //iSetColor(255,0,0);
    //iSetLineWidth(2);
    //iRectangle(birdx,birdy,birdWidth,birdHeight);
    pillerSpace = 240;



    for (int i = 0; i < 4; i++)
    {
        iSetColor(46, 99, 32);
        iFilledRectangle(pillerX[i], 0, pillerWidth, piller_height_down[i]);
        iFilledRectangle(pillerX[i], piller_height_down[i] + pillerSpace, pillerWidth, screenY - (piller_height_down[i] + pillerSpace));
    }

    char scoreStr[20];
    sprintf(scoreStr, "Score: %d", score);
    iSetColor(42, 157, 143);
    iText(50, 700, scoreStr, GLUT_BITMAP_TIMES_ROMAN_24);

    //pause bg
    if(pause == 1) iShowImage(0,0, "pause.png");

    if(start == 1)
    {
        if(sleep_counter >= PAUSETIME*2/3) iShowImage(0,0, "3.png");
        else if(sleep_counter >= PAUSETIME/3) iShowImage(0,0, "2.png");
        else iShowImage(0,0, "1.png");
    }
}

void mediumstate()
{
    if(sleep == 1)
    {
        sleep_counter--;
        if(sleep_counter <= 0) gamestate = (start == 0) ? GAMEOVER : gamestate, sleep = 0, start = 0 , bg_x1 = 0, bg_x2 = screenx*2, birdx = 100;
    }
    iShowLoadedImage(bg_x1, 0, &medium);
    iShowLoadedImage(bg_x2, 0, &medium);
    if(sleep == 0 && pause == 0) bg_x1 -= BGSPEED_MEDIUM, bg_x2 -= BGSPEED_MEDIUM; //pause
    if(bg_x1 == screenx*-2) bg_x1 = screenx*2;
    if(bg_x2 == screenx*-2) bg_x2 = screenx*2;

    iShowLoadedImage(0,0,&bgeffect);

    iShowImage(birdx, birdy, birdimage[idx]);
    pillerSpace = 220;

    for (int i = 0; i < 4; i++)
    {
        iSetColor(46, 99, 32);
        iFilledRectangle(pillerX[i], 0, pillerWidth, piller_height_down[i]);
        iFilledRectangle(pillerX[i], piller_height_down[i] + pillerSpace, pillerWidth, screenY - (piller_height_down[i] + pillerSpace));
    }
    char scoreStr[20];
    sprintf(scoreStr, "Score: %d", score);
    iSetColor(42, 157, 143);
    iText(50, 700, scoreStr, GLUT_BITMAP_TIMES_ROMAN_24);

    //pause bg
    if(pause == 1) iShowImage(0,0, "pause.png");

    if(start == 1)
    {
        if(sleep_counter >= PAUSETIME*2/3) iShowImage(0,0, "3.png");
        else if(sleep_counter >= PAUSETIME/3) iShowImage(0,0, "2.png");
        else iShowImage(0,0, "1.png");
    }
}
void hardstate()
{
    if(sleep == 1)
    {
        sleep_counter--;
        if(sleep_counter <= 0) gamestate = (start == 0) ? GAMEOVER : gamestate, sleep = 0, start = 0 , bg_x1 = 0, bg_x2 = screenx*2, birdx = 100;
    }
    iShowLoadedImage(bg_x1, 0, &hard);
    iShowLoadedImage(bg_x2, 0, &hard);
    if(sleep == 0 && pause == 0) bg_x1 -= BGSPEED_HARD, bg_x2 -= BGSPEED_HARD; // pause
    if(bg_x1 == screenx*-2) bg_x1 = screenx*2;
    if(bg_x2 == screenx*-2) bg_x2 = screenx*2;

    iShowLoadedImage(0,0,&bgeffect);

    iShowImage(birdx, birdy, birdimage[idx]);
    pillerSpace = 200;

    for (int i = 0; i < 4; i++)
    {
        iSetColor(107, 144, 128);
        iFilledRectangle(pillerX[i], 0, pillerWidth, piller_height_down[i]);
        iFilledRectangle(pillerX[i], piller_height_down[i] + pillerSpace, pillerWidth, screenY - (piller_height_down[i] + pillerSpace));
    }
    char scoreStr[20];
    sprintf(scoreStr, "Score: %d", score);
    iSetColor(0, 0, 0);
    iText(50, 700, scoreStr, GLUT_BITMAP_TIMES_ROMAN_24);

    //pause bg
    if(pause == 1) iShowImage(0,0, "pause.png");

    if(start == 1)
    {
        if(sleep_counter >= PAUSETIME*2/3) iShowImage(0,0, "3.png");
        else if(sleep_counter >= PAUSETIME/3) iShowImage(0,0, "2.png");
        else iShowImage(0,0, "1.png");
    }
}

void leaderboard()
{
    iShowImage(0, 0, "Leaderboard page.png");

    count = 0;

    FILE *fp = fopen("leaderboard.txt", "r");
    if (fp != NULL)
    {
        while (fscanf(fp, "%s %d", names[count], &scores[count]) == 2)
        {
            count++;
            if (count >= 100)
                break;
        }
        fclose(fp);
    }
    // bubble sort the score array
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            if (scores[i] < scores[j])
            {

                int tempScore = scores[i];
                scores[i] = scores[j];
                scores[j] = tempScore;

                // swap corresponding names
                char tempName[50];
                strcpy(tempName, names[i]);
                strcpy(names[i], names[j]);
                strcpy(names[j], tempName);
            }
        }
    }

    iSetColor(0, 0, 0);
    iText(600, 700, "LEADERBOARD");

    for (int i = 0; i < count && i < 5; i++)
    {
        char line[100];
        sprintf(line, "%d. %s - %d", i + 1, names[i], scores[i]);
        iText(580, 650 - i * 40, line); // there will be 40 del y gap vertical
    }
}

void iDraw()
{
    // place your drawing codes here
    iClear();
    switch (gamestate)
    {
    case -1:
        iShowImage(0, 0, "Loading page.png");
        iSetColor(0, 0, 0);
        iText(100, 100, "press q anytime to exit");

        break;
    case 0:
        if(game_bgm_status == 1) iStopSound(game_bgm), game_bgm_status = 0;
        if(main_bgm_status == 0) main_bgm = iPlaySound("bgmusic.wav", true, MAINSOUND), main_bgm_status = 1;
        iShowImage(0, 0, "Main Background.png");
        //iText(1000,100,"press M to mute or unmute")

        break;
    case 1:
        iShowImage(0, 0, "Difficulty page.png");

        break;
    case 2:
        leaderboard();
        break;
    case 3:
        iShowImage(0, 0, "instruction.png");

        break;
    case 4:
        iShowImage(0, 0, "credits.png");

        break;
    case 5:
        if(game_bgm_status == 0) game_bgm = iPlaySound("gamebgm.wav", true, GAMESOUND), game_bgm_status = 1;
        if(main_bgm_status == 1) iStopSound(main_bgm), main_bgm_status = 0;
        easystate();
        break;
    case 6:
        if(game_bgm_status == 0) game_bgm = iPlaySound("gamebgm.wav", true, GAMESOUND), game_bgm_status = 1;
        if(main_bgm_status == 1) iStopSound(main_bgm), main_bgm_status = 0;
        mediumstate();
        break;
    case 7:
        if(game_bgm_status == 0) game_bgm = iPlaySound("gamebgm.wav", true, GAMESOUND), game_bgm_status = 1;
        if(main_bgm_status == 1) iStopSound(main_bgm), main_bgm_status = 0;
        hardstate();
        break;
    case 8:
        iStopSound(main_bgm);
        iStopSound(game_bgm);
        gameover();
        break;
    }
}

void iMouseMove(int mx, int my)
{
    //printf("x= %d\t  y=%d\n", mx, my);
}

/*
function iMouseDrag() is called when the user presses and drags the mouse.
(mx, my) is the position where the mouse pointer is.
*/
int idx1;
void iMouseDrag(int mx, int my)
{
    // place your codes here
}

void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)

    {
        if (gamestate == 0) // if the opened page is home page
        {
            if (mx > 605 && mx < 870 && my > 235 && my < 300)
            {

                PlaySound(TEXT("swoosh.wav"), NULL, SND_ASYNC);
                gamestate = 3;
            }
            else if (mx > 670 && mx < 800 && my > 135 && my < 195)
            {
                PlaySound(TEXT("swoosh.wav"), NULL, SND_ASYNC);

                gamestate = 4;
            }
            else if (mx > 605 && mx < 870 && my > 465 && my < 530)
            {
                PlaySound(TEXT("swoosh.wav"), NULL, SND_ASYNC);
                gamestate = 1;
            }
            else if (mx > 605 && mx < 870 && my > 350 && my < 405)
            {

                PlaySound(TEXT("swoosh.wav"), NULL, SND_ASYNC);

                gamestate = 2;
            }
            else if (mx > 695 && mx < 785 && my > 35 && my < 95)
                exit(0);
        }

        else if (gamestate == 1) // if opened page is difficulty page
        {
            if (mx > 660 && mx < 795 && my > 505 && my < 590) // selected for easy level
            {
                gamestate = 5;
                laststate = 5;
                PlaySound(TEXT("swoosh.wav"), NULL, SND_ASYNC);
                pillerSpeed = 4;
                start = 1;
                sleep = 1;
                sleep_counter = PAUSETIME;
                startPipes();
            }
            else if (mx > 615 && mx < 852 && my > 360 && my < 450) // selected for medium level
            {
                gamestate = 6;
                laststate = 6;
                pillerSpeed = 7;
                pillerWidth=130;
                start = 1;
                sleep = 1;
                sleep_counter = PAUSETIME;
                PlaySound(TEXT("swoosh.wav"), NULL, SND_ASYNC);
                startPipes();
            }
            else if (mx > 657 && mx < 814 && my > 220 && my < 305) // selected for hard level
            {
                gamestate = 7;
                laststate = 7;
                pillerSpeed = 10;
                pillerWidth=160;
                start = 1;
                sleep = 1;
                sleep_counter = PAUSETIME;
                startPipes();
                PlaySound(TEXT("swoosh.wav"), NULL, SND_ASYNC);
            }
            else if (mx > 660 && mx < 810 && my > 80 && my < 157) // back button
            {

                PlaySound(TEXT("swoosh.wav"), NULL, SND_ASYNC);
                gamestate = 0;
            }
        }

        else if (gamestate == 4) // if the opened page is credit page
        {
            if (mx > 680 && mx < 810 && my > 60 && my < 126)
            {
                gamestate = 0;
                PlaySound(TEXT("swoosh.wav"), NULL, SND_ASYNC);
            }
        }
        else if (gamestate == 3) // if opened page is instruction page
        {
            if (mx > 695 && mx < 810 && my > 75 && my < 138)
            {
                gamestate = 0;
                PlaySound(TEXT("swoosh.wav"), NULL, SND_ASYNC);
            }
        }
        else if (gamestate == 2)
            if (mx > 680 && mx < 810 && my > 60 && my < 126)
            {
                gamestate = 0;
                PlaySound(TEXT("swoosh.wav"), NULL, SND_ASYNC);
            }
    }
}

/*
function iMouseWheel() is called when the user scrolls the mouse wheel.
dir = 1 for up, -1 for down.
*/
void iMouseWheel(int dir, int mx, int my)
{
    // place your code here
}

/*
function iKeyboard() is called whenever the user hits a key in keyboard.
key- holds the ASCII value of the key pressed.
*/
void iKeyboard(unsigned char key)
{
    switch (key)
    {
    case 'q':
        exit(0);

        break;
    case ' ':
        if (gamestate != GAMEOVER)
            birdVelocity = flapPower;
        if ((gamestate == 5 || gamestate == 6 || gamestate == 7) && pause == 0)
            //PlaySound(TEXT("flapsound.wav"), NULL, SND_ASYNC);
            iPlaySound("flapsound.wav", false, 5);

        break;
    case 'p':
        if (gamestate != GAMEOVER && sleep == 0)
            pause = (pause == 0) ? 1 : 0;
        else if (gamestate == GAMEOVER && nameLen < 49 && key >= 32 && key <= 126)
        {
            playerName[nameLen++] = key;
            playerName[nameLen] = '\0';
        }
        break;
    case '\r':
        if (gamestate == GAMEOVER)
        {

            // Save name and score to leaderboard
            FILE *fp = fopen("leaderboard.txt", "a");
            if (fp != NULL)
            {

                fprintf(fp, "%s %d\n", playerName[0] ? playerName : "Anonymous", score);
                fclose(fp);
            }

            // Reset all for new game
            gamestate = 0; // back to main menu
            score = 0;
            nextpiller = 0;
            birdy = 350;
            birdVelocity = 0;
            playerName[0] = '\0'; // clear name
            nameLen = 0;
        }

        break;

    case 'v':
        if (gamestate == -1)
            gamestate = 0;
        else if (gamestate == GAMEOVER && nameLen < 49 && key >= 32 && key <= 126)
        {
            playerName[nameLen++] = key;
            playerName[nameLen] = '\0';
        }
        break;
    case 'r':
        if (gamestate != GAMEOVER && birdx+ birdWidth < screenY && sleep == 0 && pause == 0)
            birdx = birdx + 30;
        else if (gamestate == GAMEOVER && nameLen < 49 && key >= 32 && key <= 126)
        {
            playerName[nameLen++] = key;
            playerName[nameLen] = '\0';
        }
        break;
    case 'e':
        if (gamestate != GAMEOVER && birdx > 0 && sleep == 0 && pause == 0)
            birdx = birdx - 30;
        else if (gamestate == GAMEOVER && nameLen < 49 && key >= 32 && key <= 126)
        {
            playerName[nameLen++] = key;
            playerName[nameLen] = '\0';
        }
        break;
    case 8:
        playerName[--nameLen] = '\0';
        if(nameLen < 0) nameLen = 0;
        break;

    default:

        if (gamestate == GAMEOVER && nameLen < 49 && key >= 32 && key <= 126)
        {
            playerName[nameLen++] = key;
            playerName[nameLen] = '\0';
        }

        break;
    }
}

void iSpecialKeyboard(unsigned char key)
{
    switch (key)
    {
    case GLUT_KEY_END:
        // do something
        break;
    // place your codes for other keys here
    default:
        break;
    }
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    srand(time(NULL));
    // place your own initialization codes here.
    iSetTimer(20, update_bird_images);
    iSetTimer(20, change1);
    iSetTimer(50, bg_effect_handle);
    iInitializeSound();

    //loading images
    iLoadImage(&easy, "easy bg.png");
    iResizeImage(&easy, screenx*2, screenY);
    iLoadImage(&medium, "medium bg.png");
    iResizeImage(&medium, screenx*2, screenY);
    iLoadImage(&hard, "hard bg.png");
    iResizeImage(&hard, screenx*2, screenY);
    iLoadImage(&rain1, "rain1.png");
    iLoadImage(&rain2, "rain2.png");
    iLoadImage(&rain3, "rain3.png");
    iLoadImage(&snow1, "snow1.png");
    iLoadImage(&snow2, "snow2.png");
    iLoadImage(&snow3, "snow3.png");

    main_bgm = iPlaySound("bgmusic.wav", true, MAINSOUND);
    main_bgm_status = 1;

    iInitialize(screenx, screenY, "Flappy Bird");

    return 0;
}