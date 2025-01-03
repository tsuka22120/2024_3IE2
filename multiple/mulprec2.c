#include "mulprec2.h"

#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "mt19937ar.c"

/// @brief 構造体の中身を0で初期化する
/// @param a 初期化する構造体
void clearByZero(Number *a) {
    int i;
    for (i = 0; i < KETA; i++) {
        a->n[i] = 0;
    }
    setSign(a, ZERO);
}

/// @brief 数値を表示する
/// @param a 表示する構造体
void dispNumber(const Number *a) {
    char format[6];
    switch (getSign(a)) {
        case PLUS:
            printf("+");
            break;
        case ZERO:
            printf("+ 0");
            return;
        case MINUS:
            printf("-");
            break;
    }
    sprintf(format, " %%0%dd", RADIX_LEN);  // format = " %02d"
    for (int i = KETA - 1; i >= 0; i--) {
        printf(format, a->n[i]);
    }
}

/// @brief 先頭の0を抜いて表示する
/// @param a 表示する構造体
void dispNumberZeroSuppress(const Number *a) {
    int i;
    char format[6];
    sprintf(format, " %%0%dd", RADIX_LEN);  // format = " %02d"
    switch (getSign(a)) {
        case PLUS:
            printf("+");
            break;
        case ZERO:
            printf("+ 0");
            return;
        case MINUS:
            printf("-");
            break;
    }
    for (i = KETA - 1; i >= 0; i--) {
        if (a->n[i] > 0) {
            break;
        }
    }
    for (; i >= 0; i--) {
        printf(format, a->n[i]);
    }
}

/// @brief n進数で表示する
/// @param a 表示する構造体
/// @param n n進数(1~36)
void dispNumberInB(const Number *a, int n) {
    if (n < 1 || n > 36) {
        printf("ERROR:invalid number\n");
        return;
    }
    Number A, y, numN;
    int numY, i;
    char str[KETA + 1];
    copyNumber(&A, a);
    setInt(&numN, n);
    switch (getSign(a)) {
        case PLUS:
            printf("+");
            break;
        case ZERO:
            printf("+ 0");
            return;
        case MINUS:
            printf("-");
            break;
    }
    i = 0;
    while (1) {
        if (isZero(&A) == 1) {
            str[i] = '\0';
            break;
        }
        divide(&A, &numN, NULL, &y);
        getInt(&y, &numY);
        str[i] = (numY < 10) ? numY + '0' : numY - 10 + 'A';
        sub(&A, &y, &A);
        divide(&A, &numN, &A, NULL);
        i++;
    }
    for (i = i - 1; i >= 0; i--) {
        printf(" %c", str[i]);
    }
}

/// @brief 乱数を生成して代入する
/// @param a 乱数を代入する構造体のポインタ
/// @param k 乱数の桁数
void setRnd(Number *a, int k) {
    clearByZero(a);
    if (k > KETA) {
        printf("ERROR:random number overflow\n");
        return;
    }
    int i;
    for (i = 0; i < k; i++) {
        a->n[i] = genrand_int32() % RADIX;
    }
    for (i = k; i >= 0; i--) {
        if (a->n[i] != 0) {
            break;
        }
        if (i == 0) {
            setSign(a, 0);
            return;
        }
    }
    if (genrand_int32() % 2 == 0) {
        setSign(a, 1);
    } else {
        setSign(a, -1);
    }
}

/// @brief 値をコピーする
/// @param a コピー先
/// @param b コピー元
void copyNumber(Number *a, const Number *b) { *a = *b; }

/// @brief 絶対値を求める
/// @param a 絶対値を求める構造体
/// @param b 絶対値を代入する構造体
void getAbs(const Number *a, Number *b) {
    copyNumber(b, a);
    if (getSign(a) == ZERO) {
        setSign(b, ZERO);
    } else {
        setSign(b, PLUS);
    }
}

/// @brief aが0かどうかを判定する
/// @param a 判定する構造体
/// @return true: 0, false: 0でない
int isZero(const Number *a) {
    if (getSign(a) == ZERO) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/// @brief aを10倍してbに代入する
/// @param a 10倍する構造体
/// @param b 10倍した値を代入する構造体
/// @return 0: 正常終了, -1: オーバーフロー
int mulBy10(const Number *a, Number *b) {
    int rtn;
    if (isZero(a)) {
        copyNumber(b, a);
        rtn = 0;
    } else {
        int i;
        Number tmp;
        copyNumber(&tmp, a);
        clearByZero(b);
        if (tmp.n[KETA - 1] != 0) {
            rtn = -1;
        } else {
            for (i = 0; i < KETA - 1; i++) {
                b->n[i + 1] = tmp.n[i];
            }
            b->n[0] = 0;
            setSign(b, getSign(&tmp));
            rtn = 0;
        }
    }
    return rtn;
}

/// @brief aを10で割ってbに代入する
/// @param a 10で割る構造体
/// @param b 10で割った値を代入する構造体
/// @return 剰余
int divBy10(const Number *a, Number *b) {
    int i;
    int rtn;
    for (i = 0; i < KETA - 1; i++) {
        b->n[i] = a->n[i + 1];
    }
    b->n[KETA - 1] = 0;
    setSign(b, getSign(a));
    rtn = a->n[0];
    return rtn;
}
/// @brief int型の値を構造体に代入する
/// @param a 代入する構造体
/// @param x 代入する値
/// @return 成功: 0, エラー(overflow): -1
int setInt(Number *a, long x) {
    clearByZero(a);
    int i;
    int r;
    if (x < 0) {
        setSign(a, MINUS);
        x *= -1;
    } else if (x == 0) {
        setSign(a, ZERO);
    } else {
        setSign(a, PLUS);
    }
    for (i = 0; i < KETA; i++) {
        r = x % RADIX;
        a->n[i] = r;
        x -= r;
        x /= RADIX;
        if (x == 0) {
            break;
        }
    }
    if (x != 0) {
        return -1;
    }
    return 0;
}

/// @brief 構造体の中身をint型に変換する
/// @param a 値を読み取る構造体
/// @param x int型に変換した値を代入する変数
/// @return 成功: 0, エラー(overflow): -1
int getInt(const Number *a, int *x) {
    int i;
    if (getSign(a) == ZERO) {
        *x = 0;
        return 0;
    }
    *x = 0;
    for (i = KETA - 1; i >= 0; i--) {
        *x += a->n[i] * (long)pow(RADIX, i);  // 基数を考慮してint型に変換
    }
    if (getSign(a) == MINUS) {
        *x *= -1;
    }
    return 0;
}

/// @brief 符号を設定する
/// @param a 符号を設定する構造体
/// @param s 1: 正, 0: 0, -1: 負
/// @return 成功: 0, エラー: -1
int setSign(Number *a, int s) {
    switch (s) {
        case PLUS:
            a->sign = PLUS;
            break;
        case ZERO:
            a->sign = ZERO;
            break;
        case MINUS:
            a->sign = MINUS;
            break;
        default:
            return -1;
    }
    return 0;
}

/// @brief 符号を取得する
/// @param a 符号を取得する構造体
/// @return 1: 正, 0: 0, -1: 負
int getSign(const Number *a) { return a->sign; }

/// @brief 2つの多倍長整数を比較する
/// @param a 比較する構造体
/// @param b 比較する構造体
/// @return 1: a > b, 0: a = b, -1: a < b
int numComp(const Number *a, const Number *b) {
    int rtn = 0;
    switch (getSign(a) * 3 + getSign(b)) {
        case -4:  // aとbが負
            for (int i = KETA - 1; i >= 0; i--) {
                if (a->n[i] < b->n[i]) {
                    rtn = 1;
                    break;
                } else if (a->n[i] > b->n[i]) {
                    rtn = -1;
                    break;
                }
            }
            break;
        case 4:  // aとbが正
            for (int i = KETA - 1; i >= 0; i--) {
                if (a->n[i] > b->n[i]) {
                    rtn = 1;
                    break;
                } else if (a->n[i] < b->n[i]) {
                    rtn = -1;
                    break;
                }
            }
            break;
        case -3:  // aが負でbが0
        case -2:  // aが負でbが正
        case 1:   // aが0でbが正
            rtn = -1;
            break;
        case -1:  // aが0でbが負
        case 2:   // aが正でbが負
        case 3:   // aが正でbが0
            rtn = 1;
            break;
        case 0:  // aとbが0
            break;
    }
    return rtn;
#if 0
    if (getSign(a) > getSign(b)) {
        return 1;
    } else if (getSign(a) < getSign(b)) {
        return -1;
    }
    if (getSign(a) == getSign(b) && getSign(a) == 0) {
        return 0;
    }
    if (getSign(a) == getSign(b) && getSign(a) == 1) {
        for (i = KETA - 1; i >= 0; i--) {
            if (a->n[i] > b->n[i]) {
                return 1;
            } else if (a->n[i] < b->n[i]) {
                return -1;
            }
        }
        return 0;
    } else if (getSign(a) == getSign(b) && getSign(a) == -1) {
        for (int i = KETA - 1; i >= 0; i--) {
            if (a->n[i] < b->n[i]) {
                return 1;
            } else if (a->n[i] > b->n[i]) {
                return -1;
            }
        }
        return 0;
    }
    return 0;
#endif
    return 0;
}

/// @brief 2つの多倍長整数を入れ替える
/// @param a 入れ替える構造体
/// @param b 入れ替える構造体
void swap(Number *a, Number *b) {
    Number tmp;
    copyNumber(&tmp, a);
    copyNumber(a, b);
    copyNumber(b, &tmp);
}

/// @brief 2つの多倍長整数を加算する(同じ変数を関数内に入れてはいけない)
/// @param a 加算する構造体
/// @param b 加算する構造体
/// @param c 加算した値を代入する構造体
/// @return オーバーフロー: -1, 正常終了: 0
int add(const Number *a, const Number *b, Number *c) {
    Number A, B;
    copyNumber(&A, a);
    copyNumber(&B, b);
    clearByZero(c);
    int i, rtn;
    int d, e = 0;
    int caseNum = getSign(&A) * 3 + getSign(&B);
    switch (caseNum) {
        case -4:  // aとbが負
        case 4:   // aとbが正
            if (caseNum == -4) {
                getAbs(&A, &A);
                getAbs(&B, &B);
                setSign(c, MINUS);
            } else {
                setSign(c, PLUS);
            }
            for (i = 0; i < KETA; i++) {
                d = A.n[i] + B.n[i] + e;
                if (d >= RADIX) {
                    d -= RADIX;
                    e = 1;
                } else {
                    e = 0;
                }
                c->n[i] = d;
            }
            if (e > 0) {
                rtn = -1;
            } else {
                rtn = 0;
            }
            break;
        case -3:  // aが負でbが0
        case 3:   // aが正でbが0
            copyNumber(c, &A);
            rtn = 0;
            break;
        case -2:  // aが負でbが正
            getAbs(&A, &A);
            rtn = sub(&B, &A, c);
            break;
        case -1:  // aが0でbが負
        case 1:   // aが0でbが正
            copyNumber(c, &B);
            rtn = 0;
            break;
        case 0:  // aとbが0
            clearByZero(c);
            rtn = 0;
            break;
        case 2:  // aが正でbが負
            getAbs(&B, &B);
            rtn = sub(&A, &B, c);
            break;
    }
    return rtn;
}

/// @brief 2つの多倍長整数を減算する(同じ変数を関数内に入れてはいけない)
/// @param a 減算する構造体
/// @param b 減算する構造体
/// @param c 減算した値を代入する構造体
/// @return オーバーフロー: -1, 正常終了: 0
int sub(const Number *a, const Number *b, Number *c) {
    Number A, B;
    copyNumber(&A, a);
    copyNumber(&B, b);
    clearByZero(c);
    int i, e, num, rtn;
    int caseNum = getSign(&A) * 3 + getSign(&B);
    Number numA, numB;
    switch (caseNum) {
        case -4:  // aとbが負
        case 4:   // aとbが正
            if (caseNum == -4) {
                getAbs(&A, &numB);
                getAbs(&B, &numA);
            } else {
                copyNumber(&numA, &A);
                copyNumber(&numB, &B);
            }
            e = 0;
            if (numComp(&A, &B) == 1) {
                for (i = 0; i < KETA; i++) {
                    num = numA.n[i] - e;
                    if (num < numB.n[i]) {
                        c->n[i] = num + RADIX - numB.n[i];
                        e = 1;
                    } else {
                        c->n[i] = num - numB.n[i];
                        e = 0;
                    }
                    setSign(c, PLUS);
                }
            } else if (numComp(&A, &B) == -1) {
                for (i = 0; i < KETA; i++) {
                    num = numB.n[i] - e;
                    if (num < numA.n[i]) {
                        c->n[i] = num + RADIX - numA.n[i];
                        e = 1;
                    } else {
                        c->n[i] = num - numA.n[i];
                        e = 0;
                    }
                }
                setSign(c, MINUS);
            } else {
                setSign(c, ZERO);
            }
            if (e > 0) {
                rtn = -1;
            } else {
                rtn = 0;
            }
            break;
        case -3:  // aが負でbが0
        case 3:   // aが正でbが0
            copyNumber(c, &A);
            rtn = 0;
            break;
        case -2:  // aが負でbが正
            getAbs(&A, &A);
            rtn = add(&A, &B, c);
            setSign(c, MINUS);
            break;
        case -1:  // aが0でbが負
            copyNumber(c, &B);
            setSign(c, PLUS);
            rtn = 0;
            break;
        case 1:  // aが0でbが正
            copyNumber(c, &B);
            setSign(c, MINUS);
            rtn = 0;
            break;
        case 0:  // aとbが0
            clearByZero(c);
            rtn = 0;
            break;
        case 2:  // aが正でbが負
            getAbs(&B, &B);
            rtn = add(&A, &B, c);
            break;
    }
    return rtn;
}

/// @brief 多倍長整数をインクリメントする
/// @param a インクリメントする構造体
/// @param b インクリメントした値を代入する構造体
/// @return オーバーフロー: -1, 正常終了: 0
int increment(Number *a, Number *b) {
    Number one;
    int r;
    setInt(&one, 1);
    r = add(a, &one, b);
    return r;
}

/// @brief 多倍長整数をデクリメントする
/// @param a デクリメントする構造体
/// @param b デクリメントした値を代入する構造体
/// @return オーバーフロー: -1, 正常終了: 0
int decrement(Number *a, Number *b) {
    Number one;
    int r;
    setInt(&one, 1);
    r = sub(a, &one, b);
    return r;
}

/// @brief 2つの整数を掛け算する
/// @param a 掛け算する値
/// @param b 掛け算する値
/// @param c 掛け算した値を代入するポインタ
/// @return オーバーフロー: -1, 正常終了: 0
int simpleMultiple(int a, int b, int *c) {
    *c = 0;
    while (1) {
        if (INT_MAX - a < b) {
            return -1;
        }
        if (b == 0) {
            break;
        }
        *c += a;
        b--;
    }
    return 0;
}

/// @brief 2つの多倍長整数を掛け算する
/// @param a 掛け算する構造体
/// @param b 掛け算する構造体
/// @param c 掛け算した値を代入する構造体
/// @return オーバーフロー: -1, 正常終了: 0
int multiple(const Number *a, const Number *b, Number *c) {
    int rtn = 0;
    if (getSign(a) == ZERO || getSign(b) == ZERO) {
        clearByZero(c);
    } else {
        int numA, numB, signA, signB;
        Number tmp, A, B, D;
        int h, e, d = 0;
        signA = getSign(a);
        signB = getSign(b);
        getAbs(a, &A);
        getAbs(b, &B);
        clearByZero(c);
        for (int i = 0; i < KETA - 1; i++) {
            numB = B.n[i];
            h = 0;
            if (numB == 0) {
                continue;
            } else if (numB == 1) {
                copyNumber(&tmp, &A);
                for (int j = 0; j < i; j++) {
                    mulBy10(&tmp, &tmp);
                }
                add(c, &tmp, c);
            } else {
                for (int j = 0; j < KETA - 1; j++) {
                    numA = A.n[j];
                    e = numA * numB + h;
                    d = e % RADIX;
                    h = e / RADIX;
                    setInt(&D, d);
                    for (int k = 0; k < j + i; k++) {
                        if (mulBy10(&D, &D) == -1) {
                            clearByZero(c);
                            rtn = -1;
                        }
                    }
                    if (add(c, &D, c) == -1) {
                        clearByZero(c);
                        rtn = -1;
                    }
                }
                if (h != 0) {
                    rtn = -1;
                }
                if (rtn == -1) {
                    break;
                }
            }
        }
        if (rtn == 0) {
            switch (signA * 3 + signB) {
                case -4:  // aとbが負
                case 4:   // aとbが正
                    setSign(c, PLUS);
                    break;
                case -2:  // aが負でbが正
                case 2:   // aが正でbが負
                    setSign(c, MINUS);
                    break;
                    // case 3:,case 1:,case 0:,case -1:,case
                    // -3:は最初で判定しているのでここには来ない
            }
        }
    }
    return rtn;
}

/// @brief 2つの整数を割り算する
/// @param x 被乗数
/// @param y 除数
/// @param z 商を代入するポインタ
/// @param w 余りを代入するポインタ
/// @return ゼロ除算: -1, 正常終了: 0
int simpleDivide(int x, int y, int *z, int *w) {
    int rtn;
    if (y == 0) {
        rtn = -1;
    } else {
        int k = 0;
        int zSign, wSign;
        switch ((x < 0) * 2 + (y < 0)) {
            case 0:  // xが正でyが正
                zSign = PLUS;
                wSign = PLUS;
                break;
            case 1:  // xが正でyが負
                y *= -1;
                zSign = MINUS;
                wSign = PLUS;
                break;
            case 2:  // xが負でyが正
                x *= -1;
                zSign = MINUS;
                wSign = MINUS;
                break;
            case 3:  // xが負でyが負
                x *= -1;
                y *= -1;
                zSign = PLUS;
                wSign = MINUS;
                break;
        }
        while (1) {
            if (x < y) {
                break;
            }
            x -= y;
            k++;
        }
        *z = k * zSign;
        *w = x * wSign;
        rtn = 0;
    }
    return rtn;
}

/// @brief 2つの多倍長整数を割り算する
/// @param a 被除数
/// @param b 除数
/// @param c 商を代入する構造体
/// @param d 余りを代入する構造体
/// @return ゼロ除算: -1, 正常終了: 0
int divide(const Number *a, const Number *b, Number *c, Number *d) {
    int rtn = -1000;  // 何もしていない状態
    if (isZero(b)) {
        rtn = -1;
    } else {
        Number one;
        setInt(&one, 1);
        switch ((c == NULL) + (d == NULL) * 2) {
            case 0:  // 商も余りも必要な場合
                if (isZero(a)) {
                    clearByZero(c);
                    clearByZero(d);
                    rtn = 0;
                } else {
                    int cSign, dSign;
                    Number A, B, numB, q;
                    getAbs(a, &A);
                    getAbs(b, &B);
                    clearByZero(c);
                    clearByZero(d);
                    switch ((getSign(a) < 0) * 2 + (getSign(b) < 0)) {
                        case 0:
                            cSign = 1;
                            dSign = 1;
                            break;
                        case 1:
                            cSign = -1;
                            dSign = 1;
                            break;
                        case 2:
                            cSign = -1;
                            dSign = -1;
                            break;
                        case 3:
                            cSign = 1;
                            dSign = -1;
                            break;
                    }
                    while (rtn != 0) {
                        switch (numComp(&A, &B)) {
                            case -1:
                                if (numComp(&A, &one) ==
                                    -1) {  // 余りがゼロの時
                                    clearByZero(d);
                                } else {
                                    copyNumber(d, &A);
                                    d->sign = dSign;
                                }
                                c->sign = cSign;
                                rtn = 0;
                                break;
                            default:
                                copyNumber(&q, &one);
                                copyNumber(&numB, &B);
                                while (1) {
                                    mulBy10(&numB, &numB);
                                    mulBy10(&q, &q);
                                    if (numComp(&numB, &A) == 1) {
                                        divBy10(&numB, &numB);
                                        divBy10(&q, &q);
                                        break;
                                    }
                                }
                                sub(&A, &numB, &A);
                                add(c, &q, c);
                                break;
                        }
                    }
                }
                break;
            case 1:  // 商が不要で余りが必要な場合
                if (isZero(a)) {
                    clearByZero(d);
                    rtn = 0;
                } else {
                    int dSign;
                    Number A, B, numB, q;
                    getAbs(a, &A);
                    getAbs(b, &B);
                    clearByZero(d);
                    switch ((getSign(a) < 0) * 2 + (getSign(b) < 0)) {
                        case 0:
                        case 1:
                            dSign = 1;
                            break;
                        case 2:
                        case 3:
                            dSign = -1;
                            break;
                    }
                    while (rtn != 0) {
                        switch (numComp(&A, &B)) {
                            case -1:
                                if (numComp(&A, &one) ==
                                    -1) {  // 余りがゼロの時
                                    clearByZero(d);
                                } else {
                                    copyNumber(d, &A);
                                    d->sign = dSign;
                                }
                                rtn = 0;
                                break;
                            default:
                                copyNumber(&q, &one);
                                copyNumber(&numB, &B);
                                while (1) {
                                    mulBy10(&numB, &numB);
                                    if (numComp(&numB, &A) == 1) {
                                        divBy10(&numB, &numB);
                                        break;
                                    }
                                }
                                sub(&A, &numB, &A);
                                break;
                        }
                    }
                }
                break;
            case 2:  // 商が必要で余りが不要な場合
                if (isZero(a)) {
                    clearByZero(c);
                    rtn = 0;
                } else {
                    int cSign;
                    Number A, B, numB, q;
                    getAbs(a, &A);
                    getAbs(b, &B);
                    clearByZero(c);
                    switch ((getSign(a) < 0) * 2 + (getSign(b) < 0)) {
                        case 0:
                            cSign = 1;
                            break;
                        case 1:
                            cSign = -1;
                            break;
                        case 2:
                            cSign = -1;
                            break;
                        case 3:
                            cSign = 1;
                            break;
                    }
                    while (rtn != 0) {
                        switch (numComp(&A, &B)) {
                            case -1:
                                c->sign = cSign;
                                rtn = 0;
                                break;
                            default:
                                copyNumber(&q, &one);
                                copyNumber(&numB, &B);
                                while (1) {
                                    mulBy10(&numB, &numB);
                                    mulBy10(&q, &q);
                                    if (numComp(&numB, &A) == 1) {
                                        divBy10(&numB, &numB);
                                        divBy10(&q, &q);
                                        break;
                                    }
                                }
                                sub(&A, &numB, &A);
                                add(c, &q, c);
                                break;
                        }
                    }
                }
                break;
        }
    }
    return rtn;
}

/// @brief 多倍長整数の平方根を求める
/// @param a 平方根を求める構造体
/// @param b 平方根を代入する構造体
/// @return エラー: -1, 正常終了: 0
int sqrt_mp(const Number *a, Number *b) {
    Number x;  //  現在の平方根の近似値
    Number c;  //  1つ前のx
    Number d;  //  2つ前のx
    Number tmp; //作業用変数
    setInt(&tmp, 1);
    if (getSign(a) == -1) {
        clearByZero(b);
        return -1;
    }  //  N<0 ならエラーで-1を返す
    if ((isZero(a)) || (numComp(a, &tmp) == 0)) {
        copyNumber(b, a);
        return 0;
    }  //  N=0 or 1なら \sqrt{N}=N

    setInt(&tmp, 2);            //  初期値
    divide(a, &tmp, &x, NULL);  //  x_{0}=N/2
    copyNumber(&c, &x);
    copyNumber(&d, &x);

    while (1) {
        copyNumber(&d, &c);          //  2つ前のx
        copyNumber(&c, &x);          //  1つ前のx
        divide(a, &c, &x, NULL);     //  x_{i+1}=N/x_{i}
        add(&c, &x, &x);             //  x_{i+1}=x_{i}+N/x_{i}
        divide(&x, &tmp, &x, NULL);  //  x_{i+1}=(x_{i}+N/x_{i})/2

        if (numComp(&x, &c) == 0) break;  //  収束
        if (numComp(&x, &d) == 0)         //  振動
        {
            if (numComp(&x, &c) == 1) copyNumber(&x, &c);  //  小さい方をとる
            break;
        }
    }
    copyNumber(b, &x);
    return 0;
}

/// @brief 多倍長整数の累乗を求める
/// @param a 累乗する構造体
/// @param n 累乗する値
/// @param b 累乗した値を代入する構造体
/// @return エラー: -1, 正常終了: 0
int power(const Number *a, int n, Number *b) {
    int i;
    if (n < 0) {
        clearByZero(b);
        return -1;
    }
    if (n == 0) {
        setInt(b, 1);
        return 0;
    }
    if (n == 1) {
        copyNumber(b, a);
        return 0;
    }
    Number one;
    setInt(&one, 1);
    if (isZero(a)) {
        clearByZero(b);
        return 0;
    }
    if (numComp(a, &one) == 0) {
        setInt(b, 1);
        return 0;
    }
    i = 1;
    copyNumber(b, a);
    while (1) {
        if (i == n) {
            break;
        }
        multiple(a, b, b);
        i++;
    }
    return 0;
}

/// @brief 再起的に整数の累乗を求める
/// @param x 底
/// @param n 指数
/// @return 累乗した値
int p_recursive(int x, int n) {
    if (n == 0) {
        printf("x: %d, n: %d\n", x, n);
        return 1;
    }
    if (n == 1) {
        printf("x: %d, n: %d\n", x, n);
        return x;
    }
    if (n % 2 == 0) {
        printf("x: %d, n: %d\n", x, n);
        return p_recursive(x * x, n / 2);
    } else {
        printf("x: %d, n: %d\n", x, n);
        return x * p_recursive(x, n - 1);
    }
}

/// @brief 再起的に多倍長整数の累乗を求める
/// @param a 底
/// @param n 指数
/// @param b 累乗した値を代入する構造体
/// @return エラー: -1, 正常終了: 0
int fastpower(const Number *a, int n, Number *b) {
    if (n < 0) {
        clearByZero(b);
        return -1;
    }
    if (n == 0) {
        setInt(b, 1);
        return 0;
    }
    if (n == 1) {
        copyNumber(b, a);
        return 0;
    }
    Number one;
    setInt(&one, 1);
    if (isZero(a)) {
        clearByZero(b);
        return 0;
    }
    if (numComp(a, &one) == 0) {
        setInt(b, 1);
        return 0;
    }
    while (1) {
        Number tmp;
        if (n % 2 == 0) {
            if (multiple(a, a, &tmp) == -1) {
                printf("ERROR:overflow\n");
                clearByZero(b);
                return -1;
            }
            fastpower(&tmp, n / 2, b);
        } else {
            fastpower(a, n - 1, b);
            if (multiple(a, b, b) == -1) {
                printf("ERROR:overflow\n");
                clearByZero(b);
                return -1;
            }
        }
        break;
    }
    return 0;
}

/// @brief 多倍長整数を階乗する
/// @param a 階乗する値
/// @param b 階乗した値を代入する構造体
/// @return エラー: -1, 正常終了: 0
int factorial(int a, Number *b) {
    if (a < 0) {
        clearByZero(b);
        return -1;
    }
    if (a == 0) {
        setInt(b, 1);
        return 0;
    }
    Number tmp;
    setInt(b, 1);
    for (int i = 1; i <= a; i++) {
        setInt(&tmp, i);
        if (multiple(b, &tmp, b) == -1) {
            printf("ERROR:overflow\n");
            clearByZero(b);
            return -1;
        }
    }
    return 0;
}

/// @brief 多倍長整数の最大公約数を求める
/// @param a 最大公約数を求める構造体
/// @param b 最大公約数を求める構造体b
/// @param c 最大公約数を代入する構造体
/// @return エラー: -1, 正常終了: 0
void gcd(const Number *a, const Number *b, Number *c) {
    if (isZero(a)) {
        copyNumber(c, b);
        return;
    } else if (isZero(b)) {
        copyNumber(c, a);
        return;
    }
    Number A, B, tmp;
    switch (numComp(a, b)) {
        case 1:
            getAbs(a, &A);
            getAbs(b, &B);
            break;
        case -1:
            getAbs(b, &A);
            getAbs(a, &B);
            break;
        case 0:
            copyNumber(c, a);
            if (isZero(a)) {
                setSign(c, ZERO);
            } else {
                setSign(c, PLUS);
            }
            return;
    }
    while (1) {
        divide(&A, &B, NULL, &tmp);
        if (isZero(&tmp)) {
            copyNumber(c, &B);
            return;
        }
        copyNumber(&A, &B);
        copyNumber(&B, &tmp);
    }
}

/// @brief 多倍長整数の最小公倍数を求める
/// @param a 最小公倍数を求める構造体
/// @param b 最小公倍数を求める構造体b
/// @param c 最小公倍数を代入する構造体
/// @return エラー: -1, 正常終了: 0
int lcm(const Number *a, const Number *b, Number *c) {
    if (isZero(a) || isZero(b)) {
        clearByZero(c);
        return 0;
    }
    Number tmp;
    switch (numComp(a, b)) {
        case 1:
            gcd(a, b, &tmp);
            divide(b, &tmp, c, NULL);
            multiple(a, c, c);
            break;
        case -1:
            gcd(a, b, &tmp);
            divide(a, &tmp, c, NULL);
            multiple(b, c, c);
            break;
        case 0:
            copyNumber(c, a);
            return 0;
    }
    return 0;
}