#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string> 
#include <sstream>
using namespace std;

struct node
{
    int val;
    int cnt;
};

struct BPTnode
{
    int size;
    bool leaf;
    vector <struct node > data;
    vector <BPTnode *> child_ptr;
};

int degree=0;
int max_data=0;
int outputBufferSize=0;
int current_out_buf_size=0;

vector<string> output_buf;

struct BPTnode *root = NULL;
struct BPTnode *np = NULL;
struct BPTnode *x = NULL;

bool sort_by_val( const struct node & node1, const struct node & node2 )
{
    if (node1.val < node2.val)
    {
        return true;
    }
    else 
        return false;
}

void traverse_level(BPTnode *p)
{   
    int i,j;
    if (p->leaf == false)
    {
        for(j=0;j<p->size;j++)

            cout << p->data[j].val << "(" <<p->data[j].cnt <<") " ;
        cout<<endl;
        for (i =0;i<p->size;i++)
        {
            cout<<"i "<<i<<"-> ";
            traverse_level(p->child_ptr[i]);
        }
    } 
    else{
        for(j=0;j<p->size;j++)
            cout << p->data[j].val << "(" <<p->data[j].cnt <<") " ;
        cout<<endl;
    }
    if (p->leaf == false)
    {
        cout<<"i "<<i<<"-> ";
        traverse_level(p->child_ptr[i]);
    }
}

BPTnode * init()
{
    struct BPTnode *newNode = new BPTnode;
    newNode->size = 0;
    newNode->leaf = true;
    newNode->data.resize(max_data+1);
    
    newNode->child_ptr.resize(degree);
    return newNode;
}

struct find_by_val :unary_function<struct node, bool> {
    int key,val1=-1;
    find_by_val(int key):key(key) { }
    bool operator()(const struct node & node1) const {
        
        if(val1>0){
            return false;
        }
        return node1.val == key;
    }
};



BPTnode* split_leaf(BPTnode* x,int key)
{
    BPTnode *node3, *node1;
    node1 = init();
    node3 = init();
    int i,j,mid_index,mid,val1,val2;
    
    for(i=0;i<degree;i++)
    {
        node1->child_ptr[i] = NULL;
    }
    for(i=0;i<degree;i++)
    {
        node3->child_ptr[i] = NULL;
    }

    node1->leaf = false;
    node3->leaf = true;

    mid_index=degree/2;
    mid=x->data[mid_index].val;

    for(j=mid_index;j<degree;j++)
    {
        node3->size=node3->size+1;
        val1=x->data[j].val;
        node3->data[j-mid_index].val=val1;
        val2=x->data[j].cnt;
        node3->data[j-mid_index].cnt=val2;
       
        x->size=x->size-1;
        x->data[j].val=0;
        x->data[j].cnt=0;
       
    }
    node3->child_ptr[degree-1]=x->child_ptr[degree-1];
    for(j=0;j<degree;j++)
    {
        x->child_ptr[j] = NULL;
    }
    x->size=mid_index;
    x->child_ptr[degree-1]=node3;
   
    node1->size=node1->size+1;
    node1->data[0].val = mid;
    node1->child_ptr[0]=x;
    node1->child_ptr[1]=node3;
    
    return node1;
}

BPTnode* split_parent(vector <BPTnode* > new_child_ptr, BPTnode* x)
{
    BPTnode *retNode,*node3, *node1;
    int i,j,val1,val2;
    int mid_index,mid,val3;
    retNode=NULL;
    node3=init();
    node3->leaf = false;

    node1=init();
    node1->leaf = false;

    mid_index=degree/2;
    mid=x->data[mid_index].val;
    
    for(i=0;i<degree;i++)
    {
        node3->child_ptr[i] = NULL;
    }
    for(i=0;i<degree;i++)
    {
        node1->child_ptr[i] = NULL;
    }

    x->size=x->size-1;
    x->data[mid_index].val=0;
    x->data[mid_index].cnt=0;
    
    for(j=mid_index+1;j<degree;j++)
    {
        node3->size=node3->size+1;
       
        val1=x->data[j].val;
        node3->data[j-(mid_index+1)].val=val1;
        val2=x->data[j].cnt;
        node3->data[j-(mid_index+1)].cnt=val2;
        x->data[j].cnt=0;
        x->data[j].val=0;
        x->size=x->size-1;
    }

    for(j=0;j<degree;j++)
    {
        x->child_ptr[j] = NULL;
    }

    x->size=mid_index;

    node1->child_ptr[0]=x;
    node1->child_ptr[1]=node3;
    node1->size=node1->size+1;
    

    node1->data[0].val = mid;

    for(i=0;i<=mid_index;i++)
    {
        x->child_ptr[i]=new_child_ptr[i];
    }
    for(i=mid_index+1;i<=degree;i++)
    {
        val3=mid_index+1;
        node3->child_ptr[i-(val3)]=new_child_ptr[i];
    }
    retNode=node1;
    return retNode;
}

BPTnode* merge_parent(BPTnode *x, BPTnode *temp)
{
    int i,newVal, loc, new_index;
    vector <BPTnode *> new_child_ptr; 
    BPTnode *left, *right;

    new_child_ptr.resize(degree);
    newVal = temp->data[0].val;
    left = temp->child_ptr[0];
    right = temp->child_ptr[1];
    new_index=0;
    
    x->data[x->size].val=newVal;
    x->size=x->size+1;

    sort(x->data.begin(), x->data.begin()+(x->size),sort_by_val); 
    loc=find_if(x->data.begin(),x->data.begin()+(x->size),find_by_val(newVal))-x->data.begin();

    
    if(loc==0)
    {
        new_child_ptr[new_index]=left;
        new_index=new_index+1;
        new_child_ptr[new_index]=right;
        new_index=new_index+1;
        
        for(i=loc+1;new_index<degree && i<degree;i++)
        {
            new_child_ptr[new_index]=x->child_ptr[i];
            new_index=new_index+1;
        }
    }
    else 
    {
        for(i=0;i<loc;i++)
        {
            new_child_ptr[new_index]=x->child_ptr[i];
            new_index=new_index+1;
        }
        new_child_ptr[new_index]=left;
        new_index=new_index+1;
        new_child_ptr[new_index]=right;
        new_index=new_index+1;
        for(i=loc+1;new_index<degree && i<degree;i++)
        {
            new_child_ptr[new_index]=x->child_ptr[i];
            new_index=new_index+1;
        }
    }
    x->child_ptr=new_child_ptr;
    return x; 
}


BPTnode* merge_split_parent(BPTnode* x,BPTnode* temp)
{
    vector <BPTnode *> new_child_ptr; 
    new_child_ptr.resize(degree+1);
    BPTnode* retNode, *left, *right;
    int i,loc,newVal,new_index;

    newVal = temp->data[0].val;
    new_index=0;
    retNode=NULL;
    left = temp->child_ptr[0];
    right = temp->child_ptr[1];
    
    x->data[max_data].val=newVal;
    sort(x->data.begin(), x->data.end(),sort_by_val);      
   
    loc=find_if(x->data.begin(), x->data.end(),find_by_val(newVal)) - x->data.begin();

    


    for(i=0;i<degree;i++)
    {
        if(i==loc)
        {
            new_child_ptr[new_index]=left;
            new_index=new_index+1;
            new_child_ptr[new_index]=right;
            new_index=new_index+1;
        }
        else
        {
            new_child_ptr[new_index]=x->child_ptr[i];
            new_index=new_index+1;
        }
    }
    retNode = split_parent(new_child_ptr,x);
    return retNode;
}

BPTnode * rec_insert(int key, BPTnode* x)
{
    int i,position=-1,val1,val2,val3;
    vector <BPTnode *> new_child_ptr;  
    BPTnode *retNode, *temp;
    retNode=x;
    temp=NULL;
    
    if(x->leaf==true)
    {
        position = find_if(x->data.begin(),x->data.begin()+(x->size),find_by_val(key)) - x->data.begin();
        if( position>-1 && position < x->size )
        {
            val1=x->data[position].cnt+1;
            x->data[position].cnt=val1;
            retNode = x;
        }
        else
        {
            if(x->size <max_data)
            {
                x->data[x->size].val=key;
                val2=x->data[x->size].cnt+1;
                x->data[x->size].cnt=val2;

                
                sort(x->data.begin(), x->data.begin()+(x->size+1),sort_by_val);
                x->size=x->size+1;
                retNode = x;
            }
            else  
            {
                val3=x->data[max_data].cnt+1;
                x->data[max_data].val = key;
                x->data[max_data].cnt=val3;
                sort(x->data.begin(), x->data.end(),sort_by_val);

                retNode=split_leaf(x,key);
            }    
        }
    }
    else        
    {
        for(i=0; i<x->size;i++)
        {
            if(i!=0){
                if(key>=x->data[i-1].val && key<x->data[i].val){
                    break;
                }
                
            }
            else
            {
                if(key<x->data[0].val){
                    break;
                }
            }
        }
        temp=rec_insert(key,x->child_ptr[i]);
        if(temp==x->child_ptr[i])       
        {
            return x;
        }
        else
        {
            if(x->size < max_data) 
            {
                x=merge_parent(x,temp);
                return x;
            }  
            else   
            {
                x=merge_split_parent(x,temp);
                return x; 
            }  
        }
    }
    return retNode;
}

void insert_key(int key)
{
    int position=-1,i;
    if(root==NULL)
    {
        root = init();
        for(i=0;i<degree;i++)
        {
            root->child_ptr[i] = NULL;
        }
        root->data[root->size].cnt=root->data[root->size].cnt+1;
        root->data[root->size].val=key;
        root->size=root->size+1;
    }
    else if(root->leaf==true && root->size<max_data)
    {
        position = find_if(root->data.begin(),root->data.begin()+(root->size),find_by_val(key)) - root->data.begin();
        if(!( position>-1 && position < root->size) )
        {
            root->data[root->size].cnt=root->data[root->size].cnt+1;
            root->data[root->size].val=key;
            root->size=root->size+1;
            sort(root->data.begin(), root->data.begin()+(root->size),sort_by_val);
        }
        else
        {
            root->data[position].cnt=root->data[position].cnt+1;
        }
    }
    else{
        root = rec_insert(key, root);
    }
}

int rec_range(BPTnode* x,int num1, int num2)
{
    int i,range_count,counttemp,val1,val2;
    range_count=0;
    if(x->leaf==true)
    {
        i=0;
        while(i< x->size && x->data[i].val < num1){
            i=i+1;
        }

        while(i < x->size && x->data[i].val<=num2)
        {
            range_count=range_count+x->data[i].cnt;
            i=i+1;
        }
        if(i>=x->size && x->child_ptr[degree -1] !=NULL){
            counttemp=range_count + rec_range(x->child_ptr[degree -1],num1,num2);
            return counttemp;
        }
        else{
            return range_count;
        }
    }
    else
    {
        i=0;
        for(i=0; i<x->size;i++)
        {
            if(i!=0){
                val1=x->data[i-1].val;
                val2=x->data[i].val;
                if(num1>=val1 && num1<val2){
                    break;
                }
                
            }
            else
            {
                val1=x->data[0].val;
                if(num1<val1){
                    break;
                }
            }
        }
        counttemp=rec_range(x->child_ptr[i],num1,num2);
        return counttemp;    
    }
}

int range_keys(int x,int y)
{
    int i,range_count;
    range_count=0;
    if(root==NULL)
    {
        return range_count;
    }
    else if(root->leaf==true)
    {
        i=0;
        while(i<root->size && root->data[i].val<x){
            i=i+1;
        }

        while(i<root->size && root->data[i].val<=y)
        {
            range_count=range_count+root->data[i].cnt;
            i=i+1;
        }
    }
    else
    {
        range_count=rec_range(root,x,y);
    }
    return range_count;
}

int rec_find(int key, BPTnode* x)
{
    int position=-1,i=0,res,val1,val2;
    if(x->leaf==true)
    {
        position = find_if(x->data.begin(),x->data.begin()+(x->size),find_by_val(key)) - x->data.begin();
        if( position>-1 && position < x->size )
        {
            res=x->data[position].cnt;
            return res;
        }
        else{
            return 0;
        }
    }
    else  
    {    
        for(i=0; i<x->size;i++)
        {
            if(i!=0){
                val1=x->data[i-1].val;
                val2=x->data[i].val;
                if(key>=val1 && key<val2)
                    break;
            }
            else
            {
                val1=x->data[0].val;
                if(key<val1)
                    break;  
            }
        }
        return rec_find(key,x->child_ptr[i]);
    }
}

int find_key(int key)
{
    int position=-1,res;
    if(root==NULL)
    {
        return 0;
    }
    else if(root->leaf==true)
    {
        position = find_if(root->data.begin(),root->data.begin()+(root->size),find_by_val(key)) - root->data.begin();
        if( position>-1 && position < root->size )
        {
            res=root->data[position].cnt;
            return res;
        }
        else{
            return 0;
        }
    }
    else
    {
        res=rec_find(key,root);
        return res;
    }
}

int count_key(int key)
{
    return find_key(key);
}
void print_output_buffer();
void process_input_buffer(vector <string> records)
{
    char *operation;
    string output="";
    int i,x,y,retVal,len,size1,size2,size3;

    len=records.size();
    for(i =0;i<len;i++)
    {
        char input[256];
        output="";
        strcpy(input,records[i].c_str());
        output="";
        operation=strtok(input," \t");
                
        
        if(strcmp(operation,"FIND")==0)
        {
            x=atoi(strtok (NULL," \t\n"));

            output="";
            if(find_key(x)!=0)
            { 
                output="YES";
            }
            else
            {   
                output="NO";
            }
            size1=current_out_buf_size;
            size2=output.length();
            size3=outputBufferSize;
            if(size1+size2>size3)
            {
                print_output_buffer();
                current_out_buf_size=0;
                output_buf.clear();
            }
            current_out_buf_size=current_out_buf_size+output.length();
            output_buf.push_back(output);
            
        }
        else if(strcmp(operation,"COUNT")==0)
        {
            output="";
            x=atoi(strtok (NULL," \t\n"));
            retVal=count_key(x);

            ostringstream convert;
            convert << retVal; 
            output=convert.str();

            if(current_out_buf_size+4>outputBufferSize)
            {
                print_output_buffer();
                current_out_buf_size=0;
                output_buf.clear();
            }
            current_out_buf_size=current_out_buf_size+4;
            output_buf.push_back(output);
            
        }
        else if(strcmp(operation,"RANGE")==0)
        {
            output="";

            x=atoi(strtok (NULL," \t"));
            y=atoi(strtok (NULL," \t\n"));
            retVal =range_keys(x,y); 

            ostringstream convert;
            convert << retVal; 
            output=convert.str();

            if(current_out_buf_size+4>outputBufferSize)
            {
                print_output_buffer();
                current_out_buf_size=0;
                output_buf.clear();
            }
            current_out_buf_size=current_out_buf_size+4;
            output_buf.push_back(output);
            
        }   
        else if(strcmp(operation,"INSERT")==0)
        {
            x=atoi(strtok (NULL," \t\n"));
            insert_key(x);
        }     
    }
}

void print_output_buffer()
{
    int i;
    int len=output_buf.size();
    //cout << "-----------printing output buffer-----------------"<<endl;
    for(i=0;i<len;i++)
        cout<<output_buf[i]<<endl;
    //cout << "--------------------------------------------------"<<endl;
}

int main(int argc, char *argv[])
{
    int n,M,B,inputBufferSize,currentBufferSize,size1,size2,size3;
    vector <string> records;                    //records=inputbuffer line=command
    string command;
    if(argc<4)
    {
        cout<<"Give command line arguments :Filename, number of buffers(M), buffer size(B)\n";
        exit(1);
    }

    string filename= argv[1];
    M = atoi(argv[2]);
    B = atoi(argv[3]);    
    
    inputBufferSize=B*(M-1); 
    currentBufferSize=0;
    outputBufferSize=B;

    n=0;
    n=(B-8)/12;
    if(n<1)                                  
    {
        cout<<"Give differrent values for number of buffers(M), buffer size(B)\n";
        exit(1);
    }
    degree=n;
    max_data=n-1; 

    FILE *fd;
    fd=fopen(filename.c_str(),"r");

    if(fd==NULL)
    {
        cout<<"Input file does not exist\n";
        exit(1);    
    }   
    
    records.clear();
    while(1)
    {
        char line[256];
        command="";
        while (fgets(line,256,fd)!=NULL)
        {
            command=line;
            size1=currentBufferSize;
            size2=strlen(line);
            size3=inputBufferSize;

            if(size1+size2> size3)
            {
                currentBufferSize=0;
                process_input_buffer(records);
                records.clear();
                
            }
            currentBufferSize= currentBufferSize+strlen(line);
            records.push_back(command);
            
        }
        if(!records.empty())
        {
            process_input_buffer(records);
        }
        break;

    }
    print_output_buffer();
    //traverse_level(root);
    return 0;
}

