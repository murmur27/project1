#include <fstream>

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


void Board::insert_page(int x, int y, int i_width, int i_height, int id, char content) {
    Page page(x,y,i_width,i_height,id,content);
    push_on_page(page,pages);//on_page
    pages.push_back(page);
    //below_page contents 저장.
    for (int h = y; h < (i_height+y); h++) {
        for (int w = x; w < (i_width+x); w++) {
            page.below_contents = new char[width*height];
            page.below_contents[h*width + w]=board[h*width + w];
        }
    }
    for (int i = 0; i < pages.size(); i++) {
            int x=pages[i].get_x();
            int y=pages[i].get_y();
            int d_width=pages[i].get_width();
            int d_height=pages[i].get_height();
        }
    for (int h = y; h < (i_height+y); h++) {
        for (int w = x; w < (i_width+x); w++) {
            //insert_page할때 해당 자리에 다른 문자 있으면, on_page임을 표시.
            board[h*width + w] = content;
            //board의 각 content 자리에 해당 값이 바뀔 때마다, id리스트를 갱신하고, 각 자리마다 id log가 남도록 구성.
        }
    }
    print_board();
}

void Board::delete_page(int id) {
    //recursive delete
    //on_page
    Page del_page = Page::find_by_id(id,pages);
    int x=del_page.get_x();
    int y=del_page.get_y();
    int d_width=del_page.get_width();
    int d_height=del_page.get_height();
    for (int h = y; h < (d_height+y); h++) {//먼저 아래틀 저장.
        for (int w = x; w < (d_width+x); w++) {
            board[h*width + w] = del_page.below_contents[h*width + w];//below_page 정의 바람., 아니면 순서식 만들어서 재 생성.
        }
    }
    if(del_page.on_page.back()==-1){//Page private value below_page값이 -1일때 지우기, id==-1이면 error
    for (int h = y; h < (d_height+y); h++) {
        for (int w = x; w < (d_width+x); w++) {
            board[h*width + w] = del_page.below_contents[h*width + w];//below_page 정의 바람., 아니면 순서식 만들어서 재 생성.
        }
    }
    print_board();
    return;//initialize consideration
    }
    else {
        for(int i=del_page.on_page.size()-1;i>=0;i--){
            int on_page_id=del_page.on_page[i];
            Board::delete_page(on_page_id);
            del_page;//수정바람
        }
    }
    for (int h = y; h < (d_height+y); h++) {
        for (int w = x; w < (d_width+x); w++) {
            board[h*width + w] = ' ';
        }
    }
    //수정사항
    print_board();//반복해서 출력해야함
}

void Board::modify_content(int id, char content) {
    print_board();

}
void Board::modify_position(int id, int x, int y) {
    print_board();
    
}
