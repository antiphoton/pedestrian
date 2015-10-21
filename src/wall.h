#include"mymath.h"
struct WallSegment {
	WallSegment(const Vector2 &r1,const Vector2 &r2);
	Vector2 r1,r2;//left side is inner side
	Vector2 d12;
	Vector2 n;
};
extern std::vector<WallSegment> *walls;
void writeWallJson(FILE *file);
Vector2 getWallForce(const Vector2 &position);
struct WallInitializer {
	WallInitializer();
};
