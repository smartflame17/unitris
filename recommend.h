#include "tetris.h"

int modified_recommend(RecNode *root, int level){
	int max = 0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int rot, i, j; //double for loop counter, rot stands for rotation
	int blockpos; // 현재 블록이 있을 수 있는 x축 위치의 개수, 루프에 사용
	int curblockID; // 해당 호출에서 고려할 블록 ID
	int cn = 0;			//index for child node
	int x, y;
    RecNode* NodeArray[TREE_PRUNING];   //pruning에서 살릴 노드 포인터 저장 배열
    RecNode* temp;

	if (level-1 > VISIBLE_BLOCKS) return max;
	curblockID = nextBlock[level-1];
	if (curblockID == 4) blockpos = 36;
	else blockpos = 34;

	root->child = (RecNode**)malloc(sizeof(RecNode*)*blockpos);
	for (rot = 0; rot < 4; rot++){
		for (j = 0; j < XLengthInfo[curblockID][rot]; j++){
			root->child[cn] = (RecNode*)malloc(sizeof(RecNode));
			root->child[cn]->accscore = 0;
			root->child[cn]->parent = root;
			root->child[cn]->level = level + 1;
			for (y = 0; y < HEIGHT; y++)
				for (x = 0; x < WIDTH; x++)
					root->child[cn]->recField[y][x] = root->recField[y][x];	//Copy root field to child's field
			root->child[cn]->curBlockID = curblockID;
			root->child[cn]->recBlockRotate = rot;
			root->child[cn]->recBlockX = XStartInfo[curblockID][rot]+j;
			root->child[cn]->recBlockY = 0;
			while (CheckToMove(root->child[cn]->recField,curblockID,rot,root->child[cn]->recBlockY+1,root->child[cn]->recBlockX)){
				root->child[cn]->recBlockY++;
			}
			root->child[cn]->accscore += root->child[cn]->recBlockY * YCoordMult;
			root->child[cn]->accscore += AddBlockToField(root->child[cn]->recField, curblockID, rot, root->child[cn]->recBlockY, root->child[cn]->recBlockX);
			root->child[cn]->accscore += DeleteLine(root->child[cn]->recField);
			// Need penalty or advantage calculation for better recommendation
			// give penalty to accscore by counting holes under block
			// give advantage to higher y coord(stack blocks lower)
			cn++;
		}
	}
    for (j = 0; j < blockpos; j++){
		if (root->child[j]->accscore > max){
			temp = root->child[j];
		}
	}
    NodeArray[0] = temp;                    //NodeArray[0] contains pointer to largest scoring child
    max = NodeArray[0]->accscore;           //max is that largest score
    for (i = 1; i < TREE_PRUNING; i++){     //iterate for # of childs we want to leave
        temp = NULL;
        max = 0;
        for (j = 0; j < blockpos; j++){
            if (root->child[j]->accscore > max && root->child[j]->accscore < NodeArray[i-1]->accscore){
                max = root->child[j]->accscore;
                temp = root->child[j];
            }
            NodeArray[i] = temp;
        }
    }
    for (i = 0; i < TREE_PRUNING; i++)
        NodeArray[i]->accscore += modified_recommend(NodeArray[i], level+1);
	max = 0;
	for (j = 0; j < TREE_PRUNING; j++){
		if (NodeArray[j]->accscore > max){
			recommendX = NodeArray[j]->recBlockX;
			recommendY = NodeArray[j]->recBlockY;
			recommendR = NodeArray[j]->recBlockRotate;
			max = NodeArray[j]->accscore;
		}
	}
	for (j = 0; j < blockpos; j++)
		free(root->child[j]);
	return max;
}
