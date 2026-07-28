// Minesweeper 样例程序
//作者：AlbertFrank-kmHuang
// 游戏信息：http://www.botzone.org/games#Minesweeper
 
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include "jsoncpp/json.h"
 
#define MAX_WIDTH 80
#define MAX_HEIGHT 40
 
using namespace std;
 
int fieldHeight, fieldWidth, mineCount;
int mineField[MAX_HEIGHT][MAX_WIDTH]; // 下标：【行(row)】【列(col)】
// 0-8：数字
// 9：雷
// 10 / A：未点开

//My Part/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int dc[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
int dr[8] = {0, -1, -1, -1, 0, 1, 1, 1};

bool inMap(int row, int col, int fieldHeight, int fieldWidth) {
    if (row<0 || row >= fieldHeight) return false;
    if (col<0 || col >= fieldWidth) return false;
    return true;
}
 

//End/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    int row, col;
 
 	// 初始化随机数产生器
 	srand(time(0));
 
 	// 读取平台输入的信息
 	string str;
 	getline(cin, str);
 	Json::Reader reader;
 	Json::Value input, output, lastInput;
 	reader.parse(str, input);
 
 	int len = input["requests"].size(); // 读取程序曾经接到的输入总数
 	lastInput = input["requests"][len - 1]; // 取出最后一次获得的输入
 	fieldHeight = lastInput["height"].asInt(); // 读取雷区高度（始终不变）
 	fieldWidth = lastInput["width"].asInt(); // 读取雷区高度（始终不变）
 	mineCount = lastInput["minecount"].asInt(); // 读取雷数（始终不变）
 	for (row = 0; row < fieldHeight; row++)
 	{
 		for (col = 0; col < fieldWidth; col++)
 		{
 			mineField[row][col] = 10;
 		}
 	}
 	
 	// 根据以往输入读取雷区状态
 	for (int i = 0; i < len; i++)
 	{
 		Json::Value changed = input["requests"][i]["changed"];
 		if (changed.isArray())
 		{
 			int changedLen = changed.size();
 			for (int j = 0; j < changedLen; j++)
 			{
 				mineField[changed[j]["row"].asInt()][changed[j]["col"].asInt()] = changed[j]["val"].asInt();
 			}
 		}
 	}
 	
 
// 开始决策！
//My Part/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



 	int decidedRow = -1, decidedCol = -1; // 决定要点开的位置

    //第一步：点中间
    if (len == 1) {
        int firstR = fieldHeight / 2;
        int firstC = fieldWidth / 2;
        decidedRow = firstR;
        decidedCol = firstC;
    }


    //level 1 - 初级一眼看
    if (decidedRow == -1) {
        //标记显性雷
        for (int row=0; row<fieldHeight; row++) {
            for (int col=0; col<fieldWidth; col++) {
                if (mineField[row][col]>=1 && mineField[row][col]<=8) {
                    int unkonwNumber = 0;
                    int bombNumber = 0;
                    for (int dir=0; dir<8; dir++) {
                        int nr = row + dr[dir];
                        int nc = col + dc[dir];
                        if (inMap(nr, nc, fieldHeight, fieldWidth)) {
                            if (mineField[nr][nc] == 10) {
                                unkonwNumber++;
                            } else if (mineField[nr][nc] == 9) {
                                bombNumber++;
                            }
                        }
                    }

                    if (unkonwNumber + bombNumber == mineField[row][col]) {
                        for (int dir=0; dir<8; dir++) {
                            int nr = row + dr[dir];
                            int nc = col + dc[dir];
                            if (inMap(nr, nc, fieldHeight, fieldWidth)) {
                                if (mineField[nr][nc] == 10) {
                                    mineField[nr][nc] = 9;
                                }
                            }
                        }
                    }
                }
            }
        }

        //点开显性雷
        for (int row = 0; row < fieldHeight; row++) {
            for (int col=0; col < fieldWidth; col++) {
                if (mineField[row][col]>=0 && mineField[row][col] <= 8) {
                    int bombNumber = 0;
                    for (int dir=0; dir<8; dir++) {
                        int nr = row + dr[dir];
                        int nc = col + dc[dir];
                        if (inMap(nr, nc, fieldHeight, fieldWidth)) {
                            if (mineField[nr][nc] == 9) {
                                bombNumber++;
                            }
                        }
                    }

                    if (bombNumber == mineField[row][col]) {
                        for (int dir=0; dir<8; dir++) {
                            int nr = row + dr[dir];
                            int nc = col + dc[dir];
                            if (inMap(nr, nc, fieldHeight, fieldWidth)) {
                                if (mineField[nr][nc] == 10) {
                                    decidedRow = nr;
                                    decidedCol = nc;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (decidedRow != -1) break;
            }
            if (decidedRow != -1) break;
        }
    }

    //level 2 - 简单枚举
    if (decidedRow == -1) {
        //找出所有与数字毗邻的未知格
        int unrevealedNearNumberPos[fieldHeight * fieldWidth][2], unrevealedNearNumberCount = 0;
        for (row = 0; row < fieldHeight; row++){
            for (col = 0; col < fieldWidth; col++){
                if (mineField[row][col] == 10){
                    for (int dir = 0; dir < 8; ++dir) {
                        int nr = row + dr[dir];
                        int nc = col + dc[dir];
                        if (mineField[nr][nc]>=1 && mineField[nr][nc]<=8){
                            unrevealedNearNumberPos[unrevealedNearNumberCount][0] = row;
                            unrevealedNearNumberPos[unrevealedNearNumberCount][1] = col;
                            unrevealedNearNumberCount++;
                            break;
                        }                       
                    }
                }
            }
        }

        //简单枚举
        int nr = unrevealedNearNumberPos[unrevealedNearNumberCount][0];
        int nc = unrevealedNearNumberPos[unrevealedNearNumberCount][1];
        mineField[nr][nc] = 9;
        ...

        mineField[nr][nc] = 10;

        // 随机选，但以与数字毗邻的格子优先
        if (unrevealedNearNumberCount != 0) {
            int myChoice = rand() % unrevealedNearNumberCount;
            decidedRow = unrevealedNearNumberPos[myChoice][0];
            decidedCol = unrevealedNearNumberPos[myChoice][1];
        } else {
            unrevealedCount = max(0, unrevealedCount);
            int myChoice = rand() % unrevealedCount;
            decidedRow = unrevealedPos[myChoice][0];
            decidedCol = unrevealedPos[myChoice][1];
        }   
    }


    //level 3 - 随机选
    if (decidedRow == -1) {
        // 先找出所有没点开的位置
        int unrevealedPos[fieldHeight * fieldWidth][2], unrevealedCount = 0;
        int unrevealedNearNumberPos[fieldHeight * fieldWidth][2], unrevealedNearNumberCount = 0;
        for (row = 0; row < fieldHeight; row++)
        {
            for (col = 0; col < fieldWidth; col++)
            {
                if (mineField[row][col] == 10)
                {
                    unrevealedPos[unrevealedCount][0] = row;
                    unrevealedPos[unrevealedCount][1] = col;
                    unrevealedCount++;

                    for (int dir = 0; dir < 8; ++dir) {
                        int nr = row + dr[dir];
                        int nc = col + dc[dir];
                        if (mineField[nr][nc]>=1 && mineField[nr][nc]<=8){
                            unrevealedNearNumberPos[unrevealedNearNumberCount][0] = row;
                            unrevealedNearNumberPos[unrevealedNearNumberCount][1] = col;
                            unrevealedNearNumberCount++;
                            break;
                        }                       
                    }
                }
            }
        }

        // 随机选，但以与数字毗邻的格子优先
        if (unrevealedNearNumberCount != 0) {
            int myChoice = rand() % unrevealedNearNumberCount;
            decidedRow = unrevealedNearNumberPos[myChoice][0];
            decidedCol = unrevealedNearNumberPos[myChoice][1];
        } else {
            unrevealedCount = max(0, unrevealedCount);
            int myChoice = rand() % unrevealedCount;
            decidedRow = unrevealedPos[myChoice][0];
            decidedCol = unrevealedPos[myChoice][1];
        }   
    }


    //防止违法
    if (decidedRow == -1 || decidedCol == -1) {
        decidedRow = 0;
        decidedCol = 0;
    }
 
 	// 将本回合决策交予平台并结束
 	output["response"]["row"] = decidedRow;
 	output["response"]["col"] = decidedCol;
 	Json::FastWriter writer;
 	cout << writer.write(output) << endl;


    //End/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}