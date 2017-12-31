# 已知Bug

## 【2018-01-01】root用户删除非管理员用户问题

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

## 【2018-01-01】root用户在非管理员用户目录下touch文件问题

root用户，在xx（非管理员）用户目录下，touch文件，会有`Permission denied.`，mkdir创建目录则可以创建。

## 【2018-01-01】不可第二次登录，提示找不到目录

## 【2018-01-01】vi编辑器按：键会闪退