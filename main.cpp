//根据groundtruth统计正确率
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define MAX_WORDS 512

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

void print_help();

void getLine(FILE* fin, float fLength, char* buffer, int& len);

//用于统计字符级别的正确率
void DPOneLine(char truth[], char rec[],int truNum,int recNum,int result[4]);

int main(int argc, char** argv)
{
	char* truth = argv[1];
	char* src = argv[2];

	if (argc == 3)
	{
		FILE* infile_1 = NULL; 
		infile_1 = fopen(truth, "rb");

		FILE* infile_2 = NULL;
		infile_2 = fopen(src, "rb");

		if (infile_1 == NULL)
		{
			printf("truth file open failure!\n");
			return 0;
		}
		if (infile_2 ==  NULL)
		{
			printf("src file open failure!\n");
			return 0;
		}

		float fLength_1, fLength_2;
		fseek(infile_1, 0, SEEK_END);
		fseek(infile_2, 0, SEEK_END);

		fLength_1 = ftell(infile_1);
		fLength_2 = ftell(infile_2);

		fseek(infile_1, 0, SEEK_SET);
		fseek(infile_2, 0, SEEK_SET);

		char buffer_1[MAX_WORDS * 2];

		char buffer_2[MAX_WORDS * 2];

		float all_crt = 0;

		//for char_level statical 
		float my_total_counter = 0;//没有被拒识的字符串中包含字符的总数
		float my_de_counter = 0;
		float my_se_counter = 0;
		float my_ie_counter = 0;

		int file_number = 0;

		char temp_char_1 = fgetc(infile_1);
		char temp_char_2 = fgetc(infile_2);


		while(!feof(infile_1) && !feof(infile_2))//go accoss the total text.
		{
			fseek(infile_1, -1, SEEK_CUR);
			fseek(infile_2, -1, SEEK_CUR);

			int temp_trunum, temp_recnum;

			memset(buffer_1, 0, MAX_WORDS * 2);
			getLine(infile_1, fLength_1, buffer_1, temp_trunum);

			memset(buffer_2, 0, 1024);
			getLine(infile_2, fLength_2, buffer_2, temp_recnum);
			
			if (temp_recnum % 2 != 0 || temp_trunum % 2 != 0)
			{
				printf("failure!\n");
				system("pause");
			}

			if (temp_trunum % 2 != 0 || temp_recnum % 2 != 0)
			{
				printf("something is wrong!\n");
				system("pause");
			}

			temp_trunum = temp_trunum / 2;
			temp_recnum = temp_recnum / 2;

			//字符级别的统计//拒识的时候不参与统计
			int temp_result[4];
			memset(temp_result, 0, 4 * sizeof(int));
			DPOneLine(buffer_1, buffer_2, temp_trunum, temp_recnum, temp_result);

			if (memcmp(buffer_1, buffer_2, temp_trunum * 2) == 0 && 
				memcmp(buffer_1, buffer_2, temp_recnum * 2) == 0)
			{
				all_crt+=1;
			}
			//cout<<memcmp((char*)con_1.c_str(), (char*)con_2.c_str(), temp_trunum)<<endl;

			//system("pause");


			my_total_counter += temp_trunum;
			my_de_counter += temp_result[1];
			my_ie_counter += temp_result[2];
			my_se_counter += temp_result[3];

			//cout<<"file_number: "<<file_number<<endl;

			file_number++;

			temp_char_1 = fgetc(infile_1);
			temp_char_2 = fgetc(infile_2);
		}

		fclose(infile_1);
		fclose(infile_2);

		cout<<"total char: "<<my_total_counter<<" lineNum: "<<file_number<<endl;

		printf("all = %.2f\n", all_crt * 100.0 / file_number);

		//字符级别的统计
		printf("CR=%.2f, AR=%.2f\n",(my_total_counter - my_de_counter - my_se_counter) * 100.0 / my_total_counter, 
			(my_total_counter - my_de_counter - my_se_counter - my_ie_counter) * 100.0 / my_total_counter);

	}
	else
	{
		print_help();
	}

	return 0;
	
}

void getLine(FILE* fin, float fLength, char* buffer, int& len)
{
	float pos_offset = min( fLength - ftell(fin), MAX_WORDS * 2 );
	fread( buffer, pos_offset, 1, fin );

	int temp_mark = -1;
	int nLen = pos_offset;

	for(int i=0; i < nLen; i += 2)
	{
		if( buffer[i] == 0x0D && buffer[i + 1] ==0x0A )
		{
			temp_mark = i;
			break;
		}
	}
	if( temp_mark == -1 )
	{
		cout<<"fail to retrieve the symbol 0x0D0A "<<endl;
		system( "pause" );
	}
	else
	{
		for (int i = temp_mark; i < MAX_WORDS * 2; i++)
		{
			buffer[i] = 0;
		}
		fseek(fin, -1 * (pos_offset - (temp_mark + 2) ), SEEK_CUR);

		len = temp_mark;
	}
}

void print_help()
{
	printf( "   correct_std crt\n" );
	printf( "   argv[1]: filename of truth (please do not forget .txt)\n" );
	printf( "   argv[2]: filename of src (please do not forget .txt)\n" );
}

/***************************************************************************
功能：利用动态规划匹配来获得其字符串对准结果
参数：truth是标准文本
	  rec是识别文本
	  result[0]:正确字数
      result[1]:删除字数
	  result[2]:插入字数
	  result[3]:替换字数
***************************************************************************/
void DPOneLine(char truth[], char rec[],int truNum,int recNum,int result[4])
{
	int i,j;
	const int InsC=1;//3;2011-3-1
	const int DelC=1;//3;
	const int RepC=1;//5;
	struct NODE{
		int value;
		NODE *parent;
		int Cr,De,Re,Ie;
	};
	NODE **nodes; 
	NODE tmpNd;
	nodes= new NODE *[truNum+1];
	for(i=0;i<truNum+1;i++)
	{
		nodes[i]=new NODE[recNum+1];
		memset(nodes[i],0,sizeof(NODE)*(recNum+1));
	}
	nodes[0][0].parent=NULL;
	nodes[0][0].value=0;
	for(i=1;i<truNum+1;i++)
	{
		nodes[i][0].value=i;//1000;2012-3-12
		nodes[i][0].parent=NULL;
		nodes[i][0].De=i;//2011-6-12
	}
	for(i=1;i<recNum+1;i++)
	{
		nodes[0][i].value =i;//1000;2012-3-12
		nodes[0][i].parent=NULL;
		nodes[0][i].Ie=i;//2011-6-12
	}
	int tmpValue;
	for(i=1;i<truNum+1;i++)
	{
		for(j=1;j<recNum+1;j++)
		{
			if(!memcmp(&truth[2 * i - 2],&rec[2 * j - 2],2))//比较内存，若相等//已经转为半角字符
			{
				tmpValue=0;
				tmpNd.Cr=nodes[i-1][j-1].Cr+1;//正确个数加1
				tmpNd.De=nodes[i-1][j-1].De;
				tmpNd.Re=nodes[i-1][j-1].Re;
				tmpNd.Ie=nodes[i-1][j-1].Ie;
			}
			else
			{
				tmpValue=RepC;
				tmpNd.Cr=nodes[i-1][j-1].Cr;
				tmpNd.De=nodes[i-1][j-1].De;
				tmpNd.Re=nodes[i-1][j-1].Re+1;//替代误差
				tmpNd.Ie=nodes[i-1][j-1].Ie;
			}
			tmpNd.value=tmpValue+nodes[i-1][j-1].value;
			tmpNd.parent=&nodes[i-1][j-1];
			if(tmpNd.value>=nodes[i][j-1].value+InsC)
			{
				tmpNd.parent=&nodes[i][j-1];
				tmpNd.value=nodes[i][j-1].value+InsC;
				tmpNd.Cr=nodes[i][j-1].Cr;
				tmpNd.De=nodes[i][j-1].De;
				tmpNd.Re=nodes[i][j-1].Re;
				tmpNd.Ie=nodes[i][j-1].Ie+1;//插入误差
			}
			if(tmpNd.value>=nodes[i-1][j].value+DelC)//>=2011-3-1
			{
				tmpNd.parent=&nodes[i-1][j];
				tmpNd.value=nodes[i-1][j].value+DelC;
				tmpNd.Cr=nodes[i-1][j].Cr;
				tmpNd.De=nodes[i-1][j].De+1;//删除误差
				tmpNd.Re=nodes[i-1][j].Re;
				tmpNd.Ie=nodes[i-1][j].Ie;
			}
			nodes[i][j]=tmpNd;
		}
	}
	result[0]=nodes[truNum][recNum].Cr;
	result[1]=nodes[truNum][recNum].De;
	result[2]=nodes[truNum][recNum].Ie;
	result[3]=nodes[truNum][recNum].Re;
	for(i=0;i<truNum+1;i++)
	{
		delete []nodes[i];
	}
	delete []nodes;
}
