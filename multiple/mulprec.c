#include "mulprec.h"

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "mt19937ar.c"

/// @brief 構造体の中身を0で初期化する
/// @param a 初期化する構造体
void clearByZero(struct NUMBER *a) {
    int i;
    for (i = 0; i < KETA; i++) {
        a->n[i] = 0;
    }
    setSign(a, ZERO);
}

/// @brief 数値を表示する
/// @param a 表示する構造体
void dispNumber(const struct NUMBER *a) {
    int i;
    if (getSign(a) == MINUS) {
        printf("-");
    } else {
        printf("+");
    }
    for (i = KETA - 1; i >= 0; i--) {
        printf("%2d", a->n[i]);
    }
}

/// @brief 先頭の0を抜いて表示する
/// @param a 表示する構造体
void dispNumberZeroSuppress(const struct NUMBER *a) {
    int i;
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
        printf("%2d", a->n[i]);
    }
}

/// @brief 乱数を生成して代入する
/// @param a 乱数を代入する構造体のポインタ
/// @param k 乱数の桁数
void setRnd(struct NUMBER *a, int k) {
    clearByZero(a);
    if (k > KETA) {
        printf("ERROR:random number overflow\n");
        return;
    }
    int i;
    for (i = 0; i < k; i++) {
        a->n[i] = genrand_int32() % 10;
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
void copyNumber(struct NUMBER *a, const struct NUMBER *b) {
    clearByZero(a);
    for (int i = 0; i < KETA; i++) {
        a->n[i] = b->n[i];
    }
    setSign(a, getSign(b));
}

/// @brief 絶対値を求める
/// @param a 絶対値を求める構造体
/// @param b 絶対値を代入する構造体
void getAbs(const struct NUMBER *a, struct NUMBER *b) {
    struct NUMBER tmp;
    copyNumber(&tmp, a);
    if (getSign(a) == ZERO) {
        setSign(&tmp, ZERO);
    } else {
        setSign(&tmp, PLUS);
    }
    copyNumber(b, &tmp);
}

/// @brief aが0かどうかを判定する
/// @param a 判定する構造体
/// @return 0: 0でない, -1: 0
int isZero(const struct NUMBER *a) {
    if (getSign(a) == ZERO) {
        return 0;
    } else {
        return -1;
    }
}

/// @brief aを10倍してbに代入する
/// @param a 10倍する構造体
/// @param b 10倍した値を代入する構造体
/// @return 0: 正常終了, -1: オーバーフロー
int mulBy10(const struct NUMBER *a, struct NUMBER *b) {
    int i;
    struct NUMBER tmp;
    copyNumber(&tmp, a);
    clearByZero(b);
    if (tmp.n[KETA - 1] != 0) {
        return -1;
    }
    for (i = 0; i < KETA - 1; i++) {
        b->n[i + 1] = tmp.n[i];
    }
    b->n[0] = 0;
    setSign(b, getSign(&tmp));
    return 0;
}

/// @brief aを10で割ってbに代入する
/// @param a 10で割る構造体
/// @param b 10で割った値を代入する構造体
/// @return 剰余
int divBy10(const struct NUMBER *a, struct NUMBER *b) {
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
int setInt(struct NUMBER *a, int x) {
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
        r = x % 10;
        a->n[i] = r;
        x -= r;
        x /= 10;
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
int getInt(const struct NUMBER *a, int *x) {
    if (getSign(a) == ZERO) {
        *x = 0;
        return 0;
    }
    int i;
    int MAX = INT_MAX;           // int型の最大値
    int MIN = INT_MIN;           // int型の最小値
    int MAXlen = 0, MINlen = 0;  // int型の最大値と最小値の桁数
    int MAXdigit[KETA], MINdigit[KETA];  // int型の最大値と最小値の各桁の値
    // int型の最大値と最小値の桁数とそれぞれの桁の値を求める
    for (i = 0; i < KETA; i++) {
        MAXdigit[i] = 0;
        MINdigit[i] = 0;
    }
    i = 0;
    while (MAX != 0) {
        MAXdigit[i] = MAX % 10;
        MAX -= MAXdigit[i];
        MAX /= 10;
        MAXlen++;
        i++;
    }
    i = 0;
    while (MIN != 0) {
        MINdigit[i] = MIN % 10;
        if (MINdigit[i] <
            0) {  // なぜかMINdigit[i]が負の値になる場合とならない場合があるので強制で正にする
            MINdigit[i] *= -1;
        }
        MIN -= MINdigit[i];
        MIN /= 10;
        MINlen++;
        i++;
    }
    // 多倍長の桁数がint型の最大値と最小値の桁数より大きい場合はエラー
    for (i = 0; i < KETA - MAXlen; i++) {
        if (getSign(a) == MINUS) {
            if (a->n[MAXlen + i] != 0) {
                return -1;
            }
        } else {
            if (a->n[MINlen + i] != 0) {
                return -1;
            }
        }
    }
    // 多倍長とint型の各桁を比較
    if (getSign(a) == PLUS) {
        for (i = 0; i < MAXlen; i++) {
            if (a->n[MAXlen - 1 - i] > MAXdigit[MAXlen - 1 - i]) {
                return -1;
            } else if (a->n[MAXlen - 1 - i] < MAXdigit[MAXlen - 1 - i]) {
                break;
            }
        }
    } else {
        for (i = 0; i < MINlen; i++) {
            if (a->n[MINlen - 1 - i] > MINdigit[MINlen - 1 - i]) {
                return -1;
            } else if (a->n[MINlen - 1 - i] < MINdigit[MINlen - 1 - i]) {
                break;
            }
        }
    }
    *x = 0;
    for (i = KETA - 1; i >= 0; i--) {
        (*x) *= 10;
        (*x) += a->n[i];
    }
    if (getSign(a) == MINUS) {
        (*x) *= -1;
    }
    return 0;
}

/// @brief 符号を設定する
/// @param a 符号を設定する構造体
/// @param s 1: 正, 0: 0, -1: 負
/// @return 成功: 0, エラー: -1
int setSign(struct NUMBER *a, int s) {
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
int getSign(const struct NUMBER *a) { return a->sign; }

/// @brief 2つの多倍長整数を比較する
/// @param a 比較する構造体
/// @param b 比較する構造体
/// @return 1: a > b, 0: a = b, -1: a < b
int numComp(const struct NUMBER *a, const struct NUMBER *b) {
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
void swap(struct NUMBER *a, struct NUMBER *b) {
    struct NUMBER tmp;
    copyNumber(&tmp, a);
    copyNumber(a, b);
    copyNumber(b, &tmp);
}

/// @brief 2つの多倍長整数を加算する(同じ変数を関数内に入れてはいけない)
/// @param a 加算する構造体
/// @param b 加算する構造体
/// @param c 加算した値を代入する構造体
/// @return オーバーフロー: -1, 正常終了: 0
int add(const struct NUMBER *a, const struct NUMBER *b, struct NUMBER *c) {
    struct NUMBER A, B;
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
                if (d >= 10) {
                    d -= 10;
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
int sub(const struct NUMBER *a, const struct NUMBER *b, struct NUMBER *c) {
    struct NUMBER A, B;
    copyNumber(&A, a);
    copyNumber(&B, b);
    clearByZero(c);
    int i, e, num, rtn;
    int caseNum = getSign(&A) * 3 + getSign(&B);
    struct NUMBER numA, numB;
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
                        c->n[i] = num + 10 - numB.n[i];
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
                        c->n[i] = num + 10 - numA.n[i];
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
int increment(struct NUMBER *a, struct NUMBER *b) {
    struct NUMBER one;
    int r;
    setInt(&one, 1);
    r = add(a, &one, b);
    return r;
}

/// @brief 多倍長整数をデクリメントする
/// @param a デクリメントする構造体
/// @param b デクリメントした値を代入する構造体
/// @return オーバーフロー: -1, 正常終了: 0
int decrement(struct NUMBER *a, struct NUMBER *b) {
    struct NUMBER one;
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
int multiple(const struct NUMBER *a, const struct NUMBER *b, struct NUMBER *c) {
    int rtn = 0;
    if (getSign(a) == ZERO || getSign(b) == ZERO) {
        clearByZero(c);
    } else {
        int numA, numB, signA, signB;
        struct NUMBER tmp, A, B, D;
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
                    d = e % 10;
                    h = e / 10;
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
        rtn  = -1;
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
int divide(const struct NUMBER *a, const struct NUMBER *b, struct NUMBER *c,
           struct NUMBER *d) {
    int rtn = -1000; // 何もしていない状態
    if (isZero(b) == 0) {
        rtn = -1;
    } else {
        clearByZero(c);
        clearByZero(d);
        if (isZero(a) == 0) {
            rtn = 0;
        } else {
            int cSign, dSign;
            struct NUMBER A, B, numB, q;
            getAbs(a, &A);
            getAbs(b, &B);
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
                        copyNumber(d, &A);
                        c->sign = cSign;
                        d->sign = dSign;
                        rtn = 0;
                        break;
                    default:
                        setInt(&q, 1);
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
    }
    return rtn;
    // devideのreturnを全部最後にもってこよう
}