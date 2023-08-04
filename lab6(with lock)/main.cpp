#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <cstring>

using namespace std;

struct my_file{
    int fd=-1;
    int open_mode=0, write_mode=0;
    char path_name[256]="";
}cur_file;

static struct flock lock_it; /* 用于加锁的flock对象 */
static struct flock unlock_it;     /* 用于解锁的flock对象 */
static int lock_fd = -1;

void my_lock_init(char *path_name, short int lk_type);
void my_lock_wait();
void my_lock_release();

void flock_wait(int opr){
    /*if(flock(cur_file.fd, opr)<0){
        cout<<"当前文件正被其他进程占用，请稍候..."<<endl;
    }*///为什么没用呢。。。
    while(flock(cur_file.fd, opr)<0);
    cout<<"已获取锁"<<endl;
}

void flock_rls(){
    flock(cur_file.fd, LOCK_UN);
    cout<<"已释放锁"<<endl;
}

int opt();
void get_route();
void open_file();
void close_file();
void new_file();
void write_file();
void read_file();
void alter_file_mode();
void view_file_mode();

int main() {
    int usr_choice;
    do{
        usr_choice=opt();
    }while(usr_choice>=0&&usr_choice<=5);
    return 0;
}

void my_lock_init(char *path_name, short int lk_type) {
    char lock_file[1024]; /* 存放文件名的临时缓存 */

    /* 由于mkstemp会修改参数的最后6个字符，为避免pathname是字符串常量，
     * 需要将字符串从pathname copy到临时缓存lock_file中，然后作为创建唯一临时文件的参数 */
    strncpy(lock_file, path_name, sizeof(lock_file));
    lock_fd = mkstemp(lock_file); /* 创建唯一的临时文件并打开 */

    unlink(lock_file); /* 删除指定文件名的文件，如果有进程打开了该文件，
 * 会在进程结束后所有指向该文件的描述符都关闭后删除文件。这样可以确保即使程序崩溃，临时文件也会完全消失 */

    /*计算文件长度*//*
    long file_size = -1;
    struct stat stat_buff;
    if(stat(lock_file, &stat_buff)>=0){
        file_size=stat_buff.st_size;
    };*/

    /* 设置flock对象的写锁属性 */
    lock_it.l_type = lk_type;
    lock_it.l_whence = SEEK_SET;
    lock_it.l_start = 0;
    lock_it.l_len = 0;

    /* 设置flock对象的解锁属性 */
    unlock_it.l_type = F_UNLCK;
    unlock_it.l_whence = SEEK_SET;
    unlock_it.l_start = 0;
    unlock_it.l_len = 0;
}

void my_lock_wait() {
    int rc;
    while((rc=fcntl(lock_fd, F_SETLKW, &lock_it))<0){
        cout<<rc<<endl;
    };
}

void my_lock_release() {
    fcntl(lock_fd, F_SETLKW, &unlock_it);
}

int opt(){
    char choice;
    cout << "--------------------------------------------" << endl;
    cout << "file_tools" << endl;
    cout << "0. 关闭当前文件" << endl;
    cout << "1. 创建新文件（这将关闭现有文件）" << endl;
    cout << "2. 写文件" << endl;
    cout << "3. 读文件" << endl;
    cout << "4. 修改文件权限" << endl;
    cout << "5. 查看当前文件权限并退出" << endl;
    cout << "6. 退出file_tools（其它输入默认为该操作）" << endl;
    cout << "请根据提示输入您的操作：";
    cin>>choice;
    if(choice>'6'||choice<'0') choice='6';
    cout<<"--------------------------------------------"<<endl;
/*    while(cur_file.fd<0&&(choice>='2'&&choice<='5')){
        open_file();
    }
不能这样写，不然的话如果打开错误就一直循环直到成功打开文件为止，不能直接退出了QAQ*/

    if(cur_file.fd<0&&(choice>='2'&&choice<='5')){
        cout<<"当前未打开任何文件，先打开一个文件吧！"<<endl;
        open_file();
    }
    if(cur_file.fd>=0||choice=='1'||choice=='0'||choice=='6'){
        switch (choice) {
            case '1':
                new_file();
                break;
            case '2':
                write_file();
                break;
            case '3':
                read_file();
                break;
            case '4':
                alter_file_mode();
                break;
            case '5':
                view_file_mode();
            case '0':
                close_file();
                break;
            default:
                cout<<"n_n bye-bye! n_n"<<endl;
                close(cur_file.fd);
                cout<<"--------------------------------------------"<<endl;
                break;
        }
    }
    return choice-'0';
}

void get_route() {
    cout<<"输入文件的路径名：";
    cin>>cur_file.path_name;
}

void open_file(){
    //获取新建文件路径名
    get_route();

    //获取打开模式
    cout<<"根据提示选择打开模式："<<endl;
    cout<<"0. 只读（输入其他默认为该模式）"<<endl;
    cout<<"1. 只写"<<endl;
    cout<<"2. 可读可写"<<endl;
    cin>>cur_file.open_mode;
    if(cur_file.open_mode<0||cur_file.open_mode>2) cur_file.open_mode=0;
    cur_file.write_mode=0;

    //打开文件
    cur_file.fd=open(cur_file.path_name, cur_file.open_mode/*|cur_file.write_mode*/);
    if(cur_file.fd<0) cout<<"warning：文件打开失败！"<<endl;
}

void close_file(){
    if(cur_file.fd<0){
        cout<<"当前未打开任何文件！"<<endl;
        return;
    }else{
        close(cur_file.fd);

        //对cur_file全局变量重新初始化
        cur_file.fd=-1;
        cur_file.open_mode=0;
        cur_file.write_mode=0;
        cout<<"已关闭"<<cur_file.path_name<<endl;
        strcpy(cur_file.path_name, "");
    }
}

void new_file() {
    //关闭已打开的文件
    close(cur_file.fd);
    //获取新建文件路径名
    get_route();
    //获取创建模式
    cout<<"根据提示选择创建模式（输入其他默认为退出）："<<endl;
    cout<<"0. 只读创建"<<endl;
    cout<<"1. 只写创建"<<endl;
    cout<<"2. 可读可写创建"<<endl;
    cin>>cur_file.open_mode;
    if(cur_file.open_mode<0||cur_file.open_mode>2) return;
    //获取文件访问权限
    mode_t file_umask;
    cout<<"请根据以下规则输入umask值设置文件访问权限："<<endl;
    cout<<"umask值有三位，第一位代表所有者权限，第二位代表群组权限，第三位代表其他用户权限；"<<endl;
    cout<<"每一位对应加权值意义如下：4表示读权限，2表示写权限，1表示执行权限，通过对所有权限的相应权值相加得到该位的值。"<<endl;
    cout<<"请输入新建文件的umask值：";
    cin>>oct>>file_umask;
    //创建新文件
    cur_file.fd=open(cur_file.path_name, cur_file.open_mode|O_CREAT|O_EXCL, file_umask);
    if(cur_file.fd<0) cout<<"warning：文件创建错误！"<<endl;
    else cout<<"文件创建成功！"<<endl;
}

void write_file() {
    //首先判断权限
    if(cur_file.open_mode==O_RDONLY){
        cout<<"warning: 文件以只读方式打开，无法写文件。"<<endl;
        return;
    }
    //根据写入方式重新打开文件
    close(cur_file.fd);
    /*创建写锁*//*
    my_lock_init(cur_file.path_name, F_WRLCK);
    *//*申请锁*//*
    my_lock_wait();*/
    cout << "根据提示输入写入方式（输入其他默认退出操作）：" << endl;
    cout << "0. 清空文件并写入内容" << endl;
    cout << "1. 在文件末尾开始写入" << endl;
    scanf("%d", &cur_file.write_mode);
    if (cur_file.write_mode == 1) cur_file.write_mode = O_APPEND;
    else if(cur_file.write_mode == 0) cur_file.write_mode = O_TRUNC;
    else return;
    cur_file.fd=open(cur_file.path_name, cur_file.open_mode|cur_file.write_mode);
    flock_wait(LOCK_EX);
    //写入文件
    char write_buf[1024] = "";
    cout << "请输入写入内容：" << endl;
    getchar();
    cin.getline(write_buf, 1024, '\n');
    write_buf[strlen(write_buf)] = '\n';
    ssize_t is_write=write(cur_file.fd, write_buf, strlen(write_buf));
    if(is_write<0) cout<<"warning: 写文件错误！"<<endl;
    /*释放锁*//*
    my_lock_release();*/
    flock_rls();
}

void read_file() {
    //首先判断权限
    if(cur_file.open_mode==O_WRONLY){
        cout<<"warning: 文件以只写方式打开，无法读文件。"<<endl;
        return;
    }
    //read函数读完文件后需要重新打开文件才能重新从头开始读
    close(cur_file.fd);
    /*创建读锁*//*
    my_lock_init(cur_file.path_name, F_RDLCK);
    *//*申请锁*//*
    my_lock_wait();*/
    open(cur_file.path_name, cur_file.open_mode);
    flock_wait(LOCK_SH);
    char read_buf;
    cout<<cur_file.path_name<<"文件的内容如下："<<endl;
    ssize_t is_read=read(cur_file.fd, &read_buf, 1);
    while(is_read==1){//read函数读入EOF返回0
        cout<<read_buf;
        is_read=read(cur_file.fd, &read_buf, 1);
    }
    if(is_read<0) cout<<"warning: 读文件错误！"<<endl;
    cout<<"按下换行键以继续";
    getchar(); getchar();
    /*释放锁*//*
    my_lock_release();*/
    flock_rls();
}

void alter_file_mode() {
    flock_wait(LOCK_EX);
    //获取新的文件访问权限
    mode_t file_umask;
    cout<<"请根据以下规则输入umask值修改文件访问权限："<<endl;
    cout<<"umask值有三位，第一位代表所有者权限，第二位代表群组权限，第三位代表其他用户权限；"<<endl;
    cout<<"每一位对应加权值意义如下：4表示读权限，2表示写权限，1表示执行权限，通过对所有权限的相应权值相加得到该位的值。"<<endl;
    cout<<"请输入修改后的umask值：";
    cin>>oct>>file_umask;
    /*创建写锁*//*
    my_lock_init(cur_file.path_name, F_WRLCK);
    *//*申请锁*//*
    my_lock_wait();*/
    //修改文件访问权限
    int is_altered=fchmod(cur_file.fd, file_umask);
    if(is_altered==0) cout<<"文件:"<<cur_file.path_name<<"的权限修改成功"<<endl;
    else if(is_altered==-1) cout<<"文件:"<<cur_file.path_name<<"的权限修改失败"<<endl;
    /*释放锁*//*
    my_lock_release();*/
    flock_rls();
}

void view_file_mode() {
    //获取当前文件状态
    unsigned int mask=0700, N_BITS=3;
    struct stat buff{};
    const static char *perm[]={"---","--x","-w-","-wx","r--","r-x","rw-","rwx"};
    if(stat(cur_file.path_name, &buff)!=-1){
        printf("%s 的权限为：", cur_file.path_name);
        //将st_mode的每一位（对应二进制的三位）与0b111按位与，三位中各位若为1则表示对应用户拥有该位对应权限）
        for(int i=3; i; i--){
            printf("%3s", perm[(buff.st_mode&mask)>>(i-1)*N_BITS]);
            mask>>=N_BITS;
        }
        cout<<endl;
    }else printf("warning: %s的权限修改失败\n", cur_file.path_name);
}
