//complete
#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <softTone.h>
#include <string.h>
#include <stdlib.h>
#include <lcd.h>

// 키패드 버튼 정의
#define B1 1
#define B2 2
#define B3 4
#define B4 8
#define B5 16
#define B6 32
#define B7 64
#define B8 128
#define B9 256
#define BS 512
#define B0 1024
#define BH 2048

// 키패드 핀 정의
#define KEYPAD_PB1 23
#define KEYPAD_PB2 24
#define KEYPAD_PB3 25
#define KEYPAD_PB5 8
#define KEYPAD_PB6 7
#define KEYPAD_PB7 12
#define KEYPAD_PB9 16
#define KEYPAD_PB10 20
#define KEYPAD_PB11 21
#define KEYPAD_PB13 13
#define KEYPAD_PB14 19
#define KEYPAD_PB15 26
#define MAX_KEY_BT_NUM 12

// 서보 모터 및 부저 핀 정의
#define SERVO 18
#define BUZZER_PIN 17
#define DO_L 523
#define RE 587
#define MI 659
#define FA 698
#define SOL 784
#define RA 880
#define SI 987
#define DO_H 1046

// 비밀번호 길이 및 실패 횟수 정의
#define PW_LEN 4
#define FAIL_CNT 3

const int KeypadTable[MAX_KEY_BT_NUM] = {
    KEYPAD_PB1, KEYPAD_PB2, KEYPAD_PB3, KEYPAD_PB5, KEYPAD_PB6, KEYPAD_PB7,
    KEYPAD_PB9, KEYPAD_PB10, KEYPAD_PB11, KEYPAD_PB13, KEYPAD_PB14, KEYPAD_PB15
};

// 키패드의 상태를 읽어오는 함수
int KeypadRead(void) {
    int nKeypadstate = 0;

    for (int i = 0; i < MAX_KEY_BT_NUM; i++) {
        if (!digitalRead(KeypadTable[i])) {
            nKeypadstate |= (1 << i);
        }
    }

    return nKeypadstate;
}

// 각 음계에 해당하는 주파수 값을 선택하는 함수
unsigned int SevenScale(unsigned char scale) {
    unsigned int _ret = 0;

    switch (scale) {
    case 0:
        _ret = DO_L;
        break;
    case 1:
        _ret = RE;
        break;
    case 2:
        _ret = MI;
        break;
    case 3:
        _ret = FA;
        break;
    case 4:
        _ret = SOL;
        break;
    case 5:
        _ret = RA;
        break;
    case 6:
        _ret = SI;
        break;
    case 7:
        _ret = DO_H;
        break;
    }

    return _ret;
}

// 부저 주파수 변경 함수
void Change_FREQ(unsigned int freq) {
    softToneWrite(BUZZER_PIN, freq);
}

// 부저 정지 함수
void STOP_FREQ(void) {
    softToneWrite(BUZZER_PIN, 0);
}

// 부저 초기화 함수
void Buzzer_Init(void) {
    softToneCreate(BUZZER_PIN);
    STOP_FREQ();
}

// 문 열기 함수
void door_open(int mute) {
    printf("Door Open\n");
    softPwmWrite(SERVO, 25);

    if (!mute) {
        for (int i = 0; i < 3; i++) {
            Change_FREQ(SevenScale(i));
            delay(200);
            STOP_FREQ();
        }
    }

    delay(2000);
    softPwmWrite(SERVO, 5);

    if (!mute) {
        for (int i = 2; i >= 0; i--) {
            Change_FREQ(SevenScale(i));
            delay(200);
            STOP_FREQ();
        }
    }
}

// 비밀번호 실패 시 소리와 메시지 출력 함수
void pw_fail(int fail, int mute) {
    printf("\nIncorrect Password\n ");

    if (!mute) {
        for (int i = 0; i < fail; i++) {
            Change_FREQ(SevenScale(7));
            delay(200);
            STOP_FREQ();
            delay(200);
        }
    }

    if (fail >= FAIL_CNT) {
        printf("\n(Input is limited for 10 seconds)\n");
        delay(10000);
    }
}

// 배열 출력 함수
void printint(int *arr) {
    printf("\npw print\n");

    for (int i = 0; i < PW_LEN; i++) {
        printf("\n%d\n ", arr[i]);
    }
}

// 비밀번호 비교 함수
int pwcmp(int *a, int *b) {
    if (a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3])
        return 1;
    else
        return 0;
}

// LCD 시작 화면 출력 함수
void startlcd(int disp1, int lcd_stat) {
    if (lcd_stat % 4 == 0) {
        lcdClear(disp1);
        lcdPosition(disp1, 0, 0);
        lcdPuts(disp1, "(1)Input PW");

        lcdPosition(disp1, 0, 1);
        lcdPuts(disp1, "(2)Set PW");

        lcdPosition(disp1, 15, 0);
        lcdPuts(disp1, "|");

        lcdPosition(disp1, 15, 1);
        lcdPuts(disp1, "V");

        delay(100);
    } else if (lcd_stat % 4 == 1) {
        lcdPosition(disp1, 0, 0);
        lcdPuts(disp1, "(2)Set PW");

        lcdPosition(disp1, 0, 1);
        lcdPuts(disp1, "(3)Change PW");
        lcdPosition(disp1, 15, 0);
        lcdPuts(disp1, "|");

        lcdPosition(disp1, 15, 1);
        lcdPuts(disp1, "V");

        delay(100);

    } else if (lcd_stat % 4 == 2) {
        lcdPosition(disp1, 0, 0);
        lcdPuts(disp1, "(3)Change PW");

        lcdPosition(disp1, 0, 1);
        lcdPuts(disp1, "(4)Sound On/Off");
        lcdPosition(disp1, 15, 0);
        lcdPuts(disp1, "|");

        lcdPosition(disp1, 15, 1);
        lcdPuts(disp1, "V");
        delay(100);
    } else if (lcd_stat % 4 == 3) {
        lcdPosition(disp1, 0, 0);
        lcdPuts(disp1, "(4)Sound On/Off");

        lcdPosition(disp1, 0, 1);
        lcdPuts(disp1, "(1)Input PW");
        lcdPosition(disp1, 15, 0);
        lcdPuts(disp1, "|");

        lcdPosition(disp1, 15, 1);
        lcdPuts(disp1, "V");
        delay(100);
    }
}

// 버튼 소리 출력 함수
void btnsound(int mute) {
    if (mute) {
        delay(200);
        return;
    } else {
        Change_FREQ(SevenScale(2));
        delay(200);
        STOP_FREQ();
    }
}

// LCD 비밀번호 입력 화면 출력 함수
void lcd_inputpw(int disp1, int t) {
    lcdClear(disp1);
    lcdPosition(disp1, 0, 0);
    lcdPuts(disp1, "1.Input PW");

    for (int i = 0; i < t; i++) {
        lcdPosition(disp1, i, 1);
        lcdPuts(disp1, "*");
    }

    delay(100);
}

// LCD 문 열림 화면 출력 함수
void lcd_dooropen(int disp1) {
    lcdClear(disp1);
    lcdPosition(disp1, 0, 0);
    lcdPuts(disp1, "Door opened");
    delay(100);
}

// LCD 비밀번호 실패 화면 출력 함수
void lcd_fail(int disp1, int fail) {
    if (fail >= FAIL_CNT) {
        lcdClear(disp1);
        lcdPosition(disp1, 0, 0);
        lcdPuts(disp1, "no more!");
        lcdPosition(disp1, 0, 1);
        lcdPuts(disp1, "Retry after 10sec");
        delay(100);
    } else {
        lcdClear(disp1);
        lcdPosition(disp1, 0, 0);
        lcdPuts(disp1, "Invalid password");
        delay(200);
    }
}

// LCD 비밀번호 설정 화면 출력 함수
void lcd_setpw(int disp1, int t) {
    lcdClear(disp1);
    lcdPosition(disp1, 0, 0);
    lcdPuts(disp1, "2.Set PW");

    for (int i = 0; i < t; i++) {
        lcdPosition(disp1, i, 1);
        lcdPuts(disp1, "*");
    }

    delay(100);
}

// LCD 비밀번호 변경 화면 출력 함수
void lcd_changepw(int disp1, int t, int j) {
    lcdClear(disp1);
    lcdPosition(disp1, 0, 0);
    lcdPuts(disp1, "3.Change PW");
    lcdPosition(disp1, 0, 1);
    lcdPuts(disp1, "As is:");

    for (int i = 0; i < t; i++) {
        lcdPosition(disp1, i + 6, 1);
        lcdPuts(disp1, "*");
    }

    delay(100);
}

// LCD 변경할 비밀번호 화면 출력 함수
void lcd_tobe(int disp1, int t, int j) {
    lcdClear(disp1);
    lcdPosition(disp1, 0, 0);
    lcdPuts(disp1, "As is:");

    for (int i = 0; i < t; i++) {
        lcdPosition(disp1, i + 6, 0);
        lcdPuts(disp1, "*");
    }

    lcdPosition(disp1, 0, 1);
    lcdPuts(disp1, "To be:");

    for (int i = 0; i < j; i++) {
        lcdPosition(disp1, i + 6, 1);
        lcdPuts(disp1, "*");
    }

    delay(100);
}

// LCD 소리 설정 화면 출력 함수
void lcd_sound(int disp1, int mute) {
    lcdClear(disp1);
    lcdPosition(disp1, 0, 0);

    if (!mute) {
        lcdPuts(disp1, "Sound off");
    } else {
        lcdPuts(disp1, "Sound on");
    }

    delay(2000);
}

// LCD 완료 화면 출력 함수
void lcd_done(int disp1) {
    lcdClear(disp1);
    lcdPosition(disp1, 0, 0);
    lcdPuts(disp1, "Done");
    delay(2000);
}

int main() {
    if (wiringPiSetupGpio() == -1)
        return 1;
    
    // LCD 초기화
    int disp1;
    disp1 = lcdInit(2, 16, 4, 2, 4, 10, 9, 27, 22, 0, 0, 0, 0);
    lcdClear(disp1);    
    
    // 서보 모터 및 부저 초기화
    softPwmCreate(SERVO, 0, 200);
    Buzzer_Init();

    // 변수 및 배열 초기화
    int past_key = 0;
    int past_num_1 = 0;
    int past_num_2 = 0;
    int past_num_3 = 0;
    int past_num_4 = 0;
    int *pw = calloc(PW_LEN, sizeof(int));
    int *buf = calloc(PW_LEN, sizeof(int));
    int tok = 0; // 3번 비밀번호 변경 항목에 대한 카운트
    int t = 0;  // 입력에 대한 카운트
    int fail = 0; // 비밀번호 오답에 대한 카운트
    int lcd_stat = 0; // LCD의 스크롤 정보
    int mute = 0; // 소리 정보
    pw[0] = B0;
    pw[1] = B0;
    pw[2] = B0;
    pw[3] = B0;
    
    // 키패드 핀을 입력으로 설정
    for (int i = 0; i < MAX_KEY_BT_NUM; i++) {
        pinMode(KeypadTable[i], INPUT);
    }

    printint(pw);
    
    // 메인 루프
    while (1) {
        startlcd(disp1, lcd_stat);
        int key = KeypadRead(); // 키를 계속 인식
        tok = 0;
        
        // 키가 눌렸다 떼지는 순간 동작
        if (key != 0 && past_key == 0) { 
            if (key != B4) btnsound(mute);
            if (key == BS) {
                lcd_stat++; // 별 버튼은 아래로 스크롤
            }
            if (key == B1) { // 1번 메뉴는 PW 입력 모드
                t = 0;
                printf("\n(1)Input PW:\n");
                while (1) {
                    lcd_inputpw(disp1, t);
                    int num_1 = KeypadRead();
                    // 숫자 버튼이 인식된 경우
                    if (past_num_1 == 0 && num_1 != 0 && num_1 != BS && t < PW_LEN) {
                        btnsound(mute);
                        buf[t] = num_1;
                        printf("\nt:%d key:(%d) b:[%d]\n", t, num_1, buf[t]);
                        t++;
                    }
                    // 해시 버튼이 인식된 경우
                    else if (past_num_1 == 0 && num_1 != 0 && num_1 == BH && t == PW_LEN) {
                        t = 0;
                        if (pwcmp(pw, buf) == 1) {
                            fail = 0;
                            lcd_dooropen(disp1);
                            door_open(mute); // 문이 열림
                            break;
                        }
                        else {
                            fail++;
                            lcd_fail(disp1, fail);
                            pw_fail(fail, mute); // 틀린 경우 fail 카운터 증가
                            if (fail >= FAIL_CNT) break;
                        }
                    }
                    past_num_1 = num_1;
                }
            }
            else if (key == B2) { // 2번 메뉴는 비밀번호 초기화
                memset(pw, 0, PW_LEN * sizeof(int)); // 비밀번호 0으로 초기화
                printf("\nInput new password >> \n");
                while (1) {
                    int num_2 = KeypadRead();
                    lcd_setpw(disp1, t);
                    // 해시 버튼이 인식된 경우
                    if (num_2 == BH && past_num_2 == 0 && t == PW_LEN) {
                        btnsound(mute);
                        lcd_done(disp1);
                        printf("\nPassword setup complete\n");
                        printint(pw); // 입력한 비밀번호를 출력
                        t = 0;
                        break;
                    }
                    // 숫자 버튼이 인식된 경우
                    if (num_2 != 0 && past_num_2 == 0 && num_2 != BS && t < PW_LEN) {
                        btnsound(mute);
                        pw[t] = num_2;
                        printf("\nt:%d num:(%d) pw:[%d]\n", t, num_2, pw[t]);
                        t++;
                    }
                    past_num_2 = num_2;
                }
            }
            else if (key == B3) { // 3번 메뉴는 비밀번호 변경
                t = 0;
                printf("\n(3)Change password >> \nAs is:\n");
                while (tok != 2) {
                    int num_3 = KeypadRead();
                    lcd_changepw(disp1, t, 0);
                    // 숫자 버튼이 인식된 경우
                    if (past_num_3 == 0 && num_3 != 0 && t < PW_LEN && num_3 != BS && num_3 != BH) {
                        btnsound(mute);
                        buf[t] = num_3;
                        printf("\n*\n");
                        t++;
                    }
                    // 4자리 입력이 완료된 경우
                    else if (t == 4) {
                        int j = 0;
                        t++;
                        if (pwcmp(pw, buf) == 1) 
                            tok = 1;
                        else 
                            tok = 0;
                        printf("%d", tok);
                        
                        printf("\nTo be\n");
                        while (1) {
                            lcd_tobe(disp1, 4, j);
                            int num_4 = KeypadRead();
                            // 숫자 버튼이 인식된 경우
                            if (past_num_4 == 0 && num_4 != 0 && j < PW_LEN && num_4 != BS && num_4 != BH) {
                                btnsound(mute);
                                buf[j] = num_4;
                                printf("\n*\n");
                                j++;
                            }
                            // 변경 완료
                            else if (j >= 4 && tok == 1) {
                                for (int i = 0; i < PW_LEN; i++) {
                                    pw[i] = buf[i];
                                }
                                lcd_done(disp1);
                                printf("\nDone\n");
                                tok = 2;
                                printf("%d", tok);
                                printint(pw);
                                break;
                            }
                            // 변경 실패
                            else if (j >= 4 && tok == 0) {
                                lcd_fail(disp1, fail);
                                printf("\nInvalid PW\n");
                                tok = 2;
                                printf("%d", tok);
                                delay(2000);
                                break;
                            }
                            past_num_4 = num_4;
                        }
                    }
                    past_num_3 = num_3;
                }
            }
            else if (key == B4) { // 음소거 버튼
                lcd_sound(disp1, mute);
                mute = !mute;
                if (mute) {
                    printf("\nSound off\n");
                }
                else {
                    printf("\nSound on\n");
                }
                btnsound(mute);
            }
        }
        
        past_key = key;
    }

    free(buf);
    free(pw);
    return 0;
}
