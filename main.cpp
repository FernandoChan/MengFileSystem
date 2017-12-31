#include "MFS.h"
using namespace std;


//ȫ�ֱ�������
const int Superblock_StartAddr = 0;																	//������ ƫ�Ƶ�ַ,ռһ�����̿�
const int InodeBitmap_StartAddr = 1 * BLOCK_SIZE;													//inodeλͼ ƫ�Ƶ�ַ��ռ�������̿飬����� 1024 ��inode��״̬
const int BlockBitmap_StartAddr = InodeBitmap_StartAddr + 2 * BLOCK_SIZE;							//blockλͼ ƫ�Ƶ�ַ��ռ��ʮ�����̿飬����� 10240 �����̿飨5120KB����״̬
const int Inode_StartAddr = BlockBitmap_StartAddr + 20 * BLOCK_SIZE;								//inode�ڵ��� ƫ�Ƶ�ַ��ռ INODE_NUM/(BLOCK_SIZE/INODE_SIZE) �����̿�
const int Block_StartAddr = Inode_StartAddr + INODE_NUM / (BLOCK_SIZE / INODE_SIZE) * BLOCK_SIZE;	//block������ ƫ�Ƶ�ַ ��ռ INODE_NUM �����̿�
const int Sum_Size = Block_StartAddr + BLOCK_NUM * BLOCK_SIZE;										//��������ļ���С
const int File_Max_Size = 10 * BLOCK_SIZE + BLOCK_SIZE / sizeof(int) * BLOCK_SIZE +					//�����ļ�����С��10��ֱ�ӿ飬һ����ӿ飬������ӿ�
		(BLOCK_SIZE / sizeof(int))*(BLOCK_SIZE / sizeof(int)) * BLOCK_SIZE;	

FILE* fw;									//��������ļ� д�ļ�ָ��
FILE* fr;									//��������ļ� ���ļ�ָ��
SuperBlock *superblock = new SuperBlock;	//������ָ��
bool inode_bitmap[INODE_NUM];				//inodeλͼ
bool block_bitmap[BLOCK_NUM];				//���̿�λͼ
char buffer[MFS_BUFFER] = { 0 };			//1M������������������ļ�

bool isLogin;								//�Ƿ����û���½
int Root_Dir_Addr;							//��Ŀ¼inode��ַ
int Cur_Dir_Addr;							//��ǰĿ¼
char Cur_Dir_Name[310];						//��ǰĿ¼��
char Cur_Host_Name[110];					//��ǰ������
char Cur_User_Name[110];					//��ǰ��½�û���
char Cur_Group_Name[110];					//��ǰ��½�û�����
char Cur_User_Dir_Name[310];				//��ǰ��½�û�Ŀ¼��
int nextUID;								//��һ��Ҫ������û���ʶ��
int nextGID;								//��һ��Ҫ������û����ʶ��

int main()
{
	char inputLine[100] = { 0 };

	// ��ʼ���ļ�ϵͳ
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
			if ((p = strstr(Cur_Dir_Name, Cur_User_Dir_Name)) == NULL)	//û�ҵ�
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
