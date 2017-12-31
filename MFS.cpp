#include "MFS.h"
using namespace std;

// ---------------- �ļ�ϵͳ���ڴ���䡢��ʼ�� ----------------------

void InitSystem()
{
	//����������ļ� 
	if ((fr = fopen(FILESYSNAME, "rb")) == NULL) {	//ֻ������������ļ����������ļ���
													//��������ļ������ڣ�����һ��
		fw = fopen(FILESYSNAME, "wb");	//ֻд����������ļ����������ļ���
		if (fw == NULL) {
			cout << "Virtual disk file open failure." << endl;
			return;	//���ļ�ʧ��
		}
		fr = fopen(FILESYSNAME, "rb");	//���ڿ��Դ���

		//��ʼ������
		nextUID = 0;
		nextGID = 0;
		isLogin = false;
		strcpy(Cur_User_Name, "root");
		strcpy(Cur_Group_Name, "root");

		//��ȡ������
		memset(Cur_Host_Name, 0, sizeof(Cur_Host_Name));
		DWORD k = 100;
		GetComputerName(Cur_Host_Name, &k);

		//��Ŀ¼inode��ַ ����ǰĿ¼��ַ������
		Root_Dir_Addr = Inode_StartAddr;	//��һ��inode��ַ
		Cur_Dir_Addr = Root_Dir_Addr;
		strcpy(Cur_Dir_Name, "/");

		cout << "-----------------------------------------" << endl;
		cout << "The file system is being formatted......" << endl;
		if (!Format()) {
			cout << "Formatting failure!" << endl;
			getchar();
			return;
		}
		cout << "Format complete." << endl;

		// ��ʼ���û�������Ŀ¼�������ļ�
		InitUser();

		if (!Install()) {
			cout << "Failed to install file system." << endl;
			return;
		}
	}
	else {	//��������ļ��Ѵ���
		fread(buffer, Sum_Size, 1, fr);

		//ȡ���ļ������ݴ浽�����У���д��ʽ���ļ�֮����д���ļ���д��ʽ�򿪻�����ļ���
		fw = fopen(FILESYSNAME, "wb");	//ֻд����������ļ����������ļ���
		if (fw == NULL) {
			cout << "Virtual disk file failed to open." << endl;
			return;	//���ļ�ʧ��
		}
		fwrite(buffer, Sum_Size, 1, fw);

		/* ��ʾ�Ƿ�Ҫ��ʽ��
		* ��Ϊ���ǵ�һ�ε�½������ȥ��һ��
		* ������Ҫ�ֶ����ñ���
		Ready();
		system("pause");
		system("cls");
		*/

		//��ʼ������
		nextUID = 0;
		nextGID = 0;
		isLogin = false;
		strcpy(Cur_User_Name, "root");
		strcpy(Cur_Group_Name, "root");

		//��ȡ������
		memset(Cur_Host_Name, 0, sizeof(Cur_Host_Name));
		DWORD k = 100;
		GetComputerName(Cur_Host_Name, &k);

		//��Ŀ¼inode��ַ ����ǰĿ¼��ַ������
		Root_Dir_Addr = Inode_StartAddr;	//��һ��inode��ַ
		Cur_Dir_Addr = Root_Dir_Addr;
		strcpy(Cur_Dir_Name, "/");

		if (!Install()) {
			cout << "Failed to install file system." << endl;
			return;
		}
	}
}

//����ʵ��
void Ready()	//��¼ϵͳǰ��׼������,������ʼ��+ע��+��װ
{
	//��ʼ������
	nextUID = 0;
	nextGID = 0;
	isLogin = false;
	strcpy(Cur_User_Name, "root");
	strcpy(Cur_Group_Name, "root");

	//��ȡ������
	memset(Cur_Host_Name, 0, sizeof(Cur_Host_Name));
	DWORD k = 100;
	GetComputerName(Cur_Host_Name, &k);

	//��Ŀ¼inode��ַ ����ǰĿ¼��ַ������
	Root_Dir_Addr = Inode_StartAddr;	//��һ��inode��ַ
	Cur_Dir_Addr = Root_Dir_Addr;
	strcpy(Cur_Dir_Name, "/");


	char c;
	cout << "Do you want to format? [y/n] ";
	while (c = getch()) {
		fflush(stdin);
		if (c == 'y') {
			printf("\n");
			cout << endl << "The file system is being formatted......" << endl;
			if (!Format()) {
				cout << "Format failur." << endl;
				return;
			}
			cout << "Format complete." << endl;
			break;
		}
		else if (c == 'n') {
			printf("\n");
			break;
		}
	}

	// ��ʼ���û�������Ŀ¼�������ļ�
	InitUser();

	//printf("�����ļ�ϵͳ����\n");
	if (!Install()) {
		cout << "Failed to install file system." << endl;
		return;
	}
	//printf("�������\n");
}

void InitUser()
{
	// ����root���� 
	cout << "Please type the root password!" << endl;
	bool ok = false;
	char password1[100] = { 0 }, password2[100] = { 0 };
	do {
		cout << "First input ";
		inputPassword(password1);
		cout << "Second input ";
		inputPassword(password2);
		ok = (strcmp(password1, "") != 0 && strcmp(password1, password2) == 0);
		if (!ok)
			cout << "The password can not be empty, and the two entries must be equal" << endl;
		else
			break;
	} while (!ok);

	//--------------- ����Ŀ¼�������ļ� ----------------
	//����Ŀ¼�������ļ�
	Mkdir(Root_Dir_Addr, "home");	//�û�Ŀ¼
	Cd(Root_Dir_Addr, "home");
	Mkdir(Cur_Dir_Addr, "root");

	Cd(Cur_Dir_Addr, "..");
	Mkdir(Cur_Dir_Addr, "etc");	//�����ļ�Ŀ¼
	Cd(Cur_Dir_Addr, "etc");

	char buf[100] = { 0 };

	sprintf(buf, "root:x:%d:%d\n", nextUID++, nextGID++);	//������Ŀ���û������������룺�û�ID���û���ID
	Create(Cur_Dir_Addr, "passwd", buf);	//�����û���Ϣ�ļ�

	char s_passwd[100] = { 0 };
	strcpy(s_passwd, "root:");
	strcat(s_passwd, password1);
	strcat(s_passwd, "\n");

	sprintf(buf, s_passwd);	//������Ŀ���û���������

	Create(Cur_Dir_Addr, "shadow", buf);	//�����û������ļ�
	Chmod(Cur_Dir_Addr, "shadow", 0660);	//�޸�Ȩ�ޣ���ֹ�����û���ȡ���ļ�

	sprintf(buf, "root::0:root\n");	//���ӹ���Ա�û��飬�û����������һ��Ϊ�գ�����û��ʹ�ã������ʶ�ţ������û��б��ã��ָ���
	sprintf(buf + strlen(buf), "user::1:\n");	//������ͨ�û��飬�����û��б�Ϊ��
	Create(Cur_Dir_Addr, "group", buf);	//�����û�����Ϣ�ļ�

	Cd(Cur_Dir_Addr, "..");	//�ص���Ŀ¼

}

bool Format()	//��ʽ��һ����������ļ�
{
	int i, j;

	//��ʼ��������
	superblock->s_INODE_NUM = INODE_NUM;
	superblock->s_BLOCK_NUM = BLOCK_NUM;
	superblock->s_SUPERBLOCK_SIZE = sizeof(SuperBlock);
	superblock->s_INODE_SIZE = INODE_SIZE;
	superblock->s_BLOCK_SIZE = BLOCK_SIZE;
	superblock->s_free_INODE_NUM = INODE_NUM;
	superblock->s_free_BLOCK_NUM = BLOCK_NUM;
	superblock->s_blocks_per_group = BLOCKS_PER_GROUP;
	superblock->s_free_addr = Block_StartAddr;	//���п��ջָ��Ϊ��һ��block
	superblock->s_Superblock_StartAddr = Superblock_StartAddr;
	superblock->s_BlockBitmap_StartAddr = BlockBitmap_StartAddr;
	superblock->s_InodeBitmap_StartAddr = InodeBitmap_StartAddr;
	superblock->s_Block_StartAddr = Block_StartAddr;
	superblock->s_Inode_StartAddr = Inode_StartAddr;
	//���п��ջ�ں��渳ֵ

	//��ʼ��inodeλͼ
	memset(inode_bitmap, 0, sizeof(inode_bitmap));
	fseek(fw, InodeBitmap_StartAddr, SEEK_SET);
	fwrite(inode_bitmap, sizeof(inode_bitmap), 1, fw);

	//��ʼ��blockλͼ
	memset(block_bitmap, 0, sizeof(block_bitmap));
	fseek(fw, BlockBitmap_StartAddr, SEEK_SET);
	fwrite(block_bitmap, sizeof(block_bitmap), 1, fw);

	//��ʼ�����̿��������ݳ������ӷ���֯	
	for (i = BLOCK_NUM / BLOCKS_PER_GROUP - 1; i >= 0; i--) {	//һ��INODE_NUM/BLOCKS_PER_GROUP�飬һ��FREESTACKNUM��128�������̿� ����һ�����̿���Ϊ����
		if (i == BLOCK_NUM / BLOCKS_PER_GROUP - 1)
			superblock->s_free[0] = -1;	//û����һ�����п���
		else
			superblock->s_free[0] = Block_StartAddr + (i + 1)*BLOCKS_PER_GROUP*BLOCK_SIZE;	//ָ����һ�����п�
		for (j = 1; j<BLOCKS_PER_GROUP; j++) {
			superblock->s_free[j] = Block_StartAddr + (i*BLOCKS_PER_GROUP + j)*BLOCK_SIZE;
		}
		fseek(fw, Block_StartAddr + i*BLOCKS_PER_GROUP*BLOCK_SIZE, SEEK_SET);
		fwrite(superblock->s_free, sizeof(superblock->s_free), 1, fw);	//����������̿飬512�ֽ�
	}
	//������д�뵽��������ļ�
	fseek(fw, Superblock_StartAddr, SEEK_SET);
	fwrite(superblock, sizeof(SuperBlock), 1, fw);

	fflush(fw);

	//��ȡinodeλͼ
	fseek(fr, InodeBitmap_StartAddr, SEEK_SET);
	fread(inode_bitmap, sizeof(inode_bitmap), 1, fr);

	//��ȡblockλͼ
	fseek(fr, BlockBitmap_StartAddr, SEEK_SET);
	fread(block_bitmap, sizeof(block_bitmap), 1, fr);

	fflush(fr);

	//������Ŀ¼ "/"
	Inode cur;

	//����inode
	int inoAddr = InodeAlloc();

	//�����inode������̿�
	int blockAddr = BockAlloc();

	//��������̿������һ����Ŀ "."
	DirItem dirlist[16] = { 0 };
	strcpy(dirlist[0].itemName, ".");
	dirlist[0].inodeAddr = inoAddr;

	//д�ش��̿�
	fseek(fw, blockAddr, SEEK_SET);
	fwrite(dirlist, sizeof(dirlist), 1, fw);

	//��inode��ֵ
	cur.i_ino = 0;
	cur.i_atime = time(NULL);
	cur.i_ctime = time(NULL);
	cur.i_mtime = time(NULL);
	strcpy(cur.i_uname, Cur_User_Name);
	strcpy(cur.i_gname, Cur_Group_Name);
	cur.i_cnt = 1;	//һ�����ǰĿ¼,"."
	cur.i_dirBlock[0] = blockAddr;
	for (i = 1; i<10; i++) {
		cur.i_dirBlock[i] = -1;
	}
	cur.i_size = superblock->s_BLOCK_SIZE;
	cur.i_indirBlock_1 = -1;	//ûʹ��һ����ӿ�
	cur.i_mode = MODE_DIR | DIR_DEF_PERMISSION;


	//д��inode
	fseek(fw, inoAddr, SEEK_SET);
	fwrite(&cur, sizeof(Inode), 1, fw);
	fflush(fw);

	return true;
}

bool Install()	//��װ�ļ�ϵͳ������������ļ��еĹؼ���Ϣ�糬������뵽�ڴ�
{
	//��д��������ļ�����ȡ�����飬��ȡinodeλͼ��blockλͼ����ȡ��Ŀ¼����ȡetcĿ¼����ȡ����ԱadminĿ¼����ȡ�û�xiaoĿ¼����ȡ�û�passwd�ļ���

	//��ȡ������
	fseek(fr, Superblock_StartAddr, SEEK_SET);
	fread(superblock, sizeof(SuperBlock), 1, fr);

	//��ȡinodeλͼ
	fseek(fr, InodeBitmap_StartAddr, SEEK_SET);
	fread(inode_bitmap, sizeof(inode_bitmap), 1, fr);

	//��ȡblockλͼ
	fseek(fr, BlockBitmap_StartAddr, SEEK_SET);
	fread(block_bitmap, sizeof(block_bitmap), 1, fr);

	return true;
}

int BockAlloc()	//���̿���亯��
{
	//ʹ�ó������еĿ��п��ջ
	//���㵱ǰջ��
	int top;	//ջ��ָ��
	if (superblock->s_free_BLOCK_NUM == 0) {	//ʣ����п���Ϊ0
		cout << "No free blocks can be allocated." << endl;
		return -1;	//û�пɷ���Ŀ��п飬����-1
	}
	else {	//����ʣ���
		top = (superblock->s_free_BLOCK_NUM - 1) % superblock->s_blocks_per_group;
	}
	//��ջ��ȡ��
	//�������ջ�ף�����ǰ��ŵ�ַ���أ���Ϊջ�׿�ţ�����ջ��ָ����¿��п��ջ����ԭ����ջ
	int retAddr;

	if (top == 0) {
		retAddr = superblock->s_free_addr;
		superblock->s_free_addr = superblock->s_free[0];	//ȡ����һ�����п��п��ջ�Ŀ��п��λ�ã����¿��п��ջָ��

															//ȡ����Ӧ���п����ݣ�����ԭ���Ŀ��п��ջ

															//ȡ����һ�����п��ջ������ԭ����
		fseek(fr, superblock->s_free_addr, SEEK_SET);
		fread(superblock->s_free, sizeof(superblock->s_free), 1, fr);
		fflush(fr);

		superblock->s_free_BLOCK_NUM--;

	}
	else {	//�����Ϊջ�ף���ջ��ָ��ĵ�ַ���أ�ջ��ָ��-1.
		retAddr = superblock->s_free[top];	//���淵�ص�ַ
		superblock->s_free[top] = -1;	//��ջ��
		top--;		//ջ��ָ��-1
		superblock->s_free_BLOCK_NUM--;	//���п���-1

	}

	//���³�����
	fseek(fw, Superblock_StartAddr, SEEK_SET);
	fwrite(superblock, sizeof(SuperBlock), 1, fw);
	fflush(fw);

	//����blockλͼ
	block_bitmap[(retAddr - Block_StartAddr) / BLOCK_SIZE] = 1;
	fseek(fw, (retAddr - Block_StartAddr) / BLOCK_SIZE + BlockBitmap_StartAddr, SEEK_SET);	//(retAddr-Block_StartAddr)/BLOCK_SIZEΪ�ڼ������п�
	fwrite(&block_bitmap[(retAddr - Block_StartAddr) / BLOCK_SIZE], sizeof(bool), 1, fw);
	fflush(fw);

	return retAddr;

}

bool BlockFree(int addr)	//���̿��ͷź���
{
	//�ж�
	//�õ�ַ���Ǵ��̿����ʼ��ַ
	if ((addr - Block_StartAddr) % superblock->s_BLOCK_SIZE != 0) {
		cout << "Address error, this location is not the starting position of block." << endl;
		return false;
	}
	unsigned int bno = (addr - Block_StartAddr) / superblock->s_BLOCK_SIZE;	//inode�ڵ��
																			//�õ�ַ��δʹ�ã������ͷſռ�
	if (block_bitmap[bno] == 0) {
		cout << "The block is not used and can not be released." << endl;
		return false;
	}

	//�����ͷ�
	//���㵱ǰջ��
	int top;	//ջ��ָ��
	if (superblock->s_free_BLOCK_NUM == superblock->s_BLOCK_NUM) {	//û�зǿ��еĴ��̿�
		cout << "There is no free disk block that can not be released." << endl;
		return false;	//û�пɷ���Ŀ��п飬����-1
	}
	else {	//����
		top = (superblock->s_free_BLOCK_NUM - 1) % superblock->s_blocks_per_group;

		//���block����
		char tmp[BLOCK_SIZE] = { 0 };
		fseek(fw, addr, SEEK_SET);
		fwrite(tmp, sizeof(tmp), 1, fw);

		if (top == superblock->s_blocks_per_group - 1) {	//��ջ����

															//�ÿ��п���Ϊ�µĿ��п��ջ
			superblock->s_free[0] = superblock->s_free_addr;	//�µĿ��п��ջ��һ����ַָ��ɵĿ��п��ջָ��
			int i;
			for (i = 1; i<superblock->s_blocks_per_group; i++) {
				superblock->s_free[i] = -1;	//���ջԪ�ص�������ַ
			}
			fseek(fw, addr, SEEK_SET);
			fwrite(superblock->s_free, sizeof(superblock->s_free), 1, fw);	//����������̿飬512�ֽ�

		}
		else {	//ջ��δ��
			top++;	//ջ��ָ��+1
			superblock->s_free[top] = addr;	//ջ���������Ҫ�ͷŵĵ�ַ����Ϊ�µĿ��п�
		}
	}


	//���³�����
	superblock->s_free_BLOCK_NUM++;	//���п���+1
	fseek(fw, Superblock_StartAddr, SEEK_SET);
	fwrite(superblock, sizeof(SuperBlock), 1, fw);

	//����blockλͼ
	block_bitmap[bno] = 0;
	fseek(fw, bno + BlockBitmap_StartAddr, SEEK_SET);	//(addr-Block_StartAddr)/BLOCK_SIZEΪ�ڼ������п�
	fwrite(&block_bitmap[bno], sizeof(bool), 1, fw);
	fflush(fw);

	return true;
}

int InodeAlloc()	//����i�ڵ�������������inode��ַ
{
	//��inodeλͼ��˳����ҿ��е�inode���ҵ��򷵻�inode��ַ������������
	if (superblock->s_free_INODE_NUM == 0) {
		cout << "No free inode can be allocated." << endl;
		return -1;
	}
	else {

		//˳����ҿ��е�inode
		int i;
		for (i = 0; i<superblock->s_INODE_NUM; i++) {
			if (inode_bitmap[i] == 0)	//�ҵ�����inode
				break;
		}


		//���³�����
		superblock->s_free_INODE_NUM--;	//����inode��-1
		fseek(fw, Superblock_StartAddr, SEEK_SET);
		fwrite(superblock, sizeof(SuperBlock), 1, fw);

		//����inodeλͼ
		inode_bitmap[i] = 1;
		fseek(fw, InodeBitmap_StartAddr + i, SEEK_SET);
		fwrite(&inode_bitmap[i], sizeof(bool), 1, fw);
		fflush(fw);

		return Inode_StartAddr + i*superblock->s_INODE_SIZE;
	}
}

bool InodeFree(int addr)	//�ͷ�i���������
{
	//�ж�
	if ((addr - Inode_StartAddr) % superblock->s_INODE_SIZE != 0) {
		cout << "Address error, this location is not the starting position of inode." << endl;
		return false;
	}
	unsigned short ino = (addr - Inode_StartAddr) / superblock->s_INODE_SIZE;	//inode�ڵ��
	if (inode_bitmap[ino] == 0) {
		cout << "The inode is not used and can not be released." << endl;
		return false;
	}

	//���inode����
	Inode tmp = { 0 };
	fseek(fw, addr, SEEK_SET);
	fwrite(&tmp, sizeof(tmp), 1, fw);

	//���³�����
	superblock->s_free_INODE_NUM++;
	//����inode��+1
	fseek(fw, Superblock_StartAddr, SEEK_SET);
	fwrite(superblock, sizeof(SuperBlock), 1, fw);

	//����inodeλͼ
	inode_bitmap[ino] = 0;
	fseek(fw, InodeBitmap_StartAddr + ino, SEEK_SET);
	fwrite(&inode_bitmap[ino], sizeof(bool), 1, fw);
	fflush(fw);

	return true;
}

// -------------------- ���� -----------------------

bool Login(char username[])	//��½����
{
	char password[100] = { 0 };

	if (strlen(username) >= MAX_NAME_SIZE) {
		cout << "The username is too long." << endl;
		return false;
	}
	if (isLogin == true) {
		cout << "You have already logged in." << endl;
		return false;
	}

	// �û���
	if (strcmp(username, "") == 0) {
		InputUsername(username);
		// �п�
		if (strcmp(username, "") == 0) {
			cout << "Username can not be empty." << endl;
			return false;
		}
	}

	//�����û�����
	inputPassword(password);
	// �п�
	if (strcmp(password, "") == 0) {
		cout << "Password can not be empty." << endl;
		return false;
	}

	if (Check(username, password)) {	//�˶��û���������
		isLogin = true;
		return true;
	}
	else {
		isLogin = false;
		return false;
	}
}

void Logout()	//�û�ע��
{
	if (isLogin == false) {
		cout << "You have not logged in yet!" << endl;
		return;
	}

	//�ص���Ŀ¼
	GotoRoot();
	isLogin = false;
}


bool Useradd(char username[])	//�û�ע��
{
	if (strcmp(Cur_User_Name, "root") != 0) {
		cout << "Permission denied." << endl;
		return false;
	}
	int passwd_Inode_Addr = -1;	//�û��ļ�inode��ַ
	int shadow_Inode_Addr = -1;	//�û������ļ�inode��ַ
	int group_Inode_Addr = -1;	//�û����ļ�inode��ַ
	Inode passwd_Inode = { 0 };		//�û��ļ���inode
	Inode shadow_Inode = { 0 };		//�û������ļ���inode
	Inode group_Inode = { 0 };		//�û����ļ�inode
									//ԭ����Ŀ¼
	char bak_Cur_User_Name[110];
	char bak_Cur_User_Name_2[110];
	char bak_Cur_User_Dir_Name[310];
	int bak_Cur_Dir_Addr;
	char bak_Cur_Dir_Name[310];
	char bak_Cur_Group_Name[310];

	Inode cur_dir_inode = { 0 };	//��ǰĿ¼��inode
	size_t i, j;
	DirItem dirlist[16] = { 0 };	//��ʱĿ¼

									//�����ֳ����ص���Ŀ¼
	strcpy(bak_Cur_User_Name, Cur_User_Name);
	strcpy(bak_Cur_User_Dir_Name, Cur_User_Dir_Name);
	bak_Cur_Dir_Addr = Cur_Dir_Addr;
	strcpy(bak_Cur_Dir_Name, Cur_Dir_Name);

	//�����û�Ŀ¼
	GotoRoot();
	Cd(Cur_Dir_Addr, "home");
	//�����ֳ�
	strcpy(bak_Cur_User_Name_2, Cur_User_Name);
	strcpy(bak_Cur_Group_Name, Cur_Group_Name);
	//����
	strcpy(Cur_User_Name, username);
	strcpy(Cur_Group_Name, "user");
	if (!Mkdir(Cur_Dir_Addr, username)) {
		strcpy(Cur_User_Name, bak_Cur_User_Name_2);
		strcpy(Cur_Group_Name, bak_Cur_Group_Name);
		//�ָ��ֳ����ص�ԭ����Ŀ¼
		strcpy(Cur_User_Name, bak_Cur_User_Name);
		strcpy(Cur_User_Dir_Name, bak_Cur_User_Dir_Name);
		Cur_Dir_Addr = bak_Cur_Dir_Addr;
		strcpy(Cur_Dir_Name, bak_Cur_Dir_Name);

		cout << "Add user failure." << endl;
		return false;
	}
	//�ָ��ֳ�
	strcpy(Cur_User_Name, bak_Cur_User_Name_2);
	strcpy(Cur_Group_Name, bak_Cur_Group_Name);

	//�ص���Ŀ¼
	GotoRoot();

	//�����û�Ŀ¼
	Cd(Cur_Dir_Addr, "etc");

	//�����û�����
	char passwd[100] = { 0 };
	inputPassword(passwd);	//��������

						//�ҵ�passwd��shadow�ļ�inode��ַ����ȡ����׼�������Ŀ

						//ȡ����ǰĿ¼��inode
	fseek(fr, Cur_Dir_Addr, SEEK_SET);
	fread(&cur_dir_inode, sizeof(Inode), 1, fr);

	//����ȡ�����̿飬����passwd�ļ���inode��ַ����shadow�ļ���inode��ַ
	for (i = 0; i<10; i++) {
		if (cur_dir_inode.i_dirBlock[i] == -1) {
			continue;
		}
		//����ȡ�����̿�
		fseek(fr, cur_dir_inode.i_dirBlock[i], SEEK_SET);
		fread(&dirlist, sizeof(dirlist), 1, fr);

		for (j = 0; j<16; j++) {	//����Ŀ¼��
			if (strcmp(dirlist[j].itemName, "passwd") == 0 ||	//�ҵ�passwd����shadow��Ŀ
				strcmp(dirlist[j].itemName, "shadow") == 0 ||
				strcmp(dirlist[j].itemName, "group") == 0) {
				Inode tmp;	//��ʱinode
							//ȡ��inode���ж��Ƿ����ļ�
				fseek(fr, dirlist[j].inodeAddr, SEEK_SET);
				fread(&tmp, sizeof(Inode), 1, fr);

				if (((tmp.i_mode >> 9) & 1) == 0) {
					//���ļ�
					//�б���passwd�ļ�����shadow�ļ�
					if (strcmp(dirlist[j].itemName, "passwd") == 0) {
						passwd_Inode_Addr = dirlist[j].inodeAddr;
						passwd_Inode = tmp;
					}
					else if (strcmp(dirlist[j].itemName, "shadow") == 0) {
						shadow_Inode_Addr = dirlist[j].inodeAddr;
						shadow_Inode = tmp;
					}
					else if (strcmp(dirlist[j].itemName, "group") == 0) {
						group_Inode_Addr = dirlist[j].inodeAddr;
						group_Inode = tmp;
					}
				}
			}
		}
		if (passwd_Inode_Addr != -1 && shadow_Inode_Addr != -1)	//���ҵ���
			break;
	}

	//����passwd�ļ������Ƿ�����û�username
	char buf[100000];	//���100K���ݴ�passwd���ļ�����
	char buf2[600];		//�ݴ���̿�����
	j = 0;	//���̿�ָ��
			//ȡ��passwd�ļ�����
	for (i = 0; i<passwd_Inode.i_size; i++) {
		if (i%superblock->s_BLOCK_SIZE == 0) {	//������
												//���µĴ��̿�
			fseek(fr, passwd_Inode.i_dirBlock[i / superblock->s_BLOCK_SIZE], SEEK_SET);
			fread(&buf2, superblock->s_BLOCK_SIZE, 1, fr);
			j = 0;
		}
		buf[i] = buf2[j++];
	}
	buf[i] = '\0';

	if (strstr(buf, username) != NULL) {
		//û�ҵ����û�
		cout << "The user has already existed." << endl;

		//�ָ��ֳ����ص�ԭ����Ŀ¼
		strcpy(Cur_User_Name, bak_Cur_User_Name);
		strcpy(Cur_User_Dir_Name, bak_Cur_User_Dir_Name);
		Cur_Dir_Addr = bak_Cur_Dir_Addr;
		strcpy(Cur_Dir_Name, bak_Cur_Dir_Name);
		return false;
	}

	//��������ڣ���passwd�д������û���Ŀ,�޸�group�ļ�
	sprintf(buf + strlen(buf), "%s:x:%d:%d\n", username, nextUID++, 1);	//������Ŀ���û������������룺�û�ID���û���ID���û���Ϊ��ͨ�û��飬ֵΪ1 
	passwd_Inode.i_size = strlen(buf);
	WriteFile(passwd_Inode, passwd_Inode_Addr, buf);	//���޸ĺ��passwdд���ļ���

														//ȡ��shadow�ļ�����
	j = 0;
	for (i = 0; i<shadow_Inode.i_size; i++) {
		if (i%superblock->s_BLOCK_SIZE == 0) {	//������������̿�
												//���µĴ��̿�
			fseek(fr, shadow_Inode.i_dirBlock[i / superblock->s_BLOCK_SIZE], SEEK_SET);
			fread(&buf2, superblock->s_BLOCK_SIZE, 1, fr);
			j = 0;
		}
		buf[i] = buf2[j++];
	}
	buf[i] = '\0';

	//����shadow��Ŀ
	sprintf(buf + strlen(buf), "%s:%s\n", username, passwd);	//������Ŀ���û���������
	shadow_Inode.i_size = strlen(buf);
	WriteFile(shadow_Inode, shadow_Inode_Addr, buf);	//���޸ĺ������д���ļ���


														//ȡ��group�ļ�����
	j = 0;
	for (i = 0; i<group_Inode.i_size; i++) {
		if (i%superblock->s_BLOCK_SIZE == 0) {	//������������̿�
												//���µĴ��̿�
			fseek(fr, group_Inode.i_dirBlock[i / superblock->s_BLOCK_SIZE], SEEK_SET);
			fread(&buf2, superblock->s_BLOCK_SIZE, 1, fr);
			j = 0;
		}
		buf[i] = buf2[j++];
	}
	buf[i] = '\0';

	//����group����ͨ�û��б�
	if (buf[strlen(buf) - 2] == ':')
		sprintf(buf + strlen(buf) - 1, "%s\n", username);	//���������û�
	else
		sprintf(buf + strlen(buf) - 1, ",%s\n", username);	//���������û�
	group_Inode.i_size = strlen(buf);
	WriteFile(group_Inode, group_Inode_Addr, buf);	//���޸ĺ������д���ļ���

													//�ָ��ֳ����ص�ԭ����Ŀ¼
	strcpy(Cur_User_Name, bak_Cur_User_Name);
	strcpy(Cur_User_Dir_Name, bak_Cur_User_Dir_Name);
	Cur_Dir_Addr = bak_Cur_Dir_Addr;
	strcpy(Cur_Dir_Name, bak_Cur_Dir_Name);

	cout << "Add user success." << endl;
	return true;
}


bool Userdel(char username[])	//�û�ɾ��
{
	if (strcmp(Cur_User_Name, "root") != 0) {
		cout << "Permission denied." << endl;
		return false;
	}
	if (strcmp(username, "root") == 0) {
		cout << "Unable to delete root user." << endl;
		return false;
	}
	int passwd_Inode_Addr = -1;	//�û��ļ�inode��ַ
	int shadow_Inode_Addr = -1;	//�û������ļ�inode��ַ
	int group_Inode_Addr = -1;	//�û����ļ�inode��ַ
	Inode passwd_Inode = { 0 };		//�û��ļ���inode
	Inode shadow_Inode = { 0 };		//�û������ļ���inode
	Inode group_Inode = { 0 };		//�û����ļ�inode
									//ԭ����Ŀ¼
	char bak_Cur_User_Name[110];
	char bak_Cur_User_Dir_Name[310];
	int bak_Cur_Dir_Addr;
	char bak_Cur_Dir_Name[310];

	Inode cur_dir_inode = { 0 };	//��ǰĿ¼��inode
	size_t i, j;
	DirItem dirlist[16] = { 0 };	//��ʱĿ¼

									//�����ֳ����ص���Ŀ¼
	strcpy(bak_Cur_User_Name, Cur_User_Name);
	strcpy(bak_Cur_User_Dir_Name, Cur_User_Dir_Name);
	bak_Cur_Dir_Addr = Cur_Dir_Addr;
	strcpy(bak_Cur_Dir_Name, Cur_Dir_Name);

	//�ص���Ŀ¼
	GotoRoot();

	//�����û�Ŀ¼
	Cd(Cur_Dir_Addr, "etc");

	//�����û�����
	//char passwd[100] = {0};
	//inputPassword(passwd);	//��������

	//�ҵ�passwd��shadow�ļ�inode��ַ����ȡ����׼�������Ŀ

	//ȡ����ǰĿ¼��inode
	fseek(fr, Cur_Dir_Addr, SEEK_SET);
	fread(&cur_dir_inode, sizeof(Inode), 1, fr);

	//����ȡ�����̿飬����passwd�ļ���inode��ַ����shadow�ļ���inode��ַ
	for (i = 0; i<10; i++) {
		if (cur_dir_inode.i_dirBlock[i] == -1) {
			continue;
		}
		//����ȡ�����̿�
		fseek(fr, cur_dir_inode.i_dirBlock[i], SEEK_SET);
		fread(&dirlist, sizeof(dirlist), 1, fr);

		for (j = 0; j<16; j++) {	//����Ŀ¼��
			if (strcmp(dirlist[j].itemName, "passwd") == 0 ||	//�ҵ�passwd����shadow��Ŀ
				strcmp(dirlist[j].itemName, "shadow") == 0 ||
				strcmp(dirlist[j].itemName, "group") == 0) {
				Inode tmp = { 0 };	//��ʱinode
									//ȡ��inode���ж��Ƿ����ļ�
				fseek(fr, dirlist[j].inodeAddr, SEEK_SET);
				fread(&tmp, sizeof(Inode), 1, fr);

				if (((tmp.i_mode >> 9) & 1) == 0) {
					//���ļ�
					//�б���passwd�ļ�����shadow�ļ�
					if (strcmp(dirlist[j].itemName, "passwd") == 0) {
						passwd_Inode_Addr = dirlist[j].inodeAddr;
						passwd_Inode = tmp;
					}
					else if (strcmp(dirlist[j].itemName, "shadow") == 0) {
						shadow_Inode_Addr = dirlist[j].inodeAddr;
						shadow_Inode = tmp;
					}
					else if (strcmp(dirlist[j].itemName, "group") == 0) {
						group_Inode_Addr = dirlist[j].inodeAddr;
						group_Inode = tmp;
					}
				}
			}
		}
		if (passwd_Inode_Addr != -1 && shadow_Inode_Addr != -1)	//���ҵ���
			break;
	}

	//����passwd�ļ������Ƿ�����û�username
	char buf[100000];	//���100K���ݴ�passwd���ļ�����
	char buf2[600];		//�ݴ���̿�����
	j = 0;	//���̿�ָ��
			//ȡ��passwd�ļ�����
	for (i = 0; i<passwd_Inode.i_size; i++) {
		if (i%superblock->s_BLOCK_SIZE == 0) {	//������
												//���µĴ��̿�
			fseek(fr, passwd_Inode.i_dirBlock[i / superblock->s_BLOCK_SIZE], SEEK_SET);
			fread(&buf2, superblock->s_BLOCK_SIZE, 1, fr);
			j = 0;
		}
		buf[i] = buf2[j++];
	}
	buf[i] = '\0';

	if (strstr(buf, username) == NULL) {
		//û�ҵ����û�
		cout << "User does not exist." << endl;

		//�ָ��ֳ����ص�ԭ����Ŀ¼
		strcpy(Cur_User_Name, bak_Cur_User_Name);
		strcpy(Cur_User_Dir_Name, bak_Cur_User_Dir_Name);
		Cur_Dir_Addr = bak_Cur_Dir_Addr;
		strcpy(Cur_Dir_Name, bak_Cur_Dir_Name);
		return false;
	}

	//������ڣ���passwd��shadow��group��ɾ�����û�����Ŀ
	//ɾ��passwd��Ŀ
	char *p = strstr(buf, username);
	*p = '\0';
	while ((*p) != '\n')	//�ճ��м�Ĳ���
		p++;
	p++;
	strcat(buf, p);
	passwd_Inode.i_size = strlen(buf);	//�����ļ���С
	WriteFile(passwd_Inode, passwd_Inode_Addr, buf);	//���޸ĺ��passwdд���ļ���

														//ȡ��shadow�ļ�����
	j = 0;
	for (i = 0; i<shadow_Inode.i_size; i++) {
		if (i%superblock->s_BLOCK_SIZE == 0) {	//������������̿�
												//���µĴ��̿�
			fseek(fr, shadow_Inode.i_dirBlock[i / superblock->s_BLOCK_SIZE], SEEK_SET);
			fread(&buf2, superblock->s_BLOCK_SIZE, 1, fr);
			j = 0;
		}
		buf[i] = buf2[j++];
	}
	buf[i] = '\0';

	//ɾ��shadow��Ŀ
	p = strstr(buf, username);
	*p = '\0';
	while ((*p) != '\n')	//�ճ��м�Ĳ���
		p++;
	p++;
	strcat(buf, p);
	shadow_Inode.i_size = strlen(buf);	//�����ļ���С
	WriteFile(shadow_Inode, shadow_Inode_Addr, buf);	//���޸ĺ������д���ļ���


	//ȡ��group�ļ�����
	j = 0;
	for (i = 0; i<group_Inode.i_size; i++) {
		if (i%superblock->s_BLOCK_SIZE == 0) {	//������������̿�
												//���µĴ��̿�
			fseek(fr, group_Inode.i_dirBlock[i / superblock->s_BLOCK_SIZE], SEEK_SET);
			fread(&buf2, superblock->s_BLOCK_SIZE, 1, fr);
			j = 0;
		}
		buf[i] = buf2[j++];
	}
	buf[i] = '\0';

	//����group����ͨ�û��б�
	p = strstr(buf, username);
	*p = '\0';
	while ((*p) != '\n' && (*p) != ',')	//�ճ��м�Ĳ���
		p++;
	if ((*p) == ',')
		p++;
	strcat(buf, p);
	group_Inode.i_size = strlen(buf);	//�����ļ���С
	WriteFile(group_Inode, group_Inode_Addr, buf);	//���޸ĺ������д���ļ���

													////�ָ��ֳ����ص�ԭ����Ŀ¼
													//strcpy(Cur_User_Name, bak_Cur_User_Name);
													//strcpy(Cur_User_Dir_Name, bak_Cur_User_Dir_Name);
													//Cur_Dir_Addr = bak_Cur_Dir_Addr;
													//strcpy(Cur_Dir_Name, bak_Cur_Dir_Name);

													//ɾ���û�Ŀ¼	
	Cur_Dir_Addr = Root_Dir_Addr;	//��ǰ�û�Ŀ¼��ַ��Ϊ��Ŀ¼��ַ
	strcpy(Cur_Dir_Name, "/");		//��ǰĿ¼��Ϊ"/"
	Cd(Cur_Dir_Addr, "home");
	Rmdir(Cur_Dir_Addr, username);

	// �����root�û�����a�û�Ŀ¼��ɾ��a�û�������aĿ¼������
	// ���Ѹġ��˴�Ӧ�ø�Ϊ���жϵ�ǰ�û�Ŀ¼�Ƿ�Ϊa�û���Ŀ¼�������ã������򷵻ظ�Ŀ¼������ָ�Ϊԭ����Ŀ¼

	// User_Dir_to_be_deleted ΪӦ��ɾ�����û�Ŀ¼
	char c, User_Dir_to_be_deleted[110] = { 0 }, cmp_Cur_Dir_Name[311];
	strcpy(User_Dir_to_be_deleted, "/home/");
	strcat(User_Dir_to_be_deleted, username);
	strcat(User_Dir_to_be_deleted, "/");

	strcpy(cmp_Cur_Dir_Name, bak_Cur_Dir_Name);
	strcat(cmp_Cur_Dir_Name, "/");

	for (i = 0, j = 0; j < 3; i++) {
		c = cmp_Cur_Dir_Name[i];
		if (c != User_Dir_to_be_deleted[i])
			break;
		else if (c == '/')
			j++;
	}

	if (j != 3) {
		//�ָ��ֳ����ص�ԭ����Ŀ¼
		strcpy(Cur_User_Name, bak_Cur_User_Name);
		strcpy(Cur_User_Dir_Name, bak_Cur_User_Dir_Name);
		Cur_Dir_Addr = bak_Cur_Dir_Addr;
		strcpy(Cur_Dir_Name, bak_Cur_Dir_Name);
	}
	else {
		//GotoRoot();
		//Cd(Root_Dir_Addr, "home");
		//Cd(Cur_Dir_Addr, "root");
		//strcpy(Cur_User_Name, "root");
		//strcpy(Cur_User_Dir_Name, "root");

		// ��root�û�����a�û�Ŀ¼��ɾ��a�û�����ص���Ŀ¼
		strcpy(Cur_User_Name, "root");
		strcpy(Cur_User_Dir_Name, "root");
		Cur_Dir_Addr = Root_Dir_Addr;	//��ǰ�û�Ŀ¼��ַ��Ϊ��Ŀ¼��ַ
		strcpy(Cur_Dir_Name, "/");		//��ǰĿ¼��Ϊ"/"

										// ɾ��usernameĿ¼
		Cd(Cur_Dir_Addr, "home");
		Rmdir(Cur_Dir_Addr, username);
		// �ص�/home/root
		Cd(Cur_Dir_Addr, "..");
	}

	cout << "User has deleted." << endl;
	return true;

}

void Chmod(int parinoAddr, char name[], int pmode)	//�޸��ļ���Ŀ¼Ȩ��
{
	if (strlen(name) >= MAX_NAME_SIZE) {
		cout << "The directory name is too long." << endl;
		return;
	}
	if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
		cout << "Operation error." << endl;
		return;
	}
	//ȡ�����ļ���Ŀ¼inode
	Inode cur = { 0 }, fileInode = { 0 };
	fseek(fr, parinoAddr, SEEK_SET);
	fread(&cur, sizeof(Inode), 1, fr);

	//����ȡ�����̿�
	int i = 0, j;
	DirItem dirlist[16] = { 0 };
	while (i<160) {
		if (cur.i_dirBlock[i / 16] == -1) {
			i += 16;
			continue;
		}
		//ȡ�����̿�
		int parblockAddr = cur.i_dirBlock[i / 16];
		fseek(fr, parblockAddr, SEEK_SET);
		fread(&dirlist, sizeof(dirlist), 1, fr);
		fflush(fr);

		//����ô��̿��е�����Ŀ¼��
		for (j = 0; j<16; j++) {
			if (strcmp(dirlist[j].itemName, name) == 0) {	//�ҵ���Ŀ¼�����ļ�
															//ȡ����Ӧ��inode
				fseek(fr, dirlist[j].inodeAddr, SEEK_SET);
				fread(&fileInode, sizeof(Inode), 1, fr);
				fflush(fr);
				goto label;
			}
		}
		i++;
	}
label:
	if (i >= 160) {
		cout << "File does not exist." << endl;
		return;
	}

	//�ж��Ƿ��Ǳ��û�
	if (strcmp(Cur_User_Name, fileInode.i_uname) != 0 && strcmp(Cur_User_Name, "root") != 0) {
		cout << "Permission denied." << endl;
		return;
	}

	//��inode��mode���Խ����޸�
	fileInode.i_mode = (fileInode.i_mode >> 9 << 9) | pmode;	//�޸�Ȩ��

																//��inodeд�ش���
	fseek(fw, dirlist[j].inodeAddr, SEEK_SET);
	fwrite(&fileInode, sizeof(Inode), 1, fw);
	fflush(fw);
}

void Touch(int parinoAddr, char name[], char buf[])	//touch������ļ��������ַ�
{
	//���ж��ļ��Ƿ��Ѵ��ڡ�������ڣ�������ļ����༭
	if (strlen(name) >= MAX_NAME_SIZE) {
		cout << "The filename name is too long.." << endl;
		return;
	}
	//��������ͬ���ļ����еĻ���ʾ�����˳�����û�еĻ�������һ�����ļ�
	DirItem dirlist[16];	//��ʱĿ¼�嵥

							//�������ַȡ��inode
	Inode cur = { 0 }, fileInode = { 0 };
	fseek(fr, parinoAddr, SEEK_SET);
	fread(&cur, sizeof(Inode), 1, fr);

	//�ж��ļ�ģʽ��6Ϊowner��3Ϊgroup��0Ϊother
	int filemode;
	if (strcmp(Cur_User_Name, cur.i_uname) == 0)
		filemode = 6;
	else if (strcmp(Cur_User_Name, cur.i_gname) == 0)
		filemode = 3;
	else
		filemode = 0;

	int i = 0, j;
	int dno;
	int fileInodeAddr = -1;	//�ļ���inode��ַ
	while (i<160) {
		//160��Ŀ¼��֮�ڣ�����ֱ����ֱ�ӿ�����
		dno = i / 16;	//�ڵڼ���ֱ�ӿ���

		if (cur.i_dirBlock[dno] == -1) {
			i += 16;
			continue;
		}
		fseek(fr, cur.i_dirBlock[dno], SEEK_SET);
		fread(dirlist, sizeof(dirlist), 1, fr);
		fflush(fr);

		//����ô��̿��е�����Ŀ¼��
		for (j = 0; j<16; j++) {
			if (strcmp(dirlist[j].itemName, name) == 0) {
				//������ȡ��inode���ж��Ƿ����ļ�
				fseek(fr, dirlist[j].inodeAddr, SEEK_SET);
				fread(&fileInode, sizeof(Inode), 1, fr);
				if (((fileInode.i_mode >> 9) & 1) == 0) {	//���ļ�����������ʾ�����˳�����
					cout << "file already exist." << endl;
					return;
				}
			}
			i++;
		}
	}

	//�ļ������ڣ�����һ�����ļ�
	if (((cur.i_mode >> filemode >> 1) & 1) == 1) {
		//��д�����Դ����ļ�
		buf[0] = '\0';
		Create(parinoAddr, name, buf);	//�����ļ�
	}
	else {
		cout << "Permission denied." << endl;
		return;
	}

}


bool Mkdir(int parinoAddr, char name[])	//Ŀ¼������������������һ��Ŀ¼�ļ�inode��ַ ,Ҫ������Ŀ¼��
{
	if (strlen(name) >= MAX_NAME_SIZE) {
		cout << "The directory name is too long." << endl;
		return false;
	}

	DirItem dirlist[16] = { 0 };	//��ʱĿ¼�嵥

									//�������ַȡ��inode
	Inode cur = { 0 };
	fseek(fr, parinoAddr, SEEK_SET);
	fread(&cur, sizeof(Inode), 1, fr);

	int i = 0;
	int cnt = cur.i_cnt + 1;	//Ŀ¼����
	int posi = -1, posj = -1;
	while (i<160) {
		//160��Ŀ¼��֮�ڣ�����ֱ����ֱ�ӿ�����
		int dno = i / 16;	//�ڵڼ���ֱ�ӿ���

		if (cur.i_dirBlock[dno] == -1) {
			i += 16;
			continue;
		}
		//ȡ�����ֱ�ӿ飬Ҫ�����Ŀ¼��Ŀ��λ��
		fseek(fr, cur.i_dirBlock[dno], SEEK_SET);
		fread(dirlist, sizeof(dirlist), 1, fr);
		fflush(fr);

		//����ô��̿��е�����Ŀ¼��
		int j;
		for (j = 0; j<16; j++) {

			if (strcmp(dirlist[j].itemName, name) == 0) {
				Inode tmp = { 0 };
				fseek(fr, dirlist[j].inodeAddr, SEEK_SET);
				fread(&tmp, sizeof(Inode), 1, fr);
				if (((tmp.i_mode >> 9) & 1) == 1) {	//����Ŀ¼
					cout << "Directory already exists." << endl;
					return false;
				}
			}
			else if (strcmp(dirlist[j].itemName, "") == 0) {
				//�ҵ�һ�����м�¼������Ŀ¼���������λ�� 
				//��¼���λ��
				if (posi == -1) {
					posi = dno;
					posj = j;
				}

			}
			i++;
		}

	}
	/*  δд�� */

	if (posi != -1) {	//�ҵ��������λ��

						//ȡ�����ֱ�ӿ飬Ҫ�����Ŀ¼��Ŀ��λ��
		fseek(fr, cur.i_dirBlock[posi], SEEK_SET);
		fread(dirlist, sizeof(dirlist), 1, fr);
		fflush(fr);

		//�������Ŀ¼��
		strcpy(dirlist[posj].itemName, name);	//Ŀ¼��
												//д��������¼ "." ".."���ֱ�ָ��ǰinode�ڵ��ַ���͸�inode�ڵ�
		int chiinoAddr = InodeAlloc();	//���䵱ǰ�ڵ��ַ 
		if (chiinoAddr == -1) {
			cout << "Inode allocation failure." << endl;
			return false;
		}
		dirlist[posj].inodeAddr = chiinoAddr; //������µ�Ŀ¼�����inode��ַ

											  //��������Ŀ��inode
		Inode p = { 0 };
		p.i_ino = (chiinoAddr - Inode_StartAddr) / superblock->s_INODE_SIZE;
		p.i_atime = time(NULL);
		p.i_ctime = time(NULL);
		p.i_mtime = time(NULL);
		strcpy(p.i_uname, Cur_User_Name);
		strcpy(p.i_gname, Cur_Group_Name);
		p.i_cnt = 2;	//�������ǰĿ¼,"."��".."

						//�������inode�Ĵ��̿飬�ڴ��̺���д��������¼ . �� ..
		int curblockAddr = BockAlloc();
		if (curblockAddr == -1) {
			cout << "Block allocation failure." << endl;
			return false;
		}
		DirItem dirlist2[16] = { 0 };	//��ʱĿ¼���б� - 2
		strcpy(dirlist2[0].itemName, ".");
		strcpy(dirlist2[1].itemName, "..");
		dirlist2[0].inodeAddr = chiinoAddr;	//��ǰĿ¼inode��ַ
		dirlist2[1].inodeAddr = parinoAddr;	//��Ŀ¼inode��ַ

											//д�뵽��ǰĿ¼�Ĵ��̿�
		fseek(fw, curblockAddr, SEEK_SET);
		fwrite(dirlist2, sizeof(dirlist2), 1, fw);

		p.i_dirBlock[0] = curblockAddr;
		int k;
		for (k = 1; k<10; k++) {
			p.i_dirBlock[k] = -1;
		}
		p.i_size = superblock->s_BLOCK_SIZE;
		p.i_indirBlock_1 = -1;	//ûʹ��һ����ӿ�
		p.i_mode = MODE_DIR | DIR_DEF_PERMISSION;

		//��inodeд�뵽�����inode��ַ
		fseek(fw, chiinoAddr, SEEK_SET);
		fwrite(&p, sizeof(Inode), 1, fw);

		//����ǰĿ¼�Ĵ��̿�д��
		fseek(fw, cur.i_dirBlock[posi], SEEK_SET);
		fwrite(dirlist, sizeof(dirlist), 1, fw);

		//д��inode
		cur.i_cnt++;
		fseek(fw, parinoAddr, SEEK_SET);
		fwrite(&cur, sizeof(Inode), 1, fw);
		fflush(fw);

		return true;
	}
	else {
		cout << "No free directory entry was found, directory creation failed." << endl;
		return false;
	}
}


bool Rmdir(int parinoAddr, char name[])	//Ŀ¼ɾ������
{
	if (strlen(name) >= MAX_NAME_SIZE) {
		cout << "The directory name is too long." << endl;
		return false;
	}

	if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
		cout << "Operation error." << endl;
		return 0;
	}

	//�������ַȡ��inode
	Inode cur = { 0 };
	fseek(fr, parinoAddr, SEEK_SET);
	fread(&cur, sizeof(Inode), 1, fr);

	//ȡ��Ŀ¼����
	int cnt = cur.i_cnt;

	//�ж��ļ�ģʽ��6Ϊowner��3Ϊgroup��0Ϊother
	int filemode;
	if (strcmp(Cur_User_Name, cur.i_uname) == 0)
		filemode = 6;
	else if (strcmp(Cur_User_Name, cur.i_gname) == 0)
		filemode = 3;
	else
		filemode = 0;

	if ((((cur.i_mode >> filemode >> 1) & 1) == 0) && (strcmp(Cur_User_Name, "root") != 0)) {
		//û��д��Ȩ��
		cout << "Permission denied." << endl;
		return false;
	}


	//����ȡ�����̿�
	int i = 0;
	while (i<160) {	//С��160
		DirItem dirlist[16] = { 0 };

		if (cur.i_dirBlock[i / 16] == -1) {
			i += 16;
			continue;
		}
		//ȡ�����̿�
		int parblockAddr = cur.i_dirBlock[i / 16];
		fseek(fr, parblockAddr, SEEK_SET);
		fread(&dirlist, sizeof(dirlist), 1, fr);

		//�ҵ�Ҫɾ����Ŀ¼
		int j;
		for (j = 0; j<16; j++) {
			Inode tmp = { 0 };
			//ȡ����Ŀ¼���inode���жϸ�Ŀ¼����Ŀ¼�����ļ�
			fseek(fr, dirlist[j].inodeAddr, SEEK_SET);
			fread(&tmp, sizeof(Inode), 1, fr);

			if (strcmp(dirlist[j].itemName, name) == 0) {
				if (((tmp.i_mode >> 9) & 1) == 1) {	//�ҵ�Ŀ¼
													//��Ŀ¼

					Rmall(dirlist[j].inodeAddr);

					//ɾ����Ŀ¼��Ŀ��д�ش���
					strcpy(dirlist[j].itemName, "");
					dirlist[j].inodeAddr = -1;
					fseek(fw, parblockAddr, SEEK_SET);
					fwrite(&dirlist, sizeof(dirlist), 1, fw);
					cur.i_cnt--;
					fseek(fw, parinoAddr, SEEK_SET);
					fwrite(&cur, sizeof(Inode), 1, fw);

					fflush(fw);
					return true;
				}
				else {
					//����Ŀ¼������
				}
			}
			i++;
		}

	}

	cout << "The directory was not found." << endl;
	return false;
}

bool Create(int parinoAddr, char name[], char buf[])	//�����ļ��������ڸ�Ŀ¼�´����ļ����ļ����ݴ���buf
{
	if (strlen(name) >= MAX_NAME_SIZE) {
		cout << "The filename is too long." << endl;
		return false;
	}

	DirItem dirlist[16];	//��ʱĿ¼�嵥

							//�������ַȡ��inode
	Inode cur = { 0 };
	fseek(fr, parinoAddr, SEEK_SET);
	fread(&cur, sizeof(Inode), 1, fr);

	int i = 0;
	int posi = -1, posj = -1;	//�ҵ���Ŀ¼λ��
	int dno;
	int cnt = cur.i_cnt + 1;	//Ŀ¼����
	while (i<160) {
		//160��Ŀ¼��֮�ڣ�����ֱ����ֱ�ӿ�����
		dno = i / 16;	//�ڵڼ���ֱ�ӿ���

		if (cur.i_dirBlock[dno] == -1) {
			i += 16;
			continue;
		}
		fseek(fr, cur.i_dirBlock[dno], SEEK_SET);
		fread(dirlist, sizeof(dirlist), 1, fr);
		fflush(fr);

		//����ô��̿��е�����Ŀ¼��
		int j;
		for (j = 0; j<16; j++) {

			if (posi == -1 && strcmp(dirlist[j].itemName, "") == 0) {
				//�ҵ�һ�����м�¼�������ļ����������λ�� 
				posi = dno;
				posj = j;
			}
			else if (strcmp(dirlist[j].itemName, name) == 0) {
				//������ȡ��inode���ж��Ƿ����ļ�
				Inode cur2 = { 0 };
				fseek(fr, dirlist[j].inodeAddr, SEEK_SET);
				fread(&cur2, sizeof(Inode), 1, fr);
				if (((cur2.i_mode >> 9) & 1) == 0) {	//���ļ������������ܴ����ļ�
					cout << "File already exist." << endl;
					buf[0] = '\0';
					return false;
				}
			}
			i++;
		}

	}
	if (posi != -1) {	//֮ǰ�ҵ�һ��Ŀ¼����
						//ȡ��֮ǰ�Ǹ�����Ŀ¼���Ӧ�Ĵ��̿�
		fseek(fr, cur.i_dirBlock[posi], SEEK_SET);
		fread(dirlist, sizeof(dirlist), 1, fr);
		fflush(fr);

		//�������Ŀ¼��
		strcpy(dirlist[posj].itemName, name);	//�ļ���
		int chiinoAddr = InodeAlloc();	//���䵱ǰ�ڵ��ַ 
		if (chiinoAddr == -1) {
			cout << "Inode allocation failure." << endl;
			return false;
		}
		dirlist[posj].inodeAddr = chiinoAddr; //������µ�Ŀ¼�����inode��ַ

											  //��������Ŀ��inode
		Inode p = { 0 };
		p.i_ino = (chiinoAddr - Inode_StartAddr) / superblock->s_INODE_SIZE;
		p.i_atime = time(NULL);
		p.i_ctime = time(NULL);
		p.i_mtime = time(NULL);
		strcpy(p.i_uname, Cur_User_Name);
		strcpy(p.i_gname, Cur_Group_Name);
		p.i_cnt = 1;	//ֻ��һ���ļ�ָ��


						//��buf���ݴ浽���̿� 
		int k;
		int len = strlen(buf);	//�ļ����ȣ���λΪ�ֽ�
		for (k = 0; k<len; k += superblock->s_BLOCK_SIZE) {	//���10�Σ�10�����̿죬�����5K
															//�������inode�Ĵ��̿飬�ӿ���̨��ȡ����
			int curblockAddr = BockAlloc();
			if (curblockAddr == -1) {
				cout << "Block allocation failure." << endl;
				return false;
			}
			p.i_dirBlock[k / superblock->s_BLOCK_SIZE] = curblockAddr;
			//д�뵽��ǰĿ¼�Ĵ��̿�
			fseek(fw, curblockAddr, SEEK_SET);
			fwrite(buf + k, superblock->s_BLOCK_SIZE, 1, fw);
		}


		for (k = len / superblock->s_BLOCK_SIZE + 1; k<10; k++) {
			p.i_dirBlock[k] = -1;
		}
		if (len == 0) {	//����Ϊ0�Ļ�Ҳ�ָ���һ��block
			int curblockAddr = BockAlloc();
			if (curblockAddr == -1) {
				cout << "Block allocation failure." << endl;
				return false;
			}
			p.i_dirBlock[k / superblock->s_BLOCK_SIZE] = curblockAddr;
			//д�뵽��ǰĿ¼�Ĵ��̿�
			fseek(fw, curblockAddr, SEEK_SET);
			fwrite(buf, superblock->s_BLOCK_SIZE, 1, fw);

		}
		p.i_size = len;
		p.i_indirBlock_1 = -1;	//ûʹ��һ����ӿ�
		p.i_mode = 0;
		p.i_mode = MODE_FILE | FILE_DEF_PERMISSION;

		//��inodeд�뵽�����inode��ַ
		fseek(fw, chiinoAddr, SEEK_SET);
		fwrite(&p, sizeof(Inode), 1, fw);

		//����ǰĿ¼�Ĵ��̿�д��
		fseek(fw, cur.i_dirBlock[posi], SEEK_SET);
		fwrite(dirlist, sizeof(dirlist), 1, fw);

		//д��inode
		cur.i_cnt++;
		fseek(fw, parinoAddr, SEEK_SET);
		fwrite(&cur, sizeof(Inode), 1, fw);
		fflush(fw);

		return true;
	}
	else
		return false;
}

bool Rm(int parinoAddr, char name[])		//ɾ���ļ��������ڵ�ǰĿ¼��ɾ���ļ�
{
	if (strlen(name) >= MAX_NAME_SIZE) {
		cout << "The directory name is too long." << endl;
		return false;
	}

	//�������ַȡ��inode
	Inode cur = { 0 };
	fseek(fr, parinoAddr, SEEK_SET);
	fread(&cur, sizeof(Inode), 1, fr);

	//ȡ��Ŀ¼����
	int cnt = cur.i_cnt;

	//�ж��ļ�ģʽ��6Ϊowner��3Ϊgroup��0Ϊother
	int filemode;
	if (strcmp(Cur_User_Name, cur.i_uname) == 0)
		filemode = 6;
	else if (strcmp(Cur_User_Name, cur.i_gname) == 0)
		filemode = 3;
	else
		filemode = 0;

	if (((cur.i_mode >> filemode >> 1) & 1) == 0) {
		//û��д��Ȩ��
		cout << "Permission denied." << endl;
		return false;
	}

	//����ȡ�����̿�
	int i = 0;
	while (i<160) {	//С��160
		DirItem dirlist[16] = { 0 };

		if (cur.i_dirBlock[i / 16] == -1) {
			i += 16;
			continue;
		}
		//ȡ�����̿�
		int parblockAddr = cur.i_dirBlock[i / 16];
		fseek(fr, parblockAddr, SEEK_SET);
		fread(&dirlist, sizeof(dirlist), 1, fr);

		//�ҵ�Ҫɾ����Ŀ¼
		int pos;
		for (pos = 0; pos<16; pos++) {
			Inode tmp = { 0 };
			//ȡ����Ŀ¼���inode���жϸ�Ŀ¼����Ŀ¼�����ļ�
			fseek(fr, dirlist[pos].inodeAddr, SEEK_SET);
			fread(&tmp, sizeof(Inode), 1, fr);

			if (strcmp(dirlist[pos].itemName, name) == 0) {
				if (((tmp.i_mode >> 9) & 1) == 1) {	//�ҵ�Ŀ¼
													//��Ŀ¼������
				}
				else {
					//���ļ�

					//�ͷ�block
					int k;
					for (k = 0; k<10; k++)
						if (tmp.i_dirBlock[k] != -1)
							BlockFree(tmp.i_dirBlock[k]);

					//�ͷ�inode
					InodeFree(dirlist[pos].inodeAddr);

					//ɾ����Ŀ¼��Ŀ��д�ش���
					strcpy(dirlist[pos].itemName, "");
					dirlist[pos].inodeAddr = -1;
					fseek(fw, parblockAddr, SEEK_SET);
					fwrite(&dirlist, sizeof(dirlist), 1, fw);
					cur.i_cnt--;
					fseek(fw, parinoAddr, SEEK_SET);
					fwrite(&cur, sizeof(Inode), 1, fw);

					fflush(fw);
					return true;
				}
			}
			i++;
		}

	}

	cout << "The file was not found." << endl;
	return false;
}

void Ls(int parinoAddr)		//��ʾ��ǰĿ¼�µ������ļ����ļ��С���������ǰĿ¼��inode�ڵ��ַ 
{
	Inode cur = { 0 };
	//ȡ�����inode
	fseek(fr, parinoAddr, SEEK_SET);
	fread(&cur, sizeof(Inode), 1, fr);
	fflush(fr);

	//ȡ��Ŀ¼����
	int cnt = cur.i_cnt;

	//�ж��ļ�ģʽ��6Ϊowner��3Ϊgroup��0Ϊother
	int filemode;
	if (strcmp(Cur_User_Name, cur.i_uname) == 0)
		filemode = 6;
	else if (strcmp(Cur_User_Name, cur.i_gname) == 0)
		filemode = 3;
	else
		filemode = 0;

	if (((cur.i_mode >> filemode >> 2) & 1) == 0) {
		//û�ж�ȡȨ��
		cout << "Permission denied." << endl;
		return;
	}

	//����ȡ�����̿�
	int i = 0;
	while (i<cnt && i<160) {
		DirItem dirlist[16] = { 0 };
		if (cur.i_dirBlock[i / 16] == -1) {
			i += 16;
			continue;
		}
		//ȡ�����̿�
		int parblockAddr = cur.i_dirBlock[i / 16];
		fseek(fr, parblockAddr, SEEK_SET);
		fread(&dirlist, sizeof(dirlist), 1, fr);
		fflush(fr);

		//����ô��̿��е�����Ŀ¼��
		int j;
		for (j = 0; j<16 && i<cnt; j++) {
			Inode tmp = { 0 };
			//ȡ����Ŀ¼���inode���жϸ�Ŀ¼����Ŀ¼�����ļ�
			fseek(fr, dirlist[j].inodeAddr, SEEK_SET);
			fread(&tmp, sizeof(Inode), 1, fr);
			fflush(fr);

			if (strcmp(dirlist[j].itemName, "") == 0) {
				continue;
			}

			//�����Ϣ
			if (((tmp.i_mode >> 9) & 1) == 1) {
				printf("d");
			}
			else {
				printf("-");
			}

			tm *ptr;	//�洢ʱ��
			ptr = gmtime(&tmp.i_mtime);

			//���Ȩ����Ϣ
			int t = 8;
			while (t >= 0) {
				if (((tmp.i_mode >> t) & 1) == 1) {
					if (t % 3 == 2)	printf("r");
					if (t % 3 == 1)	printf("w");
					if (t % 3 == 0)	printf("x");
				}
				else {
					printf("-");
				}
				t--;
			}
			printf(" ");

			//����
			printf("%d ", tmp.i_cnt);	//����
			printf("%s ", tmp.i_uname);	//�ļ������û���
			printf("%s\t", tmp.i_gname);	//�ļ������û���
			printf("%d B\t", tmp.i_size);	//�ļ���С
			printf("%d.%d.%d %02d:%02d:%02d  ", 1900 + ptr->tm_year, ptr->tm_mon + 1, ptr->tm_mday, (8 + ptr->tm_hour) % 24, ptr->tm_min, ptr->tm_sec);	//��һ���޸ĵ�ʱ��
			printf("%s", dirlist[j].itemName);	//�ļ���
			printf("\n");
			i++;
		}

	}
	/*  δд�� */

}

void Cd(int parinoAddr, char name[])	//���뵱ǰĿ¼�µ�nameĿ¼
{
	// Ŀ¼��
	if (strlen(name) >= MAX_NAME_SIZE) {
		cout << "The directory name is too long." << endl;
		return;
	}
	// û������Ŀ¼������Cd���û�Ŀ¼��
	if (strcmp(name, "") == 0) {
		while (strcmp(Cur_Dir_Name, "/") != 0) {	// �����root�û�����a�û�Ŀ¼��ɾ��a�û�����"Cd\n",����Ϊû��".."Ŀ¼�����´˴���ѭ������
			Cd(Cur_Dir_Addr, "..");
		}
		Cd(Cur_Dir_Addr, "home");
		Cd(Cur_Dir_Addr, Cur_User_Name);
		return;
	}

	//ȡ����ǰĿ¼��inode
	Inode cur = { 0 };
	fseek(fr, parinoAddr, SEEK_SET);
	fread(&cur, sizeof(Inode), 1, fr);

	//����ȡ��inode��Ӧ�Ĵ��̿飬������û������Ϊname��Ŀ¼��
	int i = 0;

	//ȡ��Ŀ¼����
	int cnt = cur.i_cnt;

	//�ж��ļ�ģʽ��6Ϊowner��3Ϊgroup��0Ϊother
	int filemode;
	if (strcmp(Cur_User_Name, cur.i_uname) == 0)
		filemode = 6;
	else if (strcmp(Cur_User_Name, cur.i_gname) == 0)
		filemode = 3;
	else
		filemode = 0;

	while (i<160) {
		DirItem dirlist[16] = { 0 };
		if (cur.i_dirBlock[i / 16] == -1) {
			i += 16;
			continue;
		}
		//ȡ�����̿�
		int parblockAddr = cur.i_dirBlock[i / 16];
		fseek(fr, parblockAddr, SEEK_SET);
		fread(&dirlist, sizeof(dirlist), 1, fr);

		//����ô��̿��е�����Ŀ¼��
		int j;
		for (j = 0; j<16; j++) {
			if (strcmp(dirlist[j].itemName, name) == 0) {
				Inode tmp = { 0 };
				//ȡ����Ŀ¼���inode���жϸ�Ŀ¼����Ŀ¼�����ļ�
				fseek(fr, dirlist[j].inodeAddr, SEEK_SET);
				fread(&tmp, sizeof(Inode), 1, fr);

				if (((tmp.i_mode >> 9) & 1) == 1) {
					//�ҵ���Ŀ¼���ж��Ƿ���н���Ȩ��
					if (((tmp.i_mode >> filemode >> 0) & 1) == 0 && strcmp(Cur_User_Name, "root") != 0) {	//root�û�����Ŀ¼�����Բ鿴 

						cout << "Permission denied." << endl;
						return;
					}

					//�ҵ���Ŀ¼������Ŀ¼��������ǰĿ¼

					Cur_Dir_Addr = dirlist[j].inodeAddr;
					if (strcmp(dirlist[j].itemName, ".") == 0) {
						//��Ŀ¼������
					}
					else if (strcmp(dirlist[j].itemName, "..") == 0) {
						//��һ��Ŀ¼
						int k;
						for (k = strlen(Cur_Dir_Name); k >= 0; k--)
							if (Cur_Dir_Name[k] == '/')
								break;
						Cur_Dir_Name[k] = '\0';
						if (strlen(Cur_Dir_Name) == 0)
							Cur_Dir_Name[0] = '/', Cur_Dir_Name[1] = '\0';
					}
					else {
						if (Cur_Dir_Name[strlen(Cur_Dir_Name) - 1] != '/')
							strcat(Cur_Dir_Name, "/");
						strcat(Cur_Dir_Name, dirlist[j].itemName);
					}

					return;
				}
				else {
					//�ҵ���Ŀ¼��������Ŀ¼��������
				}

			}

			i++;
		}

	}

	//û�ҵ�
	printf("cd %s : No such file or directory.\n", name);
	return;

}

void Vi(int parinoAddr, char name[], char buf[])	//ģ��һ����vi�������ı���nameΪ�ļ���
{
	//���ж��ļ��Ƿ��Ѵ��ڡ�������ڣ�������ļ����༭
	if (strlen(name) >= MAX_NAME_SIZE) {
		cout << "The filename is too long." << endl;
		return;
	}

	//��ջ�����
	memset(buf, 0, sizeof(buf));
	int maxlen = 0;	//���������󳤶�

					//��������ͬ���ļ����еĻ�����༭ģʽ��û�н��봴���ļ�ģʽ
	DirItem dirlist[16] = { 0 };	//��ʱĿ¼�嵥

									//�������ַȡ��inode
	Inode cur = { 0 }, fileInode = { 0 };
	fseek(fr, parinoAddr, SEEK_SET);
	fread(&cur, sizeof(Inode), 1, fr);

	//�ж��ļ�ģʽ��6Ϊowner��3Ϊgroup��0Ϊother
	int filemode;
	if (strcmp(Cur_User_Name, cur.i_uname) == 0)
		filemode = 6;
	else if (strcmp(Cur_User_Name, cur.i_gname) == 0)
		filemode = 3;
	else
		filemode = 0;

	int i = 0, j;
	int dno;
	int fileInodeAddr = -1;	//�ļ���inode��ַ
	bool isExist = false;	//�ļ��Ƿ��Ѵ���
	while (i<160) {
		//160��Ŀ¼��֮�ڣ�����ֱ����ֱ�ӿ�����
		dno = i / 16;	//�ڵڼ���ֱ�ӿ���

		if (cur.i_dirBlock[dno] == -1) {
			i += 16;
			continue;
		}
		fseek(fr, cur.i_dirBlock[dno], SEEK_SET);
		fread(dirlist, sizeof(dirlist), 1, fr);
		fflush(fr);

		//����ô��̿��е�����Ŀ¼��
		for (j = 0; j<16; j++) {
			if (strcmp(dirlist[j].itemName, name) == 0) {
				//������ȡ��inode���ж��Ƿ����ļ�
				fseek(fr, dirlist[j].inodeAddr, SEEK_SET);
				fread(&fileInode, sizeof(Inode), 1, fr);
				if (((fileInode.i_mode >> 9) & 1) == 0) {	//���ļ���������������ļ������༭	
					fileInodeAddr = dirlist[j].inodeAddr;
					isExist = true;
					goto label;
				}
			}
			i++;
		}
	}
label:

	//��ʼ��vi
	int cnt = 0;
	system("cls");	//����

	int winx, winy, curx, cury;

	HANDLE handle_out;                              //����һ�����  
	CONSOLE_SCREEN_BUFFER_INFO screen_info;         //���崰�ڻ�������Ϣ�ṹ��  
	COORD pos = { 0, 0 };                             //����һ������ṹ��

	if (isExist) {	//�ļ��Ѵ��ڣ�����༭ģʽ�������֮ǰ���ļ�����

					//Ȩ���жϡ��ж��ļ��Ƿ�ɶ�
		if (((fileInode.i_mode >> filemode >> 2) & 1) == 0) {
			//���ɶ�
			cout << "Permission denied." << endl;
			return;
		}

		//���ļ����ݶ�ȡ��������ʾ�ڣ�������
		i = 0;
		int sumlen = fileInode.i_size;	//�ļ�����
		int getlen = 0;	//ȡ�����ĳ���
		for (i = 0; i<10; i++) {
			char fileContent[1000] = { 0 };
			if (fileInode.i_dirBlock[i] == -1) {
				continue;
			}
			//����ȡ�����̿������
			fseek(fr, fileInode.i_dirBlock[i], SEEK_SET);
			fread(fileContent, superblock->s_BLOCK_SIZE, 1, fr);	//��ȡ��һ�����̿��С������
			fflush(fr);
			//����ַ���
			int curlen = 0;	//��ǰָ��
			while (curlen<superblock->s_BLOCK_SIZE) {
				if (getlen >= sumlen)	//ȫ��������
					break;
				printf("%c", fileContent[curlen]);	//�������Ļ 
				buf[cnt++] = fileContent[curlen];	//�����buf
				curlen++;
				getlen++;
			}
			if (getlen >= sumlen)
				break;
		}
		maxlen = sumlen;
	}

	//������֮��Ĺ��λ��
	handle_out = GetStdHandle(STD_OUTPUT_HANDLE);   //��ñ�׼����豸���  
	GetConsoleScreenBufferInfo(handle_out, &screen_info);   //��ȡ������Ϣ  
	winx = screen_info.srWindow.Right - screen_info.srWindow.Left + 1;
	winy = screen_info.srWindow.Bottom - screen_info.srWindow.Top + 1;
	curx = screen_info.dwCursorPosition.X;
	cury = screen_info.dwCursorPosition.Y;


	//����vi
	//����vi��ȡ�ļ�����

	int mode = 0;	//viģʽ��һ��ʼ������ģʽ
	unsigned char c;
	while (1) {
		if (mode == 0) {	//������ģʽ
			c = getch();

			if (c == 'i' || c == 'a') {	//����ģʽ
				if (c == 'a') {
					curx++;
					if (curx == winx) {
						curx = 0;
						cury++;

						/*
						if(cury>winy-2 || cury%(winy-1)==winy-2){
						//������һ�������·�ҳ
						if(cury%(winy-1)==winy-2)
						printf("\n");
						SetConsoleTextAttribute(handle_out, screen_info.wAttributes); // �ָ�ԭ��������
						int i;
						for(i=0;i<winx-1;i++)
						printf(" ");
						Gotoxy(handle_out,0,cury+1);
						printf(" - ����ģʽ - ");
						Gotoxy(handle_out,0,cury);
						}
						*/
					}
				}

				if (cury>winy - 2 || cury % (winy - 1) == winy - 2) {
					//������һ�������·�ҳ
					if (cury % (winy - 1) == winy - 2)
						printf("\n");
					SetConsoleTextAttribute(handle_out, screen_info.wAttributes); // �ָ�ԭ��������
					int i;
					for (i = 0; i<winx - 1; i++)
						printf(" ");
					Gotoxy(handle_out, 0, cury + 1);

					printf("-- INSERT --");
					Gotoxy(handle_out, 0, cury);
				}
				else {
					//��ʾ "����ģʽ"
					Gotoxy(handle_out, 0, winy - 1);
					SetConsoleTextAttribute(handle_out, screen_info.wAttributes); // �ָ�ԭ��������
					int i;
					for (i = 0; i<winx - 1; i++)
						printf(" ");
					Gotoxy(handle_out, 0, winy - 1);
					printf("-- INSERT --");
					Gotoxy(handle_out, curx, cury);
				}

				Gotoxy(handle_out, curx, cury);
				mode = 1;


			}
			else if (c == ':') {
				//system("color 09");//�����ı�Ϊ��ɫ
				if (cury - winy + 2>0)
					Gotoxy(handle_out, 0, cury + 1);
				else
					Gotoxy(handle_out, 0, winy - 1);
				_COORD pos;
				if (cury - winy + 2>0)
					pos.X = 0, pos.Y = cury + 1;
				else
					pos.X = 0, pos.Y = winy - 1;
				SetConsoleTextAttribute(handle_out, screen_info.wAttributes); // �ָ�ԭ��������
				int i;
				for (i = 0; i<winx - 1; i++)
					printf(" ");

				if (cury - winy + 2>0)
					Gotoxy(handle_out, 0, cury + 1);
				else
					Gotoxy(handle_out, 0, winy - 1);

				WORD att = BACKGROUND_RED | BACKGROUND_BLUE | FOREGROUND_INTENSITY; // �ı�����
				FillConsoleOutputAttribute(handle_out, att, winx, pos, NULL);	//����̨������ɫ 
				SetConsoleTextAttribute(handle_out, FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE | FOREGROUND_GREEN);	//�����ı���ɫ
				printf(":");

				char pc;
				int tcnt = 1;	//������ģʽ������ַ�����
				while (c = getch()) {
					if (c == '\r') {	//�س�
						break;
					}
					else if (c == '\b') {	//�˸񣬴�������ɾ��һ���ַ� 
											//SetConsoleTextAttribute(handle_out, screen_info.wAttributes); // �ָ�ԭ��������
						tcnt--;
						if (tcnt == 0)
							break;
						printf("\b");
						printf(" ");
						printf("\b");
						continue;
					}
					pc = c;
					printf("%c", pc);
					tcnt++;
				}
				if (pc == 'q') {
					buf[cnt] = '\0';
					//buf[maxlen] = '\0'; 
					SetConsoleTextAttribute(handle_out, screen_info.wAttributes); // �ָ�ԭ��������
					system("cls");
					break;	//vi >>>>>>>>>>>>>> �˳�����
				}
				else {
					if (cury - winy + 2>0)
						Gotoxy(handle_out, 0, cury + 1);
					else
						Gotoxy(handle_out, 0, winy - 1);
					SetConsoleTextAttribute(handle_out, screen_info.wAttributes); // �ָ�ԭ��������
					int i;
					for (i = 0; i<winx - 1; i++)
						printf(" ");

					if (cury - winy + 2>0)
						Gotoxy(handle_out, 0, cury + 1);
					else
						Gotoxy(handle_out, 0, winy - 1);
					SetConsoleTextAttribute(handle_out, FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE | FOREGROUND_GREEN);	//�����ı���ɫ
					FillConsoleOutputAttribute(handle_out, att, winx, pos, NULL);	//����̨������ɫ
					printf("--  Command Error --");
					//getch();
					SetConsoleTextAttribute(handle_out, screen_info.wAttributes); // �ָ�ԭ��������
					Gotoxy(handle_out, curx, cury);
				}
			}
			else if (c == 27) {	//ESC��������ģʽ����״̬��
				Gotoxy(handle_out, 0, winy - 1);
				SetConsoleTextAttribute(handle_out, screen_info.wAttributes); // �ָ�ԭ��������
				int i;
				for (i = 0; i<winx - 1; i++)
					printf(" ");
				Gotoxy(handle_out, curx, cury);

			}

		}
		else if (mode == 1) {	//����ģʽ

			Gotoxy(handle_out, winx / 4 * 3, winy - 1);
			int i = winx / 4 * 3;
			while (i<winx - 1) {
				printf(" ");
				i++;
			}
			if (cury>winy - 2)
				Gotoxy(handle_out, winx / 4 * 3, cury + 1);
			else
				Gotoxy(handle_out, winx / 4 * 3, winy - 1);
			printf("[Row: %d, Column: %d]", curx == -1 ? 0 : curx, cury);
			Gotoxy(handle_out, curx, cury);

			c = getch();
			if (c == 27) {	// ESC����������ģʽ
				mode = 0;
				//��״̬��
				Gotoxy(handle_out, 0, winy - 1);
				SetConsoleTextAttribute(handle_out, screen_info.wAttributes); // �ָ�ԭ��������
				int i;
				for (i = 0; i<winx - 1; i++)
					printf(" ");
				continue;
			}
			else if (c == '\b') {	//�˸�ɾ��һ���ַ�
				if (cnt == 0)	//�Ѿ��˵��ʼ
					continue;
				printf("\b");
				printf(" ");
				printf("\b");
				curx--;
				cnt--;	//ɾ���ַ�
				if (buf[cnt] == '\n') {
					//Ҫɾ��������ַ��ǻس������ص���һ��
					if (cury != 0)
						cury--;
					int k;
					curx = 0;
					for (k = cnt - 1; buf[k] != '\n' && k >= 0; k--)
						curx++;
					Gotoxy(handle_out, curx, cury);
					printf(" ");
					Gotoxy(handle_out, curx, cury);
					if (cury - winy + 2 >= 0) {	//��ҳʱ
						Gotoxy(handle_out, curx, 0);
						Gotoxy(handle_out, curx, cury + 1);
						SetConsoleTextAttribute(handle_out, screen_info.wAttributes); // �ָ�ԭ��������
						int i;
						for (i = 0; i<winx - 1; i++)
							printf(" ");
						Gotoxy(handle_out, 0, cury + 1);
						printf("-- INSERT --");

					}
					Gotoxy(handle_out, curx, cury);

				}
				else
					buf[cnt] = ' ';
				continue;
			}
			else if (c == 224) {	//�ж��Ƿ��Ǽ�ͷ
				c = getch();
				if (c == 75) {	//���ͷ
					if (cnt != 0) {
						cnt--;
						curx--;
						if (buf[cnt] == '\n') {
							//��һ���ַ��ǻس�
							if (cury != 0)
								cury--;
							int k;
							curx = 0;
							for (k = cnt - 1; buf[k] != '\n' && k >= 0; k--)
								curx++;
						}
						Gotoxy(handle_out, curx, cury);
					}
				}
				else if (c == 77) {	//�Ҽ�ͷ
					cnt++;
					if (cnt>maxlen)
						maxlen = cnt;
					curx++;
					if (curx == winx) {
						curx = 0;
						cury++;

						if (cury>winy - 2 || cury % (winy - 1) == winy - 2) {
							//������һ�������·�ҳ
							if (cury % (winy - 1) == winy - 2)
								printf("\n");
							SetConsoleTextAttribute(handle_out, screen_info.wAttributes); // �ָ�ԭ��������
							int i;
							for (i = 0; i<winx - 1; i++)
								printf(" ");
							Gotoxy(handle_out, 0, cury + 1);
							printf("-- INSERT --");
							Gotoxy(handle_out, 0, cury);
						}

					}
					Gotoxy(handle_out, curx, cury);
				}
				continue;
			}
			if (c == '\r') {	//�����س�
				printf("\n");
				curx = 0;
				cury++;

				if (cury>winy - 2 || cury % (winy - 1) == winy - 2) {
					//������һ�������·�ҳ
					if (cury % (winy - 1) == winy - 2)
						printf("\n");
					SetConsoleTextAttribute(handle_out, screen_info.wAttributes); // �ָ�ԭ��������
					int i;
					for (i = 0; i<winx - 1; i++)
						printf(" ");
					Gotoxy(handle_out, 0, cury + 1);
					printf("-- INSERT --");
					Gotoxy(handle_out, 0, cury);
				}

				buf[cnt++] = '\n';
				if (cnt>maxlen)
					maxlen = cnt;
				continue;
			}
			else {
				printf("%c", c);
			}
			//�ƶ����
			curx++;
			if (curx == winx) {
				curx = 0;
				cury++;

				if (cury>winy - 2 || cury % (winy - 1) == winy - 2) {
					//������һ�������·�ҳ
					if (cury % (winy - 1) == winy - 2)
						printf("\n");
					SetConsoleTextAttribute(handle_out, screen_info.wAttributes); // �ָ�ԭ��������
					int i;
					for (i = 0; i<winx - 1; i++)
						printf(" ");
					Gotoxy(handle_out, 0, cury + 1);
					printf("-- INSERT --");
					Gotoxy(handle_out, 0, cury);
				}

				buf[cnt++] = '\n';
				if (cnt>maxlen)
					maxlen = cnt;
				if (cury == winy) {
					printf("\n");
				}
			}
			//��¼�ַ� 
			buf[cnt++] = c;
			if (cnt>maxlen)
				maxlen = cnt;
		}
		else {	//����ģʽ
		}
	}
	if (isExist) {	//����Ǳ༭ģʽ
					//��buf����д���ļ��Ĵ��̿�

		if (((fileInode.i_mode >> filemode >> 1) & 1) == 1) {	//��д
			WriteFile(fileInode, fileInodeAddr, buf);
		}
		else {	//����д
			cout << "Permission denied." << endl;
		}

	}
	else {	//�Ǵ����ļ�ģʽ
		if (((cur.i_mode >> filemode >> 1) & 1) == 1) {
			//��д�����Դ����ļ�
			Create(parinoAddr, name, buf);	//�����ļ�
		}
		else {
			cout << "Permission denied." << endl;
			return;
		}
	}
}

// ����
void Clear()
{
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
	system("cls");
#else
	system("clear");
#endif
}

void Pwd()
{
	printf("%s\n", Cur_Dir_Name);
}

void Cat(int parinoAddr, char name[])
{
	//��ջ�����
	char buf[FILE_BUFFER] = { 0 };

	//��������ͬ���ļ�
	DirItem dirlist[16] = { 0 };	//��ʱĿ¼�嵥

									//�������ַȡ��inode
	Inode cur = { 0 }, fileInode = { 0 };
	fseek(fr, parinoAddr, SEEK_SET);
	fread(&cur, sizeof(Inode), 1, fr);

	//�ж��ļ�ģʽ��6Ϊowner��3Ϊgroup��0Ϊother
	int filemode;
	if (strcmp(Cur_User_Name, cur.i_uname) == 0)
		filemode = 6;
	else if (strcmp(Cur_User_Name, cur.i_gname) == 0)
		filemode = 3;
	else
		filemode = 0;

	size_t i = 0, j;
	int dno;
	int fileInodeAddr = -1;	//�ļ���inode��ַ
	bool isExist = false;	//�ļ��Ƿ��Ѵ���
	while (i<160) {
		//160��Ŀ¼��֮�ڣ�����ֱ����ֱ�ӿ�����
		dno = i / 16;	//�ڵڼ���ֱ�ӿ���

		if (cur.i_dirBlock[dno] == -1) {
			i += 16;
			continue;
		}
		fseek(fr, cur.i_dirBlock[dno], SEEK_SET);
		fread(dirlist, sizeof(dirlist), 1, fr);
		fflush(fr);

		//����ô��̿��е�����Ŀ¼��
		for (j = 0; j<16; j++) {
			if (strcmp(dirlist[j].itemName, name) == 0) {
				//������ȡ��inode���ж��Ƿ����ļ�
				fseek(fr, dirlist[j].inodeAddr, SEEK_SET);
				fread(&fileInode, sizeof(Inode), 1, fr);
				if (((fileInode.i_mode >> 9) & 1) == 0) {	//���ļ���������������ļ������༭	
					fileInodeAddr = dirlist[j].inodeAddr;
					isExist = true;
					goto label;
				}
			}
			i++;
		}
	}
label:

	int cnt = 0;

	if (isExist) {	//�ļ��Ѵ��ڣ�����ļ�����

					//Ȩ���жϡ��ж��ļ��Ƿ�ɶ�
		if (((fileInode.i_mode >> filemode >> 2) & 1) == 0) {
			//���ɶ�
			cout << "Permission denied." << endl;
			return;
		}

		//���ļ����ݶ�ȡ��������ʾ�ڣ�������
		i = 0;
		int sumlen = fileInode.i_size;	//�ļ�����
		int getlen = 0;	//ȡ�����ĳ���
		for (i = 0; i<10; i++) {
			char fileContent[1000] = { 0 };
			if (fileInode.i_dirBlock[i] == -1) {
				continue;
			}
			//����ȡ�����̿������
			fseek(fr, fileInode.i_dirBlock[i], SEEK_SET);
			fread(fileContent, superblock->s_BLOCK_SIZE, 1, fr);	//��ȡ��һ�����̿��С������
			fflush(fr);
			//����ַ���
			int curlen = 0;	//��ǰָ��
			while (curlen<superblock->s_BLOCK_SIZE) {
				if (getlen >= sumlen)	//ȫ��������
					break;
				printf("%c", fileContent[curlen]);	//�������Ļ 
				buf[cnt++] = fileContent[curlen];	//�����buf
				curlen++;
				getlen++;
			}
			if (getlen >= sumlen)
				break;
		}
	}
	else {
		printf("cat %s : No such file\n", name);
	}
}

// -------------------- ����ʵ�ֵĶ��⺯�� -----------------------

void Rmall(int parinoAddr)	//ɾ���ýڵ��������ļ���Ŀ¼
{
	//�������ַȡ��inode
	Inode cur = { 0 };
	fseek(fr, parinoAddr, SEEK_SET);
	fread(&cur, sizeof(Inode), 1, fr);

	//ȡ��Ŀ¼����
	int cnt = cur.i_cnt;
	if (cnt <= 2) {
		BlockFree(cur.i_dirBlock[0]);
		InodeFree(parinoAddr);
		return;
	}

	//����ȡ�����̿�
	int i = 0;
	while (i<160) {	//С��160
		DirItem dirlist[16] = { 0 };

		if (cur.i_dirBlock[i / 16] == -1) {
			i += 16;
			continue;
		}
		//ȡ�����̿�
		int parblockAddr = cur.i_dirBlock[i / 16];
		fseek(fr, parblockAddr, SEEK_SET);
		fread(&dirlist, sizeof(dirlist), 1, fr);

		//�Ӵ��̿�������ȡ��Ŀ¼��ݹ�ɾ��
		int j;
		bool f = false;
		for (j = 0; j<16; j++) {
			//Inode tmp;

			if (!(strcmp(dirlist[j].itemName, ".") == 0 ||
				strcmp(dirlist[j].itemName, "..") == 0 ||
				strcmp(dirlist[j].itemName, "") == 0)) {
				f = true;
				Rmall(dirlist[j].inodeAddr);	//�ݹ�ɾ��
			}

			cnt = cur.i_cnt;
			i++;
		}

		//�ô��̿��ѿգ�����
		if (f)
			BlockFree(parblockAddr);

	}
	//��inode�ѿգ�����
	InodeFree(parinoAddr);
	return;

}


void Gotoxy(HANDLE hOut, int x, int y)	//�ƶ���굽ָ��λ��
{
	COORD pos;
	pos.X = x;             //������
	pos.Y = y;            //������
	SetConsoleCursorPosition(hOut, pos);
}


void WriteFile(Inode fileInode, int fileInodeAddr, char buf[])	//��buf����д���ļ��Ĵ��̿�
{
	//��buf����д�ش��̿� 
	int k;
	int len = strlen(buf);	//�ļ����ȣ���λΪ�ֽ�
	for (k = 0; k<len; k += superblock->s_BLOCK_SIZE) {	//���10�Σ�10�����̿죬�����5K
														//�������inode�Ĵ��̿飬�ӿ���̨��ȡ����
		int curblockAddr;
		if (fileInode.i_dirBlock[k / superblock->s_BLOCK_SIZE] == -1) {
			//ȱ�ٴ��̿飬����һ��
			curblockAddr = BockAlloc();
			if (curblockAddr == -1) {
				cout << "Block allocation failure." << endl;
				return;
			}
			fileInode.i_dirBlock[k / superblock->s_BLOCK_SIZE] = curblockAddr;
		}
		else {
			curblockAddr = fileInode.i_dirBlock[k / superblock->s_BLOCK_SIZE];
		}
		//д�뵽��ǰĿ¼�Ĵ��̿�
		fseek(fw, curblockAddr, SEEK_SET);
		fwrite(buf + k, superblock->s_BLOCK_SIZE, 1, fw);
		fflush(fw);
	}
	//���¸��ļ���С
	fileInode.i_size = len;
	fileInode.i_mtime = time(NULL);
	fseek(fw, fileInodeAddr, SEEK_SET);
	fwrite(&fileInode, sizeof(Inode), 1, fw);
	fflush(fw);
}

void InputUsername(char username[])	//�����û���
{
	printf("username: ");
	scanf("%s", username);	//�û���
}

void inputPassword(char passwd[])	//��������
{
	int plen = 0;
	char c;
	fflush(stdin);	//��ջ�����
	printf("password: ");
	while (c = getch()) {
		if (c == '\r') {	//����س�������ȷ��
			passwd[plen] = '\0';
			fflush(stdin);	//�建����
			printf("\n");
			break;
		}
		else if (c == '\b') {	//�˸�ɾ��һ���ַ�
			if (plen != 0) {	//û��ɾ��ͷ
				plen--;
			}
		}
		else {	//�����ַ�
			passwd[plen++] = c;
		}
	}
}


bool Check(char username[], char passwd[])	//�˶��û���������
{
	int passwd_Inode_Addr = -1;	//�û��ļ�inode��ַ
	int shadow_Inode_Addr = -1;	//�û������ļ�inode��ַ
	Inode passwd_Inode = { 0 };		//�û��ļ���inode
	Inode shadow_Inode = { 0 };		//�û������ļ���inode

	Inode cur_dir_inode = { 0 };	//��ǰĿ¼��inode
	size_t i, j;
	DirItem dirlist[16] = { 0 };	//��ʱĿ¼

	Cd(Cur_Dir_Addr, "etc");	//���������ļ�Ŀ¼

								//�ҵ�passwd��shadow�ļ�inode��ַ����ȡ��
								//ȡ����ǰĿ¼��inode
	fseek(fr, Cur_Dir_Addr, SEEK_SET);
	fread(&cur_dir_inode, sizeof(Inode), 1, fr);
	//����ȡ�����̿飬����passwd�ļ���inode��ַ����shadow�ļ���inode��ַ
	for (i = 0; i<10; i++) {
		if (cur_dir_inode.i_dirBlock[i] == -1) {
			continue;
		}
		//����ȡ�����̿�
		fseek(fr, cur_dir_inode.i_dirBlock[i], SEEK_SET);
		fread(&dirlist, sizeof(dirlist), 1, fr);

		for (j = 0; j<16; j++) {	//����Ŀ¼��
			if (strcmp(dirlist[j].itemName, "passwd") == 0 ||	//�ҵ�passwd����shadow��Ŀ
				strcmp(dirlist[j].itemName, "shadow") == 0) {
				Inode tmp = { 0 };	//��ʱinode
									//ȡ��inode���ж��Ƿ����ļ�
				fseek(fr, dirlist[j].inodeAddr, SEEK_SET);
				fread(&tmp, sizeof(Inode), 1, fr);

				if (((tmp.i_mode >> 9) & 1) == 0) {
					//���ļ�
					//�б���passwd�ļ�����shadow�ļ�
					if (strcmp(dirlist[j].itemName, "passwd") == 0) {
						passwd_Inode_Addr = dirlist[j].inodeAddr;
						passwd_Inode = tmp;
					}
					else if (strcmp(dirlist[j].itemName, "shadow") == 0) {
						shadow_Inode_Addr = dirlist[j].inodeAddr;
						shadow_Inode = tmp;
					}
				}
			}
		}
		if (passwd_Inode_Addr != -1 && shadow_Inode_Addr != -1)	//���ҵ���
			break;
	}

	//����passwd�ļ������Ƿ�����û�username
	char buf[1000] = { 0 };	//���1K���ݴ�passwd���ļ�����
	char buf2[600] = { 0 };		//�ݴ���̿�����
	j = 0;	//���̿�ָ��
			//ȡ��passwd�ļ�����
	for (i = 0; i<passwd_Inode.i_size; i++) {
		if (i%superblock->s_BLOCK_SIZE == 0) {	//������
												//���µĴ��̿�
			fseek(fr, passwd_Inode.i_dirBlock[i / superblock->s_BLOCK_SIZE], SEEK_SET);
			fread(&buf2, superblock->s_BLOCK_SIZE, 1, fr);
			j = 0;
		}
		buf[i] = buf2[j++];
	}
	buf[i] = '\0';
	if (strstr(buf, username) == NULL) {
		//û�ҵ����û�
		cout << "User does not exist." << endl;
		Cd(Cur_Dir_Addr, "..");	//�ص���Ŀ¼
		return false;
	}

	//������ڣ��鿴shadow�ļ���ȡ�����룬�˶�passwd�Ƿ���ȷ
	//ȡ��shadow�ļ�����
	j = 0;
	for (i = 0; i<shadow_Inode.i_size; i++) {
		if (i%superblock->s_BLOCK_SIZE == 0) {	//������������̿�
												//���µĴ��̿�
			fseek(fr, shadow_Inode.i_dirBlock[i / superblock->s_BLOCK_SIZE], SEEK_SET);
			fread(&buf2, superblock->s_BLOCK_SIZE, 1, fr);
			j = 0;
		}
		buf[i] = buf2[j++];
	}
	buf[i] = '\0';

	char *p;	//�ַ�ָ��
	if ((p = strstr(buf, username)) == NULL) {
		//û�ҵ����û�
		cout << "The user does not exist in the shadow file." << endl;
		Cd(Cur_Dir_Addr, "..");	//�ص���Ŀ¼
		return false;
	}
	//�ҵ����û���ȡ������
	while ((*p) != ':') {
		p++;
	}
	p++;
	j = 0;
	while ((*p) != '\n') {
		buf2[j++] = *p;
		p++;
	}
	buf2[j] = '\0';

	//�˶�����
	if (strcmp(buf2, passwd) == 0) {	//������ȷ����½
		strcpy(Cur_User_Name, username);
		if (strcmp(username, "root") == 0)
			strcpy(Cur_Group_Name, "root");	//��ǰ��½�û�����
		else
			strcpy(Cur_Group_Name, "user");	//��ǰ��½�û�����
		Cd(Cur_Dir_Addr, "..");
		Cd(Cur_Dir_Addr, "home"); \
			Cd(Cur_Dir_Addr, username);	//���뵽�û�Ŀ¼
		strcpy(Cur_User_Dir_Name, Cur_Dir_Name);	//���Ƶ�ǰ��½�û�Ŀ¼��
		return true;
	}
	else {

		cout << "Password is not correct." << endl;
		Cd(Cur_Dir_Addr, "..");	//�ص���Ŀ¼
		return false;
	}
}

void GotoRoot()	//�ص���Ŀ¼
{
	memset(Cur_User_Name, 0, sizeof(Cur_User_Name));		//��յ�ǰ�û���
	memset(Cur_User_Dir_Name, 0, sizeof(Cur_User_Dir_Name));	//��յ�ǰ�û�Ŀ¼
	Cur_Dir_Addr = Root_Dir_Addr;	//��ǰ�û�Ŀ¼��ַ��Ϊ��Ŀ¼��ַ
	strcpy(Cur_Dir_Name, "/");		//��ǰĿ¼��Ϊ"/"
}


// ----------- ���������ϵͳ��Ϣ ----------------


void Cmd(char inputLine[])	//�������������
{
	char inputCommand[100] = { 0 };
	char inputParameter[100] = { 0 };
	char inputParameter2[100] = { 0 };
	char buf[FILE_BUFFER] = { 0 };	//���10K
	int tmp = 0;
	int i = 0;
	sscanf(inputLine, "%s%s%s", inputCommand, inputParameter, inputParameter2);

	if (isLogin == false) {
		// ��δ��¼ֻ��ʹ��login��ע�ᡢHelp��δ��¼��Help����exit��clear
		if (strcmp(inputCommand, "login") == 0) {
			if (strcmp(inputParameter, "--help") == 0) {
				Login_help();
			}
			else {
				Login(inputParameter);
			}
		}
		else if (strcmp(inputCommand, "help") == 0)
			Help_NotLogin();
		else if (strcmp(inputCommand, "clear") == 0)
			Clear();
		else if (strcmp(inputCommand, "exit") == 0)
			exit(0);
		else if (strcmp(inputCommand, "") == 0)
			return;
		else {
			printf("%s: command not found...\n", inputCommand);
			cout << "If you need help, please input 'help'." << endl;
		}
	}
	else {
		// �Ѿ���¼������ʹ��ȫ������
		if (strcmp(inputCommand, "login") == 0) {
			if (strcmp(inputParameter, "--help") == 0) {
				Login_help();
			}
			else {
				Login(inputParameter);
			}
		}
		else if (strcmp(inputCommand, "logout") == 0) {
			if (strcmp(inputCommand, "--help") == 0) {
				Logout_help();
			}
			else {
				Logout();
			}
		}
		else if (strcmp(inputCommand, "useradd") == 0) {
			if (strcmp(inputParameter, "") == 0) {
				CommandError(inputCommand);
			}
			else if (strcmp(inputParameter, "--help") == 0) {
				Useradd_help();
			}
			else {
				Useradd(inputParameter);
			}
		}
		else if (strcmp(inputCommand, "userdel") == 0) {
			if (strcmp(inputParameter, "") == 0) {
				CommandError(inputCommand);
			}
			else if (strcmp(inputParameter, "--help") == 0) {
				Userdel_help();
			}
			else {
				Userdel(inputParameter);
			}
		}
		else if (strcmp(inputCommand, "cat") == 0) {
			if (strcmp(inputParameter, "") == 0) {
				CommandError(inputCommand);
			}
			else if (strcmp(inputParameter, "--help") == 0) {
				Cat_help();
			}
			else {
				Cat(Cur_Dir_Addr, inputParameter);
			}
		}
		else if (strcmp(inputCommand, "touch") == 0) {
			if (strcmp(inputParameter, "") == 0) {
				CommandError(inputCommand);
			}
			else if (strcmp(inputParameter, "--help") == 0) {
				Touch_help();
			}
			else {
				Touch(Cur_Dir_Addr, inputParameter, buf);
			}
		}
		else if (strcmp(inputCommand, "rm") == 0) {
			if (strcmp(inputParameter, "") == 0) {
				CommandError(inputCommand);
			}
			else if (strcmp(inputParameter, "--help") == 0) {
				Rm_help();
			}
			else {
				Rm(Cur_Dir_Addr, inputParameter);
			}
		}
		else if (strcmp(inputCommand, "vi") == 0) {
			if (strcmp(inputParameter, "") == 0) {
				CommandError(inputCommand);
			}
			else if (strcmp(inputParameter, "--help") == 0) {
				Vi_help();
			}
			else {
				Vi(Cur_Dir_Addr, inputParameter, buf);
			}
		}
		else if (strcmp(inputCommand, "chmod") == 0) {
			if (strcmp(inputParameter, "--help") == 0) {
				Chmod_help();
			}
			else if (strcmp(inputParameter, "") == 0 || strcmp(inputParameter2, "") == 0) {
				CommandError(inputCommand);
			}
			else {
				tmp = 0;
				for (size_t i = 0; inputParameter2[i]; i++)
					tmp = tmp * 8 + inputParameter2[i] - '0';
				Chmod(Cur_Dir_Addr, inputParameter, tmp);
			}
		}
		else if (strcmp(inputCommand, "ls") == 0) {
			if (strcmp(inputParameter, "") == 0) {
				Ls(Cur_Dir_Addr);
			}
			else if (strcmp(inputParameter, "--help") == 0) {
				Ls_help();
			}
			else {
				CommandError(inputCommand);
			}
		}
		else if (strcmp(inputCommand, "cd") == 0) {
			if (strcmp(inputParameter, "--help") == 0) {
				Cd_help();
			}
			else {
				Cd(Cur_Dir_Addr, inputParameter);
			}
		}
		else if (strcmp(inputCommand, "pwd") == 0) {
			if (strcmp(inputParameter, "") == 0) {
				Pwd();
			}
			else if (strcmp(inputParameter, "--help") == 0) {
				Pwd_help();
			}
			else {
				CommandError(inputCommand);
			}
		}
		else if (strcmp(inputCommand, "mkdir") == 0) {
			if (strcmp(inputParameter, "") == 0) {
				CommandError(inputCommand);
			}
			else if (strcmp(inputParameter, "--help") == 0) {
				Mkdir_help();
			}
			else {
				Mkdir(Cur_Dir_Addr, inputParameter);
			}
		}
		else if (strcmp(inputCommand, "rmdir") == 0) {
			if (strcmp(inputParameter, "") == 0 || strcmp(inputParameter, ".") == 0 || strcmp(inputParameter, "..") == 0) {
				CommandError(inputCommand);
			}
			else if (strcmp(inputParameter, "--help") == 0) {
				Rmdir_help();
			}
			else {
				Rmdir(Cur_Dir_Addr, inputParameter);
			}
		}
		else if (strcmp(inputCommand, "format") == 0) {
			if (strcmp(inputParameter, "") == 0) {
				if (strcmp(Cur_User_Name, "root") != 0) {
					cout << "Permission denied." << endl;
					return;
				}
				else {
					Ready();
					Logout();
				}
			}
			else if (strcmp(inputParameter, "--help") == 0) {
				Rmdir_help();
			}
			else {
				CommandError(inputCommand);
			}
		}
		else if (strcmp(inputCommand, "super") == 0) {
			if (strcmp(inputParameter, "") == 0) {
				PrintSuperBlock();
			}
			else if (strcmp(inputParameter, "--help") == 0) {
				Super_help();
			}
			else {
				CommandError(inputCommand);
			}
		}
		else if (strcmp(inputCommand, "inode") == 0) {
			if (strcmp(inputParameter, "") == 0) {
				PrintInodeBitmap();
			}
			else if (strcmp(inputParameter, "--help") == 0) {
				Inode_help();
			}
			else {
				CommandError(inputCommand);
			}
		}
		else if (strcmp(inputCommand, "block") == 0) {
			if (strcmp(inputParameter, "") == 0) {
				PrintBlockBitmap(superblock->s_BLOCK_NUM);
			}
			else if (strcmp(inputParameter, "--help") == 0) {
				Block_help();
			}
			else {
				tmp = 0;
				if ('0' <= inputParameter[0] && inputParameter[0] <= '9') {
					for (size_t i = 0; inputParameter[i]; i++)
						tmp = tmp * 10 + inputParameter[i] - '0';
					PrintBlockBitmap(tmp);
				}
				else
					PrintBlockBitmap(superblock->s_BLOCK_NUM);
			}
		}
		else if (strcmp(inputCommand, "help") == 0)
			Help();
		else if (strcmp(inputCommand, "version") == 0)
			Version();
		else if (strcmp(inputCommand, "clear") == 0)
			Clear();
		else if (strcmp(inputCommand, "exit") == 0)
			exit(0);
		else if (strcmp(inputCommand, "") == 0)
			return;
		else {
			printf("%s: command not found...\n", inputCommand);
			cout << "If you need help, please input 'help'." << endl;
		}
	}
}

void Help()
{
	Command_help();
	Help_help();
	Version_help();

	Useradd_help();
	Userdel_help();

	Login_help();
	Logout_help();

	Cat_help();
	Touch_help();
	Rm_help();
	Vi_help();
	Chmod_help();

	Ls_help();
	Cd_help();
	Pwd_help();
	Mkdir_help();
	Rmdir_help();

	Clear_help();
	Exit_help();

	Super_help();
	Inode_help();
	Block_help();
}
void Help_NotLogin() {
	Command_help();
	Help_help();
	Login_help();
	Clear_help();
	Exit_help();
}

void PrintSuperBlock()		//��ӡ��������Ϣ
{
	printf("\n");
	printf("Free inode / Total Inode ��%d / %d\n", superblock->s_free_INODE_NUM, superblock->s_INODE_NUM);
	printf("Free block / Total block ��%d / %d\n", superblock->s_free_BLOCK_NUM, superblock->s_BLOCK_NUM);
	printf("File system block size��%d B��Each inode %d B��true size��%d B��\n", superblock->s_BLOCK_SIZE, superblock->s_INODE_SIZE, sizeof(Inode));
	printf("\tThe number of block per disk block group��%d\n", superblock->s_blocks_per_group);
	printf("\tSuperblock %d B��true size��%d B��\n", superblock->s_BLOCK_SIZE, superblock->s_SUPERBLOCK_SIZE);
	printf("Disk distribution��\n");
	printf("\tSuperblock start position��%d B\n", superblock->s_Superblock_StartAddr);
	printf("\tInode bitmap start position��%d B\n", superblock->s_InodeBitmap_StartAddr);
	printf("\tBlock bitmap start position��%d B\n", superblock->s_BlockBitmap_StartAddr);
	printf("\tInode start position��%d B\n", superblock->s_Inode_StartAddr);
	printf("\tBlock start position��%d B\n", superblock->s_Block_StartAddr);
	printf("\n");

	return;
}

void PrintInodeBitmap()	//��ӡinodeʹ�����
{
	printf("\n");
	printf("Inode use table��[uesd:%d %d/%d]\n", superblock->s_INODE_NUM - superblock->s_free_INODE_NUM, superblock->s_free_INODE_NUM, superblock->s_INODE_NUM);
	int i;
	i = 0;
	printf("0 ");
	while (i<superblock->s_INODE_NUM) {
		if (inode_bitmap[i])
			printf("*");
		else
			printf(".");
		i++;
		if (i != 0 && i % 32 == 0) {
			printf("\n");
			if (i != superblock->s_INODE_NUM)
				printf("%d ", i / 32);
		}
	}
	printf("\n");
	printf("\n");
	return;
}

void PrintBlockBitmap(int num)	//��ӡblockʹ�����
{
	printf("\n");
	printf("Block (disk block) use table��[used:%d %d/%d]\n", superblock->s_BLOCK_NUM - superblock->s_free_BLOCK_NUM, superblock->s_free_BLOCK_NUM, superblock->s_BLOCK_NUM);
	int i;
	i = 0;
	printf("0\t");
	while (i<num) {
		if (block_bitmap[i])
			printf("*");
		else
			printf(".");
		i++;
		if (i != 0 && i % 32 == 0) {
			printf("\n");
			if (num == superblock->s_BLOCK_NUM)
				//getchar();
				if (i != superblock->s_BLOCK_NUM)
					printf("%d\t", i / 32);
		}
	}
	printf("\n");
	printf("\n");
	return;
}

void Command_help() {
	cout << "Command format : 'command [Necessary parameter] ([Unnecessary parameter])'" << endl;
}

void Help_help() {
	cout << "help : Get help" << endl;
}

void Version()
{
	cout << MFS_NAME << ", version " << MFS_VERSION << endl;
}
void Version_help() {
	cout << "version : Print the MFS name and version" << endl;
}
void Useradd_help() {
	cout << "useradd [username] : Add user" << endl;
}
void Userdel_help() {
	cout << "userdel [username] : Delete users" << endl;
}
void Login_help() {
	cout << "login ([username]): Login MFS" << endl;
}
void Logout_help() {
	cout << "logout : Exit the current user" << endl;
}
void Cat_help() {
	cout << "cat [filename] : Output file content" << endl;
}
void Rm_help() {
	cout << "rm [filename] : Remove file" << endl;
}
void Mkdir_help() {
	cout << "mkdir [directoryName] : Create subdirectory" << endl;
}
void Rmdir_help() {
	cout << "rmdir [directoryName] : Delete subdirectory" << endl;
}
void Cd_help() {
	cout << "cd [directoryName] : Change current directory" << endl;
}
void Ls_help() {
	cout << "ls : List the file directory" << endl;
}
void Pwd_help() {
	cout << "pwd : List the current directory name" << endl;
}
void Chmod_help() {
	cout << "chmod [filename] [permissions] : Change the file permissions" << endl;
}
void Clear_help() {
	cout << "clear : Clear the terminal" << endl;
}
void Exit_help() {
	cout << "exit : Exit the MFS" << endl;
}
void Touch_help() {
	cout << "touch [filename] : Create a new empty file" << endl;
}
void Vi_help() {
	cout << "vi [filename] : Remove file" << endl;
}
void Super_help() {
	cout << "super : Check the state of super block" << endl;
}
void Inode_help() {
	cout << "inode : Check the inode bitmap" << endl;
}
void Block_help() {
	cout << "block ([block number]): Check the block bitmap" << endl;
}
void CommandError(char command[])
{
	printf("%s: missing operand\n", command);
	printf("Try '%s --help' for more information.\n", command);
}