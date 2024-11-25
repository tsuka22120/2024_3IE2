#define KETA 20

struct NUMBER {
    int n[KETA];  // 各桁の変数
    int sign;     // 符号変数 -1: 負, 0: 0, 1: 正
};

void clearByZero(struct NUMBER *);
void dispNumber(const struct NUMBER *);
void dispNumberZeroSuppress(const struct NUMBER *);
void setRnd(struct NUMBER *, int);
void copyNumber(struct NUMBER *, const struct NUMBER *);
void getAbs(const struct NUMBER *, struct NUMBER *);
int isZero(const struct NUMBER *);
int mulBy10(const struct NUMBER *, struct NUMBER *);
int divBy10(const struct NUMBER *, struct NUMBER *);
int setInt(struct NUMBER *, int);
int getInt(const struct NUMBER *, int *);
int setSign(struct NUMBER *, int);
int getSign(const struct NUMBER *);
int numComp(const struct NUMBER *, const struct NUMBER *);
void swap(struct NUMBER *, struct NUMBER *);