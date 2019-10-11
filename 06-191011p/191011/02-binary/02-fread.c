#include <stdio.h>
#include <assert.h>

typedef struct point {
    int x;
    int y;
} point;

int main() {
    point pts[2];
    FILE *f = fopen("01-fwrite-demo.bin", "rb");
    assert(f);
    assert(fread(pts, sizeof pts[0], 2, f) == 2);
    fclose(f);
    printf("%d %d\n", pts[0].x, pts[0].y);
    printf("%d %d\n", pts[1].x, pts[1].y);
}

