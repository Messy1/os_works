#include <iostream>
#include <cstdlib>
#include <ctime>
#include <queue>
#define ins_num 320
#define mem_size 4 //�ڴ��С��4~32ҳ��

using namespace std;

class ins{
public:
    ins() : num(-1), pg_num(-1) {};
    int num;
    int pg_num;
}ins_list[ins_num];

class page{
public:
    page() : num(-1), wt(0) {};
    explicit page(int p_num, int p_wt) : num(p_num), wt(p_wt){};
    int num;
    int wt;//Ȩֵ
};

//class ptr_epg{
//public:
//    ext_page *p;
//
//    bool operator<(const ptr_epg &b) const{
//        return this->p->wt > b.p->wt;
//        return this->p->wt == b.p->wt ? this->p->wt > b.p->wt : this->p->wt < b.p->wt;
//    }//����LFU�㷨�жԵ��ô����ıȽ�
//}epg_ptr[32];

//int ins_list[ins_num];//ָ������
//int in_mem[ins_num]={0}; //��ʶָ�����ڴ���

void init_ins_list();
void show_ins_list();

//�����㷨����ȱҳ����
int FIFO();
int LRU();
int LFR();
int OPT();

int main() {
    init_ins_list();
    //show_ins_list();
    cout<<"      Page Fault times\t"<<"hit rate"<<endl;
    cout<<"FIFO: "<<FIFO()<<"\t\t"<<(ins_num-FIFO())*100.0/ins_num<<"%"<<endl;
    cout<<"LRU : "<<LRU()<<"\t\t"<<(ins_num-LRU())*100.0/ins_num<<"%"<<endl;
    cout<<"LFR : "<<LFR()<<"\t\t"<<(ins_num-LFR())*100.0/ins_num<<"%"<<endl;
    cout<<"OPT : "<<OPT()<<"\t\t"<<(ins_num-OPT())*100.0/ins_num<<"%"<<endl;
    printf("Memory Size��%2d pages\n", mem_size);
    return 0;
}

void init_ins_list() {
    time_t t;
    srand((unsigned)time(&t));
    int cur_ins=rand()%ins_num;//��ǰָ��
    ins_list[0].num=cur_ins; ins_list[0].pg_num=cur_ins/10;
    cur_ins=(cur_ins+1)%ins_num;//˳��ִ����һָ��
    ins_list[1].num=cur_ins; ins_list[1].pg_num=cur_ins/10;
    for(int i=2; i<ins_num; i+=2){
        int front, rear;//ȡָ����
        if((i/2)%2){
            front=0; rear=cur_ins;
        }else{
            front=cur_ins+1; rear=319;
        }
        cur_ins=front+rand()%(rear-front);
        ins_list[i].num=cur_ins; ins_list[i].pg_num=cur_ins/10;
        cur_ins=(cur_ins+1)%ins_num;
        ins_list[i+1].num=cur_ins; ins_list[i+1].pg_num=cur_ins/10;
    }
}

void show_ins_list() {
    cout<<"ָ����������ҳ���£�"<<endl;
    for(int i=0; i<ins_num; i++){
        printf("%2d ", ins_list[i].pg_num);
        if((i+1)%20==0) cout<<endl;
    }
    cout<<"==========================================================="<<endl;
}

int FIFO() {
    int pg_ft=0;
    queue <int> pg_tb;//�洢��ǰλ���ڴ��ҳ��
    for(int i=0; i<ins_num; i++){
        bool pg_in_mem=false;//��ʶλ�Ƿ����ڴ���
        //printf("ҳ%3d����ʱ�ڴ���ҳ�ţ�", ins_list[i].pg_num);
        int j;
        for(j=(int)pg_tb.size(); j>0; j--){
            //printf("%2d ", pg_tb.front());
            if(pg_tb.front()==ins_list[i].pg_num)
                pg_in_mem= true;
            pg_tb.push(pg_tb.front());
            pg_tb.pop();
        }
        //cout<<endl;
        if(!pg_in_mem){
            pg_ft++;
            if(pg_tb.size()==mem_size) pg_tb.pop();
            pg_tb.push(ins_list[i].pg_num);
        }
    }
    return pg_ft;
}

int LRU() {
    int pg_ft=0;
    queue <int> pg_tb;//�洢��ǰλ���ڴ��ҳ��
    for(int i=0; i<ins_num; i++){
        bool pg_in_mem=false;//��ʶλ�Ƿ����ڴ���
        //printf("ָ��%3d����ʱ�ڴ���ҳ�ţ�", i);
        int j;
        for(j=(int)pg_tb.size(); j>0; j--){
            //printf("%2d ", pg_tb.front());
            if(pg_tb.front()==ins_list[i].pg_num)
                pg_in_mem= true;
            else{
                pg_tb.push(pg_tb.front());
            }
            pg_tb.pop();
        }

        //cout<<endl;

        if(!pg_in_mem){
            pg_ft++;
            if(pg_tb.size()==mem_size){
                pg_tb.pop();
            }
        }
        pg_tb.push(ins_list[i].pg_num);
    }
    return pg_ft;
}

int LFR() {
    int pg_ft=0;
    queue <page> pg_tb;
    for(int i=0; i<ins_num; i++){
        bool pg_in_mem = false;
        int j;
//        printf("ҳ%3d����ʱ�ڴ���ҳ�ţ�", ins_list[i].pg_num);
        for(j = (int)pg_tb.size(); j>0; j--){
//            printf("%2d:%d  ",pg_tb.front().num, pg_tb.front().wt);
            if (pg_tb.front().num == ins_list[i].pg_num) {
                pg_in_mem = true;
                pg_tb.front().wt++;
            }
            pg_tb.push(pg_tb.front());
            pg_tb.pop();
        }
//        cout<<endl;
        if(!pg_in_mem){//������ҳ
            pg_ft++;
            if(pg_tb.size()==mem_size){
//                �ҵ���СȨֵ��ҳ�����ٷ��ʣ�
                int j;
                page lfr_pg = pg_tb.front();
                for(j=(int)pg_tb.size(); j>0; j--){
                    if(lfr_pg.wt>pg_tb.front().wt){
                        lfr_pg=pg_tb.front();
                    }
                    pg_tb.push(pg_tb.front());
                    pg_tb.pop();
                }
//                pop lfr_page
                for(j=(int)pg_tb.size(); j>0; j--){
                    if(lfr_pg.num==pg_tb.front().num){
                        pg_tb.pop();
                    }else{
                        pg_tb.push(pg_tb.front());
                        pg_tb.pop();
                    }
                }

            }
//            ������ҳ�ĳ�ʼ��
            page cur_pg(ins_list[i].pg_num, 1);
            pg_tb.push(cur_pg);
        }
    }
    return pg_ft;
    {
//    priority_queue����ʵʱ���£��������ã�����ֱ�ӱ�������СȨֵҳ
//    for(int i=0; i<32; i++){
//        e_pg[i].num=i;
//        epg_ptr[i].p=&e_pg[i];
//    }

//    priority_queue<ptr_epg > pg_tb;//�洢��ǰλ���ڴ��ҳ��
//    priority_queue<ptr_epg > cpy_tb;//�������ڱ���
//    for(int i=0; i<ins_num; i++){
//        bool pg_in_mem=false;//��ʶλ�Ƿ����ڴ���
//        printf("ָ��%3d����ʱ�ڴ���ҳ�ż���Ȩֵ��", ins_list[i].num);
//        cpy_tb=pg_tb;
//        while(cpy_tb.size()>0){
//            printf("%2d:%d ", cpy_tb.top().p->num, cpy_tb.top().p->wt);
//            if(cpy_tb.top().p->num==ins_list[i].pg_num){
//                pg_in_mem= true;
//            }
//            cpy_tb.pop();
//        }
//        if(pg_in_mem== false){
//            if(pg_tb.size()==mem_size){
//                pg_tb.top().p->wt=0;//�Ƴ��ڴ��ҳ����wtȨֵ
//                pg_tb.pop();
//            }
//            pg_tb.push(epg_ptr[ins_list[i].pg_num]);
//            pg_ft++;
//        }
//        e_pg[ins_list[i].pg_num].wt++; //Priority_Queue����ʵʱ���£�
//        cpy_tb=pg_tb;
//        pg_tb=cpy_tb;
//        cout<<endl;
//    }
    }
}

int OPT() {
    int pg_ft=0;
    queue <page> pg_tb;
    for(int i=0; i<ins_num; i++){
        bool pg_in_mem = false;
        int j;

//        ����
        {
//        j = (int)pg_tb.size();
//        //����Ȩֵ�����ǿ��Ƕ��Ժ���Ҳ������ҳ��wt���£�����
//        //�����ҳȨֵ��wt=ins_num-��future-current��
//        j=(int)pg_tb.size();
//        for(j; j>0; j--){
//            bool will_come= false;
//            for(int k = i+1; k<ins_num; k++){
//                if(pg_tb.front().num==ins_list[k].pg_num){
//                    will_come= true;
//                    pg_tb.front().wt=ins_num-(k-i);
//                    break;
//                }
//                if(will_come== false) pg_tb.front().wt=0;
//            }
//            pg_tb.push(pg_tb.front());
//            pg_tb.pop();
//        }
        }
//        printf("ҳ%3d����ʱ�ڴ���ҳ�ţ�", ins_list[i].pg_num);

        for(j=(int)pg_tb.size(); j>0; j--){
//            printf("%2d:%d  ",pg_tb.front().num, pg_tb.front().wt);
            if (pg_tb.front().num == ins_list[i].pg_num) {
                pg_in_mem = true;
            }
            pg_tb.push(pg_tb.front());
            pg_tb.pop();
        }
//        cout<<endl;
//        ����Ȩֵ��
//        �����ҳȨֵ��wt=ins_num-��future-current��,������Ҳ������ҳ��ȨֵΪ0
//        ����Ȩֵ�����ǿ��Ƕ��Ժ���Ҳ������ҳ��wt����Ϊ0������
//        Ȩֵ������Ż�������0<wt<319��ҳ�����Զ���wtֱ��++���������ٱ���һ��,
//        �������Ļ��͵�ÿ�θ��¶���ȱҳʱ���£������ֲ��ñ��������ǿ�һЩ�����Ի���ÿ�θ��°�
        for(j=(int)pg_tb.size(); j>0; j--){
            if(pg_tb.front().wt>0&&pg_tb.front().wt<319){
                pg_tb.front().wt++;
            }else if(pg_tb.front().wt==319)
            {
                bool will_come= false;
                for(int k = i+1; k<ins_num; k++){
                    if(pg_tb.front().num==ins_list[k].pg_num){
                        will_come= true;
                        pg_tb.front().wt=ins_num-(k-i);
                        break;
                    }
                }
                if(!will_come) pg_tb.front().wt=0;
            }
            pg_tb.push(pg_tb.front());
            pg_tb.pop();
        }
        if(!pg_in_mem){//������ҳ
            pg_ft++;
            if(pg_tb.size()==mem_size){
//                �ҵ�������ò������ʵ�ҳ��
//                �ҵ���СȨֵ��ҳ������������ʣ�
                page lfr_pg = pg_tb.front();
                for(j=(int)pg_tb.size(); j>0; j--){
                    if(lfr_pg.wt>pg_tb.front().wt){
                        lfr_pg=pg_tb.front();
                    }
                    pg_tb.push(pg_tb.front());
                    pg_tb.pop();
                }

//                pop future_lru_page
                for(j=(int)pg_tb.size(); j>0; j--){
                    if(lfr_pg.num==pg_tb.front().num){
                        pg_tb.pop();
                    }else{
                        pg_tb.push(pg_tb.front());
                        pg_tb.pop();
                    }
                }
            }
//            ������ҳ�ĳ�ʼ��
            page cur_pg(ins_list[i].pg_num, 0);
            for(int k = i+1; k<ins_num; k++){
                if(cur_pg.num==ins_list[k].pg_num){
                    cur_pg.wt=ins_num-(k-i);
                    break;
                }
            }
            pg_tb.push(cur_pg);
        }
        {
//        //�����ҳȨֵ��wt=ins_num-��future-current��
//        j=(int)pg_tb.size();
//        for(j; j>0; j--){
//            bool will_come= false;
//            for(int k = i+1; k<ins_num; k++){
//                if(pg_tb.front().num==ins_list[k].pg_num){
//                    will_come= true;
//                    pg_tb.front().wt=ins_num-(k-i);
//                    break;
//                }
//                if(will_come== false) pg_tb.front().wt=0;
//            }
//            pg_tb.push(pg_tb.front());
//            pg_tb.pop();
//        }


//        j=pg_tb.size();
//        printf("ҳ%3d������ڴ���ҳ�ţ�", ins_list[i].pg_num);
//        for(j; j>0; j--){
//            printf("%2d:%d  ",pg_tb.front().num, pg_tb.front().wt);
//            pg_tb.push(pg_tb.front());
//            pg_tb.pop();
//        }
//        cout<<endl;
        }
    }
    return pg_ft;
}

