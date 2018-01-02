# 已知Bug

## 【2018-01-01】root用户删除非管理员用户问题

### 表现

root用户登录，进入xx用户（非管理员）目录下，删除xx用户，

导致xx用户目录删除了，无法去到其他目录。

```
[root@HUIHUT /home/xx]$ userdel xx
User has deleted.
[root@HUIHUT /home/xx]$ ls
Permission denied.
[root@HUIHUT /home/xx]$ cd ..
cd ..: No such file or directory.
[root@HUIHUT /home/xx]$ cd
[root@HUIHUT ~ome/root]$ ls
drwxr-xr-x      2       root    root    512 B   2017.12.22 23:55:50  .
drwxr-xr-x      3       root    root    512 B   2017.12.22 23:55:50  ..
[root@HUIHUT ~ome/root]$ cd ..
[root@HUIHUT /home/xx/home]$ ls
drwxr-xr-x      3       root    root    512 B   2017.12.22 23:55:50  .
drwxr-xr-x      3       root    root    512 B   2017.12.22 23:55:48  ..
drwxr-xr-x      2       root    root    512 B   2017.12.22 23:55:50  root
[root@HUIHUT /home/xx/home]$ cd ..
[root@HUIHUT /home/xx]$ ls
drwxr-xr-x      3       root    root    512 B   2017.12.22 23:55:48  .
drwxr-xr-x      3       root    root    512 B   2017.12.22 23:55:50  home
drwxr-xr-x      5       root    root    512 B   2017.12.22 23:55:50  etc
```

### 解决（未解决）

#### 【2018-01-02】

通过先返回根目录再删除xx用户解决部分问题，仍有当前目录名错乱的问题。

```
[root@HUIHUT /home/xx]$ userdel xx
Permission denied.
User has deleted.
[root@HUIHUT /]$ ls
drwxr-xr-x 3 root root  512 B   2018.1.2 12:09:43  .
drwxr-xr-x 3 root root  512 B   2018.1.2 12:09:49  home
drwxr-xr-x 5 root root  512 B   2018.1.2 12:09:49  etc
[root@HUIHUT /]$ cd home
[root@HUIHUT /home]$ ls
drwxr-xr-x 3 root root  512 B   2018.1.2 12:09:49  .
drwxr-xr-x 3 root root  512 B   2018.1.2 12:09:43  ..
drwxr-xr-x 2 root root  512 B   2018.1.2 12:09:49  root
[root@HUIHUT /home]$ cd root
[root@HUIHUT ~e/root]$ pwd
/home/root
```

## 【2018-01-01】root用户在非管理员用户目录下touch文件问题

### 表现

root用户，在xx（非管理员）用户目录下，touch文件，会有`Permission denied.`，mkdir创建目录则可以创建。

## 【2018-01-01】不可第二次登录，提示找不到目录

### 表现

不可第二次登录，提示找不到目录，当前只能删除sys文件重装。

```
-----------------------------------------
Wellcome to MFS(Meng-File-System)
If you need help, please type 'help'.
-----------------------------------------
$ ls
ls: command not found...
If you need help, please type 'help'.
$ login
username: root
password:
cd etc : No such file or directory.
User does not exist.
cd .. : No such file or directory.
$
```

## 【2018-01-01】vi编辑器按：键会闪退