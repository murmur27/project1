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
        void first_delete_process(int id, vector<int>& del_log);//del_log를 반환. 떼어낸 페이지의 id를 순서대로 저장.
        void second_delete_process(vector<int> &inverted_del_log);//del_log를 뒤집어서 inverted_del_log로 넣고, 그 안의 순서대로 id를 뽑아 그것의 content 삽입.

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
    pages.push_back(page);//pages는 모든 insert_page의 log를 저장한다. 순서 불변해야 함!!
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

void Board::delete_page(int id) {//pages에서 page 제거해야함.
    //recursive delete
    vector <int> del_log={};
    first_delete_process(id,del_log);//delete process
    pages.erase(pages.begin()+Page::find_by_id(id,pages));//해당 페이지를 pages에서 제거.
    del_log.pop_back();//삭제할 페이지의 id는 목록에서 제거.
    vector <int> inverted_del_log={};
    for(int i = 0; i < del_log.size(); i++){//del_log inverting.
        inverted_del_log.push_back(del_log[del_log.size()-1-i]);
    }
    second_delete_process(inverted_del_log);//rebuilding process
}

void Board::modify_content(int id, char content) {//해당 page의 내용만 바꿔서 넣으면 된다.
    vector <int> del_log={};
    first_delete_process(id,del_log);//delete process
    del_log.pop_back();
    int page_order = Page::find_by_id(id,pages);
    pages[page_order].modify_content(content);//pages로 접근하여 variable 수정.
    vector <int> inverted_del_log={};
    for(int i = 0; i < del_log.size(); i++){//del_log inverting.
        inverted_del_log.push_back(del_log[del_log.size()-1-i]);
    }
    Page current_page = pages[page_order];
    int i_x=current_page.get_x();
    int i_y=current_page.get_y();
    int i_width=current_page.get_width();
    int i_height=current_page.get_height();
    int i_id=current_page.get_id();
    char i_content=current_page.get_content();
    for (int h = 0; h < height; h++) {//below_contents initialize
        for (int w = 0; w < width; w++) {
            pages[Page::find_by_id(i_id,pages)].below_contents[h*width + w] = ' ';
        }
    }
    for (int h = i_y; h < (i_height+i_y); h++) {
        for (int w = i_x; w < (i_width+i_x); w++) {
            pages[Page::find_by_id(i_id,pages)].below_contents[h*width + w]=board[h*width + w];//below_contents update!
        }
    }
    for (int h = i_y; h < (i_height+i_y); h++) {
        for (int w = i_x; w < (i_width+i_x); w++) {
            board[h*width + w] = i_content;//다시 채우기.
        }
    }
    print_board();
    second_delete_process(inverted_del_log);//rebuilding process
}

void Board::modify_position(int id, int x, int y) {//해당 page의 위치만 바꿔서 넣으면 된다.
    vector <int> del_log={};
    first_delete_process(id,del_log);//delete process
    del_log.pop_back();
    int page_order = Page::find_by_id(id,pages);
    pages[page_order].modify_position_xy(x,y);//pages로 접근하여 variable 수정.
    vector <int> inverted_del_log={};
    for(int i = 0; i < del_log.size(); i++){//del_log inverting.
        inverted_del_log.push_back(del_log[del_log.size()-1-i]);
    }
    Page current_page = pages[page_order];
    int i_x=current_page.get_x();
    int i_y=current_page.get_y();
    int i_width=current_page.get_width();
    int i_height=current_page.get_height();
    int i_id=current_page.get_id();
    char i_content=current_page.get_content();
    for (int h = 0; h < height; h++) {//below_contents initialize
        for (int w = 0; w < width; w++) {
            pages[Page::find_by_id(i_id,pages)].below_contents[h*width + w] = ' ';
            }
        }
    for (int h = i_y; h < (i_height+i_y); h++) {
        for (int w = i_x; w < (i_width+i_x); w++) {
            pages[Page::find_by_id(i_id,pages)].below_contents[h*width + w]=board[h*width + w];//below_contents update!
        }
    }
    for (int h = i_y; h < (i_height+i_y); h++) {
        for (int w = i_x; w < (i_width+i_x); w++) {
            board[h*width + w] = i_content;//다시 채우기.
        }
    }
    print_board();
    second_delete_process(inverted_del_log);//rebuilding process
}
//first_delete_process에 먼가 오류있음.
void Board::first_delete_process(int id,vector <int> &del_log){//위의 장 중 선택해서 제거하는 단계.//여기서 들어오는 log는 비어있지만, 이 재귀함수를 통과한 뒤의 log는 id들이 순서를 이루어 배열.
    Page current_page=pages[Page::find_by_id(id,pages)];
    int x=current_page.get_x();
    int y=current_page.get_y();
    int d_width=current_page.get_width();
    int d_height=current_page.get_height();
    if(on_page_ids(pages[Page::find_by_id(id,pages)],pages).size()==0){//예외 처리.
    }
    else{//on_page_ids는 오름차순 배열이므로, 그대로 따르면 된다.
        for(int i=0;i<on_page_ids(pages[Page::find_by_id(id,pages)],pages).size();i++){
            Board::first_delete_process(on_page_ids(pages[Page::find_by_id(id,pages)],pages)[i],del_log);
        }
    }
    for (int h = y; h < (d_height+y); h++) {
        for (int w = x; w < (d_width+x); w++) {
            board[h*width + w] = pages[Page::find_by_id(id,pages)].below_contents[h*width + w];
        }
    }
    del_log.push_back(pages[Page::find_by_id(id,pages)].get_id());//가장 처음 떼는 것부터 id가 들어가게 된다. 적어도 한개는 제거한다. 가장 마지막에 오는 항목을 필요하다면, pop_back()을 통해 제거한다. ex) modify_position
    print_board();
    return;
}

void Board::second_delete_process(vector <int> &inverted_del_log){//re-posit 이때 on_page 업데이트
    for(int i = 0; i < inverted_del_log.size(); i++){
        int id = inverted_del_log[i];//id 불러오기.
        Page current_page = pages[Page::find_by_id(id,pages)];
        int i_x=current_page.get_x();
        int i_y=current_page.get_y();
        int i_width=current_page.get_width();
        int i_height=current_page.get_height();
        int i_id=current_page.get_id();
        char i_content=current_page.get_content();
        for (int h = 0; h < height; h++) {//below_contents initialize
            for (int w = 0; w < width; w++) {
                pages[Page::find_by_id(i_id,pages)].below_contents[h*width + w] = ' ';
            }
        }
        for (int h = i_y; h < (i_height+i_y); h++) {
            for (int w = i_x; w < (i_width+i_x); w++) {
                pages[Page::find_by_id(i_id,pages)].below_contents[h*width + w]=board[h*width + w];//below_contents update!
            }
        }
        for (int h = i_y; h < (i_height+i_y); h++) {
            for (int w = i_x; w < (i_width+i_x); w++) {
                board[h*width + w] = i_content;//다시 채우기.
            }
        }
        print_board();
    }
}
