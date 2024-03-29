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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define SUCCESS 1
#define FAIL 0
#define MAX 1000
#define CLASSMAX 5

typedef struct 
{
    char english[30];
    char class[CLASSMAX][CLASSMAX];
    char chinese[80];
    time_t timer;
} word;

/*Bellow: Assistant Functions*/

void getstr(char *str, int len)
{
    fgets(str, len, stdin);
    char *p=str;
    for(; *p!='\n' && *p!='\0'; p++);
    *p='\0';
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
        printf("%20s ",w.english);
    }
    else if(ch=='n') printf("%s ",w.english);
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

/*Upon: Assistant Functions*/

/*Below: Operating Functions*/

int function_detector(char str[], char param[], char content[])  /*return type and cut str*/
{
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
            if(*second=='-') 
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

int addword(char str[], FILE *fp) /*1*/
{
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

    new.timer=time(NULL);

    fseek(fp,0,2);
    int condition=fwrite(&new,sizeof(word),1,fp);
    if(condition==0) return FAIL;

    return SUCCESS;
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
        if(str[0]<0) p=strstr(all[i].chinese,str);
        else p=strstr(all[i].english,str);
        if(p!=NULL) 
        {
            found=1;
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

    char *p=NULL;
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

    if(p!=NULL)
    {
        printword(all[tape],'y');
        return SUCCESS;
    }
    else
    {
        return FAIL;
    }
}

void list(FILE *fp, char param[], char content[])  /*4*/
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
    word all[MAX], list[MAX];
    loadall(all, fp);
    int num=calculate(fp);

    if(strlen(content)==0 || strcmp(content,"all")==0 || strcmp(content,"")==0)
    {
        int i;
        for(i=0;i<num;i++) list[i]=all[i];
    }
    else
    {
        int year,month,day;
        int condition=date_split(content, &year, &month, &day);
        if(condition==FAIL)
        {
            printf("ERROR: date format error.\n");
            return;
        }

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
            printf("ERROR: No word stored on %d.%d.%d\n", year, month, day);
            return;
        }
    }

    time_t timer=time(NULL);
    unsigned int seed=(unsigned int)timer;
    srand(seed);
    int idx=rand()%num;

    if(strcmp(param,"-e")==0)
    {
        printf("%s\n",list[idx].english);
        printf("Chinese: ");

        char ans[200];
        getstr(ans,199);

        char *p=strstr(list[idx].chinese, ans);
        if(p==NULL || strlen(ans)==0)
        {

            printf("Incurrect. Correct answer: ");
            printword(list[idx],'n');
        }
        else
        {
            printf("Correct. ");
            printword(list[idx],'n');
        }
    }
    else
    {
        printword(list[idx],'e');
        printf("English: ");

        char ans[200];
        getstr(ans,199);

        char *p=strstr(list[idx].english, ans);
        if(p==NULL || strlen(ans)==0)
        {
            printf("Incurrect. Correct answer: ");
            printword(list[idx],'n');
        }
        else 
        {
            printf("Correct. ");
            printword(list[idx],'n');
        }
    }

}

void help()
{
    printf("基础命令如下：\n");
    printf("    %-20s 用于计数内核中全部单词。无参数。\n","calculate");
    printf("    %-20s 用于向内核添加单词。\n","addword/add");
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
        getstr(kernel_dir, 199);
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
    printf("                     \033[7;1m WordBank v0.4 \033[0m\n\n\033[1mCopyright 2024 Xiangnan Zhang\nSchool of Future Technology, Beijing Institute of Technology\n");
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

    while(1)
    {
        int type;
        char str[150];
        printf(">>> ");
        getstr(str,145);

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
                    int condition=addword(content,fp);
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
                        printf("ERROR: kernel start failed.\n");
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
                        printf(    "%-20s 用于列出内核中全部单词\n","list -a/--all");
                        printf(    "%-20s 按词性列出单词\n","list -c [class]");
                        printf(    "%-20s 按添加日期列出单词\n","list -d year.month.day");
                        continue;
                    }
                    if(fp==NULL) 
                    {
                        printf("\033[31mERROR: kernel start failed.\033[0m\n");
                        continue;
                    }
                    list(fp,param,content);
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
                        strcpy(kernel_dir,content);
                        if(strcmp(kernel_dir,".")==0)
                        {
                            strcpy(kernel_dir,AcquiesentKernelDir);
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
                        printf("kernel -c [path]：更换内核。\n");
                        continue;
                    }
                    else printf("\033[31mInvalid parameter.\033[0m Available:\n-c: change kernel path\n-p: print current kernel path\n");
                } break;

                case 7:  /*practice*/
                {
                    if(strcmp(param,"-h")==0 || strcmp(param,"--help")==0)
                    {
                        printf("练习功能，按特定条件随机给出中文或英文，用户输入对应的英文或中文，自动判断用户回答是否正确，并给出正确答案。\n");
                        printf("参数说明：\n");
                        printf("    -c：给出中文，输入英文。\n    -e：给出英文，输入中文\n");
                        printf("值说明：\n");
                        printf("    [year].[month].[day]：从在指定日期加入的单词中选词。\n    all：从内核所有单词中选词。\n");
                        printf("当无参数时，默认给出中文；无值时，默认从整个内核中选词。\n");
                        continue;
                    }
                    practice(fp, param, content);
                } break;

                case 8:
                {
                    help();
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
