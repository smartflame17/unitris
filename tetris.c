#include "tetris.h"

static struct sigaction act, oact;
int B,count;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	
	createRankList();
	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break; 
		case MENU_RANK: rank(); break;
		case MENU_REC_PLAY: break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}
	writeRankFile();
	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	recRoot = (RecNode*)malloc(sizeof(RecNode));
	recRoot->accscore = 0;
	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++){
			field[j][i]=0;
			recRoot->recField[j][i] = 0;
		}
	recRoot->level = 1;
	recRoot->parent = NULL;
	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;
	

	DrawOutline();
	DrawField();
	recommend(recRoot, 1);
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(9, WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}

void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
	for( i = 0; i < 4; i++ ){
		move(10+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();
			newRank(score);
			return;
		}
		timed_out = 0;
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	free(recRoot);
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j;

	for (i = 0; i < BLOCK_HEIGHT; i++){
		for (j = 0; j < BLOCK_WIDTH; j++){
			if (i + blockY < HEIGHT && j + blockX < WIDTH){ 								//field 범위 벗어나는지 체크
				if (i + blockY >= 0 && j + blockX >= 0){
					if (block[currentBlock][blockRotate][i][j]){							//block이 유효한 칸이면
						if (f[i + blockY][j + blockX]) return 0;							//field에 놓인 것과 겹치면 return 0
					}
				}
				else if (block[currentBlock][blockRotate][i][j]) return 0;
			}
			else if (block[currentBlock][blockRotate][i][j]) return 0; //field 범위 벗어난 블록 칸 있으면 return 0
		}
	}
	return 1;			//범위 안 벗어나고 겹치는 곳 없으면 return 1
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다. 
	int i,j, yTemp;

	switch(command){
		case KEY_UP:
			blockRotate = (blockRotate+3)%4;		
			for(i=0;i<4;i++){
				for(j=0;j<4;j++){
					move(i+blockY+1,j+blockX+1);
					if (block[currentBlock][blockRotate][i][j])
						printw(".");
				}
			}
			yTemp = blockY;
			while (CheckToMove(field, currentBlock, blockRotate, yTemp+1, blockX)){
				yTemp++;
			}
			for(i=0;i<4;i++){
				for(j=0;j<4;j++){
					move(i+yTemp+1,j+blockX+1);
					if (block[currentBlock][blockRotate][i][j])
						printw(".");
				}
			}
			move(HEIGHT,WIDTH+10);
			blockRotate = (blockRotate+1)%4;
			DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
			break;
		case KEY_DOWN:
			blockY--;		
			for(i=0;i<4;i++){
				for(j=0;j<4;j++){
					move(i+blockY+1,j+blockX+1);
					if (block[currentBlock][blockRotate][i][j])
						printw(".");
				}
			}
			yTemp = blockY;
			while (CheckToMove(field, currentBlock, blockRotate, yTemp+1, blockX)){
				yTemp++;
			}
			for(i=0;i<4;i++){
				for(j=0;j<4;j++){
					move(i+yTemp+1,j+blockX+1);
					if (block[currentBlock][blockRotate][i][j])
						printw(".");
				}
			}
			move(HEIGHT,WIDTH+10);
			blockY++;
			DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
			break;
		case KEY_RIGHT:
			blockX--;		
			for(i=0;i<4;i++){
				for(j=0;j<4;j++){
					move(i+blockY+1,j+blockX+1);
					if (block[currentBlock][blockRotate][i][j])
						printw(".");
				}
			}
			yTemp = blockY;
			while (CheckToMove(field, currentBlock, blockRotate, yTemp+1, blockX)){
				yTemp++;
			}
			for(i=0;i<4;i++){
				for(j=0;j<4;j++){
					move(i+yTemp+1,j+blockX+1);
					if (block[currentBlock][blockRotate][i][j])
						printw(".");
				}
			}
			move(HEIGHT,WIDTH+10);
			blockX++;
			DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
			break;
		case KEY_LEFT:
			blockX++;		
			for(i=0;i<4;i++){
				for(j=0;j<4;j++){
					move(i+blockY+1,j+blockX+1);
					if (block[currentBlock][blockRotate][i][j])
						printw(".");
				}
			}
			yTemp = blockY;
			while (CheckToMove(field, currentBlock, blockRotate, yTemp+1, blockX)){
				yTemp++;
			}
			for(i=0;i<4;i++){
				for(j=0;j<4;j++){
					move(i+yTemp+1,j+blockX+1);
					if (block[currentBlock][blockRotate][i][j])
						printw(".");
				}
			}
			move(HEIGHT,WIDTH+10);
			blockX--;
			DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
			break;
	}
	//for checking y,x coordinates for debugging
	move(20, WIDTH+11);
	printw("Y : %3d", blockY);
	move(21, WIDTH+11);
	printw("X : %3d", blockX);
}

void BlockDown(int sig){
	int y, x;
	if (CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX)){
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}
	else{
		if (blockY == -1) gameOver = 1;
		score = AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		score = DeleteLine(field);
		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = rand()%7;
		for (y = 0; y < HEIGHT; y++)
				for (x = 0; x < WIDTH; x++){
					if (field[y][x] == 1) recRoot->recField[y][x] = 1;
					else recRoot->recField[y][x] = 0;
				}
		recommend(recRoot, 1);
		blockRotate = 0;
		blockY = -1;
		blockX = WIDTH/2-2;
		DrawNextBlock(nextBlock);
		PrintScore(score);
		DrawField();
	}
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	//Block이 추가된 영역의 필드값을 바꾼다.
	int i,j;
	int touched = 0;											//touching bottom or other block on field adds up score

	for (i = 0; i < BLOCK_HEIGHT; i++){
		for (j = 0; j < BLOCK_WIDTH; j++){
			if (block[currentBlock][blockRotate][i][j]){
				field[blockY+i][blockX+j] = 1;
				if (blockY+i == HEIGHT-1) touched++;			//if touching bottom
				else if (field[blockY+i+1][blockX+j] == 1) touched++;	//if another block is under
			}
		}
	}
	score += touched * 10;
	return score;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
	int fullFlag = 0;											//1 if line is full
	int full_lines = 0;											//# of full lines for score calculation
	int i, j, y, x;

	for (i = 0; i < HEIGHT; i++){
		fullFlag = 1;
		for (j = 0; j < WIDTH; j++){
			if (!field[i][j]) {
				fullFlag = 0;
				break;
			}
		}
		if (fullFlag){
			full_lines++;
			for (y = i-1; y >= 0; y--){
				for (x = 0; x < WIDTH; x++){
					field[y+1][x] = field[y][x];
				}
			}
			for (x = 0; x < WIDTH; x++) field[0][x] = 0;
		}
	}
	score += full_lines * full_lines * 100;
	return score;
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	DrawBlock(recommendY, recommendX, blockID, recommendR, 'R');
	DrawShadow(y, x, blockID, blockRotate);
	DrawBlock(y, x, blockID, blockRotate, ' ');
	
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	while (CheckToMove(field, blockID, blockRotate, y+1, x)){
		y++;
	}
	DrawBlock(y, x, blockID, blockRotate, '/');
}

void createRankList(){
	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	// 1. "rank.txt"열기
	// 2. 파일에서 랭킹정보 읽어오기
	// 3. LinkedList로 저장
	// 4. 파일 닫기
	FILE *fp;
	int i;
	char str[NAMELEN+1];
	int score;
	rankNode* temp;
	//1. 파일 열기
	fp = fopen("rank.txt", "r");
	if (fp == NULL) return;
	// 2. 정보읽어오기
	/* int fscanf(FILE* stream, const char* format, ...);
	stream:데이터를 읽어올 스트림의 FILE 객체를 가리키는 파일포인터
	format: 형식지정자 등등
	변수의 주소: 포인터
	return: 성공할 경우, fscanf 함수는 읽어들인 데이터의 수를 리턴, 실패하면 EOF리턴 */
	// EOF(End Of File): 실제로 이 값은 -1을 나타냄, EOF가 나타날때까지 입력받아오는 if문
	if (fscanf(fp, "%d", &ranklength) != EOF) {
		if (ranklength == 0) return;
		fscanf(fp, "%s %d", str, &score);
		rankNode* newRank = (rankNode*)malloc(sizeof(rankNode));
		strcpy(newRank->name, str);
		newRank->score = score;
		newRank->next = NULL;
		head = newRank;
		temp = head;
		for (i = 1; i < ranklength; i++){
			fscanf(fp, "%s %d", str, &score);
			newRank = (rankNode*)malloc(sizeof(rankNode));
			strcpy(newRank->name, str);
			newRank->score = score;
			newRank->next = NULL;
			temp->next = newRank;
			temp = temp->next;
		}
	}
	// 4. 파일닫기
	fclose(fp);
}

void rank(){
	//목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
	//1. 문자열 초기화
	int X=1, Y=ranklength, ch, i;
	rankNode* temp = head;
	clear();

	//2. printw()로 3개의 메뉴출력
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	//3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
	ch = wgetch(stdscr);

	//4. 각 메뉴에 따라 입력받을 값을 변수에 저장
	
	switch (ch){
		//4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
		case '1': {
			echo();
			printw("X : ");
			scanw("%d", &X);
			printw("Y : ");
			scanw("%d", &Y);
			noecho();
			printw("\n        name    |   score  \n");
			printw("----------------------------\n");
			//if (X < 0) X = 1;					just in case if standards for X changes
			if (Y > ranklength) Y = ranklength;
			if (X > Y || X < 1 || X > ranklength || Y < 1) {
				printw("\nsearch failure : no rank in the list\n");
				break;
			}
			else{
				Y = Y - X + 1;
				for (i = 1; i < X; i++) temp = temp->next;
				for (i = 0; i < Y; i++){
					printw(" %-15s| %-9d\n", temp->name, temp->score);
					temp = temp->next;
				}
			}
			break;
		}
			
		//4-2. 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
		case '2': {
			char str[NAMELEN+1];
			int flag = 0;			//stays 0 if no matches found

			echo();
			printw("Input the name : ");
			scanw("%s", str);
			noecho();
			printw("\n        name    |   score  \n");
			printw("----------------------------\n");
			for (i = 0; i < ranklength; i++){
				if (!strcmp(str, temp->name)){
					printw(" %-15s| %-9d\n", temp->name, temp->score);
					flag = 1;
				}
				temp = temp->next;
			}
			if (!flag) printw("\nsearch failure : no name in the list\n");
			break;
		}
			
		//4-3. 메뉴3: rank번호를 입력받아 리스트에서 삭제
		case '3': {
			int num;			//stores rank for deletion
			rankNode* prev = NULL;

			echo();
			printw("Input the rank : ");
			scanw("%d", &num);
			noecho();
			if (num < 1 || num > ranklength) {
				printw("\nsearch failure : rank not in the list\n");
				break;
			}
			else if (num == 1){
				head = temp->next;
				free(temp);
			}
			else{
				for (i = 1; i < num; i++){
					prev = temp;
					temp = temp->next;
				}
				prev->next = temp->next;
				free(temp);
			}
			printw("\nresult : rank successfully deleted\n");
			ranklength--;
			break;
		}
		default: break;
	}
	getch();

}

void writeRankFile(){
	// 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
	int i;
	rankNode* curr = head;
	rankNode* prev = NULL;
	//1. "rank.txt" 연다
	if (ranklength != 0){
		FILE *fp = fopen("rank.txt", "w");

		//2. 랭킹 정보들의 수를 "rank.txt"에 기록
		fprintf(fp, "%d\n",ranklength);
		//3. 탐색할 노드가 더 있는지 체크하고 있으면 다음 노드로 이동, 없으면 종료
		for ( i= 0; i < ranklength; i++) {
			fprintf(fp, "%s %d\n", curr->name, curr->score);
			prev = curr;
			curr = curr->next;
			free(prev);
		}
		fclose(fp);
	}
}

void newRank(int score){
	// 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	rankNode* newRank = (rankNode*)malloc(sizeof(rankNode));
	rankNode* curr = head;
	char str[NAMELEN+1];
	int i, j;
	clear();
	//1. 사용자 이름을 입력받음
	echo();
	printw("Your Name : ");
	getnstr(str, NAMELEN+1);
	noecho();
	//2. 새로운 노드를 생성해 이름과 점수를 저장
	strcpy(newRank->name, str);
	newRank->score = score;
	newRank->next = NULL;
	if (!head) head = newRank;
	else {
		if (head->score < newRank->score){
			newRank->next = head;
			head = newRank;
		}
		else{
			do {
				curr = curr->next;
				if (!curr->next) break;
			}while (curr->next->score > newRank->score);
			newRank->next = curr->next;
			curr->next = newRank;
		}
	}
	ranklength++;
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root, int level){
	int max = 0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int rot, j; //double for loop counter, rot stands for rotation
	int blockpos; // 현재 블록이 있을 수 있는 x축 위치의 개수, 루프에 사용
	int curblockID; // 해당 호출에서 고려할 블록 ID
	int cn = 0;			//index for child node
	int x, y;

	if (level-1 > VISIBLE_BLOCKS) return 0;
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
				for (x = 0; x < WIDTH; x++){
					if (root->recField[y][x] == 1) root->child[cn]->recField[y][x] = 1;
					else root->child[cn]->recField[y][x] = 0;
				}															//Copy root field to child's field
			root->child[cn]->curBlockID = curblockID;
			root->child[cn]->recBlockRotate = rot;
			root->child[cn]->recBlockX = XStartInfo[curblockID][rot]+j;
			root->child[cn]->recBlockY = 0;
			while (CheckToMove(root->recField,curblockID,rot,root->child[cn]->recBlockY+1,root->child[cn]->recBlockX)){
				root->child[cn]->recBlockY++;
			}
			root->child[cn]->accscore += AddBlockToField(root->child[cn]->recField, curblockID, rot, root->child[cn]->recBlockY, root->child[cn]->recBlockX);
			root->child[cn]->accscore += DeleteLine(root->child[cn]->recField);
			root->child[cn]->accscore += recommend(root->child[cn], level+1);
			cn++;
		}
	}
	for (j = 0; j < blockpos; j++){
		if (root->child[j]->accscore > max){
			recommendX = root->child[j]->recBlockX;
			recommendY = root->child[j]->recBlockY;
			recommendR = root->child[j]->recBlockRotate;
			max = root->child[j]->accscore;
		}
		free(root->child[j]);
	}
	return max;
}

void recommendedPlay(){
	// user code
}
