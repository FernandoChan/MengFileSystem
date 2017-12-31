#include "MFS.h"
using namespace std;


//全局变量定义
const int Superblock_StartAddr = 0;																	//超级块 偏移地址,占一个磁盘块
const int InodeBitmap_StartAddr = 1 * BLOCK_SIZE;													//inode位图 偏移地址，占两个磁盘块，最多监控 1024 个inode的状态
const int BlockBitmap_StartAddr = InodeBitmap_StartAddr + 2 * BLOCK_SIZE;							//block位图 偏移地址，占二十个磁盘块，最多监控 10240 个磁盘块（5120KB）的状态
const int Inode_StartAddr = BlockBitmap_StartAddr + 20 * BLOCK_SIZE;								//inode节点区 偏移地址，占 INODE_NUM/(BLOCK_SIZE/INODE_SIZE) 个磁盘块
const int Block_StartAddr = Inode_StartAddr + INODE_NUM / (BLOCK_SIZE / INODE_SIZE) * BLOCK_SIZE;	//block数据区 偏移地址 ，占 INODE_NUM 个磁盘块
const int Sum_Size = Block_StartAddr + BLOCK_NUM * BLOCK_SIZE;										//虚拟磁盘文件大小
const int File_Max_Size = 10 * BLOCK_SIZE + BLOCK_SIZE / sizeof(int) * BLOCK_SIZE +					//单个文件最大大小，10个直接块，一级间接块，二级间接块
		(BLOCK_SIZE / sizeof(int))*(BLOCK_SIZE / sizeof(int)) * BLOCK_SIZE;	

FILE* fw;									//虚拟磁盘文件 写文件指针
FILE* fr;									//虚拟磁盘文件 读文件指针
SuperBlock *superblock = new SuperBlock;	//超级块指针
bool inode_bitmap[INODE_NUM];				//inode位图
bool block_bitmap[BLOCK_NUM];				//磁盘块位图
char buffer[MFS_BUFFER] = { 0 };			//1M，缓存整个虚拟磁盘文件

bool isLogin;								//是否有用户登陆
int Root_Dir_Addr;							//根目录inode地址
int Cur_Dir_Addr;							//当前目录
char Cur_Dir_Name[310];						//当前目录名
char Cur_Host_Name[110];					//当前主机名
char Cur_User_Name[110];					//当前登陆用户名
char Cur_Group_Name[110];					//当前登陆用户组名
char Cur_User_Dir_Name[310];				//当前登陆用户目录名
int nextUID;								//下一个要分配的用户标识号
int nextGID;								//下一个要分配的用户组标识号

int main()
{
	char inputLine[100] = { 0 };

	// 初始化文件系统
	InitSystem();

	if (isLogin == false) {
		cout << "-----------------------------------------" << endl;
		cout << "Wellcome to " << MFS_NAME << endl;
		cout << "If you need help, please type 'help'." << endl;
		cout << "-----------------------------------------" << endl;
	}

	while (1) {

		if (isLogin == false) {
			cout << "$ ";
		}
		else {
			char *p;
			if ((p = strstr(Cur_Dir_Name, Cur_User_Dir_Name)) == NULL)	//没找到
				printf("[%s@%s %s]$ ", Cur_User_Name, Cur_Host_Name, Cur_Dir_Name);
			else
				printf("[%s@%s ~%s]$ ", Cur_User_Name, Cur_Host_Name, Cur_Dir_Name + strlen(Cur_User_Dir_Name));
		}

		gets(inputLine);
		Cmd(inputLine);
	}

	delete(superblock);
	superblock = NULL;
	fclose(fw);	
	fclose(fr);

	return 0;
}
