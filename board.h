#include <fstream>
#include <algorithm>
#include "page.h"

using std::endl;
using std::ofstream;

class Board {
    public:
        Board(int num_jobs, int width, int height, ofstream& output_stream);
        ~Board();

        void print_board();
        void print_job(int job_idx, char job_type, int id);

        //job functions
        void insert_page(int x, int y, int width, int height, int id, char content);
        void delete_page(int id);
        void modify_content(int id, char content);
        void modify_position(int id, int x, int y);
        void first_delete_process(int id);
        void second_delete_process(int id);
        void second_delete_process_onlyfor_del(vector <int> &del_on_pages);
        vector<int> recursive_find_on_index(int id,vector<int> &new_index);

    private:
        int num_jobs, width, height; 
        ofstream& output; 
        char* board; 
};


Board::Board(int num_jobs, int width, int height, ofstream& output_stream): output(output_stream) {
    this->width = width;
    this->height = height;
    this->num_jobs = num_jobs;

    board = new char[width*height];

    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            board[h*width + w] = ' ';
        }
    }

}

Board::~Board() {
    delete board;
    
}


void Board::print_board() {
    int h, w;
    for (w = 0; w < width+2; w++) output << "- ";
    output << endl;
    
    for (h = 0; h < height; h++) {
        output << "| ";
        for (w = 0; w < width; w++) {
            output << board[h*width + w] << " ";
        }
        output << "| " << endl;
    }

    for (w = 0; w < width+2; w++) output << "- ";
    output << endl;
}

void Board::print_job(int job_idx, char job_type, int id) {
    output << ">> (" << job_idx <<") ";
    switch(job_type) {
        
        case 'i':
            output << "Insert ";
            break;
        case 'd':
            output << "Delete ";
            break;
        case 'm':
            output << "Modify ";
            break;
    }

    output << id << endl;
}


void Board::insert_page(int i_x, int i_y, int i_width, int i_height, int id, char content) {
    Page page=Page(i_x,i_y,i_width,i_height,id,content,width,height);
    pages.push_back(page);
    page.on_pages={};
    push_on_page(page,pages);//on_page
    //below_page contents 저장.
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            page.below_contents[h*width + w] = ' ';
        }
    }
    for (int h = i_y; h < (i_height+i_y); h++) {//make_below_contents
        for (int w = i_x; w < (i_width+i_x); w++) {
            page.below_contents[h*width + w]=board[h*width + w];
            }
        }
    for (int h = i_y; h < (i_height+i_y); h++) {
        for (int w = i_x; w < (i_width+i_x); w++) {
            //insert_page할때 해당 자리에 다른 문자 있으면, on_page임을 표시.
            board[h*width + w] = content;
            //board의 각 content 자리에 해당 값이 바뀔 때마다, id리스트를 갱신하고, 각 자리마다 id log가 남도록 구성.
        }
    }
    print_board();
}

void Board::delete_page(int id) {//pages에서, on_page에서 id 삭제해야함. 노노. process를 크게 세가지로 나누고 부분에 대해 recursive 먹이면된다.
    //recursive delete
    //on_page
    first_delete_process(id);//1st. delete process
    int flag1=-1;
    int flag2=-1;
    vector <int> del_on_pages;
    for (int i = 0; i < pages.size(); i++) {//2nd. delete id on vectors error 위치!!
        flag1=-1;
        for (int j = 0; j < pages[i].on_pages.size(); j++){
            if(pages[i].on_pages[j]==id){
                flag1=j;
            }
        }
        if(flag1>=0){
            pages[i].on_pages.erase(pages[i].on_pages.begin() + flag1);
        }
    }
    for (int i = 0; i < pages.size(); i++) {
        flag2=-1;
        if (pages[i].get_id() == id) {
            flag2=i;
        }
        if(flag2>=0){
            del_on_pages=pages[flag2].on_pages;
            pages.erase(pages.begin() + flag2);
        }
    }
    //page 순서 편집 recursive
    
    second_delete_process_onlyfor_del(del_on_pages);//3rd. rebuilding process
}

void Board::modify_content(int id, char content) { 
    first_delete_process(id);
    int page_order = Page::find_by_id(id,pages);
    pages[page_order].modify_content(content);//variable 수정.
    int m_x=pages[page_order].get_x();
    int m_y=pages[page_order].get_y();
    int m_width=pages[page_order].get_width();
    int m_height=pages[page_order].get_height();
    for (int h = m_y; h < (m_height+m_y); h++) {//modified content로 다시 채우기.
        for (int w = m_x; w < (m_width+m_x); w++) {
            board[h*width + w] = content;
        }
    }
    print_board();
    second_delete_process(id);
    //해당 id의 page 업데이트.
}

void Board::modify_position(int id, int x, int y) {//유일하게 에러 발생함...
    first_delete_process(id);   
    int flag1=-1;
    int flag2=-1;
    vector <int> del_on_pages;
    for (int i = 0; i < pages.size(); i++) {
        flag2=-1;
        if (pages[i].get_id() == id) {
            flag2=i;
        }
        if(flag2>=0){
            del_on_pages=pages[flag2].on_pages;
        }
    }
    for (int i = 0; i < pages.size(); i++) {//on_page 고려.
        flag1=-1;
        for (int j = 0; j < pages[i].on_pages.size(); j++){
            if(pages[i].on_pages[j]==id){
                flag1=j;
            }
        }
        if(flag1>=0){
            pages[i].on_pages.erase(pages[i].on_pages.begin() + flag1);
        }
    }
    int page_order = Page::find_by_id(id,pages);
    pages[page_order].modify_position_xy(x,y);//variable 수정.
    Page current_page=pages[page_order];//현재 페이지.
    int m_width=pages[page_order].get_width();
    int m_height=pages[page_order].get_height();
    int m_content=pages[page_order].get_content();
    for (int h = y; h < (m_height+y); h++) {//새로운 위치의 below_contents 받아오기.
        for (int w = x; w < (m_width+x); w++) {
            pages[page_order].below_contents[h*width + w]=board[h*width + w];
        }
    }
    for (int h = y; h < (m_height+y); h++) {//modified x,y를 기준으로 다시 채우기.
        for (int w = x; w < (m_width+x); w++) {
            board[h*width + w] = m_content;
        }
    }
    vector <Page> new_pages=pages;
    if(new_pages[page_order].on_pages.size()>0){
        int min_order;
        min_order=Page::find_by_id((new_pages[page_order].on_pages[0]),new_pages);
        for(int j=0;j<new_pages[page_order].on_pages.size();j++){
            if(min_order>Page::find_by_id((new_pages[page_order].on_pages[j]),new_pages)){
                min_order=Page::find_by_id((new_pages[page_order].on_pages[j]),new_pages);
            }
        }
        for(int i=min_order;i<new_pages.size();i++){
            pages.pop_back();
        }
        pages.erase(pages.begin()+page_order);
        pages.push_back(new_pages[page_order]);
        for(int i=min_order;i<new_pages.size();i++){
            pages.push_back(new_pages[i]);
        }
    }
    vector <int> new_index={};//떼어낼 pages의 index 값들 저장.
    vector <Page> allocate_pages={};//떼어낸 page들 저장.
    recursive_find_on_index(id,new_index);
    sort(new_index.begin(),new_index.end());//new_index 오름차순 배열.
    if(new_index.size()>0){
        for(int i=0;i<new_index.size();i++){
            allocate_pages.push_back(pages[new_index[i]]);
        }
        for(int i=0;i<new_index.size();i++){
            pages.erase(pages.begin()+new_index[i]-i);//for 루프 돌면서 pages의 size 바뀜. 고려.
        }
        pages.push_back(current_page);//현재 페이지 새로 만들어 넣기.
        current_page.on_pages={};
        push_on_page(current_page,pages);
        for(int j=0;j<allocate_pages.size();j++){
            pages.push_back(allocate_pages[j]);
        }
    }
    page_order = Page::find_by_id(id,pages);
    push_on_page(pages[page_order],pages);//error.. pages에서 인덱스의 꼬임 발생.order 바꿔 넣자!!
    print_board();
    second_delete_process_onlyfor_del(del_on_pages);
}

void Board::first_delete_process(int id){//위의 장 중 선택해서 제거하는 단계.
    int page_order=Page::find_by_id(id,pages);
    int x=pages[page_order].get_x();
    int y=pages[page_order].get_y();
    int d_width=pages[page_order].get_width();
    int d_height=pages[page_order].get_height();
    if(pages[page_order].on_pages.size()==0){

    }
    else{
        std::sort(pages[page_order].on_pages.begin(),pages[page_order].on_pages.begin());
        for(int i=0;i<pages[page_order].on_pages.size();i++){
            Board::first_delete_process(pages[page_order].on_pages[i]);
        }
    }
    for (int h = y; h < (d_height+y); h++) {
        for (int w = x; w < (d_width+x); w++) {
            board[h*width + w] = pages[page_order].below_contents[h*width + w];
        }
    }
    print_board();
    return;
}

void Board::second_delete_process(int id){//re-posit 이때 on_page 업데이트
    int page_order = Page::find_by_id(id,pages);
    int on_pages_size = pages[page_order].on_pages.size();
    
    if (on_pages_size>0){
        std::sort(pages[page_order].on_pages.begin(),pages[page_order].on_pages.end());
        for(int i=0;i<on_pages_size;i++){//유사 insert_page 그렇지만, 새로운 page 생성파트와 push_back 부분은 빠짐.
            int i_page_order=Page::find_by_id(pages[page_order].on_pages[on_pages_size-i-1],pages);
            int i_x=pages[i_page_order].get_x();
            int i_y=pages[i_page_order].get_y();
            int i_width=pages[i_page_order].get_width();
            int i_height=pages[i_page_order].get_height();
            int i_id=pages[i_page_order].get_id();
            char i_content=pages[i_page_order].get_content();
            for (int h = i_y; h < (i_height+i_y); h++) {
                for (int w = i_x; w < (i_width+i_x); w++) {
                    pages[i_page_order].below_contents[h*width + w]=board[h*width + w];//below_contents update!
                }
            }
            for (int h = i_y; h < (i_height+i_y); h++) {
                for (int w = i_x; w < (i_width+i_x); w++) {
                    board[h*width + w] = i_content;//다시 채우기.
                }
            }
            print_board();
            push_on_page(pages[i_page_order],pages);//on_page
            Board::second_delete_process(pages[i_page_order].get_id());
        }
    }
    else {  
        return;
    }
}

void Board::second_delete_process_onlyfor_del(vector <int> &del_on_pages){//re-posit 이때 on_page 업데이트
    int del_on_pages_size = del_on_pages.size();
    
    if (del_on_pages_size>0){
        std::sort(del_on_pages.begin(),del_on_pages.end());
        for(int i=0;i<del_on_pages_size;i++){//유사 insert_page 그렇지만, 새로운 page 생성파트와 push_back 부분은 빠짐.
            int i_page_order=Page::find_by_id(del_on_pages[del_on_pages_size-i-1],pages);
            int i_x=pages[i_page_order].get_x();
            int i_y=pages[i_page_order].get_y();
            int i_width=pages[i_page_order].get_width();
            int i_height=pages[i_page_order].get_height();
            int i_id=pages[i_page_order].get_id();
            char i_content=pages[i_page_order].get_content();
            for (int h = i_y; h < (i_height+i_y); h++) {
                for (int w = i_x; w < (i_width+i_x); w++) {
                    pages[i_page_order].below_contents[h*width + w]=board[h*width + w];//below_contents update!
                }
            }
            for (int h = i_y; h < (i_height+i_y); h++) {
                for (int w = i_x; w < (i_width+i_x); w++) {
                    board[h*width + w] = i_content;//다시 채우기.
                }
            }
            print_board();
            push_on_page(pages[i_page_order],pages);//on_page
            Board::second_delete_process(del_on_pages[del_on_pages_size-i-1]);
        }
    }
    else {  
        return;
    }
}

vector<int> Board::recursive_find_on_index(int id,vector<int> &new_index){
    if(pages[Page::find_by_id(id,pages)].on_pages.size()==0){

    }
    else{
        for(int i = 0; i < pages[Page::find_by_id(id,pages)].on_pages.size(); i++){
            int on_id=pages[Page::find_by_id(id,pages)].on_pages[i];
            new_index.push_back(Page::find_by_id(on_id,pages));
            recursive_find_on_index(on_id,new_index);
        }
    }
    return new_index;
}