/*
Copyright 2024 Xiangnan Zhang, School of Future Technology, Beijing Institute of Technology

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define SUCCESS 1
#define FAIL 0
#define MAX 1000
#define STR_MAX 200
#define CLASSMAX 5
#define FP_CLOSED 2

typedef struct 
{
    char english[30];
    char class[CLASSMAX][CLASSMAX];
    char chinese[80];
    time_t timer;
} word_old;

typedef struct 
{
    char english[30];
    char class[CLASSMAX][CLASSMAX];
    char chinese[80];
    time_t timer;
    char note[MAX];
} word;

/*Bellow: Assistant Functions*/

void str_split(char str[], char store[][STR_MAX])
{
    char *end;
    for(end=str;*end!='\0';end++)
    {
        if(*end==' ') *end='\0';
    }
    char *p=str; int i=0;
    while(p!=end)
    {
        strcpy(store[i],p);
        i++;
        for(;*p!='\0';p++);
        if(p!=end) for(;*p=='\0'; p++);
    }
    store[i][0]='\0';
}

void getstr(char *str, char str_list[][STR_MAX])
{
    str[0]='\0';  /* 初始化 */
    int list_idx=0;
    if(str_list!=NULL) for(; str_list[list_idx][0]!='\0'; list_idx++);
    int list_rem=list_idx; /* list_rem记录str_list的尾部 */
    int keep=0;  /* 用于后续上下键控制，表明是否记录当前字符串 */

    char ch;  /*用以存储getch读取的字符*/
    char *cursor=str, *end=str+1;  /*二者作用于数组str，是控制台中光标与输入结尾在内存中的映射*/
    int counter=0;  /*用于字节计数*/
    char chich[4];  /*以字符串的形式存储一个中文字符*/
    do
    {
        ch=getch();
        switch(ch)
        {
            case 127:  /*127是backspace键键入的ASCII码*/
                {
                    if(cursor<=str) continue;
                    char temp[200];
                    char *p;
                    int i=0;
                    for(p=cursor,i=0;p<end-1;p++,i++) temp[i]=*p;
                    temp[i]='\0';
                    if(*(cursor-1)>0) {printf("\b%s \b",temp); cursor--; end--;}
                    else {printf("\b\b%s  \b\b",temp); cursor-=3; end-=3;} 
                    if(end-cursor!=1)
                    {
                        int idx;
                        for(idx=strlen(temp)-1;idx>=0;idx--)
                        {
                            if(temp[idx]>0) printf("\b");
                            else
                            {
                                printf("\b\b");
                                idx-=2;
                            }
                        }   
                    }
                    strcpy(cursor, temp);
                    list_idx=list_rem;
                    keep=0;
                } break;
            case 27:
                {
                    char ch2=getch();
                    char ch3=getch();
                    if(ch2==91 && ch3==68 && cursor>str)  /* left */
                    {
                        if(*(cursor-1)>0)
                        {
                            printf("\b"); 
                            cursor--;
                        }
                        else
                        {
                            printf("\b\b");
                            cursor-=3;
                        }
                    }
                    else if(ch2==91 && ch3==67 && cursor<end-1)  /* right */
                    {
                        if(*(cursor)>0)
                        {
                            printf("\033[1C"); 
                            cursor++;
                        }
                        else
                        {
                            printf("\033[1C\033[1C");
                            cursor+=3;
                        }
                    }
                    else if(ch2==91 && str_list!=NULL)  /* 上下键控制原理：list_idx总指向当前显示的字符串（指向空字符串时显示用户正在输入的文本）*/
                    {
                        char currency[STR_MAX];
                        if(keep==0)
                        {
                            char *p; int i=0;
                            for(p=str; p<end-1; p++, i++) currency[i]=*p;
                            currency[i]='\0';
                        }

                        if(ch3==65 && list_idx>=0)  /* up */
                        {
                            list_idx--;
                            if(list_idx<0)
                            {
                                list_idx++;
                                continue;
                            }

                            int back_num=cursor-str;  /* to clear the exsisted string in the screen */
                            for(; back_num>0; back_num--) printf("\b");
                            int clear_i=0;
                            for(; clear_i<strlen(str); clear_i++) printf(" ");
                            for(clear_i=0; clear_i<strlen(str); clear_i++) printf("\b");

                            strcpy(str, str_list[list_idx]);
                            int sub;
                            for(sub=0; str[sub]!='\0'; sub++);
                            cursor=&(str[sub]);
                            end=cursor+1;
                            printf("%s", str);

                            keep=1;
                        }
                        else if(ch3==66)  /* down */
                        {
                            list_idx++;
                            if(str_list[list_idx][0]=='\0' && str_list[list_idx-1][0]=='\0')
                            {
                                list_idx--;
                                continue;
                            }
                            else
                            {
                                int back_num=cursor-str;  /* to clear the exsisted string in the screen */
                                for(; back_num>0; back_num--) printf("\b");
                                int clear_i=0;
                                for(; clear_i<strlen(str); clear_i++) printf(" ");
                                for(clear_i=0; clear_i<strlen(str); clear_i++) printf("\b");

                                if(str_list[list_idx][0]!='\0') strcpy(str, str_list[list_idx]);
                                else strcpy(str, currency);
                                int sub;
                                for(sub=0; str[sub]!='\0'; sub++);
                                cursor=&(str[sub]);
                                end=cursor+1;
                                printf("%s", str);

                                keep=1;
                            }
                        }
                    }
                    else;
                } break;
            default:
            {
                if(ch!='\n')
                {    
                    char temp[200];
                    int i=0; char *p;
                    for(i=0;i<200;i++) temp[i]='\0'; i=0;
                    for(p=cursor;p<end-1;p++,i++) temp[i]=*p;
                    temp[i]='\0';
                    if(ch>0)
                    {
                        counter=0;
                        printf("%c",ch);
                        printf("%s",temp);
                        if(end-cursor!=1)
                        {
                            int idx;
                            for(idx=strlen(temp)-1;idx>=0;idx--)
                            {
                                if(temp[idx]>0) printf("\b");
                                else
                                {
                                    printf("\b\b");
                                    idx-=2;
                                }
                            }   
                        }
                    }
                    else
                    {
                        chich[counter]=ch;
                        counter++;
                        if(counter==3) 
                        {
                            chich[counter]='\0';
                            printf("%s",chich);
                            printf("%s",temp);
                            counter=0;
                            if(end-cursor!=1)
                            {
                                int idx;
                                for(idx=strlen(temp)-1;idx>=0;idx--)
                                {
                                    if(temp[idx]>0) printf("\b");
                                    else
                                    {
                                        printf("\b\b");
                                        idx-=2;
                                    }
                                }
                            }
                        }
                    }
                    *cursor=ch;
                    cursor++;
                    strcpy(cursor,temp);
                    end++;
                }
                list_idx=list_rem;
                keep=0;
            }
        }
    }while(ch!='\n');
    printf("\n");
    *(end--)='\0';
    if(str_list!=NULL)
    {
        int append_idx=0;
        for(; str_list[append_idx][0]!='\0'; append_idx++);
        strcpy(str_list[append_idx], str);
    }
}

void wash_str(char str[], char cleared[])
{
    char *start, *end;
    for(start=str; *start==' '; start++);
    for(end=&(str[strlen(str)]); *end=='\0' || *end==' '; end--);
    char *p; int i;
    for(p=start,i=0; p<=end; p++,i++)
    {
        cleared[i]=*p;
    }
    cleared[i]='\0';
}

void convert_dir(char dir[])
{
    if(dir[0]=='~')
    {
        FILE *fp=popen("cd ~ && pwd", "r");
        char root_dir[100];
        int i=0;
        do
        {
            root_dir[i]=fgetc(fp);
            if(feof(fp)!=0) root_dir[i-1]='\0';
            i++;
        }while(feof(fp)==0);
        char *p=dir+1;
        strcat(root_dir, p);
        strcpy(dir, root_dir);
        pclose(fp);
    }
}

void fgetstr(char *str, int len, FILE *fp)
{
    fgets(str, len, fp);
    char *p;
    for(p=str;*p!='\n' && *p!='\0'; p++);
    *p='\0';
}

void path_split(char full[], char dir[], char file[])
{
    char *label;
    char spl;
    for(label=full; *label!='\0'; label++)
    {
        if(*label=='\\' || *label=='/') spl=*label;
    }
    for(;*label!='\\' && *label!='/'; label--);
    char *p;
    int i;
    for(i=0,p=full;p!=label;p++,i++) dir[i]=*p;
    dir[i]=spl; dir[i+1]='\0';
    p++;
    strcpy(file,p);
}

int date_split(char content[], int *year, int *month, int *day)
{
    if(content[0]=='p')
    {
        char *pnum=content+1;
        int pday=strtol(pnum, &pnum, 10);
        time_t timer=time(NULL); timer=timer-pday*86400;
        struct tm *t=localtime(&timer);
        *year=t->tm_year+1900;
        *month=t->tm_mon+1;
        *day=t->tm_mday;
        return SUCCESS;
    }

    char *dot_1, *dot_2;
    for(dot_1=content; *dot_1!='.' && *dot_1!='\0'; dot_1++);
    if(*dot_1=='\0' || *(dot_1+1)=='\0')
    {
        return FAIL;
    }
    for(dot_2=dot_1+1;*dot_2!='.' && *dot_2!='\0'; dot_2++);
    if(*dot_2=='\0' || *(dot_2+1)=='\0')
    {
        return FAIL;
    }

    char year_str[10], month_str[10], day_str[10];
    char *p; int i;
    for(p=content, i=0; p!=dot_1; p++, i++)
    {
        year_str[i]=*p;
    }
    for(p=dot_1+1, i=0; p!=dot_2; p++,i++)
    {
        month_str[i]=*p;
    }
    p=dot_2+1;
    strcpy(day_str,p);

    *year=strtol(year_str, &p, 10);
    *month=strtol(month_str, &p, 10);
    *day=strtol(day_str, &p, 10);

    return SUCCESS;
}

void printword(word w, char ch)
{
    struct tm *t=localtime(&(w.timer));
    if(ch=='y')
    {
        printf("%4d.%2d.%2d ",t->tm_year+1900, t->tm_mon+1, t->tm_mday);
        printf("\033[1m%20s\033[0m ",w.english);
    }
    else if(ch=='n') printf("\033[1m%s\033[0m ",w.english);
    int i=0;
    while(i<CLASSMAX && strcmp(w.class[i],"")!=0)
    {
        if(i==0) printf("%s",w.class[i]);
        else printf("/%s", w.class[i]);
        i++;
    }
    printf(".%s\n",w.chinese);
}

int classcmp(word w, char str[])
{
    int idx=0;
    while(idx<CLASSMAX && strcmp(w.class[idx],"")!=0)
    {
        if(strcmp(w.class[idx],str)==0)
        {
            return 0;
        }
        idx++;
    }
    return 1;
}

int check_kernel_list(char KernelListFile_name[], char kernel_name[], char found_dir[])
{
    FILE *KernelListFile_fp=fopen(KernelListFile_name, "r");
    if(KernelListFile_fp==NULL) return FAIL;
    do
    {
        char aline[STR_MAX];
        int i=0;
        do
        {
            aline[i]=fgetc(KernelListFile_fp);
            i++;
        }while(!feof(KernelListFile_fp) && aline[i-1]!='\n');
        if(feof(KernelListFile_fp)!=0) continue;
        aline[i-1]='\0';
        char name[STR_MAX], dir[STR_MAX];
        char *p;
        for(p=aline; *p!='='; p++);
        *p='\0';
        strcpy(name, aline); p++; strcpy(dir, p);
        if(strcmp(name, kernel_name)==0)
        {
            strcpy(found_dir, dir);
            fclose(KernelListFile_fp);
            return SUCCESS;
        }
    }while(!feof(KernelListFile_fp));
    fclose(KernelListFile_fp);
    found_dir[0]='\0';
    return FAIL;
}

/*Upon: Assistant Functions*/

/*Below: Operating Functions*/

int upgrade(char content[])
{
    char store[4][STR_MAX];
    str_split(content,store);
    char old_dir[STR_MAX], new_dir[STR_MAX];
    strcpy(old_dir, store[0]);
    strcpy(new_dir, store[1]);
    if(strlen(old_dir)==0 || strlen(new_dir)==0) return FAIL;

    FILE *old_fp=fopen(old_dir,"rb");
    FILE *new_fp=fopen(new_dir,"wb");
    do
    {
        word_old w_o; word w;
        fread(&w_o,sizeof(word_old),1,old_fp);
        if(feof(old_fp)==0)
        {
            strcpy(w.english,w_o.english);
            int i;
            for(i=0;i<CLASSMAX;i++) strcpy(w.class[i],w_o.class[i]);
            strcpy(w.chinese, w_o.chinese);
            w.timer=w_o.timer;
            w.note[0]='\0';
            fwrite(&w, sizeof(word), 1, new_fp);
        }
    }while(feof(old_fp)==0);
    fclose(new_fp);
    return SUCCESS;
}

int function_detector(char str[], char param[], char content[])  /*return type and cut str*/
{
    strcpy(param,"");
    strcpy(content,"");
    char *first=str;
    for(;*first!=' ' && *first!='\0';first++);

    int is=0;
    char *detect;
    for(detect=first;*detect!='\0';detect++)
    {
        if(*detect!=' ' && *detect!='\0') 
        {
            is=1;
            break;
        }
    }
    
    char command[30];
    *first='\0';
    strcpy(command,str);
    if(is==1)
    {
        char *second=NULL;
        int flag=0;
        for(second=first+1;*second!='\0';second++)
        {
            if(*second=='-' && (*(second-1)==' ' || *(second-1)=='\0')) 
            {
                flag=1;
                break;
            }
        }
        if(flag==1)
        {
            char *third;
            for(third=second;*third!=' ' && *third!='\0';third++);
            if(*third==' ')
            {
                *third='\0';
                strcpy(param,second);
                char *forth;
                for(forth=third+1;*forth==' ';forth++);
                strcpy(content,forth);
            }
            else
            {
                strcpy(param,second);
                strcpy(content,"");
            }
        }
        else
        {
            for(second=first+1;*second==' ';second++);
            strcpy(param,"");
            strcpy(content,second);
        }
    }

    if(strcmp(command,"addword")==0 || strcmp(command,"add")==0) return 1;
    else if(strcmp(command,"delword")==0 || strcmp(command,"del")==0 || strcmp(command,"delete")==0) return 2;
    else if(strcmp(command,"searchword")==0 || strcmp(command,"search")==0) return 3;
    else if(strcmp(command,"list")==0 || strcmp(command,"listall")==0) return 4;
    else if(strcmp(command,"calculate")==0) return 5;
    else if(strcmp(command, "kernel")==0) return 6;
    else if(strcmp(command, "practice")==0 || strcmp(command, "practise")==0) return 7;
    else if(strcmp(command,"help")==0) return 8;
    else if(strcmp(command,"upgrade")==0) return 9;
    else if(strcmp(command, "note")==0) return 10;
    else return -1;
}

int calculate(FILE *fp)  /*5*/
{
    rewind(fp);

    int counter=0;
    
    fgetc(fp);
    fseek(fp,-1,1);

    if(feof(fp))
    {
        return 0;
    }
    else
    {
        word empty;
        do
        {
            counter++;
            fread(&empty,sizeof(word),1,fp);
        }while(!feof(fp));
        counter--;
    }

    return counter;
}

void loadall(word vocab[], FILE *fp) /*No permission to users*/
{
    rewind(fp);

    int n=calculate(fp);
    int i;
    rewind(fp);
    for(i=0;i<n;i++)
    {
        fread(&(vocab[i]),sizeof(word),1,fp);
    }
}

int addword(char str[], FILE *fp, char *kernel_dir) /*1*/
{
    int k;
    if(strlen(str)<=3) 
    {
        return FAIL;
    }

    fseek(fp,0,2);

    char *p=str;
    for(;*p!=' ';p++);
    if(*p=='\0') {return FAIL;}
    else *p='\0';
    word new;
    new.note[0]='\0';
    int i;
    for(i=0;i<CLASSMAX;i++) strcpy(new.class[i],"");
    strcpy(new.english,str);

    for(p++;*p==' ';p++);
    if(*p=='\0') {return FAIL;}

    char *pb;
    for(pb=p;*pb!='.';pb++);
    if(*pb=='\0') {return FAIL;}
    *pb='\0';
    char class[25];
    strcpy(class,p);
    char *g1,*g2;
    g1=class;
    for(g2=g1+1,i=0; *g2!='\0';g2++)
    {
        if(*g2=='/')
        {
            *g2='\0';
            strcpy(new.class[i],g1);
            g1=g2+1;
            if(*g1=='\0') return FAIL;
            else
            {
                g2=g1;
            }
            i++;
        }
    }
    strcpy(new.class[i],g1);

    pb++;
    if(*pb=='\0') {return FAIL;}
    strcpy(new.chinese,pb);

    int is_closed=0;  /* to check if the word has been added */
    word all[MAX];
    loadall(all, fp);
    int num=calculate(fp);
    int found=0;
    for(i=0; i<num; i++)
    {
        if(strcmp(all[i].english, new.english)==0)
        {
            found=1;
            break;
        }
    }
    if(found==1)
    {
        int delword(char str[], FILE *fp, char kernel_dir[]); 
        printf("\033[2mWord \"%s\" has been added, do you want to renew it? (y/n): \033[0m", new.english);
        char ch[10];
        getstr(ch, NULL);
        if(strcmp(ch, "y")==0 || strcmp(ch, "Y")==0) 
        {
            new=all[i];
            delword(new.english, fp, kernel_dir);
            fp=fopen(kernel_dir, "ab");
            is_closed=FP_CLOSED;
        }
        else return SUCCESS;
    }

    new.timer=time(NULL);

    fseek(fp,0,2);
    int condition=fwrite(&new,sizeof(word),1,fp);
    if(condition==0) return FAIL;
    else if(is_closed==FP_CLOSED) 
    {
        fclose(fp);
        return FP_CLOSED;
    }
    else return SUCCESS;
}

int delword(char str[], FILE *fp, char kernel_dir[])  /*2*/
{
    if(strlen(str)==0)
    {
        return FAIL;
    }

    word all[MAX];
    loadall(all,fp);
    int n=calculate(fp);
    fclose(fp);

    int i;
    int found=0;

    for(i=0;i<n;i++)
    {
        char *p;
        if(str[0]<0) found=!(strcmp(all[i].chinese, str));
        else found=!(strcmp(all[i].english,str));
        if(found==1) 
        {
            int j;
            for(j=i;j+1<n;j++) all[j]=all[j+1];
            break;
        }
    }

    if(found==1)
    {
        char new_dir[500];
        strcpy(new_dir,kernel_dir);
        strcat(new_dir,".new");

        FILE *newf=fopen(new_dir,"w");
        for(i=0;i<n-1;i++) fwrite(&(all[i]),sizeof(word),1,newf);
        fclose(newf);

        remove(kernel_dir);
        rename(new_dir,kernel_dir);

        return SUCCESS;
    }
    else 
    {
        return FAIL;
    }
}

int search(char str[], FILE *fp)  /*3*/
{
    if(strlen(str)==0) return FAIL;
    
    word all[MAX];
    loadall(all,fp);
    int n=calculate(fp);
int i;
    int tape=-1;
    int found=0;

    for(i=0;i<n;i++)
    {
        if(strcmp(all[i].english, str)==0)
        {
            tape=i;
            found=1;
            break;
        }
    }
    
    char *p=NULL;
    if(found==0)
    {
        for(i=0;i<n;i++)
        {
            if(str[0]<0) p=strstr(all[i].chinese,str);
            else p=strstr(all[i].english,str);
            if(p!=NULL) 
            {
                tape=i;
                break;
            }
        }
    }

    if(found==1 || p!=NULL)
    {
        printword(all[tape],'y');
        printf("\033[2mNOTE:\n");
        printf("%s\033[0m\n", all[tape].note);
        return SUCCESS;
    }
    else
    {
        return FAIL;
    }
}

void listall(FILE *fp, char param[], char content[])  /*4*/
{
    word all[MAX];
    loadall(all,fp);
    int num=calculate(fp);
    if(num==0)
    {
        printf("Kernel is empty\n");
        return;
    }

    if(strcmp(param,"--all")==0 || strcmp(param, "-a")==0)
    {
        int i;
        for(i=0;i<num;i++)
        {
            printword(all[i],'y');
        }
    }
    else if(strcmp(param, "--date")==0 || strcmp(param, "-d")==0)
    {
        int year, month, day;
        int condition=date_split(content, &year, &month, &day);
        if(condition==FAIL)
        {
            printf("\033[31mERROR: date format error.\033[0m\n");
            return;
        }
        
        word list[MAX];
        int calc=0, i=0;
        for(i=0;i<num;i++)
        {
            struct tm *t=localtime(&(all[i].timer));
            if((t->tm_year+1900)==year && (t->tm_mon+1)==month && t->tm_mday==day)
            {
                list[calc]=all[i];
                calc++;
            }
        }
        num=calc;

        if(calc==0)
        {
            printf("No word stored on %d.%d.%d\n", year, month, day);
            return;
        }
        else
        {
            for(i=0;i<num;i++)
            {
                printword(list[i],'y');
            }
        }
    }
    else if(strcmp(param,"--class")==0 || strcmp(param, "-c")==0)
    {
        word list[MAX];
        int i,calc=0;
        for(i=0;i<num;i++)
        {
            if(classcmp(all[i],content)==0) 
            {
                list[calc]=all[i];
                calc++;
            }
        }
        num=calc;

        if(num==0)
        {
            printf("No word in class %s\n", content);
            return;
        }
        else
        {
            for(i=0;i<num;i++)
            {
                printword(list[i],'y');
            }
        }
    }
    else printf("\033[31mERROR: parameter error.\033[0m Available parameter: -a (--all), -c (--class), -d (--date)\n");
}

void practice(FILE *fp, char param[], char content[])
{
    word all[MAX], u_list[MAX]; /* unshuffled list */
    loadall(all, fp);
    int num=calculate(fp);

    if(strlen(content)==0 || strcmp(content,"all")==0 || strcmp(content,"")==0)
    {
        int i;
        for(i=0;i<num;i++) u_list[i]=all[i];
    }
    else
    {
        int year,month,day;
        int condition=date_split(content, &year, &month, &day);
        if(condition==FAIL)
        {
            printf("\033[31mERROR: date format error.\033[0m\n");
            return;
        }

        int calc=0, i=0;
        for(i=0;i<num;i++)
        {
            struct tm *t=localtime(&(all[i].timer));
            if((t->tm_year+1900)==year && (t->tm_mon+1)==month && t->tm_mday==day)
            {
                u_list[calc]=all[i];
                calc++;
            }
        }
        num=calc;
        if(calc==0)
        {
            printf("\033[31mERROR: No word stored on %d.%d.%d\033[0m\n", year, month, day);
            return;
        }
    }

    word list[MAX];  /* this block is aimed to shuffle the list */
    int idx_list[MAX]; idx_list[0]=-1;  /* used to store the index generated by rand() */
    int idx_i=0; /* idx_list's index, meanwhile it is shuffled's index, because they will increase at the same thime */
    unsigned int seed_s=time(NULL);
    while(idx_i<num)
    {
        srand(seed_s);
        seed_s-=10;  /*make the seed different */
        int idx=rand()%num;
        int j, idx_found=0; /* j is a pointer used to traverse the idx_list */
        for(j=0; j<idx_i; j++) /* to check if the index has beed generated */
        {
            if(idx_list[j]==idx)
            {
                idx_found=1;
                break;
            }
        }
        if(idx_found==0) 
        {
            list[idx]=u_list[idx_i];
            idx_list[idx_i]=idx;
            idx_i++;
        }
    }

    if(strstr(param, "l")!=NULL)
    {
        int i=0;
        if(strstr(param, "c")!=NULL)
        {
            for(; i<num; i++)
            {
                printf("%d/%d  %s: ", i+1, num, list[i].chinese);
                char eng[STR_MAX], eng_washed[STR_MAX];
                getstr(eng, NULL); wash_str(eng, eng_washed);  /* delete the space at the begining and end of eng, if they exist */
                if(strcmp(eng_washed, "!q")==0) return;
                printf("  ");
                if(strstr(list[i].english, eng_washed)!=NULL && eng_washed[0]!='\0')
                {
                    printf("\033[32mCorrect.\033[0m ");
                    printword(list[i],'n');
                }
                else
                {
                    printf("\033[31mIncorrect.\033[0m ");
                    printword(list[i],'n');
                }
            }
        }
        else
        {
            for(; i<num; i++)
            {
                printf("%d/%d  %s: ", i+1, num, list[i].english);
                char chi[STR_MAX], chi_washed[STR_MAX];
                getstr(chi, NULL); wash_str(chi, chi_washed);  /* delete the space at the begining and end of chi, if they exist */
                if(strcmp(chi_washed, "!q")==0) return;
                printf("  ");
                if(strstr(list[i].chinese, chi_washed)!=NULL && chi_washed[0]!='\0')
                {
                    printf("\033[32mCorrect.\033[0m ");
                    printword(list[i],'n');
                }
                else
                {
                    printf("\033[31mIncorrect.\033[0m ");
                    printword(list[i],'n');
                }
            }
        }
        
        return;
    }

    int idx=0;

    if(strcmp(param,"-e")==0)
    {
        printf("%s\n",list[idx].english);
        printf("Chinese: ");

        char ans[200];
        getstr(ans, NULL);

        char *p=strstr(list[idx].chinese, ans);
        if(p==NULL || strlen(ans)==0)
        {

            printf("\033[31mIncorrect.\033[0m Correct answer: ");
            printword(list[idx],'n');
        }
        else
        {
            printf("\033[32mCorrect.\033[0m ");
            printword(list[idx],'n');
        }
    }
    else
    {
        printword(list[idx],'e');
        printf("English: ");

        char ans[200];
        getstr(ans, NULL);

        char *p=strstr(list[idx].english, ans);
        if(p==NULL || strlen(ans)==0)
        {
            printf("\033[31mIncurrect.\033[0m Correct answer: ");
            printword(list[idx],'n');
        }
        else 
        {
            printf("\033[32mCorrect.\033[0m ");
            printword(list[idx],'n');
        }
    }

}

int note(char content[], FILE *fp, char WBWorkDir[], char kernel_dir[])
{
    word all[MAX];
    loadall(all, fp);
    int num=calculate(fp);
    int i, found=0;
    for(i=0;i<num;i++)
    {
        if(strstr(content,all[i].english)!=NULL) {found=1;break;}
    }
    if(found==0) return FAIL;
   char temp_name[200];
   strcpy(temp_name,WBWorkDir); strcat(temp_name,"/.temp");
   FILE *fp_t=fopen(temp_name,"w"); 
   fputs(all[i].note, fp_t);
   fclose(fp_t);
   char comm[200]; strcpy(comm,"vim "); strcat(comm,temp_name);
   int condition=system(comm);
   if(condition==-1) return FAIL;
   fp_t=fopen(temp_name,"r");
   int j=0; 
   do
   {
       char ch=fgetc(fp_t);
       if(feof(fp_t)==0) all[i].note[j]=ch;
       j++;
   }while(feof(fp_t)==0);
   all[i].note[j-2]='\0'; /* Vi在每行末自动追加换行符，换行符个数与行数总相等，因此j-1对应EOF（实际未被写入变量），j-2对应最后一行的换行符 */
   fclose(fp_t);
   condition=remove(temp_name);
   if(condition==-1) return FAIL;
   fclose(fp); condition=remove(kernel_dir); if(condition==-1) return FAIL;
   FILE *newf=fopen(kernel_dir,"wb");
   for(i=0;i<num;i++) fwrite(&(all[i]), sizeof(word), 1, newf);
   fclose(newf);
   return SUCCESS;
}

int kernel_list(char KernelListFile_name[], char kernel_dir[], char param[],  char content[])
{
    if(strstr(param, "a")!=NULL)
    {
        char st_kernel_dir[STR_MAX];  /* stored kernel's directory */
        if(strcmp(content, "this")==0) strcpy(st_kernel_dir, kernel_dir);
        else if(strcmp(content, "")!=0) strcpy(st_kernel_dir, content);
        else return FAIL;
        convert_dir(st_kernel_dir);

        char *after=kernel_dir;
        for(; *after!='.'; after++);
        char *before;
        for(before=after; *before!='/'; before--); before++;
        char kernel_name[STR_MAX];
        int i=0;
        for(; before!=after; before++, i++)
        {
            kernel_name[i]=*before;
        }
        kernel_name[i]='\0';

        FILE *KernelListFile_fp=fopen(KernelListFile_name, "a");
        fputs(kernel_name, KernelListFile_fp); fputc('=', KernelListFile_fp); fputs(kernel_dir, KernelListFile_fp);
        fputc('\n', KernelListFile_fp);
        fclose(KernelListFile_fp);
        return SUCCESS;
    }
    else if(strstr(param, "s")!=NULL)  /* s means "show" */
    {
        FILE *KernelListFile_fp=fopen(KernelListFile_name, "r");
        do
        {
            int i=0;
            char aline[STR_MAX]; /* a line of the file */
            do
            {
                aline[i]=fgetc(KernelListFile_fp);
                i++;
            }while(!feof(KernelListFile_fp) && aline[i-1]!='\n');
            if(feof(KernelListFile_fp)!=0) continue;
            aline[i-1]='\0';

            char name[STR_MAX], dir[STR_MAX];
            char *p=aline;
            for(; *p!='='; p++);
            *p='\0';
            strcpy(name, aline);
            p++; strcpy(dir, p);
            printf("%10s  %s\n", name, dir);
        }while(!feof(KernelListFile_fp));
        fclose(KernelListFile_fp);
        return SUCCESS;
     }
     else if(strstr(param, "d")!=NULL) /* d means "delete */
     {
        char temp_dir[50];
        strcpy(temp_dir, KernelListFile_name);
        char *p;
        for(p=temp_dir; *p!='\0'; p++);
        for(; *p!='/'; p--);
        *p='\0';
        strcat(temp_dir, "/.temp");

        FILE *origin_fp=fopen(KernelListFile_name, "r");
        FILE *new_fp=fopen(temp_dir, "w");
        do
        {
            char name[STR_MAX];
            char ch=0; int i=0;
            do
            {
                ch=fgetc(origin_fp);
                name[i]=ch;
                i++;
            }while(ch!='=' && !feof(origin_fp));
            if(feof(origin_fp)!=0) continue;
            name[i-1]='\0';
            if(strcmp(name, content)==0) continue;
            else
            {
                fputs(name, new_fp);
                fputc('=', new_fp);
                do
                {
                    ch=fgetc(origin_fp);
                    fputc(ch, new_fp);
                }while(ch!='\n');
            }
        }while(!feof(origin_fp));
        fclose(origin_fp); fclose(new_fp);
        remove(KernelListFile_name);
        rename(temp_dir, KernelListFile_name);
        return SUCCESS;
     }
    else
    {
        return FAIL;
    }
}

void help()
{
    printf("基础命令如下：\n");
    printf("    %-20s 用于计数内核中全部单词。无参数。\n","calculate");
    printf("    %-20s 用于向内核添加单词。\n","addword/add");
    printf("    %-20s 用于对已添加的单词进行注释。\n","note");
    printf("    %-20s 用于删除内核中已经存在的单词。\n","delword/del");
    printf("    %-20s 用于查询内核中的单词。\n","search");
    printf("    %-20s 用于列出满足特定条件的单词。\n","list");
    printf("    %-20s 练习功能。\n","practice/practise");
    printf("    %-20s 用于内核管理。\n","kernel");
    printf("键入“命令 -h”或“命令 --help”获取更多详情\n");
}
/*Upon: Operating Functions*/

/*Below: File managing Functions*/

void rc_init(char WBWorkDir[], char AcquiesentKernelDir[], char CurrentKernelStoreFile_name[], char KernelListFile_name[])
{
    FILE *BashRec=popen("cd ~ && pwd", "r");
    fgetstr(WBWorkDir, 199, BashRec);
    fclose(BashRec);

    strcat(WBWorkDir, "/WordBank");

    strcpy(AcquiesentKernelDir,WBWorkDir);
    strcat(AcquiesentKernelDir,"/kernel.wb");

    strcpy(CurrentKernelStoreFile_name,WBWorkDir);
    strcpy(KernelListFile_name, WBWorkDir);
    strcat(CurrentKernelStoreFile_name, "/current_kernel_dir.txt");
    strcat(KernelListFile_name, "/kernel_list.txt");

    FILE *CurrentKernelStoreFile_fp=fopen(CurrentKernelStoreFile_name, "r+");
    if(CurrentKernelStoreFile_fp==NULL)
    {
        system("cd ~ && mkdir WordBank");
        CurrentKernelStoreFile_fp=fopen(CurrentKernelStoreFile_name, "w+");
    }
    fclose(CurrentKernelStoreFile_fp);

    FILE *KernelListFile_fp=fopen(KernelListFile_name, "r+");
    if(KernelListFile_fp==NULL)
    {
        KernelListFile_fp=fopen(KernelListFile_name, "w+");
    }
    fclose(KernelListFile_fp);
}

void get_kernel(char CurrentKernelStoreFile_name[], char AcquiesentKernelDir[], char kernel_dir[])
{
    FILE *CurrentKernelStoreFile_fp=fopen(CurrentKernelStoreFile_name, "r+");
    fgetc(CurrentKernelStoreFile_fp); fseek(CurrentKernelStoreFile_fp,-1,1);
    if(feof(CurrentKernelStoreFile_fp))
    {
        printf("Kernel path not found. Initialize the path (end with /kernel.wb) or input \".\": ");
        getstr(kernel_dir, NULL);
        if(strcmp(kernel_dir,".")==0)
        {
            strcpy(kernel_dir,AcquiesentKernelDir);
        }
        fputs(kernel_dir, CurrentKernelStoreFile_fp);
    }

    rewind(CurrentKernelStoreFile_fp);
    fgetstr(kernel_dir,199,CurrentKernelStoreFile_fp);
    fclose(CurrentKernelStoreFile_fp);
}

int main(void)
{
    printf("\n\033[1m------------------------------------------------------------\n------------------------------------------------------------\033[0m\n");
    printf("                     \033[7;1m WordBank v0.6 \033[0m\n\n\033[1mCopyright 2024 Xiangnan Zhang\nSchool of Future Technology, Beijing Institute of Technology\n");
    printf("------------------------------------------------------------\n------------------------------------------------------------\033[0m\n\n");
    
    char WBWorkDir[200], AcquiesentKernelDir[200], CurrentKernelStoreFile_name[200], KernelListFile_name[200];
    rc_init(WBWorkDir, AcquiesentKernelDir, CurrentKernelStoreFile_name, KernelListFile_name);

    char kernel_dir[200];
    get_kernel(CurrentKernelStoreFile_name, AcquiesentKernelDir, kernel_dir);

    FILE *fp=fopen(kernel_dir,"rb+");
    if(fp==NULL) fp=fopen(kernel_dir, "wb+");
    
    int number=calculate(fp);
    printf("Current kernel directory: %s\n",kernel_dir);
    printf("Current number of words: %d\n",number);
    fclose(fp);
    
    char str_list[50][STR_MAX];
    int init_idx=0;
    for(; init_idx<50; init_idx++) str_list[init_idx][0]='\0';

    while(1)
    {
        int str_counter=0;
        for(; str_list[str_counter][0]!='\0'; str_counter++);
        if(str_counter==40)  /* keep the length of str_list less than 40 */
        {
            int move_idx=0;
            for(; move_idx<str_counter; move_idx++) strcpy(str_list[move_idx], str_list[move_idx+1]);
        }
                      
        int type;
        char str[150];
        printf(">>> ");
        getstr(str, str_list);

        if(strcmp(str,"exit")==0 || strcmp(str,"Exit")==0) break;
        else
        {
            FILE *fp=fopen(kernel_dir,"rb+");
            char param[10], content[300];
            type=function_detector(str,param,content);

            switch(type)
            {
                case 1:  /*add word*/
                {
                    if(strcmp(param,"-h")==0 || strcmp(param,"--help")==0)
                    {
                        printf("该命令用于向内核添加单词。\n");
                        printf("语法：addword\\add [English] [class].[中文]\n");
                        continue;
                    }
                    if(fp==NULL) fp=fopen(kernel_dir,"wb+");
                    int before=calculate(fp);
                    int condition=addword(content,fp, kernel_dir);
                    if(condition==FP_CLOSED) fp=fopen(kernel_dir, "rb");
                    if(condition==FAIL)
                    {
                        printf("\033[31mERROR: addword failed.\033[0m Command format:\naddword [English] [class].[Chinese]\n");
                    }
                    else
                    {
                        int after=calculate(fp);
                        printf("%d -> %d\n",before,after);
                    }
                } break;

                case 2:  /*delete word*/
                {
                    if(strcmp(param,"-h")==0 || strcmp(param,"--help")==0)
                    {
                        printf("该命令用于删除内核中已经存在的单词。\n");
                        printf("语法：delword [English/中文]\n");
                        continue;
                    }
                    if(fp==NULL) 
                    {
                        printf("\033[31mERROR: kernel start failed.\033[0m\n");
                        continue;
                    }
                    int before=calculate(fp);

                    if(before==0)
                    {
                        printf("\033[31mERROR: kernel is empty.\033[0m\n");
                        continue;
                    }

                    int condition=delword(content,fp, kernel_dir);
                    if(condition==FAIL)
                    {
                        printf("\033[31mERROR: delete word failed.\033[0m Command format:\ndelword [English or Chinese]\n");
                        continue;
                    }

                    fp=fopen(kernel_dir,"rb");
                    int after=calculate(fp);
                    printf("%d -> %d\n",before,after);
                } break;

                case 3:  /*search word*/
                {
                    if(strcmp(param,"-h")==0 || strcmp(param,"--help")==0)
                    {
                        printf("该命令用于查询内核中的单词\n");
                        printf("语法：search [English/中文]\n");
                        continue;
                    }
                    if(fp==NULL) 
                    {
                        printf("\033[31mERROR: kernel start failed.\033[0m\n");
                        continue;
                    }
                    char cont[200];
                    strcpy(cont,content);
                    int condition=search(content,fp);
                    if(condition==FAIL) printf("\033[31mERROR: search failed.\033[0m No word is related to \"%s\". Command format:\nsearch [English or Chinese]\n",cont);
                } break;

                case 4:  /*list*/
                {
                    if(strcmp(param,"-h")==0 || strcmp(param,"--help")==0)
                    {
                        printf("该命令用于列出满足特定条件的单词。\n");
                        printf(    "%35s  用于列出内核中全部单词\n","list -a/--all");
                        printf(    "%35s  按词性列出单词\n","list -c [class]");
                        printf(    "%35s  按添加日期列出单词\n","list -d [year].[month].[day]");
                        printf("%35s  列出x天前添加的单词\n", "list -d p[x]");
                        continue;
                    }
                    if(fp==NULL) 
                    {
                        printf("\033[31mERROR: kernel start failed.\033[0m\n");
                        continue;
                    }
                    listall(fp,param,content);
                } break;

                case 5:  /*calculate word number*/
                {
                    if(strcmp(param,"-h")==0 || strcmp(param,"--help")==0)
                    {
                        printf("此命令无参数与值。\n");
                        continue;
                    }

                    if(fp==NULL) 
                    {
                        printf("\033[31mERROR: kernel start failed.\033[0m\n");
                        continue;
                    }
                    int num=calculate(fp);
                    printf("Number of words: %d\n",num);
                } break;

                case 6: /*kernel*/
                {
                    if(strcmp(param,"-c")==0)
                    {
                        if(strlen(content)==0) 
                        {
                            printf("\033[31mERROR: kernel directory cannot be empty\033[0m\n");
                            continue;
                        }
                        fclose(fp);

                        if(strstr(content, "/")==NULL)
                        {
                            char checked_dir[STR_MAX];
                            int check_result=check_kernel_list(KernelListFile_name, content, checked_dir);
                            if(check_result==FAIL || checked_dir[0]=='\0')
                            {
                                printf("\033[31mError: kernel not found.\n\033[0m");
                                continue;
                            }
                            else strcpy(kernel_dir, checked_dir);
                        }
                        else if(strcmp(content,".")==0)
                        {
                            strcpy(kernel_dir,AcquiesentKernelDir);
                        }
                        else 
                        {
                            strcpy(kernel_dir,content);
                            convert_dir(kernel_dir);
                        }

                        FILE *CurrentKernelStoreFile_fp=fopen(CurrentKernelStoreFile_name,"w");
                        fputs(kernel_dir, CurrentKernelStoreFile_fp);
                        fclose(CurrentKernelStoreFile_fp);

                        fp=fopen(kernel_dir,"rb+");
                        if(fp==NULL) fp=fopen(kernel_dir,"wb+");

                        int num=calculate(fp);
                        printf("Current kernel's number of words: %d\n",num);
                    }
                    else if(strcmp(param,"-p")==0)
                    {
                        printf("%s\n",kernel_dir);
                    }
                    else if(strcmp(param,"-h")==0 || strcmp(param,"--help")==0)
                    {
                        printf("kernel -p：打印当前内核所在路径。\n");
                        printf("kernel -c [path]：更换内核。其中的[path]可为内核列表中的内核名，也可为完整的内核目录\n");
                        printf("使用参数-l并后跟一补充参数进行内核列表相关操作。补充参数：\n");
                        printf("    a: 代表append，给内核列表追加内核目录。值为this时自动追加当前目录，否则以值作为目录。例如：kernel -la this\n");
                        printf("    s: 代表show, 打印当前内核列表。无值。例如：kernel -ls\n");
                        printf("    d：代表delete，删除值对应的内核目录。例如：kernel -ld kernelname\n");
                        printf("注意：参数-l不能单独使用。\n");
                        continue;
                    }
                    else if(strstr(param, "l")!=NULL)
                    {
                        int condition=kernel_list(KernelListFile_name, kernel_dir, param, content);
                        if(condition==FAIL) printf("\033Error.\033[0m Use 'a', 's' or 'd' after parameter 'l'.\n");
                    }
                    else printf("\033[31mInvalid parameter.\033[0m Available:\n-c: change kernel path\n-p: print current kernel path\n");
                } break;

                case 7:  /*practice*/
                {
                    if(strcmp(param,"-h")==0 || strcmp(param,"--help")==0)
                    {
                        printf("练习功能，按特定条件随机给出中文或英文，用户输入对应的英文或中文，自动判断用户回答是否正确，并给出正确答案。\n");
                        printf("参数说明：\n");
                        printf("    -c：给出中文，输入英文。\n    -e：给出英文，输入中文\n    -l：乱序逐个给出中文或英文\n    c或e可与l搭配使用，不要求结合顺序\n");
                        printf("值说明：\n");
                        printf("    [year].[month].[day]：从在指定日期加入的单词中选词。\n    all：从内核所有单词中选词。\n    p[x]：从x天前加入的单词中选词。\n");
                        printf("当无参数时，默认给出中文；无值时，默认从整个内核中选词。\n");
                        continue;
                    }
                    practice(fp, param, content);
                } break;

                case 8:
                {
                    help();
                } break;

                case 9:
                {
                    int condition=upgrade(content);
                    if(condition==FAIL) printf("\033[31mUpdate Kernel Error:\033[0m directories should be declared.\n");
                } break;

                case 10:  /*note*/
                {
                    int condition=note(content, fp, WBWorkDir, kernel_dir);
                    if(strcmp(param,"-h")==0 || strcmp(param,"--help")==0)
                    {
                        printf("用于为已添加的单词进行注释。\n");
                        printf("用法：note [English]\n");
                        printf("命令执行后将在vi内进行编辑。保存并推出vi界面后，可通过search查看注释。对曾进行注释的单词执行note命令时，原有注释不会被清空，用户可在原有注释的基础上进行修改。\n");
                        continue;
                    }
                    if(condition==FAIL)
                    {
                        printf("\033[31mAdd Note Error.\033[0m\n");
                        continue;
                    }
                    fp=fopen(kernel_dir,"rb");
                } break;

                default: 
                {
                    printf("\033[31mCommand Error.\033[0m\n");
                    printf("Available commands: addword, delword, search, list, calculate, practice, kernel, help\n");
                }
            }
            fclose(fp);
        }
    }

    return 0;

}
