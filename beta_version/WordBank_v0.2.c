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

typedef struct 
{
    char english[30];
    char class[5];
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

    if(strcmp(command,"addword")==0) return 1;
    else if(strcmp(command,"delword")==0) return 2;
    else if(strcmp(command,"searchword")==0 || strcmp(command,"search")==0) return 3;
    else if(strcmp(command,"list")==0 || strcmp(command,"listall")==0) return 4;
    else if(strcmp(command,"calculate")==0) return 5;
    else if(strcmp(command, "kernel")==0) return 6;
    else if(strcmp(command, "practice")==0 || strcmp(command, "practise")==0) return 7;
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
    strcpy(new.english,str);

    for(p++;*p==' ';p++);
    if(*p=='\0') {return FAIL;}

    char *pb;
    for(pb=p;*pb!='.';pb++);
    if(*pb=='\0') {return FAIL;}
    *pb='\0';
    strcpy(new.class,p);

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
        struct tm *t=localtime(&(all[tape].timer));
        printf("%d.%d.%d    %s %s.%s\n",t->tm_year+1900, t->tm_mon+1, t->tm_mday, all[tape].english,all[tape].class,all[tape].chinese);
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

    if(strcmp(param,"-all")==0 || strcmp(param, "-a")==0)
    {
        int i;
        for(i=0;i<num;i++)
        {
            struct tm *t=localtime(&(all[i].timer));
            printf("%4d.%2d.%2d    %20s %s.%s\n",t->tm_year+1900, t->tm_mon+1, t->tm_mday, all[i].english, all[i].class, all[i].chinese);
        }
    }
    else if(strcmp(param, "-date")==0 || strcmp(param, "-d")==0)
    {
        int year, month, day;
        int condition=date_split(content, &year, &month, &day);
        if(condition==FAIL)
        {
            printf("ERROR: date format error.\n");
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
                struct tm *t=localtime(&(list[i].timer));
                printf("%4d.%2d.%2d    %20s %s.%s\n",t->tm_year+1900, t->tm_mon+1, t->tm_mday, list[i].english, list[i].class, list[i].chinese);
            }
        }
    }
    else if(strcmp(param,"-class")==0 || strcmp(param, "-c")==0)
    {
        word list[MAX];
        int i,calc=0;
        for(i=0;i<num;i++)
        {
            if(strcmp(content, all[i].class)==0) 
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
                struct tm *t=localtime(&(list[i].timer));
                printf("%4d.%2d.%2d    %20s %s.%s\n",t->tm_year+1900, t->tm_mon+1, t->tm_mday, list[i].english, list[i].class, list[i].chinese);
            }
        }
    }
    else printf("ERROR: parameter error. Available parameter: -a (-all), -c (-class), -d (-date)\n");
}

void practice(FILE *fp, char param[], char content[])
{
    word all[MAX], list[MAX];
    loadall(all, fp);
    int num=calculate(fp);

    if(strlen(content)==0 || strcmp(content,"all")==0)
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
        if(p==NULL || strlen(ans)==0) printf("Incurrect. Correct answer: %s.%s\n", list[idx].class, list[idx].chinese);
        else printf("Correct. %s %s.%s\n", list[idx].english, list[idx].class, list[idx].chinese);
    }
    else if(strcmp(param, "-h")==0)
    {
        printf("Valid cammand:\npractice [-c (provide Chinese, acquiesent) / -e (provide English)] [year.month.day / all(acquiesent)]\n");
    }
    else
    {
        printf("%s.%s\n",list[idx].class, list[idx].chinese);
        printf("English: ");

        char ans[200];
        getstr(ans,199);

        char *p=strstr(list[idx].english, ans);
        if(p==NULL || strlen(ans)==0) printf("Incurrect. Correct answer: %s\n", list[idx].english);
        else printf("Correct. %s %s.%s\n", list[idx].english, list[idx].class, list[idx].chinese);
    }

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
    printf("\n------------------------------------------------------------\n------------------------------------------------------------\n");
    printf("                      WordBank v0.2\n\nCopyright 2024 Xiangnan Zhang\nSchool of Future Technology, Beijing Institute of Technology\n");
    printf("------------------------------------------------------------\n------------------------------------------------------------\n\n");
    
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
                    if(fp==NULL) fp=fopen(kernel_dir,"wb+");

                    int before=calculate(fp);
                    int condition=addword(content,fp);
                    if(condition==FAIL)
                    {
                        printf("ERROR: addword failed. Command format:\naddword [English] [class].[Chinese]\n");
                    }
                    else
                    {
                        int after=calculate(fp);
                        printf("%d -> %d\n",before,after);
                    }
                } break;

                case 2:  /*delete word*/
                {
                    if(fp==NULL) 
                    {
                        printf("ERROR: kernel start failed.\n");
                        continue;
                    }
                    int before=calculate(fp);

                    if(before==0)
                    {
                        printf("ERROR: kernel is empty.\n");
                        continue;
                    }

                    int condition=delword(content,fp, kernel_dir);
                    if(condition==FAIL)
                    {
                        printf("ERROR: delete word failed. Command format:\ndelword [English or Chinese]\n");
                        continue;
                    }

                    fp=fopen(kernel_dir,"rb");
                    int after=calculate(fp);
                    printf("%d -> %d\n",before,after);
                } break;

                case 3:  /*search word*/
                {
                    if(fp==NULL) 
                    {
                        printf("ERROR: kernel start failed.\n");
                        continue;
                    }
                    int condition=search(content,fp);
                    if(condition==FAIL) printf("ERROR: search failed. Command format:\nsearch [English or Chinese]\n");
                } break;

                case 4:  /*list*/
                {
                    if(fp==NULL) 
                    {
                        printf("ERROR: kernel start failed.\n");
                        continue;
                    }
                    list(fp,param,content);
                } break;

                case 5:  /*calculate word number*/
                {
                    if(fp==NULL) 
                    {
                        printf("ERROR: kernel start failed.\n");
                        continue;
                    }
                    int num=calculate(fp);
                    printf("Number of words: %d\n",num);
                } break;

                case 6: /*kernel*/
                {
                    if(strcmp(param,"-c")==0)
                    {
                        fclose(fp);
                        strcpy(kernel_dir,content);
                        FILE *CurrentKernelStoreFile_fp=fopen(CurrentKernelStoreFile_name,"w");
                        fputs(kernel_dir,CurrentKernelStoreFile_fp);
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
                    else printf("Invalid parameter. Available:\n-c: change kernel path\n-p: print current kernel path\n");
                } break;

                case 7:  /*practice*/
                {
                    practice(fp, param, content);
                } break;

                default: printf("Command Error. Available commands:\naddword\ndelword\nsearch\nlist\ncalculate\npractice\nkernel\n");
            }
            fclose(fp);
        }
    }

    return 0;

}