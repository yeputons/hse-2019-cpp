#include <stdio.h>
#include <assert.h>

typedef struct point {
    int x;
    int y;
} point;

int main() {
    point pts[2];
    pts[0].x = 10;
    pts[0].y = 20;
    pts[1].x = 30;
    pts[1].y = 40;
    FILE *f = fopen("01-fwrite-demo.bin", "wb");
    assert(f);
    assert(fwrite(pts, sizeof pts[0], 2, f) == 2);
    fclose(f);
}

