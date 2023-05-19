#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winuser.h>

#define VK_LEFT 0x25
#define VK_RIGHT 0x27
#define VK_ESC 27
#define VK_SPACE 0x20

#define ROWS 20
#define COLS 119
#define PADDLE_LEN 10

// Global degiskenler
char board[ROWS][COLS];
int ball_x, ball_y, ball_dir_x, ball_dir_y;
int paddle_x = COLS/2 - PADDLE_LEN/2 +10;
int bricks_left = ROWS * COLS /30 + 13 ;
int brickCreated = ROWS * COLS /30 + 13;//WIN durumu kontrolü için kullanılıyor.
int lives=3;
int score;



// Fonksiyonlar
void init_board();
void clear_screen();
void draw_board();
void game_loop();
void update_ball();
void update_paddle(int direction);
int hit_paddle();
int hit_brick(int x, int y);
void reset_game();





int main() {
    init_board();
    game_loop();
    return 0;
}






// board'u tasarlar.
void init_board() {
    // tuglalari, duvarları atar.
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (i == 0 || j == 0  || j == COLS-1) {
                board[i][j] = ' ';
            }
              //i sayısı(ROW)tek olduğunda ve j(COLUMN) sayısının 5 ile bölümünden kalanın 3'ten büyük ve 5'ten küçük olduğu yerlerde tuğla çizer.
              else if (i % 2 == 1 && j % 5 > 3 && j % 5 < 5 && bricks_left > 0) {
    			board[i][j] = '#';
    			bricks_left--;
            } else {
                board[i][j] = ' ';
            }
        }
    }
    // topun pozisyonunu ve yönünü tanimlar.
    ball_x = COLS/2;
    ball_y = ROWS/2+5;
    ball_dir_x = 1;
    ball_dir_y = -1;

    // Raket pozisyonunu tasarlar.
    for (int i = paddle_x; i < paddle_x + PADDLE_LEN; i++) {
        board[ROWS - 1][i] = '|';
    }
}





// Ekrani temizler.
void clear_screen() {
    system("cls");
}





// Board'u çizer.
void draw_board() {
    clear_screen();
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);//renk ataması
    printf("\t\tYour Live(s): %d\t\t\t\t \t\t\t\tYour score: %d", lives,score );
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (board[i][j] == '#') {
                // tuğlalara yeşil rengini verir.
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
            } else if(board[i][j] == '|'){
                // diğer karakterler için beyaz renk ataması yapar.
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
            }
            else if(board[i][j] == 'O'){
                // top için kırmızı renk ataması yapar.
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),12);
            }
            printf("%c", board[i][j]);
        }
        printf("\n");
    }

}




// OYUNUN ÇALIŞMASINI SAĞLAYAN DÖNGÜ
void game_loop() {
    int game_over = 0;
    int start_game = 0;
    int read_inst=0;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    printf("\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\t\t  BRICK BREAKER GAME\n\n\t\t\t\t\t\t Press ENTER to start\n\n\t\t\t\t\t   Press SPACE to read INSTRUCTIONS");
    int i=0;

    //Instruction okumak isterse space tuşuna bastığı anda ekrana bu gelir.
    while(!read_inst){
        if (_kbhit()) {
            if (GetAsyncKeyState(VK_SPACE)!= 0 && i==0) {//i atanmasının nedeni sürekli olarak yazdırmaması için, bir counter değer olarak.
                clear_screen();
                printf("\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\t    Use arrow keys to move the paddle\n\t\t\t\t\t      You will have 3 lives to play\n\t\t\t\t\t     Try to hit the ball with paddle\n\t\t\t\t\tOnce you break all the bricks you will win");
                 SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
                printf("\n\t\t\t\t\t\t Press ENTER to start\n\n\t\t\t\t\t");
            }
            i++;
            if(GetAsyncKeyState(VK_RETURN)!= 0){
                    break;
            }
        }
    }


    // Enter'a basılması durumunda looptan çıkıp, oyunu başlatır.
    while (!start_game) {
        if (_kbhit()) {
            if (GetAsyncKeyState(VK_RETURN)!= 0) {
               start_game = 1;// looptan çıkarır.
            }
        }
    }

    //eğer oyun başladıysa ve kaybedilmediyse bu loop devam eder.
    while (!game_over) {
        draw_board();//oyun ekranıni çizer.
        update_ball();//topu günceller.

        //Tuğlaların hepsi kırılırsa oyunu durdur.
        if(brickCreated == 0){
            game_over=1;
        }

        //Oyunun bitip bitmediğini kontrol eder. (Top en alt row'a değerse 3 haktan sonra oyun biter)
        if (ball_y == ROWS) {
            lives--;
            if(lives==0){
                game_over=1;
            }
            else{
                //eğer top en alt row'a değer ve hala can hakkın varsa enter'a basması halinde, oyuncu diğer hakkını kullanır.
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),12);
                printf("\n\t\t\t\t\tYou lost a life. Press Enter to continue.\n");
                while (1){
                    if (_kbhit()) {
                        if (GetAsyncKeyState(VK_RETURN)!= 0) {
                            break;
                        }
                    }
                }
            //can hakkı kaybedilirse, topun konumu ve yönü güncellenir.
                ball_x = COLS/2;
                ball_y = ROWS/2+8;
                ball_dir_x = 1;
                ball_dir_y = -1;
                update_paddle(0);
            }
        }

        //Paddle'ın konumu burada güncellenir.
        if (GetAsyncKeyState(VK_LEFT) != 0) {//oyuncu sol arrow tuşuna basarsa, paddle -2 yönünde kayar.
            update_paddle(-2);
        } else if (GetAsyncKeyState(VK_RIGHT) != 0) {//oyuncu sağ arrow tuşuna basarsa, paddle 2 yönünde kayar.
            update_paddle(2);
        } else {
            update_paddle(0);
        }
        Sleep(50);// oyunu yavaşlatır.
        update_paddle(0);
    }
    if(brickCreated == 0){//kazanma kontrolünü sağlar.
        clear_screen();
        printf("\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\tYour Live(s): %d\n\t\t\t\t\tYour score: %d \n", lives,score );
        printf("\t\t\t\t\tYou Win! \n\t\t\t\t\tPress Enter to restart the game\n\t\t\t\t\tPress ESC to exit");
        reset_game();
    }else{//kazanılamadıysa, kaybedilmiştir. Enter'a basılırsa oyun tekrar başlar. (bu durum reset game içinde kontrol edilir)
        clear_screen();
        printf("\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\tYour Live(s): %d\n\t\t\t\t\tYour score: %d \n", lives,score );
        printf("\t\t\t\t\tGame Over! \t\t\t\t\t\n\t\t\t\t\tPress Enter to restart the game\n\t\t\t\t\tPress ESC to exit");
        reset_game();


    }
}




// Topun pozisyonunu günceller.
void update_ball() {
    // Topun yeni pozisyonunu hesaplar.
    int new_x = ball_x + ball_dir_x;
    int new_y = ball_y + ball_dir_y;



    // "Top sağ ve sol duvara vuruyor mu" kontrolu saglanir.
    if (new_x == 0 || new_x == COLS - 1) {
        ball_dir_x *= -1;//topun yönü güncellenir.
        new_x = ball_x + ball_dir_x;
    }
    // "Top tavana vuruyor mu" kontrolü sağlanır.
    else if (new_y == 0) {
        ball_dir_y *= -1;//topun yönü güncellenir.
        new_y = ball_y + ball_dir_y;
    }

    //"Top rakete vuruyor mu" kontrolu saglanir.
    else if (hit_paddle()) {
        ball_dir_y = -1;//topun yönü güncellenir.
        new_y = ball_y + ball_dir_y;
    }
    //"Top tuğlalara üstten alttan vuruyor mu" kontrolu saglanir.
    else if (hit_brick(new_x, new_y)) {//topun gideceği konumda tuğla var mı? varsa tuğla silinir.
            if ((ball_dir_y == 1 && ball_y < new_y) || (ball_dir_y == -1 && ball_y> new_y)) {
                ball_dir_y *= -1; // eğer top tuğlaya üstten ya da alttan vurursa topun y yönünü değiştirir.
            }
    }
    //Top, tuğlaya sağ ya da soldan vurursa durumu
    if(board[new_y][new_x - 1] == '#' && board[new_y][new_x]!='#'  &&  board[ball_y+1][ball_x]!='#' &&  board[ball_y-1][ball_x]!='#'){
       board[new_y][new_x - 1] = ' ';
       ball_dir_x *= -1;
       score+=10;
    }
    if (board[new_y][new_x + 1] == '#' && board[new_y][new_x]!='#' &&  board[ball_y+1][ball_x]!='#' && board[ball_y-1][ball_x]!='#'){
        board[new_y][new_x + 1] = ' ';
        ball_dir_x *= -1;
        score+=10;
    }


    //Topun pozisyonunu gunceller.
    board[ball_y][ball_x] = ' ';
    ball_x = new_x;
    ball_y = new_y;
    board[ball_y][ball_x] = 'O';
}






// Raketin pozisyonunu gunceller.
void update_paddle(int dir) {//game_loop'tan alınan değer ile paddle durumu güncellenir.
    int new_paddle_x = paddle_x + dir;

    // "Raket duvara degiyor mu" kontrolunu saglar değerse silmez ve çizmez tekrar.
    if (new_paddle_x < 1 || new_paddle_x + PADDLE_LEN > COLS - 1) {//sağ ve sol duvara değme durumları kontrolü sağlanır.
    return;
    }
    // Eski raketi siler.
    for (int i = paddle_x; i < paddle_x + PADDLE_LEN; i++) {
        board[ROWS - 1][i] = ' ';
    }
    // Raketin pozisyonunu güncelleyip, çizer.
    paddle_x = new_paddle_x;
    for (int i = paddle_x; i < paddle_x + PADDLE_LEN; i++) {
        board[ROWS - 1][i] = '|';
    }
}





//Topun raketle olan etkilesimini kontrol eder.
int hit_paddle() {// eğer top raketin içine düşerse 1 döndürür ve update_ball'daki kontrolde topun yönü güncellenir.
    if (ball_dir_y == 1 && ball_y == ROWS - 2 && ball_x >= paddle_x && ball_x < paddle_x + PADDLE_LEN) {
        return 1;
    } else {
        return 0;
    }
}





//Topun tuglalarla olan etkilesimini kontrol eder.
int hit_brick(int x, int y) {
    if (board[y][x] == '#' ) {// eğer yeni konumda tuğla varsa siler ve 1 döndürür. update_ball'd< ise topun yönünü güncelleyen kontrol yapılır.
        board[y][x] = ' ';
        brickCreated--;
        score+=10;
        return 1;
    } else {
        return 0;
    }


}



//oyunu tekrar başlatır
void reset_game(){
    while(1){
            if (_kbhit()) {//Enter tuşuna basılması halinde oyun resetlenir. ESC'ye basılırsa oyundan çıkılır.s
                if (GetAsyncKeyState(VK_RETURN)!= 0) {
                        bricks_left = ROWS * COLS /30 +13 ;
                        brickCreated = ROWS * COLS /30 + 13;
                        lives=3;
                        score=0;
                        main();
                }if (GetAsyncKeyState(VK_ESC)!= 0) {
                        break;
                }
            }
        }
}

