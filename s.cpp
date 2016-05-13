#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

FILE *file;
int t;
string str;
vector<string> vec;

const char * dsn = "%d\t\"%s\"\n";
const char * dn = "%d\t\n";
const char * dbn = "%d\t";
int depth = 0;

#pragma region in out

void printTab() {
	for (int i = 0; i < depth; i++)
		printf("\t");
}
void printHex() {
	printTab();
	for (int i = 0; i < str.length(); i++) {
		
		printf("%2x ", 0xff & (int)str[i]);
	}
	printf("\n");
}
void printDec() {
	printTab();
	for (int i = 0; i < str.length(); i++) {
		printf("%d ", 0xff & (int)str[i]);
	}
	printf("\n");
}

void printDecln() {
	printTab();
	for (int i = 0; i < str.length(); i++) {
		if (i % 7 == 0 && i != 0)
		{
			printf("\n");
			printTab();
		}
		printf("%03d ", 0xff & (int)str[i]);
	}
	printf("\n");
}
void printDecnn() {
	printTab();
	for (int i = 0; i < str.length(); i++) {
		printf("%d ", 0xff & (int)str[i]);
	}
}

void read(int count) {
	str = "";
	while (count-->0)
		str += fgetc(file);
}
int readd(int count) {
	str = "";
	int i = 0;
	for (; i < count; i++)
		str += (char)fgetc(file);
	int res = 0;
	for (i = 0; i < count; i++)
		res = res * 0xff + 0xff & (int)str[i];
	return res;
}

#pragma endregion

#pragma region all



void info(int c) {
	read(c);
	printDecln();
}

void attribute_info(int c) {
	depth++;
	for (int i = 0; i < c; i++) {
		//u2 attribute_name_index;
		printTab();
		printf("attribute_name_index ");
		int res = readd(2);
		printTab();
		printf(dsn, res, vec[res - 1].c_str());
		//u4 attribute_length;
		printTab();
		printf("attribute_length ");
		res = readd(4);
		printTab();
		printf(dn, res);
		//u1 info[attribute_length];
		printTab();
		printf("info\n");
		info(res);
	}
	depth--;
}

void field_info(int c) {
	depth++;

	for (int i = 0; i < c; i++) {
		//u2 access_flags;
		printTab();
		printf("access_flags ");
		read(2);
		printDec();
		//u2 name_index;
		printTab();
		printf("name_index ");
		int res = readd(2);
		printTab();
		printf(dsn, res, vec[res - 1].c_str());
		//u2 descriptor_index;
		printTab();
		printf("descriptor_index ");
		res = readd(2);
		printTab();
		printf(dsn, res, vec[res - 1].c_str());
		//u2 attributes_count;
		printTab();
		printf("descriptor_index ");
		res = readd(2);
		printTab();
		printf(dsn, res, vec[res - 1].c_str());
		//attribute_info attributes[attributes_count];
		printTab();
		printf("attribute_info\n");
		attribute_info(res);
	}
	depth--;
}

void method_info(int c) {
	depth++;
	for (int i = 0; i < c; i++) {
		//u2 access_flags;
		printTab();
		printf("access_flags ");
		read(2);
		printDec();
		//u2 name_index;
		printTab();
		printf("name_index ");
		int res = readd(2);
		printTab();
		printf(dsn, res, vec[res-1].c_str());
		//u2 descriptor_index;
		printTab();
		printf("descriptor_index ");
		res = readd(2);
		printTab();
		printf(dsn, res, vec[res-1].c_str());
		//u2 attributes_count;
		printTab();
		printf("attributes_count ");
		res = readd(2);
		printTab();
		printf(dn, res);
		//attribute_info attributes[attributes_count];
		printTab();
		printf("attribute_info\n");
		attribute_info(res);
	}
	depth--;
}

void cp_info(int c) {
	int res;
	for (int i = 1; i < c;i++) {
		res = readd(1);
		printf("%d:\t%d\t",i, res);
		switch (res) {
		case 0x7:
			res = readd(2);
			printf(dn, res);
			//char b[4];
			//itoa(res, b, 10);
			str = res;
			break;
		case 0xa:
			res = readd(2);
			printf(dbn, res);
			res = readd(2);
			printf(dn, res);
			break;
		case 0x1:
			res = readd(2);
			read(res);
			printf("%s\n", str.c_str());
			break;
		case 0x9:
			read(4);
			printDec();
			break;
		case 0x8:
			read(2);
			printDec();
			break;
		case 0x3:
			res = readd(2);
			printf(dbn, res);
			res = readd(2);
			printf(dn, res);
			break;
		case 0xc:
			res = readd(2);
			printf(dbn, res);
			res = readd(2);
			printf(dn, res);
			break;
		case 6:
			read(8);
			printDec();
			vec.push_back("double");
			i++;
			break;
		default:
			break;
		}
		vec.push_back(str);
	}
}

#pragma endregion

void ClassFile() {
	int res;
	//u4 magic;
	read(4);
	printHex();
	//u2 minor_version;
	printf("minor_version ");
	res = readd(2);
	printf(dn, res);
	//u2 major_version;
	printf("major_version ");
	res = readd(2);
	printf(dn, res);
	//u2 constant_pool_count;
	int constant_pool_count = readd(2);
	printf("constant_pool_count= %d\n", constant_pool_count);
	//cp_info constant_pool[constant_pool_count - 1];
	cp_info(constant_pool_count);
	//u2 access_flags;
	printf("access_flags ");
	res = readd(2);
	printHex();
	//u2 this_class;
	printf("this_class ");
	res = readd(2);
	printf(dsn, res, vec[(int )vec[res-1].c_str()[0]-1].c_str());
	//u2 super_class;
	printf("super_class ");
	res = readd(2);
	printf(dsn, res, vec[(int)vec[res - 1].c_str()[0] - 1].c_str());
	//u2 interfaces_count;
	printf("interfaces_count ");
	res = readd(2);
	printf(dn, res);
	//u2 interfaces[interfaces_count];
	readd(res * 2);
	//u2 fields_count;
	printf("fields_count ");
	res = readd(2);
	printf(dn, res);
	//field_info fields[fields_count];
	printf("field_info\n");
	field_info(res);
	//u2 methods_count;
	printf("methods_count ");
	res = readd(2);
	printf(dn, res);
	//method_info methods[methods_count];
	printf("method_info\n");
	method_info(res);
	//u2 attributes_count;
	printf("attributes_count ");
	res = readd(2);
	printf(dn, res);
	//attribute_info attributes[attributes_count];
	printf("attribute_info\n");
	attribute_info(res);
}

void readALL(int c) {
	int i = 0,cc = 0;
	while(i++ < c ){
		cc = fgetc(file);
		if (cc == -1) break;
		printf(dbn,cc);
		if (i % 7 == 0) printf("\n");
	}
}

int main(int argc,char** argv) {
	if(argc > 1)
	file = fopen(argv[1], "rb");
	if (file != nullptr) {
		ClassFile();
		//printf("\nmain\n");
		//method_info(1);
		printf("\n something left \n");
		readALL(200);
		fclose(file);
	}
	else
		printf("file not exits!");
	getchar();
}

