#include <math.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <string.h>
#include <unordered_set>
#include <unordered_map>
using namespace std;
long long int disk_size = 524288000, block_size = 8192 , fd_counter = 1;
class disk_info
{
public:
    unordered_set<string> all_files, all_opened_files;
    disk_info()
    {
    }
};
unordered_map<string, disk_info> all_disks;
struct data_block_struct
{
    //char data[block_size];
    long long int curr_size;
    long long int next;
    char data[8192  - 50];
};
//jhg
//hjfyhgfv
//hgyfyfy
struct inode_block_struct
{
    char file_name[30];
    long long int file_size;
    // int mem_arr[10];
    long long int data_block_num;
};
class file_info
{
public:
    string file_name;
    string mode;
    long long int inode_number, file_desc;
    file_info()
    {
    }
};
struct super_block
{
    long long int no_of_inodes;
    long long int no_of_inode_blocks;
    long long int no_of_data_blocks;
    long long int start_inode_bm;
    long long int start_data_bm, start_inode_block, start_data_block;
};

void create_disk(string disk_no)
{
    string disk_name = disk_no;
    all_disks[disk_name] = disk_info();
    char disk_name_char[disk_name.size() + 1];
    strcpy(disk_name_char, disk_name.c_str());
    FILE *fp = fopen(disk_name_char, "w");
    fseek(fp, disk_size - 1, SEEK_SET);
    fputc('\0', fp);
    fseek(fp, 0, SEEK_SET);
    struct super_block *obj = (struct super_block *)malloc(sizeof(struct super_block));

    obj->no_of_inode_blocks = 80;
    obj->no_of_inodes = (80*block_size)/sizeof(struct inode_block_struct);
    obj->no_of_data_blocks = 63900;
    obj->start_inode_bm = 8192;
    obj->start_data_bm = 73728;
    obj->start_inode_block = 139264 ;
    obj->start_data_block = 794624;

    if (fp != NULL)
    {
        fwrite(obj, sizeof(struct super_block), 1, fp);
    }

    long long int te = obj->no_of_inodes;
    char inode_bitmap[te + 1];
    for (long long int i = 0; i < obj->no_of_inodes; i++)
    {
        inode_bitmap[i] = '0';
    }

    fseek(fp, obj->start_inode_bm, SEEK_SET);
    fputs(inode_bitmap, fp);

    te = obj->no_of_data_blocks;
    char data_bitmap[te + 1];
    for (long long int i = 0; i < obj->no_of_data_blocks; i++)
    {
        data_bitmap[i] = '0';
    }

    fseek(fp, obj->start_data_bm, SEEK_SET);
    fputs(data_bitmap, fp);

    fclose(fp);
    free(obj);
}

void mount_disk(string para)
{
    string disk_name = para;
    if (all_disks.find(disk_name) == all_disks.end())
    {
        cout << "no such disk" << endl;
        return;
    }
    char disk_name_char[disk_name.size() + 1];
    strcpy(disk_name_char, disk_name.c_str());
    FILE *fp = fopen(disk_name_char, "r+");
    struct super_block *super_block_obj = (struct super_block *)malloc(sizeof(struct super_block));
    fseek(fp, 0, SEEK_SET);
    fread(super_block_obj, sizeof(struct super_block), 1, fp);

    /*cout << super_block_obj->no_of_data_blocks << endl;
    cout << super_block_obj->no_of_inode_blocks << endl;
    cout << super_block_obj->no_of_inodes << endl;
    cout << super_block_obj->start_data_bm << endl;
    cout << super_block_obj->start_inode_bm << endl; */
    fseek(fp, super_block_obj->start_inode_bm, SEEK_SET);

    string inode_bitmap = "";

    for (long long int i = 0; i < super_block_obj->no_of_inodes; i++)
    {
        char t = (char)fgetc(fp);
        inode_bitmap += t;
    }

    //cout << inode_bitmap << endl;

    fseek(fp, super_block_obj->start_data_bm, SEEK_SET);

    string data_bitmap = "";
    for (long long int i = 0; i < super_block_obj->no_of_data_blocks; i++)
    {
        data_bitmap += (char)fgetc(fp);
    }
    //cout << data_bitmap << endl;

    unordered_map<string, file_info> files_map;
    for (long long int i = 0; i < inode_bitmap.size(); i++)
    {
        if (inode_bitmap[i] == '1')
        {
            long long int curr = super_block_obj->start_inode_block + i * (sizeof(struct inode_block_struct));
            fseek(fp, curr, SEEK_SET);
            struct inode_block_struct *inode_obj = (struct inode_block_struct *)malloc(sizeof(struct inode_block_struct));
            fread(inode_obj, sizeof(struct inode_block_struct), 1, fp);
            string fname = inode_obj->file_name;
            files_map[fname] = file_info();
            files_map[fname].file_desc = -1;
            files_map[fname].file_name = fname;
            files_map[fname].inode_number = i;
            files_map[fname].mode = "-1";
        }
    }

    unordered_map<long long int, string> fd_map;

    while (1)
    {
        cout << "1.create file" << endl;
        cout << "2.open file" << endl;
        cout << "3.read file" << endl;
        cout << "4.write file" << endl;
        cout << "5.append file" << endl;
        cout << "6.close file" << endl;
        cout << "7.delete file" << endl;
        cout << "8.list of files" << endl;
        cout << "9.list of opened files" << endl;
        cout << "10.unmount" << endl;

        int inp;
        cin >> inp;
        if (inp == 1)
        {
            long long int i;
            cout << "enter file name: " << endl;
            string file_name;
            cin >> file_name;
            if (files_map.find(file_name) != files_map.end())
            {
                cout << "file name already exists" << endl;
                cout<<"showing menu" <<endl;
                continue;
            }
            for (i = 0; i < inode_bitmap.size(); i++)
            {
                if (inode_bitmap[i] == '0')
                {
                    break;
                }
            }
            if (i >= inode_bitmap.size())
            {
                cout << "all inodes are full" << endl;
                cout<<"showing menu" <<endl;
                continue;
            }
            inode_bitmap[i] = '1';
            struct inode_block_struct *inode_obj = (struct inode_block_struct *)malloc(sizeof(struct inode_block_struct));

            inode_obj->file_size = 0;
            /*   for(int j=0;j<10;j++){
                    inode_obj->mem_arr[j]=-1;
                }  */
            inode_obj->data_block_num = -1; //new
            for (long long int j = 0; j < file_name.size(); j++)
            {
                inode_obj->file_name[j] = file_name[j];
            }
            inode_obj->file_name[file_name.size()] = '\0';

            long long int curr = super_block_obj->start_inode_block + i * (sizeof(struct inode_block_struct));
            fseek(fp, curr, SEEK_SET);
            if (fp != NULL)
            {
                fwrite(inode_obj, sizeof(struct inode_block_struct), 1, fp);
            }

            files_map[file_name] = file_info();
            files_map[file_name].file_desc = -1;
            files_map[file_name].file_name = file_name;
            files_map[file_name].inode_number = i;
            files_map[file_name].mode = "-1";
        }
        else if (inp == 2)
        {

            /*
            //display
            for (int i = 0; i < inode_bitmap.size(); i++)
            {
                if (inode_bitmap[i] == '1')
                {
                    long long int curr = super_block_obj->start_inode_block + i * (sizeof(struct inode_block_struct));
                    fseek(fp, curr, SEEK_SET);
                    struct inode_block_struct *inode_obj = (struct inode_block_struct *)malloc(sizeof(struct inode_block_struct));
                    fread(inode_obj, sizeof(struct inode_block_struct), 1, fp);
                    cout << inode_obj->file_name << endl;
                    cout << inode_obj->file_size << endl;
                //for(int j=0;j<10;j++){
                //    cout<<inode_obj->mem_arr[j]<<" ";
                //} 
                    cout << inode_obj->data_block_num;
                    cout << endl;
                }
            }

            for (auto i = files_map.begin(); i != files_map.end(); i++)
            {
                cout << i->first << " " << i->second.file_name << " " << i->second.file_desc << " " << i->second.inode_number << " " << i->second.mode << endl;
            }

             */

            string fname;
            cout << "enter file name to open: ";
            cin >> fname;
            if (files_map.find(fname) == files_map.end())
            {
                cout << "no such file found" << endl;
                cout<<"showing menu" <<endl;
                continue;
            }
            if (files_map[fname].file_desc != -1)
            {
                cout << "file is already opened" << endl;
                cout<<"showing menu" <<endl;
                continue;
            }
            cout << "enter mode 1.read 2.write 3.append : ";
            int inp_mode;
            cin >> inp_mode;
            if (inp_mode == 1)
            {
                fd_map[fd_counter] = fname;
                files_map[fname].mode = "read";
                files_map[fname].file_desc = fd_counter;
                cout<<"file opened in read mode with fd= "<<files_map[fname].file_desc<<endl;
                fd_counter++;

            }
            else if (inp_mode == 2)
            {
                fd_map[fd_counter] = fname;
                files_map[fname].mode = "write";
                files_map[fname].file_desc = fd_counter;
                fd_counter++;
                cout<<"file opened in write mode with fd= "<<files_map[fname].file_desc<<endl;
            }
            else if (inp_mode == 3)
            {
                fd_map[fd_counter] = fname;
                files_map[fname].mode = "append";
                files_map[fname].file_desc = fd_counter;
                fd_counter++;
                cout<<"file opened in append mode with fd= "<<files_map[fname].file_desc<<endl;
            }
            else
            {
                cout << "no such mode" << endl;
            }

            // need for continue in no such mode
        }
        else if (inp == 3)
        {
            cout << "enter file descriptor: ";
            long long int inp_fd;
            cin >> inp_fd;
            if (fd_map.find(inp_fd) == fd_map.end())
            {
                cout << "no such file discriptor" << endl;
                cout<<"showing menu" <<endl;
                continue;
            }
            string fnme = fd_map[inp_fd];
            if (files_map[fnme].mode != "read")
            {
                cout << "given fd is not opened in read mode" << endl;
                cout<<"showing menu" <<endl;
                continue;
            }
            


            long long int ind = files_map[fnme].inode_number;
            long long int curr = super_block_obj->start_inode_block + ind * (sizeof(struct inode_block_struct));
            fseek(fp, curr, SEEK_SET);
            struct inode_block_struct *inode_obj = (struct inode_block_struct *)malloc(sizeof(struct inode_block_struct));
            fread(inode_obj, sizeof(struct inode_block_struct), 1, fp);

            long long int dnum=inode_obj->data_block_num;
            if(dnum==-1){
                cout<<"file is empty"<<endl;
                free(inode_obj);
                cout<<"showing menu" <<endl;
                continue;
            }
            while(1){

            long long int curr = super_block_obj->start_data_block + dnum * (sizeof(struct data_block_struct));
            fseek(fp, curr, SEEK_SET);
            struct data_block_struct *dblock_obj = (struct data_block_struct *)malloc(sizeof(struct data_block_struct));
            fread(dblock_obj, sizeof(struct data_block_struct), 1, fp);
            //cout<<dblock_obj->data;
            string temp_s(dblock_obj->data);
            for(long long int i=0;i<temp_s.size();i++){
                if(temp_s[i]=='~'){
                    cout<<"\n";
                }
                else{
                    cout<<temp_s[i];
                }
            }
            dnum=dblock_obj->next;

            free(dblock_obj);

            if(dnum==-1){
                break;
            }

                
            }
            cout<<endl;

            free(inode_obj);
        }
        else if (inp == 4)
        {
            cout << "enter file descriptor: ";
            long long int inp_fd;
            cin >> inp_fd;
            if (fd_map.find(inp_fd) == fd_map.end())
            {
                cout << "no such file discriptor" << endl;
                cout<<"showing menu" <<endl;
                continue;
            }
            string fnme = fd_map[inp_fd];
            if (files_map[fnme].mode != "write")
            {
                cout << "given fd is not opened in write mode" << endl;
                cout<<"showing menu" <<endl;
                continue;
            }
            cout << "enter data to write to file: ";
            string data="";
            while(1){
                string st;
            getline(cin, st);
            
            if(st=="end"){
                break;
            }
            st+="~";
            data+=st;

            }
            data=data.substr(1,data.size()-1);
            data=data.substr(0,data.size()-1);
            //cout<<data<<"sp"<<endl;
            //cout<<data<<"hi"<<endl;

            if (data.size() == 0)
            {
                cout << "no data entered" << endl;
                cout<<"showing menu" <<endl;
                continue;
            }


           {
                // test clearing data before write

                
                long long int ind = files_map[fnme].inode_number;
            long long int curr = super_block_obj->start_inode_block + ind * (sizeof(struct inode_block_struct));
            fseek(fp, curr, SEEK_SET);
            struct inode_block_struct *inode_obj = (struct inode_block_struct *)malloc(sizeof(struct inode_block_struct));
            fread(inode_obj, sizeof(struct inode_block_struct), 1, fp);

            long long int dnum=inode_obj->data_block_num;
            inode_obj->data_block_num=-1;
            fseek(fp, curr, SEEK_SET);
            fwrite(inode_obj, sizeof(struct inode_block_struct), 1, fp);
            free(inode_obj);
            if(dnum!=-1){
                
            while(1){

            long long int curr = super_block_obj->start_data_block + dnum * (sizeof(struct data_block_struct));
            fseek(fp, curr, SEEK_SET);
            struct data_block_struct *dblock_obj = (struct data_block_struct *)malloc(sizeof(struct data_block_struct));
            fread(dblock_obj, sizeof(struct data_block_struct), 1, fp);
            dblock_obj->data[0]='\0';
            dblock_obj->curr_size=0;
            long long int nex=dblock_obj->next;
            dblock_obj->next=-1;
            data_bitmap[dnum]='0';
            curr = super_block_obj->start_data_block + dnum * (sizeof(struct data_block_struct));
            fseek(fp, curr, SEEK_SET);
            fwrite(dblock_obj, sizeof(struct data_block_struct), 1, fp);

            free(dblock_obj);
            dnum=nex;
            if(dnum==-1){
                break;
            }

                
            }
            }

            

            } 

    //cout<<"hi"<<endl;

            data_block_struct temp_obj;
            

            long long int inp_data_len = data.size(),data_arr_len=sizeof(temp_obj.data);
            long long int blocks_required = ceil((float)inp_data_len / (float)(data_arr_len - 1));

           // cout<<"dff"<<blocks_required<<endl;
            //cout<<"sadf"<<inp_data_len<<endl;
           // cout<<"dsfdf"<<data_arr_len<<endl;

            long long int free_data_blocks = count(data_bitmap.begin(), data_bitmap.end(), '0');

            if (free_data_blocks < blocks_required)
            {
                cout << "no enough space" << endl;
                cout<<"showing menu" <<endl;
                continue;
            }

//cout<<"his"<<endl;
                //cout<<blocks_required<<endl;
            long long int list_arr[blocks_required], t_count = 0;

//cout<<"ahufgyhdi"<<endl;

            for (long long int i = 0; i < data_bitmap.size(); i++)
            {
               // cout<<"hddasi"<<endl;

                if (data_bitmap[i] == '0')
                {
                    list_arr[t_count] = i;

//cout<<"hasaai"<<endl;

                    data_bitmap[i] = '1';
                    t_count++;

                    //cout<<"hadgfsi"<<endl;

                    if (t_count == blocks_required)
                    {
                        break;
                    }
                }
            }

           // cout<<"hasi"<<endl;

            long long int ind = files_map[fnme].inode_number;

            long long int curr = super_block_obj->start_inode_block + ind * (sizeof(struct inode_block_struct));
            fseek(fp, curr, SEEK_SET);
            struct inode_block_struct *inode_obj = (struct inode_block_struct *)malloc(sizeof(struct inode_block_struct));
            fread(inode_obj, sizeof(struct inode_block_struct), 1, fp);
            inode_obj->data_block_num = list_arr[0];

            fseek(fp, curr, SEEK_SET);
            fwrite(inode_obj, sizeof(struct inode_block_struct), 1, fp);
            long long int inp_data_ind = 0, dblock_ind = 0, curr_db;

//cout<<"hsd52fi"<<endl;

            for (long long int i = 0; i < blocks_required - 1; i++)
            {
                curr_db = list_arr[i];
                struct data_block_struct *datablock_obj = (struct data_block_struct *)malloc(sizeof(struct data_block_struct));
                while (dblock_ind < (data_arr_len - 1))
                {
                    datablock_obj->data[dblock_ind] = data[inp_data_ind];
                    dblock_ind++;
                    inp_data_ind++;
                }
                datablock_obj->data[dblock_ind] = '\0';

//cout<<"hsdaxdsi"<<endl;

                datablock_obj->curr_size = (data_arr_len - 1);
                datablock_obj->next = list_arr[i + 1];

                long long int curr = super_block_obj->start_data_block + list_arr[i] * (sizeof(struct data_block_struct));
                fseek(fp, curr, SEEK_SET);
                fwrite(datablock_obj, sizeof(struct data_block_struct), 1, fp);

                free(datablock_obj);

                dblock_ind = 0;
            }

//cout<<"hsddffi"<<endl;

            curr_db = list_arr[blocks_required - 1];
            struct data_block_struct *datablock_obj = (struct data_block_struct *)malloc(sizeof(struct data_block_struct));
            while (inp_data_ind < data.size())
            {
                datablock_obj->data[dblock_ind] = data[inp_data_ind];
                dblock_ind++;
                inp_data_ind++;
            }

            long long int t_size = (data.size()) % (data_arr_len - 1);
            if (t_size == 0)
            {
                t_size = data_arr_len - 1;
            }

            datablock_obj->data[dblock_ind] = '\0';

            datablock_obj->curr_size = t_size;
            datablock_obj->next = -1;
//cout<<"h23456i"<<endl;
             curr = super_block_obj->start_data_block + list_arr[blocks_required - 1] * (sizeof(struct data_block_struct));
            fseek(fp, curr, SEEK_SET);
            fwrite(datablock_obj, sizeof(struct data_block_struct), 1, fp);

            free(datablock_obj);
            free(inode_obj);
        }
        else if (inp == 5)
        {
            

            cout << "enter file descriptor: ";
            long long int inp_fd;
            cin >> inp_fd;
            if (fd_map.find(inp_fd) == fd_map.end())
            {
                cout << "no such file discriptor" << endl;
                cout<<"showing menu" <<endl;
                continue;
            }
            string fnme = fd_map[inp_fd];
            if (files_map[fnme].mode != "append")
            {
                cout << "given fd is not opened in append mode" << endl;
                cout<<"showing menu" <<endl;
                continue;
            }
            cout << "enter data to write to file: ";
            string data="";
            while(1){
                string st;
            getline(cin, st);
            
            if(st=="end"){
                break;
            }
            st+="~";
            data+=st;

            }
            data=data.substr(1,data.size()-1);
            data=data.substr(0,data.size()-1);
            //cout<<data<<"hi"<<endl;

            if (data.size() == 0)
            {
                cout << "no data entered" << endl;
                cout<<"showing menu" <<endl;
                continue;
            }





            string read_data="";

            { //read


            
            
            
            
            
            


            long long int ind = files_map[fnme].inode_number;
            long long int curr = super_block_obj->start_inode_block + ind * (sizeof(struct inode_block_struct));
            fseek(fp, curr, SEEK_SET);
            struct inode_block_struct *inode_obj = (struct inode_block_struct *)malloc(sizeof(struct inode_block_struct));
            fread(inode_obj, sizeof(struct inode_block_struct), 1, fp);

            long long int dnum=inode_obj->data_block_num;
            if(dnum!=-1){
                
            
            while(1){

            long long int curr = super_block_obj->start_data_block + dnum * (sizeof(struct data_block_struct));
            fseek(fp, curr, SEEK_SET);
            struct data_block_struct *dblock_obj = (struct data_block_struct *)malloc(sizeof(struct data_block_struct));
            fread(dblock_obj, sizeof(struct data_block_struct), 1, fp);
            read_data+=dblock_obj->data;
            //cout<<dblock_obj->data;
            dnum=dblock_obj->next;

            free(dblock_obj);

            if(dnum==-1){
                break;
            }

                
            }
            //cout<<endl;

            free(inode_obj);
            }

                


            }  

        data=read_data+data;

            { // write


             {
                // test clearing data before write

                
                long long int ind = files_map[fnme].inode_number;
            long long int curr = super_block_obj->start_inode_block + ind * (sizeof(struct inode_block_struct));
            fseek(fp, curr, SEEK_SET);
            struct inode_block_struct *inode_obj = (struct inode_block_struct *)malloc(sizeof(struct inode_block_struct));
            fread(inode_obj, sizeof(struct inode_block_struct), 1, fp);

            long long int dnum=inode_obj->data_block_num;
            inode_obj->data_block_num=-1;
            fseek(fp, curr, SEEK_SET);
            fwrite(inode_obj, sizeof(struct inode_block_struct), 1, fp);
            free(inode_obj);
            if(dnum!=-1){
                
            while(1){

            long long int curr = super_block_obj->start_data_block + dnum * (sizeof(struct data_block_struct));
            fseek(fp, curr, SEEK_SET);
            struct data_block_struct *dblock_obj = (struct data_block_struct *)malloc(sizeof(struct data_block_struct));
            fread(dblock_obj, sizeof(struct data_block_struct), 1, fp);
            dblock_obj->data[0]='\0';
            dblock_obj->curr_size=0;
            long long int nex=dblock_obj->next;
            dblock_obj->next=-1;
            data_bitmap[dnum]='0';
            curr = super_block_obj->start_data_block + dnum * (sizeof(struct data_block_struct));
            fseek(fp, curr, SEEK_SET);
            fwrite(dblock_obj, sizeof(struct data_block_struct), 1, fp);

            free(dblock_obj);
            dnum=nex;
            if(dnum==-1){
                break;
            }

                
            }
            }

            

            } 

    //cout<<"hi"<<endl;

            data_block_struct temp_obj;
            

            long long int inp_data_len = data.size(),data_arr_len=sizeof(temp_obj.data);
            long long int blocks_required = ceil((float)inp_data_len / (float)(data_arr_len - 1));

           // cout<<"dff"<<blocks_required<<endl;
            //cout<<"sadf"<<inp_data_len<<endl;
           // cout<<"dsfdf"<<data_arr_len<<endl;

            long long int free_data_blocks = count(data_bitmap.begin(), data_bitmap.end(), '0');

            if (free_data_blocks < blocks_required)
            {
                cout << "no enough space" << endl;
                cout<<"showing menu" <<endl;
                continue;
            }

//cout<<"his"<<endl;
                //cout<<blocks_required<<endl;
            long long int list_arr[blocks_required], t_count = 0;

//cout<<"ahufgyhdi"<<endl;

            for (long long int i = 0; i < data_bitmap.size(); i++)
            {
               // cout<<"hddasi"<<endl;

                if (data_bitmap[i] == '0')
                {
                    list_arr[t_count] = i;

//cout<<"hasaai"<<endl;

                    data_bitmap[i] = '1';
                    t_count++;

                    //cout<<"hadgfsi"<<endl;

                    if (t_count == blocks_required)
                    {
                        break;
                    }
                }
            }

           // cout<<"hasi"<<endl;

            long long int ind = files_map[fnme].inode_number;

            long long int curr = super_block_obj->start_inode_block + ind * (sizeof(struct inode_block_struct));
            fseek(fp, curr, SEEK_SET);
            struct inode_block_struct *inode_obj = (struct inode_block_struct *)malloc(sizeof(struct inode_block_struct));
            fread(inode_obj, sizeof(struct inode_block_struct), 1, fp);
            inode_obj->data_block_num = list_arr[0];

            fseek(fp, curr, SEEK_SET);
            fwrite(inode_obj, sizeof(struct inode_block_struct), 1, fp);
            long long int inp_data_ind = 0, dblock_ind = 0, curr_db;

//cout<<"hsd52fi"<<endl;

            for (long long int i = 0; i < blocks_required - 1; i++)
            {
                curr_db = list_arr[i];
                struct data_block_struct *datablock_obj = (struct data_block_struct *)malloc(sizeof(struct data_block_struct));
                while (dblock_ind < (data_arr_len - 1))
                {
                    datablock_obj->data[dblock_ind] = data[inp_data_ind];
                    dblock_ind++;
                    inp_data_ind++;
                }
                datablock_obj->data[dblock_ind] = '\0';

//cout<<"hsdaxdsi"<<endl;

                datablock_obj->curr_size = (data_arr_len - 1);
                datablock_obj->next = list_arr[i + 1];

                long long int curr = super_block_obj->start_data_block + list_arr[i] * (sizeof(struct data_block_struct));
                fseek(fp, curr, SEEK_SET);
                fwrite(datablock_obj, sizeof(struct data_block_struct), 1, fp);

                free(datablock_obj);

                dblock_ind = 0;
            }

//cout<<"hsddffi"<<endl;

            curr_db = list_arr[blocks_required - 1];
            struct data_block_struct *datablock_obj = (struct data_block_struct *)malloc(sizeof(struct data_block_struct));
            while (inp_data_ind < data.size())
            {
                datablock_obj->data[dblock_ind] = data[inp_data_ind];
                dblock_ind++;
                inp_data_ind++;
            }

            long long int t_size = (data.size()) % (data_arr_len - 1);
            if (t_size == 0)
            {
                t_size = data_arr_len - 1;
            }

            datablock_obj->data[dblock_ind] = '\0';

            datablock_obj->curr_size = t_size;
            datablock_obj->next = -1;
//cout<<"h23456i"<<endl;
             curr = super_block_obj->start_data_block + list_arr[blocks_required - 1] * (sizeof(struct data_block_struct));
            fseek(fp, curr, SEEK_SET);
            fwrite(datablock_obj, sizeof(struct data_block_struct), 1, fp);

            free(datablock_obj);
            free(inode_obj);




            }
           



        }
        else if (inp == 6)
        {
            cout << "enter file descriptor: ";
            long long int inp_fd;
            cin >> inp_fd;
            if (fd_map.find(inp_fd) == fd_map.end())
            {
                cout << "no such file discriptor" << endl;
                cout<<"showing menu" <<endl;
                continue;
            }
            string fnme = fd_map[inp_fd];
            files_map[fnme].file_desc = -1;
            files_map[fnme].mode = "-1";
            fd_map.erase(inp_fd);
            cout<<"file closed"<<endl;
        }
        else if (inp == 7)
        {

            cout<<"enter file name: ";
            string fnme;
            cin>>fnme;
            if(files_map.find(fnme)==files_map.end()){
                cout<<"no such file"<<endl;
            }




            {
                // test clearing data before write

                
                long long int ind = files_map[fnme].inode_number;
                 inode_bitmap[ind]='0';

            long long int curr = super_block_obj->start_inode_block + ind * (sizeof(struct inode_block_struct));
            fseek(fp, curr, SEEK_SET);
            struct inode_block_struct *inode_obj = (struct inode_block_struct *)malloc(sizeof(struct inode_block_struct));
            fread(inode_obj, sizeof(struct inode_block_struct), 1, fp);

            long long int dnum=inode_obj->data_block_num;
            inode_obj->data_block_num=-1;
            inode_obj->file_size=0;
            
            fseek(fp, curr, SEEK_SET);
            fwrite(inode_obj, sizeof(struct inode_block_struct), 1, fp);
            free(inode_obj);
            if(dnum!=-1){
                
            while(1){

            long long int curr = super_block_obj->start_data_block + dnum * (sizeof(struct data_block_struct));
            fseek(fp, curr, SEEK_SET);
            struct data_block_struct *dblock_obj = (struct data_block_struct *)malloc(sizeof(struct data_block_struct));
            fread(dblock_obj, sizeof(struct data_block_struct), 1, fp);
            dblock_obj->data[0]='\0';
            dblock_obj->curr_size=0;
            long long int nex=dblock_obj->next;
            dblock_obj->next=-1;
            data_bitmap[dnum]='0';
            curr = super_block_obj->start_data_block + dnum * (sizeof(struct data_block_struct));
            fseek(fp, curr, SEEK_SET);
            fwrite(dblock_obj, sizeof(struct data_block_struct), 1, fp);

            free(dblock_obj);
            dnum=nex;
            if(dnum==-1){
                break;
            }

                
            }
            }

            

            }
            files_map.erase(fnme); 
        }
        else if (inp == 8)
        {

            for (auto i = files_map.begin(); i != files_map.end(); i++)
            {
                cout << i->first << endl;
            }
        }
        else if (inp == 9)
        {
            for (auto i = files_map.begin(); i != files_map.end(); i++)
            {
                if (i->second.file_desc >= 0)
                {
                    cout << i->first <<" "<<i->second.file_desc <<" "<< i->second.mode<<endl;
                }
            }
        }
        else if (inp == 10)
        {

            long long int te = super_block_obj->no_of_inodes;
            char char_inode_bitmap[te + 1];
            for (long long int i = 0; i < te; i++)
            {
                char_inode_bitmap[i] = inode_bitmap[i];
            }

            fseek(fp, super_block_obj->start_inode_bm, SEEK_SET);
            fputs(char_inode_bitmap, fp);

            te = super_block_obj->no_of_data_blocks;
            char char_data_bitmap[te + 1];
            for (long long int i = 0; i < te; i++)
            {
                char_data_bitmap[i] = data_bitmap[i];
            }

            fseek(fp, super_block_obj->start_data_bm, SEEK_SET);
            fputs(char_data_bitmap, fp);

            // close all files before umount and delete

            break;
        }

        else
        {
            cout << "wrong choice" << endl;
        }
    }
    fclose(fp);
    free(super_block_obj);
}

int main()
{
    int disk_count = 0;
    while (1)
    {
        cout << "1.create disk" << endl;
        cout << "2.mount disk" << endl;
        cout << "3.exit" << endl;
        int inp;
        cin >> inp;
        if (inp == 1)
        {
            string dname;
            cout<<"enter disk name: ";
            cin>>dname;

            if (all_disks.find(dname) != all_disks.end())
                {
                    cout << "disk already exists" << endl;
                    
                }
            else{
            create_disk(dname);
            }
            //cout << "the disk number is " << disk_count << endl;
            disk_count++;
        }
        else if (inp == 2)
        {
            string ins_inp;
            cout << "enter disk name: ";
            cin >> ins_inp;
            
            mount_disk(ins_inp);
        }
        else if (inp == 3)
        {
            break;
        }
        else
        {
            cout << "wrong choice" << endl;
        }
    }

    return 0;
}

// changed inode structure from arry of int to single int
// chechked whether inodes are full or not in create file


// three  -> read input line by line


// five-> making new line

// six ->deleting files
// 7-> update sizes
// 8-> take disk name from user